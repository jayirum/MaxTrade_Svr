#include "StdAfx.h"
#include "Helper.h"

static TEXTVALUE MarketType[] = 
{
    { "�ֽ�", MK_STOCK },
    { "����", MK_IND },
    { "����", MK_FO },
    { NULL, NULL },
};


static TEXTVALUE PeriodType[] =
{
    { "��", CHART_PER_DAILY },
    { "��", CHART_PER_WEEKLY },
    { "��", CHART_PER_MONTHLY },
    { "��", CHART_PER_MIN },
    { "ƽ", CHART_PER_TICK },
    { NULL, NULL },
};


int GetModuleDir( LPTSTR pszPath, int nLength )
{
    TCHAR   szFilePath[ MAX_PATH ];
    TCHAR   szDrive[ _MAX_DRIVE ];
    TCHAR   szDir[ _MAX_DIR ];
    TCHAR   szFileName[ _MAX_FNAME ];
    TCHAR   szExtension[ _MAX_EXT ];
    int     len;

    GetModuleFileName( NULL, szFilePath, MAX_PATH );

    _tsplitpath( szFilePath, szDrive, szDir, szFileName, szExtension );

    memset( szFilePath, 0x00, MAX_PATH );
    lstrcat( szFilePath, szDrive );
    lstrcat( szFilePath, szDir );

    len = lstrlen( szFilePath ) + 1;

    if ( !pszPath )
        return len;

    if ( len <= nLength )
        lstrcpy( pszPath, szFilePath );
    else
        *pszPath = 0x00;

    return len;
}

void SetPacketData( char* pszData, int nSize, LPCTSTR pszSrc, 
                   int nType, int nDotPos )
{
    //-----------------------------------------------------------------------
    // ���ڿ�
    if( nType == DATA_TYPE_STRING )
    {
        // ���� ����
        // ���� ���ڸ��� ' ' �� ä��

        // ����ũ�Ⱑ �� �����ͺ��� �۴ٸ� �����ʹ� ©���� �ϹǷ� �����߻�
        int nSrcLen = strlen( pszSrc );

        // ���� Space�� ä���
        FillMemory( pszData, nSize, ' ' );

        // �տ����� �����͸� �ִ´�.
        // �������Ͱ� ũ�ٸ� �޺κ��� ������.
        CopyMemory( pszData, pszSrc, min( nSize, nSrcLen ) );
    }

    //-----------------------------------------------------------------------
    // ����
    else if( nType == DATA_TYPE_LONG )
    {
        // ������ ����
        // ���� ���ڸ��� '0' ���� ä��

        // ����ũ�Ⱑ �� �����ͺ��� �۴ٸ� �����ʹ� ©���� �ϹǷ� �����߻�
        int nSrcLen = strlen( pszSrc );
        ASSERT( nSize >= nSrcLen );

        // ���� 0 ���� ä���
        FillMemory( pszData, nSize, '0' );

        // �ڿ������� �����͸� �ִ´�.
        if( nSize >= nSrcLen )
        {
            CopyMemory( pszData+nSize-nSrcLen, pszSrc, nSrcLen );
        }
        // �������Ͱ� ũ�ٸ� ���������� �޺κ��� ������.
        else
        {
            CopyMemory( pszData, pszSrc, nSize );
        }
    }

    //-----------------------------------------------------------------------
    // �Ǽ� : �Ҽ����� ���� �ʴ´�.
    else if( nType == DATA_TYPE_FLOAT )
    {
        // �Ҽ��� ��ġ�� �������� ����
        // �Ҽ����� ���� ������ �������� ���ڸ��� �Ҽ����� ���ڸ��� 0���� ä��

        int nSrcLen = strlen( pszSrc );

        // ���� 0 ���� ä���
        FillMemory( pszData, nSize, '0' );

        // �������Ϳ��� �Ҽ����� ��ġ�� ã�Ƽ�
        // ���������� �������� ���̿� �Ҽ����� ���̸� ���Ѵ�.
        int nSrcIntLen;
        int nSrcDotLen;
        LPCTSTR psz = _tcschr( pszSrc, '.' );
        if( psz == NULL )		// �Ҽ��ΰ� ����.
        {
            nSrcIntLen = strlen( pszSrc );
            nSrcDotLen = 0;
        }
        else					// �Ҽ��ΰ� �ִ�.
        {
            nSrcIntLen = psz - pszSrc;
            nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
        }

        // �����θ� �ִ´�.
        if( nSize-nDotPos >= nSrcIntLen )
        {
            CopyMemory( pszData+nSize-nDotPos-nSrcIntLen, pszSrc, nSrcIntLen );
        }
        else
        {
            // ���������� ������ ���̰� �� �� ��� �������� ���ڸ��� �����ȴ�.
            ASSERT( FALSE );
            CopyMemory( pszData, pszSrc, nSize-nDotPos );
        }

        // �Ҽ��θ� �ִµ� ���������� �Ҽ��� ���̰� �� �� ��� �Ҽ����� ���ڸ��� �����ȴ�.
        ASSERT( nDotPos >= nSrcDotLen );
        CopyMemory( pszData+nSize-nDotPos, pszSrc + strlen( pszSrc ) - nSrcDotLen, min( nDotPos, nSrcDotLen ) );
    }

    //-----------------------------------------------------------------------
    // �Ǽ� : �Ҽ����� ����
    else if( nType == DATA_TYPE_FLOAT_DOT )
    {
        // �Ҽ��� ��ġ�� �������� ����
        // �Ҽ����� ���� ������ �������� ���ڸ��� �Ҽ����� ���ڸ��� 0���� ä��

        int nSrcLen = strlen( pszSrc );

        // ���� 0 ���� ä���
        FillMemory( pszData, nSize, '0' );

        // �������Ϳ��� �Ҽ����� ��ġ�� ã�Ƽ�
        // ���������� �������� ���̿� �Ҽ����� ���̸� ���Ѵ�.
        int nSrcIntLen;
        int nSrcDotLen;
        LPCTSTR psz = _tcschr( pszSrc, '.' );
        if( psz == NULL )		// �Ҽ��ΰ� ����.
        {
            nSrcIntLen = strlen( pszSrc );
            nSrcDotLen = 0;
        }
        else					// �Ҽ��ΰ� �ִ�.
        {
            nSrcIntLen = psz - pszSrc;
            nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
        }

        // �����θ� �ִ´�.
        if( nSize-nDotPos-1 >= nSrcIntLen )
        {
            CopyMemory( pszData+nSize-nDotPos-nSrcIntLen-1, pszSrc, nSrcIntLen );
        }
        else
        {
            // ���������� ������ ���̰� �� �� ��� �������� ���ڸ��� �����ȴ�.
            ASSERT( FALSE );
            CopyMemory( pszData, pszSrc, nSize-nDotPos-1 );
        }

        // �Ҽ����� ��´�.
        pszData[nSize-nDotPos-1] = '.';

        // �Ҽ��θ� �ִµ� ���������� �Ҽ��� ���̰� �� �� ��� �Ҽ����� ���ڸ��� �����ȴ�.
        ASSERT( nDotPos >= nSrcDotLen );
        CopyMemory( pszData+nSize-nDotPos, pszSrc + 
            strlen( pszSrc ) - nSrcDotLen, min( nDotPos, nSrcDotLen ) );
    }
}




