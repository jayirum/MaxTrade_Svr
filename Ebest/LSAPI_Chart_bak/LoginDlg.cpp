// LoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSAPIApp.h"
#include "LoginDlg.h"
#include "CGlobals.h"
#include "MainFrm.h"



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
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CLoginDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_edtID.SetWindowText((LPCSTR)__common.get_userid());
    m_edtPwd.SetWindowText((LPCSTR)__common.get_userpwd());
    m_edtCert.SetWindowText((LPCSTR)__common.get_certpwd());

    m_edtID.SetReadOnly();
    m_edtPwd.SetReadOnly();
    m_edtCert.SetReadOnly();

    SetTimer(TIMER_LOGIN_DLG, 1, NULL); // 1 second

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLoginDlg::OnTimer(UINT_PTR nIDEvent)
{
    KillTimer(TIMER_LOGIN_DLG);
    OnBnClickedLogin();
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


    //m_edtID.GetWindowText(g_strUserID);
    //m_edtPwd.GetWindowText(g_strUserPwd);
    //m_edtCert.GetWindowText(g_strCertPwd);

    

    Login();
}

//----------------------------------------------------------------------------------------------------
// 로그인 처리를 합니다
//----------------------------------------------------------------------------------------------------
BOOL CLoginDlg::Login()
{
	// 서버 접속을 합니다
    g_bConnect = g_iXingAPI.Connect(m_hWnd, 
                                    g_zSvrIp,
                                    atoi(g_zSvrPort),
                                    WM_USER);

    if (!g_bConnect)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        
        __common.log_fmt(ERR, FALSE, "Failed to connect(%s). Server IP(%s), Server Port(%s)",(LPCSTR)strMsg, g_zSvrIp, g_zSvrPort);

		MessageBox(strMsg, "서버접속실패", MB_ICONSTOP);

        
        
		return FALSE;
    }

	// 로그인을 요청합니다
	// -> 로그인 성공여부는 로그인 메시지를 수신하여 확인합니다
    g_bLogin = FALSE;
    BOOL bRet = g_iXingAPI.Login(
                                m_hWnd, 
                                __common.get_userid(),
                                __common.get_userpwd(),
                                __common.get_certpwd(),
                                0, 
                                TRUE);

    __common.log_fmt(INFO, "Request Login. ID(%s), UserPwd(%s), CertPwd(%s)",
        __common.get_userid(), __common.get_userpwd(),__common.get_certpwd());

    if (!bRet)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        
        __common.log_fmt(ERR, "Failed to Login Request(%s). ID(%s), UserPwd(%s), CertPwd(%s)",
            (LPCSTR)strMsg, __common.get_userid(), __common.get_userpwd(), __common.get_certpwd());

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
        OnOK();
        __common.log_fmt(INFO, "[CLoginDlg::OnLogin]Login OK(%s)", (LPCSTR)strMsg);
    }
    else
    {
        // 로그인 실패
        g_bLogin = FALSE;
        __common.log_fmt(ERR, "[CLoginDlg::OnLogin] Login Error(%s)", (LPCSTR)strMsg);
		AfxMessageBox(strMsg);
        //OnCancel();
    }

    return 1L;
}
