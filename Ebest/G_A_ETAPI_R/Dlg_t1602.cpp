// Dlg_t1602.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_t1602.h"

#include "./Packet/t1602.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1602 dialog


IMPLEMENT_DYNCREATE(CDlg_t1602, CDialog)

CDlg_t1602::CDlg_t1602(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_t1602::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_t1602)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t1602::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t1602)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control( pDX, IDC_OUTBLOCK,  m_ctrlOutBlock  );
	DDX_Control( pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1 );
}


BEGIN_MESSAGE_MAP(CDlg_t1602, CDialog)
	//{{AFX_MSG_MAP(CDlg_t1602)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1602 message handlers

BOOL CDlg_t1602::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_t1602::OnButtonRequest() 
{
	RequestData( FALSE );
}

void CDlg_t1602::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t1602::InitCtrls()
{
	int nCol;

	//-------------------------------------------------------------------------
	// InBlock : 기본값 세팅

	//-------------------------------------------------------------------------
	// OutBlock1
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "        ", TRUE ,   10 );    //         
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "개인    ", FALSE,   12 );    // 개인    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "외국인  ", FALSE,   12 );    // 외국인  
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "기관계  ", FALSE,   12 );    // 기관계  
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "증권    ", FALSE,   12 );    // 증권    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "투신    ", FALSE,   12 );    // 투신    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "은행    ", FALSE,   12 );    // 은행    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "보험    ", FALSE,   12 );    // 보험    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "종금    ", FALSE,   12 );    // 종금    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "기금    ", FALSE,   12 );    // 기금    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "기타    ", FALSE,   12 );    // 기타    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "국가    ", FALSE,   12 );    // 국가    
	InsertColumn4List( this, IDC_OUTBLOCK, nCol++, "사모펀드", FALSE,   12 );    // 사모펀드

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "시간          ", TRUE ,    8 );    // [string,    8] 시간                  
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "개인순매수    ", FALSE,   12 );    // [long  ,   12] 개인순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "외국인순매수  ", FALSE,   12 );    // [long  ,   12] 외국인순매수          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "기관계순매수  ", FALSE,   12 );    // [long  ,   12] 기관계순매수          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "증권순매수    ", FALSE,   12 );    // [long  ,   12] 증권순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "투신순매수    ", FALSE,   12 );    // [long  ,   12] 투신순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "은행순매수    ", FALSE,   12 );    // [long  ,   12] 은행순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "보험순매수    ", FALSE,   12 );    // [long  ,   12] 보험순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "종금순매수    ", FALSE,   12 );    // [long  ,   12] 종금순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "기금순매수    ", FALSE,   12 );    // [long  ,   12] 기금순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "기타순매수    ", FALSE,   12 );    // [long  ,   12] 기타순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "국가순매수    ", FALSE,   12 );    // [long  ,   12] 국가순매수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "사모펀드순매수", FALSE,   12 );    // [long  ,   12] 사모펀드순매수        
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1602::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	t1602InBlock	pckInBlock;

	TCHAR			szTrNo[]		= "t1602";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString str_market  ;		GetDlgItemText( IDC_INBLOCK_MARKET  , str_market   );    // [string,    1] 시장구분  
    CString str_upcode  ;		GetDlgItemText( IDC_INBLOCK_UPCODE  , str_upcode   );    // [string,    3] 업종코드  
    CString str_gubun1  ;		GetDlgItemText( IDC_INBLOCK_GUBUN1  , str_gubun1   );    // [string,    1] 수량구분  
    CString str_gubun2  ;		GetDlgItemText( IDC_INBLOCK_GUBUN2  , str_gubun2   );    // [string,    1] 전일분구분
    CString str_cts_time;		GetDlgItemText( IDC_INBLOCK_CTS_TIME, str_cts_time );    // [string,    8] CTSTIME   
    CString str_cts_idx ;		GetDlgItemText( IDC_INBLOCK_CTS_IDX , str_cts_idx  );    // [long  ,    4] CTSIDX    
    CString str_cnt     ;		GetDlgItemText( IDC_INBLOCK_CNT     , str_cnt      );    // [string,    4] 조회건수  
    CString str_gubun3  ;		GetDlgItemText( IDC_INBLOCK_GUBUN3  , str_gubun3   );    // [string,    1] 직전대비구분(C:직전대비)  

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.market  , sizeof( pckInBlock.market   ), str_market  , DATA_TYPE_STRING );    // [string,    1] 시장구분  
	SetPacketData( pckInBlock.upcode  , sizeof( pckInBlock.upcode   ), str_upcode  , DATA_TYPE_STRING );    // [string,    3] 업종코드  
	SetPacketData( pckInBlock.gubun1  , sizeof( pckInBlock.gubun1   ), str_gubun1  , DATA_TYPE_STRING );    // [string,    1] 수량구분  
	SetPacketData( pckInBlock.gubun2  , sizeof( pckInBlock.gubun2   ), str_gubun2  , DATA_TYPE_STRING );    // [string,    1] 전일분구분
	SetPacketData( pckInBlock.cts_time, sizeof( pckInBlock.cts_time ), str_cts_time, DATA_TYPE_STRING );    // [string,    8] CTSTIME   
	SetPacketData( pckInBlock.cts_idx , sizeof( pckInBlock.cts_idx  ), str_cts_idx , DATA_TYPE_LONG   );    // [long  ,    4] CTSIDX    
	SetPacketData( pckInBlock.gubun3  , sizeof( pckInBlock.gubun3   ), str_gubun3  , DATA_TYPE_STRING );    // [string,    1] 직전대비구분(C:직전대비)  

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		"",							// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);

	m_ctrlOutBlock .DeleteAllItems();
	if( bNext == FALSE )
	{
		m_ctrlOutBlock1.DeleteAllItems();
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
LRESULT CDlg_t1602::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRecvPacket = (LPRECV_PACKET)lParam;

		if( strcmp( pRecvPacket->szBlockName, "t1602OutBlock" ) == 0 )
		{
			LPt1602OutBlock pOutBlock = (LPt1602OutBlock)pRecvPacket->lpData;

			char szTemp[100] = { 0 };
			CopyMemory( szTemp, pOutBlock->cts_time, sizeof( pOutBlock->cts_time ) );
			SetDlgItemText( IDC_INBLOCK_CTS_TIME, szTemp );

			m_ctrlOutBlock.InsertItem( 0, "투자자코드" );
			m_ctrlOutBlock.InsertItem( 1, "매수" );
			m_ctrlOutBlock.InsertItem( 2, "매도" );
			m_ctrlOutBlock.InsertItem( 3, "증감" );
			m_ctrlOutBlock.InsertItem( 4, "순매수" );
			m_ctrlOutBlock.SetItemText(  0,  1, GetDispData( pOutBlock->tjjcode_08, sizeof( pOutBlock->tjjcode_08 ), DATA_TYPE_STRING ) );    // [string,    4] 개인투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 개인매수         
			m_ctrlOutBlock.SetItemText(  2,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 개인매도         
			m_ctrlOutBlock.SetItemText(  3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 개인증감         
			m_ctrlOutBlock.SetItemText(  4,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 개인순매수       
			m_ctrlOutBlock.SetItemText(  0,  2, GetDispData( pOutBlock->tjjcode_17, sizeof( pOutBlock->tjjcode_17 ), DATA_TYPE_STRING ) );    // [string,    4] 외국인투자자코드 
			m_ctrlOutBlock.SetItemText(  1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 외국인매수       
			m_ctrlOutBlock.SetItemText(  2,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 외국인매도       
			m_ctrlOutBlock.SetItemText(  3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 외국인증감       
			m_ctrlOutBlock.SetItemText(  4,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 외국인순매수     
			m_ctrlOutBlock.SetItemText(  0,  3, GetDispData( pOutBlock->tjjcode_18, sizeof( pOutBlock->tjjcode_18 ), DATA_TYPE_STRING ) );    // [string,    4] 기관계투자자코드 
			m_ctrlOutBlock.SetItemText(  1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기관계매수       
			m_ctrlOutBlock.SetItemText(  2,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기관계매도       
			m_ctrlOutBlock.SetItemText(  3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기관계증감       
			m_ctrlOutBlock.SetItemText(  4,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기관계순매수     
			m_ctrlOutBlock.SetItemText(  0,  4, GetDispData( pOutBlock->tjjcode_01, sizeof( pOutBlock->tjjcode_01 ), DATA_TYPE_STRING ) );    // [string,    4] 증권투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 증권매수         
			m_ctrlOutBlock.SetItemText(  2,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 증권매도         
			m_ctrlOutBlock.SetItemText(  3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 증권증감         
			m_ctrlOutBlock.SetItemText(  4,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 증권순매수       
			m_ctrlOutBlock.SetItemText(  0,  5, GetDispData( pOutBlock->tjjcode_03, sizeof( pOutBlock->tjjcode_03 ), DATA_TYPE_STRING ) );    // [string,    4] 투신투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 투신매수         
			m_ctrlOutBlock.SetItemText(  2,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 투신매도         
			m_ctrlOutBlock.SetItemText(  3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 투신증감         
			m_ctrlOutBlock.SetItemText(  4,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 투신순매수       
			m_ctrlOutBlock.SetItemText(  0,  6, GetDispData( pOutBlock->tjjcode_04, sizeof( pOutBlock->tjjcode_04 ), DATA_TYPE_STRING ) );    // [string,    4] 은행투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 은행매수         
			m_ctrlOutBlock.SetItemText(  2,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 은행매도         
			m_ctrlOutBlock.SetItemText(  3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 은행증감         
			m_ctrlOutBlock.SetItemText(  4,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 은행순매수       
			m_ctrlOutBlock.SetItemText(  0,  7, GetDispData( pOutBlock->tjjcode_02, sizeof( pOutBlock->tjjcode_02 ), DATA_TYPE_STRING ) );    // [string,    4] 보험투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 보험매수         
			m_ctrlOutBlock.SetItemText(  2,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 보험매도         
			m_ctrlOutBlock.SetItemText(  3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 보험증감         
			m_ctrlOutBlock.SetItemText(  4,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 보험순매수       
			m_ctrlOutBlock.SetItemText(  0,  8, GetDispData( pOutBlock->tjjcode_05, sizeof( pOutBlock->tjjcode_05 ), DATA_TYPE_STRING ) );    // [string,    4] 종금투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 종금매수         
			m_ctrlOutBlock.SetItemText(  2,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 종금매도         
			m_ctrlOutBlock.SetItemText(  3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 종금증감         
			m_ctrlOutBlock.SetItemText(  4,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 종금순매수       
			m_ctrlOutBlock.SetItemText(  0,  9, GetDispData( pOutBlock->tjjcode_06, sizeof( pOutBlock->tjjcode_06 ), DATA_TYPE_STRING ) );    // [string,    4] 기금투자자코드   
			m_ctrlOutBlock.SetItemText(  1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기금매수         
			m_ctrlOutBlock.SetItemText(  2,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기금매도         
			m_ctrlOutBlock.SetItemText(  3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기금증감         
			m_ctrlOutBlock.SetItemText(  4,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기금순매수       
			m_ctrlOutBlock.SetItemText(  0, 10, GetDispData( pOutBlock->tjjcode_07, sizeof( pOutBlock->tjjcode_07 ), DATA_TYPE_STRING ) );    // [string,    4] 기타투자자코드   
			m_ctrlOutBlock.SetItemText(  1, 10, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기타매수         
			m_ctrlOutBlock.SetItemText(  2, 10, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기타매도         
			m_ctrlOutBlock.SetItemText(  3, 10, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기타증감         
			m_ctrlOutBlock.SetItemText(  4, 10, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기타순매수       
			m_ctrlOutBlock.SetItemText(  0, 11, GetDispData( pOutBlock->tjjcode_11, sizeof( pOutBlock->tjjcode_11 ), DATA_TYPE_STRING ) );    // [string,    4] 국가투자자코드   
			m_ctrlOutBlock.SetItemText(  1, 11, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 국가매수         
			m_ctrlOutBlock.SetItemText(  2, 11, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 국가매도         
			m_ctrlOutBlock.SetItemText(  3, 11, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 국가증감         
			m_ctrlOutBlock.SetItemText(  4, 11, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 국가순매수       
			m_ctrlOutBlock.SetItemText(  0, 12, GetDispData( pOutBlock->tjjcode_00, sizeof( pOutBlock->tjjcode_00 ), DATA_TYPE_STRING ) );    // [string,    4] 사모펀드코드     
			m_ctrlOutBlock.SetItemText(  1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 사모펀드매수     
			m_ctrlOutBlock.SetItemText(  2, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG   ) );    // [long  ,   12] 사모펀드매도     
			m_ctrlOutBlock.SetItemText(  3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG   ) );    // [long  ,   12] 사모펀드증감     
			m_ctrlOutBlock.SetItemText(  4, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 사모펀드순매수   
		}
		else if( strcmp( pRecvPacket->szBlockName, "t1602OutBlock1" ) == 0 )
		{
			LPt1602OutBlock1 pOutBlock = (LPt1602OutBlock1)pRecvPacket->lpData;
			int nCount = pRecvPacket->nDataLength / sizeof( t1602OutBlock1 );

			int nCol, nRow;
			for( int i=0; i<nCount; i++ )
			{
				nCol = 0;
				nRow = m_ctrlOutBlock1.InsertItem( m_ctrlOutBlock1.GetItemCount(), "" );
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].time , sizeof( pOutBlock[i].time  ), DATA_TYPE_STRING ) );    // [string,    8] 시간          
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_08, sizeof( pOutBlock[i].sv_08 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 개인순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_17, sizeof( pOutBlock[i].sv_17 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 외국인순매수  
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_18, sizeof( pOutBlock[i].sv_18 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기관계순매수  
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_01, sizeof( pOutBlock[i].sv_01 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 증권순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_03, sizeof( pOutBlock[i].sv_03 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 투신순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_04, sizeof( pOutBlock[i].sv_04 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 은행순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_02, sizeof( pOutBlock[i].sv_02 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 보험순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_05, sizeof( pOutBlock[i].sv_05 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 종금순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_06, sizeof( pOutBlock[i].sv_06 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기금순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_07, sizeof( pOutBlock[i].sv_07 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 기타순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_11, sizeof( pOutBlock[i].sv_11 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 국가순매수    
				m_ctrlOutBlock1.SetItemText( nRow, nCol++, GetDispData( pOutBlock[i].sv_00, sizeof( pOutBlock[i].sv_00 ), DATA_TYPE_LONG   ) );    // [long  ,   12] 사모펀드순매수
			}
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
LRESULT CDlg_t1602::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
