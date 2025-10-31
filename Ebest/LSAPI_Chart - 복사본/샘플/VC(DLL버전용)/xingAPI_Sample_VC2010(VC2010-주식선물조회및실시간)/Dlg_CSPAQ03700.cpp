// Dlg_CSPAQ03700.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPAQ03700.h"

#include "./Packet/CSPAQ13700.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ03700 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPAQ03700, CDialog)

CDlg_CSPAQ03700::CDlg_CSPAQ03700(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPAQ03700::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPAQ03700)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPAQ03700::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPAQ03700)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
	DDX_Control(pDX, IDC_OUTBLOCK3, m_ctrlOutBlock3);
}


BEGIN_MESSAGE_MAP(CDlg_CSPAQ03700, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPAQ03700)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ03700 message handlers

BOOL CDlg_CSPAQ03700::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPAQ03700::OnButtonRequest() 
{
	RequestData();
}

void CDlg_CSPAQ03700::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ03700::InitCtrls()
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
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수   ", FALSE,    5 );    // [long  ,    5] 레코드갯수   
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호     ", TRUE ,   20 );    // [string,   20] 계좌번호     
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "입력비밀번호 ", TRUE ,    8 );    // [string,    8] 입력비밀번호 
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문시장코드 ", TRUE ,    2 );    // [string,    2] 주문시장코드 
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매매구분     ", TRUE ,    1 );    // [string,    1] 매매구분     
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "종목번호     ", TRUE ,   12 );    // [string,   12] 종목번호     
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "체결여부     ", TRUE ,    1 );    // [string,    1] 체결여부     
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문일       ", TRUE ,    8 );    // [string,    8] 주문일       
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "시작주문번호2", FALSE,   10 );    // [long  ,   10] 시작주문번호2
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "역순구분     ", TRUE ,    1 );    // [string,    1] 역순구분     
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문유형코드 ", TRUE ,    2 );    // [string,    2] 주문유형코드 
	
	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수  ", FALSE,    5 );    // [long  ,    5] 레코드갯수  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도체결금액", FALSE,   16 );    // [long  ,   16] 매도체결금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매수체결금액", FALSE,   16 );    // [long  ,   16] 매수체결금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도체결수량", FALSE,   16 );    // [long  ,   16] 매도체결수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매수체결수량", FALSE,   16 );    // [long  ,   16] 매수체결수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도주문수량", FALSE,   16 );    // [long  ,   16] 매도주문수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매수주문수량", FALSE,   16 );    // [long  ,   16] 매수주문수량
	
	//-------------------------------------------------------------------------
	// OutBlock3
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문일          ", TRUE ,    8 );    // [string,    8] 주문일          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "관리지점번호    ", TRUE ,    3 );    // [string,    3] 관리지점번호    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문시장코드    ", TRUE ,    2 );    // [string,    2] 주문시장코드    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문번호        ", FALSE,   10 );    // [long  ,   10] 주문번호        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "원주문번호      ", FALSE,   10 );    // [long  ,   10] 원주문번호      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "종목번호        ", TRUE ,   12 );    // [string,   12] 종목번호        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "종목명          ", TRUE ,   40 );    // [string,   40] 종목명          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매매구분        ", TRUE ,    1 );    // [string,    1] 매매구분        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매매구분        ", TRUE ,   10 );    // [string,   10] 매매구분        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문유형코드    ", TRUE ,    2 );    // [string,    2] 주문유형코드    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문유형명      ", TRUE ,   40 );    // [string,   40] 주문유형명      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문처리유형코드", FALSE,    9 );    // [long  ,    9] 주문처리유형코드
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문처리유형명  ", TRUE ,   50 );    // [string,   50] 주문처리유형명  
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "정정취소구분    ", TRUE ,    1 );    // [string,    1] 정정취소구분    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "정정취소구분명  ", TRUE ,   10 );    // [string,   10] 정정취소구분명  
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "정정취소수량    ", FALSE,   16 );    // [long  ,   16] 정정취소수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "정정취소가능수량", FALSE,   16 );    // [long  ,   16] 정정취소가능수량
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문수량        ", FALSE,   16 );    // [long  ,   16] 주문수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문가격        ", FALSE, 15.2 );    // [double, 15.2] 주문가격        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결수량        ", FALSE,   16 );    // [long  ,   16] 체결수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결가          ", FALSE, 15.2 );    // [double, 15.2] 체결가          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결처리시각    ", TRUE ,    9 );    // [string,    9] 체결처리시각    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "최종체결시각    ", TRUE ,    9 );    // [string,    9] 최종체결시각    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "호가유형코드    ", TRUE ,    2 );    // [string,    2] 호가유형코드    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "호가유형명      ", TRUE ,   40 );    // [string,   40] 호가유형명      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문조건구분    ", TRUE ,    1 );    // [string,    1] 주문조건구분    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전체체결수량    ", FALSE,   16 );    // [long  ,   16] 전체체결수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "통신매체코드    ", TRUE ,    2 );    // [string,    2] 통신매체코드    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "통신매체명      ", TRUE ,   40 );    // [string,   40] 통신매체명      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "회원번호        ", TRUE ,    3 );    // [string,    3] 회원번호        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "예약주문여부    ", TRUE ,    1 );    // [string,    1] 예약주문여부    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "대출일          ", TRUE ,    8 );    // [string,    8] 대출일          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문시각        ", TRUE ,    9 );    // [string,    9] 주문시각        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "운용지시번호    ", TRUE ,   12 );    // [string,   12] 운용지시번호    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문자ID        ", TRUE ,   16 );    // [string,   16] 주문자ID        
	
	ZeroMemory( m_szContKey, sizeof( m_szContKey ) );
	GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( FALSE );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ03700::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	CSPAQ13700InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPAQ13700";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo     ;		GetDlgItemText( IDC_INBLOCK_ACNTNO     , strAcntNo      );    // [string,   20] 계좌번호     
    CString strInptPwd    ;		GetDlgItemText( IDC_INBLOCK_INPTPWD    , strInptPwd     );    // [string,    8] 입력비밀번호 
    CString strOrdMktCode ;		GetDlgItemText( IDC_INBLOCK_ORDMKTCODE , strOrdMktCode  );    // [string,    2] 주문시장코드 
    CString strBnsTpCode  ;		GetDlgItemText( IDC_INBLOCK_BNSTPCODE  , strBnsTpCode   );    // [string,    1] 매매구분     
    CString strIsuNo      ;		GetDlgItemText( IDC_INBLOCK_ISUNO      , strIsuNo       );    // [string,   12] 종목번호     
    CString strExecYn     ;		GetDlgItemText( IDC_INBLOCK_EXECYN     , strExecYn      );    // [string,    1] 체결여부     
    CString strOrdDt      ;		GetDlgItemText( IDC_INBLOCK_ORDDT      , strOrdDt       );    // [string,    8] 주문일       
    CString strSrtOrdNo2  ;		GetDlgItemText( IDC_INBLOCK_SRTORDNO2  , strSrtOrdNo2   );    // [long  ,   10] 시작주문번호2
    CString strBkseqTpCode;		GetDlgItemText( IDC_INBLOCK_BKSEQTPCODE, strBkseqTpCode );    // [string,    1] 역순구분     
    CString strOrdPtnCode ;		GetDlgItemText( IDC_INBLOCK_ORDPTNCODE , strOrdPtnCode  );    // [string,    2] 주문유형코드 

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt     , sizeof( pckInBlock.RecCnt      ), "1"           , DATA_TYPE_LONG   );    // [long  ,    5] 레코드갯수   
	SetPacketData( pckInBlock.AcntNo     , sizeof( pckInBlock.AcntNo      ), strAcntNo     , DATA_TYPE_STRING );    // [string,   20] 계좌번호     
	SetPacketData( pckInBlock.InptPwd    , sizeof( pckInBlock.InptPwd     ), strInptPwd    , DATA_TYPE_STRING );    // [string,    8] 입력비밀번호 
	SetPacketData( pckInBlock.OrdMktCode , sizeof( pckInBlock.OrdMktCode  ), strOrdMktCode , DATA_TYPE_STRING );    // [string,    2] 주문시장코드 
	SetPacketData( pckInBlock.BnsTpCode  , sizeof( pckInBlock.BnsTpCode   ), strBnsTpCode  , DATA_TYPE_STRING );    // [string,    1] 매매구분     
	SetPacketData( pckInBlock.IsuNo      , sizeof( pckInBlock.IsuNo       ), strIsuNo      , DATA_TYPE_STRING );    // [string,   12] 종목번호     
	SetPacketData( pckInBlock.ExecYn     , sizeof( pckInBlock.ExecYn      ), strExecYn     , DATA_TYPE_STRING );    // [string,    1] 체결여부     
	SetPacketData( pckInBlock.OrdDt      , sizeof( pckInBlock.OrdDt       ), strOrdDt      , DATA_TYPE_STRING );    // [string,    8] 주문일       
	SetPacketData( pckInBlock.SrtOrdNo2  , sizeof( pckInBlock.SrtOrdNo2   ), strSrtOrdNo2  , DATA_TYPE_LONG   );    // [long  ,   10] 시작주문번호2
	SetPacketData( pckInBlock.BkseqTpCode, sizeof( pckInBlock.BkseqTpCode ), strBkseqTpCode, DATA_TYPE_STRING );    // [string,    1] 역순구분     
	SetPacketData( pckInBlock.OrdPtnCode , sizeof( pckInBlock.OrdPtnCode  ), strOrdPtnCode , DATA_TYPE_STRING );    // [string,    2] 주문유형코드 

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
	}

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
LRESULT CDlg_CSPAQ03700::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		typedef struct
		{
			CSPAQ13700OutBlock1	outBlock1;
			CSPAQ13700OutBlock2	outBlock2;
			char				sCountOutBlock3[5];
			CSPAQ13700OutBlock3	outBlock3[1];
		} CSPAQ13700AllOutBlock, *LPCSPAQ13700AllOutBlock;

		LPRECV_PACKET			pRpData		 = (LPRECV_PACKET)lParam;
		LPCSPAQ13700AllOutBlock pAllOutBlock = (LPCSPAQ13700AllOutBlock)pRpData->lpData;
		int						nDataLength  = pRpData->nDataLength;
		int						nCol;
		int						nRow;
		int						nCount;
		char					szCount[6] = { 0 };

		// 연속키를 저장한다.
		strcpy_s( m_szContKey, pRpData->szContKey );
		GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( pRpData->cCont[0] == 'Y' );

		//------------------------------------------------
		// OutBlock1
		
		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		nDataLength -= sizeof( CSPAQ13700OutBlock1 );
		if( nDataLength < 0 )
		{
			return 0L;
		}

		nCol = 0;
		m_ctrlOutBlock1.InsertItem( 0, "" );
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt     , sizeof( pAllOutBlock->outBlock1.RecCnt      ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수   
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo     , sizeof( pAllOutBlock->outBlock1.AcntNo      ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호     
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.InptPwd    , sizeof( pAllOutBlock->outBlock1.InptPwd     ), DATA_TYPE_STRING ) );    // [string,    8] 입력비밀번호 
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdMktCode , sizeof( pAllOutBlock->outBlock1.OrdMktCode  ), DATA_TYPE_STRING ) );    // [string,    2] 주문시장코드 
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BnsTpCode  , sizeof( pAllOutBlock->outBlock1.BnsTpCode   ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분     
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.IsuNo      , sizeof( pAllOutBlock->outBlock1.IsuNo       ), DATA_TYPE_STRING ) );    // [string,   12] 종목번호     
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.ExecYn     , sizeof( pAllOutBlock->outBlock1.ExecYn      ), DATA_TYPE_STRING ) );    // [string,    1] 체결여부     
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdDt      , sizeof( pAllOutBlock->outBlock1.OrdDt       ), DATA_TYPE_STRING ) );    // [string,    8] 주문일       
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.SrtOrdNo2  , sizeof( pAllOutBlock->outBlock1.SrtOrdNo2   ), DATA_TYPE_LONG   ) );    // [long  ,   10] 시작주문번호2
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BkseqTpCode, sizeof( pAllOutBlock->outBlock1.BkseqTpCode ), DATA_TYPE_STRING ) );    // [string,    1] 역순구분     
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdPtnCode , sizeof( pAllOutBlock->outBlock1.OrdPtnCode  ), DATA_TYPE_STRING ) );    // [string,    2] 주문유형코드 
		
		//------------------------------------------------
		// OutBlock2
		nDataLength -= sizeof( CSPAQ13700OutBlock2 );
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		nCol = 0;
		m_ctrlOutBlock2.InsertItem( 0, "" );
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt     , sizeof( pAllOutBlock->outBlock2.RecCnt      ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수       
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SellExecAmt, sizeof( pAllOutBlock->outBlock2.SellExecAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도체결금액     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BuyExecAmt , sizeof( pAllOutBlock->outBlock2.BuyExecAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수체결금액     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SellExecQty, sizeof( pAllOutBlock->outBlock2.SellExecQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도체결수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BuyExecQty , sizeof( pAllOutBlock->outBlock2.BuyExecQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수체결수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SellOrdQty , sizeof( pAllOutBlock->outBlock2.SellOrdQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도주문수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BuyOrdQty  , sizeof( pAllOutBlock->outBlock2.BuyOrdQty   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수주문수량     
		
		//------------------------------------------------
		// OutBlock3
		
		// Count 계산
		nDataLength -= 5;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		CopyMemory( szCount, pAllOutBlock->sCountOutBlock3, 5 );
		nCount = atoi( szCount );
		
		// Data
		nDataLength -= sizeof( CSPAQ13700OutBlock3 ) * nCount;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		for( int i=0; i<nCount; i++ )
		{
			nCol = 0;
			nRow = m_ctrlOutBlock3.InsertItem( m_ctrlOutBlock3.GetItemCount(), "" );
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdDt        , sizeof( pAllOutBlock->outBlock3[i].OrdDt         ), DATA_TYPE_STRING ) );    // [string,    8] 주문일              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MgmtBrnNo    , sizeof( pAllOutBlock->outBlock3[i].MgmtBrnNo     ), DATA_TYPE_STRING ) );    // [string,    3] 관리지점번호        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdMktCode   , sizeof( pAllOutBlock->outBlock3[i].OrdMktCode    ), DATA_TYPE_STRING ) );    // [string,    2] 주문시장코드        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdNo        , sizeof( pAllOutBlock->outBlock3[i].OrdNo         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 주문번호            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrgOrdNo     , sizeof( pAllOutBlock->outBlock3[i].OrgOrdNo      ), DATA_TYPE_LONG   ) );    // [long  ,   10] 원주문번호          
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].IsuNo        , sizeof( pAllOutBlock->outBlock3[i].IsuNo         ), DATA_TYPE_STRING ) );    // [string,   12] 종목번호            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].IsuNm        , sizeof( pAllOutBlock->outBlock3[i].IsuNm         ), DATA_TYPE_STRING ) );    // [string,   40] 종목명              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BnsTpCode    , sizeof( pAllOutBlock->outBlock3[i].BnsTpCode     ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BnsTpNm      , sizeof( pAllOutBlock->outBlock3[i].BnsTpNm       ), DATA_TYPE_STRING ) );    // [string,   10] 매매구분            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdPtnCode   , sizeof( pAllOutBlock->outBlock3[i].OrdPtnCode    ), DATA_TYPE_STRING ) );    // [string,    2] 주문유형코드        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdPtnNm     , sizeof( pAllOutBlock->outBlock3[i].OrdPtnNm      ), DATA_TYPE_STRING ) );    // [string,   40] 주문유형명          
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdTrxPtnCode, sizeof( pAllOutBlock->outBlock3[i].OrdTrxPtnCode ), DATA_TYPE_LONG   ) );    // [long  ,    9] 주문처리유형코드    
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdTrxPtnNm  , sizeof( pAllOutBlock->outBlock3[i].OrdTrxPtnNm   ), DATA_TYPE_STRING ) );    // [string,   50] 주문처리유형명      
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MrcTpCode    , sizeof( pAllOutBlock->outBlock3[i].MrcTpCode     ), DATA_TYPE_STRING ) );    // [string,    1] 정정취소구분        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MrcTpNm      , sizeof( pAllOutBlock->outBlock3[i].MrcTpNm       ), DATA_TYPE_STRING ) );    // [string,   10] 정정취소구분명      
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MrcQty       , sizeof( pAllOutBlock->outBlock3[i].MrcQty        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 정정취소수량        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MrcAbleQty   , sizeof( pAllOutBlock->outBlock3[i].MrcAbleQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 정정취소가능수량    
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdQty       , sizeof( pAllOutBlock->outBlock3[i].OrdQty        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문수량            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdPrc       , sizeof( pAllOutBlock->outBlock3[i].OrdPrc        ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 주문가격            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecQty      , sizeof( pAllOutBlock->outBlock3[i].ExecQty       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 체결수량            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecPrc      , sizeof( pAllOutBlock->outBlock3[i].ExecPrc       ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 체결가              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecTrxTime  , sizeof( pAllOutBlock->outBlock3[i].ExecTrxTime   ), DATA_TYPE_STRING ) );    // [string,    9] 체결처리시각        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].LastExecTime , sizeof( pAllOutBlock->outBlock3[i].LastExecTime  ), DATA_TYPE_STRING ) );    // [string,    9] 최종체결시각        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdprcPtnCode, sizeof( pAllOutBlock->outBlock3[i].OrdprcPtnCode ), DATA_TYPE_STRING ) );    // [string,    2] 호가유형코드        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdprcPtnNm  , sizeof( pAllOutBlock->outBlock3[i].OrdprcPtnNm   ), DATA_TYPE_STRING ) );    // [string,   40] 호가유형명          
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdCndiTpCode, sizeof( pAllOutBlock->outBlock3[i].OrdCndiTpCode ), DATA_TYPE_STRING ) );    // [string,    1] 주문조건구분        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].AllExecQty   , sizeof( pAllOutBlock->outBlock3[i].AllExecQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전체체결수량        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].RegCommdaCode, sizeof( pAllOutBlock->outBlock3[i].RegCommdaCode ), DATA_TYPE_STRING ) );    // [string,    2] 통신매체코드        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CommdaNm     , sizeof( pAllOutBlock->outBlock3[i].CommdaNm      ), DATA_TYPE_STRING ) );    // [string,   40] 통신매체명          
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MbrNo        , sizeof( pAllOutBlock->outBlock3[i].MbrNo         ), DATA_TYPE_STRING ) );    // [string,    3] 회원번호            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].RsvOrdYn     , sizeof( pAllOutBlock->outBlock3[i].RsvOrdYn      ), DATA_TYPE_STRING ) );    // [string,    1] 예약주문여부        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].LoanDt       , sizeof( pAllOutBlock->outBlock3[i].LoanDt        ), DATA_TYPE_STRING ) );    // [string,    8] 대출일              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdTime      , sizeof( pAllOutBlock->outBlock3[i].OrdTime       ), DATA_TYPE_STRING ) );    // [string,    9] 주문시각            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OpDrtnNo     , sizeof( pAllOutBlock->outBlock3[i].OpDrtnNo      ), DATA_TYPE_STRING ) );    // [string,   12] 운용지시번호        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OdrrId       , sizeof( pAllOutBlock->outBlock3[i].OdrrId        ), DATA_TYPE_STRING ) );    // [string,   16] 주문자ID            
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
LRESULT CDlg_CSPAQ03700::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
