#ifndef _t1866_H_
#define _t1866_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// �����������Ǹ���Ʈ��ȸ(API/MTS)(t1866) ( ENCRYPT,block,headtype=A )
#pragma pack( push, 1 )

#define NAME_t1866     "t1866"

// �⺻�Է�                       
typedef struct _t1866InBlock
{
    char    user_id             [   8];    // [string,    8] �α���ID                        StartPos 0, Length 8
    char    gb                  [   1];    // [string,    1] ��ȸ����                        StartPos 8, Length 1
    char    group_name          [  40];    // [string,   40] �׷��                          StartPos 9, Length 40
    char    cont                [   1];    // [string,    1] ���ӿ���                        StartPos 49, Length 1
    char    cont_key            [  40];    // [string,   40] ����Ű                          StartPos 50, Length 40
} t1866InBlock, *LPt1866InBlock;
#define NAME_t1866InBlock     "t1866InBlock"

// ���                           
typedef struct _t1866OutBlock
{
    char    result_count        [   5];    // [long  ,    5] �������Ǽ�                      StartPos 0, Length 5
    char    cont                [   1];    // [string,    1] ���ӿ���                        StartPos 5, Length 1
    char    cont_key            [  40];    // [string,   40] ����Ű                          StartPos 6, Length 40
} t1866OutBlock, *LPt1866OutBlock;
#define NAME_t1866OutBlock     "t1866OutBlock"

// ���1                          , occurs
typedef struct _t1866OutBlock1
{
    char    query_index         [  12];    // [string,   12] ���������ε���                  StartPos 0, Length 12
    char    group_name          [  40];    // [string,   40] �׷��                          StartPos 12, Length 40
    char    query_name          [  40];    // [string,   40] ���������                      StartPos 52, Length 40
} t1866OutBlock1, *LPt1866OutBlock1;
#define NAME_t1866OutBlock1     "t1866OutBlock1"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _t1866_H_
