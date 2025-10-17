
#pragma warning(disable:4786)
#pragma warning(disable:4503)


//#include "AlgoFront.h"

#include "main.h"

#include <stdio.h>
#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include <tchar.h>
#include <locale.h>
#include <afxinet.h>
#include <vector>
#include <memory>

HANDLE	g_hDieEvent;				// event for terminating process
CRITICAL_SECTION	g_Console;

/// global variables shared with all classes
BOOL		g_bContinue = TRUE;	// flag whether continue process or not
CLogMsg		g_log;
char		g_wzConfig[_MAX_PATH];
BOOL		g_bDebugLog = FALSE;
//CMemPool	g_memPool(_IRUM::MEM_PRE_ALLOC, _IRUM::MEM_MAX_ALLOC, _IRUM::MEM_BLOCK_SIZE);
TCHAR		g_zMyName[128];

BOOL WINAPI ControlHandler(DWORD dwCtrlType);

std::vector<CString>	g_vecDB;


int main(int argc, LPWSTR* argv)
{
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	InitializeCriticalSection(&g_Console);

	TCHAR	msg[512] = { 0, };
	TCHAR	szDir[_MAX_PATH] = { 0, }, zBackupDir[_MAX_PATH] = { 0, };

	//	GET LOG DIR
	CUtil::GetMyModuleAndDir(szDir, msg, g_wzConfig);
	CUtil::GetConfig(g_wzConfig, TEXT("DIR"), TEXT("BACKUP"), zBackupDir);

	char zLogDir[_MAX_PATH]; sprintf(zLogDir, "%s\\Log", szDir);
	g_log.OpenLog(zLogDir, EXENAME);
	g_log.log(LOGTP_SUCC, TEXT("-----------------------------------------------------"));
	g_log.log(LOGTP_SUCC, TEXT("AutoBackup - FTP 를 시작합니다. [%s][%s]"), TEXT(__APP_VERSION), TEXT(__DATE__));
	g_log.log(LOGTP_SUCC, TEXT("-----------------------------------------------------"));

	CUtil::GetConfig(g_wzConfig, TEXT("DEBUG"), TEXT("LOG_DEBUG"), msg);
	if (msg[0] == 'Y')	g_bDebugLog = TRUE;



	char zSvrIP[128] = { 0, }, zSvrPort[128] = { 0, }, zID[128] = { 0, }, zPwd[128] = { 0, }, zPassive[32] = { 0, };
	CUtil::GetConfig(g_wzConfig, TEXT("FTPSERVER"), TEXT("IP"), zSvrIP);
	CUtil::GetConfig(g_wzConfig, TEXT("FTPSERVER"), TEXT("PORT"), zSvrPort);
	CUtil::GetConfig(g_wzConfig, TEXT("FTPSERVER"), TEXT("ID"), zID);
	CUtil::GetConfig(g_wzConfig, TEXT("FTPSERVER"), TEXT("PWD"), zPwd);
	CUtil::GetConfig(g_wzConfig, TEXT("FTPSERVER"), TEXT("PASSIVE"), zPassive);


	if (zSvrIP[0] == 0 || zSvrPort[0] == 0 || zID[0] == 0 || zPwd[0] == 0 || zPassive[0] == 0)
	{
		g_log.log(LOGTP_ERR, "FTP 서버 정보 확인 필요 (INI파일)");
		return 0;
	}


	if (!LoadDBName()) {
		return 0;
	}



	auto_ptr< CInternetSession> iSession ( new CInternetSession);
	CFtpConnection* pFtp;

	
	
	try
	{
		pFtp = iSession->GetFtpConnection(zSvrIP, zID , zPwd, atoi(zSvrPort), (zPassive[0]=='Y'));
	}
	catch(CInternetException* e)
	{
		char zErr[1024] = { 0, };
		e->GetErrorMessage(zErr, 1024);
		printf("FTP connect failed:%s\n", zErr);
		g_log.log(LOGTP_ERR, "FTP connect failed:%s\n", zErr);
		return 0;
	}
	printf("FTP connect ok\n");
	g_log.log(LOGTP_SUCC, "FTP connect OK(%s)(%s)(%s)(%s)\n", zSvrIP, zSvrPort, zID, zPwd);


	char zFileFind[_MAX_PATH];
	sprintf(zFileFind, "%s\\*.*", zBackupDir);
	CFileFind finder;
	BOOL bResult = finder.FindFile(zFileFind);
	SYSTEMTIME st;
	char zDate[32] = { 0, };
	GetLocalTime(&st);
	sprintf(zDate, "%04d_%02d_%02d", st.wYear, st.wMonth, st.wDay);
	char* pDbName = NULL;
	char* pDate = NULL;
	int nSentCnt = 0, nFoundCnt = 0;
	while (bResult)
	{
		bResult = finder.FindNextFile();

		if (finder.IsDirectory() || finder.IsDots())
		{
			continue;
		}
		CString sFileName = finder.GetFileName();
		char zFileName[_MAX_PATH];	strcpy(zFileName, (LPCSTR)sFileName);
		//printf("FileName:%s\n", zFileName);

		BOOL bFind = IsSubject(zFileName);
		if (bFind)
		{
			pDate = strstr(zFileName, zDate);
			if (pDate)
			{
				nFoundCnt++;
				printf("[%s] FTP 전송 합니다.\n", zFileName);
				g_log.log(LOGTP_SUCC, "[%s] FTP 전송 합니다.\n", zFileName);

				char sending[_MAX_PATH];
				sprintf(sending, "%s\\%s", zBackupDir, zFileName);
				if (!pFtp->PutFile(sending, zFileName, FTP_TRANSFER_TYPE_BINARY, 1))
				{
					g_log.log(LOGTP_ERR, "ftp send 오류(%s):%d\n", zFileName, GetLastError());
					printf("ftp send 오류:%d\n", GetLastError());
					continue;
				}
				printf("[%s] FTP 전송 완료했습니다.\n", zFileName);
				g_log.log(LOGTP_SUCC, "[%s] FTP 전송 완료했습니다.\n", zFileName);
				nSentCnt++;
			}
		}

		
	}
	finder.Close();

	printf("[%d]/[%d] 파일을 전송했습니다.\n", nSentCnt, nFoundCnt);
	g_log.log(LOGTP_SUCC, "[%d]/[%d] 파일을 전송했습니다.\n", nSentCnt, nFoundCnt);

	Sleep(3000);
	
	printf("AutoBackup 종료합니다.\n");
	g_log.log(LOGTP_SUCC, "AutoBackup 종료합니다.\n");


	return 0;
}


