#ifndef EPOLL_CLI_H
#define EPOLL_CLI_H

#include "../packet.h"
#include "netcommon.h"
#include "netqueue.h"

class LcEpollCli
{
public:
	LcEpollCli();
	int Init(BaseConfig* pBaseConfig, LogConfig* logConfig);
	int Connect();

private:
	TextLog m_cCoreLog;
	BaseConfig* m_pBaseConfig;
};



#endif
