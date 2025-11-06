// Dlg_CSPBQ00200.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPBQ00200.h"

#include "./Packet/CSPBQ00200.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPBQ00200 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPBQ00200, CDialog)

CDlg_CSPBQ00200::CDlg_CSPBQ00200(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPBQ00200::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPBQ00200)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPBQ00200::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPBQ00200)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
}


BEGIN_MESSAGE_MAP(CDlg_CSPBQ00200, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPBQ00200)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPBQ00200 message handlers

BOOL CDlg_CSPBQ00200::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPBQ00200::OnButtonRequest() 
{
	RequestData();
}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPBQ00200::InitCtrls()
{
	int nCol;

	//-------------------------------------------------------------------------
	// InBlock : 기본값 세팅
	
	// 계좌번호 세팅
	char szAccount[20];
	int nCount = g_iXingAPI.GetAccountListCount();
	for( int i=0; i<nCount; i++ )
	{
		if( g_iXingAPI.GetAccountList( i, szAccount, sizeof( szAccount ) ) )
		{
			((CComboBox*)GetDlgItem( IDC_INBLOCK_ACNTNO ))->AddString( szAccount );
		}	
	}

	//-------------------------------------------------------------------------
	// OutBlock1
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수  ", FALSE,    5 );    // [long  ,    5] 레코드갯수  
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매매구분    ", TRUE ,    1 );    // [string,    1] 매매구분    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호    ", TRUE ,   20 );    // [string,   20] 계좌번호    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "입력비밀번호", TRUE ,    8 );    // [string,    8] 입력비밀번호
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "종목번호    ", TRUE ,   12 );    // [string,   12] 종목번호    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문가격    ", FALSE, 15.2 );    // [double, 15.2] 주문가격    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "통신매체코드", TRUE ,    2 );    // [string,    2] 통신매체코드

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수                       ", FALSE,    5 );    // [long  ,    5] 레코드갯수                      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명                           ", TRUE ,   40 );    // [string,   40] 계좌명                          
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "종목명                           ", TRUE ,   40 );    // [string,   40] 종목명                          
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예수금                           ", FALSE,   16 );    // [long  ,   16] 예수금                          
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용금액                         ", FALSE,   16 );    // [long  ,   16] 대용금액                        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보재사용금액               ", FALSE,   16 );    // [long  ,   16] 신용담보재사용금액              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문가능금액                 ", FALSE,   16 );    // [long  ,   16] 현금주문가능금액                
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용주문가능금액                 ", FALSE,   16 );    // [long  ,   16] 대용주문가능금액                
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금증거금액                     ", FALSE,   16 );    // [long  ,   16] 현금증거금액                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용증거금액                     ", FALSE,   16 );    // [long  ,   16] 대용증거금액                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "거래소금액                       ", FALSE,   16 );    // [long  ,   16] 거래소금액                      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "코스닥금액                       ", FALSE,   16 );    // [long  ,   16] 코스닥금액                      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추정예수금(D+1)                  ", FALSE,   16 );    // [long  ,   16] 추정예수금(D+1)                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추정예수금(D+2)                  ", FALSE,   16 );    // [long  ,   16] 추정예수금(D+2)                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "출금가능금액                     ", FALSE,   16 );    // [long  ,   16] 출금가능금액                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "미수금액                         ", FALSE,   16 );    // [long  ,   16] 미수금액                        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "수수료율                         ", FALSE, 15.5 );    // [double, 15.5] 수수료율                        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추가징수금액                     ", FALSE,   16 );    // [long  ,   16] 추가징수금액                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "재사용대상금액                   ", FALSE,   16 );    // [long  ,   16] 재사용대상금액                  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금재사용대상금액               ", FALSE,   16 );    // [long  ,   16] 현금재사용대상금액              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "이용사증거금률                   ", FALSE,  7.4 );    // [double,  7.4] 이용사증거금률                  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용재사용대상금액               ", FALSE,   16 );    // [long  ,   16] 대용재사용대상금액              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "종목증거금률                     ", FALSE,  7.4 );    // [double,  7.4] 종목증거금률                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌증거금률                     ", FALSE,  7.4 );    // [double,  7.4] 계좌증거금률                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "거래증거금률                     ", FALSE,  7.4 );    // [double,  7.4] 거래증거금률                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "수수료                           ", FALSE,   16 );    // [long  ,   16] 수수료                          
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률20퍼센트주문가능금액     ", FALSE,   16 );    // [long  ,   16] 증거금률20퍼센트주문가능금액    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트현금주문가능수량", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트현금주문가능수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률30퍼센트주문가능금액     ", FALSE,   16 );    // [long  ,   16] 증거금률30퍼센트주문가능금액    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률30퍼센트주문가능수량     ", FALSE,   16 );    // [long  ,   16] 증거금률30퍼센트주문가능수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률40퍼센트주문가능금액     ", FALSE,   16 );    // [long  ,   16] 증거금률40퍼센트주문가능금액    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률40퍼센트주문가능수량     ", FALSE,   16 );    // [long  ,   16] 증거금률40퍼센트주문가능수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트주문가능금액    ", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트주문가능금액   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트주문가능수량    ", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트주문가능수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트현금주문가능금액", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트현금주문가능금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트현금주문가능수량", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트현금주문가능수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률20퍼센트재사용가능금액   ", FALSE,   16 );    // [long  ,   16] 증거금률20퍼센트재사용가능금액  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률30퍼센트재사용가능금액   ", FALSE,   16 );    // [long  ,   16] 증거금률30퍼센트재사용가능금액  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률40퍼센트재사용가능금액   ", FALSE,   16 );    // [long  ,   16] 증거금률40퍼센트재사용가능금액  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트재사용가능금액  ", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트재사용가능금액 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문가능수량                     ", FALSE,   16 );    // [long  ,   16] 주문가능수량                    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문가능금액                     ", FALSE,   16 );    // [long  ,   16] 주문가능금액                    
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPBQ00200::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
// 현물정상주문(CSPBQ00200) ( base21=SONAT000,headtype=B )
	CSPBQ00200InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPBQ00200";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strBnsTpCode    ;		GetDlgItemText( IDC_INBLOCK_BNSTPCODE    , strBnsTpCode     );    // [string,    1] 매매구분    
    CString strAcntNo       ;		GetDlgItemText( IDC_INBLOCK_ACNTNO       , strAcntNo        );    // [string,   20] 계좌번호    
    CString strInptPwd      ;		GetDlgItemText( IDC_INBLOCK_INPTPWD      , strInptPwd       );    // [string,    8] 입력비밀번호
    CString strIsuNo        ;		GetDlgItemText( IDC_INBLOCK_ISUNO        , strIsuNo         );    // [string,   12] 종목번호    
    CString strOrdPrc       ;		GetDlgItemText( IDC_INBLOCK_ORDPRC       , strOrdPrc        );    // [double, 15.2] 주문가격    

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt       , sizeof( pckInBlock.RecCnt        ), "1"                      , DATA_TYPE_LONG   );    // [long  ,    5] 레코드갯수  
	SetPacketData( pckInBlock.BnsTpCode    , sizeof( pckInBlock.BnsTpCode     ), strBnsTpCode             , DATA_TYPE_STRING );    // [string,    1] 매매구분    
	SetPacketData( pckInBlock.AcntNo       , sizeof( pckInBlock.AcntNo        ), strAcntNo                , DATA_TYPE_STRING );    // [string,   20] 계좌번호    
	SetPacketData( pckInBlock.InptPwd      , sizeof( pckInBlock.InptPwd       ), strInptPwd               , DATA_TYPE_STRING );    // [string,    8] 입력비밀번호
	SetPacketData( pckInBlock.IsuNo        , sizeof( pckInBlock.IsuNo         ), strIsuNo                 , DATA_TYPE_STRING );    // [string,   12] 종목번호    
	SetPacketData( pckInBlock.OrdPrc       , sizeof( pckInBlock.OrdPrc        ), strOrdPrc                , DATA_TYPE_DOT, 2 );    // [double, 15.2] 주문가격    
	SetPacketData( pckInBlock.RegCommdaCode, sizeof( pckInBlock.RegCommdaCode ), g_iXingAPI.GetCommMedia(), DATA_TYPE_STRING );    // [string,    2] 통신매체코드

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		szNextKey,					// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);

	m_ctrlOutBlock1.DeleteAllItems();
	m_ctrlOutBlock2.DeleteAllItems();

	//-----------------------------------------------------------
	// Request ID가 0보다 작을 경우에는 에러이다.
	if( nRqID < 0 )
	{
		MessageBox( "조회실패", "에러", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_CSPBQ00200::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		typedef struct
		{
			CSPBQ00200OutBlock1	outBlock1;
			CSPBQ00200OutBlock2	outBlock2;
		} CSPBQ00200AllOutBlock, *LPCSPBQ00200AllOutBlock;

		LPCSPBQ00200AllOutBlock pAllOutBlock = (LPCSPBQ00200AllOutBlock)pRpData->lpData;

		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		if( pRpData->nDataLength >= sizeof( CSPBQ00200OutBlock1 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock1.InsertItem( 0, "" );
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt       , sizeof( pAllOutBlock->outBlock1.RecCnt        ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수  
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BnsTpCode    , sizeof( pAllOutBlock->outBlock1.BnsTpCode     ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분    
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo       , sizeof( pAllOutBlock->outBlock1.AcntNo        ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호    
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.InptPwd      , sizeof( pAllOutBlock->outBlock1.InptPwd       ), DATA_TYPE_STRING ) );    // [string,    8] 입력비밀번호
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.IsuNo        , sizeof( pAllOutBlock->outBlock1.IsuNo         ), DATA_TYPE_LONG   ) );    // [string,   12] 종목번호    
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdPrc       , sizeof( pAllOutBlock->outBlock1.OrdPrc        ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 주문가격            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RegCommdaCode, sizeof( pAllOutBlock->outBlock1.RegCommdaCode ), DATA_TYPE_STRING ) );    // [string,    2] 통신매체코드
		}

		if( pRpData->nDataLength >= sizeof( CSPBQ00200OutBlock1 ) + sizeof( CSPBQ00200OutBlock2 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock2.InsertItem( 0, "" );
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt                 , sizeof( pAllOutBlock->outBlock2.RecCnt                  ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm                 , sizeof( pAllOutBlock->outBlock2.AcntNm                  ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명                         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.IsuNm                  , sizeof( pAllOutBlock->outBlock2.IsuNm                   ), DATA_TYPE_STRING ) );    // [string,   40] 종목명                         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.Dps                    , sizeof( pAllOutBlock->outBlock2.Dps                     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 예수금                         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstAmt               , sizeof( pAllOutBlock->outBlock2.SubstAmt                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용금액                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgRuseAmt        , sizeof( pAllOutBlock->outBlock2.CrdtPldgRuseAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 신용담보재사용금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAbleAmt          , sizeof( pAllOutBlock->outBlock2.MnyOrdAbleAmt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문가능금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstOrdAbleAmt        , sizeof( pAllOutBlock->outBlock2.SubstOrdAbleAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용주문가능금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyMgn                 , sizeof( pAllOutBlock->outBlock2.MnyMgn                  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금증거금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstMgn               , sizeof( pAllOutBlock->outBlock2.SubstMgn                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용증거금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SeOrdAbleAmt           , sizeof( pAllOutBlock->outBlock2.SeOrdAbleAmt            ), DATA_TYPE_LONG   ) );    // [long  ,   16] 거래소금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.KdqOrdAbleAmt          , sizeof( pAllOutBlock->outBlock2.KdqOrdAbleAmt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 코스닥금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrsmptDpsD1            , sizeof( pAllOutBlock->outBlock2.PrsmptDpsD1             ), DATA_TYPE_LONG   ) );    // [long  ,   16] 추정예수금(D+1)                
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrsmptDpsD2            , sizeof( pAllOutBlock->outBlock2.PrsmptDpsD2             ), DATA_TYPE_LONG   ) );    // [long  ,   16] 추정예수금(D+2)                
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyoutAbleAmt          , sizeof( pAllOutBlock->outBlock2.MnyoutAbleAmt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 출금가능금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RcvblAmt               , sizeof( pAllOutBlock->outBlock2.RcvblAmt                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 미수금액                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CmsnRat                , sizeof( pAllOutBlock->outBlock2.CmsnRat                 ), DATA_TYPE_DOT, 5 ) );    // [double, 15.5] 수수료율                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AddLevyAmt             , sizeof( pAllOutBlock->outBlock2.AddLevyAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 추가징수금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RuseObjAmt             , sizeof( pAllOutBlock->outBlock2.RuseObjAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 재사용대상금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyRuseObjAmt          , sizeof( pAllOutBlock->outBlock2.MnyRuseObjAmt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금재사용대상금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.FirmMgnRat             , sizeof( pAllOutBlock->outBlock2.FirmMgnRat              ), DATA_TYPE_DOT, 4 ) );    // [double,  7.4] 이용사증거금률                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstRuseObjAmt        , sizeof( pAllOutBlock->outBlock2.SubstRuseObjAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용재사용대상금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.IsuMgnRat              , sizeof( pAllOutBlock->outBlock2.IsuMgnRat               ), DATA_TYPE_DOT, 4 ) );    // [double,  7.4] 종목증거금률                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntMgnRat             , sizeof( pAllOutBlock->outBlock2.AcntMgnRat              ), DATA_TYPE_DOT, 4 ) );    // [double,  7.4] 계좌증거금률                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.TrdMgnrt               , sizeof( pAllOutBlock->outBlock2.TrdMgnrt                ), DATA_TYPE_DOT, 4 ) );    // [double,  7.4] 거래증거금률                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.Cmsn                   , sizeof( pAllOutBlock->outBlock2.Cmsn                    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 수수료                         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat20pctOrdAbleAmt  , sizeof( pAllOutBlock->outBlock2.MgnRat20pctOrdAbleAmt   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률20퍼센트주문가능금액   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat20OrdAbleQty     , sizeof( pAllOutBlock->outBlock2.MgnRat20OrdAbleQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트현금주문가능수
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat30pctOrdAbleAmt  , sizeof( pAllOutBlock->outBlock2.MgnRat30pctOrdAbleAmt   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률30퍼센트주문가능금액   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat30OrdAbleQty     , sizeof( pAllOutBlock->outBlock2.MgnRat30OrdAbleQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률30퍼센트주문가능수량?? 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat40pctOrdAbleAmt  , sizeof( pAllOutBlock->outBlock2.MgnRat40pctOrdAbleAmt   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률40퍼센트주문가능금액   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat40OrdAbleQty     , sizeof( pAllOutBlock->outBlock2.MgnRat40OrdAbleQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률40퍼센트주문가능수량?? 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt , sizeof( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트주문가능금액  
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100OrdAbleQty    , sizeof( pAllOutBlock->outBlock2.MgnRat100OrdAbleQty     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트주문가능수량??
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100MnyOrdAbleAmt , sizeof( pAllOutBlock->outBlock2.MgnRat100MnyOrdAbleAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트현금주문가능금
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100MnyOrdAbleQty , sizeof( pAllOutBlock->outBlock2.MgnRat100MnyOrdAbleQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트현금주문가능수
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat20pctRuseAbleAmt , sizeof( pAllOutBlock->outBlock2.MgnRat20pctRuseAbleAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률20퍼센트재사용가능금액 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat30pctRuseAbleAmt , sizeof( pAllOutBlock->outBlock2.MgnRat30pctRuseAbleAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률30퍼센트재사용가능금액 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat40pctRuseAbleAmt , sizeof( pAllOutBlock->outBlock2.MgnRat40pctRuseAbleAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률40퍼센트재사용가능금액 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100pctRuseAbleAmt, sizeof( pAllOutBlock->outBlock2.MgnRat100pctRuseAbleAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트재사용가능금액
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAbleQty             , sizeof( pAllOutBlock->outBlock2.OrdAbleQty              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문가능수량                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAbleAmt             , sizeof( pAllOutBlock->outBlock2.OrdAbleAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문가능금액                   
		}
	}
	
	//-------------------------------------------------------------------------------------
	// 메시지를 받음
	else if( wParam == MESSAGE_DATA )
	{
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;

		CString strMsg( (char*)pMsg->lpszMessageData, pMsg->nMsgLength );
		GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg );		
		
		g_iXingAPI.ReleaseMessageData( lParam );
	}
	
	//-------------------------------------------------------------------------------------
	// System Error를 받음
	else if( wParam == SYSTEM_ERROR_DATA )
	{
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		
		CString strMsg( (char*)pMsg->lpszMessageData, pMsg->nMsgLength );
		GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg );		

		g_iXingAPI.ReleaseMessageData( lParam );
	}

	//-------------------------------------------------------------------------------------
	// Release Data를 받음
	else if( wParam == RELEASE_DATA )
	{
		g_iXingAPI.ReleaseRequestData( (int)lParam );
	}

	return 0L;
}

//--------------------------------------------------------------------------------------
// Timeout 발생
//--------------------------------------------------------------------------------------
LRESULT CDlg_CSPBQ00200::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
