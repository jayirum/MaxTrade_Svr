#include "CUserThread.h"


extern CStkInfo	g_stkInfo;

CUserThread::CUserThread(string sUserId, UINT unMainThrdId)
{
	m_User.sUserId = sUserId;
	m_unMainThrdId = unMainThrdId;
	m_db = NULL;
}

CUserThread::~CUserThread()
{
	m_bContinue = FALSE;
	Finalize();
}
BOOL CUserThread::Initialize()
{
	m_bContinue = TRUE;
	m_hMainThrd = (HANDLE)_beginthreadex(NULL, 0, &Thread_Main, this, 0, &m_unMainThrdId);

	if (!ConnectDB())
		return FALSE;

	return TRUE;
}

VOID CUserThread::Finalize()
{
	Clear_PosInfo();
	delete m_db;
}



VOID CUserThread::Clear_PosInfo()
{
	for (auto it = m_mapPos.begin(); it != m_mapPos.end(); ++it)
		delete (*it).second;
	m_mapPos.clear();
}


BOOL CUserThread::ConnectDB()
{
	if (!m_db)
	{
		m_db = new CODBC;
		if (!m_db->Initialize())
		{
			LOGGING(LOGTP_ERR, TRUE, m_db->getMsg());
			return FALSE;
		}
	}

	if (!m_db->IsConnected())
	{
		if (!MakeDBConnStr(m_zDBConnStr))
			return FALSE;

		if (!m_db->Connect(m_zDBConnStr))
		{
			LOGGING(LOGTP_ERR, TRUE, "DB Connect 오류:%s", m_db->getMsg());
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CUserThread::Read_AcntAmt()
{
	char zQ[1024];
	sprintf(zQ,
		"SELECT "
		" ISNULL(Y.ACNT_NO, '') ACNT_NO"
		", Y.ACNT_AMT + Y.CLR_PL - Y.CMSN AS NET_AMT"
		", LEVERAGE"
		", ISNULL(PROFITCUT_AMT,0)"
		" FROM ACNT_MST WHERE USER_ID = '%s'"
		, m_User.sUserId.c_str()
	);

	if (!ConnectDB())
		return FALSE;

	m_db->Init_ExecQry(zQ);
	if (!m_db->Exec_Qry()) {
		LOGGING(LOGTP_ERR, TRUE, m_db->getMsg());
		return FALSE;
	}


	int row = 0;
	BOOL bSendSise = TRUE;
	while (m_db->GetNextData())
	{
		char zVal[1024] = { 0, };
		LONG lVal = 0;
		m_db->GetDataStr(1, sizeof(zVal), zVal);
		m_User.sAcntNo = zVal;

		m_db->GetDataLong(2, &m_User.lNetAmt);
		m_db->GetDataLong(3, &m_User.lLvg);
		m_db->GetDataLong(3, &m_User.lPCAmt);
	}

	m_db->DeInit_ExecQry();

	return TRUE;
}

BOOL CUserThread::Read_Compose_PosInfo()
{
	char zQ[1024];
	sprintf(zQ,
		"SELECT "
		" ISNULL(X.STK_CD, '') STK_CD"
		", ISNULL(X.ARTC_CD, '') ARTC_CD"
		", ISNULL(X.BS_TP, '') BS_TP"
		", ISNULL(X.AVG_PRC, 0) AVG_PRC"
		", ISNULL(X.NCLR_POS_QTY, 0) NCLR_POS_QTY"
		" FROM "
		" (SELECT ACNT_NO, ARTC_CD, STK_CD, BS_TP, AVG_PRC, NCLR_POS_QTY FROM NCLR_POS) X"
		", ACNT_MST Y "
		" WHERE USER_ID = '%s'"
		" AND X.ACNT_NO = Y.ACNT_NO"
		, m_User.sUserId.c_str()
	);

	if (!ConnectDB())
		return FALSE;

	m_db->Init_ExecQry(zQ);
	if (!m_db->Exec_Qry()) {
		LOGGING(LOGTP_ERR, TRUE, m_db->getMsg());
		return FALSE;
	}


	int row = 0;
	BOOL bSendSise = TRUE;
	while (m_db->GetNextData())
	{
		char zVal[1024] = { 0, };
		double dVal = 0;
		LONG lVal = 0;

		// STK_CD
		m_db->GetDataStr(1, sizeof(zVal), zVal);
		if (zVal[0] == 0x00)
			break;
		string sStkCd = zVal;

		CPos* pos = new CPos;
		if (!g_stkInfo.GetArtcInfo(sStkCd, &pos->dTickVal, &pos->dTickSize, &pos->nDotCnt, &m_User.lLCAmt))
		{
			delete pos;
			LOGGING(LOGTP_ERR, TRUE, "(%s)종목에 대한 종목정보가 없다.", sStkCd.c_str());
			return FALSE;
		}

		m_db->GetDataStr(2, sizeof(zVal), zVal);
		pos->sArtcCd = zVal;

		m_db->GetDataStr(3, sizeof(zVal), zVal);
		pos->cBuySell = zVal[0];

		m_db->GetDataDbl(4, &pos->dAvgPrc);
		
		m_db->GetDataLong(5, &lVal);
		pos->dVol = (double)lVal;

		m_mapPos[sStkCd] = pos;
	}

	m_db->DeInit_ExecQry();

	return TRUE;
}

VOID CUserThread::Noti_Cntr()
{
	if (!Read_AcntAmt())
		return;

	Clear_PosInfo();
	Read_Compose_PosInfo();		
}


VOID CUserThread::Noti_InOut()
{
	Read_AcntAmt();
}

VOID CUserThread::Noti_MktClose(string sArtcCd)
{
	if (!Read_AcntAmt())
		return;

	Clear_PosInfo();
	Read_Compose_PosInfo();
}

unsigned WINAPI CUserThread::Thread_Main(LPVOID lp)
{
	CUserThread* pThis = (CUserThread*)lp;

	while (pThis->m_bContinue)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_CNTR)
			{
				pThis->Noti_Cntr();
			}
			else if (msg.message == WM_INOUT)
			{
				pThis->Noti_InOut();
			}
			else if (msg.message == WM_MKT_CLOSE)
			{
				pThis->Noti_MktClose( ((TData*)msg.lParam)->data);
			}
			delete(char*)msg.lParam;
		}

		pThis->Calc_Cut();

	}
	return 0;
}


