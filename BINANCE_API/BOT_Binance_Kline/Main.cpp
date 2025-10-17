#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "main.h"
#include "CBinanceApi.h"
#include "CGlobals.h"
#include "CDBSave.h"
#include "CMSSQLSave.h"
#include <Windows.h>
#include "../../Common/util.h"
#include "../../Common/MaxTradeInc.h"
#include <tchar.h>
#include <mutex>

HANDLE		g_hDieEvent;				// event for terminating process
std::mutex	g_mtxConsole;
CGlobals	gCommon;


int  _Start()
{
	if (!gCommon.Initialize()) {
		printf("gCommon.Initialize() failed...\n");
		return -1;
	}

	gCommon.log(INFO, TRUE, "[%s][%s] Start....", EXENAME, EXE_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
	DBSaver
	*/
	std::shared_ptr<CDBSave> dbSave = std::make_shared<CDBSave>();
	char zNum[128] = { 0 };
	gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_THREAD_CNT", zNum);
	if (!dbSave->create_workers( atoi(zNum) ))
		return 0;

	/*
	MSSQL Saver
	*/
	std::shared_ptr<CMSSQLSave> mssqlSave = std::make_shared<CMSSQLSave>();
	if (!mssqlSave->create_workers())
		return 0;

	/*
	API of Binance
	*/
	CBinanceApi binance(dbSave, mssqlSave);
	if (!binance.Initialize())
		return 0;
	gCommon.log(INFO, TRUE, "CBinanceApi initialized successfully***");

	{
		DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	}


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
