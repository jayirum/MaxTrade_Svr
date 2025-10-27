#ifndef _t1857_H_
#define _t1857_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// ����������������˻�(API/MTS)(t1857) ( attr,ENCRYPT,block,headtype=A )
#pragma pack( push, 1 )

#define NAME_t1857     "t1857"

// �⺻�Է�                       
typedef struct _t1857InBlock
{
	char	sRealFlag           [   1];    char    _sRealFlag           ;    // [string,    1] �ǽð�����      1:��� 0:��ȸ�� StartPos 0, Length 1
	char	sSearchFlag         [   1];    char    _sSearchFlag         ;    // [string,    1] ����˻� ����   F:���� S:����   StartPos 1, Length 1
    char    query_index         [ 256];    char    _query_index         ;    // [string,   12] ����˻� �Է°�                 StartPos 2, Length 256
} t1857InBlock, *LPt1857InBlock;
#define NAME_t1857InBlock     "t1857InBlock"

// ���                           
typedef struct _t1857OutBlock
{
    char    result_count        [   5];    char    _result_count        ;    // [long  ,    5] �˻������                      StartPos 0, Length 5
    char    result_time         [   6];    char    _result_time         ;    // [string,    6] �����ð�                        StartPos 5, Length 6
	char    AlertNum            [  11];    char    _AlertNum            ;    // [string,   11] �ǽð�Ű                        StartPos 11, Length 11
} t1857OutBlock, *LPt1857OutBlock;
#define NAME_t1857OutBlock     "t1857OutBlock"

// ���1                          , occurs
typedef struct _t1857OutBlock1
{
    char    shcode              [   7];    char    _shcode              ;    // [string,    7] �����ڵ�                        StartPos 0, Length 7
    char    hname               [  40];    char    _hname               ;    // [string,   40] �����                          StartPos 8, Length 40
    char    price               [   9];    char    _price               ;    // [long  ,    9] ���簡                          StartPos 49, Length 9
    char    sign                [   1];    char    _sign                ;    // [string,    1] ���ϴ�񱸺�                    StartPos 59, Length 1
    char    change              [   9];    char    _change              ;    // [long  ,    9] ���ϴ��                        StartPos 61, Length 9
    char    diff                [   6];    char    _diff                ;    // [float ,  6.2] �����                          StartPos 71, Length 6
    char    volume              [  12];    char    _volume              ;    // [long  ,   12] �ŷ���                          StartPos 78, Length 12
	char    JobFlag             [   1];    char    _JobFlag             ;    // [string,    1] �������                        StartPos 91, Length 1
} t1857OutBlock1, *LPt1857OutBlock1;
#define NAME_t1857OutBlock1     "t1857OutBlock1"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _t1857_H_
