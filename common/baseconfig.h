#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include "BaseCommon.h"

class BaseConfig
{
public:
	virtual int ReadCfgFile(const char* szpFileName) = 0;
	virtual int CheckCfg() = 0;

	unsigned short m_usServPort;
	char szpLogPath[FILE_PATH_SIZE];
	char szpLogName[FIELD_NAME_SIZE];
	unsigned int m_uiMaxPacketSize;
	unsigned int m_uiConcurrentNum;
	unsigned int m_uiHeadPacketSize;
	int m_iBackLog;
};

#endif
