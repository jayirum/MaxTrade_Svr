#pragma once

int                 GetModuleDir        ( LPTSTR            pszPath             ,
                                          int               nLength             );

enum
{
    DATA_TYPE_STRING = 0,		// 문자열
    DATA_TYPE_LONG,				// 정수
    DATA_TYPE_FLOAT,			// 실수
    DATA_TYPE_FLOAT_DOT,		// 실수( 소숫점을 가지고 있음 )
};


typedef struct _TEXTVALUE
{
    char *  pszText ;
    int     nValue  ;
}TEXTVALUE;


#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT

void                SetPacketData       ( char            * pszData             ,
                                          int               nSize               ,
                                          LPCTSTR           pszSrc              ,
                                          int               nType               ,
                                          int               nDotPos = 0         );

void                InitMarketTypeCombo ( CComboBox       * pCombo              );
int                 GetMarketType       ( CComboBox       * pCombo              );
void                InitPeriodCombo     ( CComboBox       * pCombo              );
int                 GetPeriodType       ( CComboBox       * pCombo              );
void                SetDate             ( CDateTimeCtrl   * pDateTime           ,
                                          LPCTSTR           pszDate             );
CString             GetDate             ( CDateTimeCtrl   * pDateTime           );
float               ConvertToFloat      ( LPCTSTR           pszSrc              ,
                                          int               nLength             );
int                 ConvertToInt        ( LPCTSTR           pszSrc              ,
                                          int               nLength             );

#define SetString( t, s )               ( t.SetString( s, sizeof( s ) ) )

enum tagCHART_PERIOD{
	CHART_PER_TICK		= 0,		// 틱
	CHART_PER_MIN		= 1,		// 분
	CHART_PER_DAILY		= 2,		// 일
	CHART_PER_WEEKLY	= 3,		// 주
	CHART_PER_MONTHLY	= 4,		// 월
};

enum tagCHART_MARKET_TYPE{
    MK_STOCK			= 1,		// 주식
    MK_IND				= 2,		// 업종
    MK_FO				= 5,		// 선물옵션
};
