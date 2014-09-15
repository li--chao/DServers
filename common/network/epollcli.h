#ifndef EPOLL_CLI_H
#define EPOLL_CLI_H

#include "../packet.h"
#include "netcommon.h"
#include "netqueue.h"
#include "lcfullpktchecker.h"
#include "../container/hashtable.h"


class LcEpollCli : public LcAbstractCli
{
public:
	LcEpollCli();
	int Init(BaseConfig* pBaseConfig, TextLog* pLog);
	int StartThread();
	void RequestSnd(long& lAddr);
	void ReleaseRequest(const long& lAddr);
	int PushRequest(const int& fd, OverLap* pOverLap);
	int Connect(const char* szpNodeIP, const unsigned short& usNodeIP, int& fd);
	void RemoveConnect(OverLap* pOverLap);

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

	NetQueue m_IONetConnMemQue;
	NetQueue m_IONetWorkMemQue;
	NetQueue m_IONetWorkQue;
	NetQueue m_IONetSndMemQue;

	LcBaseChecker* m_pChecker;

	LcHashTable<unsigned long long, OverLap*> m_ConnTable;
	pthread_mutex_t m_SndSync;
private:
	static void* Thread_EpollNet(void* vparam);
	void EpollRecv(OverLap* pOverLap);
	int CheckPacket(OverLap* pOverLap, bool& bIsHeadChked);
	void ReleaseSndList(OverLap* pOverLap);
};

#endif
