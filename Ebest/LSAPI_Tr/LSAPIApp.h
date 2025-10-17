// ChartAPISample.h : ChartAPISample ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "CDBConnector.h"

// CLSAPIApp:
// �� Ŭ������ ������ ���ؼ��� ChartAPISample.cpp�� �����Ͻʽÿ�.
//

class CLSAPIApp : public CWinApp
{
public:
	CLSAPIApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	bool	connect_db();
	bool	load_symbols_timeframes();
	//bool	init_dbsave();

// �����Դϴ�.
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

	CDBConnector* m_dbConnector;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
    afx_msg void OnChartlib();
};

extern CLSAPIApp theApp;