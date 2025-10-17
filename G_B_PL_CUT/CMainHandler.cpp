#include "CMainHandler.h"
#include "../Common/CODBC.h"
#include "../Common/Util.h"
#include "Comm.h"


extern CStkInfo	g_stkInfo;

CMainHandler::CMainHandler()
{
	InitializeCriticalSection(&m_csUser);
}

CMainHandler::~CMainHandler()
{
	Finalize();
	DeleteCriticalSection(&m_csUser);
}


BOOL CMainHandler::Initialize()
{
	m_bContinue = TRUE;
	m_hMainThrd = (HANDLE)_beginthreadex(NULL, 0, &Thread_Main, this, 0, &m_unMainThrdId);


	Load_StkInfo();

	UserInfo_Load();


	return TRUE;
}


VOID	CMainHandler::Finalize()
{
	LOCK_CS(m_csUser);
	for (auto it = m_mapUser.begin(); it != m_mapUser.end(); ++it)
	{
		delete (*it).second;
	}
	m_mapUser.clear();
	UNLOCK_CS(m_csUser);
}

unsigned WINAPI CMainHandler::Thread_Main(LPVOID lp)
{
	CMainHandler* pThis = (CMainHandler*)lp;

	while (pThis->m_bContinue)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_CNTR)
			{
				pThis->CntrData( (string*)msg.lParam);
			}
			else if (msg.message == WM_INOUT)
			{
				pThis->InOutData((string*)msg.lParam);
			}
			else if (msg.message == WM_MKT_CLOSE)
			{
				pThis->MktCloseData((string*)msg.lParam);
			}
			else if (msg.message == WM_UNREG_CLIENT)
			{
				string sUserId = *(string*)msg.lParam;
				pThis->UserInfo_Delete(sUserId);
			}
			delete(char*)msg.lParam;
		}

	}
	return 0;
}

BOOL	CMainHandler::CntrData(_In_ string* psData)
{
	__MAX::TTA001* p = (__MAX::TTA001*)psData->c_str();

	char zUserId[32];
	sprintf(zUserId, "%.*s", sizeof(p->header.userid), p->header.userid);

	LOCK_CS(m_csUser);
	auto find = m_mapUser.find(zUserId);
	if (find == m_mapUser.end())
	{
		UserInfo_Create(zUserId);
		find = m_mapUser.find(zUserId);
		if (find == m_mapUser.end())
		{
			LOGGING(LOGTP_ERR, TRUE, "[%s]체결데이터 받았는데 UserInfo 생성 실패", zUserId);
			UNLOCK_CS(m_csUser);
			return FALSE;
		}
	}

	TData* pData = new TData;
	PostThreadMessage((*find).second->ThreadId(), WM_CNTR, (WPARAM)0, (LPARAM)pData);

	UNLOCK_CS(m_csUser);

	return TRUE;
}

BOOL	CMainHandler::InOutData(_In_ string* psData)
{
	__MAX::TNM001* p = (__MAX::TNM001*)psData->c_str();

	char zUserId[32];
	sprintf(zUserId, "%.*s", sizeof(p->header.userid), p->header.userid);

	LOCK_CS(m_csUser);
	auto find = m_mapUser.find(zUserId);
	if (find != m_mapUser.end())
	{
		TData* pData = new TData;
		PostThreadMessage((*find).second->ThreadId(), WM_INOUT, (WPARAM)0, (LPARAM)pData);
	}
	
	UNLOCK_CS(m_csUser);

	return TRUE;
}

BOOL	CMainHandler::MktCloseData(_In_ string* psData)
{
	__MAX::TNL001* p = (__MAX::TNL001*)psData->c_str();

	char zArtcCd[32];
	sprintf(zArtcCd, "%.*s", sizeof(p->artc_cd), p->artc_cd);

	LOCK_CS(m_csUser);
	for( auto it = m_mapUser.begin(); it!=m_mapUser.end(); ++it)
	{
		TData* pData = new TData;
		strcpy(pData->data, zArtcCd);
		PostThreadMessage((*it).second->ThreadId(), WM_MKT_CLOSE, (WPARAM)0, (LPARAM)pData);

	}
	
	UNLOCK_CS(m_csUser);

	return TRUE;
}


