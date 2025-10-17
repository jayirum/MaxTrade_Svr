// ChartAPISample.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "MainFrm.h"
#include "ChartFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChartAPISampleApp

BEGIN_MESSAGE_MAP(CChartAPISampleApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CChartAPISampleApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CChartAPISampleApp::OnFileNew)
    ON_COMMAND(ID_CHARTLIB, &CChartAPISampleApp::OnChartlib)
END_MESSAGE_MAP()


// CChartAPISampleApp ����

CChartAPISampleApp::CChartAPISampleApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CChartAPISampleApp ��ü�Դϴ�.

CChartAPISampleApp theApp;


// CChartAPISampleApp �ʱ�ȭ

BOOL CChartAPISampleApp::InitInstance()
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
    if ( dlg.DoModal() == IDCANCEL )
    {
        pFrame->DestroyWindow();
        return TRUE;
    }

	// �α����� �����ϸ� ������ ǥ���մϴ�.
	pFrame->ShowWindow(SW_SHOW);
	//------------------------------------------------------------

	return TRUE;
}

// CChartAPISampleApp �޽��� ó����

int CChartAPISampleApp::ExitInstance() 
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

void CChartAPISampleApp::OnFileNew() 
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
void CChartAPISampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CChartAPISampleApp �޽��� ó����

//----------------------------------------------------------------------------------------------------
// ��Ʈ ȭ���� �����մϴ�
//----------------------------------------------------------------------------------------------------
void CChartAPISampleApp::OnChartlib()
{
    CChartFrame * pChild = new CChartFrame();
    pChild->Create( NULL, "��Ʈ���̺귯��", WS_CHILD | /*WS_VISIBLE |*/ WS_OVERLAPPEDWINDOW, CRect( 0, 0, 0, 0 ) );
    pChild->SetIndexName( 0, NULL );
	pChild->SetWindowPos( NULL, 0, 0, 1000, 700, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
}

