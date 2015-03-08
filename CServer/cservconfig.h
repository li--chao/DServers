#ifndef C_SERV_CONFIG_H
#define C_SERV_CONFIG_H

#include <string.h>
#include "../common/baseconfig.h"
#include "../common/BaseCommon.h"

#define DEFAULT_PORT 3040
#define DEFAULT_MAX_PACKET_SIZE 8192
#define DEFAULT_CONCURRENT_NUM 64
#define DEFAULT_HEAD_PACKET_SIZE 12
#define DEFAULT_LOG_NAME "CServer"
#define DEFAULT_FILE_PATH_NAME "./logs/"
#define DEFAULT_BACK_LOG 20
#define DEFAULT_MAX_OVERLAP_NUM 10240
#define HEART_BEAT_INTERVAL 4


class CServConfig : public BaseConfig
{
public:
	CServConfig();
	virtual ~CServConfig();
	int ReadCfgFile(const char* szpFileName);
	int CheckCfg();

	char szaBServClusterCfgFile[64];
private:
	
};


#endif
