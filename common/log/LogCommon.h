/**
 * @Description: Log Common
 * @Author: guoyc.2009@gmail.com
 * @Create Date: 2013-7
*/

#ifndef _LOG_COMMON_H_
#define _LOG_COMMON_H_

#include "../BaseCommon.h"

struct LogConfig
{
	LogConfig()
	{
		*szpLogPath = 0;
		*szpLogName = 0;
		u64LimitSize = 512<<20;
		uiExpiredDays = 30;	
	}

	char szpLogPath[FILE_PATH_SIZE];
	char szpLogName[FIELD_NAME_SIZE];
	unsigned long long u64LimitSize;
	unsigned int uiExpiredDays;	
};

#endif
