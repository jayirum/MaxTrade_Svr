
// Sample_xingAceRdSender.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CSample_xingAceRdSenderApp:
// �� Ŭ������ ������ ���ؼ��� Sample_xingAceRdSender.cpp�� �����Ͻʽÿ�.
//

class CSample_xingAceRdSenderApp : public CWinApp
{
public:
	CSample_xingAceRdSenderApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CSample_xingAceRdSenderApp theApp;