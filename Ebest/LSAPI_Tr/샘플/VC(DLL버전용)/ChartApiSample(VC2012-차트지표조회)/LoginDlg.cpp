// LoginDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "LoginDlg.h"


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
END_MESSAGE_MAP()


BOOL CLoginDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_edtID.SetWindowText("");
    m_edtPwd.SetWindowText("");
    m_edtCert.SetWindowText("");

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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

    m_edtID.GetWindowText(g_strUserID);
    m_edtPwd.GetWindowText(g_strUserPwd);
    m_edtCert.GetWindowText(g_strCertPwd);

    Login();
}

//----------------------------------------------------------------------------------------------------
// �α��� ó���� �մϴ�
//----------------------------------------------------------------------------------------------------
BOOL CLoginDlg::Login()
{
	// ���� ������ �մϴ�
    g_bConnect = g_iXingAPI.Connect(m_hWnd, 
                                    SERVER_IP,
                                    SERVER_PORT, 
                                    WM_USER);

    if (!g_bConnect)
    {
        int nErrorCode = g_iXingAPI.GetLastError();
        CString strMsg = g_iXingAPI.GetErrorMessage(nErrorCode);
        TRACE("�������ӽ���\n");
		MessageBox(strMsg, "�������ӽ���", MB_ICONSTOP);
        
		return FALSE;
    }

	// �α����� ��û�մϴ�
	// -> �α��� �������δ� �α��� �޽����� �����Ͽ� Ȯ���մϴ�
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
        TRACE("�α��� ����\n");
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
		AfxMessageBox(strMsg);			
        OnOK();
    }
    else
    {
        // �α��� ����
        g_bLogin = FALSE;
		AfxMessageBox(strMsg);
        //OnCancel();
    }

    return 1L;
}
