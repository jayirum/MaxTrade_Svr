// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// ChartAPISample.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"





CString g_strUserID;
CString g_strUserPwd;
CString g_strCertPwd;

HMENU   g_hMenu;
HACCEL  g_hAccel;

IXingAPI        g_iXingAPI          ;
BOOL            g_bConnect          = FALSE;
BOOL            g_bLogin            = FALSE;