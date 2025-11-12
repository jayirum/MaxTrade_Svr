#if !defined(AFX_DLG_T1305_H__F4C5A65D_9764_4D75_90E0_0AAC152F25B0__INCLUDED_)
#define AFX_DLG_T1305_H__F4C5A65D_9764_4D75_90E0_0AAC152F25B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_t1305.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1305 dialog

class CDlg_t1305 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_t1305 )
// Construction
public:
	CDlg_t1305(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_t1305)
	enum { IDD = IDD_t1305 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_t1305)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				InitCtrls();
	void				RequestData( BOOL bNext=FALSE );
	
	// Generated message map functions
	//{{AFX_MSG(CDlg_t1305)
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

#endif // !defined(AFX_DLG_T1305_H__F4C5A65D_9764_4D75_90E0_0AAC152F25B0__INCLUDED_)
