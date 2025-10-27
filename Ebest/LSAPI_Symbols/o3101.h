#ifndef _o3101_H_
#define _o3101_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// 해외선물 종목마스터 조회 ( ATTR,BLOCK,HEADTYPE=A )
#pragma pack( push, 1 )

#define NAME_o3101     "o3101"

// 기본입력                       
typedef struct _o3101InBlock
{
    char    gubun               [   1];    char    _gubun               ;    // [string,    1] 입력구분(예비)                  StartPos 0, Length 1
} o3101InBlock, *LPo3101InBlock;
#define NAME_o3101InBlock     "o3101InBlock"

// 출력-마스터                    , occurs
typedef struct _o3101OutBlock
{
    char    Symbol              [   8];    char    _Symbol              ;    // [string,    8] 종목코드                        StartPos 0, Length 8
    char    SymbolNm            [  50];    char    _SymbolNm            ;    // [string,   50] 종목명                          StartPos 9, Length 50
    char    ApplDate            [   8];    char    _ApplDate            ;    // [string,    8] 종목배치수신일(한국일자)        StartPos 60, Length 8
    char    BscGdsCd            [  10];    char    _BscGdsCd            ;    // [string,   10] 기초상품코드                    StartPos 69, Length 10
    char    BscGdsNm            [  40];    char    _BscGdsNm            ;    // [string,   40] 기초상품명                      StartPos 80, Length 40
    char    ExchCd              [  10];    char    _ExchCd              ;    // [string,   10] 거래소코드                      StartPos 121, Length 10
    char    ExchNm              [  40];    char    _ExchNm              ;    // [string,   40] 거래소명                        StartPos 132, Length 40
    char    CrncyCd             [   3];    char    _CrncyCd             ;    // [string,    3] 기준통화코드                    StartPos 173, Length 3
    char    NotaCd              [   3];    char    _NotaCd              ;    // [string,    3] 진법구분코드                    StartPos 177, Length 3
    char    UntPrc              [  15];    char    _UntPrc              ;    // [double, 15.9] 호가단위가격                    StartPos 181, Length 15
    char    MnChgAmt            [  15];    char    _MnChgAmt            ;    // [double, 15.9] 최소가격변동금액                StartPos 197, Length 15
    char    RgltFctr            [  15];    char    _RgltFctr            ;    // [double,15.10] 가격조정계수                    StartPos 213, Length 15
    char    CtrtPrAmt           [  15];    char    _CtrtPrAmt           ;    // [double, 15.2] 계약당금액                      StartPos 229, Length 15
    char    GdsCd               [   3];    char    _GdsCd               ;    // [string,    3] 상품구분코드                    StartPos 245, Length 3
    char    LstngYr             [   4];    char    _LstngYr             ;    // [string,    4] 월물(년)                        StartPos 249, Length 4
    char    LstngM              [   1];    char    _LstngM              ;    // [string,    1] 월물(월)                        StartPos 254, Length 1
    char    EcPrc               [  15];    char    _EcPrc               ;    // [double, 15.9] 정산가격                        StartPos 256, Length 15
    char    DlStrtTm            [   6];    char    _DlStrtTm            ;    // [string,    6] 거래시작시간                    StartPos 272, Length 6
    char    DlEndTm             [   6];    char    _DlEndTm             ;    // [string,    6] 거래종료시간                    StartPos 279, Length 6
    char    DlPsblCd            [   1];    char    _DlPsblCd            ;    // [string,    1] 거래가능구분코드                StartPos 286, Length 1
    char    MgnCltCd            [   1];    char    _MgnCltCd            ;    // [string,    1] 증거금징수구분코드              StartPos 288, Length 1
    char    OpngMgn             [  15];    char    _OpngMgn             ;    // [double, 15.2] 개시증거금                      StartPos 290, Length 15
    char    MntncMgn            [  15];    char    _MntncMgn            ;    // [double, 15.2] 유지증거금                      StartPos 306, Length 15
    char    OpngMgnR            [   7];    char    _OpngMgnR            ;    // [double,  7.3] 개시증거금율                    StartPos 322, Length 7
    char    MntncMgnR           [   7];    char    _MntncMgnR           ;    // [double,  7.3] 유지증거금율                    StartPos 330, Length 7
    char    DotGb               [   2];    char    _DotGb               ;    // [long  ,    2] 유효소수점자리수                StartPos 338, Length 2
} o3101OutBlock, *LPo3101OutBlock;
#define NAME_o3101OutBlock     "o3101OutBlock"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _o3101_H_
