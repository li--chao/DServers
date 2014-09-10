#include <iostream>
#include "testservconfig.h"
#include "testserv.h"
#include "../common/network/epollnet.h"
#include "../common/log/TextLog.h"
#include "../common/network/cluster.h"


pthread_mutex_t deadlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "wrong arguments given, usage: Server Config" << std::endl;
		return -1;
	}

//	daemon(1, 0);
	TestServConfig servCfg;
	if(servCfg.ReadCfgFile(argv[1]))
	{
		return -1;
	}



	LogConfig logCfg;
	strcpy(logCfg.szpLogPath, servCfg.szpLogPath);
	strcpy(logCfg.szpLogName, servCfg.szpLogName);
	TextLog log;
	if(log.Init(&logCfg))
	{
		std::cerr << "Log Init Error" << std::endl;
		return 1;
	}

	Cluster cCluster;
	cCluster.LoadClusterInfo(servCfg.szaBServClusterCfgFile);

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
