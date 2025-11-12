#if !defined(AFX_DLG_CFOAQ00600_H__BC4B5811_51C9_4FD8_B363_2C386F4ED160__INCLUDED_)
#define AFX_DLG_CFOAQ00600_H__BC4B5811_51C9_4FD8_B363_2C386F4ED160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CFOAQ00600.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOAQ00600 dialog

class CDlg_CFOAQ00600 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CFOAQ00600 )
		// Construction
public:
	CDlg_CFOAQ00600(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlg_CFOAQ00600)
	enum { IDD = IDD_CFOAQ00600 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CFOAQ00600)
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
	//{{AFX_MSG(CDlg_CFOAQ00600)
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

#endif // !defined(AFX_DLG_CFOAQ00600_H__BC4B5811_51C9_4FD8_B363_2C386F4ED160__INCLUDED_)
