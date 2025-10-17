#pragma once
#include "afxwin.h"

#define TIMER_LOGIN_DLG 1001

// CLoginDlg ��ȭ �����Դϴ�.

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)


private:
    BOOL	Login();

public:
	CLoginDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoginDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedLogin();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    LRESULT OnLogin( WPARAM wParam, LPARAM lParam );

	CEdit m_edtID;
    CEdit m_edtPwd;
    CEdit m_edtCert;
};
