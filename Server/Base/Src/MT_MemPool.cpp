#include "MT_MemPool.h"
#include <set>

#	define  round(s)						( ((s) + MTMP_ALIGN - 1) & ~(MTMP_ALIGN - 1) )
#	define  getMemPoolID(s)				    ( (round((s)) - 1) >> MTMP_SHIFT )
#	define  getPoolNodeSize(pool)           ((pool << MTMP_SHIFT ) + MTMP_ALIGN )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void * MT_MemPool::allocate(size_t size)
{
	// 不能分配0字节
	if ( size==0 ) 
	{ 
		size=1; 
	}

	// 超大内存
	if ( size >= MTMP_MAX_SIZE )
	{
		MEM_NODE * pNode = (MEM_NODE *)malloc( size + sizeof(MEM_NODE) );
		pNode->_system_block.bPoolNode = 0; 
		pNode->_system_block.block_len = size;
		return pNode + 1;
	}else
	{
		int pool = getMemPoolID(size);
		MEM_NODE * pNode = pop( pool );

		while( pNode==0 )
		{
			buy_node( pool );
			pNode = pop( pool );
		}

		return pNode + 1;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void * MT_MemPool::reallocate(void * p,size_t size)
{
	// size=0等同于free
	if ( size==0 )
	{
		deallocate(p);
		return 0;
	}

	// 传入null等同alloc
	if  ( p==0 )
	{
		return allocate(size);
	}

	MEM_NODE * pNode = (MEM_NODE *)p-1;


	size_t block_len = 0;
	if ( pNode->_system_block.bPoolNode==0 )
	{
		block_len = pNode->_system_block.block_len;
	}else
	{
		int pool = 0;
		if ( pNode->_node_l.bIsLarge )
			pool = pNode->_node_l.nPoolID;
		else
			pool = pNode->_node_s.nPoolID;

		block_len = getPoolNodeSize(pool);
	}

	// 长度不够则重新分配
	void * new_ptr = allocate(size);
	memcpy( new_ptr,p,block_len>=size?size:block_len);
	deallocate(p);
	return new_ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::deallocate(void * p)
{
	if ( p==0 )
	{
		return;
	}

	// 超大内存
	MEM_NODE * pNode = (MEM_NODE *)p-1;

	if ( pNode->_system_block.bPoolNode==0 )
	{
		free(pNode);
		return;
	}

	if ( pNode->_node_l.bIsLarge==1 )
	{
		// 放回回收站
		int pool = pNode->_node_l.nPoolID;
		push( pool,pNode );

		// 检查库存
		check_mempool_store( pool );
	}else
	{
		// 放回回收站
		int pool = pNode->_node_s.nPoolID;
		push( pool,pNode );

		// 检查库存
		check_mempool_store( pool );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
MT_MemPool::MEM_NODE * MT_MemPool::pop(int pool)
{
	MEM_POOL & mem_pool = m_memPool[pool];

	MEM_NODE * pNode = mem_pool.pFreeHead;
	if ( pNode )
	{
		pNode->_node_s.bAllotted= 1;
		mem_pool.pFreeHead = pNode->pNext;

		mem_pool.nAlloted += 1;
		mem_pool.nFreeNum -= 1;
	}

	return pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::push( int pool,MEM_NODE * pNode )
{
	MEM_POOL & mem_pool = m_memPool[pool];
	pNode->_node_s.bAllotted = 0;
	pNode->pNext        = mem_pool.pFreeHead;
	mem_pool.pFreeHead  = pNode;

	mem_pool.nFreeNum  += 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::buy_node( int pool )
{
	size_t size = getPoolNodeSize( pool );

	if ( size < MTMP_SMALL_SIZE )
	{
		// 小内存一次分配多个,减少内存碎块
		size += sizeof( MEM_NODE );

		MEM_NODE * pNode = (MEM_NODE *)malloc(size*MTMP_GROW);

		for ( int i=0;i<MTMP_GROW;++i )
		{
			pNode->_node_s.bIsLarge= 0;
			pNode->_node_s.nPoolID = pool;
			pNode->_node_s.nIndex= i;

			push( pool,pNode );

			pNode = (MEM_NODE*)(((const char *)pNode) + size);
		}
	}else
	{
		size += sizeof( MEM_NODE );
		MEM_NODE * pNode = (MEM_NODE *)malloc(size);
		pNode->_node_l.bIsLarge= 1;
		pNode->_node_l.nPoolID = pool;

		push( pool,pNode ); 
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::check_mempool_store( int pool )
{
	MEM_POOL & mem_pool = m_memPool[pool];
	unsigned long now   = GetTickCount();
	unsigned long dt    = now - mem_pool.nLastStatistic;

	if ( dt > MTMP_STATISTIC_INTERVAL )
	{
		// 调试断言
		assert((int)mem_pool.nFreeNum >= 0);

		// 统计平均需求量
		unsigned long need = mem_pool.nAlloted;

		if ( mem_pool.nLastStatistic!=0 && dt > (MTMP_STATISTIC_INTERVAL * 2) )
		{
			need = (mem_pool.nAlloted * MTMP_STATISTIC_INTERVAL) / dt;
		}

		mem_pool.nAverNeed = ( need + mem_pool.nAverNeed ) / 2;

		// 库存是否超过
		size_t size = getPoolNodeSize( pool );
		if ( size < MTMP_SMALL_SIZE )
		{
			// 小内存释放原则: 超过平均需求量的4倍
			if ( mem_pool.nFreeNum > mem_pool.nAverNeed * MTMP_SMALL_STORAGE )
			{
				free_small_mempool( pool );
			}
		}else
		{
			// 大内存释放原则: 超过平均需求量即释放
			if ( mem_pool.nFreeNum > mem_pool.nAverNeed * MTMP_LARGE_STORAGE )
			{
				free_large_mempool( pool );
			}
		}

		// 重置统计数据
		mem_pool.nAlloted  = 0;
		mem_pool.nLastStatistic = now;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::free_large_mempool( int pool )
{
	MEM_POOL & mem_pool = m_memPool[pool];

	// 释放大内存池至需求量以下
	while( mem_pool.nFreeNum > mem_pool.nAverNeed )
	{
		MEM_NODE * pNode = pop( pool );
		if ( pNode )
		{
			free( pNode );
		}else
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MT_MemPool::free_small_mempool( int pool )
{
	MEM_POOL & mem_pool = m_memPool[pool];	

	// 释放小内存池至需求量以下

	std::set<MEM_NODE *> node_set;

	MEM_NODE * pNode = pop(pool);
	while(pNode)
	{
		node_set.insert(pNode);
		pNode = pop(pool);
	}

	size_t size = getPoolNodeSize( pool );
	size += sizeof( MEM_NODE );

	// 优先释放连续的内存
	MEM_NODE * head_node = 0;
	size_t continued_num    = 0;
	std::set<MEM_NODE *>::iterator it = node_set.begin();
	for ( ;it!=node_set.end(); )
	{
		MEM_NODE * node = *it;
		++it;

		size_t block_index = node->_node_s.nIndex;

		if ( head_node )
		{
			// 是否和当前检查的节点是同一块内存
			if (continued_num + 1 == block_index && size_t(node - head_node) == block_index * size)
			{
				++continued_num;

				// 找到一块完整内存可以释放
				if ( block_index==MTMP_GROW-1)
				{
					char * free_ptr = (char*)head_node;
					for ( int i=0;i<MTMP_GROW;++i )
					{
						node_set.erase( (MEM_NODE*)free_ptr );
						free_ptr += size;
					}

					free( head_node );
					head_node = 0;
					continued_num = 0;

					// 库存量已经低于需求量了
					if ( node_set.size()<= mem_pool.nAverNeed )
						break;
				}

				// 继续匹配
				continue;
			}
		}

		if ( block_index==0 )
		{
			head_node = node;
			continued_num = 0;
		}else
		{
			head_node = 0;
			continued_num = 0;
		}
	}

	// 将不能释放的内存放回去
	for ( it = node_set.begin();it!=node_set.end();++it )
	{
		push( pool,*it );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 通过Thread Local Storage取得当前线程对应的内存池
MT_MemPool * MT_Allocator::TLS_MemPool()
{
	MT_MemPool * pool = (MT_MemPool*)::TlsGetValue(m_dwTLSIndex);
	if ( pool==0 )
	{
		pool = (MT_MemPool*)malloc(sizeof(MT_MemPool));
		pool->MT_MemPool_();

		BOOL bSet = ::TlsSetValue(m_dwTLSIndex,pool);
		assert(bSet);
		return pool;
	}

	return pool;
}

void* mt_alloc(size_t s)
{
	return MT_Allocator::getInstance().allocate(s);
}

void* mt_realloc(void* p, size_t s)
{
	return MT_Allocator::getInstance().reallocate(p,s);
}

void  mt_free(void* p)
{
	MT_Allocator::getInstance().deallocate(p);
}
