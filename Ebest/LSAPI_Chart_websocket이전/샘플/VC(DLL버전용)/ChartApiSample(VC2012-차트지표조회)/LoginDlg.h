#pragma once
#include "afxwin.h"


// CLoginDlg 대화 상자입니다.

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)


private:
    BOOL	Login();

public:
	CLoginDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedLogin();
    LRESULT OnLogin( WPARAM wParam, LPARAM lParam );

	CEdit m_edtID;
    CEdit m_edtPwd;
    CEdit m_edtCert;
};