VOID CUserThread::Calc_Cut()
{
	LONG lAcmlPL = 0;
	for (auto it = m_mapPos.begin(); it != m_mapPos.end(); ++it)
	{
		double dCurrPrc = 0;
		char zLasTime[TIME_LEN + 1] = { 0 };
		CPos* pos = (*it).second;

		if (!g_stkInfo.GetCurrPrc(pos->sStkCd, &dCurrPrc, zLasTime))
		{
			//TODO. LOG
			return;
		}
		if (strncmp(zLasTime, pos->zLastSiseTime, TIME_LEN) >= 0)
		{
			continue;
		}

		double dSign = (pos->cBuySell == 'S') ? 1 : -1;
		double dPL = (pos->dAvgPrc - dCurrPrc) * dSign * pos->dVol;
		dPL = dPL * pos->dTickVal / pos->dTickSize / (double)m_User.lLvg;

		lAcmlPL += dPL;
	}

	BOOL bLossCut = TRUE;
	if (lAcmlPL < m_User.lLCAmt)
	{
		CutOrders(bLossCut);
	}
	else if (lAcmlPL >= m_User.lPCAmt)
	{
		CutOrders(!bLossCut);
	}
}

VOID CUserThread::Cancel_AllOrders()
{
}

VOID CUserThread::CutOrders(BOOL bLosscut)
{
	Cancel_AllOrders();
}

/*
	cut

	1. acnt_mst 에 losscut_yn - db
	2. biz 취소
	3. oms 취소
	4. cut 청산 - 자체적으로 체결 sp 호출
	5. pos clear
*/