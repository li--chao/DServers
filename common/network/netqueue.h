#ifndef NET_QUEUE_H
#define NET_QUEUE_H


#include <pthread.h>
#include <semaphore.h>

class NetQueue
{
public:
	NetQueue();
	virtual ~NetQueue();
	int Init(const unsigned int& uiMaxLen, const unsigned int& uiInitSemVal);
	void Push(const long& ptr);
	void Pop(long& ptr);
	unsigned int DataCountNeedHandle();

private:
	unsigned int m_uiPrevPos;
	unsigned int m_uiAftPos;
	unsigned int m_uiMaxQueLen;
	pthread_mutex_t m_mutex;
	sem_t m_OutSem;
	sem_t m_InSem;
	long* m_ppArrData;
	unsigned int m_uiArrDataNum;
};


#endif
