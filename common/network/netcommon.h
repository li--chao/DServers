#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "../log/TextLog.h"
#include "../packet.h"
#include "../baseconfig.h"
#include <pthread.h>

struct OverLap
{
	OverLap()
	{
		u64SessionID = 0;
		pNext = 0;
		szpComBuf = 0;
		fd = -1;
		uiComLen = MAX_PACKET_SIZE;
		uiFinishLen = 0;
		uiPeerIP = 0;
		usPeerPort = 0;
		u64LastRecvPack = 0ULL;
	}

	unsigned long long u64SessionID;
	OverLap* pNext;
	char* szpComBuf;
	int fd;
	unsigned int uiComLen;
	unsigned int uiFinishLen;
	unsigned int uiPeerIP;
	unsigned short usPeerPort;
	unsigned long long u64LastRecvPack;
};

class LcAbstractNet
{
public:
	virtual int Init() = 0;
	inline const unsigned int& PacketHeadSize() const {	return m_uiPacketHeadSize;	}
	inline const unsigned int& MinPacketSize() const {	return m_uiMinPacketSize;  }
	inline const unsigned int& MaxPacketSize() const {	return m_uiMaxPacketSize;  }
	virtual int Init(BaseConfig* pBaseConfig, TextLog& textLog) = 0;
	virtual int StartThread() = 0;

protected:
	unsigned int m_uiPacketHeadSize;
	unsigned int m_uiMinPacketSize;
	unsigned int m_uiMaxPacketSize;
	int m_lsnSocket;
	TextLog* m_txlNetLog;

	virtual int BindAndLsn(const int& iBackLog, const unsigned short& usPort) = 0;
};

#endif
