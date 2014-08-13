#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "../log/TextLog.h"
#include "../packet.h"
#include "../baseconfig.h"
#include <pthread.h>

struct OverLap
{
	OverLap()
	{
		u64SessionID = 0;
		fd = -1;
		uiComLen = MAX_PACKET_SIZE;
		uiFinishLen = 0;
		uiSndComLen = 0;
		uiSndFinishLen = 0;
		uiPeerIP = 0;
		usPeerPort = 0;
		u64LastRecvPack = 0ULL;
		szpComBuf = NULL;
		uiPacketLen = 0;
	}

	unsigned long long u64SessionID;
	char* szpComBuf;
//	char* szpSndComBuf;
//	char* szpWorkComBuf;
	int fd;
	unsigned int uiComLen;
	unsigned int uiFinishLen;
	unsigned int uiSndComLen;
	unsigned int uiSndFinishLen;
	unsigned int uiPacketLen;
	unsigned int uiPeerIP;
	unsigned short usPeerPort;
	unsigned long long u64LastRecvPack;
};

class LcAbstractNet
{
public:
	virtual int Init(BaseConfig* pBaseConfig, TextLog& textLog) = 0;
	virtual int StartThread() = 0;
	virtual void RemoveConnect(OverLap* pOverLap) = 0;
	virtual void GetRequest(long& lptr) = 0;
	virtual void SendData(const long& lptr) = 0;

protected:
	int m_lsnSocket;
	TextLog* m_txlNetLog;

	virtual int BindAndLsn(const int& iBackLog, const unsigned short& usPort) = 0;
};

#endif
