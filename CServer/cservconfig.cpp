#include "cservconfig.h"


CServConfig::CServConfig()
{
/**
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
**/
	//m_eClusterType = E_ClusterType_None;
}

CServConfig::~CServConfig()
{


}

int CServConfig::ReadCfgFile(const char* szpFileName)
{
	char szaFileBuff[256];
	FILE *fp = fopen(szpFileName, "rb");
	if(fp == NULL)
	{
		return -1;
	}

	while(1)
	{
		char* szpValue = NULL;
		ECfgError ret = FileUtil::GetCfgKey(fp, szaFileBuff, 255, szpValue);
		if(ret == E_Cfg_EOF)
		{
			break;
		}
		else if(ret != E_Cfg_Content)
		{
			continue;
		}

		if(strcmp(szaFileBuff, "Port") == 0)
		{
			m_usServPort = (unsigned short)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "MaxPacketSize") == 0)
		{
			m_uiMaxPacketSize = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "ConcurrentNum") == 0)
		{
			m_uiConcurrentNum = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "MaxOverlapNum") == 0)
		{
			m_uiMaxOverLapNum = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "PacketHeadSize") == 0)
		{
			m_uiHeadPacketSize = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "HeartBeatInterval") == 0)
		{
			m_uiHeartBeatInterval = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "LogName") == 0)
		{
			strcpy(szpLogName, szpValue);
		}
		else if(strcmp(szaFileBuff, "LogPath") == 0)
		{
			strcpy(szpLogPath, szpValue);
		}
		else if(strcmp(szaFileBuff, "BackLog") == 0)
		{
			m_iBackLog = atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "ServerType") == 0)
		{
			m_eClusterType = (EClusterType)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "BServerCfgFile") == 0)
		{
			strcpy(szaBServClusterCfgFile, szpValue);
		}
		else if(strcmp(szaFileBuff, "HeartBeatSndInterval") == 0)
		{
			m_uiHeartBeatSndInterval = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "MaxCliOverlapNum") == 0)
		{
			m_uiCliMaxOverLapNum = (unsigned int)atoi(szpValue);
		}
		else if(strcmp(szaFileBuff, "MaxCliConcurrentNum") == 0)
		{
			m_uiCliConcurrentNum = (unsigned int)atoi(szpValue);
		}
	}

	fclose(fp);
	return 0;
}

int CServConfig::CheckCfg()
{
	return 0;
}