void InitMarketTypeCombo( CComboBox * pCombo )
{
    pCombo->ResetContent();

    for ( int i=0; MarketType[ i ].pszText != NULL; i++ )
        pCombo->AddString( MarketType[ i ].pszText );

    pCombo->SetCurSel( 0 );

}

int GetMarketType( CComboBox * pCombo )
{
    int index = pCombo->GetCurSel();

    return MarketType[ index ].nValue;
}

void InitPeriodCombo( CComboBox * pCombo )
{
    pCombo->ResetContent();

    for ( int i=0; PeriodType[ i ].pszText != NULL; i++ )
        pCombo->AddString( PeriodType[ i ].pszText );

	// ƽ
    pCombo->SetCurSel( 0 );

}

int GetPeriodType( CComboBox * pCombo )
{
    int index = pCombo->GetCurSel();

    return PeriodType[ index ].nValue;
}

void SetDate( CDateTimeCtrl * pDateTime, LPCTSTR pszDate )
{
    CString strDate;
    SYSTEMTIME si;

    memset( &si, 0, sizeof( si ) );

    if ( pszDate == NULL )
    {
        GetLocalTime( &si );
    }
    else 
    {
        strDate = pszDate;

        si.wYear    = atoi( strDate.Left( 4 ) );
        si.wMonth   = atoi( strDate.Mid( 4, 2 ) );
        si.wDay     = atoi( strDate.Mid( 6, 2 ) );
    }

    pDateTime->SetTime( &si );
}

CString GetDate( CDateTimeCtrl * pDatetTime )
{
    CString strDate;
    SYSTEMTIME si;

    memset( &si, 0, sizeof( si ) );
    pDatetTime->GetTime( &si );

    strDate.Format( "%04d%02d%02d", si.wYear, si.wMonth, si.wDay );

    return strDate;
}

int ConvertToInt( LPCTSTR pszSrc , int nLength )
{
    char buff[ 100 ] = { 0, };
    memcpy( buff, pszSrc, nLength );
    return atoi( buff );
}

float ConvertToFloat( LPCTSTR pszSrc , int nLength )
{
    char buff[ 100 ] = { 0, };
    memcpy( buff, pszSrc, nLength );
    return atof( buff );
}
