// ChartAPISample.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "LSAPIApp.h"
#include "MainFrm.h"
#include "ChartFrame.h"
#include "CGlobals.h"
#include "CDBWorks.h"
#include "CSymbolsets.h"
#include <set>
#include "CIOCPServer.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CLSAPIApp

BEGIN_MESSAGE_MAP(CLSAPIApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CLSAPIApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CLSAPIApp::OnFileNew)
    ON_COMMAND(ID_CHARTLIB, &CLSAPIApp::OnChartlib)
END_MESSAGE_MAP()


// CLSAPIApp ����

CLSAPIApp::CLSAPIApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}
CLSAPIApp::~CLSAPIApp()
{
	__iocpSvr.Stop();
	__iocpSvr.Join();
}

// ������ CLSAPIApp ��ü�Դϴ�.

CLSAPIApp theApp;


// CLSAPIApp �ʱ�ȭ

BOOL CLSAPIApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));


	if (!__common.Initialize()) {
		AfxMessageBox("__common.Initialize() error");
		return FALSE;
	}

	__common.logStart("[%s][%s] Start....", EXENAME, EXE_VERSION);

	if(!__common.read_config_all() ) {
		AfxMessageBox("__common.read_config_all() error");
		return FALSE;
	}

	//IOCP ���� ����
	if (!__iocpSvr.Start(__common.app_listen_ip(), __common.app_listen_port()))
		return FALSE;
	__common.log_fmt(INFO, "IOCP Server started.(Port:%d)", __common.app_listen_port());

	if (!connect_db())
		return FALSE;
	__common.log_fmt(INFO, "DB Connect OK(DNS Name:%s)", __common.get_dsn());

	if (!load_timeframes_symbols())
		return FALSE;


	// �� â�� ����� ���� �� �ڵ忡���� �� ������ â ��ü��
	// ���� ���� �̸� ���� ���α׷��� �� â ��ü�� �����մϴ�.
	CMDIFrameWndEx* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	// �� MDI ������ â�� ����ϴ�.
	m_pMainWnd = pFrame;
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// ���� MDI �޴� �� �׼��������� Ű ���̺��� �ε带 �õ��մϴ�.
	//TODO: �߰� ��� ������ �߰��ϰ� ���� ���α׷��� �ʿ��� �߰� �޴� ���Ŀ� ����
	//	ȣ���� �ε��մϴ�.
	HINSTANCE hInst = AfxGetResourceHandle();
    m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
    m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));

    g_hMenu = m_hMDIMenu;
    g_hAccel= m_hMDIAccel;

	//------------------------------------------------------------
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	// �α��� ���ο� ���� ������ ǥ���ϱ� ���� �ϴ��� ����ϴ�.
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();

	// �α��� ���̾�α׸� ǥ���մϴ�.
    CLoginDlg dlg( pFrame );
    if ( dlg.DoModal() == IDCANCEL || g_bLogin == FALSE )
    {
		__common.log(ERR, "XingAPI �α��� ���з� ���μ��� ����");
        pFrame->DestroyWindow();
        return TRUE;
    }

	// �α����� �����ϸ� ������ ǥ���մϴ�.
	pFrame->ShowWindow(SW_SHOW);

	pFrame->PostMessage(WM_OPENSCREEN);
	//------------------------------------------------------------

	return TRUE;
}

bool CLSAPIApp::connect_db()
{
	return __dbworks.connect();
}

bool CLSAPIApp::load_timeframes_symbols()
{
	if (!__dbworks.is_connected())
		return false;
	
	std::set<int> tfs = __dbworks.load_timeframes();
	if (tfs.empty()) {
		__common.log(ERR, "There is no timeframe in DB");
		return false;
	}

	std::set<std::string> symbols = __dbworks.load_symbols();
	if (symbols.empty()) {
		__common.log(ERR, "There is no symbols in DB");
		return false;
	}

	for (const int& tf : tfs) 
	{
		for (const string& symbol : symbols)
		{
			__SymbolSets.emplace_back(make_shared<CSymbolSets>(tf,symbol));
			__common.log_fmt(INFO, "SymbolSet ���� - (timeframe:%d)(symbol:%s)", tf, symbol.c_str());
		}		
	}


	return true;
}


// CLSAPIApp �޽��� ó����

int CLSAPIApp::ExitInstance() 
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

void CLSAPIApp::OnFileNew() 
{
    OnChartlib();
}


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CLSAPIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CLSAPIApp �޽��� ó����

//----------------------------------------------------------------------------------------------------
// ��Ʈ ȭ���� �����մϴ�
//----------------------------------------------------------------------------------------------------
void CLSAPIApp::OnChartlib()
{
    CChartFrame * pChild = new CChartFrame();
    pChild->Create( NULL, "��Ʈ���̺귯��", WS_CHILD | /*WS_VISIBLE |*/ WS_OVERLAPPEDWINDOW, CRect( 0, 0, 0, 0 ) );
    pChild->SetIndexName( 0, NULL );
	pChild->SetWindowPos( NULL, 0, 0, 1000, 700, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
}

