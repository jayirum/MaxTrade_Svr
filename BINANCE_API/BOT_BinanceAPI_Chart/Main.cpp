#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "CBinanceApi.h"
#include "CSenderList.h"
#include "CAcceptor.h"
#include "main.h"
#include "CGlobals.h"
#include "CSenderQList.h"
#include "CDBSave.h"
#include <Windows.h>
#include "../../Common/util.h"
#include <tchar.h>


HANDLE				g_hDieEvent;				// event for terminating process
CRITICAL_SECTION	g_Console;


CGlobals		gCommon;
CSenderQList	gQList;

int  _Start()
{
	if (!gCommon.Initialize()) {
		printf("gCommon.Initialize() failed...\n");
		return -1;
	}

	gCommon.log(INFO, TRUE, "[%s][%s] 를 시작합니다.", EXENAME, EXE_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	///*
	//CSendeQ
	//*/
	//if (!gQList.Initialize()) {
	//	gCommon.log(INFO, FALSE, "Failed to init QList");
	//	return -1;
	//}
	//gCommon.log(INFO, TRUE, "시세전송용Q 생성 성공");

	///*
	//CSenders
	//*/
	//CSenderList* sender = new CSenderList;
	//if (!sender->Initialize()) {
	//	gCommon.log(INFO, FALSE, "Failed to create senders");
	//	return 0;
	//}
	//gCommon.log(INFO, TRUE, "시세전송Worker 리스트 생성 성공 ");


	/////*
	////CAcceptor
	////*/
	//boost::asio::io_context ioContext;
	//char zPort[128] = { 0 };
	//gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT", zPort);
	//CAcceptor acceptor(sender, ioContext, atoi(zPort));
	//acceptor.Initialize();
	//gCommon.log(INFO, TRUE, "CAcceptor 초기화 성공");

	/*
	DBSaver
	*/
	std::shared_ptr<CDBSave> dbSave = std::make_shared<CDBSave>();
	char zNum[128] = { 0 };
	gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_THREAD_CNT", zNum);
	if (!dbSave->create_workers( atoi(zNum) ))
		return 0;


	/*
	API of Binance
	*/
	CBinanceApi binance(dbSave);
	if (!binance.Initialize())
		return 0;
	gCommon.log(INFO, TRUE, "CBinanceApi 초기화 성공");

	{
		DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	}

	//delete sender;

	//acceptor.DeIitialize();
	//sender.DeInitialize();
	//binance.DeInitialize();
	//gQList.DeInitialize();

	return 0;
}


BOOL WINAPI ControlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate  
	case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode  
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		SetEvent(g_hDieEvent);
		return TRUE;
		break;

	}
	return FALSE;
}


int main(int argc, LPSTR* argv)
{
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	InitializeCriticalSection(&g_Console);

	_Start();
	printf("After _Start( in main))\n");
	DeleteCriticalSection(&g_Console);
	printf("Stopped in main\n");
	return 0;
}
