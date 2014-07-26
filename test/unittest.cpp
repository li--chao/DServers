#include <iostream>
#include "../common/network/epollnet.h"
#include "../servconfig.h"
#include "../common/log/TextLog.h"

int main(int argc, char** argv)
{
	LogConfig logCfg;
	ServConfig servCfg;
	strcpy(logCfg.szpLogPath, servCfg.szpLogPath);
	strcpy(logCfg.szpLogName, servCfg.szpLogName);
	TextLog log;
	if(log.Init(&logCfg))
	{
		std::cout << "log Init error" << std::endl;
		return 1;
	}

	log.Write("Hello World");
	return 0;
}
