#pragma once


// CChartFrame �������Դϴ�.

class CChartFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChartFrame)

public:
    void            SetIndexName(int iInexType, LPCTSTR pszName);	// ChartFrame�� IndexView�� ǥ��

private:
    CView          *m_pwndView;		// IndexView �� 

protected:
public:
	CChartFrame();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CChartFrame();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


