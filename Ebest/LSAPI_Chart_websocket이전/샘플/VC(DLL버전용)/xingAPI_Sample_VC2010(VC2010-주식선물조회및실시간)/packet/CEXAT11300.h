#ifndef _CEXAT11300_H_
#define _CEXAT11300_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// 유렉스 취소주문 ( SERVICE=CEXAT11300,headtype=B,CREATOR=이시종,CREDATE=2012/06/27 21 )
#pragma pack( push, 1 )

#define NAME_CEXAT11300     "CEXAT11300"

// In(*EMPTY*)                    
typedef struct _CEXAT11300InBlock1
{
    char    OrgOrdNo            [  10];    // [long  ,   10] 원주문번호                      
    char    AcntNo              [  20];    // [string,   20] 계좌번호                        
    char    Pwd                 [   8];    // [string,    8] 비밀번호                        
    char    FnoIsuNo            [  12];    // [string,   12] 선물옵션종목번호                
} CEXAT11300InBlock1, *LPCEXAT11300InBlock1;
#define NAME_CEXAT11300InBlock1     "CEXAT11300InBlock1"

// In(*EMPTY*)                    
typedef struct _CEXAT11300OutBlock1
{
    char    RecCnt              [   5];    // [long  ,    5] 레코드갯수                      
    char    OrgOrdNo            [  10];    // [long  ,   10] 원주문번호                      
    char    AcntNo              [  20];    // [string,   20] 계좌번호                        
    char    Pwd                 [   8];    // [string,    8] 비밀번호                        
    char    FnoIsuNo            [  12];    // [string,   12] 선물옵션종목번호                
    char    CancQty             [  16];    // [long  ,   16] 취소수량                        
    char    CommdaCode          [   2];    // [string,    2] 통신매체코드                    
} CEXAT11300OutBlock1, *LPCEXAT11300OutBlock1;
#define NAME_CEXAT11300OutBlock1     "CEXAT11300OutBlock1"

// Out(*EMPTY*)                   
typedef struct _CEXAT11300OutBlock2
{
    char    RecCnt              [   5];    // [long  ,    5] 레코드갯수                      
    char    OrdNo               [  10];    // [long  ,   10] 주문번호                        
    char    BrnNm               [  40];    // [string,   40] 지점명                          
    char    AcntNm              [  40];    // [string,   40] 계좌명                          
    char    IsuNm               [  50];    // [string,   50] 종목명                          
    char    OrdAbleAmt          [  16];    // [long  ,   16] 주문가능금액                    
    char    MnyOrdAbleAmt       [  16];    // [long  ,   16] 현금주문가능금액                
    char    OrdMgn              [  16];    // [long  ,   16] 주문증거금액                    
    char    MnyOrdMgn           [  16];    // [long  ,   16] 현금주문증거금액                
    char    OrdAbleQty          [  16];    // [long  ,   16] 주문가능수량                    
} CEXAT11300OutBlock2, *LPCEXAT11300OutBlock2;
#define NAME_CEXAT11300OutBlock2     "CEXAT11300OutBlock2"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _CEXAT11300_H_
