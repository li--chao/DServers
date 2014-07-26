#ifndef SERV_CONFIG_H
#define SERV_CONFIG_H

#include <string.h>
#include "common/baseconfig.h"
#include "common/BaseCommon.h"

#define DEFAULT_PORT 3030
#define DEFAULT_MAX_PACKET_SIZE 8192
#define DEFAULT_CONCURRENT_NUM 10240
#define DEFAULT_HEAD_PACKET_SIZE 20
#define DEFAULT_LOG_NAME "UnitTest"
#define DEFAULT_FILE_PATH_NAME "./logs/"


class ServConfig : public BaseConfig
{
public:
	ServConfig();
	~ServConfig();
	int ReadCfgFile(const char* szpFileName);
	int CheckCfg();

	unsigned short m_usServPort;
	char szpLogPath[FILE_PATH_SIZE];
	char szpLogName[FIELD_NAME_SIZE];
	unsigned int m_uiMaxPacketSize;
	unsigned int m_uiConcurrentNum;
	unsigned int m_uiHeadPacketSize;
};

#endif
