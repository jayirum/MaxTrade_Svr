
#pragma warning(disable:4786)
#pragma warning(disable:4503)


//#include "AlgoFront.h"

#include "main.h"

#include <stdio.h>
#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include "../../Common/timeutils.h"
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


	if (!LoadDBName()) {
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	char zFileFind[_MAX_PATH];
	sprintf(zFileFind, "%s\\*.*", zBackupDir);
	CFileFind finder;
	BOOL bResult = finder.FindFile(zFileFind);

	char zDelDateCnt[32] = { 0, };
	CUtil::GetConfig(g_wzConfig, TEXT("DELETE_DATE_CNT"), TEXT("CNT"), zDelDateCnt);
	if (zDelDateCnt[0] == 0)
	{
		g_log.log(LOGTP_ERR, "AUTO DEL 일자 정보 확인 필요 (INI파일)");
		return 0;
	}


	SYSTEMTIME st;	char zToday[32] = { 0, }, zDelDate[32] = { 0, };
	GetLocalTime(&st);
	sprintf(zToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	CTimeUtils timeUtil;
	strcpy(zDelDate, timeUtil.GetPrevDate(zToday, atoi(zDelDateCnt)));
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
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

		BOOL bFind = IsSubject(zFileName);
		if (bFind)
		{
			//echatweb1000g_backup_2024_01_29_060001_0264354
			char* pDate = strstr(zFileName, "_202");
			char zFileDate[32] = { 0, };
			sprintf(zFileDate, "%.4s%.2s%.2s", pDate + 1, pDate + 6, pDate + 9);

			int nDateComp = strcmp(zDelDate, zFileDate);
			if (nDateComp>0 )
			{
				nFoundCnt++;
				printf("[%s] 기존파일을 삭제 합니다.\n", zFileName);
				g_log.log(LOGTP_SUCC, "[%s] 기존파일을 삭제 합니다.\n", zFileName);

				char zFullFileName[_MAX_PATH] = { 0, };
				sprintf(zFullFileName, "%s\\%s", zBackupDir, zFileName);
				if (!DeleteFile(zFullFileName))
				{
					printf("DeleteFile failed:%d\n", GetLastError());
					g_log.log(LOGTP_ERR, "DeleteFile failed:%d\n", GetLastError());
					continue;
				}
				nSentCnt++;
			}
		}

		
	}
	finder.Close();

	printf("[%d]/[%d] 파일을 삭제했습니다.\n", nSentCnt, nFoundCnt);
	g_log.log(LOGTP_SUCC, "[%d]/[%d] 파일을 삭제했습니다.\n", nSentCnt, nFoundCnt);

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

