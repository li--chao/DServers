#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include <iostream>
#include "netcommon.h"
#include "netqueue.h"
#include "lcfullpktchecker.h"

class LcEpollNet : public LcAbstractNet
{
public:
	LcEpollNet();
	virtual ~LcEpollNet();
	int Init(BaseConfig* pBaseConfig, TextLog& textLog);
	int StartThread();
	void RemoveConnect(OverLap* pOverLap);
	void GetRequest(long& lptr);
	void ReleaseRequest(const long& lptr);
	void SendData(const long& lptr);
	
protected:
	int BindAndLsn(const int& iBackLog, const unsigned short& usPort);
	inline void IP2Str(const unsigned int& uiPeerIP, char* szpIP)
	{
		unsigned char* p = (unsigned char*)&uiPeerIP;
		sprintf(szpIP, "%u.%u.%u.%u", *p, *(p + 1), *(p + 2), *(p + 3));
	}

private:
	OverLap* m_pIORecvQueue;
	OverLap* m_pIOWorkQueue;
	OverLap* m_pIOSndQueue;
	int m_epSocket;
	struct epoll_event *m_pEpollEvs;
	char* m_szpRecvPackMem;
	char* m_szpWorkPackMem;
	char* m_szpSndPackMem;
	BaseConfig* m_pBaseConfig;
	NetQueue m_IONetConnQue;
	NetQueue m_IONetWorkMemQue;
	NetQueue m_IONetWorkQue;
	NetQueue m_IONetSndMemQue;
//	NetQueue m_IONetSndQue;
	LcBaseChecker* m_pChecker;

	static void* Thread_NetServ(void* param);
	void EpollAccept(const int& fd, const unsigned int& uiPeerIP, const unsigned short& usPeerPort);
	void EpollRecv(OverLap* pOverLap);
	void EpollSend(OverLap* pOverLap);
	int CheckPacket(OverLap* pOverLap, bool& bIsHeadChked);
	void SendToWorkQue(OverLap* pOverLap, const unsigned int& uiPacketLen);
	void ReleaseSndList(OverLap* pOverLap);
};

#endif
