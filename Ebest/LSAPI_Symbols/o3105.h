#ifndef _o3105_H_
#define _o3105_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// �ؿܼ��� �������� ( ATTR,BLOCK,SVR=OVS,HEADTYPE=A )
#pragma pack( push, 1 )

#define NAME_o3105     "o3105"

// �⺻�Է�                       
typedef struct _o3105InBlock
{
    char    symbol              [   8];    char    _symbol              ;    // [string,    8] �����ڵ�                        StartPos 0, Length 8
} o3105InBlock, *LPo3105InBlock;
#define NAME_o3105InBlock     "o3105InBlock"

// ���                           
typedef struct _o3105OutBlock
{
    char    Symbol              [   8];    char    _Symbol              ;    // [string,    8] �����ڵ�                        StartPos 0, Length 8
    char    SymbolNm            [  50];    char    _SymbolNm            ;    // [string,   50] �����                          StartPos 9, Length 50
    char    ApplDate            [   8];    char    _ApplDate            ;    // [string,    8] �����ġ������                  StartPos 60, Length 8
    char    BscGdsCd            [  10];    char    _BscGdsCd            ;    // [string,   10] ���ʻ�ǰ�ڵ�                    StartPos 69, Length 10
    char    BscGdsNm            [  40];    char    _BscGdsNm            ;    // [string,   40] ���ʻ�ǰ��                      StartPos 80, Length 40
    char    ExchCd              [  10];    char    _ExchCd              ;    // [string,   10] �ŷ����ڵ�                      StartPos 121, Length 10
    char    ExchNm              [  40];    char    _ExchNm              ;    // [string,   40] �ŷ��Ҹ�                        StartPos 132, Length 40
    char    EcCd                [   1];    char    _EcCd                ;    // [string,    1] ���걸���ڵ�                    StartPos 173, Length 1
    char    CrncyCd             [   3];    char    _CrncyCd             ;    // [string,    3] ������ȭ�ڵ�                    StartPos 175, Length 3
    char    NotaCd              [   3];    char    _NotaCd              ;    // [string,    3] ���������ڵ�                    StartPos 179, Length 3
    char    UntPrc              [  15];    char    _UntPrc              ;    // [double, 15.9] ȣ����������                    StartPos 183, Length 15
    char    MnChgAmt            [  15];    char    _MnChgAmt            ;    // [double, 15.9] �ּҰ��ݺ����ݾ�                StartPos 199, Length 15
    char    RgltFctr            [  15];    char    _RgltFctr            ;    // [double,15.10] �����������                    StartPos 215, Length 15
    char    CtrtPrAmt           [  15];    char    _CtrtPrAmt           ;    // [double, 15.2] ����ݾ�                      StartPos 231, Length 15
    char    LstngMCnt           [   2];    char    _LstngMCnt           ;    // [long  ,    2] ���尳����                      StartPos 247, Length 2
    char    GdsCd               [   3];    char    _GdsCd               ;    // [string,    3] ��ǰ�����ڵ�                    StartPos 250, Length 3
    char    MrktCd              [   3];    char    _MrktCd              ;    // [string,    3] ���屸���ڵ�                    StartPos 254, Length 3
    char    EminiCd             [   1];    char    _EminiCd             ;    // [string,    1] Emini�����ڵ�                   StartPos 258, Length 1
    char    LstngYr             [   4];    char    _LstngYr             ;    // [string,    4] �����                          StartPos 260, Length 4
    char    LstngM              [   1];    char    _LstngM              ;    // [string,    1] �����                          StartPos 265, Length 1
    char    SeqNo               [   5];    char    _SeqNo               ;    // [long  ,    5] ��������                        StartPos 267, Length 5
    char    LstngDt             [   8];    char    _LstngDt             ;    // [string,    8] ��������                        StartPos 273, Length 8
    char    MtrtDt              [   8];    char    _MtrtDt              ;    // [string,    8] �����ŷ�������                  StartPos 282, Length 8
    char    FnlDlDt             [   8];    char    _FnlDlDt             ;    // [string,    8] �����ŷ���                      StartPos 291, Length 8
    char    FstTrsfrDt          [   8];    char    _FstTrsfrDt          ;    // [string,    8] �����ε���������                StartPos 300, Length 8
    char    EcPrc               [  15];    char    _EcPrc               ;    // [double, 15.9] ���갡��                        StartPos 309, Length 15
    char    DlDt                [   8];    char    _DlDt                ;    // [string,    8] �ŷ���������(�ѱ�)              StartPos 325, Length 8
    char    DlStrtTm            [   6];    char    _DlStrtTm            ;    // [string,    6] �ŷ����۽ð�(�ѱ�)              StartPos 334, Length 6
    char    DlEndTm             [   6];    char    _DlEndTm             ;    // [string,    6] �ŷ�����ð�(�ѱ�)              StartPos 341, Length 6
    char    OvsStrDay           [   8];    char    _OvsStrDay           ;    // [string,    8] �ŷ���������(����)              StartPos 348, Length 8
    char    OvsStrTm            [   6];    char    _OvsStrTm            ;    // [string,    6] �ŷ����۽ð�(����)              StartPos 357, Length 6
    char    OvsEndDay           [   8];    char    _OvsEndDay           ;    // [string,    8] �ŷ���������(����)              StartPos 364, Length 8
    char    OvsEndTm            [   6];    char    _OvsEndTm            ;    // [string,    6] �ŷ�����ð�(����)              StartPos 373, Length 6
    char    DlPsblCd            [   1];    char    _DlPsblCd            ;    // [string,    1] �ŷ����ɱ����ڵ�                StartPos 380, Length 1
    char    MgnCltCd            [   1];    char    _MgnCltCd            ;    // [string,    1] ���ű�¡�������ڵ�              StartPos 382, Length 1
    char    OpngMgn             [  15];    char    _OpngMgn             ;    // [double, 15.2] �������ű�                      StartPos 384, Length 15
    char    MntncMgn            [  15];    char    _MntncMgn            ;    // [double, 15.2] �������ű�                      StartPos 400, Length 15
    char    OpngMgnR            [   7];    char    _OpngMgnR            ;    // [double,  7.3] �������ű���                    StartPos 416, Length 7
    char    MntncMgnR           [   7];    char    _MntncMgnR           ;    // [double,  7.3] �������ű���                    StartPos 424, Length 7
    char    DotGb               [   2];    char    _DotGb               ;    // [long  ,    2] ��ȿ�Ҽ����ڸ���                StartPos 432, Length 2
    char    TimeDiff            [   5];    char    _TimeDiff            ;    // [long  ,    5] ����                            StartPos 435, Length 5
    char    OvsDate             [   8];    char    _OvsDate             ;    // [string,    8] ����ü������                    StartPos 441, Length 8
    char    KorDate             [   8];    char    _KorDate             ;    // [string,    8] �ѱ�ü������                    StartPos 450, Length 8
    char    TrdTm               [   6];    char    _TrdTm               ;    // [string,    6] ����ü��ð�                    StartPos 459, Length 6
    char    RcvTm               [   6];    char    _RcvTm               ;    // [string,    6] �ѱ�ü��ð�                    StartPos 466, Length 6
    char    TrdP                [  15];    char    _TrdP                ;    // [double, 15.9] ü�ᰡ��                        StartPos 473, Length 15
    char    TrdQ                [  10];    char    _TrdQ                ;    // [long  ,   10] ü�����                        StartPos 489, Length 10
    char    TotQ                [  15];    char    _TotQ                ;    // [long  ,   15] �����ŷ���                      StartPos 500, Length 15
    char    TrdAmt              [  15];    char    _TrdAmt              ;    // [double, 15.2] ü��ŷ����                    StartPos 516, Length 15
    char    TotAmt              [  15];    char    _TotAmt              ;    // [double, 15.2] �����ŷ����                    StartPos 532, Length 15
    char    OpenP               [  15];    char    _OpenP               ;    // [double, 15.9] �ð�                            StartPos 548, Length 15
    char    HighP               [  15];    char    _HighP               ;    // [double, 15.9] ��                            StartPos 564, Length 15
    char    LowP                [  15];    char    _LowP                ;    // [double, 15.9] ����                            StartPos 580, Length 15
    char    CloseP              [  15];    char    _CloseP              ;    // [double, 15.9] ��������                        StartPos 596, Length 15
    char    YdiffP              [  15];    char    _YdiffP              ;    // [double, 15.9] ���ϴ��                        StartPos 612, Length 15
    char    YdiffSign           [   1];    char    _YdiffSign           ;    // [string,    1] ���ϴ�񱸺�                    StartPos 628, Length 1
    char    Cgubun              [   1];    char    _Cgubun              ;    // [string,    1] ü�ᱸ��                        StartPos 630, Length 1
    char    Diff                [   6];    char    _Diff                ;    // [double,  6.2] �����                          StartPos 632, Length 6
} o3105OutBlock, *LPo3105OutBlock;
#define NAME_o3105OutBlock     "o3105OutBlock"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _o3105_H_
