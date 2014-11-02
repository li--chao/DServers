/**
 * @Description: Multiple Memory Pool
 * @Author: guoyc.2009@gmail.com
 * @Create Date: 2011-10
*/

#ifndef _MULTIPLE_MEMORY_POOL_H_
#define _MULTIPLE_MEMORY_POOL_H_

#include <pthread.h>
#include "FixedMemoryPool.h"

class MultipleMemoryPool
{
public:
	MultipleMemoryPool();
	~MultipleMemoryPool();

	int Init(unsigned long long u64TotalMemSize, unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator, unsigned int uiHeadSize = 0);

	char *AllocateMem(unsigned int uiMemSize);
	void FreeMem(char *pBuf, unsigned int uiMemSize);

private:
	unsigned long long GetUsedMem();
	int FindPool(unsigned int uiMemSize);
	
	unsigned int CalcPoolNum(unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator);
	void CalcUnitSize(unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator);

private:
	unsigned long long m_u64TotalMemSize;
	unsigned int m_uiHeadSize;

	FixedMemoryPool *m_pFixedMem;
	unsigned int *m_uipUnitSize; //内存递减
	unsigned short m_usPoolNum;

	pthread_mutex_t m_mtxMem;
};

#endif
