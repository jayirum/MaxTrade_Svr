// LoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "LoginDlg.h"


// CLoginDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_edtID);
    DDX_Control(pDX, IDC_EDIT2, m_edtPwd);
    DDX_Control(pDX, IDC_EDIT3, m_edtCert);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
    ON_BN_CLICKED(IDC_CANCEL,	&CLoginDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_LOGIN,	&CLoginDlg::OnBnClickedLogin)
    ON_MESSAGE(WM_USER+XM_LOGIN, OnLogin)
END_MESSAGE_MAP()


BOOL CLoginDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_edtID.SetWindowText("");
    m_edtPwd.SetWindowText("");
    m_edtCert.SetWindowText("");

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        if (pMsg->hwnd == m_edtCert)
        {
            OnBnClickedLogin();
            return CWnd::PreTranslateMessage(pMsg);
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}


// CLoginDlg 메시지 처리기입니다.

void CLoginDlg::OnBnClickedCancel()
{
    OnCancel();
}


void CLoginDlg::OnBnClickedLogin()
{
	UpdateData();

    m_edtID.GetWindowText(g_strUserID);
    m_edtPwd.GetWindowText(g_strUserPwd);
    m_edtCert.GetWindowText(g_strCertPwd);

    Login();
}

//----------------------------------------------------------------------------------------------------
// 로그인 처리를 합니다
//----------------------------------------------------------------------------------------------------
BOOL CLoginDlg::Login()
{
	// 서버 접속을 합니다
    g_bConnect = g_iXingAPI.Connect(m_hWnd, 
                                    SERVER_IP,
                                    SERVER_PORT, 
                                    WM_USER);

    if (!g_bConnect)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        TRACE("서버접속실패\n");
		MessageBox(strMsg, "서버접속실패", MB_ICONSTOP);
        
		return FALSE;
    }

	// 로그인을 요청합니다
	// -> 로그인 성공여부는 로그인 메시지를 수신하여 확인합니다
    g_bLogin = FALSE;
    BOOL bRet = g_iXingAPI.Login(m_hWnd, 
                                  g_strUserID,
                                  g_strUserPwd,
                                  g_strCertPwd,
                                  0, 
                                  FALSE);

    if (!bRet)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        TRACE("로그인 실패\n");
		MessageBox(strMsg, "로그인 실패", MB_ICONSTOP);

        return FALSE;
    }

    return TRUE;
}

//----------------------------------------------------------------------------------------------------
// 로그인 성공여부 메시지 처리를 합니다
//----------------------------------------------------------------------------------------------------
LRESULT CLoginDlg::OnLogin( WPARAM wParam, LPARAM lParam )
{
    LPCSTR pszCode = (LPCSTR)wParam;
    LPCSTR pszMsg  = (LPCSTR)lParam;

    CString strMsg;
    strMsg.Format("[%s] %s", pszCode, pszMsg);
    if (atoi(pszCode) == 0)
    {
        // 로그인 성공
        g_bLogin = TRUE;
		AfxMessageBox(strMsg);			
        OnOK();
    }
    else
    {
        // 로그인 실패
        g_bLogin = FALSE;
		AfxMessageBox(strMsg);
        //OnCancel();
    }

    return 1L;
}
