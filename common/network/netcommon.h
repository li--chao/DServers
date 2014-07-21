#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "../log/TextLog.h"

struct OverLap
{
	OverLap()
	{
		u64SessionID = 0;
		pNext = 0;
		szpComBuf = 0;
		fd = -1;
		uiComLen = 0;
		uiFinishLen = 0;
		uiPeerIP = 0;
		usPeerPort = 0;
	}

	unsigned long long u64SessionID;
	OverLap* pNext;
	char* szpComBuf;
	int fd;
	unsigned int uiComLen;
	unsigned int uiFinishLen;
	unsigned int uiPeerIP;
	unsigned short usPeerPort;
};

class LCAbstractNet
{
public:
	virtual int Init() = 0;
	virtual int BindAndLsn(const int& iBackLog, const unsigned short& usPort) = 0;

protected:
	unsigned int m_uiPacketHeadSize;
	unsigned int m_uiMinPacketSize;
	unsigned int m_uiMaxPacketSize;
};

#endif
