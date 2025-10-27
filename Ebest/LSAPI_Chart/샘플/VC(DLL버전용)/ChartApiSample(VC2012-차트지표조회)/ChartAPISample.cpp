// ChartAPISample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CChartAPISampleApp 생성

CChartAPISampleApp::CChartAPISampleApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CChartAPISampleApp 개체입니다.

CChartAPISampleApp theApp;


// CChartAPISampleApp 초기화

BOOL CChartAPISampleApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMDIFrameWndEx* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	// 주 MDI 프레임 창을 만듭니다.
	m_pMainWnd = pFrame;
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// 공유 MDI 메뉴 및 액셀러레이터 키 테이블의 로드를 시도합니다.
	//TODO: 추가 멤버 변수를 추가하고 응용 프로그램에 필요한 추가 메뉴 형식에 대한
	//	호출을 로드합니다.
	HINSTANCE hInst = AfxGetResourceHandle();
    m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
    m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));

    g_hMenu = m_hMDIMenu;
    g_hAccel= m_hMDIAccel;

	//------------------------------------------------------------
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	// 로그인 여부에 따라 메인을 표시하기 위해 일단은 숨김니다.
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();

	// 로그인 다이얼로그를 표시합니다.
    CLoginDlg dlg( pFrame );
    if ( dlg.DoModal() == IDCANCEL )
    {
        pFrame->DestroyWindow();
        return TRUE;
    }

	// 로그인이 성공하면 메인을 표시합니다.
	pFrame->ShowWindow(SW_SHOW);
	//------------------------------------------------------------

	return TRUE;
}

// CChartAPISampleApp 메시지 처리기

int CChartAPISampleApp::ExitInstance() 
{
	//TODO: 추가한 추가 리소스를 처리합니다.
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


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CChartAPISampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CChartAPISampleApp 메시지 처리기

//----------------------------------------------------------------------------------------------------
// 차트 화면을 생성합니다
//----------------------------------------------------------------------------------------------------
void CChartAPISampleApp::OnChartlib()
{
    CChartFrame * pChild = new CChartFrame();
    pChild->Create( NULL, "차트라이브러리", WS_CHILD | /*WS_VISIBLE |*/ WS_OVERLAPPEDWINDOW, CRect( 0, 0, 0, 0 ) );
    pChild->SetIndexName( 0, NULL );
	pChild->SetWindowPos( NULL, 0, 0, 1000, 700, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
}

