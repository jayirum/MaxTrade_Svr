// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN							// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>								// MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>								// MFC 확장입니다.
#include <afxdisp.h>							// MFC 자동화 클래스입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>							// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>								// Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>						// MFC의 리본 및 컨트롤 막대 지원

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
// 차트 지표데이터 이용 관련 정의
//----------------------------------------------------------------------------------------------------
#include "ChartIndex.h"					// 차트 지표데이터 조회용 TR의 헤더
#include "ChartExcel.h"					// 차트 엑셀데이터 조회용 TR의 헤더
#include "Helper.h"						// 데이터 표시용	
#include "IXingAPI.h"					// xingAPI 인터페이스

extern CString      g_strUserID;		// 아이디
extern CString      g_strUserPwd;		// 비밀번호
extern CString      g_strCertPwd;		// 공인인증 비밀번호

extern IXingAPI     g_iXingAPI;			// xingAPI
extern BOOL         g_bConnect;			// 접속 여부
extern BOOL         g_bLogin;			// 로그인 여부

//------------------------------------------------------------
// 접속 주소 및 포트
#define SERVER_IP                       "hts.etrade.co.kr"
#define SERVER_PORT                     20001

//------------------------------------------------------------
// 사용자 정의 메시지
#define WM_SETMESSAGE                   WM_USER + 1
#define WM_OPENSCREEN                   WM_USER + 2
#define WMU_SET_MESSAGE					WM_USER + 3

//------------------------------------------------------------
// 지표명 
#define IDS_INDEX01                     "가격 이동평균"
#define IDS_INDEX02                     "이격도"
#define IDS_INDEX03                     "Average True Range"  // Averange True Range
#define IDS_INDEX04                     "MACD"
#define IDS_INDEX05                     "Momentum"
#define IDS_INDEX06                     "RSI"
#define IDS_INDEX07                     "Price ROC"
#define IDS_INDEX08                     "Sonar Momentum"
#define IDS_INDEX09                     "OBV"
#define IDS_INDEX10                     "Volume ROC"

//------------------------------------------------------------
// 지표데이터 표시용 
typedef struct _COLUMNDATA 
{
    int     nWidth;
    int     nFormat;
    char *  pText;
}COLUMNDATA;


