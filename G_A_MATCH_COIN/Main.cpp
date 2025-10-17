#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "CServerSock.h"
#include "CAPIClient.h"
#include "CMarketData.h"
#include "CLimitOrders_bySymbol.h"
#include "CMarketOrders_bySymbol.h"
#include "COrderProcess.h"
#include "CMatchProcess_bySymbol.h"
#include "main.h"
#include "CGlobals.h"
#include <Windows.h>
#include "../Common/util.h"
#include "../Common/CODBC.h"
#include <tchar.h>


BOOL	g_bDebug;
HANDLE	g_hDieEvent;				// event for terminating process
volatile
BOOL	g_bContinue = TRUE;	// flag whether continue process or not
CRITICAL_SECTION	g_Console;


CGlobals	gCommon;
CMarketData	gMarketData;
map< string, CLimitOrders_bySymbol*>	gLimitOrdList;
map< string, CMarketOrders_bySymbol*>	gMarketOrdList;
vector< CMatchProcess_bySymbol*>		gMatchProcesses;

bool create_orderlists_matchprocess_by_symbol();
void destroy_orderlists_matchprocess_by_symbol();

int  _Start()
{
	if (!gCommon.Initialize())
		return -1;

	gCommon.log(INFO, FALSE, "[%s][%s] 를 시작합니다.", EXENAME, EXEC_VERSION);

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (!gMarketData.Initialize())
		return -1;

	CAPIClient apiClient;
	if (!apiClient.Initialize())
		return -1;

	/***/
	if (!create_orderlists_matchprocess_by_symbol())
		return -1;

	/***/
	COrderProcess orderProcess;

	/***/
	boost::asio::io_context ioContext;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard(boost::asio::make_work_guard(ioContext));
	
	char zPort[128];
	gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT", zPort);
	gCommon.log(INFO, TRUE, "Listen Port:%s", zPort);
	CServerSock server(ioContext, &orderProcess);
	if (!server.Initialize(atoi(zPort)))
		return 0;

	std::thread ioThread([&ioContext]() { ioContext.run(); });  // ioContext 실행
	


	DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	ioThread.join();

	destroy_orderlists_matchprocess_by_symbol();


	return 0;
}

bool create_orderlists_matchprocess_by_symbol()
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "DB정보 조회 오류.INI파일을 확인하세요");
		return FALSE;
	}

	char zConnStr[512] = { 0 };
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	CODBC odbc(DBMS_TYPE::MSSQL);
	if (!odbc.Initialize())
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}
	if (!odbc.Connect(zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "DB Connect 오류:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024] = { 0 };
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_SYMBOLS", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "Ini 파일에 쿼리가 없습니다.([QUERY][LOAD_SYMBOLS])");
		return false;
	}

	bool bReconnect;
	odbc.Init_ExecQry(zQ);
	if (!odbc.Exec_Qry(bReconnect)) {
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[OrderLists_MatchProcess]Load Symbols Query(%s)", zQ);

	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zSymbol[128] = { 0, };
		long dotCnt;

		odbc.GetDataStr(1, sizeof(zSymbol), zSymbol);
		odbc.GetDataLong(2, &dotCnt);

		CLimitOrders_bySymbol* pL = new CLimitOrders_bySymbol(zSymbol, dotCnt);
		CMarketOrders_bySymbol* pM = new CMarketOrders_bySymbol(zSymbol, dotCnt);
		gLimitOrdList[zSymbol] = pL;
		gMarketOrdList[zSymbol] = pM;
		
		gMatchProcesses.push_back(new CMatchProcess_bySymbol(zSymbol, dotCnt, pL, pM));
	}
	odbc.DeInit_ExecQry();

	if (gLimitOrdList.size() == 0)
	{
		gCommon.log(ERR, TRUE, "[종목정보조회 오류]INI 파일의 QUERY 확인필요(%s)", zQ);
		return false;
	}

	return true;
}

void destroy_orderlists_matchprocess_by_symbol()
{
	for (int i = 0; i < gMatchProcesses.size(); i++)
		delete gMatchProcesses[i];	

	for (auto& it : gLimitOrdList)
		delete it.second;
	
	for (auto& it2 : gMarketOrdList)
		delete it2.second;

	gMatchProcesses.clear();
	gLimitOrdList.clear();
	gMarketOrdList.clear();
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
