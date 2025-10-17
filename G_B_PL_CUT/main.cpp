#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "main.h"
//#include "CAPIClient.h"
#include <Windows.h>
#include <stdio.h>
//#include "../CommonAnsi/IRUM_Common.h"
#include "../Common/LogMsg.h"
#include "../Common/util.h"
#include "../Common/prop.h"
#include <tchar.h>
#include "CAPIClient.h"
#include "CBizClient.h"
#include "CMktTimeClient.h"
#include "CMainHandler.h"


BOOL	g_bDebug;
HANDLE	g_hDieEvent;				// event for terminating process
volatile 
BOOL	g_bContinue = TRUE;	// flag whether continue process or not
CRITICAL_SECTION	g_Console;


extern CLogMsg	g_log;
extern TCHAR	g_zConDir[_MAX_PATH];
extern TCHAR	g_zLogDir[_MAX_PATH];
extern char	g_zConfigFileName[MAX_PATH];

int  _Start()
{
	TCHAR	msg[512] = { 0, };

	//	GET Config DIR
	GetCurrentDirectory(_MAX_PATH, g_zConDir);
	CUtil::GetCnfgFileNm(g_zConDir, (CHAR*) EXENAME, g_zConfigFileName);
	sprintf(g_zLogDir, "%s\\Log", g_zConDir);

	if (!g_log.OpenLog(g_zLogDir, EXENAME))
	{
		printf("Open Log Error(%s)\n", g_zLogDir);
		return -1;
	}
		
	LOGGING(INFO, FALSE, "[%s][%s] 를 시작합니다.", EXENAME, EXEC_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	
	CMainHandler main;
	main.Initialize();

	CAPIClient apiF(main.GetThreadId());
	CAPIClient apiGf(main.GetThreadId());
	CBizClient biz(main.GetThreadId());
	CMktTimeClient mkt(main.GetThreadId());

	//CAPIClient apiClient;
	//if (!apiClient.Begin())
	//	return 0;

	//CIocp workers;
	//if (workers.Initialize() == FALSE)
	//{
	//	//LOGGING(ERR,TRUE, TEXT("IOCP initialize failed"));
	//}
	//else
	{
		DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	}

	DeleteCriticalSection(&g_Console);
	printf("Stopped.\n");
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

	DeleteCriticalSection(&g_Console);
	printf("Stopped.\n");
	return 0;
}
