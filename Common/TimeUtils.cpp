#include <Windows.h>
#include "TimeUtils.h"
#include <ctime>

CTimeUtils::CTimeUtils()
{}

CTimeUtils::~CTimeUtils()
{


}

char* CTimeUtils::DateTime_yyyymmdd_hh_mm_ss_mmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d_%02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

void  CTimeUtils::DateTime(_Out_ char* yyyymmdd, _Out_ char* hhmmss)
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(yyyymmdd, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(hhmmss, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
}

char* CTimeUtils::Today_yyyymmdd()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d",st.wYear, st.wMonth, st.wDay);
	return m_z;
}

char* CTimeUtils::Time_hhmmssmmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%02d%02d%02d%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::Time_hh_mm_ss_mmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::DateTime_yyyymmdd_hhmmss()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return m_z;
}

std::string CTimeUtils::sDateTime_yyyymmdd_hhmmss()
{
	char s[128];
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(s, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return std::string(s);
}

char* CTimeUtils::DateTime_yyyymmdd_hhmmssmmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d_%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::DateTime_yyyymmddhhmmss()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return m_z;
}

int CTimeUtils::Get_WeekDay()
{	struct tm today;
	time_t now = time(0);
	localtime_s(&today, &now);
	return today.tm_wday;
}


// //yyyymmdd-hhmmss
char* CTimeUtils::GmtDateTime_yyyymmdd_hhmmssmmm()
{
	time_t rawtime;
	time(&rawtime);
	struct tm* ltime = gmtime(&rawtime);

	sprintf(m_z, "%04d%02d%02d_%02d%02d%02d",
		1900 + ltime->tm_year, ltime->tm_mon + 1, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

	return m_z;
}


char* CTimeUtils::Get_WeekDayS()
{
	int nDay = Get_WeekDay();
	switch (nDay)
	{
	case EN_SUN:strcpy(m_z, "SUNDAY"); break;
	case EN_MON:strcpy(m_z, "MONDAY"); break;
	case EN_TUE:strcpy(m_z, "TUESDAY"); break;
	case EN_WED:strcpy(m_z, "WEDNESDAY"); break;
	case EN_THUR:strcpy(m_z, "THURSDAY"); break;
	case EN_FRI:strcpy(m_z, "FRIDAY"); break;
	case EN_SAT:strcpy(m_z, "SATURDAY"); break;
	}
	return m_z;
}

bool CTimeUtils::Is_Weekend()
{
	struct tm today;
	time_t now = time(0);
	localtime_s(&today, &now);
	bool ret = false;
	if (today.tm_wday == EN_SUN || today.tm_wday == EN_SAT)
		ret = true;
	return ret;
}

char* CTimeUtils::GetYesterdate()
{
	time_t timer;
	struct tm* t;


	timer = time(NULL) - (24 * 60 * 60);	// 24hours * 60 mins * 60 sec
	t = localtime(&timer);

	sprintf(m_z, "%04d%02d%02d",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return m_z;
}


/*
	get previous date
	return: date (yyyymmdd)

	ex) pzBaseDate : 20240131, nPrevDateCnt : 7 ==> 20240124
*/
char* CTimeUtils::GetPrevDate(char* pzBaseDate /*yyyymmdd*/, int nPrevDateCnt)
{
	time_t timer;
	struct tm* t;


	timer = time(NULL) - (24 * 60 * 60 * nPrevDateCnt);
	t = localtime(&timer);

	sprintf(m_z, "%04d%02d%02d",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return m_z;
}

bool CTimeUtils::Is_DayPassed(char* pCompared, bool bChange, bool bDot)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR szToday[11];

	int nComp = 0;
	if (bDot) {
		sprintf_s(szToday, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
		nComp = strncmp(szToday, pCompared, 10);
	}
	else {
		sprintf_s(szToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
		nComp = strncmp(szToday, pCompared, 8);
	}

	if (nComp > 0)
	{
		if (bChange)
			sprintf_s(pCompared, lstrlen(szToday) + 1, "%.*s", lstrlen(szToday), szToday);
	}

	return (nComp > 0);
}


void CTimeUtils::AddSeconds(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addSec, _Out_ char* yyyymmddhhmmss)
{
	AddTime('s', yyyymmdd, hhmmss, addSec, yyyymmddhhmmss);
}

void CTimeUtils::AddMins(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmddhhmmss)
{
	AddTime('m', yyyymmdd, hhmmss, addMin, yyyymmddhhmmss);
}

void CTimeUtils::AddMins_(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmdd_hhmmss)
{
	char tm[64]={};
	AddTime('m', yyyymmdd, hhmmss, addMin, tm);
	sprintf(yyyymmdd_hhmmss, "%.8s_%6s", tm, tm + 8);		//yyyymmdd_hhmmss
}

void CTimeUtils::AddDates(_In_ char* yyyymmdd, _In_ int addDates, _Out_ char* o_yyyymmdd)
{
	// typedef __int64. get now time
	time_t now = time(&now);

	// breadown the current time into tm struct.
	struct tm * tmLocal = localtime(&now);

	char y[32], m[32], d[32];
	sprintf(y, "%.4s", yyyymmdd);
	sprintf(m, "%.2s", yyyymmdd+4);
	sprintf(d, "%.2s", yyyymmdd+6);

	tmLocal->tm_year = atoi(y) - 1900;
	tmLocal->tm_mon = atoi(m) - 1;
	tmLocal->tm_mday = atoi(d);

	tmLocal->tm_mday += addDates;
	
	
	// make new time_t data using modified tm struct
	time_t newTimeT = mktime(tmLocal);
	struct tm* newTm = localtime(&newTimeT);

	sprintf(o_yyyymmdd, "%04d%02d%02d",
		newTm->tm_year + 1900,
		newTm->tm_mon + 1,
		newTm->tm_mday
	);
}

void CTimeUtils::AddTime(_In_ char timeFrame, _In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addTime, _Out_ char* yyyymmddhhmmss)
{
	// typedef __int64. get now time
	time_t now = time(&now);

	// breadown the current time into tm struct.
	struct tm * tmLocal = localtime(&now);

	char y[32], m[32], d[32], h[32], min[32], s[32];
	sprintf(y, "%.4s", yyyymmdd);
	sprintf(m, "%.2s", yyyymmdd+4);
	sprintf(d, "%.2s", yyyymmdd+6);

	sprintf(h, "%.2s", hhmmss);
	sprintf(min, "%.2s", hhmmss+2);
	sprintf(s, "%.2s", hhmmss+4);

	tmLocal->tm_year = atoi(y) - 1900;
	tmLocal->tm_mon = atoi(m) - 1;
	tmLocal->tm_mday = atoi(d);
	tmLocal->tm_hour = atoi(h);
	tmLocal->tm_min = atoi(min);
	tmLocal->tm_sec = atoi(s);

	switch (timeFrame)
	{
	case 'Y':	tmLocal->tm_year += addTime;	break;
	case 'M':	tmLocal->tm_mon += addTime;		break;
	case 'D':	tmLocal->tm_mday += addTime;	break;
	case 'h':	tmLocal->tm_hour += addTime;	break;
	case 'm':	tmLocal->tm_min += addTime;	break;
	case 's':	tmLocal->tm_sec += addTime;	break;
	}
	
	// make new time_t data using modified tm struct
	time_t newTimeT = mktime(tmLocal);
	struct tm* newTm = localtime(&newTimeT);

	sprintf(yyyymmddhhmmss, "%04d%02d%02d%02d%02d%02d",
		newTm->tm_year + 1900,
		newTm->tm_mon + 1,
		newTm->tm_mday,
		newTm->tm_hour,
		newTm->tm_min,
		newTm->tm_sec
	);
}

// yyyymmdd_hhmm 형식
long  CTimeUtils::diff_mins(
	const std::string& y1,
	const std::string& m1,
	const std::string& d1,
	const std::string& h1,
	const std::string& min1,
	const std::string& y2,
	const std::string& m2,
	const std::string& d2,
	const std::string& h2,
	const std::string& min2)
{
	std::tm tm1{}, tm2{};
	tm1.tm_year = std::stoi(y1) - 1900;
	tm1.tm_mon = std::stoi(m1) - 1;
	tm1.tm_mday = std::stoi(d1);
	tm1.tm_hour = std::stoi(h1);
	tm1.tm_min = std::stoi(min1);

	tm2.tm_year = std::stoi(y2) - 1900;
	tm2.tm_mon = std::stoi(m2) - 1;
	tm2.tm_mday = std::stoi(d2);
	tm2.tm_hour = std::stoi(h2);
	tm2.tm_min = std::stoi(min2);

	// std::mktime 은 로컬 타임존 기준 time_t 반환
	std::time_t time1 = std::mktime(&tm1);
	std::time_t time2 = std::mktime(&tm2);

	// 차이를 분으로 변환
	auto diff_sec = std::difftime(time1, time2); // double (초 단위)
	return static_cast<long>(diff_sec / 60);     // 분 단위
}