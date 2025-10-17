#pragma once


#ifdef __DEV
	#define EXENAME			"BOT_Binance_Kline_Dev.exe"
	#define EXE_VERSION		"v2.2.0"		// 
#elif __DEMO
	#define EXENAME			"BOT_Binance_Kline_Demo.exe"
	#define EXE_VERSION		"v3.0.0"	// Kline �� ������ DB ����
#elif __LIVE
	#define EXENAME			"BOT_Binance_Kline_Prime.exe"
	#define EXE_VERSION		"v3.0.0"	// Kline �� ������ DB ����
#else
	#define EXENAME			"BOT_Binance_Kline.exe"
	#define EXE_VERSION		"v2.1.0"	// 
#endif
/*
 v1.0.0

 v2.0.0
	- ��ȸ ������ �ð��� �̿��ؼ� candle ������ �������� ���� ����
	- �� interval �� ���۽����� �°� ��û

v2.1.1
	- DB Connection Ping �߰�

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