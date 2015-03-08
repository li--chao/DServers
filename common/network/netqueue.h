#ifndef NET_QUEUE_H
#define NET_QUEUE_H


#include <pthread.h>
#include <semaphore.h>

class NetQueue
{
public:
	NetQueue();
	virtual ~NetQueue();
	int Init(const unsigned int& uiMaxLen);
	void Push(const long& ptr);
	void Pop(long& ptr);
	unsigned int DataCountNeedHandle();
	unsigned int DataNum()
	{
		pthread_mutex_lock(&m_mutex);
		unsigned int num = m_uiArrDataNum;
		pthread_mutex_unlock(&m_mutex);
		return num;
	}

private:
	unsigned int m_uiPrevPos;		//  write index position
	unsigned int m_uiAftPos;			//  read index position
	unsigned int m_uiMaxQueLen;
	pthread_mutex_t m_mutex;
	sem_t m_OutSem;
	sem_t m_InSem;
	long* m_ppArrData;
	unsigned int m_uiArrDataNum;
};


#endif
