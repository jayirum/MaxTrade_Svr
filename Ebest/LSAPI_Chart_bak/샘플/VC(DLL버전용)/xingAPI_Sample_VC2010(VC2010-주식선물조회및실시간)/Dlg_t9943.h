#if !defined(AFX_DLG_T9943_H__C699C929_FA3A_4514_B6CF_36256E83CC53__INCLUDED_)
#define AFX_DLG_T9943_H__C699C929_FA3A_4514_B6CF_36256E83CC53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_t9943.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_t9943 dialog

class CDlg_t9943 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_t9943 )
// Construction
public:
	CDlg_t9943(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_t9943)
	enum { IDD = IDD_t9943 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_t9943)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl			m_ctrlOutBlock;
	void				InitCtrls();
	void				RequestData( BOOL bNext );

	// Generated message map functions
	//{{AFX_MSG(CDlg_t9943)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_T9943_H__C699C929_FA3A_4514_B6CF_36256E83CC53__INCLUDED_)
