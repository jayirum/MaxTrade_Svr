#if !defined(AFX_DLG_JC0_H__3B322ECB_4258_4137_A4BA_D12B853E446F__INCLUDED_)
#define AFX_DLG_JC0_H__3B322ECB_4258_4137_A4BA_D12B853E446F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_JC0.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_JC0 dialog

class CDlg_JC0 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_JC0 )
// Construction
public:
	CDlg_JC0(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_JC0)
	enum { IDD = IDD_JC0 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_JC0)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString				m_strCode;
	CListCtrl			m_ctrlOutBlock;
	void				InitCtrls();
	void				AdviseData();
	void				UnadviseData();
	
	// Generated message map functions
	//{{AFX_MSG(CDlg_JC0)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void		OnDestroy();
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveRealData	( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_JC0_H__3B322ECB_4258_4137_A4BA_D12B853E446F__INCLUDED_)
