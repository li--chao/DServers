#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include <stdio.h>
#include "BaseCommon.h"
#include "packet.h"
#include "io/FileUtil.h"


enum EClusterType
{
	E_ClusterType_None = -1,
	E_ClusterType_A,
	E_ClusterType_B,
	E_ClusterType_Num
};

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
	unsigned int m_uiMaxOverLapNum;
	unsigned int m_uiHeartBeatInterval;

	unsigned int m_uiCliMaxOverLapNum;
	unsigned int m_uiCliConcurrentNum;
	unsigned int m_uiHeartBeatSndInterval;
	int m_iBackLog;
	EClusterType m_eClusterType;
};

#endif
