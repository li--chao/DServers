#ifndef TEST_SERV_H
#define TEST_SERV_H

#include "../common/network/epollnet.h"
#include "../common/network/epollcli.h"
#include "../common/network/cluster.h"
#include "../common/log/TextLog.h"
#include "testservconfig.h"
#include "../common/packet.h"


class TestServ
{
public:
	TestServ();
	TestServ(TextLog* pLog, LcAbstractNet* pServNet, Cluster* pClusters, BaseConfig* pBaseConfig);
	int MainFun();
	int StartThread();

private:
	TextLog* m_pLog;
	LcAbstractNet* m_pExtServNet;
	Cluster* m_pClusters;
	BaseConfig* m_pBaseConfig;

private:
	int HandleTestProtocol(OverLap* pOverLap);
	int HandleGetSessionID(OverLap* pOverLap);
	int HandleGetIOPacket(OverLap* pOverLap);
	int HandleHeartBeat(OverLap* pOverLap);

	static void* Thread_HeartBeat(void* vparam);
	static void* Thread_InnerNet(void* vparam);
};

#endif
