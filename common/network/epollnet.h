#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include <iostream>
#include "netcommon.h"

class LcEpollNet : public LcAbstractNet
{
public:
	LcEpollNet();
	virtual ~LcEpollNet();
	int Init();
	int InitDefault();
	
protected:
	OverLap* m_pIOQueue;
	int m_epSocket;
	struct epoll_event *m_pEpollEvs;
	char* m_szpPackMem;

	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);
private:

};

#endif
