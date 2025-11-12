#if !defined(AFX_DLG_CFOAT00300_H__4DE94A76_22EC_4FE2_B92F_45024267068B__INCLUDED_)
#define AFX_DLG_CFOAT00300_H__4DE94A76_22EC_4FE2_B92F_45024267068B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CFOAT00300.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOAT00300 dialog

class CDlg_CFOAT00300 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CFOAT00300 )
		
		// Construction
public:
	CDlg_CFOAT00300(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlg_CFOAT00300)
	enum { IDD = IDD_CFOAT00300 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CFOAT00300)
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
	//{{AFX_MSG(CDlg_CFOAT00300)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CFOAT00300_H__4DE94A76_22EC_4FE2_B92F_45024267068B__INCLUDED_)
