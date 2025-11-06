#if !defined(AFX_DLG_C01_H__70E89A9E_0011_4DDB_AE82_9B5AD2BF8091__INCLUDED_)
#define AFX_DLG_C01_H__70E89A9E_0011_4DDB_AE82_9B5AD2BF8091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_C01.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_C01 dialog

class CDlg_C01 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_C01 )

// Construction
public:
	CDlg_C01(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_C01)
	enum { IDD = IDD_C01 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_C01)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListCtrl			m_ctrlOutBlock;
	void				InitCtrls();
	void				AdviseData();
	void				UnadviseData();

	// Generated message map functions
	//{{AFX_MSG(CDlg_C01)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnDestroy();
	afx_msg	LRESULT		OnXMReceiveRealData	( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_C01_H__70E89A9E_0011_4DDB_AE82_9B5AD2BF8091__INCLUDED_)
