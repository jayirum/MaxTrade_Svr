#pragma once


#define EXE_VERSION		"v1.0.0"
#ifdef __DEF_OVERSEAS
#define DEF_EXENAME		"LSAPI_MarketData_OF.exe"
#else
#define DEF_EXENAME		"LSAPI_MarketData_KF.exe"
#endif


//========== TR CODE ==========//
#ifdef __DEF_OVERSEAS
	#define DEF_TR_SISE		"OVC"
	#define DEF_TR_HOGA		"OVH"
#else
	#define DEF_TR_SISE		"FC0"
	#define DEF_TR_HOGA		"FH0"
#endif



//========== API 데이터와 시세, 호가 스레드간 데이터 전달 포캣 ==========//

#define LEN_UNIT	1024

struct TDataUnit
{
	char	data[LEN_UNIT]{0};
	int		data_size{0};
};