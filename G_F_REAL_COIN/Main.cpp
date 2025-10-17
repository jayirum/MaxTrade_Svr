#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "CAPIClient.h"
#include "CSenderList.h"
#include "CAcceptor.h"
#include "main.h"
#include "CGlobals.h"
#include "CSenderQList.h"
#include <Windows.h>
#include "../Common/util.h"
#include <tchar.h>


BOOL	g_bDebug;
HANDLE	g_hDieEvent;				// event for terminating process
volatile
BOOL	g_bContinue = TRUE;	// flag whether continue process or not
CRITICAL_SECTION	g_Console;


CGlobals		gCommon;
CSenderQList	gQList;

int  _Start()
{
	if (!gCommon.Initialize())
		return -1;

	gCommon.log(INFO, FALSE, "[%s][%s] 를 시작합니다.", EXENAME, EXEC_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
	CSendeQ
	*/
	if (!gQList.Initialize()) {
		gCommon.log(INFO, FALSE, "Failed to init QList");
		return -1;
	}

	/*
	CSenders
	*/
	CSenderList sender;
	if (!sender.Initialize()) {
		gCommon.log(INFO, FALSE, "Failed to create senders");
		return 0;
	}


	/*
	CAcceptor
	*/
	boost::asio::io_context ioContext;
	char zPort[128] = { 0 };
	gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT", zPort);
	gCommon.log(INFO, TRUE, "Listen Port:%s", zPort);
	CAcceptor acceptor(&sender, ioContext, atoi(zPort));
	acceptor.Initialize();

	/*
	API of Binance
	*/
	CAPIClient api;
	if (!api.Initialize())
		return 0;
	while (!api.Connect())
	{
		Sleep(api.reconnTimeout());
	}

	
	DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	
	acceptor.DeIitialize();
	sender.DeInitialize();
	api.DeInitialize();
	gQList.DeInitialize();

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
		g_bContinue = FALSE;
		return TRUE;
		break;

	}
	return FALSE;
}


int main(int argc, LPSTR* argv)
{
	g_bDebug = TRUE;
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	InitializeCriticalSection(&g_Console);

	_Start();
	printf("After _Start( in main))\n");
	DeleteCriticalSection(&g_Console);
	printf("Stopped in main\n");
	return 0;
}
