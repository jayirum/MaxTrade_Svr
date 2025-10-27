#if !defined(AFX_DLG_CSPAQ02200_H__FFCBF869_EF4B_4446_9D62_8344FBC172E5__INCLUDED_)
#define AFX_DLG_CSPAQ02200_H__FFCBF869_EF4B_4446_9D62_8344FBC172E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CSPAQ02200.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ02200 dialog

class CDlg_CSPAQ02200 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CSPAQ02200 )
// Construction
public:
	CDlg_CSPAQ02200(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_CSPAQ02200)
	enum { IDD = IDD_CSPAQ02200 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CSPAQ02200)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl			m_ctrlOutBlock1;
	CListCtrl			m_ctrlOutBlock2;
	void				InitCtrls();
	void				RequestData( BOOL bNext=FALSE );

	// Generated message map functions
	//{{AFX_MSG(CDlg_CSPAQ02200)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CSPAQ02200_H__FFCBF869_EF4B_4446_9D62_8344FBC172E5__INCLUDED_)
