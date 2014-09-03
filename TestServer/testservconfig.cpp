#include "testservconfig.h"


TestServConfig::TestServConfig()
{
    m_usServPort = DEFAULT_PORT;
    m_uiMaxPacketSize = DEFAULT_MAX_PACKET_SIZE;
    m_uiConcurrentNum = DEFAULT_CONCURRENT_NUM;
//    m_uiHeadPacketSize = DEFAULT_HEAD_PACKET_SIZE;
	m_uiHeadPacketSize = HEADSIZE;
    m_iBackLog = DEFAULT_BACK_LOG;
    m_uiMaxOverLapNum = DEFAULT_MAX_OVERLAP_NUM;
    strcpy(szpLogName, DEFAULT_LOG_NAME);
	strcpy(szpLogPath, DEFAULT_FILE_PATH_NAME);

	m_uiHeartBeatInterval = HEART_BEAT_INTERVAL;
}

TestServConfig::~TestServConfig()
{


}

int TestServConfig::ReadCfgFile(const char* szpFileName)
{
	return 0;
}

int TestServConfig::CheckCfg()
{
	return 0;
}
