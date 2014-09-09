#ifndef TEST_SERV_H
#define TEST_SERV_H

#include "../common/network/epollnet.h"
#include "../common/log/TextLog.h"
#include "testservconfig.h"
#include "../common/packet.h"

class TestServ
{
public:
	TestServ();
	TestServ(TextLog* pLog, LcAbstractNet* pServNet);
	int MainFun();

private:
	TextLog* m_pLog;
	LcAbstractNet* m_pExtServNet;

private:
	int HandleTestProtocol(OverLap* pOverLap);
	int HandleGetSessionID(OverLap* pOverLap);
	int HandleGetIOPacket(OverLap* pOverLap);
	int HandleHeartBeat(OverLap* pOverLap);
};

#endif
