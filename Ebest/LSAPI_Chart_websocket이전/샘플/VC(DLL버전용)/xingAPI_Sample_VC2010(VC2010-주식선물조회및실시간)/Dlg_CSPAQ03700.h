#if !defined(AFX_DLG_CSPAQ03700_H__C8EBA490_172F_4534_84A5_938BEEE02470__INCLUDED_)
#define AFX_DLG_CSPAQ03700_H__C8EBA490_172F_4534_84A5_938BEEE02470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CSPAQ03700.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ03700 dialog

class CDlg_CSPAQ03700 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CSPAQ03700 )
// Construction
public:
	CDlg_CSPAQ03700(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_CSPAQ03700)
	enum { IDD = IDD_CSPAQ03700 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CSPAQ03700)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char				m_szContKey[30];
	
	CListCtrl			m_ctrlOutBlock1;
	CListCtrl			m_ctrlOutBlock2;
	CListCtrl			m_ctrlOutBlock3;
	void				InitCtrls();
	void				RequestData( BOOL bNext=FALSE );

	// Generated message map functions
	//{{AFX_MSG(CDlg_CSPAQ03700)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg void		OnButtonRequestNext	();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CSPAQ03700_H__C8EBA490_172F_4534_84A5_938BEEE02470__INCLUDED_)
