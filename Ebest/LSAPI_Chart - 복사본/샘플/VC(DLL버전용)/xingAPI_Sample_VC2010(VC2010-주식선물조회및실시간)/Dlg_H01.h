#if !defined(AFX_DLG_H01_H__12BEBC5F_2DAC_4C34_8D68_DE069E41E508__INCLUDED_)
#define AFX_DLG_H01_H__12BEBC5F_2DAC_4C34_8D68_DE069E41E508__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_H01.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_H01 dialog

class CDlg_H01 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_H01 )

// Construction
public:
	CDlg_H01(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_H01)
	enum { IDD = IDD_H01 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_H01)
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
	//{{AFX_MSG(CDlg_H01)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnDestroy();
	afx_msg	LRESULT		OnXMReceiveRealData	( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H01_H__12BEBC5F_2DAC_4C34_8D68_DE069E41E508__INCLUDED_)
