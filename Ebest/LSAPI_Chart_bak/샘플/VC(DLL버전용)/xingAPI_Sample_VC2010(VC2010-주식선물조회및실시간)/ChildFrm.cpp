// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "XingAPI_Sample.h"

#include "ChildFrm.h"

// 주식
#include "Dlg_t1101.h"				// 주식현재가호가조회
#include "Dlg_t1301.h"				// 주식시간대별체결조회
#include "Dlg_t1305.h"				// 기간별주가
#include "Dlg_t1452.h"				// 거래량상위
#include "Dlg_t8411.h"				// 주식챠트(틱/n틱)

#include "Dlg_H1_.h"
#include "Dlg_H2_.h"
#include "Dlg_h3_ELW.h"
#include "Dlg_h2_ELW.h"

#include "Dlg_CSPAT00600.h"			// 현물정상주문
#include "Dlg_CSPAT00700.h"			// 현물정정주문
#include "Dlg_CSPAT00800.h"			// 현물취소주문

#include "Dlg_SC0.h"				// 주식주문접수
#include "Dlg_SC1.h"				// 주식주문체결
#include "Dlg_SC2.h"				// 주식주문정정
#include "Dlg_SC3.h"				// 주식주문취소
#include "Dlg_SC4.h"				// 주식주문거부

#include "Dlg_CSPAQ03700.h"			// 현물계좌 주문체결내역 조회
#include "Dlg_CSPAQ02300.h"			// 현물계좌 잔고내역 조회
#include "Dlg_CSPAQ02200.h"			// 현물계좌 예수금/주문가능금액/총평가 조회
#include "Dlg_CSPBQ00200.h"			// 현물계좌 증거금률별 주문가능수량 조회

// 선물옵션
#include "Dlg_FC0.h"				// 선물체결

#include "Dlg_CFOAT00100.h"			// 선물옵션 정상주문
#include "Dlg_CFOAT00200.h"			// 선물옵션 정상주문
#include "Dlg_CFOAT00300.h"			// 선물옵션 취소주문
#include "Dlg_C01.h"				// 선물옵션 주문체결
#include "Dlg_H01.h"				// 선물옵션 정정취소

#include "Dlg_CFOAQ00600.h"			// 선물옵션 계좌주문체결내역 조회
#include "Dlg_CFOFQ02400.h"			// 선물옵션 미결제약정현황

// CME 야간선물
#include "Dlg_CCEAT00100.h"			// CME 야간선물 정상주문
#include "Dlg_CCEAT00200.h"			// CME 야간선물 정상주문
#include "Dlg_CCEAT00300.h"			// CME 야간선물 취소주문

// EUREX 야간옵션
#include "Dlg_CEXAT11100.h"			// EUREX 야간옵션 정상주문
#include "Dlg_CEXAT11200.h"			// EUREX 야간옵션 정상주문
#include "Dlg_CEXAT11300.h"			// EUREX 야간옵션 취소주문

// 주식선물
#include "Dlg_JC0.h"

// 투자자별
#include "Dlg_t1601.h"				// 투자자별종합
#include "Dlg_t1602.h"				// 시간대별 투자자매매추이

#include "Dlg_BMT.h"
#include "Dlg_BM_.h"

// 기타
#include "Dlg_t9942.h"				// API용 ELW 마스터조회
#include "Dlg_t9943.h"				// API용 지수선물 마스터조회

