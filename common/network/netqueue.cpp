#include "netqueue.h"

NetQueue::NetQueue() :
m_uiPrevPos(0),
m_uiAftPos(0),
m_uiMaxQueLen(0),
m_ppArrData(NULL),
m_uiArrDataNum(0)
{

}

NetQueue::~NetQueue()
{



}

int NetQueue::Init(const unsigned int& uiMaxLen)
{
	m_ppArrData = new long [uiMaxLen];
	if(m_ppArrData == NULL)
	{
		return -1;
	}

	if(pthread_mutex_init(&m_mutex, NULL))
	{
		return -2;
	}
	m_uiMaxQueLen = uiMaxLen;
	sem_init(&m_InSem, 0, uiMaxLen);
	sem_init(&m_OutSem, 0, 0);
	return 0;
}

void NetQueue::Push(const long& ptr)
{
	sem_wait(&m_InSem);
	pthread_mutex_lock(&m_mutex);

	m_uiPrevPos = (m_uiPrevPos + 1) % m_uiMaxQueLen;
	m_ppArrData[m_uiPrevPos] = ptr;
	m_uiArrDataNum++;

	pthread_mutex_unlock(&m_mutex);
	sem_post(&m_OutSem);
}

void NetQueue::Pop(long& ptr)
{
	sem_wait(&m_OutSem);
	pthread_mutex_lock(&m_mutex);

	m_uiAftPos = (m_uiAftPos + 1) % m_uiMaxQueLen;
	ptr = m_ppArrData[m_uiAftPos];
	m_uiArrDataNum--;
	pthread_mutex_unlock(&m_mutex);
	sem_post(&m_InSem);
}

unsigned int NetQueue::DataCountNeedHandle()
{
	pthread_mutex_lock(&m_mutex);
	unsigned int uiCount = 0;
	if(m_uiPrevPos >= m_uiAftPos)
	{
		uiCount = m_uiPrevPos - m_uiAftPos;
		pthread_mutex_unlock(&m_mutex);
		return uiCount;
	}
	uiCount = m_uiMaxQueLen - (m_uiAftPos - m_uiPrevPos);
	pthread_mutex_unlock(&m_mutex);
	return uiCount;
}
