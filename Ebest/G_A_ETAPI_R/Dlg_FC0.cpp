// Dlg_FC0.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "Dlg_FC0.h"
#include "./Packet/FC0.h"
#include "Comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern vector<string>			g_vecStk;
extern map<string, CCircularQ*>	g_mapQ;



/////////////////////////////////////////////////////////////////////////////
// CDlg_FC0 dialog

IMPLEMENT_DYNCREATE(CDlg_FC0, CDialog)

CDlg_FC0::CDlg_FC0(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_FC0::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_FC0)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_FC0::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_FC0)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK, m_ctrlOutBlock);
}


BEGIN_MESSAGE_MAP(CDlg_FC0, CDialog)
	//{{AFX_MSG_MAP(CDlg_FC0)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY  ()
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,				OnButtonRequest	    )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_REAL_DATA,	OnXMReceiveRealData	)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_FC0 message handlers

BOOL CDlg_FC0::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();

	SetThreadRun();
	HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_SaveIntoQ, this, 0, &m_unThrdSave);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_FC0::OnDestroy()
{
	SetThreadStop();

	//UnadviseData();

	CDialog::OnDestroy();
}

void CDlg_FC0::OnButtonRequest() 
{
	BOOL bSise = TRUE;

	// 선물 호가/시세 요청
	for (auto it = g_vecStk.begin(); it != g_vecStk.end(); ++it)
	{
		UnadviseData(bSise, (*it));
		Sleep(10);
		AdviseData(bSise, (*it));

		BOOL bSise = TRUE;
		UnadviseData(!bSise, (*it));
		Sleep(10);
		AdviseData(!bSise, (*it));
	}
}


//--------------------------------------------------------------------------------------
// 데이터 Advise
//--------------------------------------------------------------------------------------
void CDlg_FC0::AdviseData(BOOL bSise, string sStkCd)
{
	char zTrCode[32] = { 0 };
	char zMsg[128] = { 0 };
	int nSize;

	if (bSise)
	{
		strcpy(zTrCode, APICODE_SISE);
		nSize = sizeof(FC0_InBlock);
		strcpy(zMsg, "시세");
	}
	{
		strcpy(zTrCode, APICODE_HOGA);
		nSize = sizeof(FH0_InBlock);
		strcpy(zMsg, "호가");
	}

	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.AdviseRealData( 
		GetSafeHwnd(),	// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		zTrCode,		// TR 번호
		sStkCd.c_str(),	// 종목코드
		nSize			// 종목코드 길이
		);

	//-----------------------------------------------------------
	// 에러체크
	if( bSuccess == FALSE )
	{
		LOGGING(LOGTP_ERR, FALSE, "[%s][%s] 조회 오류(g_iXingAPI.AdviseRealData)", sStkCd.c_str(), zMsg);
		MessageBox( "조회실패", "에러", MB_ICONSTOP );
	}
	else
	{
		LOGGING(LOGTP_SUCC, FALSE, "[%s][%s] 조회 성공", sStkCd.c_str(), zMsg);
	}
}

