#if !defined(AFX_DLG_CCEAT00300_H__919BF1F2_DBE8_438E_9E66_067AEDDCE5E1__INCLUDED_)
#define AFX_DLG_CCEAT00300_H__919BF1F2_DBE8_438E_9E66_067AEDDCE5E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_CCEAT00300.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_CCEAT00300 dialog

class CDlg_CCEAT00300 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_CCEAT00300 )
		
		// Construction
public:
	CDlg_CCEAT00300(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlg_CCEAT00300)
	enum { IDD = IDD_CCEAT00300 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_CCEAT00300)
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
	//{{AFX_MSG(CDlg_CCEAT00300)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveData		( WPARAM wParam, LPARAM lParam );
	afx_msg	LRESULT		OnXMTimeoutData		( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_CCEAT00300_H__919BF1F2_DBE8_438E_9E66_067AEDDCE5E1__INCLUDED_)
