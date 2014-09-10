#include "epollcli.h"

LcEpollCli::LcEpollCli() :
m_pBaseConfig(NULL)
{



}

int LcEpollCli::Init(BaseConfig* pBaseConfig, LogConfig* pLogConfig)
{
	m_pBaseConfig = pBaseConfig;

	if(m_cCoreLog.Init(pLogConfig))
	{
		return 1;
	}

	return 0;
}

int LcEpollCli::Connect()
{

	return 0;
}