BOOL IsSubject(char* pzFileName)
{
	for (int i = 0; i < g_vecDB.size(); i++)
	{
		char zDBName[128];
		strcpy(zDBName, (LPCSTR)g_vecDB[i]);
		char* pFind = strstr(pzFileName, zDBName);
		if (pFind)
			return TRUE;
	}
	return FALSE;
}

BOOL LoadDBName()
{
	char zCnt[32] = { 0, };
	char zDB[128] = { 0, };

	g_vecDB.clear();

	CUtil::GetConfig(g_wzConfig, TEXT("DB"), TEXT("CNT"), zCnt);
	int nCnt = atoi(zCnt);
	if (nCnt == 0)
	{
		printf("INI 의 [DB] 정보 확인 요.CNT==0");
		g_log.log(LOGTP_ERR, "INI 의 [DB] 정보 확인 요.CNT==0");
		return FALSE;
	}

	for (int i = 0; i < nCnt; i++)
	{
		char zIdx[32]; sprintf(zIdx, "%d", i);
		char zDBName[128] = { 0, };
		CUtil::GetConfig(g_wzConfig, TEXT("DB"), zIdx, zDBName);
		if (zDBName[0] == 0)
		{
			printf("INI 의 [DB] 정보 확인 요.IDX=%s", zIdx);
			g_log.log(LOGTP_ERR, "INI 의 [DB] 정보 확인 요.IDX=%s", zIdx);
			return FALSE;
		}
		g_vecDB.push_back(CString(zDBName));
	}

	return TRUE;
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
		printf("Stopping ...\n");
		SetEvent(g_hDieEvent);
		g_bContinue = FALSE;
		return TRUE;
		break;

	}
	return FALSE;
}

