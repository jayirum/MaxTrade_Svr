#if !defined(AFX_DLG_CFOAT00100_H__320B01BC_6300_4A45_85AE_83D9FF2AFDCF__INCLUDED_)
#define AFX_DLG_CFOAT00100_H__320B01BC_6300_4A45_85AE_83D9FF2AFDCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CFOAT00100.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOAT00100 dialog

class CDlg_CFOAT00100 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CFOAT00100 )
		
// Construction
public:
	CDlg_CFOAT00100(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_CFOAT00100)
	enum { IDD = IDD_CFOAT00100 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CFOAT00100)
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
	//{{AFX_MSG(CDlg_CFOAT00100)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CFOAT00100_H__320B01BC_6300_4A45_85AE_83D9FF2AFDCF__INCLUDED_)
