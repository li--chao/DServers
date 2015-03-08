#ifndef _SINGLE_MEMORY_POOL_H_
#define _SINGLE_MEMORY_POOL_H_

#include <pthread.h>
#include "FixedMemoryPool.h"

class SingleMemoryPool
{
public:
	SingleMemoryPool();
	~SingleMemoryPool();

	int Init(int maxBlockNum, int maxNumOfPerBlock, int perAllocateSize);

	char *AllocateMem();
	void FreeMem(char *pBuf);

	unsigned long long GetUsedMem();
	
private:
	FixedMemoryPool m_cFixedMem;
	pthread_mutex_t m_mtxMem;
};

#endif
