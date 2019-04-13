#include "MT_MemPool.h"
#include <set>
#include <chrono>

thread_local MT_MemPool *g_pMemPool = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void * MT_MemPool::Allocate(size_t nSize)
{
	// 不能分配0字节
	if (0 == nSize)
	{
		nSize = 1;
	}

	// 超大内存
	if (nSize >= MTMP_MAX_SIZE)
	{
		MEM_NODE * pNode = (MEM_NODE *)malloc(nSize + sizeof(MEM_NODE));
		pNode->_system_block.bPoolNode = 0;
		pNode->_system_block.block_len = nSize;
		return pNode + 1;
	}
	else
	{
		auto GetMemPoolID = [](size_t n) -> unsigned short
		{
			return (((n + MTMP_ALIGN - 1) & ~(MTMP_ALIGN - 1)) - 1) >> MTMP_SHIFT;
		};
		unsigned short nPoolID = GetMemPoolID(nSize);
		MEM_NODE * pNode = Pop(nPoolID);

		while (nullptr == pNode)
		{
			AddNode(nPoolID);
			pNode = Pop(nPoolID);
		}

		return pNode + 1;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void * MT_MemPool::Reallocate(void * p, size_t nSize)
{
	// size=0等同于free
	if (0 == nSize)
	{
		Deallocate(p);
		return 0;
	}

	// 传入null等同alloc
	if (nullptr == p)
	{
		return Allocate(nSize);
	}

	MEM_NODE * pNode = (MEM_NODE *)p - 1;


	size_t block_len = 0;
	if (pNode->_system_block.bPoolNode == 0)
	{
		block_len = pNode->_system_block.block_len;
	}
	else
	{
		unsigned short nPoolID = 0;
		if (pNode->_node_l.bIsLarge)
			nPoolID = pNode->_node_l.nPoolID;
		else
			nPoolID = pNode->_node_s.nPoolID;

		auto GetPoolNodeSize = [](unsigned short nID)
		{
			return (nID << MTMP_SHIFT) + MTMP_ALIGN;
		};

		block_len = GetPoolNodeSize(nPoolID);
	}

	// 长度不够则重新分配
	void * new_ptr = Allocate(nSize);
	memcpy(new_ptr, p, block_len >= nSize ? nSize : block_len);
	Deallocate(p);
	return new_ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::Deallocate(void * p)
{
	if (nullptr == p)
	{
		return;
	}

	// 超大内存
	MEM_NODE * pNode = (MEM_NODE *)p - 1;

	if (pNode->_system_block.bPoolNode == 0)
	{
		free(pNode);
		return;
	}

	if (pNode->_node_l.bIsLarge == 1)
	{
		// 放回回收站
		unsigned short nPoolID = pNode->_node_l.nPoolID;
		Push(nPoolID, pNode);

		// 检查库存
		CheckMempoolStore(nPoolID);
	}
	else
	{
		// 放回回收站
		unsigned short nPoolID = pNode->_node_s.nPoolID;
		Push(nPoolID, pNode);

		// 检查库存
		CheckMempoolStore(nPoolID);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
MT_MemPool::MEM_NODE * MT_MemPool::Pop(unsigned short nPoolID)
{
	MEM_POOL & mem_pool = m_memPool[nPoolID];

	MEM_NODE * pNode = mem_pool.pFreeHead;
	if (pNode)
	{
		pNode->_node_s.bAllotted = 1;
		mem_pool.pFreeHead = pNode->pNext;

		mem_pool.nAlloted += 1;
		mem_pool.nFreeNum -= 1;
	}

	return pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::Push(unsigned short nPoolID, MEM_NODE * pNode)
{
	MEM_POOL & mem_pool = m_memPool[nPoolID];
	pNode->_node_s.bAllotted = 0;
	pNode->pNext = mem_pool.pFreeHead;
	mem_pool.pFreeHead = pNode;

	mem_pool.nFreeNum += 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::AddNode(unsigned short nPoolID)
{
	auto GetPoolNodeSize = [](unsigned short nID)
	{
		return (nID << MTMP_SHIFT) + MTMP_ALIGN;
	};
	size_t size = GetPoolNodeSize(nPoolID);

	if (size < MTMP_SMALL_SIZE)
	{
		// 小内存一次分配多个,减少内存碎块
		size += sizeof(MEM_NODE);

		MEM_NODE * pNode = (MEM_NODE *)malloc(size*MTMP_GROW);

		for (int i = 0; i < MTMP_GROW; ++i)
		{
			pNode->_node_s.bIsLarge = 0;
			pNode->_node_s.nPoolID = nPoolID;
			pNode->_node_s.nIndex = i;

			Push(nPoolID, pNode);

			pNode = (MEM_NODE*)(((const char *)pNode) + size);
		}
	}
	else
	{
		size += sizeof(MEM_NODE);
		MEM_NODE * pNode = (MEM_NODE *)malloc(size);
		pNode->_node_l.bIsLarge = 1;
		pNode->_node_l.nPoolID = nPoolID;

		Push(nPoolID, pNode);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::CheckMempoolStore(unsigned short nPoolID)
{
	MEM_POOL & mem_pool = m_memPool[nPoolID];
	auto tickNow = std::chrono::steady_clock::now().time_since_epoch().count();
	unsigned long now = unsigned long(tickNow >> 20);
	unsigned long dt = now - mem_pool.nLastStatistic;

	if (dt > MTMP_STATISTIC_INTERVAL)
	{
		// 调试断言
		assert((int)mem_pool.nFreeNum >= 0);

		// 统计平均需求量
		unsigned long need = mem_pool.nAlloted;

		if (mem_pool.nLastStatistic != 0 && dt > (MTMP_STATISTIC_INTERVAL * 2))
		{
			need = (mem_pool.nAlloted * MTMP_STATISTIC_INTERVAL) / dt;
		}

		mem_pool.nAverNeed = (need + mem_pool.nAverNeed) / 2;

		// 库存是否超过
		auto GetPoolNodeSize = [](unsigned short pool)
		{
			return (pool << MTMP_SHIFT) + MTMP_ALIGN;
		};
		size_t size = GetPoolNodeSize(nPoolID);

		if (size < MTMP_SMALL_SIZE)
		{
			// 小内存释放原则: 超过平均需求量的4倍
			if (mem_pool.nFreeNum > mem_pool.nAverNeed * MTMP_SMALL_STORAGE)
			{
				FreeSmallMempool(nPoolID);
			}
		}
		else
		{
			// 大内存释放原则: 超过平均需求量即释放
			if (mem_pool.nFreeNum > mem_pool.nAverNeed * MTMP_LARGE_STORAGE)
			{
				FreeLargeMempool(nPoolID);
			}
		}

		// 重置统计数据
		mem_pool.nAlloted = 0;
		mem_pool.nLastStatistic = now;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::FreeLargeMempool(unsigned short nPoolID)
{
	MEM_POOL & mem_pool = m_memPool[nPoolID];

	// 释放大内存池至需求量以下
	while (mem_pool.nFreeNum > mem_pool.nAverNeed)
	{
		MEM_NODE * pNode = Pop(nPoolID);
		if (pNode)
		{
			free(pNode);
		}
		else
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::FreeSmallMempool(unsigned short nPoolID)
{
	MEM_POOL & mem_pool = m_memPool[nPoolID];

	// 释放小内存池至需求量以下

	std::set<MEM_NODE *> node_set;

	MEM_NODE * pNode = Pop(nPoolID);
	while (pNode)
	{
		node_set.insert(pNode);
		pNode = Pop(nPoolID);
	}

	auto GetPoolNodeSize = [](unsigned short nID)
	{
		return (nID << MTMP_SHIFT) + MTMP_ALIGN;
	};

	size_t size = GetPoolNodeSize(nPoolID);
	size += sizeof(MEM_NODE);

	// 优先释放连续的内存
	MEM_NODE * pHeadNode = nullptr;
	size_t continued_num = 0;
	std::set<MEM_NODE *>::iterator it = node_set.begin();
	for (; it != node_set.end(); )
	{
		MEM_NODE * pOneNode = *it;
		++it;

		size_t block_index = pOneNode->_node_s.nIndex;

		if (pHeadNode)
		{
			// 是否和当前检查的节点是同一块内存
			if (continued_num + 1 == block_index && size_t(pOneNode - pHeadNode) == block_index * size)
			{
				++continued_num;

				// 找到一块完整内存可以释放
				if (block_index == MTMP_GROW - 1)
				{
					char * free_ptr = (char*)pHeadNode;
					for (int i = 0; i < MTMP_GROW; ++i)
					{
						node_set.erase((MEM_NODE*)free_ptr);
						free_ptr += size;
					}

					free(pHeadNode);
					pHeadNode = nullptr;
					continued_num = 0;

					// 库存量已经低于需求量了
					if (node_set.size() <= mem_pool.nAverNeed)
						break;
				}

				// 继续匹配
				continue;
			}
		}

		if (block_index == 0)
		{
			pHeadNode = pOneNode;
			continued_num = 0;
		}
		else
		{
			pHeadNode = 0;
			continued_num = 0;
		}
	}

	// 将不能释放的内存放回去
	for (it = node_set.begin(); it != node_set.end(); ++it)
	{
		Push(nPoolID, *it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 通过Thread Local Storage取得当前线程对应的内存池
MT_MemPool * MT_Allocator::TLS_GetMemPool()
{
	if (nullptr == g_pMemPool)
	{
		g_pMemPool = (MT_MemPool*)malloc(sizeof(MT_MemPool));
		g_pMemPool->ReSet();
		assert(g_pMemPool);
	}

	return g_pMemPool;
}

void* MT_Alloc(size_t nSize)
{
	return MT_Allocator::GetInstance().Allocate(nSize);
}

void* MT_ReAlloc(void* p, size_t nSize)
{
	return MT_Allocator::GetInstance().Reallocate(p, nSize);
}

void MT_Free(void* p)
{
	MT_Allocator::GetInstance().Deallocate(p);
}
