// Dlg_CFOFQ02400.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CFOFQ02400.h"

#include "./Packet/CFOFQ02400.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOFQ02400 dialog


IMPLEMENT_DYNCREATE(CDlg_CFOFQ02400, CDialog)

CDlg_CFOFQ02400::CDlg_CFOFQ02400(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CFOFQ02400::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CFOFQ02400)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CFOFQ02400::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CFOFQ02400)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
	DDX_Control(pDX, IDC_OUTBLOCK3, m_ctrlOutBlock3);
	DDX_Control(pDX, IDC_OUTBLOCK4, m_ctrlOutBlock4);
}


BEGIN_MESSAGE_MAP(CDlg_CFOFQ02400, CDialog)
	//{{AFX_MSG_MAP(CDlg_CFOFQ02400)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOFQ02400 message handlers

BOOL CDlg_CFOFQ02400::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CFOFQ02400::OnButtonRequest() 
{
	RequestData();
}

void CDlg_CFOFQ02400::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CFOFQ02400::InitCtrls()
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
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수      ", FALSE,    5 );    // [long  ,    5] 레코드갯수      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌명          ", TRUE ,   40 );    // [string,   40] 계좌명          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물약정수량    ", FALSE,   16 );    // [long  ,   16] 선물약정수량    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "옵션약정수량    ", FALSE,   16 );    // [long  ,   16] 옵션약정수량    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "약정수량        ", FALSE,   16 );    // [long  ,   16] 약정수량        
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물약정금액    ", FALSE,   16 );    // [long  ,   16] 선물약정금액    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물매수약정금액", FALSE,   16 );    // [long  ,   16] 선물매수약정금액
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물매도약정금액", FALSE,   16 );    // [long  ,   16] 선물매도약정금액
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "콜옵션약정금액  ", FALSE,   16 );    // [long  ,   16] 콜옵션약정금액  
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "콜매수금액      ", FALSE,   16 );    // [long  ,   16] 콜매수금액      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "콜매도금액      ", FALSE,   16 );    // [long  ,   16] 콜매도금액      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "풋옵션약정금액  ", FALSE,   16 );    // [long  ,   16] 풋옵션약정금액  
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "풋매수금액      ", FALSE,   16 );    // [long  ,   16] 풋매수금액      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "풋매도금액      ", FALSE,   16 );    // [long  ,   16] 풋매도금액      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "전체약정금액    ", FALSE,   16 );    // [long  ,   16] 전체약정금액    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매수약정누계금액", FALSE,   16 );    // [long  ,   16] 매수약정누계금액
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매도약정누계금액", FALSE,   16 );    // [long  ,   16] 매도약정누계금액
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물손익합계    ", FALSE,   16 );    // [long  ,   16] 선물손익합계    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "옵션손익합계    ", FALSE,   16 );    // [long  ,   16] 옵션손익합계    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "전체손익합계    ", FALSE,   16 );    // [long  ,   16] 전체손익합계    
	
	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수      ", FALSE,    5 );    // [long  ,    5] 레코드갯수      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명          ", TRUE ,   40 );    // [string,   40] 계좌명          
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물약정수량    ", FALSE,   16 );    // [long  ,   16] 선물약정수량    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "옵션약정수량    ", FALSE,   16 );    // [long  ,   16] 옵션약정수량    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "약정수량        ", FALSE,   16 );    // [long  ,   16] 약정수량        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물약정금액    ", FALSE,   16 );    // [long  ,   16] 선물약정금액    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물매수약정금액", FALSE,   16 );    // [long  ,   16] 선물매수약정금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물매도약정금액", FALSE,   16 );    // [long  ,   16] 선물매도약정금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "콜옵션약정금액  ", FALSE,   16 );    // [long  ,   16] 콜옵션약정금액  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "콜매수금액      ", FALSE,   16 );    // [long  ,   16] 콜매수금액      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "콜매도금액      ", FALSE,   16 );    // [long  ,   16] 콜매도금액      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "풋옵션약정금액  ", FALSE,   16 );    // [long  ,   16] 풋옵션약정금액  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "풋매수금액      ", FALSE,   16 );    // [long  ,   16] 풋매수금액      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "풋매도금액      ", FALSE,   16 );    // [long  ,   16] 풋매도금액      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전체약정금액    ", FALSE,   16 );    // [long  ,   16] 전체약정금액    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매수약정누계금액", FALSE,   16 );    // [long  ,   16] 매수약정누계금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도약정누계금액", FALSE,   16 );    // [long  ,   16] 매도약정누계금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물손익합계    ", FALSE,   16 );    // [long  ,   16] 선물손익합계    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "옵션손익합계    ", FALSE,   16 );    // [long  ,   16] 옵션손익합계    
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전체손익합계    ", FALSE,   16 );    // [long  ,   16] 전체손익합계    
	
	//-------------------------------------------------------------------------
	// OutBlock3
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물옵션품목구분", TRUE ,    1 );    // [string,    1] 선물옵션품목구분
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물매도수량    ", FALSE,   16 );    // [long  ,   16] 선물매도수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물매도손익    ", FALSE,   16 );    // [long  ,   16] 선물매도손익    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물매수수량    ", FALSE,   16 );    // [long  ,   16] 선물매수수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물매수손익    ", FALSE,   16 );    // [long  ,   16] 선물매수손익    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "콜매도수량      ", FALSE,   16 );    // [long  ,   16] 콜매도수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "콜매도손익      ", FALSE,   16 );    // [long  ,   16] 콜매도손익      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "콜매수수량      ", FALSE,   16 );    // [long  ,   16] 콜매수수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "콜매수손익      ", FALSE,   16 );    // [long  ,   16] 콜매수손익      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "풋매도수량      ", FALSE,   16 );    // [long  ,   16] 풋매도수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "풋매도손익      ", FALSE,   16 );    // [long  ,   16] 풋매도손익      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "풋매수수량      ", FALSE,   16 );    // [long  ,   16] 풋매수수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "풋매수손익      ", FALSE,   16 );    // [long  ,   16] 풋매수손익      
	
	//-------------------------------------------------------------------------
	// OutBlock4
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "종목번호    ", TRUE ,   12 );    // [string,   12] 종목번호    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "종목명      ", TRUE ,   40 );    // [string,   40] 종목명      
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "매매구분    ", TRUE ,    1 );    // [string,    1] 매매구분    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "매매구분    ", TRUE ,   10 );    // [string,   10] 매매구분    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "잔고수량    ", FALSE,   16 );    // [long  ,   16] 잔고수량    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "평균가      ", FALSE, 19.8 );    // [double, 19.8] 평균가      
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "당초금액    ", FALSE,   16 );    // [long  ,   16] 당초금액    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "당일청산수량", FALSE,   16 );    // [long  ,   16] 당일청산수량
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "현재가      ", FALSE, 13.2 );    // [double, 13.2] 현재가      
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "평가금액    ", FALSE,   16 );    // [long  ,   16] 평가금액    
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "평가손익금액", FALSE,   16 );    // [long  ,   16] 평가손익금액
	InsertColumn4List( this, IDC_OUTBLOCK4, nCol++, "평가수익률  ", FALSE, 12.6 );    // [double, 12.6] 평가수익률  

	ZeroMemory( m_szContKey, sizeof( m_szContKey ) );
	GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( FALSE );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CFOFQ02400::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	CFOFQ02400InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CFOFQ02400";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo    ;		GetDlgItemText( IDC_INBLOCK_ACNTNO    , strAcntNo     );    // 계좌번호       
    CString strPwd       ;		GetDlgItemText( IDC_INBLOCK_PWD       , strPwd        );    // 비밀번호       
    CString strRegMktCode;		GetDlgItemText( IDC_INBLOCK_REGMKTCODE, strRegMktCode );    // 등록시장코드   
    CString strBuyDt     ;		GetDlgItemText( IDC_INBLOCK_BUYDT     , strBuyDt      );    // 매수일자       

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt    , sizeof( pckInBlock.RecCnt     ), "1"          , DATA_TYPE_LONG   );    // [long  ,    5] 레코드갯수  
	SetPacketData( pckInBlock.AcntNo    , sizeof( pckInBlock.AcntNo     ), strAcntNo    , DATA_TYPE_STRING );    // [string,   20] 계좌번호    
	SetPacketData( pckInBlock.Pwd       , sizeof( pckInBlock.Pwd        ), strPwd       , DATA_TYPE_STRING );    // [string,    8] 비밀번호    
	SetPacketData( pckInBlock.RegMktCode, sizeof( pckInBlock.RegMktCode ), strRegMktCode, DATA_TYPE_STRING );    // [string,    2] 등록시장코드
	SetPacketData( pckInBlock.BuyDt     , sizeof( pckInBlock.BuyDt      ), strBuyDt     , DATA_TYPE_STRING );    // [string,    8] 매수일자    

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		bNext ? m_szContKey : "",	// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);

	m_ctrlOutBlock1.DeleteAllItems();
	m_ctrlOutBlock2.DeleteAllItems();
	if( bNext == FALSE )
	{
		m_ctrlOutBlock3.DeleteAllItems();
		m_ctrlOutBlock4.DeleteAllItems();
	}

	//-----------------------------------------------------------
	// Request ID가 0보다 작을 경우에는 에러이다.
	if( nRqID < 0 )
	{
		CString strMsg;
		strMsg.Format( "[%d] %s", nRqID, g_iXingAPI.GetErrorMessage( nRqID ) );
		MessageBox( strMsg, "조회실패", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_CFOFQ02400::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET	pRpData		= (LPRECV_PACKET)lParam;
		LPBYTE			lpData		= pRpData->lpData;
		int				nDataLength = pRpData->nDataLength;
		int				nOffset		= 0;
		int				nBlockSize	= 0;
		int				nCol;
		int				nRow;
		int				nCount;
		char			szCount[6] = { 0 };

		// 연속키를 저장한다.
		strcpy_s( m_szContKey, pRpData->szContKey );
		GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( pRpData->cCont[0] == 'Y' );
		
		//------------------------------------------------
		// OutBlock1
		
		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		nBlockSize   = sizeof( CFOFQ02400OutBlock1 );
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		LPCFOFQ02400OutBlock1 pOutBlock1 = (LPCFOFQ02400OutBlock1)( lpData + nOffset );
		nOffset += nBlockSize;

		nCol = 0;
		m_ctrlOutBlock1.InsertItem( 0, "" );
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pOutBlock1->RecCnt    , sizeof( pOutBlock1->RecCnt     ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수  
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pOutBlock1->AcntNo    , sizeof( pOutBlock1->AcntNo     ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호    
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pOutBlock1->Pwd       , sizeof( pOutBlock1->Pwd        ), DATA_TYPE_STRING ) );    // [string,    8] 비밀번호    
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pOutBlock1->RegMktCode, sizeof( pOutBlock1->RegMktCode ), DATA_TYPE_STRING ) );    // [string,    2] 등록시장코드
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pOutBlock1->BuyDt     , sizeof( pOutBlock1->BuyDt      ), DATA_TYPE_STRING ) );    // [string,    8] 매수일자    
		
		//------------------------------------------------
		// OutBlock2
		
		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		nBlockSize   = sizeof( CFOFQ02400OutBlock2 );
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		LPCFOFQ02400OutBlock2 pOutBlock2 = (LPCFOFQ02400OutBlock2)( lpData + nOffset );
		nOffset += nBlockSize;

		nCol = 0;
		m_ctrlOutBlock2.InsertItem( 0, "" );
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->RecCnt         , sizeof( pOutBlock2->RecCnt          ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수      
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->AcntNm         , sizeof( pOutBlock2->AcntNm          ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명          
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->FutsCtrctQty   , sizeof( pOutBlock2->FutsCtrctQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물약정수량    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->OptCtrctQty    , sizeof( pOutBlock2->OptCtrctQty     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 옵션약정수량    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->CtrctQty       , sizeof( pOutBlock2->CtrctQty        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 약정수량        
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->FutsCtrctAmt   , sizeof( pOutBlock2->FutsCtrctAmt    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물약정금액    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->FutsBuyctrAmt  , sizeof( pOutBlock2->FutsBuyctrAmt   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매수약정금액
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->FutsSlctrAmt   , sizeof( pOutBlock2->FutsSlctrAmt    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매도약정금액
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->CalloptCtrctAmt, sizeof( pOutBlock2->CalloptCtrctAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜옵션약정금액  
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->CallBuyAmt     , sizeof( pOutBlock2->CallBuyAmt      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매수금액      
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->CallSellAmt    , sizeof( pOutBlock2->CallSellAmt     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매도금액      
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->PutoptCtrctAmt , sizeof( pOutBlock2->PutoptCtrctAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋옵션약정금액  
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->PutBuyAmt      , sizeof( pOutBlock2->PutBuyAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매수금액      
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->PutSellAmt     , sizeof( pOutBlock2->PutSellAmt      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매도금액      
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->AllCtrctAmt    , sizeof( pOutBlock2->AllCtrctAmt     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전체약정금액    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->BuyctrAsmAmt   , sizeof( pOutBlock2->BuyctrAsmAmt    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수약정누계금액
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->SlctrAsmAmt    , sizeof( pOutBlock2->SlctrAsmAmt     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도약정누계금액
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->FutsPnlSum     , sizeof( pOutBlock2->FutsPnlSum      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물손익합계    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->OptPnlSum      , sizeof( pOutBlock2->OptPnlSum       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 옵션손익합계    
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pOutBlock2->AllPnlSum      , sizeof( pOutBlock2->AllPnlSum       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전체손익합계    
		
		//------------------------------------------------
		// OutBlock3

		// Count 계산
		nBlockSize   = 5;
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}

		CopyMemory( szCount, lpData + nOffset, 5 );
		nCount = atoi( szCount );
		nOffset += nBlockSize;

		// Data
		nBlockSize   = sizeof( CFOFQ02400OutBlock3 ) * nCount;
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		LPCFOFQ02400OutBlock3 pOutBlock3 = (LPCFOFQ02400OutBlock3)( lpData + nOffset );
		nOffset += nBlockSize;

		for( int i=0; i<nCount; i++ )
		{
			nCol = 0;
			nRow = m_ctrlOutBlock3.InsertItem( m_ctrlOutBlock3.GetItemCount(), "" );
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].FnoClssCode, sizeof( pOutBlock3[i].FnoClssCode ), DATA_TYPE_STRING ) );    // [string,    1] 선물옵션품목구분
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].FutsSellQty, sizeof( pOutBlock3[i].FutsSellQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매도수량    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].FutsSellPnl, sizeof( pOutBlock3[i].FutsSellPnl ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매도손익    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].FutsBuyQty , sizeof( pOutBlock3[i].FutsBuyQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매수수량    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].FutsBuyPnl , sizeof( pOutBlock3[i].FutsBuyPnl  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물매수손익    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].CallSellQty, sizeof( pOutBlock3[i].CallSellQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매도수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].CallSellPnl, sizeof( pOutBlock3[i].CallSellPnl ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매도손익      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].CallBuyQty , sizeof( pOutBlock3[i].CallBuyQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매수수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].CallBuyPnl , sizeof( pOutBlock3[i].CallBuyPnl  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 콜매수손익      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].PutSellQty , sizeof( pOutBlock3[i].PutSellQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매도수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].PutSellPnl , sizeof( pOutBlock3[i].PutSellPnl  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매도손익      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].PutBuyQty  , sizeof( pOutBlock3[i].PutBuyQty   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매수수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pOutBlock3[i].PutBuyPnl  , sizeof( pOutBlock3[i].PutBuyPnl   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 풋매수손익      
		}
		
		//------------------------------------------------
		// OutBlock4
		
		// Count 계산
		nBlockSize   = 5;
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		CopyMemory( szCount, lpData + nOffset, 5 );
		nCount = atoi( szCount );
		nOffset += nBlockSize;
		
		// Data
		nBlockSize   = sizeof( CFOFQ02400OutBlock4 ) * nCount;
		nDataLength -= nBlockSize;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		LPCFOFQ02400OutBlock4 pOutBlock4 = (LPCFOFQ02400OutBlock4)( lpData + nOffset );
		nOffset += nBlockSize;
		int i = 0;
		for( i=0; i<nCount; i++ )
		{
			nCol = 0;
			nRow = m_ctrlOutBlock4.InsertItem( m_ctrlOutBlock4.GetItemCount(), "" );
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].IsuNo       , sizeof( pOutBlock4[i].IsuNo        ), DATA_TYPE_STRING ) );    // [string,   12] 종목번호    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].IsuNm       , sizeof( pOutBlock4[i].IsuNm        ), DATA_TYPE_STRING ) );    // [string,   40] 종목명      
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].BnsTpCode   , sizeof( pOutBlock4[i].BnsTpCode    ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].BnsTpNm     , sizeof( pOutBlock4[i].BnsTpNm      ), DATA_TYPE_STRING ) );    // [string,   10] 매매구분    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].BalQty      , sizeof( pOutBlock4[i].BalQty       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 잔고수량    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].FnoAvrPrc   , sizeof( pOutBlock4[i].FnoAvrPrc    ), DATA_TYPE_DOT, 8 ) );    // [double, 19.8] 평균가      
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].BgnAmt      , sizeof( pOutBlock4[i].BgnAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 당초금액    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].ThdayLqdtQty, sizeof( pOutBlock4[i].ThdayLqdtQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 당일청산수량
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].Curprc      , sizeof( pOutBlock4[i].Curprc       ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 현재가      
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].EvalAmt     , sizeof( pOutBlock4[i].EvalAmt      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 평가금액    
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].EvalPnlAmt  , sizeof( pOutBlock4[i].EvalPnlAmt   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 평가손익금액
			m_ctrlOutBlock4.SetItemText( nRow, nCol++, GetDispData( pOutBlock4[i].EvalErnrat  , sizeof( pOutBlock4[i].EvalErnrat   ), DATA_TYPE_DOT, 6 ) );    // [double, 12.6] 평가수익률  
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
LRESULT CDlg_CFOFQ02400::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		
	
	return 0L;
}
