#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include "netcommon.h"

class LCEpollNet : public LCAbstractNet
{
public:
	LCEpollNet();
	virtual ~LCEpollNet();
	int Init();
	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);
	
protected:

private:

};

#endif
