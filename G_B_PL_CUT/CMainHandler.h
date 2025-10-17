#pragma once

#include "../Common/MaxTradeInc.h"
#include "Comm.h"
#include "CUserThread.h"
#include <map>
using namespace std;



class CMainHandler
{
public:
	CMainHandler();
	~CMainHandler();

	BOOL	Initialize();
	VOID	Finalize();

	static unsigned WINAPI Thread_Main(LPVOID lp);
	UINT	GetThreadId() { return m_unMainThrdId; }
private:
	BOOL	Load_StkInfo();
	BOOL	UserInfo_Load();
	BOOL	UserInfo_Create(string sUserID);
	VOID	UserInfo_Delete(string sUserID);

	BOOL	CntrData(_In_ string* psData);
	BOOL	InOutData(_In_ string* psData); 
	BOOL	MktCloseData(_In_ string* psData);
	

private:
	UINT						m_unMainThrdId = 0;
	HANDLE						m_hMainThrd = NULL;
	map<string, CUserThread*>	m_mapUser;
	CRITICAL_SECTION			m_csUser;
	BOOL						m_bContinue = FALSE;
};

