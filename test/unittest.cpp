#include <iostream>
#include "../common/network/epollnet.h"
#include "../servconfig.h"
#include "../common/log/TextLog.h"
#include "../common/network/netqueue.h"

pthread_mutex_t deadlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
//	LogConfig logCfg;
//	ServConfig servCfg;
//	strcpy(logCfg.szpLogPath, servCfg.szpLogPath);
//	strcpy(logCfg.szpLogName, servCfg.szpLogName);
//	TextLog log;
//	if(log.Init(&logCfg))
//	{
//		std::cout << "log Init error" << std::endl;
//		return 1;
//	}

//	LcAbstractNet* pServNet = new LcEpollNet();
//	pServNet->Init(&servCfg, log);
//	pServNet->StartThread();

	pthread_mutex_lock(&deadlock);
	pthread_mutex_lock(&deadlock);
	return 0;
}