static TR_WINDOW_TABLE s_tblTrWindow[] = 
{
	//---------------------------------------------------------------------------------------------
	// 주식/ELW 시세
	{ "주식/ELW" , "시세", "조회TR",   "t1101"     , RUNTIME_CLASS(CDlg_t1101     ), IDD_t1101     , "주식현재가호가조회(t1101)" },
 	{ "주식/ELW" , "시세", "조회TR",   "t1301"     , RUNTIME_CLASS(CDlg_t1301     ), IDD_t1301     , "주식시간대별체결조회(t1301)" },
 	{ "주식/ELW" , "시세", "조회TR",   "t1305"     , RUNTIME_CLASS(CDlg_t1305     ), IDD_t1305     , "기간별주가(t1305)" },
 	{ "주식/ELW" , "시세", "조회TR",   "t1452"     , RUNTIME_CLASS(CDlg_t1452     ), IDD_t1452     , "거래량상위(t1452)" },
 	{ "주식/ELW" , "시세", "조회TR",   "t8411"     , RUNTIME_CLASS(CDlg_t8411     ), IDD_t8411     , "주식챠트(틱/n틱)(t8411)" },

	{ "주식/ELW" , "시세", "실시간TR", "H1_"       , RUNTIME_CLASS(CDlg_H1_       ), IDD_H1_       , "KOSPI 호가잔량" },
	{ "주식/ELW" , "시세", "실시간TR", "H2_"       , RUNTIME_CLASS(CDlg_H2_       ), IDD_H2_       , "KOSPI 시간외호가잔량" },
	{ "주식/ELW" , "시세", "실시간TR", "h3_"       , RUNTIME_CLASS(CDlg_h3_ELW    ), IDD_h3_ELW    , "ELW 호가잔량" },
	{ "주식/ELW" , "시세", "실시간TR", "h2_"       , RUNTIME_CLASS(CDlg_h2_ELW    ), IDD_h2_ELW    , "ELW 시간외호가잔량" },
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 주식/ELW 주문
	{ "주식/ELW" , "주문", "조회TR",   "CSPAT00600", RUNTIME_CLASS(CDlg_CSPAT00600), IDD_CSPAT00600, "현물정상주문(CSPAT00600)" },
	{ "주식/ELW" , "주문", "조회TR",   "CSPAT00700", RUNTIME_CLASS(CDlg_CSPAT00700), IDD_CSPAT00700, "현물정정주문(CSPAT00700)" },
 	{ "주식/ELW" , "주문", "조회TR",   "CSPAT00800", RUNTIME_CLASS(CDlg_CSPAT00800), IDD_CSPAT00800, "현물취소주문(CSPAT00800)" },
	
	{ "주식/ELW" , "주문", "실시간TR", "SC0"       , RUNTIME_CLASS(CDlg_SC0       ), IDD_SC0       , "주식주문접수" },
	{ "주식/ELW" , "주문", "실시간TR", "SC1"       , RUNTIME_CLASS(CDlg_SC1       ), IDD_SC1       , "주식주문체결" },
	{ "주식/ELW" , "주문", "실시간TR", "SC2"       , RUNTIME_CLASS(CDlg_SC2       ), IDD_SC2       , "주식주문정정" },
	{ "주식/ELW" , "주문", "실시간TR", "SC3"       , RUNTIME_CLASS(CDlg_SC3       ), IDD_SC3       , "주식주문취소" },
	{ "주식/ELW" , "주문", "실시간TR", "SC4"       , RUNTIME_CLASS(CDlg_SC4       ), IDD_SC4       , "주식주문거부" },
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 주식/ELW 계좌
	{ "주식/ELW" , "계좌", "조회TR"  , "CSPAQ13700", RUNTIME_CLASS(CDlg_CSPAQ03700), IDD_CSPAQ03700, "현물계좌 주문체결내역 조회" },
	{ "주식/ELW" , "계좌", "조회TR"  , "CSPAQ12300", RUNTIME_CLASS(CDlg_CSPAQ02300), IDD_CSPAQ02300, "현물계좌 잔고내역 조회" },
	{ "주식/ELW" , "계좌", "조회TR"  , "CSPAQ12200", RUNTIME_CLASS(CDlg_CSPAQ02200), IDD_CSPAQ02200, "현물계좌 예수금/주문가능금액/총평가 조회" },
	{ "주식/ELW" , "계좌", "조회TR"  , "CSPBQ00200", RUNTIME_CLASS(CDlg_CSPBQ00200), IDD_CSPBQ00200, "현물계좌 증거금률별 주문가능수량 조회" },
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 선물/옵션 시세
	{ "선물/옵션", "시세", "실시간TR", "FC0"       , RUNTIME_CLASS(CDlg_FC0       ), IDD_FC0       , "선물체결" },
	
	//---------------------------------------------------------------------------------------------
	// 선물/옵션 주문
	{ "선물/옵션", "주문", "조회TR"  , "CFOAT00100", RUNTIME_CLASS(CDlg_CFOAT00100), IDD_CFOAT00100, "선물옵션 정상주문" },
	{ "선물/옵션", "주문", "조회TR"  , "CFOAT00200", RUNTIME_CLASS(CDlg_CFOAT00200), IDD_CFOAT00200, "선물옵션 정정주문" },
	{ "선물/옵션", "주문", "조회TR"  , "CFOAT00300", RUNTIME_CLASS(CDlg_CFOAT00300), IDD_CFOAT00300, "선물옵션 취소주문" },
	{ "선물/옵션", "주문", "실시간TR", "C01",		 RUNTIME_CLASS(CDlg_C01),		 IDD_C01,	     "선물옵션 주문체결" },
	{ "선물/옵션", "주문", "실시간TR", "H01",		 RUNTIME_CLASS(CDlg_H01),		 IDD_H01,	     "선물옵션 정정취소" },
	//---------------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------------
	// 선물/옵션 계좌
	{ "선물/옵션", "계좌", "조회TR"  , "CFOAQ00600", RUNTIME_CLASS(CDlg_CFOAQ00600), IDD_CFOAQ00600, "선물옵션 계좌주문체결내역 조회" },
	{ "선물/옵션", "계좌", "조회TR"  , "CFOFQ02400", RUNTIME_CLASS(CDlg_CFOFQ02400), IDD_CFOFQ02400, "선물옵션 미결제약정현황" },
	//---------------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------------
	// CME 야간선물 주문
	{ "CME 야간선물", "주문", "조회TR"  , "CCEAT00100", RUNTIME_CLASS(CDlg_CCEAT00100), IDD_CCEAT00100, "CME 야간선물 정상주문" },
	{ "CME 야간선물", "주문", "조회TR"  , "CCEAT00200", RUNTIME_CLASS(CDlg_CCEAT00200), IDD_CCEAT00200, "CME 야간선물 정정주문" },
	{ "CME 야간선물", "주문", "조회TR"  , "CCEAT00300", RUNTIME_CLASS(CDlg_CCEAT00300), IDD_CCEAT00300, "CME 야간선물 취소주문" },
	//---------------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------------
	// EUREX 야간옵션 주문
	{ "EUREX 야간옵션", "주문", "조회TR"  , "CEXAT11100", RUNTIME_CLASS(CDlg_CEXAT11100), IDD_CEXAT11100, "EUREX 야간옵션 정상주문" },
	{ "EUREX 야간옵션", "주문", "조회TR"  , "CEXAT11200", RUNTIME_CLASS(CDlg_CEXAT11200), IDD_CEXAT11200, "EUREX 야간옵션 정정주문" },
	{ "EUREX 야간옵션", "주문", "조회TR"  , "CEXAT11300", RUNTIME_CLASS(CDlg_CEXAT11300), IDD_CEXAT11300, "EUREX 야간옵션 취소주문" },
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 주식선물 시세
	{ "주식선물" , "시세", "실시간TR", "JC0"       , RUNTIME_CLASS(CDlg_JC0       ), IDD_JC0       , "선물체결" },
	//---------------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------------
	// 업종 시세
	//---------------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------------
	// 투자자
	{ "투자자", "", "조회TR", "t1601", RUNTIME_CLASS(CDlg_t1601), IDD_t1601, "투자자별종합" },
	{ "투자자", "", "조회TR", "t1602", RUNTIME_CLASS(CDlg_t1602), IDD_t1602, "시간대별 투자자매매추이" },

	{ "투자자", "", "실시간TR", "BMT"  , RUNTIME_CLASS(CDlg_BMT), IDD_BMT, "시간대별 투자자매매추이" },
	{ "투자자", "", "실시간TR", "BM_"  , RUNTIME_CLASS(CDlg_BM_), IDD_BM_, "투자자별 투자자별 매매현황" },
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 프로그램
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 투자정보
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 차트
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// 기타
	{ "기타", "", "조회TR", "t9942", RUNTIME_CLASS(CDlg_t9942), IDD_t9942, "ELW마스터조회API용(t9942)" },
	{ "기타", "", "조회TR", "t9943", RUNTIME_CLASS(CDlg_t9943), IDD_t9943, "지수선물마스터조회API용(t9943)" },
	//---------------------------------------------------------------------------------------------
};

