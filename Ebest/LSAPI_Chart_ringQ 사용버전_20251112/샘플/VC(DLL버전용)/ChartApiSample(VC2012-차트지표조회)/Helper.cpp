#include "StdAfx.h"
#include "Helper.h"

static TEXTVALUE MarketType[] = 
{
    { "주식", MK_STOCK },
    { "업종", MK_IND },
    { "선옵", MK_FO },
    { NULL, NULL },
};


static TEXTVALUE PeriodType[] =
{
    { "일", CHART_PER_DAILY },
    { "주", CHART_PER_WEEKLY },
    { "월", CHART_PER_MONTHLY },
    { "분", CHART_PER_MIN },
    { "틱", CHART_PER_TICK },
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
    // 문자열
    if( nType == DATA_TYPE_STRING )
    {
        // 왼쪽 정렬
        // 뒤의 빈자리는 ' ' 로 채움

        // 버퍼크기가 원 데이터보다 작다면 데이터는 짤려야 하므로 에러발생
        int nSrcLen = strlen( pszSrc );

        // 먼저 Space를 채우고
        FillMemory( pszData, nSize, ' ' );

        // 앞에부터 데이터를 넣는다.
        // 원데이터가 크다면 뒷부분을 버린다.
        CopyMemory( pszData, pszSrc, min( nSize, nSrcLen ) );
    }

    //-----------------------------------------------------------------------
    // 정수
    else if( nType == DATA_TYPE_LONG )
    {
        // 오른쪽 정렬
        // 앞의 빈자리는 '0' 으로 채움

        // 버퍼크기가 원 데이터보다 작다면 데이터는 짤려야 하므로 에러발생
        int nSrcLen = strlen( pszSrc );
        ASSERT( nSize >= nSrcLen );

        // 먼저 0 으로 채우고
        FillMemory( pszData, nSize, '0' );

        // 뒤에서부터 데이터를 넣는다.
        if( nSize >= nSrcLen )
        {
            CopyMemory( pszData+nSize-nSrcLen, pszSrc, nSrcLen );
        }
        // 원데이터가 크다면 원데이터의 뒷부분을 버린다.
        else
        {
            CopyMemory( pszData, pszSrc, nSize );
        }
    }

    //-----------------------------------------------------------------------
    // 실수 : 소숫점을 찍지 않는다.
    else if( nType == DATA_TYPE_FLOAT )
    {
        // 소숫점 위치를 기준으로 정렬
        // 소숫점을 찍지 않으며 정수부의 빈자리와 소수부의 빈자리는 0으로 채움

        int nSrcLen = strlen( pszSrc );

        // 먼저 0 으로 채우고
        FillMemory( pszData, nSize, '0' );

        // 원데이터에서 소숫점의 위치를 찾아서
        // 원데이터의 정수부의 길이와 소수부의 길이를 구한다.
        int nSrcIntLen;
        int nSrcDotLen;
        LPCTSTR psz = _tcschr( pszSrc, '.' );
        if( psz == NULL )		// 소수부가 없다.
        {
            nSrcIntLen = strlen( pszSrc );
            nSrcDotLen = 0;
        }
        else					// 소수부가 있다.
        {
            nSrcIntLen = psz - pszSrc;
            nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
        }

        // 정수부를 넣는다.
        if( nSize-nDotPos >= nSrcIntLen )
        {
            CopyMemory( pszData+nSize-nDotPos-nSrcIntLen, pszSrc, nSrcIntLen );
        }
        else
        {
            // 원데이터의 정수부 길이가 더 긴 경우 정수부의 뒷자리는 삭제된다.
            ASSERT( FALSE );
            CopyMemory( pszData, pszSrc, nSize-nDotPos );
        }

        // 소수부를 넣는데 원데이터의 소수부 길이가 더 긴 경우 소수부의 뒷자리는 삭제된다.
        ASSERT( nDotPos >= nSrcDotLen );
        CopyMemory( pszData+nSize-nDotPos, pszSrc + strlen( pszSrc ) - nSrcDotLen, min( nDotPos, nSrcDotLen ) );
    }

    //-----------------------------------------------------------------------
    // 실수 : 소숫점을 포함
    else if( nType == DATA_TYPE_FLOAT_DOT )
    {
        // 소숫점 위치를 기준으로 정렬
        // 소숫점을 찍지 않으며 정수부의 빈자리와 소수부의 빈자리는 0으로 채움

        int nSrcLen = strlen( pszSrc );

        // 먼저 0 으로 채우고
        FillMemory( pszData, nSize, '0' );

        // 원데이터에서 소숫점의 위치를 찾아서
        // 원데이터의 정수부의 길이와 소수부의 길이를 구한다.
        int nSrcIntLen;
        int nSrcDotLen;
        LPCTSTR psz = _tcschr( pszSrc, '.' );
        if( psz == NULL )		// 소수부가 없다.
        {
            nSrcIntLen = strlen( pszSrc );
            nSrcDotLen = 0;
        }
        else					// 소수부가 있다.
        {
            nSrcIntLen = psz - pszSrc;
            nSrcDotLen = strlen( pszSrc ) - nSrcIntLen - 1;
        }

        // 정수부를 넣는다.
        if( nSize-nDotPos-1 >= nSrcIntLen )
        {
            CopyMemory( pszData+nSize-nDotPos-nSrcIntLen-1, pszSrc, nSrcIntLen );
        }
        else
        {
            // 원데이터의 정수부 길이가 더 긴 경우 정수부의 뒷자리는 삭제된다.
            ASSERT( FALSE );
            CopyMemory( pszData, pszSrc, nSize-nDotPos-1 );
        }

        // 소숫점을 찍는다.
        pszData[nSize-nDotPos-1] = '.';

        // 소수부를 넣는데 원데이터의 소수부 길이가 더 긴 경우 소수부의 뒷자리는 삭제된다.
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

	// 틱
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
