#pragma once

int                 GetModuleDir        ( LPTSTR            pszPath             ,
                                          int               nLength             );

enum
{
    DATA_TYPE_STRING = 0,		// ���ڿ�
    DATA_TYPE_LONG,				// ����
    DATA_TYPE_FLOAT,			// �Ǽ�
    DATA_TYPE_FLOAT_DOT,		// �Ǽ�( �Ҽ����� ������ ���� )
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

//JAY void                InitMarketTypeCombo ( CComboBox       * pCombo              );
//JAY int                 GetMarketType       ( CComboBox       * pCombo              );
//JAY void                InitPeriodCombo     ( CComboBox       * pCombo              );
//JAY int                 GetPeriodType       ( CComboBox       * pCombo              );
//JAY void                SetDate             ( CDateTimeCtrl   * pDateTime           ,
//JAY                                           LPCTSTR           pszDate             );
//JAY CString             GetDate             ( CDateTimeCtrl   * pDateTime           );
float               ConvertToFloat      ( LPCTSTR           pszSrc              ,
                                          int               nLength             );
int                 ConvertToInt        ( LPCTSTR           pszSrc              ,
                                          int               nLength             );

#define SetString( t, s )               ( t.SetString( s, sizeof( s ) ) )

//enum tagCHART_PERIOD{
//	CHART_PER_TICK		= 0,		// ƽ
//	CHART_PER_MIN		= 1,		// ��
//	CHART_PER_DAILY		= 2,		// ��
//	CHART_PER_WEEKLY	= 3,		// ��
//	CHART_PER_MONTHLY	= 4,		// ��
//};
//
//enum tagCHART_MARKET_TYPE{
//    MK_STOCK			= 1,		// �ֽ�
//    MK_IND				= 2,		// ����
//    MK_FO				= 5,		// �����ɼ�
//};
