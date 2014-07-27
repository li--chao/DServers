#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include <iostream>
#include "netcommon.h"

class LcEpollNet : public LcAbstractNet
{
public:
	LcEpollNet();
	virtual ~LcEpollNet();
	int Init(const BaseConfig* pBaseConfig, TextLog& textLog);
	int InitDefault();
	
protected:
	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);

private:
	OverLap* m_pIOQueue;
	int m_epSocket;
	struct epoll_event *m_pEpollEvs;
	char* m_szpPackMem;
};

#endif
