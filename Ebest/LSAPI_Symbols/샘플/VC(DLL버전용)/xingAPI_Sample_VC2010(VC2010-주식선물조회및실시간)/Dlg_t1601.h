#if !defined(AFX_DLG_T1601_H__01C9FF81_F979_4625_B440_44F04D360C13__INCLUDED_)
#define AFX_DLG_T1601_H__01C9FF81_F979_4625_B440_44F04D360C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_t1601.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1601 dialog

class CDlg_t1601 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_t1601 )
// Construction
public:
	CDlg_t1601(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_t1601)
	enum { IDD = IDD_t1601 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_t1601)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				InitCtrls();
	void				InitOutBlockCtrl( UINT nID, LPCTSTR lpszTitle );
	void				RequestData();

	// Generated message map functions
	//{{AFX_MSG(CDlg_t1601)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_T1601_H__01C9FF81_F979_4625_B440_44F04D360C13__INCLUDED_)
