#if !defined(AFX_DLG_T1866_H__011EDD60_0251_451C_ACE5_FE21B12283B7__INCLUDED_)
#define AFX_DLG_T1866_H__011EDD60_0251_451C_ACE5_FE21B12283B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_t1866.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1866 dialog

class CDlg_t1866 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_t1866 )
// Construction
public:
	CDlg_t1866();   // standard constructor

	CStringArray m_arrPushKey;

// Dialog Data
	//{{AFX_DATA(CDlg_t1866)
	enum { IDD = IDD_t1866 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_t1866)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl			m_ctrlOutBlock;
	CListCtrl			m_ctrlOutBlock1;
	void				InitCtrls();
	void				RequestData( BOOL bNext=FALSE );

	// Generated message map functions
	//{{AFX_MSG(CDlg_t1866)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg void		OnButtonRequest1	();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_T1866_H__011EDD60_0251_451C_ACE5_FE21B12283B7__INCLUDED_)
