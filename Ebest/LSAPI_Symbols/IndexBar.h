#pragma once


// CIndexBar

class CIndexBar : public CDockablePane
{
	DECLARE_DYNAMIC(CIndexBar)

private:
    CTreeCtrl	m_treIndex;
    void		InitTreeCtrl();

public:
	CIndexBar();
	virtual ~CIndexBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDblClkTree( NMHDR * pNMHDR, LRESULT * pResult );
};


