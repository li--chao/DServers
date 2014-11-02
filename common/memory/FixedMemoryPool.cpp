#include "FixedMemoryPool.h"

FixedMemoryPool::FixedMemoryPool()
{
	m_arrBuf = NULL;
	m_uiIdleNum = 0;
}

FixedMemoryPool::~FixedMemoryPool()
{
	if(m_arrBuf == NULL)
	{
		return;
	}
	for(int i = 0; i < m_maxBlockNum; i++)
	{
		if(m_arrBuf[i])
		{
			delete []m_arrBuf[i];
		}
	}
	delete []m_arrBuf;
}

int FixedMemoryPool::Init(int maxBlockNum, int maxNumOfPerBlock, int perAllocateSize)
{
	if(maxBlockNum < 1 || maxBlockNum > 1048576)
	{
		return -1;
	}
	m_maxBlockNum = maxBlockNum;
	
	if(maxNumOfPerBlock < 1 || perAllocateSize < (int)sizeof(long))
	{
		return -1;
	}
	m_perAllocateSize = perAllocateSize;
	m_maxBlockSize = maxNumOfPerBlock * perAllocateSize;
	
	m_nowOff = 0;
	m_nowIndex = 0;
	m_pIdleHead = NULL;
	
	m_arrBuf = new char *[m_maxBlockNum];
	if(m_arrBuf == NULL)
	{
		return -1;
	}
	memset(m_arrBuf, 0, m_maxBlockNum*sizeof(long));

	return 0;
}

void FixedMemoryPool::ResetMem()
{
	m_nowOff = 0;
	m_nowIndex = 0;
	m_pIdleHead = NULL;
	m_uiIdleNum = 0;
}

char *FixedMemoryPool::AllocateMem()
{
	if(m_arrBuf[0] == NULL)
	{
		m_arrBuf[0] = new char[m_maxBlockSize];
		if(m_arrBuf[0] == NULL)
		{
			return NULL;
		}
	}

	char *p = NULL;
	if(m_pIdleHead)
	{
		p = m_pIdleHead;
		m_pIdleHead = *(char **)m_pIdleHead;
		m_uiIdleNum--;
		return p;
	}
	
	if(m_perAllocateSize + m_nowOff > m_maxBlockSize)
	{
		m_nowIndex++;
		if(m_nowIndex == m_maxBlockNum)
		{
			m_nowIndex--;
			return NULL;
		}
		
		if(m_arrBuf[m_nowIndex] == NULL)
		{
			m_arrBuf[m_nowIndex] = new char[m_maxBlockSize];
			if(m_arrBuf[m_nowIndex] == NULL)
			{
				m_nowIndex--;
				return NULL;
			}
		}
		
		m_nowOff = 0;
	}
	
	p = m_arrBuf[m_nowIndex] + m_nowOff;
	m_nowOff += m_perAllocateSize;
	
	return p;
}

void FixedMemoryPool::FreeMem(char *pBuf)
{
	if(pBuf == NULL)
	{
		return;
	}
	
	*(char **)pBuf = m_pIdleHead;
	m_pIdleHead = pBuf;
	m_uiIdleNum++;
}

unsigned long long FixedMemoryPool::GetUsedMem()
{
	unsigned long long u64MemSize = (m_nowIndex + 1) * (unsigned long long)m_maxBlockSize;
	u64MemSize -= m_uiIdleNum * (unsigned long long)m_perAllocateSize;
	return u64MemSize;
}