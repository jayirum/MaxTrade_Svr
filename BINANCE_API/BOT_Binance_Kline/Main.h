#pragma once


#ifdef __DEV
	#define EXENAME			"BOT_Binance_Kline_Dev.exe"
	#define EXE_VERSION		"v2.2.0"		// 
#elif __DEMO
	#define EXENAME			"BOT_Binance_Kline_Demo.exe"
	#define EXE_VERSION		"v3.0.0"	// Kline 값 별도의 DB 저장
#elif __LIVE
	#define EXENAME			"BOT_Binance_Kline_Prime.exe"
	#define EXE_VERSION		"v3.0.0"	// Kline 값 별도의 DB 저장
#else
	#define EXENAME			"BOT_Binance_Kline.exe"
	#define EXE_VERSION		"v2.1.0"	// 
#endif
/*
 v1.0.0

 v2.0.0
	- 조회 시점의 시간을 이용해서 candle 데이터 가져오는 오류 수정
	- 각 interval 의 시작시점에 맞게 요청

v2.1.1
	- DB Connection Ping 추가

*/

#include <string>
#define PARAM_ARRAY_CNT 10
struct TConvertedData
{
	std::string symbol;
	std::string interval;
	std::string time_s;
	std::string time_e;
	std::string o;
	std::string h;
	std::string l;
	std::string c;

	void init() {
		symbol = ""; interval = ""; time_s = ""; time_e = "";
		o = ""; h = ""; l = ""; c = "";
	}
};