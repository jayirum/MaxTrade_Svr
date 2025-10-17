#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "CAPIClient.h"
#include "CSenderList.h"
#include "CAcceptor.h"
#include "main.h"
#include "CGlobals.h"
#include <Windows.h>
#include "../Common/util.h"
#include <tchar.h>


HANDLE	g_hDieEvent;				// event for terminating process



CGlobals		gCommon;
CSenderList		gSenders;

int  _Start()
{
	if (!gCommon.Initialize()) {
		printf("Failed to Initialze Globals\n");
		return -1;
	}

	//std::this_thread::sleep_for(std::chrono::seconds(1));

	gCommon.log(INFO, TRUE, "-----------------------------------------------");
	gCommon.log(INFO, TRUE, "1.[%s][%s] Starting.", EXENAME, EXEC_VERSION);
	gCommon.log(INFO, TRUE, "-----------------------------------------------");

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	
	/*
	CSenders
	*/
	char val[1024] = { 0 };
	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"SENDER_THREAD_CNT", val)) {
		gCommon.log(LOGTP_ERR, TRUE, "Ini file does not have [SENDER_THREAD_CNT]");
		return false;
	}
	gCommon.debug("2.SENDER_THREAD_CNT:%s", val);
	CSenderList sender;
	if (!sender.Initialize(atoi(val))) {
		gCommon.log(INFO, FALSE, "Failed to Initialize CSenderList");
		return 0;
	}
	gCommon.log(INFO, TRUE, "3.CSenderList Initialized successfully");
	std::this_thread::sleep_for(std::chrono::seconds(1));

	/*
	CAcceptor
	*/
	boost::asio::io_context ioContext;
	char zPort[128] = { 0 };
	gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT", zPort);
	gCommon.debug("4.Listen Port:%s", zPort);
	CAcceptor acceptor(ioContext, atoi(zPort));
	acceptor.Initialize();
	gCommon.log(INFO, TRUE, "5.CAcceptor Initialized successfully.ListenPort:%s", zPort);

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
	gCommon.log(INFO, TRUE, "6.CAPIClient Connected successfully");

	
	DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	
	acceptor.DeIitialize();
	sender.DeInitialize();
	api.DeInitialize();

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

	_Start();

	printf("Stopped in main\n");
	return 0;
}
