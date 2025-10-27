#if !defined(AFX_DLG_CEXAT11100_H__ACBC7006_60B0_46D5_A073_5C86CB10B2D0__INCLUDED_)
#define AFX_DLG_CEXAT11100_H__ACBC7006_60B0_46D5_A073_5C86CB10B2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CEXAT11100.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CEXAT11100 dialog

class CDlg_CEXAT11100 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CEXAT11100 )
		
// Construction
public:
	CDlg_CEXAT11100(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_CEXAT11100)
	enum { IDD = IDD_CEXAT11100 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CEXAT11100)
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
	//{{AFX_MSG(CDlg_CEXAT11100)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CEXAT11100_H__ACBC7006_60B0_46D5_A073_5C86CB10B2D0__INCLUDED_)
