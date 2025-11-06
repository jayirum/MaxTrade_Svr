#if !defined(AFX_DLG_CSPBQ00200_H__B21D6FBF_CF00_46E5_BC97_1D3A10A7E9D7__INCLUDED_)
#define AFX_DLG_CSPBQ00200_H__B21D6FBF_CF00_46E5_BC97_1D3A10A7E9D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CSPBQ00200.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPBQ00200 dialog

class CDlg_CSPBQ00200 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CSPBQ00200 )
// Construction
public:
	CDlg_CSPBQ00200(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_CSPBQ00200)
	enum { IDD = IDD_CSPBQ00200 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CSPBQ00200)
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
	//{{AFX_MSG(CDlg_CSPBQ00200)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CSPBQ00200_H__B21D6FBF_CF00_46E5_BC97_1D3A10A7E9D7__INCLUDED_)
