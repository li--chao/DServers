#include "netqueue.h"

NetQueue::NetQueue() :
m_uiPrevPos(0),
m_uiAftPos(0),
m_uiMaxQueLen(0),
m_pu64ArrData(NULL)
{
	pthread_mutex_init(&m_mutex, NULL);
}

NetQueue::~NetQueue()
{



}

int NetQueue::Init(const unsigned int& uiMaxLen)
{
	m_pu64ArrData = new unsigned long long[uiMaxLen];
	if(m_pu64ArrData == NULL)
	{
		return -1;
	}

	m_uiMaxQueLen = uiMaxLen;
	sem_init(&m_sem, 0, 0);
	return 0;
}

void NetQueue::Push(const unsigned long long& ptr)
{


}

void NetQueue::Pop(unsigned long long& ptr)
{

}

unsigned int NetQueue::DataCountNeedHandle()
{
	if(m_uiPrevPos >= m_uiAftPos)
	{
		return m_uiPrevPos - m_uiAftPos;
	}
	return m_uiMaxQueLen - (m_uiAftPos - m_uiPrevPos);
}
