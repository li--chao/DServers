#ifndef _TEXT_LOG_H_
#define _TEXT_LOG_H_

#include <pthread.h>
#include "../time/TimeUtil.h"
#include "../io/FileUtil.h"
#include "LogCommon.h"

#define TEXT_LOG_FILE_NAME "%s/%s_%u%.2u%.2u_%u.log"
#define TEXT_LOG_TERM_HEAD "[%.2d:%.2d:%.2d] "
#define TEXT_LOG_TERM_END "\r\n"

class TextLog
{
public:
	TextLog()
	{
		m_pLogConfig = NULL;
		m_uiFileNo = 0;

		pthread_mutex_init(&m_mtxIO, NULL);
	}
	~TextLog()
	{
		pthread_mutex_destroy(&m_mtxIO);
	}

	int Init(LogConfig *pLogConfig)
	{
		m_pLogConfig = pLogConfig;
		if(!pLogConfig)
		{
			return -1;
		}
		TimeUtil::GetObjTime(&m_cToday, 0); //今天时间

		unsigned int u = 0;
		char szaLogFile[FILE_NAME_SIZE];
		while(1) //获取今天日志个数
		{
			sprintf(szaLogFile, TEXT_LOG_FILE_NAME, m_pLogConfig->szpLogPath, m_pLogConfig->szpLogName, m_cToday.uiYear, m_cToday.ucMonth, m_cToday.ucDay, u);
			if(access(szaLogFile, F_OK))
			{
				break;
			}
			u++;
		}
		m_uiFileNo = u;
		if(m_uiFileNo)
		{
			m_uiFileNo--;
		}

		return 0;
	}

	int Write(char *szpLogInfo, unsigned int uiLogLen)
	{
		if(!szpLogInfo || *szpLogInfo == 0 || uiLogLen == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			char chTmp = *(szpLogInfo + uiLogLen);
			*(szpLogInfo + uiLogLen) = 0;
			fputs(szpLogInfo, fp);
			*(szpLogInfo + uiLogLen) = chTmp;
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	int Write(const char * szpLogInfo)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fputs(szpLogInfo, fp);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1>
	int Write(const char * szpLogInfo, t_arg1 arg1)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1, class t_arg2>
	int Write(const char * szpLogInfo, t_arg1 arg1, t_arg2 arg2)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1, arg2);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1, class t_arg2, class t_arg3>
	int Write(const char * szpLogInfo, t_arg1 arg1, t_arg2 arg2, t_arg3 arg3)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1, arg2, arg3);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1, class t_arg2, class t_arg3, class t_arg4>
	int Write(const char * szpLogInfo, t_arg1 arg1, t_arg2 arg2, t_arg3 arg3, t_arg4 arg4)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1, arg2, arg3, arg4);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1, class t_arg2, class t_arg3, class t_arg4, class t_arg5>
	int Write(const char * szpLogInfo, t_arg1 arg1, t_arg2 arg2, t_arg3 arg3, t_arg4 arg4, t_arg5 arg5)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1, arg2, arg3, arg4, arg5);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

	template <class t_arg1, class t_arg2, class t_arg3, class t_arg4, class t_arg5, class t_arg6>
	int Write(const char * szpLogInfo, t_arg1 arg1, t_arg2 arg2, t_arg3 arg3, t_arg4 arg4, t_arg5 arg5, t_arg6 arg6)
	{
		if(!szpLogInfo || *szpLogInfo == 0)
		{
			return 0;
		}
		MyDate cNow;
		TimeUtil::GetObjTime(&cNow, 0);

		pthread_mutex_lock(&m_mtxIO);
		FILE *fp = OpenLog(&cNow);
		if(fp)
		{
			fprintf(fp, TEXT_LOG_TERM_HEAD, cNow.ucHour, cNow.ucMinute, cNow.ucSecond);
			fprintf(fp, szpLogInfo, arg1, arg2, arg3, arg4, arg5, arg6);
			fputs(TEXT_LOG_TERM_END, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&m_mtxIO);

		return 0;
	}

private:
	FILE * OpenLog(MyDate *pNow)
	{
		char szaLogFile[FILE_NAME_SIZE];

		MyDate cExpired = *pNow;
		TimeUtil::AddTime(&cExpired, (long)m_pLogConfig->uiExpiredDays * (-TIME_ONE_DAY));
		unsigned int u = 0;
		while(1) //删除过期日志
		{
			sprintf(szaLogFile, TEXT_LOG_FILE_NAME, m_pLogConfig->szpLogPath, m_pLogConfig->szpLogName, cExpired.uiYear, cExpired.ucMonth, cExpired.ucDay, u);
			if(access(szaLogFile, F_OK))
			{
				break;
			}
			unlink(szaLogFile);
			u++;
		}

		if(TimeUtil::CmpDate(&m_cToday, pNow, E_Date_Day))
		{
			m_uiFileNo = 0;
			m_cToday = *pNow; //新的一天
		}

		sprintf(szaLogFile, TEXT_LOG_FILE_NAME, m_pLogConfig->szpLogPath, m_pLogConfig->szpLogName, m_cToday.uiYear, m_cToday.ucMonth, m_cToday.ucDay, m_uiFileNo);
		if(access(szaLogFile, F_OK) == 0) //这个编号的日志文件存在
		{
			struct stat64 f_stat;
			if(stat64(szaLogFile, &f_stat))
			{
				return NULL;
			}
			if((unsigned long long)f_stat.st_size >= m_pLogConfig->u64LimitSize) //文件大大了，新生成编号日志文件
			{
				m_uiFileNo++;
				sprintf(szaLogFile, TEXT_LOG_FILE_NAME, m_pLogConfig->szpLogPath, m_pLogConfig->szpLogName, m_cToday.uiYear, m_cToday.ucMonth, m_cToday.ucDay, m_uiFileNo);
			}
		}

		FILE *fp = fopen(szaLogFile, "ab");
		return fp;
	}

private:
	LogConfig *m_pLogConfig;

	MyDate m_cToday;
	unsigned int m_uiFileNo;

	pthread_mutex_t m_mtxIO;
};

#endif
