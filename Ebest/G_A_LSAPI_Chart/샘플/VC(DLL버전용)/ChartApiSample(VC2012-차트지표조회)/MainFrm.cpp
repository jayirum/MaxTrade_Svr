// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "MainFrm.h"
#include "ChartFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_UPDATE_COMMAND_UI(ID_LOGIN,		&CMainFrame::OnUpdateMenuEnable)
    ON_UPDATE_COMMAND_UI(ID_FILE_NEW,	&CMainFrame::OnUpdateMenuEnable)
    ON_UPDATE_COMMAND_UI(ID_CHARTINDEX, &CMainFrame::OnUpdateMenuEnable)
    ON_COMMAND(ID_LOGIN,				&CMainFrame::OnLogin)
    ON_MESSAGE(WM_OPENSCREEN,			&CMainFrame::OnOpenScreen)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndToolBar);

	//------------------------------------------------------------
	// 지표표시바를 생성합니다
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | 
                    WS_CLIPSIBLINGS | CBRS_LEFT | CBRS_FLOAT_MULTI;

    if (!m_wndIndexBar.Create("지표", this, CRect( 0, 0, 200, 200 ), TRUE, 1, dwStyle))
    {
        return -1;
    }

    m_wndIndexBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndIndexBar);

	//------------------------------------------------------------
	// xingAPI를 초기화합니다
    TCHAR szPath[ MAX_PATH ] = { 0, };
    GetModuleDir(szPath, sizeof(szPath));
    if (!g_iXingAPI.Init(szPath))
    {
        TRACE("XingAPI DLL을 로드할 수 없습니다.");
        return -1;
    }

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnClose()
{
	CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnUpdateMenuEnable(CCmdUI *pCmdUI)
{
    BOOL bEnable = FALSE;

    if (pCmdUI->m_nID == ID_LOGIN)
    {
        if (!g_bLogin)    
            bEnable = TRUE;
    }
    else
    {
        if (g_bLogin)    
            bEnable = TRUE;
    }

    pCmdUI->Enable(bEnable);
}

//----------------------------------------------------------------------------------------------------
// 로그인 다이얼로그 표시 
//----------------------------------------------------------------------------------------------------
void CMainFrame::OnLogin()
{
    CLoginDlg dlg(this);

    if (dlg.DoModal() != IDCANCEL)
    {
        g_bLogin = TRUE;
    }
}

//----------------------------------------------------------------------------------------------------
// 지표표시창에서 지표 클릭 시, 지표 화면을 표시합니다
//----------------------------------------------------------------------------------------------------
LRESULT CMainFrame::OnOpenScreen(WPARAM wParam , LPARAM lParam)
{
    LPCTSTR pszText = (LPCTSTR)lParam;

    if (!g_bLogin)
    {
        AfxMessageBox( "로그인하시기 바랍니다." );
        return 1;
    }

    CChartFrame * pChild = new CChartFrame();
    pChild->Create( NULL, pszText, WS_CHILD | /*WS_VISIBLE |*/ WS_OVERLAPPEDWINDOW, CRect( 0, 0, 0, 0 ) );
    pChild->SetIndexName( wParam, pszText );
    pChild->SetWindowPos( NULL, 0, 0, 1000, 700, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );

    return 1;
}