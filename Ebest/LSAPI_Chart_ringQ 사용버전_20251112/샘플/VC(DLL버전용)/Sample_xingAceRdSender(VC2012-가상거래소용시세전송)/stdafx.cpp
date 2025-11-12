
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Sample_xingAceRdSender.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


CString GetToken( CString& strData, char cSep )
{
	CString strResult;

	strData.Trim();
	int nFind = strData.Find( cSep );
	if( nFind < 0 )
	{
		strResult = strData;
		strData   = "";
	}
	else
	{
		strResult = strData.Left( nFind );
		strData   = strData.Mid( nFind+1 );
		strResult.TrimRight();
		strData.TrimLeft();
	}
	return strResult;
}

int GetSelectedItem( CListCtrl& ctrl )
{
	POSITION pos = ctrl.GetFirstSelectedItemPosition();
	return ( pos != NULL ) ? ctrl.GetNextSelectedItem( pos ) : -1;
}

void SelectItem( CListCtrl& ctrl, int nSelect, int nUnselect )
{
	// 현재 선택되어진 Row를 먼저 선택이 안된 상태로 바꾼후에
	if( nUnselect == -1 )
	{
		nUnselect = GetSelectedItem( ctrl );
	}
	if( nUnselect >= 0 )
	{
		ctrl.SetItemState( nUnselect, LVIS_SELECTED | LVIS_FOCUSED, 0 );
	}

	// 원하는 Row를 선택이 된 상태로 바꾼다.
	ctrl.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
}

CString MakeBlockData( CString strType, CString strLen, CString strData, BOOL bAttr, BOOL bHideDot )
{
	CString strResult;

	// 정수형
	if( strType.CompareNoCase( "int" ) == 0 || strType.CompareNoCase( "long" ) == 0 )
	{
		strResult.Format( "%0*d", atoi( strLen ), atoi( strData ) );
	}
	// 실수형
	else if( strType.CompareNoCase( "float" ) == 0 || strType.CompareNoCase( "double" ) == 0 )
	{
		int nInt = atoi( GetToken( strLen, '.' ) );
		int nDec = atoi( strLen );
		CString strFormat;
		if( bHideDot )
		{
			strFormat.Format( "%%0%d.%df", nInt+1, nDec);
		}
		else
		{
			strFormat.Format( "%%0%d.%df", nInt, nDec);
		}

		strResult.Format( strFormat, atof( strData ) );
		if( bHideDot )
		{
			strResult.Remove( '.' );
		}
	}
	// 문자형
	else
	{
		strResult.Format( "%-*s", atoi( strLen ), strData );
	}

	if( bAttr )
	{
		strResult += " ";
	}

	return strResult;
}