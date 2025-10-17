#pragma once

#include "comm.h"
#include "../Common/CODBC.h"

struct CPos
{
	string	sArtcCd;
	string	sStkCd;
	double	dAvgPrc;
	char	cBuySell;	// B, S
	double	dVol;
	double	dTickVal;
	double	dTickSize;
	int 	nDotCnt;
	char	zLastSiseTime[TIME_LEN + 1];

	CPos()
	{
		dAvgPrc = 0; dTickVal = 0; dTickSize = 0; ZeroMemory(zLastSiseTime, sizeof(zLastSiseTime));
	}
};

struct TUser
{
	string sUserId;
	string sAcntNo;
	LONG lNetAmt;
	LONG lLvg;
	LONG lLCAmt;
	LONG lPCAmt;
};

class CUserThread
{
public:
	CUserThread(string sUserId, UINT unMainThrdId);
	~CUserThread();

	BOOL Initialize();
	VOID Finalize();

	VOID Noti_Cntr();
	VOID Noti_InOut();
	VOID Noti_MktClose(string sArtcCd);

	UINT ThreadId() {return m_unMainThrdId;	}
private:
	static unsigned WINAPI Thread_Main(LPVOID lp);
	BOOL Read_Compose_PosInfo();
	VOID Clear_PosInfo();
	BOOL Read_AcntAmt();
	BOOL ConnectDB();
	VOID Calc_Cut();

	VOID Cancel_AllOrders();
	VOID CutOrders(BOOL bLosscut);
private:

	TUser	m_User;
	BOOL	m_bContinue;

	UINT	m_unMainThrdId = 0;
	HANDLE	m_hMainThrd = NULL;

	char	m_zDBConnStr[256];
	
	map<string, CPos*>	m_mapPos;

	CODBC	*m_db;

};

