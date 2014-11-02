#include "SingleMemoryPool.h"

SingleMemoryPool::SingleMemoryPool()
{
	pthread_mutex_init(&m_mtxMem, NULL);
}

SingleMemoryPool::~SingleMemoryPool()
{
	pthread_mutex_destroy(&m_mtxMem);
}

int SingleMemoryPool::Init(int maxBlockNum, int maxNumOfPerBlock, int perAllocateSize)
{
	return m_cFixedMem.Init(maxBlockNum, maxNumOfPerBlock, perAllocateSize);
}

char *SingleMemoryPool::AllocateMem()
{
	pthread_mutex_lock(&m_mtxMem);
	char *p = m_cFixedMem.AllocateMem();
	pthread_mutex_unlock(&m_mtxMem);

	return p;
}

void SingleMemoryPool::FreeMem(char *pBuf)
{
	pthread_mutex_lock(&m_mtxMem);
	m_cFixedMem.FreeMem(pBuf);
	pthread_mutex_unlock(&m_mtxMem);
}

unsigned long long SingleMemoryPool::GetUsedMem()
{
	pthread_mutex_lock(&m_mtxMem);
	unsigned long long u64MemSize = m_cFixedMem.GetUsedMem();
	pthread_mutex_unlock(&m_mtxMem);
	return u64MemSize;
}
