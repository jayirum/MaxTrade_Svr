#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "CMktOpenClose.h"
#include "main.h"
#include "CGlobals.h"
#include "CBatchProcess.h"
#include <Windows.h>
#include "../Common/util.h"
#include <tchar.h>


HANDLE	g_hDieEvent;				// event for terminating process
CRITICAL_SECTION	g_Console;


CGlobals		gCommon;


int  _Start()
{
	if (!gCommon.Initialize()) {
		printf("gCommon.Initialize() failed...\n");
		return -1;
	}

	gCommon.log(INFO, TRUE, "\n[%s][%s] 를 시작합니다.\n", EXENAME, EXE_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	CMktOpenClose mkt;
	if (!mkt.Initialize())
		return -1;

	CBatchProcess batch;
	if (!batch.Initialize())
		return -1;



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
	//case CTRL_LOGOFF_EVENT:
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
