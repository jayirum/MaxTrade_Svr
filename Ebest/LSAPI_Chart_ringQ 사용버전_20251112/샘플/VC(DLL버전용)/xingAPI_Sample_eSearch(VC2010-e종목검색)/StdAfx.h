// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__03BDAA55_6CFE_4FB1_8FCC_807F84D7A3A1__INCLUDED_)
#define AFX_STDAFX_H__03BDAA55_6CFE_4FB1_8FCC_807F84D7A3A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "IXingAPI.h"
extern IXingAPI		g_iXingAPI;
extern	BOOL		g_bShowRaw;

#define WMU_SHOW_TR		( WM_USER + 1000 )
#define WMU_SET_MESSAGE	( WM_USER + 1001 )

enum
{
	DATA_TYPE_STRING = 0,		// 문자열
	DATA_TYPE_LONG,				// 정수
	DATA_TYPE_FLOAT,			// 실수
	DATA_TYPE_FLOAT_DOT,		// 실수( 소숫점을 가지고 있음 )
};
#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT

typedef struct
{
	char			szMenu[3][50];
	char			szTrCode[ 11];
	CRuntimeClass*	pRuntimeClass;
	UINT			uDlgID;
	char			szDesc	[101];
} TR_WINDOW_TABLE, *LPTR_WINDOW_TABLE;
int					GetTRWindowTableCount();
LPTR_WINDOW_TABLE	GetTRWindowTableData();

CString GetDispData			( char* psData, int nSize, int nType, int nDotPos=0 );
void	SetPacketData		( char* psData, int nSize, LPCTSTR pszSrc, int nType, int nDotPos=0 );
void	SetPacketData		( char* psData, int nSize, CWnd* pWnd, int nID, int nType, int nDotPos=0 );
void	InsertColumn4List	( CWnd* pDlg, UINT nID, int nCol, LPCTSTR pszTitle, BOOL bString, double fLen );
CString GetDlgItemText		( CWnd* pWnd, UINT nID );
void	SetCtrlAccountList	( CComboBox* pCtrl, LPCTSTR pszGoods );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__03BDAA55_6CFE_4FB1_8FCC_807F84D7A3A1__INCLUDED_)
