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
	void Push(const unsigned long long& ptr);
	void Pop(unsigned long long& ptr);
	inline unsigned int DataCountNeedHandle()
	{
		return 
	}

private:
	unsigned int m_uiPrevPos;
	unsigned int m_uiAftPos;
	unsigned int m_uiMaxQueLen;
	pthread_mutex_t m_mutex;
	sem_t m_sem;
	unsigned long long* m_pu64ArrData;
};


#endif
