// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// ChartAPISample.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"





char g_zSvrIp[128];
char g_zSvrPort[128];

CString g_strUserID;
CString g_strUserPwd;
CString g_strCertPwd;

HMENU   g_hMenu;
HACCEL  g_hAccel;

IXingAPI        g_iXingAPI          ;
BOOL            g_bConnect          = FALSE;
BOOL            g_bLogin            = FALSE;