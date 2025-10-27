// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN							// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>								// MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>								// MFC Ȯ���Դϴ�.
#include <afxdisp.h>							// MFC �ڵ�ȭ Ŭ�����Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>							// Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>								// Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>						// MFC�� ���� �� ��Ʈ�� ���� ����

//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif

extern HMENU        g_hMenu;
extern HACCEL       g_hAccel;

//----------------------------------------------------------------------------------------------------
// ��Ʈ ��ǥ������ �̿� ���� ����
//----------------------------------------------------------------------------------------------------
#include "ChartIndex.h"					// ��Ʈ ��ǥ������ ��ȸ�� TR�� ���
#include "ChartExcel.h"					// ��Ʈ ���������� ��ȸ�� TR�� ���
#include "Helper.h"						// ������ ǥ�ÿ�	
#include "IXingAPI.h"					// xingAPI �������̽�

extern CString      g_strUserID;		// ���̵�
extern CString      g_strUserPwd;		// ��й�ȣ
extern CString      g_strCertPwd;		// �������� ��й�ȣ

extern IXingAPI     g_iXingAPI;			// xingAPI
extern BOOL         g_bConnect;			// ���� ����
extern BOOL         g_bLogin;			// �α��� ����

//------------------------------------------------------------
// ���� �ּ� �� ��Ʈ
#define SERVER_IP                       "hts.etrade.co.kr"
#define SERVER_PORT                     20001

//------------------------------------------------------------
// ����� ���� �޽���
#define WM_SETMESSAGE                   WM_USER + 1
#define WM_OPENSCREEN                   WM_USER + 2
#define WMU_SET_MESSAGE					WM_USER + 3

//------------------------------------------------------------
// ��ǥ�� 
#define IDS_INDEX01                     "���� �̵����"
#define IDS_INDEX02                     "�̰ݵ�"
#define IDS_INDEX03                     "Average True Range"  // Averange True Range
#define IDS_INDEX04                     "MACD"
#define IDS_INDEX05                     "Momentum"
#define IDS_INDEX06                     "RSI"
#define IDS_INDEX07                     "Price ROC"
#define IDS_INDEX08                     "Sonar Momentum"
#define IDS_INDEX09                     "OBV"
#define IDS_INDEX10                     "Volume ROC"

//------------------------------------------------------------
// ��ǥ������ ǥ�ÿ� 
typedef struct _COLUMNDATA 
{
    int     nWidth;
    int     nFormat;
    char *  pText;
}COLUMNDATA;


