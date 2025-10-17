// Dlg_CSPAT00600.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPAT00600.h"

#include "./Packet/CSPAT00600.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAT00600 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPAT00600, CDialog)

CDlg_CSPAT00600::CDlg_CSPAT00600(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPAT00600::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPAT00600)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPAT00600::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPAT00600)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
}


BEGIN_MESSAGE_MAP(CDlg_CSPAT00600, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPAT00600)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAT00600 message handlers

BOOL CDlg_CSPAT00600::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPAT00600::OnButtonRequest() 
{
	RequestData();
}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPAT00600::InitCtrls()
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

	((CComboBox*)GetDlgItem( IDC_INBLOCK_ACNTNO        ))->SetCurSel( 0 );	// 계좌번호
	((CComboBox*)GetDlgItem( IDC_INBLOCK_BNSTPCODE     ))->SetCurSel( 0 );	// 매매구분             
	((CComboBox*)GetDlgItem( IDC_INBLOCK_ORDPRCPTNCODE ))->SetCurSel( 0 );	// 호가유형코드         
	((CComboBox*)GetDlgItem( IDC_INBLOCK_MGNTRNCODE	   ))->SetCurSel( 0 );	// 신용거래코드         
	((CComboBox*)GetDlgItem( IDC_INBLOCK_ORDCNDITPCODE ))->SetCurSel( 0 );	// 주문조건구분         

	//-------------------------------------------------------------------------
	// OutBlock1
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수          ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호            ", TRUE ,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "입력비밀번호        ", TRUE ,    8 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "종목번호            ", TRUE ,   12 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문수량            ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문가              ", FALSE, 13.2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매매구분            ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "호가유형코드        ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "프로그램호가유형코드", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "공매도가능여부      ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "공매도호가구분      ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "통신매체코드        ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "신용거래코드        ", TRUE ,    3 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "대출일              ", TRUE ,    8 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "회원번호            ", TRUE ,    3 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문조건구분        ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "전략코드            ", TRUE ,    6 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "그룹ID              ", TRUE ,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문회차            ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "포트폴리오번호      ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "바스켓번호          ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "트렌치번호          ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "아이템번호          ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "운용지시번호        ", TRUE ,   12 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "유동성공급자여부    ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "반대매매구분        ", TRUE ,    1 );

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수      ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문번호        ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문시각        ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문시장코드    ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문유형코드    ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "단축종목번호    ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "관리사원번호    ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문금액        ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예비주문번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "반대매매일련번호", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예약주문번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "실물주문수량    ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "재사용주문수량  ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문금액    ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용주문금액    ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "재사용주문금액  ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명          ", TRUE ,   40 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "종목명          ", TRUE ,   40 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPAT00600::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
// 현물정상주문(CSPAT00600) ( base21=SONAT000,headtype=B )
	CSPAT00600InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPAT00600";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo       ;		GetDlgItemText( IDC_INBLOCK_ACNTNO       , strAcntNo        );		// 계좌번호                       StartPos 5, Length 20
    CString strInptPwd      ;		GetDlgItemText( IDC_INBLOCK_INPTPWD      , strInptPwd       );		// 입력비밀번호                   StartPos 25, Length 8
    CString strIsuNo        ;		GetDlgItemText( IDC_INBLOCK_ISUNO        , strIsuNo         );		// 종목번호                       StartPos 33, Length 12
    CString strOrdQty       ;		GetDlgItemText( IDC_INBLOCK_ORDQTY       , strOrdQty        );		// 주문수량                       StartPos 45, Length 16
    CString strOrdPrc       ;		GetDlgItemText( IDC_INBLOCK_ORDPRC       , strOrdPrc        );		// 주문가                         StartPos 61, Length 13
    CString strBnsTpCode    ;		GetDlgItemText( IDC_INBLOCK_BNSTPCODE    , strBnsTpCode     );		// 매매구분                       StartPos 74, Length 1
    CString strOrdprcPtnCode;		GetDlgItemText( IDC_INBLOCK_ORDPRCPTNCODE, strOrdprcPtnCode );		// 호가유형코드                   StartPos 75, Length 2
    CString strMgntrnCode   ;		GetDlgItemText( IDC_INBLOCK_MGNTRNCODE   , strMgntrnCode    );		// 신용거래코드                   StartPos 83, Length 3
    CString strLoanDt       ;		GetDlgItemText( IDC_INBLOCK_LOANDT       , strLoanDt        );		// 대출일                         StartPos 86, Length 8
    CString strOrdCndiTpCode;		GetDlgItemText( IDC_INBLOCK_ORDCNDITPCODE, strOrdCndiTpCode ); 		// 주문조건구분                   StartPos 97, Length 1

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.AcntNo       , sizeof( pckInBlock.AcntNo        ), strAcntNo       , DATA_TYPE_STRING );		// 계좌번호                 
	SetPacketData( pckInBlock.InptPwd      , sizeof( pckInBlock.InptPwd       ), strInptPwd      , DATA_TYPE_STRING );		// 입력비밀번호             
	SetPacketData( pckInBlock.IsuNo        , sizeof( pckInBlock.IsuNo         ), strIsuNo        , DATA_TYPE_STRING );		// 종목번호                 
	SetPacketData( pckInBlock.OrdQty       , sizeof( pckInBlock.OrdQty        ), strOrdQty       , DATA_TYPE_LONG   );		// 주문수량                 
	SetPacketData( pckInBlock.OrdPrc       , sizeof( pckInBlock.OrdPrc        ), strOrdPrc       , DATA_TYPE_FLOAT_DOT, 2 );	// 주문가, Header Type이 B 인 경우 Data Type이 실수면 소수점을 포함해야 한다.
	SetPacketData( pckInBlock.BnsTpCode    , sizeof( pckInBlock.BnsTpCode     ), strBnsTpCode    , DATA_TYPE_STRING );		// 매매구분                 
	SetPacketData( pckInBlock.OrdprcPtnCode, sizeof( pckInBlock.OrdprcPtnCode ), strOrdprcPtnCode, DATA_TYPE_STRING );		// 호가유형코드             
	SetPacketData( pckInBlock.MgntrnCode   , sizeof( pckInBlock.MgntrnCode    ), strMgntrnCode   , DATA_TYPE_STRING );		// 신용거래코드             
	SetPacketData( pckInBlock.LoanDt       , sizeof( pckInBlock.LoanDt        ), strLoanDt       , DATA_TYPE_STRING );		// 대출일                   
	SetPacketData( pckInBlock.OrdCndiTpCode, sizeof( pckInBlock.OrdCndiTpCode ), strOrdCndiTpCode, DATA_TYPE_STRING ); 		// 주문조건구분             

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
LRESULT CDlg_CSPAT00600::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		typedef struct
		{
			CSPAT00600OutBlock1	outBlock1;
			CSPAT00600OutBlock2	outBlock2;
		} CSPAT00600AllOutBlock, *LPCSPAT00600AllOutBlock;

		LPCSPAT00600AllOutBlock pAllOutBlock = (LPCSPAT00600AllOutBlock)pRpData->lpData;

		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		if( pRpData->nDataLength >= sizeof( CSPAT00600OutBlock1 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock1.InsertItem( 0, "" );
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt           , sizeof( pAllOutBlock->outBlock1.RecCnt            ), DATA_TYPE_LONG   ) );    // 레코드갯수          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo           , sizeof( pAllOutBlock->outBlock1.AcntNo            ), DATA_TYPE_STRING ) );    // 계좌번호            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.InptPwd          , sizeof( pAllOutBlock->outBlock1.InptPwd           ), DATA_TYPE_STRING ) );    // 입력비밀번호        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.IsuNo            , sizeof( pAllOutBlock->outBlock1.IsuNo             ), DATA_TYPE_STRING ) );    // 종목번호            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdQty           , sizeof( pAllOutBlock->outBlock1.OrdQty            ), DATA_TYPE_LONG   ) );    // 주문수량            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdPrc           , sizeof( pAllOutBlock->outBlock1.OrdPrc            ), DATA_TYPE_FLOAT_DOT, 2 ) );  // 주문가, Header가 B Type 일 경우 실수는 무조건 소숫점을 포함한다.          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BnsTpCode        , sizeof( pAllOutBlock->outBlock1.BnsTpCode         ), DATA_TYPE_STRING ) );    // 매매구분            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdprcPtnCode    , sizeof( pAllOutBlock->outBlock1.OrdprcPtnCode     ), DATA_TYPE_STRING ) );    // 호가유형코드        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.PrgmOrdprcPtnCode, sizeof( pAllOutBlock->outBlock1.PrgmOrdprcPtnCode ), DATA_TYPE_STRING ) );    // 프로그램호가유형코드
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.StslAbleYn       , sizeof( pAllOutBlock->outBlock1.StslAbleYn        ), DATA_TYPE_STRING ) );    // 공매도가능여부      
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.StslOrdprcTpCode , sizeof( pAllOutBlock->outBlock1.StslOrdprcTpCode  ), DATA_TYPE_STRING ) );    // 공매도호가구분      
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CommdaCode       , sizeof( pAllOutBlock->outBlock1.CommdaCode        ), DATA_TYPE_STRING ) );    // 통신매체코드        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.MgntrnCode       , sizeof( pAllOutBlock->outBlock1.MgntrnCode        ), DATA_TYPE_STRING ) );    // 신용거래코드        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.LoanDt           , sizeof( pAllOutBlock->outBlock1.LoanDt            ), DATA_TYPE_STRING ) );    // 대출일              
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.MbrNo            , sizeof( pAllOutBlock->outBlock1.MbrNo             ), DATA_TYPE_STRING ) );    // 회원번호            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdCndiTpCode    , sizeof( pAllOutBlock->outBlock1.OrdCndiTpCode     ), DATA_TYPE_STRING ) );    // 주문조건구분        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.StrtgCode        , sizeof( pAllOutBlock->outBlock1.StrtgCode         ), DATA_TYPE_STRING ) );    // 전략코드            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.GrpId            , sizeof( pAllOutBlock->outBlock1.GrpId             ), DATA_TYPE_STRING ) );    // 그룹ID              
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdSeqNo         , sizeof( pAllOutBlock->outBlock1.OrdSeqNo          ), DATA_TYPE_LONG   ) );    // 주문회차            
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.PtflNo           , sizeof( pAllOutBlock->outBlock1.PtflNo            ), DATA_TYPE_LONG   ) );    // 포트폴리오번호      
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BskNo            , sizeof( pAllOutBlock->outBlock1.BskNo             ), DATA_TYPE_LONG   ) );    // 바스켓번호          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.TrchNo           , sizeof( pAllOutBlock->outBlock1.TrchNo            ), DATA_TYPE_LONG   ) );    // 트렌치번호          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.ItemNo           , sizeof( pAllOutBlock->outBlock1.ItemNo            ), DATA_TYPE_LONG   ) );    // 아이템번호          
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OpDrtnNo         , sizeof( pAllOutBlock->outBlock1.OpDrtnNo          ), DATA_TYPE_STRING ) );    // 운용지시번호        
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.LpYn             , sizeof( pAllOutBlock->outBlock1.LpYn              ), DATA_TYPE_STRING ) );    // 유동성공급자여부    
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CvrgTpCode       , sizeof( pAllOutBlock->outBlock1.CvrgTpCode        ), DATA_TYPE_STRING ) );    // 반대매매구분        
		}

		if( pRpData->nDataLength >= sizeof( CSPAT00600OutBlock1 ) + sizeof( CSPAT00600OutBlock2 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock2.InsertItem( 0, "" );
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt     , sizeof( pAllOutBlock->outBlock2.RecCnt      ), DATA_TYPE_LONG   ) );    // 레코드갯수       
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdNo      , sizeof( pAllOutBlock->outBlock2.OrdNo       ), DATA_TYPE_LONG   ) );    // 주문번호         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdTime    , sizeof( pAllOutBlock->outBlock2.OrdTime     ), DATA_TYPE_STRING ) );    // 주문시각         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdMktCode , sizeof( pAllOutBlock->outBlock2.OrdMktCode  ), DATA_TYPE_STRING ) );    // 주문시장코드     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdPtnCode , sizeof( pAllOutBlock->outBlock2.OrdPtnCode  ), DATA_TYPE_STRING ) );    // 주문유형코드     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.ShtnIsuNo  , sizeof( pAllOutBlock->outBlock2.ShtnIsuNo   ), DATA_TYPE_STRING ) );    // 단축종목번호     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgempNo    , sizeof( pAllOutBlock->outBlock2.MgempNo     ), DATA_TYPE_STRING ) );    // 관리사원번호     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAmt     , sizeof( pAllOutBlock->outBlock2.OrdAmt      ), DATA_TYPE_LONG   ) );    // 주문금액         
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SpareOrdNo , sizeof( pAllOutBlock->outBlock2.SpareOrdNo  ), DATA_TYPE_LONG   ) );    // 예비주문번호     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CvrgSeqno  , sizeof( pAllOutBlock->outBlock2.CvrgSeqno   ), DATA_TYPE_LONG   ) );    // 반대매매일련번호 
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RsvOrdNo   , sizeof( pAllOutBlock->outBlock2.RsvOrdNo    ), DATA_TYPE_LONG   ) );    // 예약주문번호     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SpotOrdQty , sizeof( pAllOutBlock->outBlock2.SpotOrdQty  ), DATA_TYPE_LONG   ) );    // 실물주문수량     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RuseOrdQty , sizeof( pAllOutBlock->outBlock2.RuseOrdQty  ), DATA_TYPE_LONG   ) );    // 재사용주문수량   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAmt  , sizeof( pAllOutBlock->outBlock2.MnyOrdAmt   ), DATA_TYPE_LONG   ) );    // 현금주문금액     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstOrdAmt, sizeof( pAllOutBlock->outBlock2.SubstOrdAmt ), DATA_TYPE_LONG   ) );    // 대용주문금액     
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RuseOrdAmt , sizeof( pAllOutBlock->outBlock2.RuseOrdAmt  ), DATA_TYPE_LONG   ) );    // 재사용주문금액   
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm     , sizeof( pAllOutBlock->outBlock2.AcntNm      ), DATA_TYPE_STRING ) );    // 계좌명           
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.IsuNm      , sizeof( pAllOutBlock->outBlock2.IsuNm       ), DATA_TYPE_STRING ) );    // 종목명           
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
LRESULT CDlg_CSPAT00600::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
