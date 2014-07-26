#include "servconfig.h"

ServConfig::ServConfig():
m_usServPort(DEFAULT_PORT),
m_uiMaxPacketSize(DEFAULT_MAX_PACKET_SIZE),
m_uiConcurrentNum(DEFAULT_CONCURRENT_NUM),
m_uiHeadPacketSize(DEFAULT_HEAD_PACKET_SIZE)
{
	strcpy(szpLogName, DEFAULT_LOG_NAME);
	strcpy(szpLogPath, DEFAULT_FILE_PATH_NAME);
}

ServConfig::~ServConfig()
{

}

int ServConfig::ReadCfgFile(const char* szpFileName)
{
	return 0;
}

int ServConfig::CheckCfg()
{
	return 0;
}
