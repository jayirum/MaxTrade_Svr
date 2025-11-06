// XingAPI_Sample.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XingAPI_Sample.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "DlgLogin.h"
#include "Comm.h"
#include "../../Common/Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSendPriceSvr	g_SendPrcSvr;
extern CLogMsg			g_log;
extern TCHAR			g_zConfigDir[_MAX_PATH];
extern TCHAR			g_zLogDir[_MAX_PATH];
extern char				g_zConfigFileName[MAX_PATH];
extern vector<string>			g_vecStk;
extern map<string, CCircularQ*>	g_mapQ;


/////////////////////////////////////////////////////////////////////////////
// CXingAPI_SampleApp

BEGIN_MESSAGE_MAP(CXingAPI_SampleApp, CWinApp)
	//{{AFX_MSG_MAP(CXingAPI_SampleApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXingAPI_SampleApp construction

CXingAPI_SampleApp::CXingAPI_SampleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXingAPI_SampleApp object

CXingAPI_SampleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXingAPI_SampleApp initialization

BOOL CXingAPI_SampleApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need. 

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_XINGAPTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_XINGAPTYPE));



	// The main window has been initialized, so show and update it.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();
	

	if (!GetDir_Cnfg_Log())
	{
		MessageBox(NULL, "CONFIG 파일명 가져오기 오류", "오류", MB_ICONSTOP);
		pFrame->DestroyWindow();
		return FALSE;
	}

	if (!g_log.OpenLog(g_zLogDir, EXENAME))
	{
		return FALSE;
	}

	// 
	if (!Read_StkCode())
		return FALSE;

	//
	Create_CircularQ();


	if (!Initialize_SendDataSvr())
		return FALSE;

	//-----------------------------------------------------------------------------------------
	// Login 표시
	CDlgLogin dlg( pFrame );
	if( dlg.DoModal() == IDCANCEL )
	{
		pFrame->DestroyWindow();
		return TRUE;
	}
	//-----------------------------------------------------------------------------------------

	// 시세, 호가창 실행
#ifdef DEF_KF
	AfxGetMainWnd()->SendMessage(WMU_SHOW_TR, (WPARAM)(LPCTSTR)"FC0", NULL);
#elif DEF_GF
	AfxGetMainWnd()->SendMessage(WMU_SHOW_TR, (WPARAM)(LPCTSTR)"OVC ", NULL);
#endif
	return TRUE;
}

BOOL CXingAPI_SampleApp::Create_CircularQ()
{
	char zMsg[1024];
	for (auto it = g_vecStk.begin(); it != g_vecStk.end(); it++)
	{
		CCircularQ* q = new CCircularQ();
		q->W_Create(Q_MINSIZE, zMsg);

		g_mapQ[(*it)] = q;
	}
	return TRUE;
}

BOOL CXingAPI_SampleApp::Read_StkCode()
{
	CODBC odbc(DBMS_TYPE::MSSQL);
	if (!odbc.Initialize())
	{
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}

	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };
	if (!ReadConfig("DB", "DSN", zDsn) ||
		!ReadConfig("DB", "UID", zUid) ||
		!ReadConfig("DB", "PWD", zPwd)
		)
	{
		LOGGING(LOGTP_ERR, TRUE, "DB정보 조회 오류.INI파일을 확인하세요");
		return FALSE;
	}
	
	char connStr[512];
	sprintf(connStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);
	if (!odbc.Connect(connStr))
	{
		LOGGING(LOGTP_ERR, TRUE, "DB Connect 오류:%s", odbc.getMsg());
		return FALSE;
	}
	

	char zQ[1024];
#ifdef DEF_KF
	sprintf(zQ, "SELECT STK_CD FROM STK_MST WHERE STK_USE_YN='Y' AND ACNT_TP='1'");
#else
	sprintf(zQ, "SELECT STK_CD FROM STK_MST WHERE STK_USE_YN='Y' AND ACNT_TP='2'");
#endif
	odbc.Init_ExecQry(zQ);
	bool bNeeded=false;
	if (!odbc.Exec_Qry(bNeeded)) {
		LOGGING(LOGTP_ERR, TRUE, odbc.getMsg());
		return FALSE;
	}

	char szRs[1024] = { 0, };
	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zStkCd[32] = { 0 };
		odbc.GetDataStr(1, sizeof(zStkCd), zStkCd);
		LOGGING(LOGTP_SUCC, TRUE, "STK_CD:%s", zStkCd);

		g_vecStk.push_back(zStkCd);
	}

	odbc.DeInit_ExecQry();
	return TRUE;
}

BOOL CXingAPI_SampleApp::Initialize_SendDataSvr()
{
	if (!g_SendPrcSvr.Initialize())
		return FALSE;

	return TRUE;
}

BOOL CXingAPI_SampleApp::GetDir_Cnfg_Log()
{
	//	GET Config DIR
	GetCurrentDirectory(_MAX_PATH, g_zConfigDir);
	CUtil::GetCnfgFileNm(g_zConfigDir, EXENAME, g_zConfigFileName);
	sprintf(g_zLogDir, "%s\\Log", g_zConfigDir);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CXingAPI_SampleApp message handlers

int CXingAPI_SampleApp::ExitInstance() 
{
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

void CXingAPI_SampleApp::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	// create a new MDI child window
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_XINGAPTYPE, m_hMDIMenu, m_hMDIAccel);
}



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CXingAPI_SampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CXingAPI_SampleApp message handlers

