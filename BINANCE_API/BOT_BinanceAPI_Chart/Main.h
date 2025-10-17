#pragma once

#define EXENAME			"BOT_BinanceAPI_Chart.exe"
#define EXE_VERSION		"v5.0.0"	// 

/*
 v3.0.0
	- API 데이터 수신 timeout 지정 (그렇지 않으면 너무 많은 데이터가 들어와서 DB 가 감당을 못한다.)
	- 이 timeout 은 ini 파일에 있으므로 조정 가능

 v4.0.0
	- ping - pong 추가
	- API 데이터 수신 timeout 제거

 v5.0.0
	- DB 저장시 여러개 패킷을 한번에

*/