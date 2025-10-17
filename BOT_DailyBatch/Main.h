#pragma once

#ifdef __DEV
	#define EXENAME			"BOT_DailyBatch_Dev.exe"
	#define EXE_VERSION		"v5.1.3"	// enhance regarding DB connection error
#elif __DEMO
	#define EXENAME			"BOT_DailyBatch_Demo.exe"
	#define EXE_VERSION		"v5.1.3"	// enhance regarding DB connection error
#elif __LIVE
	#define EXENAME			"BOT_DailyBatch_Prime.exe"
	#define EXE_VERSION		"v5.1.3"	// enhance regarding DB connection error
#else
	#define EXENAME			"BOT_DailyBatch.exe"
	#define EXE_VERSION		"v5.1.3"	// enhance regarding DB connection error
#endif

/*

v3.0.0
	- 장마감, 미체결주문 정리 각각 하나의 스레드로

v3.1.0
	- Enhancement of logging

v4.0.0
	- When batch job completes, update "done" on table by sp from plain query

v5.0.0
	- batch job use just one thread
	- bug fix

v5.0.1
	- Mkt 버그 fix

v5.1.1
	- DB Connection Ping 추가

v5.1.3
	- thread 함수에 __try __except  추가
*/