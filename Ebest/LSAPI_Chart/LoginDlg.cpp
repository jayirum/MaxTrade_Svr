// LoginDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LSAPIApp.h"
#include "LoginDlg.h"
#include "CGlobals.h"
#include "MainFrm.h"



// CLoginDlg ��ȭ �����Դϴ�.

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
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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


// CLoginDlg �޽��� ó�����Դϴ�.

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
// �α��� ó���� �մϴ�
//----------------------------------------------------------------------------------------------------
BOOL CLoginDlg::Login()
{
	// ���� ������ �մϴ�
    g_bConnect = g_iXingAPI.Connect(m_hWnd, 
                                    g_zSvrIp,
                                    atoi(g_zSvrPort),
                                    WM_USER);

    if (!g_bConnect)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        
        __common.log_fmt(ERR, FALSE, "Failed to connect(%s). Server IP(%s), Server Port(%s)",(LPCSTR)strMsg, g_zSvrIp, g_zSvrPort);

		MessageBox(strMsg, "�������ӽ���", MB_ICONSTOP);

        
        
		return FALSE;
    }

	// �α����� ��û�մϴ�
	// -> �α��� �������δ� �α��� �޽����� �����Ͽ� Ȯ���մϴ�
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

		MessageBox(strMsg, "�α��� ����", MB_ICONSTOP);

        return FALSE;
    }

    return TRUE;
}

//----------------------------------------------------------------------------------------------------
// �α��� �������� �޽��� ó���� �մϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CLoginDlg::OnLogin( WPARAM wParam, LPARAM lParam )
{
    LPCSTR pszCode = (LPCSTR)wParam;
    LPCSTR pszMsg  = (LPCSTR)lParam;

    CString strMsg;
    strMsg.Format("[%s] %s", pszCode, pszMsg);
    if (atoi(pszCode) == 0)
    {
        // �α��� ����
        g_bLogin = TRUE;
        OnOK();
        __common.log_fmt(INFO, "[CLoginDlg::OnLogin]Login OK(%s)", (LPCSTR)strMsg);
    }
    else
    {
        // �α��� ����
        g_bLogin = FALSE;
        __common.log_fmt(ERR, "[CLoginDlg::OnLogin] Login Error(%s)", (LPCSTR)strMsg);
		AfxMessageBox(strMsg);
        //OnCancel();
    }

    return 1L;
}
