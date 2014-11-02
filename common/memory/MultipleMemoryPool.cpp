#include "MultipleMemoryPool.h"

MultipleMemoryPool::MultipleMemoryPool()
{
	m_u64TotalMemSize = 0;
	m_uiHeadSize = 0;

	m_pFixedMem = NULL;
	m_uipUnitSize = NULL;
	m_usPoolNum = 0;

	pthread_mutex_init(&m_mtxMem, NULL);
}

MultipleMemoryPool::~MultipleMemoryPool()
{
	if(m_pFixedMem)
	{
		delete []m_pFixedMem;
	}

	if(m_uipUnitSize)
	{
		delete []m_uipUnitSize;
	}

	pthread_mutex_destroy(&m_mtxMem);
}

int MultipleMemoryPool::Init(unsigned long long u64TotalMemSize, unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator, unsigned int uiHeadSize)
{
	m_u64TotalMemSize = u64TotalMemSize;
	m_uiHeadSize = uiHeadSize;
	m_usPoolNum = CalcPoolNum(uiBaseUnitSize, uiMinUnitSize, numerator, denominator); //计算内存池个数

	m_pFixedMem = new FixedMemoryPool[m_usPoolNum];
	if(m_pFixedMem == NULL)
	{
		return -1;
	}
	m_uipUnitSize = new unsigned int[m_usPoolNum];
	if(m_uipUnitSize == NULL)
	{
		return -1;
	}

	CalcUnitSize(uiBaseUnitSize, uiMinUnitSize, numerator, denominator); //计算每个内存池内存大小
	
	unsigned int uiMaxBlockNum = u64TotalMemSize / (m_uipUnitSize[s] * 64); //每个内存池内存块数
	for(unsigned short s = 0; s < m_usPoolNum; s++)
	{
		m_uipUnitSize[s] += m_uiHeadSize;
		if(m_pFixedMem[s].Init(uiMaxBlockNum, 64, m_uipUnitSize[s])) //初始化内存池
		{
			return -1;
		}
	}
	
	return 0;
}

char *MultipleMemoryPool::AllocateMem(unsigned int uiMemSize)
{
	if(m_uiHeadSize)
	{
		uiMemSize += m_uiHeadSize;
	}

	int pos = FindPool(uiMemSize);
	if(pos == -1 || m_pFixedMem[pos] == NULL)
	{
		return NULL;
	}

	char *pMem = NULL;
	pthread_mutex_lock(&m_mtxMem);
	if(GetUsedMem() <= m_u64TotalMemSize)
	{
		pMem = m_pFixedMem[pos].AllocateMem();
	}
	pthread_mutex_unlock(&m_mtxMem);

	return pMem;
}

void MultipleMemoryPool::FreeMem(char *pBuf, unsigned int uiMemSize)
{
	if(m_uiHeadSize)
	{
		uiMemSize += m_uiHeadSize;
	}

	int pos = FindPool(uiMemSize);
	if(pos == -1 || m_pFixedMem[pos] == NULL)
	{
		return;
	}

	pthread_mutex_lock(&m_mtxMem);
	m_pFixedMem[pos].FreeMem(pBuf);
	pthread_mutex_unlock(&m_mtxMem);
}

unsigned long long MultipleMemoryPool::GetUsedMem()
{
	unsigned long long u64MemSize = 0;
	for(unsigned short s = 0; s < m_usPoolNum; s++)
	{
		u64MemSize += m_pFixedMem[s].GetUsedMem();
	}
	return u64MemSize;
}

int MultipleMemoryPool::FindPool(unsigned int uiMemSize)
{
	for(int i = m_usPoolNum - 1; i > -1; i--) //从小到大查找
	{
		if(uiMemSize <= m_uipUnitSize[i])
		{
			return i;
		}
	}

	return -1;
}

unsigned int MultipleMemoryPool::CalcPoolNum(unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator)
{
	unsigned int num = 0;
	while(uiBaseUnitSize >= uiMinUnitSize)
	{
		num++;
		uiBaseUnitSize = (unsigned int)(((double)uiBaseUnitSize * denominator) / (double)numerator); //递减
	}

	return num;	
}

void MultipleMemoryPool::CalcUnitSize(unsigned int uiBaseUnitSize, unsigned int uiMinUnitSize, unsigned int numerator, unsigned int denominator)
{
	unsigned int num = 0;
	while(uiBaseUnitSize >= uiMinUnitSize)
	{
		m_uipUnitSize[num] = uiBaseUnitSize;
		num++;
		uiBaseUnitSize = (unsigned int)(((double)uiBaseUnitSize * denominator) / (double)numerator);
	}
}

