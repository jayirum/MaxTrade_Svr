#pragma once
#include <string>
#include "../../Common/StringUtils.h"
using namespace std;

#define DEF_EXENAME		"LSAPI_Chart.exe"
//#define EXE_VERSION	"v5.0.0"	// IOCP 탑재 및 심볼데이터 관리 변경
//#define EXE_VERSION	"v6.0.0"	// WebSocket client 
#define EXE_VERSION		"v7.0.0"	//실시간 차트 처리

#define DEF_API_TR_OF	"o3103"


constexpr int APIQRYCNT_FIRST = 1;
constexpr int APIQRYCNT_NEXT = 2;

constexpr int	LEN_TM_FULL	= 15;	//yyyymmdd_hhmmss
constexpr int	LEN_TM_MIN	= 13;	//yyyymmdd_hhmm
constexpr int	LEN_TM_DATE = 8;	//yyyymmdd

struct TSymbol {
	string sb;
	long dot_cnt;
};


//========== API 데이터와 시세, 호가 스레드간 데이터 전달 포캣 ==========//

#define LEN_UNIT	512

enum class DATA_TP { API, CLOSE, REQUEST };
struct TDataUnit {
	DATA_TP		tp;
	long		tf;
	std::string symbol, timediff;
	std::string	candle_tm_kor;
	std::string	o, h, l, c, v;

	TDataUnit(DATA_TP type):tp{type}{}

	void set(
		std::string& s,
		long		timeframe,
		char* candle_kor_ymd_hms, // yyyymmdd_hhmmss 20251031_152700
		char* po, char* ph, char* pl, char* pc, char* pv
	)
	{
		CStringUtils util;
		symbol			= util.trim_all(s);
		tf				= timeframe;
		candle_tm_kor	= candle_kor_ymd_hms;

		o = std::string(po);	if(o.size()==0)	o = "0";
		h = std::string(ph);	if(h.size()==0)	h = "0";
		l = std::string(pl);	if(l.size()==0)	l = "0";
		c = std::string(pc);	if(c.size()==0)	c = "0";
		v = std::string(pv);	if(v.size()==0)	v = "0";
	}
};

using DataUnitPtr = std::shared_ptr<TDataUnit>;