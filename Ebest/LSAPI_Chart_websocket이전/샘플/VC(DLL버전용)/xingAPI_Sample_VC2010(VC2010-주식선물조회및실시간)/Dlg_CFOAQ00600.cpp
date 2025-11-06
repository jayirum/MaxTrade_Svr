// Dlg_CFOAQ00600.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CFOAQ00600.h"

#include "./Packet/CFOAQ00600.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOAQ00600 dialog


IMPLEMENT_DYNCREATE(CDlg_CFOAQ00600, CDialog)

CDlg_CFOAQ00600::CDlg_CFOAQ00600(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CFOAQ00600::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CFOAQ00600)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CFOAQ00600::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CFOAQ00600)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
	DDX_Control(pDX, IDC_OUTBLOCK3, m_ctrlOutBlock3);
}


BEGIN_MESSAGE_MAP(CDlg_CFOAQ00600, CDialog)
	//{{AFX_MSG_MAP(CDlg_CFOAQ00600)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CFOAQ00600 message handlers

BOOL CDlg_CFOAQ00600::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CFOAQ00600::OnButtonRequest() 
{
	RequestData();
}

void CDlg_CFOAQ00600::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CFOAQ00600::InitCtrls()
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
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호        ", TRUE ,   20 );    // [string,   20] 계좌번호        
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "입력비밀번호    ", TRUE ,    8 );    // [string,    8] 입력비밀번호    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "조회시작일      ", TRUE ,    8 );    // [string,    8] 조회시작일      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "조회종료일      ", TRUE ,    8 );    // [string,    8] 조회종료일      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물옵션분류코드", TRUE ,    2 );    // [string,    2] 선물옵션분류코드
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "상품군코드      ", TRUE ,    2 );    // [string,    2] 상품군코드      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "체결구분        ", TRUE ,    1 );    // [string,    1] 체결구분        
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "정렬순서구분    ", TRUE ,    1 );    // [string,    1] 정렬순서구분    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "통신매체코드    ", TRUE ,    2 );    // [string,    2] 통신매체코드    
	
	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수  ", FALSE,    5 );    // [long  ,    5] 레코드갯수  
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명      ", TRUE ,   40 );    // [string,   40] 계좌명      
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물주문수량", FALSE,   16 );    // [long  ,   16] 선물주문수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "선물체결수량", FALSE,   16 );    // [long  ,   16] 선물체결수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "옵션주문수량", FALSE,   16 );    // [long  ,   16] 옵션주문수량
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "옵션체결수량", FALSE,   16 );    // [long  ,   16] 옵션체결수량
	
	//-------------------------------------------------------------------------
	// OutBlock3
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문일              ", TRUE ,    8 );    // [string,    8] 주문일              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문번호            ", FALSE,   10 );    // [long  ,   10] 주문번호            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "원주문번호          ", FALSE,   10 );    // [long  ,   10] 원주문번호          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문시각            ", TRUE ,    9 );    // [string,    9] 주문시각            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물옵션종목번호    ", TRUE ,   12 );    // [string,   12] 선물옵션종목번호    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "종목명              ", TRUE ,   40 );    // [string,   40] 종목명              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매매구분            ", TRUE ,   10 );    // [string,   10] 매매구분            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "정정취소구분명      ", TRUE ,   10 );    // [string,   10] 정정취소구분명      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물옵션호가유형코드", TRUE ,    2 );    // [string,    2] 선물옵션호가유형코드
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "선물옵션호가유형명  ", TRUE ,   40 );    // [string,   40] 선물옵션호가유형명  
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문가              ", FALSE, 13.2 );    // [double, 13.2] 주문가              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문수량            ", FALSE,   16 );    // [long  ,   16] 주문수량            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문구분명          ", TRUE ,   10 );    // [string,   10] 주문구분명          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결구분명          ", TRUE ,   10 );    // [string,   10] 체결구분명          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결가              ", FALSE, 13.2 );    // [double, 13.2] 체결가              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결수량            ", FALSE,   16 );    // [long  ,   16] 체결수량            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "약정시각            ", TRUE ,    9 );    // [string,    9] 약정시각            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "약정번호            ", FALSE,   10 );    // [long  ,   10] 약정번호            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "체결번호            ", FALSE,   10 );    // [long  ,   10] 체결번호            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매매손익금액        ", FALSE,   16 );    // [long  ,   16] 매매손익금액        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "미체결수량          ", FALSE,   16 );    // [long  ,   16] 미체결수량          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "사용자ID            ", TRUE ,   16 );    // [string,   16] 사용자ID            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "통신매체코드        ", TRUE ,    2 );    // [string,    2] 통신매체코드        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "통신매체코드명      ", TRUE ,   40 );    // [string,   40] 통신매체코드명      
	
	ZeroMemory( m_szContKey, sizeof( m_szContKey ) );
	GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( FALSE );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CFOAQ00600::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	CFOAQ00600InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CFOAQ00600";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo        ;		GetDlgItemText( IDC_INBLOCK_ACNTNO        , strAcntNo         );    // 계좌번호        
    CString strInptPwd       ;		GetDlgItemText( IDC_INBLOCK_INPTPWD       , strInptPwd        );    // 입력비밀번호    
    CString strQrySrtDt      ;		GetDlgItemText( IDC_INBLOCK_QRYSRTDT      , strQrySrtDt       );    // 조회시작일      
    CString strQryEndDt      ;		GetDlgItemText( IDC_INBLOCK_QRYENDDT      , strQryEndDt       );    // 조회종료일      
    CString strPrdgrpCode    ;		GetDlgItemText( IDC_INBLOCK_PRDGRPCODE    , strPrdgrpCode     );    // 상품군코드      
    CString strPrdtExecTpCode;		GetDlgItemText( IDC_INBLOCK_PRDTEXECTPCODE, strPrdtExecTpCode );    // 체결구분        
    CString strStnlnSeqTp    ;		GetDlgItemText( IDC_INBLOCK_STNLNSEQTP    , strStnlnSeqTp     );    // 정렬순서구분    

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt        , sizeof( pckInBlock.RecCnt         ), "1"              , DATA_TYPE_LONG   );    // [long  ,    5] 레코드갯수      
	SetPacketData( pckInBlock.AcntNo        , sizeof( pckInBlock.AcntNo         ), strAcntNo        , DATA_TYPE_STRING );    // [string,   20] 계좌번호        
	SetPacketData( pckInBlock.InptPwd       , sizeof( pckInBlock.InptPwd        ), strInptPwd       , DATA_TYPE_STRING );    // [string,    8] 입력비밀번호    
	SetPacketData( pckInBlock.QrySrtDt      , sizeof( pckInBlock.QrySrtDt       ), strQrySrtDt      , DATA_TYPE_STRING );    // [string,    8] 조회시작일      
	SetPacketData( pckInBlock.QryEndDt      , sizeof( pckInBlock.QryEndDt       ), strQryEndDt      , DATA_TYPE_STRING );    // [string,    8] 조회종료일      
	SetPacketData( pckInBlock.FnoClssCode   , sizeof( pckInBlock.FnoClssCode    ), "00"             , DATA_TYPE_STRING );    // [string,    2] 선물옵션분류코드
	SetPacketData( pckInBlock.PrdgrpCode    , sizeof( pckInBlock.PrdgrpCode     ), strPrdgrpCode    , DATA_TYPE_STRING );    // [string,    2] 상품군코드      
	SetPacketData( pckInBlock.PrdtExecTpCode, sizeof( pckInBlock.PrdtExecTpCode ), strPrdtExecTpCode, DATA_TYPE_STRING );    // [string,    1] 체결구분        
	SetPacketData( pckInBlock.StnlnSeqTp    , sizeof( pckInBlock.StnlnSeqTp     ), strStnlnSeqTp    , DATA_TYPE_STRING );    // [string,    1] 정렬순서구분    
	SetPacketData( pckInBlock.CommdaCode    , sizeof( pckInBlock.CommdaCode     ), "99"             , DATA_TYPE_STRING );    // [string,    2] 통신매체코드    

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
		CString strMsg;
		strMsg.Format( "[%d] %s", nRqID, g_iXingAPI.GetErrorMessage( nRqID ) );
		MessageBox( strMsg, "조회실패", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_CFOAQ00600::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		typedef struct
		{
			CFOAQ00600OutBlock1	outBlock1;
			CFOAQ00600OutBlock2	outBlock2;
			char				sCountOutBlock3[5];
			CFOAQ00600OutBlock3	outBlock3[1];
		} CFOAQ00600AllOutBlock, *LPCFOAQ00600AllOutBlock;

		LPRECV_PACKET			pRpData		 = (LPRECV_PACKET)lParam;
		LPCFOAQ00600AllOutBlock pAllOutBlock = (LPCFOAQ00600AllOutBlock)pRpData->lpData;
		int						nDataLength  = pRpData->nDataLength;
		int						nCol;
		int						nRow;
		int						nCount;
		char					szCount[6] = { 0 };

		// 연속키를 저장한다.
		strcpy_s( m_szContKey, sizeof(pRpData->szContKey), pRpData->szContKey );
		GetDlgItem( IDC_BUTTON_REQUEST_NEXT )->EnableWindow( pRpData->cCont[0] == 'Y' );

		//------------------------------------------------
		// OutBlock1
		
		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		nDataLength -= sizeof( CFOAQ00600OutBlock1 );
		if( nDataLength < 0 )
		{
			return 0L;
		}

		nCol = 0;
		m_ctrlOutBlock1.InsertItem( 0, "" );
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt        , sizeof( pAllOutBlock->outBlock1.RecCnt         ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo        , sizeof( pAllOutBlock->outBlock1.AcntNo         ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호        
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.InptPwd       , sizeof( pAllOutBlock->outBlock1.InptPwd        ), DATA_TYPE_STRING ) );    // [string,    8] 입력비밀번호    
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.QrySrtDt      , sizeof( pAllOutBlock->outBlock1.QrySrtDt       ), DATA_TYPE_STRING ) );    // [string,    8] 조회시작일      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.QryEndDt      , sizeof( pAllOutBlock->outBlock1.QryEndDt       ), DATA_TYPE_STRING ) );    // [string,    8] 조회종료일      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.FnoClssCode   , sizeof( pAllOutBlock->outBlock1.FnoClssCode    ), DATA_TYPE_STRING ) );    // [string,    2] 선물옵션분류코드
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.PrdgrpCode    , sizeof( pAllOutBlock->outBlock1.PrdgrpCode     ), DATA_TYPE_STRING ) );    // [string,    2] 상품군코드      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.PrdtExecTpCode, sizeof( pAllOutBlock->outBlock1.PrdtExecTpCode ), DATA_TYPE_STRING ) );    // [string,    1] 체결구분        
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.StnlnSeqTp    , sizeof( pAllOutBlock->outBlock1.StnlnSeqTp     ), DATA_TYPE_STRING ) );    // [string,    1] 정렬순서구분    
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CommdaCode    , sizeof( pAllOutBlock->outBlock1.CommdaCode     ), DATA_TYPE_STRING ) );    // [string,    2] 통신매체코드    
		
		//------------------------------------------------
		// OutBlock2
		nDataLength -= sizeof( CFOAQ00600OutBlock2 );
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		nCol = 0;
		m_ctrlOutBlock2.InsertItem( 0, "" );
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt     , sizeof( pAllOutBlock->outBlock2.RecCnt      ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수       
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm     , sizeof( pAllOutBlock->outBlock2.AcntNm      ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명           
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.FutsOrdQty , sizeof( pAllOutBlock->outBlock2.FutsOrdQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물주문수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.FutsExecQty, sizeof( pAllOutBlock->outBlock2.FutsExecQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 선물체결수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OptOrdQty  , sizeof( pAllOutBlock->outBlock2.OptOrdQty   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 옵션주문수량     
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OptExecQty , sizeof( pAllOutBlock->outBlock2.OptExecQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 옵션체결수량     
		
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
		nDataLength -= sizeof( CFOAQ00600OutBlock3 ) * nCount;
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		for( int i=0; i<nCount; i++ )
		{
			nCol = 0;
			nRow = m_ctrlOutBlock3.InsertItem( m_ctrlOutBlock3.GetItemCount(), "" );
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdDt           , sizeof( pAllOutBlock->outBlock3[i].OrdDt            ), DATA_TYPE_STRING ) );    // [string,    8] 주문일                  
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdNo           , sizeof( pAllOutBlock->outBlock3[i].OrdNo            ), DATA_TYPE_LONG   ) );    // [long  ,   10] 주문번호                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrgOrdNo        , sizeof( pAllOutBlock->outBlock3[i].OrgOrdNo         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 원주문번호              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdTime         , sizeof( pAllOutBlock->outBlock3[i].OrdTime          ), DATA_TYPE_STRING ) );    // [string,    9] 주문시각                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].FnoIsuNo        , sizeof( pAllOutBlock->outBlock3[i].FnoIsuNo         ), DATA_TYPE_STRING ) );    // [string,   12] 선물옵션종목번호        
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].IsuNm           , sizeof( pAllOutBlock->outBlock3[i].IsuNm            ), DATA_TYPE_STRING ) );    // [string,   40] 종목명                  
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BnsTpNm         , sizeof( pAllOutBlock->outBlock3[i].BnsTpNm          ), DATA_TYPE_STRING ) );    // [string,   10] 매매구분                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MrcTpNm         , sizeof( pAllOutBlock->outBlock3[i].MrcTpNm          ), DATA_TYPE_STRING ) );    // [string,   10] 정정취소구분명          
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].FnoOrdprcPtnCode, sizeof( pAllOutBlock->outBlock3[i].FnoOrdprcPtnCode ), DATA_TYPE_STRING ) );    // [string,    2] 선물옵션호가유형코드    
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].FnoOrdprcPtnNm  , sizeof( pAllOutBlock->outBlock3[i].FnoOrdprcPtnNm   ), DATA_TYPE_STRING ) );    // [string,   40] 선물옵션호가유형명      
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdPrc          , sizeof( pAllOutBlock->outBlock3[i].OrdPrc           ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 주문가                  
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdQty          , sizeof( pAllOutBlock->outBlock3[i].OrdQty           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문수량                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdTpNm         , sizeof( pAllOutBlock->outBlock3[i].OrdTpNm          ), DATA_TYPE_STRING ) );    // [string,   10] 주문구분명              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecTpNm        , sizeof( pAllOutBlock->outBlock3[i].ExecTpNm         ), DATA_TYPE_STRING ) );    // [string,   10] 체결구분명              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecPrc         , sizeof( pAllOutBlock->outBlock3[i].ExecPrc          ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 체결가                  
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecQty         , sizeof( pAllOutBlock->outBlock3[i].ExecQty          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 체결수량                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CtrctTime       , sizeof( pAllOutBlock->outBlock3[i].CtrctTime        ), DATA_TYPE_STRING ) );    // [string,    9] 약정시각                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CtrctNo         , sizeof( pAllOutBlock->outBlock3[i].CtrctNo          ), DATA_TYPE_LONG   ) );    // [long  ,   10] 약정번호                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].ExecNo          , sizeof( pAllOutBlock->outBlock3[i].ExecNo           ), DATA_TYPE_LONG   ) );    // [long  ,   10] 체결번호                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BnsplAmt        , sizeof( pAllOutBlock->outBlock3[i].BnsplAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매매손익금액            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].UnercQty        , sizeof( pAllOutBlock->outBlock3[i].UnercQty         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 미체결수량              
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].UserId          , sizeof( pAllOutBlock->outBlock3[i].UserId           ), DATA_TYPE_STRING ) );    // [string,   16] 사용자ID                
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CommdaCode      , sizeof( pAllOutBlock->outBlock3[i].CommdaCode       ), DATA_TYPE_STRING ) );    // [string,    2] 통신매체코드            
			m_ctrlOutBlock3.SetItemText(  nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CommdaCodeNm    , sizeof( pAllOutBlock->outBlock3[i].CommdaCodeNm     ), DATA_TYPE_STRING ) );    // [string,   40] 통신매체코드명          
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
LRESULT CDlg_CFOAQ00600::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
