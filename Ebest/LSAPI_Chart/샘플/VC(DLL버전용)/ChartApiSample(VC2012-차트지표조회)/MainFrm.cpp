// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/�Ҹ�

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
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndToolBar);

	//------------------------------------------------------------
	// ��ǥǥ�ùٸ� �����մϴ�
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | 
                    WS_CLIPSIBLINGS | CBRS_LEFT | CBRS_FLOAT_MULTI;

    if (!m_wndIndexBar.Create("��ǥ", this, CRect( 0, 0, 200, 200 ), TRUE, 1, dwStyle))
    {
        return -1;
    }

    m_wndIndexBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndIndexBar);

	//------------------------------------------------------------
	// xingAPI�� �ʱ�ȭ�մϴ�
    TCHAR szPath[ MAX_PATH ] = { 0, };
    GetModuleDir(szPath, sizeof(szPath));
    if (!g_iXingAPI.Init(szPath))
    {
        TRACE("XingAPI DLL�� �ε��� �� �����ϴ�.");
        return -1;
    }

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}


// CMainFrame ����

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


// CMainFrame �޽��� ó����

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
// �α��� ���̾�α� ǥ�� 
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
// ��ǥǥ��â���� ��ǥ Ŭ�� ��, ��ǥ ȭ���� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CMainFrame::OnOpenScreen(WPARAM wParam , LPARAM lParam)
{
    LPCTSTR pszText = (LPCTSTR)lParam;

    if (!g_bLogin)
    {
        AfxMessageBox( "�α����Ͻñ� �ٶ��ϴ�." );
        return 1;
    }

    CChartFrame * pChild = new CChartFrame();
    pChild->Create( NULL, pszText, WS_CHILD | /*WS_VISIBLE |*/ WS_OVERLAPPEDWINDOW, CRect( 0, 0, 0, 0 ) );
    pChild->SetIndexName( wParam, pszText );
    pChild->SetWindowPos( NULL, 0, 0, 1000, 700, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );

    return 1;
}