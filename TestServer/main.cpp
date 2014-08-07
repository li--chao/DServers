#include <iostream>
#include "testservconfig.h"
#include "testserv.h"
#include "../common/network/epollnet.h"
#include "../common/log/TextLog.h"

pthread_mutex_t deadlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
	daemon(1, 0);
	TestServConfig servCfg;	
	LogConfig logCfg;
	strcpy(logCfg.szpLogPath, servCfg.szpLogPath);
	strcpy(logCfg.szpLogName, servCfg.szpLogName);
	TextLog log;
	if(log.Init(&logCfg))
	{
		std::cerr << "Log Init Error" << std::endl;
		return 1;
	}

	LcAbstractNet* pServNet = new LcEpollNet();
	if(pServNet->Init(&servCfg, log))
	{
		return 1;
	}

	log.Write("Serv Init Success");
	TestServ serv(&log, pServNet);
	pServNet->StartThread();
	serv.MainFun();

	return 0;
}
