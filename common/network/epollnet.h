#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include <iostream>
#include "netcommon.h"
#include "netqueue.h"

class LcEpollNet : public LcAbstractNet
{
public:
	LcEpollNet();
	virtual ~LcEpollNet();
	int Init(BaseConfig* pBaseConfig, TextLog& textLog);
	int StartThread();
	
protected:
	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);

private:
	OverLap* m_pIOQueue;
	int m_epSocket;
	struct epoll_event *m_pEpollEvs;
	char* m_szpPackMem;
	BaseConfig* m_pBaseConfig;	
	NetQueue m_IONetMemQue;
	NetQueue m_IONetWorkQue;

	static void* Thread_NetServ(void* param);
	void EpollAccept(const int& fd, const unsigned int& uiPeerIP, const unsigned short& usPeerPort);
	void EpollRecv(OverLap* pOverLap);
};

#endif