BOOL	CMainHandler::Load_StkInfo()
{
	char connStr[512] = { 0 };
	if (!MakeDBConnStr(connStr))
		return FALSE;

	CODBC odbc;
	if (!odbc.Initialize())
	{
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}
	if (!odbc.Connect(connStr))
	{
		LOGGING(LOGTP_ERR, TRUE, "DB Connect 오류:%s", odbc.getMsg());
		return FALSE;
	}


	char zQ[1024];
	sprintf(zQ,
		" SELECT X.ACNT_TP, X.ARTC_CD, X.TICK_SIZE, X.TICK_VALUE, X.DOT_CNT, X.STK_CD, X.MKT_TP, X.LOSSCUT_AMT"
		", ISNULL(Y.CNTR_PRC, 0) CURR_PRC"
		" FROM"
		" ("
		" SELECT A.ACNT_TP, A.ARTC_CD, TICK_SIZE, TICK_VALUE, DOT_CNT, b.STK_CD, A.MKT_TP"
		" FROM ARTC_MST A, STK_MST B"
		" WHERE A.ARTC_USE_YN = 'Y'"
		" AND A.ARTC_CD = B.ARTC_CD"
		" AND B.STK_USE_YN = 'Y'"
		" )X"
		" LEFT OUTER JOIN"
		" CURR_PRC Y"
		" ON X.STK_CD = Y.STK_CD"
	);

	odbc.Init_ExecQry(zQ);
	if (!odbc.Exec_Qry()) {
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}

	
	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zVal[1024] = { 0, };
		double dVal = 0;
		long lVal;

		TStkInfo* pInfo = new TStkInfo;


		odbc.GetDataStr(1, sizeof(zVal), zVal);
		pInfo->acntTp = zVal[0];

		odbc.GetDataStr(2, sizeof(zVal), zVal);
		pInfo->sArtcCd = zVal;

		odbc.GetDataDbl(3, &dVal);
		pInfo->dTickSize = dVal;

		odbc.GetDataDbl(4, &dVal);
		pInfo->dTickValue = dVal;

		odbc.GetDataLong(5, &lVal);
		pInfo->nDotCnt = lVal;

		odbc.GetDataStr(6, sizeof(zVal), zVal);
		pInfo->sStkCd = zVal;

		odbc.GetDataStr(7, sizeof(zVal), zVal);
		pInfo->mktTp = (EN_MKT_TP)atoi(zVal);

		odbc.GetDataLong(5, &pInfo->lLCAmt);


		odbc.GetDataDbl(9, &dVal);
		pInfo->dCurrPrc = dVal;
		__MAX::FormatPrc(dVal, pInfo->zCurrPrc);

		ZeroMemory(pInfo->zLastTime, sizeof(pInfo->zLastTime));

		g_stkInfo.AddStkInfo(pInfo);
	}

	odbc.DeInit_ExecQry();

	return TRUE;
}

BOOL CMainHandler::UserInfo_Load()
{
	char connStr[512] = { 0 };
	if (!MakeDBConnStr(connStr))
		return FALSE;


	CODBC odbc;
	if (!odbc.Initialize())
	{
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}
	if (!odbc.Connect(connStr))
	{
		LOGGING(LOGTP_ERR, TRUE, "DB Connect 오류:%s", odbc.getMsg());
		return FALSE;
	}


	char zQ[1024];
	sprintf(zQ,
		"SELECT X.ACNT_NO, Y.USER_ID"
		"FROM"
			"(SELECT ACNT_NO FROM NCLR_POS GROUP BY ACNT_NO) X"
			", ACNT_MST Y"
		"WHERE X.ACNT_NO = Y.ACNT_NO"
	);

	odbc.Init_ExecQry(zQ);
	if (!odbc.Exec_Qry()) {
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}


	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zVal[1024] = { 0, };

		odbc.GetDataStr(1, sizeof(zVal), zVal);
		string sAcntNo = zVal;

		odbc.GetDataStr(2, sizeof(zVal), zVal);
		string sUserId = zVal;

		UserInfo_Create(sUserId);
	}

	odbc.DeInit_ExecQry();

	return TRUE;
}
BOOL CMainHandler::UserInfo_Create(string sUserID)
{
	LOCK_CS(m_csUser);
	auto find = m_mapUser.find(sUserID);
	if (find == m_mapUser.end()) {
		UNLOCK_CS(m_csUser);
		return TRUE;
	}


	CUserThread* p = new CUserThread(sUserID, m_unMainThrdId);
	if (!p->Initialize()) {
		UNLOCK_CS(m_csUser);
		return FALSE;
	}

	UNLOCK_CS(m_csUser);

	m_mapUser[sUserID] = p;	
	return TRUE;
}

VOID CMainHandler::UserInfo_Delete(string sUserID)
{
	LOCK_CS(m_csUser);
	auto find = m_mapUser.find(sUserID);
	if (find != m_mapUser.end()) 
	{
		delete (*find).second;
		m_mapUser.erase(sUserID);
	}

	UNLOCK_CS(m_csUser);

}
