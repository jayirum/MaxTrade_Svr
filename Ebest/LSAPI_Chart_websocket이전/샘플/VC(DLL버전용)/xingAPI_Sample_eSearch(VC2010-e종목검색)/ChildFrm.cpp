// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "XingAPI_Sample_eSearch.h"

#include "ChildFrm.h"

#include "Dlg_t1857.h"				// 서버종목검색
#include "Dlg_t1866.h"				// 서버종목검색리스트

static TR_WINDOW_TABLE s_tblTrWindow[] = 
{
	//---------------------------------------------------------------------------------------------
	// 부가서비스
	{ "e종목검색"		, "부가서비스", "조회TR",   "t1857"	, RUNTIME_CLASS(CDlg_t1857     ), IDD_t1857     , "서버종목검색(t1857)" },
	{ "e종목검색"		, "부가서비스", "조회TR",   "t1866"	, RUNTIME_CLASS(CDlg_t1866     ), IDD_t1866     , "서버종목리스트조회(t1866)" },
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