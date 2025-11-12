#ifndef _o3103_H_
#define _o3103_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// 해외선물차트 분봉 조회 ( ATTR,BLOCK,HEADTYPE=A )
#pragma pack( push, 1 )

#define NAME_o3103     "o3103"

// 기본입력                       
typedef struct _o3103InBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                        StartPos 0, Length 8
    char    ncnt                [   4];    char    _ncnt                ;    // [long  ,    4] N분주기                         StartPos 9, Length 4
    char    readcnt             [   4];    char    _readcnt             ;    // [long  ,    4] 조회건수                        StartPos 14, Length 4
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                        StartPos 19, Length 8
    char    cts_time            [   6];    char    _cts_time            ;    // [string,    6] 연속시간                        StartPos 28, Length 6
} o3103InBlock, *LPo3103InBlock;
#define NAME_o3103InBlock     "o3103InBlock"

// 출력                           
typedef struct _o3103OutBlock
{
    char    shcode              [   8];    char    _shcode              ;    // [string,    8] 단축코드                        StartPos 0, Length 8
    char    timediff            [   4];    char    _timediff            ;    // [long  ,    4] 시차                            StartPos 9, Length 4
    char    readcnt             [   4];    char    _readcnt             ;    // [long  ,    4] 조회건수                        StartPos 14, Length 4
    char    cts_date            [   8];    char    _cts_date            ;    // [string,    8] 연속일자                        StartPos 19, Length 8
    char    cts_time            [   6];    char    _cts_time            ;    // [string,    6] 연속시간                        StartPos 28, Length 6
} o3103OutBlock, *LPo3103OutBlock; // 35BYTES
#define NAME_o3103OutBlock     "o3103OutBlock"

// 출력1                          , occurs
typedef struct _o3103OutBlock1
{
    char    date                [   8];    char    _date                ;    // [string,    8] 날짜                            StartPos 0, Length 8
    char    time                [   6];    char    _time                ;    // [string,    6] 현지시간                        StartPos 9, Length 6
    char    open                [  15];    char    _open                ;    // [double, 15.9] 시가                            StartPos 16, Length 15
    char    high                [  15];    char    _high                ;    // [double, 15.9] 고가                            StartPos 32, Length 15
    char    low                 [  15];    char    _low                 ;    // [double, 15.9] 저가                            StartPos 48, Length 15
    char    close               [  15];    char    _close               ;    // [double, 15.9] 종가                            StartPos 64, Length 15
    char    volume              [  12];    char    _volume              ;    // [long  ,   12] 거래량                          StartPos 80, Length 12
} o3103OutBlock1, *LPo3103OutBlock1; // 93bytes
#define NAME_o3103OutBlock1     "o3103OutBlock1"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _o3103_H_
