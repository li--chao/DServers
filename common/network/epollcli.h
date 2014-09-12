#ifndef EPOLL_CLI_H
#define EPOLL_CLI_H

#include "../packet.h"
#include "netcommon.h"
#include "netqueue.h"
#include "lcfullpktchecker.h"
#include "cluster.h"


class LcEpollCli : public LcAbstractCli
{
public:
	LcEpollCli();
	int Init(BaseConfig* pBaseConfig, TextLog* pLog);
	int StartThread();

private:
	TextLog* m_pCoreLog;
	BaseConfig* m_pBaseConfig;

	int m_epSocket;
	struct epoll_event *m_pEpollEvs;
	char* m_szpRecvPackMem;
	char* m_szpWorkPackMem;
	char* m_szpSndPackMem;

	OverLap* m_pIORecvQue;
	OverLap* m_pIOWorkQue;
	OverLap* m_pIOSndQue;

	NetQueue m_IONetConnQue;
	NetQueue m_IONetWorkQue;
	NetQueue m_IONetSndQue;

	LcBaseChecker* m_pChecker;

private:
	static void* Thread_EpollNet(void* vparam);
};

#endif
