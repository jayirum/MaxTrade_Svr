#ifndef _t1866_H_
#define _t1866_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// 서버저장조건리스트조회(API/MTS)(t1866) ( ENCRYPT,block,headtype=A )
#pragma pack( push, 1 )

#define NAME_t1866     "t1866"

// 기본입력                       
typedef struct _t1866InBlock
{
    char    user_id             [   8];    // [string,    8] 로그인ID                        StartPos 0, Length 8
    char    gb                  [   1];    // [string,    1] 조회구분                        StartPos 8, Length 1
    char    group_name          [  40];    // [string,   40] 그룹명                          StartPos 9, Length 40
    char    cont                [   1];    // [string,    1] 연속여부                        StartPos 49, Length 1
    char    cont_key            [  40];    // [string,   40] 연속키                          StartPos 50, Length 40
} t1866InBlock, *LPt1866InBlock;
#define NAME_t1866InBlock     "t1866InBlock"

// 출력                           
typedef struct _t1866OutBlock
{
    char    result_count        [   5];    // [long  ,    5] 저장조건수                      StartPos 0, Length 5
    char    cont                [   1];    // [string,    1] 연속여부                        StartPos 5, Length 1
    char    cont_key            [  40];    // [string,   40] 연속키                          StartPos 6, Length 40
} t1866OutBlock, *LPt1866OutBlock;
#define NAME_t1866OutBlock     "t1866OutBlock"

// 출력1                          , occurs
typedef struct _t1866OutBlock1
{
    char    query_index         [  12];    // [string,   12] 서버저장인덱스                  StartPos 0, Length 12
    char    group_name          [  40];    // [string,   40] 그룹명                          StartPos 12, Length 40
    char    query_name          [  40];    // [string,   40] 조건저장명                      StartPos 52, Length 40
} t1866OutBlock1, *LPt1866OutBlock1;
#define NAME_t1866OutBlock1     "t1866OutBlock1"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _t1866_H_
