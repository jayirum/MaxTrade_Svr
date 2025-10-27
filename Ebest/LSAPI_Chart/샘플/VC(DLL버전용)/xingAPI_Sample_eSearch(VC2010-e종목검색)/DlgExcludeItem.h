#if !defined(AFX_DLGEXCLUDEITEM_H__20C5F9F6_E023_44AE_8725_82A3CEB86881__INCLUDED_)
#define AFX_DLGEXCLUDEITEM_H__20C5F9F6_E023_44AE_8725_82A3CEB86881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExcludeItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExcludeItem dialog

class CDlgExcludeItem : public CDialog
{
// Construction
public:
	CDlgExcludeItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExcludeItem)
	enum { IDD = IDD_EXCLUDE_ITEM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int		GetData()				{ return m_nData;  }
	void	SetData( int nData )	{ m_nData = nData; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExcludeItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int			m_nData;

	// Generated message map functions
	//{{AFX_MSG(CDlgExcludeItem)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXCLUDEITEM_H__20C5F9F6_E023_44AE_8725_82A3CEB86881__INCLUDED_)