int GetTRWindowTableCount()
{
	return sizeof( s_tblTrWindow ) / sizeof( TR_WINDOW_TABLE );
}

LPTR_WINDOW_TABLE GetTRWindowTableData()
{
	return s_tblTrWindow;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
		ON_WM_SETFOCUS()
		ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
void CChildFrame::OnFileClose() 
{
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu.

	SendMessage(WM_CLOSE);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	return 0;
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	
	// otherwise, do default handling
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CChildFrame::SetTR( LPCTSTR pszTrCode )
{
	int nCount = sizeof( s_tblTrWindow ) / sizeof( TR_WINDOW_TABLE );
	for( int i=0; i<nCount; i++ )
	{
		if( strcmp( s_tblTrWindow[i].szTrCode, pszTrCode ) == 0 )
		{
			char szTitle[256] = { 0 };
			sprintf_s( szTitle, "[%s] %s", s_tblTrWindow[i].szTrCode, s_tblTrWindow[i].szDesc );
			SetWindowText( szTitle  );
			CDocument* pDocument = GetActiveDocument();
			if( pDocument )
			{
				pDocument->SetTitle( szTitle );
			}
			else
			{
				m_strTitle = szTitle;
			}
			m_wndView.SetTR( s_tblTrWindow[i].pRuntimeClass, s_tblTrWindow[i].uDlgID );
			return TRUE;
		}
	}

	CString strMsg;
	strMsg.Format( "%s 의 Sample을 찾을 수 없습니다.", pszTrCode );
	
	MessageBox( strMsg, "에러", MB_ICONSTOP );

	SendMessage( WM_CLOSE );
	
	return FALSE;
}