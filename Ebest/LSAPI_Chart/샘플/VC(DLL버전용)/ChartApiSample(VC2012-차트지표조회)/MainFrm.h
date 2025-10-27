// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "Helper.h"
#include "LoginDlg.h"
#include "IndexBar.h"


class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 특성입니다.
public:

// 작업입니다.
public:

private:
    CIndexBar		m_wndIndexBar;		// 지표표시바

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCStatusBar	m_wndStatusBar;
	CMFCToolBar		m_wndToolBar;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
    afx_msg void OnUpdateMenuEnable(CCmdUI *pCmdUI);
    afx_msg void OnLogin();
    afx_msg LRESULT OnOpenScreen(WPARAM wParam , LPARAM lParam);
};


