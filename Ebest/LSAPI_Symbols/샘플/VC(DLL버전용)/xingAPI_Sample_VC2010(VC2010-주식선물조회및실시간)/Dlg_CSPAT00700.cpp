// Dlg_CSPAT00700.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPAT00700.h"

#include "./Packet/CSPAT00700.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAT00700 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPAT00700, CDialog)

CDlg_CSPAT00700::CDlg_CSPAT00700(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPAT00700::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPAT00700)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPAT00700::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPAT00700)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
}


BEGIN_MESSAGE_MAP(CDlg_CSPAT00700, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPAT00700)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAT00700 message handlers

BOOL CDlg_CSPAT00700::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPAT00700::OnButtonRequest() 
{
	RequestData();
}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPAT00700::InitCtrls()
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
	((CComboBox*)GetDlgItem( IDC_INBLOCK_ORDPRCPTNCODE ))->SetCurSel( 0 );	// 호가유형코드         
	((CComboBox*)GetDlgItem( IDC_INBLOCK_ORDCNDITPCODE ))->SetCurSel( 0 );	// 주문조건구분         

	//-------------------------------------------------------------------------
	// OutBlock1
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수    ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "원주문번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호      ", TRUE ,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "입력비밀번호  ", TRUE ,    8 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "종목번호      ", TRUE ,   12 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문수량      ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "호가유형코드  ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문조건구분  ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문가        ", FALSE, 13.2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "통신매체코드  ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "전략코드      ", TRUE ,    6 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "그룹ID        ", TRUE ,   20 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문회차      ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "포트폴리오번호", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "바스켓번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "트렌치번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "아이템번호    ", FALSE,   10 );

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수          ", FALSE,    5 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문번호            ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "모주문번호          ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문시각            ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문시장코드        ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문유형코드        ", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "단축종목번호        ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "프로그램호가유형코드", TRUE ,    2 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "공매도호가구분      ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "공매도가능여부      ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용거래코드        ", TRUE ,    3 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대출일              ", TRUE ,    8 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "반대매매주문구분    ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "유동성공급자여부    ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "관리사원번호        ", TRUE ,    9 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문금액            ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매매구분            ", TRUE ,    1 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예비주문번호        ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "반대매매일련번호    ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예약주문번호        ", FALSE,   10 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문금액        ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용주문금액        ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "재사용주문금액      ", FALSE,   16 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명              ", TRUE ,   40 );
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "종목명              ", TRUE ,   40 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPAT00700::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 현물정정주문 (  )
	CSPAT00700InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPAT00700";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strOrgOrdNo     ;		GetDlgItemText( IDC_INBLOCK_ORGORDNO     , strOrgOrdNo      );		// [long  ,   10] 원주문번호                      StartPos 5, Length 10
    CString strAcntNo       ;		GetDlgItemText( IDC_INBLOCK_ACNTNO       , strAcntNo        );		// [string,   20] 계좌번호                        StartPos 15, Length 20
    CString strInptPwd      ;		GetDlgItemText( IDC_INBLOCK_INPTPWD      , strInptPwd       );		// [string,    8] 입력비밀번호                    StartPos 35, Length 8
    CString strIsuNo        ;		GetDlgItemText( IDC_INBLOCK_ISUNO        , strIsuNo         );		// [string,   12] 종목번호                        StartPos 43, Length 12
    CString strOrdQty       ;		GetDlgItemText( IDC_INBLOCK_ORDQTY       , strOrdQty        );		// [long  ,   16] 주문수량                        StartPos 55, Length 16
    CString strOrdprcPtnCode;		GetDlgItemText( IDC_INBLOCK_ORDPRCPTNCODE, strOrdprcPtnCode );		// [string,    2] 호가유형코드                    StartPos 71, Length 2
    CString strOrdCndiTpCode;		GetDlgItemText( IDC_INBLOCK_ORDCNDITPCODE, strOrdCndiTpCode ); 		// [string,    1] 주문조건구분                    StartPos 73, Length 1
    CString strOrdPrc       ;		GetDlgItemText( IDC_INBLOCK_ORDPRC       , strOrdPrc        );		// [double, 13.2] 주문가                          StartPos 74, Length 13

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.OrgOrdNo     , sizeof( pckInBlock.OrgOrdNo      ), strOrgOrdNo     , DATA_TYPE_LONG         );		// [long  ,   10] 원주문번호  
	SetPacketData( pckInBlock.AcntNo       , sizeof( pckInBlock.AcntNo        ), strAcntNo       , DATA_TYPE_STRING       );		// [string,   20] 계좌번호    
	SetPacketData( pckInBlock.InptPwd      , sizeof( pckInBlock.InptPwd       ), strInptPwd      , DATA_TYPE_STRING       );		// [string,    8] 입력비밀번호
	SetPacketData( pckInBlock.IsuNo        , sizeof( pckInBlock.IsuNo         ), strIsuNo        , DATA_TYPE_STRING       );		// [string,   12] 종목번호    
	SetPacketData( pckInBlock.OrdQty       , sizeof( pckInBlock.OrdQty        ), strOrdQty       , DATA_TYPE_LONG         );		// [long  ,   16] 주문수량    
	SetPacketData( pckInBlock.OrdprcPtnCode, sizeof( pckInBlock.OrdprcPtnCode ), strOrdprcPtnCode, DATA_TYPE_STRING       );		// [string,    2] 호가유형코드
	SetPacketData( pckInBlock.OrdCndiTpCode, sizeof( pckInBlock.OrdCndiTpCode ), strOrdCndiTpCode, DATA_TYPE_STRING	      ); 		// [string,    1] 주문조건구분
	SetPacketData( pckInBlock.OrdPrc       , sizeof( pckInBlock.OrdPrc        ), strOrdPrc       , DATA_TYPE_FLOAT_DOT, 2 );		// [double, 13.2] 주문가      , Header Type이 B 인 경우 Data Type이 실수면 소수점을 포함해야 한다.

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
LRESULT CDlg_CSPAT00700::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		typedef struct
		{
			CSPAT00700OutBlock1	outBlock1;
			CSPAT00700OutBlock2	outBlock2;
		} CSPAT00700AllOutBlock, *LPCSPAT00700AllOutBlock;

		LPCSPAT00700AllOutBlock pAllOutBlock = (LPCSPAT00700AllOutBlock)pRpData->lpData;

		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		if( pRpData->nDataLength >= sizeof( CSPAT00700OutBlock1 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock1.InsertItem( 0, "" );
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt       , sizeof( pAllOutBlock->outBlock1.RecCnt        ), DATA_TYPE_LONG         ) );   // [long  ,    5] 레코드갯수                      StartPos 0, Length 5
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrgOrdNo     , sizeof( pAllOutBlock->outBlock1.OrgOrdNo      ), DATA_TYPE_LONG         ) );   // [long  ,   10] 원주문번호                      StartPos 5, Length 10
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo       , sizeof( pAllOutBlock->outBlock1.AcntNo        ), DATA_TYPE_STRING       ) );   // [string,   20] 계좌번호                        StartPos 15, Length 20
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.InptPwd      , sizeof( pAllOutBlock->outBlock1.InptPwd       ), DATA_TYPE_STRING       ) );   // [string,    8] 입력비밀번호                    StartPos 35, Length 8
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.IsuNo        , sizeof( pAllOutBlock->outBlock1.IsuNo         ), DATA_TYPE_STRING       ) );   // [string,   12] 종목번호                        StartPos 43, Length 12
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdQty       , sizeof( pAllOutBlock->outBlock1.OrdQty        ), DATA_TYPE_LONG         ) );	// [long  ,   16] 주문수량                        StartPos 55, Length 16      
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdprcPtnCode, sizeof( pAllOutBlock->outBlock1.OrdprcPtnCode ), DATA_TYPE_STRING       ) );   // [string,    2] 호가유형코드                    StartPos 71, Length 2
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdCndiTpCode, sizeof( pAllOutBlock->outBlock1.OrdCndiTpCode ), DATA_TYPE_STRING       ) );   // [string,    1] 주문조건구분                    StartPos 73, Length 1
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdPrc       , sizeof( pAllOutBlock->outBlock1.OrdPrc        ), DATA_TYPE_FLOAT_DOT, 2 ) );   // [double, 13.2] 주문가                          StartPos 74, Length 13
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CommdaCode   , sizeof( pAllOutBlock->outBlock1.CommdaCode    ), DATA_TYPE_STRING       ) );   // [string,    2] 통신매체코드                    StartPos 87, Length 2
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.StrtgCode    , sizeof( pAllOutBlock->outBlock1.StrtgCode     ), DATA_TYPE_STRING       ) );   // [string,    6] 전략코드                        StartPos 89, Length 6
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.GrpId        , sizeof( pAllOutBlock->outBlock1.GrpId         ), DATA_TYPE_STRING       ) );   // [string,   20] 그룹ID                          StartPos 95, Length 20
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdSeqNo     , sizeof( pAllOutBlock->outBlock1.OrdSeqNo      ), DATA_TYPE_LONG         ) );   // [long  ,   10] 주문회차                        StartPos 115, Length 10
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.PtflNo       , sizeof( pAllOutBlock->outBlock1.PtflNo        ), DATA_TYPE_LONG         ) );   // [long  ,   10] 포트폴리오번호                  StartPos 125, Length 10
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BskNo        , sizeof( pAllOutBlock->outBlock1.BskNo         ), DATA_TYPE_LONG         ) );   // [long  ,   10] 바스켓번호                      StartPos 135, Length 10
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.TrchNo       , sizeof( pAllOutBlock->outBlock1.TrchNo        ), DATA_TYPE_LONG         ) );   // [long  ,   10] 트렌치번호                      StartPos 145, Length 10
			m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.ItemNo       , sizeof( pAllOutBlock->outBlock1.ItemNo        ), DATA_TYPE_LONG         ) );   // [long  ,   10] 아이템번호                      StartPos 155, Length 10
		}

		if( pRpData->nDataLength >= sizeof( CSPAT00700OutBlock1 ) + sizeof( CSPAT00700OutBlock2 ) )
		{
			int nCol = 0;
			m_ctrlOutBlock2.InsertItem( 0, "" );
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt           , sizeof( pAllOutBlock->outBlock2.RecCnt            ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수                      StartPos 0, Length 5
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdNo            , sizeof( pAllOutBlock->outBlock2.OrdNo             ), DATA_TYPE_LONG   ) );    // [long  ,   10] 주문번호                        StartPos 5, Length 10
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrntOrdNo        , sizeof( pAllOutBlock->outBlock2.PrntOrdNo         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 모주문번호                      StartPos 15, Length 10
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdTime          , sizeof( pAllOutBlock->outBlock2.OrdTime           ), DATA_TYPE_STRING ) );    // [string,    9] 주문시각                        StartPos 25, Length 9
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdMktCode       , sizeof( pAllOutBlock->outBlock2.OrdMktCode        ), DATA_TYPE_STRING ) );    // [string,    2] 주문시장코드                    StartPos 34, Length 2
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdPtnCode       , sizeof( pAllOutBlock->outBlock2.OrdPtnCode        ), DATA_TYPE_STRING ) );    // [string,    2] 주문유형코드                    StartPos 36, Length 2
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.ShtnIsuNo        , sizeof( pAllOutBlock->outBlock2.ShtnIsuNo         ), DATA_TYPE_STRING ) );    // [string,    9] 단축종목번호                    StartPos 38, Length 9
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrgmOrdprcPtnCode, sizeof( pAllOutBlock->outBlock2.PrgmOrdprcPtnCode ), DATA_TYPE_STRING ) );    // [string,    2] 프로그램호가유형코드            StartPos 47, Length 2
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.StslOrdprcTpCode , sizeof( pAllOutBlock->outBlock2.StslOrdprcTpCode  ), DATA_TYPE_STRING ) );    // [string,    1] 공매도호가구분                  StartPos 49, Length 1
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.StslAbleYn       , sizeof( pAllOutBlock->outBlock2.StslAbleYn        ), DATA_TYPE_STRING ) );    // [string,    1] 공매도가능여부                  StartPos 50, Length 1
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgntrnCode       , sizeof( pAllOutBlock->outBlock2.MgntrnCode        ), DATA_TYPE_STRING ) );    // [string,    3] 신용거래코드                    StartPos 51, Length 3
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.LoanDt           , sizeof( pAllOutBlock->outBlock2.LoanDt            ), DATA_TYPE_STRING ) );    // [string,    8] 대출일                          StartPos 54, Length 8
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CvrgOrdTp        , sizeof( pAllOutBlock->outBlock2.CvrgOrdTp         ), DATA_TYPE_STRING ) );    // [string,    1] 반대매매주문구분                StartPos 62, Length 1
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.LpYn             , sizeof( pAllOutBlock->outBlock2.LpYn              ), DATA_TYPE_STRING ) );    // [string,    1] 유동성공급자여부                StartPos 63, Length 1
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgempNo          , sizeof( pAllOutBlock->outBlock2.MgempNo           ), DATA_TYPE_STRING ) );    // [string,    9] 관리사원번호                    StartPos 64, Length 9
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAmt           , sizeof( pAllOutBlock->outBlock2.OrdAmt            ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문금액                        StartPos 73, Length 16
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BnsTpCode        , sizeof( pAllOutBlock->outBlock2.BnsTpCode         ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분                        StartPos 89, Length 1
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SpareOrdNo       , sizeof( pAllOutBlock->outBlock2.SpareOrdNo        ), DATA_TYPE_LONG   ) );    // [long  ,   10] 예비주문번호                    StartPos 90, Length 10
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.CvrgSeqno        , sizeof( pAllOutBlock->outBlock2.CvrgSeqno         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 반대매매일련번호                StartPos 100, Length 10
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RsvOrdNo         , sizeof( pAllOutBlock->outBlock2.RsvOrdNo          ), DATA_TYPE_LONG   ) );    // [long  ,   10] 예약주문번호                    StartPos 110, Length 10
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAmt        , sizeof( pAllOutBlock->outBlock2.MnyOrdAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문금액                    StartPos 120, Length 16
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstOrdAmt      , sizeof( pAllOutBlock->outBlock2.SubstOrdAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용주문금액                    StartPos 136, Length 16
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RuseOrdAmt       , sizeof( pAllOutBlock->outBlock2.RuseOrdAmt        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 재사용주문금액                  StartPos 152, Length 16
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm           , sizeof( pAllOutBlock->outBlock2.AcntNm            ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명                          StartPos 168, Length 40
			m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.IsuNm            , sizeof( pAllOutBlock->outBlock2.IsuNm             ), DATA_TYPE_STRING ) );    // [string,   40] 종목명                          StartPos 208, Length 40
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
LRESULT CDlg_CSPAT00700::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