//--------------------------------------------------------------------------------------
// 데이터 Unadvise
//--------------------------------------------------------------------------------------
void CDlg_FC0::UnadviseData(BOOL bSise, string sStkCd)
{
	char zTrCode[32] = { 0 };
	char zMsg[128] = { 0 };
	int nSize;

	if (bSise)
	{
		strcpy(zTrCode, APICODE_SISE);
		nSize = sizeof(FC0_InBlock);
		strcpy(zMsg, "시세");
	}
	{
		strcpy(zTrCode, APICODE_HOGA);
		nSize = sizeof(FH0_InBlock);
		strcpy(zMsg, "호가");
	}
	
	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.UnadviseRealData( 
		GetSafeHwnd(),		// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		zTrCode,			// TR 번호
		sStkCd.c_str(),		// 종목코드
		nSize				// 종목코드 길이
		);
	
	
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_FC0::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	TCQUnit* pQUnit = new TCQUnit;
	memcpy(pQUnit->buf, pRealPacket->pszData, pRealPacket->nDataLength);
	pQUnit->dataSize = pRealPacket->nDataLength;

	PostThreadMessage(m_unThrdSave, WM_MARKETDATA, 0, (LPARAM)pQUnit);

	//if( strcmp( pRealPacket->szTrCode, "FC0" ) == 0 )
	//{
	//	LPFC0_OutBlock pOutBlock = (LPFC0_OutBlock)pRealPacket->pszData;
	//	int nRow = 0;
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->chetime    , sizeof( pOutBlock->chetime     ), DATA_TYPE_STRING    ) );    // 체결시간          
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->sign       , sizeof( pOutBlock->sign        ), DATA_TYPE_STRING    ) );    // 전일대비구분      
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->change     , sizeof( pOutBlock->change      ), DATA_TYPE_FLOAT , 2 ) );    // 전일대비          
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->drate      , sizeof( pOutBlock->drate       ), DATA_TYPE_FLOAT , 2 ) );    // 등락율            
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->price      , sizeof( pOutBlock->price       ), DATA_TYPE_FLOAT , 2 ) );    // 현재가            
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->open       , sizeof( pOutBlock->open        ), DATA_TYPE_FLOAT , 2 ) );    // 시가              
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->high       , sizeof( pOutBlock->high        ), DATA_TYPE_FLOAT , 2 ) );    // 고가              
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->low        , sizeof( pOutBlock->low         ), DATA_TYPE_FLOAT , 2 ) );    // 저가              
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->cgubun     , sizeof( pOutBlock->cgubun      ), DATA_TYPE_STRING    ) );    // 체결구분          
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->cvolume    , sizeof( pOutBlock->cvolume     ), DATA_TYPE_LONG      ) );    // 체결량            
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->volume     , sizeof( pOutBlock->volume      ), DATA_TYPE_LONG      ) );    // 누적거래량        
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->value      , sizeof( pOutBlock->value       ), DATA_TYPE_LONG      ) );    // 누적거래대금      
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mdvolume   , sizeof( pOutBlock->mdvolume    ), DATA_TYPE_LONG      ) );    // 매도누적체결량    
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mdchecnt   , sizeof( pOutBlock->mdchecnt    ), DATA_TYPE_LONG      ) );    // 매도누적체결건수  
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->msvolume   , sizeof( pOutBlock->msvolume    ), DATA_TYPE_LONG      ) );    // 매수누적체결량    
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mschecnt   , sizeof( pOutBlock->mschecnt    ), DATA_TYPE_LONG      ) );    // 매수누적체결건수  
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->cpower     , sizeof( pOutBlock->cpower      ), DATA_TYPE_FLOAT , 2 ) );    // 체결강도          
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->offerho1   , sizeof( pOutBlock->offerho1    ), DATA_TYPE_FLOAT , 2 ) );    // 매도호가1         
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->bidho1     , sizeof( pOutBlock->bidho1      ), DATA_TYPE_FLOAT , 2 ) );    // 매수호가1         
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->openyak    , sizeof( pOutBlock->openyak     ), DATA_TYPE_LONG      ) );    // 미결제약정수량    
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->k200jisu   , sizeof( pOutBlock->k200jisu    ), DATA_TYPE_FLOAT , 2 ) );    // KOSPI200지수      
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->theoryprice, sizeof( pOutBlock->theoryprice ), DATA_TYPE_FLOAT , 2 ) );    // 이론가            
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->kasis      , sizeof( pOutBlock->kasis       ), DATA_TYPE_FLOAT , 2 ) );    // 괴리율            
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->sbasis     , sizeof( pOutBlock->sbasis      ), DATA_TYPE_FLOAT , 2 ) );    // 시장BASIS         
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ibasis     , sizeof( pOutBlock->ibasis      ), DATA_TYPE_FLOAT , 2 ) );    // 이론BASIS         
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->openyakcha , sizeof( pOutBlock->openyakcha  ), DATA_TYPE_LONG      ) );    // 미결제약정증감    
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->jgubun     , sizeof( pOutBlock->jgubun      ), DATA_TYPE_STRING    ) );    // 장운영정보        
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->jnilvolume , sizeof( pOutBlock->jnilvolume  ), DATA_TYPE_LONG      ) );    // 전일동시간대거래량
	//	m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->futcode    , sizeof( pOutBlock->futcode     ), DATA_TYPE_STRING    ) );    // 단축코드          
	//}

	return 0L;
}


