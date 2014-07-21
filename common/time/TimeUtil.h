#ifndef _TIME_UTIL_H_
#define _TIME_UTIL_H_

#include <time.h>
#include <stdio.h>

#define TIME_ONE_HOUR 3600
#define TIME_ONE_DAY 86400

#define TIME_FMT_LEN 19

enum EDateType
{
	E_Date_Day = 0,
	E_Date_Hour,
	E_Date_Second,
	E_Date_TypeNum
};

struct MyDate
{
	MyDate()
	{
		uiYear = 0;
		ucMonth = 0;
		ucDay = 0;
		ucHour = 0;
		ucMinute = 0;
		ucSecond = 0;
	}

	MyDate & operator =( MyDate &rfParam)
	{
		uiYear = rfParam.uiYear;
		ucMonth = rfParam.ucMonth;
		ucDay = rfParam.ucDay;
		ucHour = rfParam.ucHour;
		ucMinute = rfParam.ucMinute;
		ucSecond = rfParam.ucSecond;
		return *this;
	}

	unsigned int uiYear;
	unsigned char ucMonth;
	unsigned char ucDay;
	unsigned char ucHour;
	unsigned char ucMinute;
	unsigned char ucSecond;
};

class TimeUtil
{
public:
	TimeUtil();
	~TimeUtil();

	static size_t FmtGmtTime(char *szpDstBuf, unsigned int uiDstLen, time_t tNow)
	{
		struct tm gtm;
		gmtime_r(&tNow, &gtm);
		size_t len = strftime(szpDstBuf, uiDstLen, "%a, %d %b %G %T GMT", &gtm);
		return len;
	}

	static long Str2Time(char *szpTime, unsigned int uiTimeLen) //2013-01-10 11:16:34
	{
		if(!szpTime || uiTimeLen != TIME_FMT_LEN)
		{
			return 0;
		}

		struct tm tms;
		sscanf(szpTime, "%d-%2d-%2d %2d:%2d:%2d", &tms.tm_year, &tms.tm_mon, &tms.tm_mday, &tms.tm_hour, &tms.tm_min, &tms.tm_sec);
		tms.tm_year -= 1900;
		tms.tm_mon -= 1;
		time_t t = mktime(&tms);
		return t;
	}

	static int CmpDate(MyDate *pDate1, MyDate *pDate2, EDateType eDateType)
	{
		if(pDate1->uiYear < pDate2->uiYear)
		{
			return -1;
		}
		else if(pDate1->uiYear > pDate2->uiYear)
		{
			return 1;
		}
		else if(pDate1->ucMonth < pDate2->ucMonth)
		{
			return -1;
		}
		else if(pDate1->ucMonth > pDate2->ucMonth)
		{
			return 1;
		}
		else if(pDate1->ucDay < pDate2->ucDay)
		{
			return -1;
		}
		else if(pDate1->ucDay > pDate2->ucDay)
		{
			return 1;
		}
		
		if(eDateType == E_Date_Day) //compare by day
		{
			return 0;
		}

		if(pDate1->ucHour < pDate2->ucHour)
		{
			return -1;
		}
		else if(pDate1->ucHour > pDate2->ucHour)
		{
			return 1;
		}

		if(eDateType == E_Date_Hour) //compare by hour
		{
			return 0;
		}

		if(pDate1->ucMinute < pDate2->ucMinute)
		{
			return -1;
		}
		else if(pDate1->ucMinute > pDate2->ucMinute)
		{
			return 1;
		}
		else if(pDate1->ucSecond < pDate2->ucSecond)
		{
			return -1;
		}
		else if(pDate1->ucSecond > pDate2->ucSecond)
		{
			return 1;
		}

		return 0;
	}

	static void GetObjTime(MyDate *pDate, long lBeforeTime)
	{
		time_t tNow = 0;
		time(&tNow);
		tNow -= lBeforeTime;
		Time2Obj(pDate, tNow);
	}

	static void AddTime(MyDate *pDate, long lAfterTime)
	{
		struct tm tms;
		tms.tm_year = pDate->uiYear;
		tms.tm_mon = pDate->ucMonth;
		tms.tm_mday = pDate->ucDay;
		tms.tm_hour = pDate->ucHour;
		tms.tm_min = pDate->ucMinute;
		tms.tm_sec = pDate->ucSecond;

		tms.tm_year -= 1900;
		tms.tm_mon -= 1;
		time_t t = mktime(&tms);
		t += lAfterTime;
		Time2Obj(pDate, t);
	}

private:
	static void Time2Obj(MyDate *pDate, time_t tTime)
	{
		struct tm tms;
		localtime_r(&tTime, &tms);
		pDate->uiYear = tms.tm_year + 1900;
		pDate->ucMonth = tms.tm_mon + 1;
		pDate->ucDay = tms.tm_mday;
		pDate->ucHour = tms.tm_hour;
		pDate->ucMinute = tms.tm_min;
		pDate->ucSecond = tms.tm_sec;
	}
};

#endif
