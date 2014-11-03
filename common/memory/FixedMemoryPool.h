#ifndef _FIXED_MEMORY_POOL_H_
#define _FIXED_MEMORY_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class FixedMemoryPool
{
public:
	FixedMemoryPool();
	~FixedMemoryPool();
	
	int Init(int maxBlockNum = 200, int maxNumOfPerBlock = 131072, int perAllocateSize = 8);
	void ResetMem();

	char *AllocateMem();
	void FreeMem(char *pBuf);

	unsigned long long GetUsedMem();

private:
	int m_perAllocateSize;	
	int m_nowOff;
	int m_nowIndex;
	int m_maxBlockSize;
	int m_maxBlockNum;
	char **m_arrBuf;
	char *m_pIdleHead;

	unsigned int m_uiIdleNum;
};

#endif
