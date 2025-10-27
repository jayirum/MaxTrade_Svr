
// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// Sample_xingAceRdSender.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

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
	// ���� ���õǾ��� Row�� ���� ������ �ȵ� ���·� �ٲ��Ŀ�
	if( nUnselect == -1 )
	{
		nUnselect = GetSelectedItem( ctrl );
	}
	if( nUnselect >= 0 )
	{
		ctrl.SetItemState( nUnselect, LVIS_SELECTED | LVIS_FOCUSED, 0 );
	}

	// ���ϴ� Row�� ������ �� ���·� �ٲ۴�.
	ctrl.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
}

CString MakeBlockData( CString strType, CString strLen, CString strData, BOOL bAttr, BOOL bHideDot )
{
	CString strResult;

	// ������
	if( strType.CompareNoCase( "int" ) == 0 || strType.CompareNoCase( "long" ) == 0 )
	{
		strResult.Format( "%0*d", atoi( strLen ), atoi( strData ) );
	}
	// �Ǽ���
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
	// ������
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