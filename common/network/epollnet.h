#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include "netcommon.h"

class LCEpollNet : public LCAbstractNet
{
public:
	LCEpollNet();
	virtual ~LCEpollNet();
	int Init();
	
protected:
	OverLap* m_pIOQueue;
	int m_epSocket;
	struct epoll_event *m_pEpollEvs;

	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);
private:

};

#endif
