#pragma once
#pragma warning(disable:4996)


#include <stdlib.h>  
#include <stdio.h>
#include <time.h>
#include <string>
#include "AlphaInc.h"
using namespace std;



enum { EN_SUN = 0, EN_MON, EN_TUE, EN_WED, EN_THUR, EN_FRI, EN_SAT };
class CTimeUtils
{
public:
	CTimeUtils();
	~CTimeUtils();

	void AddSeconds(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addSec, _Out_ char* yyyymmddhhmmss);
	void AddMins(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmddhhmmss);
	void AddMins_(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmdd_hhmmss);
	void AddDates(_In_ char* yyyymmdd,  int addDates, _Out_ char* o_yyyymmdd);
	
	char* LocalTime_Full_WithDot(_Out_ char* pzDate);

	char* Today_yyyymmdd();
	char* Time_hhmmssmmm();
	char* Time_hh_mm_ss_mmm();
	
	std::string sDateTime_yyyymmdd_hhmmss();
	char* DateTime_yyyymmdd_hhmmss();
	char* DateTime_yyyymmdd_hhmmssmmm();
	char* DateTime_yyyymmddhhmmss();
	char* DateTime_yyyymmdd_hh_mm_ss_mmm();
	char* GmtDateTime_yyyymmdd_hhmmssmmm();
	void  DateTime(_Out_ char* yyyymmdd, _Out_ char* hhmmss);

	long  diff_mins(const std::string& y1,
		const std::string& m1,
		const std::string& d1,
		const std::string& h1, 
		const std::string& min1,
		const std::string& y2,
		const std::string& m2,
		const std::string& d2,
		const std::string& h2,
		const std::string& min2
		);
	int		Get_WeekDay();
	char*	Get_WeekDayS();
	bool	Is_Weekend();
	
	char*	GetYesterdate();
	char* GetPrevDate(char* pzBaseDate /*yyyymmdd*/, int nPrevDateCnt);
	bool	Is_DayPassed(char* pCompared, bool bChange, bool bDot = false);

private:
	void AddTime(_In_ char timeFrame, _In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addTime, _Out_ char* yyyymmddhhmmss);

private:
	char m_z[__ALPHA::LEN_BUF];
};
