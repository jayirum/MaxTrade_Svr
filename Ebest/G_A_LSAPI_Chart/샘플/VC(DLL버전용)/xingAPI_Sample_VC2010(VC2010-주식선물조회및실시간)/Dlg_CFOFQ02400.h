#if !defined(AFX_DLG_CFOFQ02400_H__21EF3152_CABF_4D09_B7E2_D1D7B7167081__INCLUDED_)
#define AFX_DLG_CFOFQ02400_H__21EF3152_CABF_4D09_B7E2_D1D7B7167081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CFOFQ02400.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOFQ02400 dialog

class CDlg_CFOFQ02400 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CFOFQ02400 )
		// Construction
public:
	CDlg_CFOFQ02400(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlg_CFOFQ02400)
	enum { IDD = IDD_CFOFQ02400 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CFOFQ02400)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	char				m_szContKey[30];
	
	CListCtrl			m_ctrlOutBlock1;
	CListCtrl			m_ctrlOutBlock2;
	CListCtrl			m_ctrlOutBlock3;
	CListCtrl			m_ctrlOutBlock4;

	void				InitCtrls();
	void				RequestData( BOOL bNext=FALSE );
	
	// Generated message map functions
	//{{AFX_MSG(CDlg_CFOFQ02400)
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

#endif // !defined(AFX_DLG_CFOFQ02400_H__21EF3152_CABF_4D09_B7E2_D1D7B7167081__INCLUDED_)
