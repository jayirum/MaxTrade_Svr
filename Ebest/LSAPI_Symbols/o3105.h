#ifndef _o3105_H_
#define _o3105_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// 해외선물 종목정보 ( ATTR,BLOCK,SVR=OVS,HEADTYPE=A )
#pragma pack( push, 1 )

#define NAME_o3105     "o3105"

// 기본입력                       
typedef struct _o3105InBlock
{
    char    symbol              [   8];    char    _symbol              ;    // [string,    8] 종목코드                        StartPos 0, Length 8
} o3105InBlock, *LPo3105InBlock;
#define NAME_o3105InBlock     "o3105InBlock"

// 출력                           
typedef struct _o3105OutBlock
{
    char    Symbol              [   8];    char    _Symbol              ;    // [string,    8] 종목코드                        StartPos 0, Length 8
    char    SymbolNm            [  50];    char    _SymbolNm            ;    // [string,   50] 종목명                          StartPos 9, Length 50
    char    ApplDate            [   8];    char    _ApplDate            ;    // [string,    8] 종목배치수신일                  StartPos 60, Length 8
    char    BscGdsCd            [  10];    char    _BscGdsCd            ;    // [string,   10] 기초상품코드                    StartPos 69, Length 10
    char    BscGdsNm            [  40];    char    _BscGdsNm            ;    // [string,   40] 기초상품명                      StartPos 80, Length 40
    char    ExchCd              [  10];    char    _ExchCd              ;    // [string,   10] 거래소코드                      StartPos 121, Length 10
    char    ExchNm              [  40];    char    _ExchNm              ;    // [string,   40] 거래소명                        StartPos 132, Length 40
    char    EcCd                [   1];    char    _EcCd                ;    // [string,    1] 정산구분코드                    StartPos 173, Length 1
    char    CrncyCd             [   3];    char    _CrncyCd             ;    // [string,    3] 기준통화코드                    StartPos 175, Length 3
    char    NotaCd              [   3];    char    _NotaCd              ;    // [string,    3] 진법구분코드                    StartPos 179, Length 3
    char    UntPrc              [  15];    char    _UntPrc              ;    // [double, 15.9] 호가단위가격                    StartPos 183, Length 15
    char    MnChgAmt            [  15];    char    _MnChgAmt            ;    // [double, 15.9] 최소가격변동금액                StartPos 199, Length 15
    char    RgltFctr            [  15];    char    _RgltFctr            ;    // [double,15.10] 가격조정계수                    StartPos 215, Length 15
    char    CtrtPrAmt           [  15];    char    _CtrtPrAmt           ;    // [double, 15.2] 계약당금액                      StartPos 231, Length 15
    char    LstngMCnt           [   2];    char    _LstngMCnt           ;    // [long  ,    2] 상장개월수                      StartPos 247, Length 2
    char    GdsCd               [   3];    char    _GdsCd               ;    // [string,    3] 상품구분코드                    StartPos 250, Length 3
    char    MrktCd              [   3];    char    _MrktCd              ;    // [string,    3] 시장구분코드                    StartPos 254, Length 3
    char    EminiCd             [   1];    char    _EminiCd             ;    // [string,    1] Emini구분코드                   StartPos 258, Length 1
    char    LstngYr             [   4];    char    _LstngYr             ;    // [string,    4] 상장년                          StartPos 260, Length 4
    char    LstngM              [   1];    char    _LstngM              ;    // [string,    1] 상장월                          StartPos 265, Length 1
    char    SeqNo               [   5];    char    _SeqNo               ;    // [long  ,    5] 월물순서                        StartPos 267, Length 5
    char    LstngDt             [   8];    char    _LstngDt             ;    // [string,    8] 상장일자                        StartPos 273, Length 8
    char    MtrtDt              [   8];    char    _MtrtDt              ;    // [string,    8] 최종거래가능일                  StartPos 282, Length 8
    char    FnlDlDt             [   8];    char    _FnlDlDt             ;    // [string,    8] 최종거래일                      StartPos 291, Length 8
    char    FstTrsfrDt          [   8];    char    _FstTrsfrDt          ;    // [string,    8] 최초인도통지일자                StartPos 300, Length 8
    char    EcPrc               [  15];    char    _EcPrc               ;    // [double, 15.9] 정산가격                        StartPos 309, Length 15
    char    DlDt                [   8];    char    _DlDt                ;    // [string,    8] 거래시작일자(한국)              StartPos 325, Length 8
    char    DlStrtTm            [   6];    char    _DlStrtTm            ;    // [string,    6] 거래시작시간(한국)              StartPos 334, Length 6
    char    DlEndTm             [   6];    char    _DlEndTm             ;    // [string,    6] 거래종료시간(한국)              StartPos 341, Length 6
    char    OvsStrDay           [   8];    char    _OvsStrDay           ;    // [string,    8] 거래시작일자(현지)              StartPos 348, Length 8
    char    OvsStrTm            [   6];    char    _OvsStrTm            ;    // [string,    6] 거래시작시간(현지)              StartPos 357, Length 6
    char    OvsEndDay           [   8];    char    _OvsEndDay           ;    // [string,    8] 거래종료일자(현지)              StartPos 364, Length 8
    char    OvsEndTm            [   6];    char    _OvsEndTm            ;    // [string,    6] 거래종료시간(현지)              StartPos 373, Length 6
    char    DlPsblCd            [   1];    char    _DlPsblCd            ;    // [string,    1] 거래가능구분코드                StartPos 380, Length 1
    char    MgnCltCd            [   1];    char    _MgnCltCd            ;    // [string,    1] 증거금징수구분코드              StartPos 382, Length 1
    char    OpngMgn             [  15];    char    _OpngMgn             ;    // [double, 15.2] 개시증거금                      StartPos 384, Length 15
    char    MntncMgn            [  15];    char    _MntncMgn            ;    // [double, 15.2] 유지증거금                      StartPos 400, Length 15
    char    OpngMgnR            [   7];    char    _OpngMgnR            ;    // [double,  7.3] 개시증거금율                    StartPos 416, Length 7
    char    MntncMgnR           [   7];    char    _MntncMgnR           ;    // [double,  7.3] 유지증거금율                    StartPos 424, Length 7
    char    DotGb               [   2];    char    _DotGb               ;    // [long  ,    2] 유효소수점자리수                StartPos 432, Length 2
    char    TimeDiff            [   5];    char    _TimeDiff            ;    // [long  ,    5] 시차                            StartPos 435, Length 5
    char    OvsDate             [   8];    char    _OvsDate             ;    // [string,    8] 현지체결일자                    StartPos 441, Length 8
    char    KorDate             [   8];    char    _KorDate             ;    // [string,    8] 한국체결일자                    StartPos 450, Length 8
    char    TrdTm               [   6];    char    _TrdTm               ;    // [string,    6] 현지체결시간                    StartPos 459, Length 6
    char    RcvTm               [   6];    char    _RcvTm               ;    // [string,    6] 한국체결시각                    StartPos 466, Length 6
    char    TrdP                [  15];    char    _TrdP                ;    // [double, 15.9] 체결가격                        StartPos 473, Length 15
    char    TrdQ                [  10];    char    _TrdQ                ;    // [long  ,   10] 체결수량                        StartPos 489, Length 10
    char    TotQ                [  15];    char    _TotQ                ;    // [long  ,   15] 누적거래량                      StartPos 500, Length 15
    char    TrdAmt              [  15];    char    _TrdAmt              ;    // [double, 15.2] 체결거래대금                    StartPos 516, Length 15
    char    TotAmt              [  15];    char    _TotAmt              ;    // [double, 15.2] 누적거래대금                    StartPos 532, Length 15
    char    OpenP               [  15];    char    _OpenP               ;    // [double, 15.9] 시가                            StartPos 548, Length 15
    char    HighP               [  15];    char    _HighP               ;    // [double, 15.9] 고가                            StartPos 564, Length 15
    char    LowP                [  15];    char    _LowP                ;    // [double, 15.9] 저가                            StartPos 580, Length 15
    char    CloseP              [  15];    char    _CloseP              ;    // [double, 15.9] 전일종가                        StartPos 596, Length 15
    char    YdiffP              [  15];    char    _YdiffP              ;    // [double, 15.9] 전일대비                        StartPos 612, Length 15
    char    YdiffSign           [   1];    char    _YdiffSign           ;    // [string,    1] 전일대비구분                    StartPos 628, Length 1
    char    Cgubun              [   1];    char    _Cgubun              ;    // [string,    1] 체결구분                        StartPos 630, Length 1
    char    Diff                [   6];    char    _Diff                ;    // [double,  6.2] 등락율                          StartPos 632, Length 6
} o3105OutBlock, *LPo3105OutBlock;
#define NAME_o3105OutBlock     "o3105OutBlock"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _o3105_H_
