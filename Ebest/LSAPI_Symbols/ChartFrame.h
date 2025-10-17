#pragma once


// CChartFrame 프레임입니다.

class CChartFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChartFrame)

public:
    void            SetIndexName(int iInexType, LPCTSTR pszName);	// ChartFrame에 IndexView를 표시

private:
    CView          *m_pwndView;		// IndexView 용 

protected:
public:
	CChartFrame();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CChartFrame();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


