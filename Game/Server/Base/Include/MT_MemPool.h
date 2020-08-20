#pragma once

#include <assert.h>
#include <thread>
#include <stdio.h>
#include <memory.h>

#pragma pack(1)

extern void* MT_Alloc(size_t nSize);
extern void* MT_ReAlloc(void *p, size_t nSize);
extern void  MT_Free(void *p);

class MT_MemPool;
extern thread_local MT_MemPool *g_pMemPool;

enum
{
	MTMP_SHIFT = 4,										// 对齐时的右移位数（决定了尺寸对齐的大小）， 可调
	MTMP_MAX_SIZE = 8192,								// 内存池支持的最大分配单元,可调,最大8191*MTMP_ALIGN
	MTMP_SMALL_SIZE = 0,								// 小内存大小限制,可调,最大256*MTMP_ALIGN
	MTMP_ALIGN = (1 << MTMP_SHIFT),						// 内存对齐大小
	MTMP_POOL_SIZE = (MTMP_MAX_SIZE >> MTMP_SHIFT),		// 内存池的桶数
	MTMP_GROW = 16,										// 小内存在没有空闲节点时一次性分配的节点数,可调,<=64
	MTMP_STATISTIC_INTERVAL = 2048,						// 库存统计频率(2秒)
	MTMP_SMALL_STORAGE = 4,								// 小内存库存 = 平均需求量的4倍
	MTMP_LARGE_STORAGE = 1,								// 大内存库存 = 平均需求量
};

static_assert(MTMP_GROW <= 64, "MTMP_GROW <= 64");
static_assert(MTMP_SMALL_SIZE < 256 * MTMP_ALIGN, "MTMP_SMALL_SIZE < 256 * MTMP_ALIGN");
static_assert(MTMP_MAX_SIZE < 8191 * MTMP_ALIGN, "MTMP_MAX_SIZE < 8191 * MTMP_ALIGN");

/**
@ name   : 多线程安全内存池(第2版)
@ brief  : 内存分配通常是影响一个系统性能的关键所在.
@        : 内存分配的主要性能瓶颈有:
@        : 1.由于Windows分配时需要进入操作系统内核态,而进入一个内核态至少需要2000个CPU时钟周期
@        : 2.频繁的小内存分配会导致内存出现大量碎片,另外容易引发内存页面错误
@        : 3.当有多个线程需要分配内存时,内存池必须加锁,加锁也需要进入内核态,更重要的是锁碰撞引起自旋等待将浪费大量cpu空转时间
@ brief  : 我们这里实现一个多线程安全版本的内存池,实现思路是:
@        : 1.用智能的内存池减少内存分配的时间和碎块(所谓智能是指智能侦测系统对内存的需求量)
@        : 2.每个线程调用各自独立的内存池,这样可以完全避免枷锁(Lock-Free).效率会有极大提高.
@ note   : PS:
@        : 我们也考虑过使用原子操作的方式实现Lock-Free-Stack.(见LockFree.h)
@        : 但这种方式内存会越用越多,无法动态释放一些内存(不安全,见LockFree代码注释)
@        : 后来干脆每个线程独立一个内存池,内次alloc时使用GetCurrentThreadId()判断当前线程ID.
@        : 由于GetCurrentThreadId函数只需要3行汇编代码,读下寄存器即可完成,所以效率非常高.又能真正避免枷锁.可以最大化的提高性能
@        : 另外根据我们的实现,在A线程分配的内存在B线程释放问题也不大,所以几乎没有太大缺陷
@ note   : 内存释放策略:
@        : 统计内存池内每个桶的内存需求频率(每秒需要多少块),如果库存量大于平均需求量则释放多余的库存
@        : 小内存释放问题,小内存尽量不释放,如果存量远大于需求则强制释放,强制释放时可以将空闲内存排序,优先释放连续的内存块
@        : 小内存不要整块new,否则释放不了,连续new的内存块一般也是连续的,只要能解决内存碎块的问题即可
*/
class MT_MemPool
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct  SMALL_MEM_NODE
	{
		unsigned short     bAllotted : 1;        // 该内存块是否已分配
		unsigned short     bIsLarge : 1;        // 该内存块是否是大内存
		unsigned short     nIndex : 6;        // 该内存块是批量分配的小内存块中的第几块
		unsigned short     nPoolID : 8;        // 该内存块所在内存池ID,即内存大小/MTMP_ALIGN
		unsigned short     nPadding : 16;
	};

	struct LARGE_MEM_NODE
	{
		unsigned short     bAllotted : 1;        // 该内存块是否已分配
		unsigned short     bIsLarge : 1;        // 该内存块是否是大内存
		unsigned short     bSysMalloc : 1;        // 是否是系统分配的超大内存
		unsigned short     nPoolID : 13;        // 该内存块所在内存池ID,即内存大小/MTMP_ALIGN
		unsigned short     nPadding : 16;

		inline void operator = (unsigned short tagNode)
		{
			*(unsigned short*)this = tagNode;
		}

		inline bool operator == (unsigned short tagNode)
		{
			return *(unsigned short*)this == tagNode;
		}
	};

	struct SYSTEM_MEM_BLOCK
	{
		unsigned long      bPoolNode : 1;        // 是否是内存池分配的内存
		unsigned long      block_len : 31;        // 内存块长度
	};

	struct MEM_NODE                              // 内存块结构
	{
		union
		{
			SMALL_MEM_NODE   _node_s;
			LARGE_MEM_NODE   _node_l;
			SYSTEM_MEM_BLOCK _system_block; // 大于MTMP_MAX_SIZE时使用系统分配内存机制
		};

		MEM_NODE * pNext = nullptr; // 指向下一个节点
	};

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 分配内存
	void* Allocate(size_t nSize);

	// 重新分配内存
	void* Reallocate(void * p, size_t nSize);

	// 释放内存
	void Deallocate(void * p);

	// 默认记的是线程ID
	auto GetID() { return m_thID; }

	void ReSet()
	{
		m_thID = std::this_thread::get_id();
		memset(m_memPool, 0, sizeof(m_memPool));
	}

protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 从内存池中取出空闲节点
	inline MEM_NODE * Pop(unsigned short nPoolID);

	// 把一个新节点放进内存池
	inline void Push(unsigned short nPoolID, MEM_NODE * pNode);

	// 创建新的节点
	inline void AddNode(unsigned short nPoolID);

	// 检查内存池存量,如果存量远大于需求则尝试释放
	inline void CheckMempoolStore(unsigned short nPoolID);

	// 释放大内存池
	inline void FreeLargeMempool(unsigned short nPoolID);

	// 释放小内存池
	inline void FreeSmallMempool(unsigned short nPoolID);

protected:
	struct MEM_POOL
	{
		MEM_NODE		*pFreeHead = nullptr;	// 内存池空闲头节点
		unsigned long	nAlloted = 0;			// 单位时间内已分配内存次数
		unsigned long	nFreeNum = 0;			// 剩余内存块数
		unsigned long	nAverNeed = 0;			// 单位时间内的平均需求量(MTMP_STATISTIC_INTERVAL指定)
		unsigned long	nLastStatistic = 0;		// 最后统计时间
	};

	std::thread::id m_thID; // 默认记得是线程ID
	MEM_POOL m_memPool[MTMP_POOL_SIZE] = { 0 }; // 内存池数组
};

/**
@ name  : 多线程内存分配器
@ brief : 根据线程ID,每个线程对应一个内存池
*/
class MT_Allocator/* : public rkt::Singleton<MT_Allocator>*/
{
public:
	static MT_Allocator& GetGetInstance()
	{
		static MT_Allocator s_GetInstance;
		return s_GetInstance;
	}

	// 分配内存
	void * Allocate(size_t nSize)
	{
		return TLS_GetMemPool()->Allocate(nSize);
	}

	// 重新分配内存
	void * Reallocate(void * p, size_t nSize)
	{
		return TLS_GetMemPool()->Reallocate(p, nSize);
	}

	// 释放内存
	void Deallocate(void * p)
	{
		TLS_GetMemPool()->Deallocate(p);
	}

	// 通过Thread Local Storage取得当前线程对应的内存池
	MT_MemPool * TLS_GetMemPool();
};

#pragma pack()
