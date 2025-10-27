// Dlg_CSPAQ02200.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPAQ02200.h"

#include "./Packet/CSPAQ12200.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ02200 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPAQ02200, CDialog)

CDlg_CSPAQ02200::CDlg_CSPAQ02200(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPAQ02200::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPAQ02200)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPAQ02200::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPAQ02200)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
}


BEGIN_MESSAGE_MAP(CDlg_CSPAQ02200, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPAQ02200)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ02200 message handlers

BOOL CDlg_CSPAQ02200::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPAQ02200::OnButtonRequest() 
{
	RequestData();
}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ02200::InitCtrls()
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
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수  ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "관리지점번호", TRUE ,    3 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호    ", TRUE ,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "비밀번호    ", TRUE ,    8 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "잔고생성구분", TRUE ,    1 );

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수                   ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "지점명                       ", TRUE ,   40 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명                       ", TRUE ,   40 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문가능금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "출금가능금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "거래소금액                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "코스닥금액                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "잔고평가금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "미수금액                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예탁자산총액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "손익율                       ", FALSE, 18.6 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "투자원금                     ", FALSE,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "투자손익금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보주문금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예수금                       ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용금액                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1예수금                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2예수금                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금미수금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금현금                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금대용                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "수표금액                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용주문가능금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트주문가능금액", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률35%주문가능금액      ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률50%주문가능금액      ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일매도정산금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일매수정산금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일매도정산금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일매수정산금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1연체변제소요금액           ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2연체변제소요금액           ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1추정인출가능금액           ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2추정인출가능금액           ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예탁담보대출금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용설정보증금               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "융자금액                     ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "변경후담보비율               ", FALSE,  9.3 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "원담보금액                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "부담보금액                   ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "소요담보금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "원담보부족금액               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "담보부족금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추가담보현금                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1주문가능금액               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용이자미납금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "기타대여금액                 ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "익일추정반대매매금액         ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "원담보합계금액               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용주문가능금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "부담보합계금액               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보금현금               ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보대용금액             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추가신용담보현금             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보재사용금액           ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "추가신용담보대용             ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도대금담보대출금액         ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "처분제한금액                 ", FALSE,   16 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ02200::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	CSPAQ12200InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPAQ12200";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo ;	GetDlgItemText( IDC_INBLOCK_ACNTNO, strAcntNo );		// 계좌번호                       StartPos 5, Length 20
    CString strPwd;		GetDlgItemText( IDC_INBLOCK_PWD   , strPwd    );		// 입력비밀번호                   StartPos 25, Length 8

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt   , sizeof( pckInBlock.RecCnt    ), "1"      , DATA_TYPE_LONG   );		// [long  ,    5] 레코드갯수                      StartPos 0, Length 5         
	SetPacketData( pckInBlock.MgmtBrnNo, sizeof( pckInBlock.MgmtBrnNo ), "0"      , DATA_TYPE_STRING );		// [string,    3] 관리지점번호                    StartPos 5, Length 3         
	SetPacketData( pckInBlock.AcntNo   , sizeof( pckInBlock.AcntNo    ), strAcntNo, DATA_TYPE_STRING );		// [string,   20] 계좌번호                        StartPos 8, Length 20         
	SetPacketData( pckInBlock.Pwd      , sizeof( pckInBlock.Pwd       ), strPwd   , DATA_TYPE_STRING );		// [string,    8] 비밀번호                        StartPos 28, Length 8         
	SetPacketData( pckInBlock.BalCreTp , sizeof( pckInBlock.BalCreTp  ), "0"      , DATA_TYPE_STRING );		// [string,    1] 잔고생성구분                    StartPos 36, Length 1         

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
LRESULT CDlg_CSPAQ02200::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		typedef struct
		{
			CSPAQ12200OutBlock1	outBlock1;
			CSPAQ12200OutBlock2	outBlock2;
		} CSPAQ12200AllOutBlock, *LPCSPAQ12200AllOutBlock;

		LPCSPAQ12200AllOutBlock pAllOutBlock = (LPCSPAQ12200AllOutBlock)pRpData->lpData;

		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		if( pRpData->nDataLength >= sizeof( CSPAQ12200OutBlock1 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock1.InsertItem( 0, "" );
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt   , sizeof( pAllOutBlock->outBlock1.RecCnt    ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.MgmtBrnNo, sizeof( pAllOutBlock->outBlock1.MgmtBrnNo ), DATA_TYPE_STRING ) );    // [string,    3] 관리지점번호        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo   , sizeof( pAllOutBlock->outBlock1.AcntNo    ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.Pwd      , sizeof( pAllOutBlock->outBlock1.Pwd       ), DATA_TYPE_STRING ) );    // [string,    8] 비밀번호            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BalCreTp , sizeof( pAllOutBlock->outBlock1.BalCreTp  ), DATA_TYPE_STRING ) );    // [string,    1] 잔고생성구분        
		}

		if( pRpData->nDataLength >= sizeof( CSPAQ12200OutBlock1 ) + sizeof( CSPAQ12200OutBlock2 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock2.InsertItem( 0, "" );
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt                , sizeof( pAllOutBlock->outBlock2.RecCnt                 ), DATA_TYPE_LONG      ) );    // [long  ,    5] 레코드갯수                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BrnNm                 , sizeof( pAllOutBlock->outBlock2.BrnNm                  ), DATA_TYPE_STRING    ) );    // [string,   40] 지점명                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm                , sizeof( pAllOutBlock->outBlock2.AcntNm                 ), DATA_TYPE_STRING    ) );    // [string,   40] 계좌명                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAbleAmt         , sizeof( pAllOutBlock->outBlock2.MnyOrdAbleAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 현금주문가능금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyoutAbleAmt         , sizeof( pAllOutBlock->outBlock2.MnyoutAbleAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 출금가능금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SeOrdAbleAmt          , sizeof( pAllOutBlock->outBlock2.SeOrdAbleAmt           ), DATA_TYPE_LONG      ) );    // [long  ,   16] 거래소금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.KdqOrdAbleAmt         , sizeof( pAllOutBlock->outBlock2.KdqOrdAbleAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 코스닥금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BalEvalAmt            , sizeof( pAllOutBlock->outBlock2.BalEvalAmt             ), DATA_TYPE_LONG      ) );    // [long  ,   16] 잔고평가금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RcvblAmt              , sizeof( pAllOutBlock->outBlock2.RcvblAmt               ), DATA_TYPE_LONG      ) );    // [long  ,   16] 미수금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.DpsastTotamt          , sizeof( pAllOutBlock->outBlock2.DpsastTotamt           ), DATA_TYPE_LONG      ) );    // [long  ,   16] 예탁자산총액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PnlRat                , sizeof( pAllOutBlock->outBlock2.PnlRat                 ), DATA_TYPE_DOUBLE, 6 ) );    // [double, 18.6] 손익율                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.InvstOrgAmt           , sizeof( pAllOutBlock->outBlock2.InvstOrgAmt            ), DATA_TYPE_LONG      ) );    // [long  ,   20] 투자원금                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.InvstPlAmt            , sizeof( pAllOutBlock->outBlock2.InvstPlAmt             ), DATA_TYPE_LONG      ) );    // [long  ,   16] 투자손익금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgOrdAmt        , sizeof( pAllOutBlock->outBlock2.CrdtPldgOrdAmt         ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용담보주문금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.Dps                   , sizeof( pAllOutBlock->outBlock2.Dps                    ), DATA_TYPE_LONG      ) );    // [long  ,   16] 예수금                       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstAmt              , sizeof( pAllOutBlock->outBlock2.SubstAmt               ), DATA_TYPE_LONG      ) );    // [long  ,   16] 대용금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1Dps                 , sizeof( pAllOutBlock->outBlock2.D1Dps                  ), DATA_TYPE_LONG      ) );    // [long  ,   16] D1예수금                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2Dps                 , sizeof( pAllOutBlock->outBlock2.D2Dps                  ), DATA_TYPE_LONG      ) );    // [long  ,   16] D2예수금                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyrclAmt             , sizeof( pAllOutBlock->outBlock2.MnyrclAmt              ), DATA_TYPE_LONG      ) );    // [long  ,   16] 현금미수금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnMny                , sizeof( pAllOutBlock->outBlock2.MgnMny                 ), DATA_TYPE_LONG      ) );    // [long  ,   16] 증거금현금                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnSubst              , sizeof( pAllOutBlock->outBlock2.MgnSubst               ), DATA_TYPE_LONG      ) );    // [long  ,   16] 증거금대용                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.ChckAmt               , sizeof( pAllOutBlock->outBlock2.ChckAmt                ), DATA_TYPE_LONG      ) );    // [long  ,   16] 수표금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstOrdAbleAmt       , sizeof( pAllOutBlock->outBlock2.SubstOrdAbleAmt        ), DATA_TYPE_LONG      ) );    // [long  ,   16] 대용주문가능금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt, sizeof( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt ), DATA_TYPE_LONG      ) );    // [long  ,   16] 증거금률100퍼센트주문가능금액
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat35ordAbleAmt    , sizeof( pAllOutBlock->outBlock2.MgnRat35ordAbleAmt     ), DATA_TYPE_LONG      ) );    // [long  ,   16] 증거금률35%주문가능금액      
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat50ordAbleAmt    , sizeof( pAllOutBlock->outBlock2.MgnRat50ordAbleAmt     ), DATA_TYPE_LONG      ) );    // [long  ,   16] 증거금률50%주문가능금액      
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdaySellAdjstAmt     , sizeof( pAllOutBlock->outBlock2.PrdaySellAdjstAmt      ), DATA_TYPE_LONG      ) );    // [long  ,   16] 전일매도정산금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayBuyAdjstAmt      , sizeof( pAllOutBlock->outBlock2.PrdayBuyAdjstAmt       ), DATA_TYPE_LONG      ) );    // [long  ,   16] 전일매수정산금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdaySellAdjstAmt     , sizeof( pAllOutBlock->outBlock2.CrdaySellAdjstAmt      ), DATA_TYPE_LONG      ) );    // [long  ,   16] 금일매도정산금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayBuyAdjstAmt      , sizeof( pAllOutBlock->outBlock2.CrdayBuyAdjstAmt       ), DATA_TYPE_LONG      ) );    // [long  ,   16] 금일매수정산금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1ovdRepayRqrdAmt     , sizeof( pAllOutBlock->outBlock2.D1ovdRepayRqrdAmt      ), DATA_TYPE_LONG      ) );    // [long  ,   16] D1연체변제소요금액           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2ovdRepayRqrdAmt     , sizeof( pAllOutBlock->outBlock2.D2ovdRepayRqrdAmt      ), DATA_TYPE_LONG      ) );    // [long  ,   16] D2연체변제소요금액           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1PrsmptWthdwAbleAmt  , sizeof( pAllOutBlock->outBlock2.D1PrsmptWthdwAbleAmt   ), DATA_TYPE_LONG      ) );    // [long  ,   16] D1추정인출가능금액           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2PrsmptWthdwAbleAmt  , sizeof( pAllOutBlock->outBlock2.D2PrsmptWthdwAbleAmt   ), DATA_TYPE_LONG      ) );    // [long  ,   16] D2추정인출가능금액           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.DpspdgLoanAmt         , sizeof( pAllOutBlock->outBlock2.DpspdgLoanAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 예탁담보대출금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.Imreq                 , sizeof( pAllOutBlock->outBlock2.Imreq                  ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용설정보증금               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MloanAmt              , sizeof( pAllOutBlock->outBlock2.MloanAmt               ), DATA_TYPE_LONG      ) );    // [long  ,   16] 융자금액                     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.ChgAfPldgRat          , sizeof( pAllOutBlock->outBlock2.ChgAfPldgRat           ), DATA_TYPE_DOUBLE, 3 ) );    // [double,  9.3] 변경후담보비율               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrgPldgAmt            , sizeof( pAllOutBlock->outBlock2.OrgPldgAmt             ), DATA_TYPE_LONG      ) );    // [long  ,   16] 원담보금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubPldgAmt            , sizeof( pAllOutBlock->outBlock2.SubPldgAmt             ), DATA_TYPE_LONG      ) );    // [long  ,   16] 부담보금액                   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RqrdPldgAmt           , sizeof( pAllOutBlock->outBlock2.RqrdPldgAmt            ), DATA_TYPE_LONG      ) );    // [long  ,   16] 소요담보금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrgPdlckAmt           , sizeof( pAllOutBlock->outBlock2.OrgPdlckAmt            ), DATA_TYPE_LONG      ) );    // [long  ,   16] 원담보부족금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PdlckAmt              , sizeof( pAllOutBlock->outBlock2.PdlckAmt               ), DATA_TYPE_LONG      ) );    // [long  ,   16] 담보부족금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AddPldgMny            , sizeof( pAllOutBlock->outBlock2.AddPldgMny             ), DATA_TYPE_LONG      ) );    // [long  ,   16] 추가담보현금                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1OrdAbleAmt          , sizeof( pAllOutBlock->outBlock2.D1OrdAbleAmt           ), DATA_TYPE_LONG      ) );    // [long  ,   16] D1주문가능금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtIntdltAmt         , sizeof( pAllOutBlock->outBlock2.CrdtIntdltAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용이자미납금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.EtclndAmt             , sizeof( pAllOutBlock->outBlock2.EtclndAmt              ), DATA_TYPE_LONG      ) );    // [long  ,   16] 기타대여금액                 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.NtdayPrsmptCvrgAmt    , sizeof( pAllOutBlock->outBlock2.NtdayPrsmptCvrgAmt     ), DATA_TYPE_LONG      ) );    // [long  ,   16] 익일추정반대매매금액         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrgPldgSumAmt         , sizeof( pAllOutBlock->outBlock2.OrgPldgSumAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 원담보합계금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtOrdAbleAmt        , sizeof( pAllOutBlock->outBlock2.CrdtOrdAbleAmt         ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용주문가능금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubPldgSumAmt         , sizeof( pAllOutBlock->outBlock2.SubPldgSumAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 부담보합계금액               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgAmtMny        , sizeof( pAllOutBlock->outBlock2.CrdtPldgAmtMny         ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용담보금현금               
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgSubstAmt      , sizeof( pAllOutBlock->outBlock2.CrdtPldgSubstAmt       ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용담보대용금액             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AddCrdtPldgMny        , sizeof( pAllOutBlock->outBlock2.AddCrdtPldgMny         ), DATA_TYPE_LONG      ) );    // [long  ,   16] 추가신용담보현금             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgRuseAmt       , sizeof( pAllOutBlock->outBlock2.CrdtPldgRuseAmt        ), DATA_TYPE_LONG      ) );    // [long  ,   16] 신용담보재사용금액           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AddCrdtPldgSubst      , sizeof( pAllOutBlock->outBlock2.AddCrdtPldgSubst       ), DATA_TYPE_LONG      ) );    // [long  ,   16] 추가신용담보대용             
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CslLoanAmtdt1         , sizeof( pAllOutBlock->outBlock2.CslLoanAmtdt1          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 매도대금담보대출금액         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.DpslRestrcAmt         , sizeof( pAllOutBlock->outBlock2.DpslRestrcAmt          ), DATA_TYPE_LONG      ) );    // [long  ,   16] 처분제한금액                 
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
LRESULT CDlg_CSPAQ02200::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
