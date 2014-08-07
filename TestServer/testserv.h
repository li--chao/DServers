#ifndef TEST_SERV_H
#define TEST_SERV_H

#include "../common/network/epollnet.h"
#include "../common/log/TextLog.h"
#include "testservconfig.h"

class TestServ
{
public:
	TestServ();
	TestServ(TextLog* pLog, LcAbstractNet* pServNet);
	int MainFun();

private:
	TextLog* m_pLog;
	LcAbstractNet* m_pExtServNet;
};

#endif
