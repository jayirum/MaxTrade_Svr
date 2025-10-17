#if !defined(AFX_DLG_T9941_H__ED6D1752_C79C_44E2_985B_479E24E30AD8__INCLUDED_)
#define AFX_DLG_T9941_H__ED6D1752_C79C_44E2_985B_479E24E30AD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_t9941.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_t9941 dialog

class CDlg_t9941 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_t9941 )
// Construction
public:
	CDlg_t9941(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_t9941)
	enum { IDD = IDD_t9941 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_t9941)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl			m_ctrlOutBlock;
	void				InitCtrls();
	void				RequestData( BOOL bNext );

	// Generated message map functions
	//{{AFX_MSG(CDlg_t9941)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_T9941_H__ED6D1752_C79C_44E2_985B_479E24E30AD8__INCLUDED_)