unsigned WINAPI CDlg_FC0::Thread_SaveIntoQ(LPVOID lp)
{
	CDlg_FC0* pThis = (CDlg_FC0*)lp;
	char	zMsg[__MAX::BUFLEN_1K];

	for (auto it = g_mapQ.begin(); it != g_mapQ.end(); ++it)
	{
		(*it).second->W_SetWriter(GetCurrentThreadId());
	}

	while (pThis->m_bContinue)
	{
		Sleep(1);
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == FALSE)
			continue;
		if (msg.message == WM_MARKETDATA)
		{
			TCQUnit* pUnit = (TCQUnit*)msg.lParam;
			LPRECV_REAL_PACKET pApiPack = (LPRECV_REAL_PACKET)pUnit->buf;
			char zStkCd[32] = { 0 };
			if (strcmp(pApiPack->szTrCode, APICODE_SISE) == 0)
			{
				LPFC0_OutBlock p = (LPFC0_OutBlock)pApiPack->pszData;
				memcpy(zStkCd, p->futcode, sizeof(p->futcode));
			}
			else if (strcmp(pApiPack->szTrCode, APICODE_HOGA) == 0)
			{
				LPFH0_OutBlock p = (LPFH0_OutBlock)pApiPack->pszData;
				memcpy(zStkCd, p->futcode, sizeof(p->futcode));
			}

			auto itFind = g_mapQ.find(zStkCd);
			if (itFind != g_mapQ.end())
			{
				CCircularQ* pQ = (CCircularQ*)(*itFind).second;
				pQ->W_AddData(pUnit, zMsg);
			}
			else
			{
				LOGGING(LOGTP_ERR, FALSE, "[%s]종목데이터 수신했는데, Q 가 없다.", zStkCd);
			}
		}
		delete (TCQUnit*)msg.lParam;

	} // while(pThis->m_bRun)


	return 0;
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_FC0::InitCtrls()
{
#if 0
	//-------------------------------------------------------------------------
	// OutBlock
	m_ctrlOutBlock.InsertColumn(0, "필드  ", LVCFMT_LEFT, 150);
	m_ctrlOutBlock.InsertColumn(1, "데이터", LVCFMT_LEFT, 200);

	int nRow = 0;
	m_ctrlOutBlock.InsertItem(nRow++, "체결시간          ");
	m_ctrlOutBlock.InsertItem(nRow++, "전일대비구분      ");
	m_ctrlOutBlock.InsertItem(nRow++, "전일대비          ");
	m_ctrlOutBlock.InsertItem(nRow++, "등락율            ");
	m_ctrlOutBlock.InsertItem(nRow++, "현재가            ");
	m_ctrlOutBlock.InsertItem(nRow++, "시가              ");
	m_ctrlOutBlock.InsertItem(nRow++, "고가              ");
	m_ctrlOutBlock.InsertItem(nRow++, "저가              ");
	m_ctrlOutBlock.InsertItem(nRow++, "체결구분          ");
	m_ctrlOutBlock.InsertItem(nRow++, "체결량            ");
	m_ctrlOutBlock.InsertItem(nRow++, "누적거래량        ");
	m_ctrlOutBlock.InsertItem(nRow++, "누적거래대금      ");
	m_ctrlOutBlock.InsertItem(nRow++, "매도누적체결량    ");
	m_ctrlOutBlock.InsertItem(nRow++, "매도누적체결건수  ");
	m_ctrlOutBlock.InsertItem(nRow++, "매수누적체결량    ");
	m_ctrlOutBlock.InsertItem(nRow++, "매수누적체결건수  ");
	m_ctrlOutBlock.InsertItem(nRow++, "체결강도          ");
	m_ctrlOutBlock.InsertItem(nRow++, "매도호가1         ");
	m_ctrlOutBlock.InsertItem(nRow++, "매수호가1         ");
	m_ctrlOutBlock.InsertItem(nRow++, "미결제약정수량    ");
	m_ctrlOutBlock.InsertItem(nRow++, "KOSPI200지수      ");
	m_ctrlOutBlock.InsertItem(nRow++, "이론가            ");
	m_ctrlOutBlock.InsertItem(nRow++, "괴리율            ");
	m_ctrlOutBlock.InsertItem(nRow++, "시장BASIS         ");
	m_ctrlOutBlock.InsertItem(nRow++, "이론BASIS         ");
	m_ctrlOutBlock.InsertItem(nRow++, "미결제약정증감    ");
	m_ctrlOutBlock.InsertItem(nRow++, "장운영정보        ");
	m_ctrlOutBlock.InsertItem(nRow++, "전일동시간대거래량");
	m_ctrlOutBlock.InsertItem(nRow++, "단축코드          ");

#endif
}