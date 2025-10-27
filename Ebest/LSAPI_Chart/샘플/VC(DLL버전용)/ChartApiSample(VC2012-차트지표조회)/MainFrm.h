// MainFrm.h : CMainFrame Ŭ������ �������̽�
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

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

private:
    CIndexBar		m_wndIndexBar;		// ��ǥǥ�ù�

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCStatusBar	m_wndStatusBar;
	CMFCToolBar		m_wndToolBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
    afx_msg void OnUpdateMenuEnable(CCmdUI *pCmdUI);
    afx_msg void OnLogin();
    afx_msg LRESULT OnOpenScreen(WPARAM wParam , LPARAM lParam);
};


