// Dlg_t1601.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "Dlg_t1601.h"

#include "./Packet/t1601.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1601 dialog

IMPLEMENT_DYNCREATE(CDlg_t1601, CDialog)

CDlg_t1601::CDlg_t1601(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_t1601::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_t1601)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t1601::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t1601)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_t1601, CDialog)
	//{{AFX_MSG_MAP(CDlg_t1601)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1601 message handlers

BOOL CDlg_t1601::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_t1601::OnButtonRequest() 
{
	RequestData();
}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t1601::InitCtrls()
{
	//-------------------------------------------------------------------------
	// InBlock : 기본값 세팅

	// 초기값 세팅
	((CComboBox*)GetDlgItem( IDC_INBLOCK_GUBUN1 ))->SetCurSel( 0 );	// 금액수량구분1   
	((CComboBox*)GetDlgItem( IDC_INBLOCK_GUBUN2 ))->SetCurSel( 0 );	// 금액수량구분2        
	((CComboBox*)GetDlgItem( IDC_INBLOCK_GUBUN3 ))->SetCurSel( 0 );	// 금액단위             

	//-------------------------------------------------------------------------
	// OutBlock
	InitOutBlockCtrl( IDC_OUTBLOCK1, "코 스 피" );
	InitOutBlockCtrl( IDC_OUTBLOCK2, "코 스 닥" );
	InitOutBlockCtrl( IDC_OUTBLOCK3, "선    물" );
	InitOutBlockCtrl( IDC_OUTBLOCK4, "콜 옵 션" );
	InitOutBlockCtrl( IDC_OUTBLOCK5, "풋 옵 션" );
}

void CDlg_t1601::InitOutBlockCtrl( UINT nID, LPCTSTR lpszTitle )
{
	InsertColumn4List( this, nID,  0, lpszTitle , TRUE , 7 );
	InsertColumn4List( this, nID,  1, "개    인", FALSE, 7 );
	InsertColumn4List( this, nID,  2, "외 국 인", FALSE, 7 );
	InsertColumn4List( this, nID,  3, "기 관 계", FALSE, 7 );
	InsertColumn4List( this, nID,  4, "증    권", FALSE, 7 );
	InsertColumn4List( this, nID,  5, "투    신", FALSE, 7 );
	InsertColumn4List( this, nID,  6, "은    행", FALSE, 7 );
	InsertColumn4List( this, nID,  7, "보    험", FALSE, 7 );
	InsertColumn4List( this, nID,  8, "종    금", FALSE, 7 );
	InsertColumn4List( this, nID,  9, "기    금", FALSE, 7 );
	InsertColumn4List( this, nID, 10, "국    가", FALSE, 7 );
	InsertColumn4List( this, nID, 11, "사모펀드", FALSE, 7 );
	InsertColumn4List( this, nID, 12, "기    타", FALSE, 7 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1601::RequestData()
{
	//-----------------------------------------------------------
	// 현물정상주문(t1601) ( base21=SONAT000,headtype=B )
	t1601InBlock	pckInBlock;

	TCHAR			szTrNo[]		= "t1601";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString str_gubun1;	GetDlgItemText( IDC_INBLOCK_GUBUN1, str_gubun1 );		// 금액수량구분1
    CString str_gubun2;	GetDlgItemText( IDC_INBLOCK_GUBUN2, str_gubun2 );		// 금액수량구분2
    CString str_gubun3;	GetDlgItemText( IDC_INBLOCK_GUBUN3, str_gubun3 );		// 금액단위     

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.gubun1, sizeof( pckInBlock.gubun1 ), str_gubun1, DATA_TYPE_STRING );		// 금액수량구분1
	SetPacketData( pckInBlock.gubun2, sizeof( pckInBlock.gubun2 ), str_gubun2, DATA_TYPE_STRING );		// 금액수량구분2
	SetPacketData( pckInBlock.gubun3, sizeof( pckInBlock.gubun3 ), str_gubun3, DATA_TYPE_STRING );		// 금액단위     

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		FALSE,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		szNextKey,					// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);

	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK1 ))->DeleteAllItems();
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK2 ))->DeleteAllItems();
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK3 ))->DeleteAllItems();
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK4 ))->DeleteAllItems();
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK5 ))->DeleteAllItems();


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
LRESULT CDlg_t1601::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		// OutBlock1 처리
		if( strcmp( pRpData->szBlockName, NAME_t1601OutBlock1 ) == 0 )
		{
			CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_OUTBLOCK1 );
			LPt1601OutBlock1 pOutBlock = (LPt1601OutBlock1)pRpData->lpData;
			
			pList->InsertItem( 0, "매    도" );
			pList->InsertItem( 1, "매    수" );
			pList->InsertItem( 2, "순 매 수" );
			pList->InsertItem( 3, "증    감" );
			
			pList->SetItemText( 1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매수          
			pList->SetItemText( 0,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매도          
			pList->SetItemText( 3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인증감          
			pList->SetItemText( 2,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인순매수        
			
			pList->SetItemText( 1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매수        
			pList->SetItemText( 0,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매도        
			pList->SetItemText( 3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인증감        
			pList->SetItemText( 2,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인순매수      
			
			pList->SetItemText( 1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매수        
			pList->SetItemText( 0,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매도        
			pList->SetItemText( 3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계증감        
			pList->SetItemText( 2,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계순매수      
			
			pList->SetItemText( 1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매수          
			pList->SetItemText( 0,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매도          
			pList->SetItemText( 3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권증감          
			pList->SetItemText( 2,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권순매수        
			
			pList->SetItemText( 1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매수          
			pList->SetItemText( 0,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매도          
			pList->SetItemText( 3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신증감          
			pList->SetItemText( 2,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신순매수        
			
			pList->SetItemText( 1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매수          
			pList->SetItemText( 0,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매도          
			pList->SetItemText( 3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행증감          
			pList->SetItemText( 2,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행순매수        
			
			pList->SetItemText( 1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매수          
			pList->SetItemText( 0,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매도          
			pList->SetItemText( 3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험증감          
			pList->SetItemText( 2,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험순매수        
			
			pList->SetItemText( 1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매수          
			pList->SetItemText( 0,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매도          
			pList->SetItemText( 3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금증감          
			pList->SetItemText( 2,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금순매수        
			
			pList->SetItemText( 1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매수          
			pList->SetItemText( 0,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매도          
			pList->SetItemText( 3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금증감          
			pList->SetItemText( 2,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금순매수        
			
			pList->SetItemText( 1, 10, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매수          
			pList->SetItemText( 0, 10, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매도          
			pList->SetItemText( 3, 10, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가증감          
			pList->SetItemText( 2, 10, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가순매수        
			
			pList->SetItemText( 1, 11, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매수          
			pList->SetItemText( 0, 11, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매도          
			pList->SetItemText( 3, 11, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타증감          
			pList->SetItemText( 2, 11, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타순매수        
			
			pList->SetItemText( 1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매수      
			pList->SetItemText( 0, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매도      
			pList->SetItemText( 3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드증감      
			pList->SetItemText( 2, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드순매수    
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1601OutBlock2 ) == 0 )
		{
			CListCtrl*		 pList		= (CListCtrl*)GetDlgItem( IDC_OUTBLOCK2 );
			LPt1601OutBlock2 pOutBlock	= (LPt1601OutBlock2)pRpData->lpData;
			
			pList->InsertItem( 0, "매    도" );
			pList->InsertItem( 1, "매    수" );
			pList->InsertItem( 2, "순 매 수" );
			pList->InsertItem( 3, "증    감" );
			
			pList->SetItemText( 1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매수          
			pList->SetItemText( 0,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매도          
			pList->SetItemText( 3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인증감          
			pList->SetItemText( 2,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인순매수        
			
			pList->SetItemText( 1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매수        
			pList->SetItemText( 0,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매도        
			pList->SetItemText( 3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인증감        
			pList->SetItemText( 2,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인순매수      
			
			pList->SetItemText( 1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매수        
			pList->SetItemText( 0,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매도        
			pList->SetItemText( 3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계증감        
			pList->SetItemText( 2,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계순매수      
			
			pList->SetItemText( 1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매수          
			pList->SetItemText( 0,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매도          
			pList->SetItemText( 3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권증감          
			pList->SetItemText( 2,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권순매수        
			
			pList->SetItemText( 1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매수          
			pList->SetItemText( 0,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매도          
			pList->SetItemText( 3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신증감          
			pList->SetItemText( 2,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신순매수        
			
			pList->SetItemText( 1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매수          
			pList->SetItemText( 0,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매도          
			pList->SetItemText( 3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행증감          
			pList->SetItemText( 2,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행순매수        
			
			pList->SetItemText( 1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매수          
			pList->SetItemText( 0,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매도          
			pList->SetItemText( 3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험증감          
			pList->SetItemText( 2,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험순매수        
			
			pList->SetItemText( 1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매수          
			pList->SetItemText( 0,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매도          
			pList->SetItemText( 3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금증감          
			pList->SetItemText( 2,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금순매수        
			
			pList->SetItemText( 1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매수          
			pList->SetItemText( 0,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매도          
			pList->SetItemText( 3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금증감          
			pList->SetItemText( 2,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금순매수        
			
			pList->SetItemText( 1, 10, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매수          
			pList->SetItemText( 0, 10, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매도          
			pList->SetItemText( 3, 10, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가증감          
			pList->SetItemText( 2, 10, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가순매수        
			
			pList->SetItemText( 1, 11, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매수          
			pList->SetItemText( 0, 11, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매도          
			pList->SetItemText( 3, 11, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타증감          
			pList->SetItemText( 2, 11, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타순매수        
			
			pList->SetItemText( 1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매수      
			pList->SetItemText( 0, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매도      
			pList->SetItemText( 3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드증감      
			pList->SetItemText( 2, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드순매수    
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1601OutBlock3 ) == 0 )
		{
			CListCtrl*		 pList		= (CListCtrl*)GetDlgItem( IDC_OUTBLOCK3 );
			LPt1601OutBlock3 pOutBlock	= (LPt1601OutBlock3)pRpData->lpData;
			
			pList->InsertItem( 0, "매    도" );
			pList->InsertItem( 1, "매    수" );
			pList->InsertItem( 2, "순 매 수" );
			pList->InsertItem( 3, "증    감" );
			
			pList->SetItemText( 1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매수          
			pList->SetItemText( 0,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매도          
			pList->SetItemText( 3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인증감          
			pList->SetItemText( 2,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인순매수        
			
			pList->SetItemText( 1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매수        
			pList->SetItemText( 0,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매도        
			pList->SetItemText( 3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인증감        
			pList->SetItemText( 2,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인순매수      
			
			pList->SetItemText( 1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매수        
			pList->SetItemText( 0,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매도        
			pList->SetItemText( 3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계증감        
			pList->SetItemText( 2,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계순매수      
			
			pList->SetItemText( 1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매수          
			pList->SetItemText( 0,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매도          
			pList->SetItemText( 3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권증감          
			pList->SetItemText( 2,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권순매수        
			
			pList->SetItemText( 1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매수          
			pList->SetItemText( 0,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매도          
			pList->SetItemText( 3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신증감          
			pList->SetItemText( 2,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신순매수        
			
			pList->SetItemText( 1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매수          
			pList->SetItemText( 0,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매도          
			pList->SetItemText( 3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행증감          
			pList->SetItemText( 2,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행순매수        
			
			pList->SetItemText( 1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매수          
			pList->SetItemText( 0,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매도          
			pList->SetItemText( 3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험증감          
			pList->SetItemText( 2,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험순매수        
			
			pList->SetItemText( 1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매수          
			pList->SetItemText( 0,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매도          
			pList->SetItemText( 3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금증감          
			pList->SetItemText( 2,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금순매수        
			
			pList->SetItemText( 1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매수          
			pList->SetItemText( 0,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매도          
			pList->SetItemText( 3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금증감          
			pList->SetItemText( 2,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금순매수        
			
			pList->SetItemText( 1, 10, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매수          
			pList->SetItemText( 0, 10, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매도          
			pList->SetItemText( 3, 10, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가증감          
			pList->SetItemText( 2, 10, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가순매수        
			
			pList->SetItemText( 1, 11, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매수          
			pList->SetItemText( 0, 11, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매도          
			pList->SetItemText( 3, 11, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타증감          
			pList->SetItemText( 2, 11, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타순매수        
			
			pList->SetItemText( 1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매수      
			pList->SetItemText( 0, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매도      
			pList->SetItemText( 3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드증감      
			pList->SetItemText( 2, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드순매수    
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1601OutBlock4 ) == 0 )
		{
			CListCtrl*		 pList		= (CListCtrl*)GetDlgItem( IDC_OUTBLOCK4 );
			LPt1601OutBlock4 pOutBlock	= (LPt1601OutBlock4)pRpData->lpData;
			
			pList->InsertItem( 0, "매    도" );
			pList->InsertItem( 1, "매    수" );
			pList->InsertItem( 2, "순 매 수" );
			pList->InsertItem( 3, "증    감" );
			
			pList->SetItemText( 1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매수          
			pList->SetItemText( 0,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매도          
			pList->SetItemText( 3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인증감          
			pList->SetItemText( 2,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인순매수        
			
			pList->SetItemText( 1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매수        
			pList->SetItemText( 0,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매도        
			pList->SetItemText( 3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인증감        
			pList->SetItemText( 2,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인순매수      
			
			pList->SetItemText( 1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매수        
			pList->SetItemText( 0,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매도        
			pList->SetItemText( 3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계증감        
			pList->SetItemText( 2,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계순매수      
			
			pList->SetItemText( 1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매수          
			pList->SetItemText( 0,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매도          
			pList->SetItemText( 3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권증감          
			pList->SetItemText( 2,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권순매수        
			
			pList->SetItemText( 1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매수          
			pList->SetItemText( 0,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매도          
			pList->SetItemText( 3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신증감          
			pList->SetItemText( 2,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신순매수        
			
			pList->SetItemText( 1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매수          
			pList->SetItemText( 0,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매도          
			pList->SetItemText( 3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행증감          
			pList->SetItemText( 2,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행순매수        
			
			pList->SetItemText( 1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매수          
			pList->SetItemText( 0,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매도          
			pList->SetItemText( 3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험증감          
			pList->SetItemText( 2,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험순매수        
			
			pList->SetItemText( 1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매수          
			pList->SetItemText( 0,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매도          
			pList->SetItemText( 3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금증감          
			pList->SetItemText( 2,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금순매수        
			
			pList->SetItemText( 1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매수          
			pList->SetItemText( 0,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매도          
			pList->SetItemText( 3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금증감          
			pList->SetItemText( 2,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금순매수        
			
			pList->SetItemText( 1, 10, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매수          
			pList->SetItemText( 0, 10, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매도          
			pList->SetItemText( 3, 10, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가증감          
			pList->SetItemText( 2, 10, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가순매수        
			
			pList->SetItemText( 1, 11, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매수          
			pList->SetItemText( 0, 11, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매도          
			pList->SetItemText( 3, 11, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타증감          
			pList->SetItemText( 2, 11, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타순매수        
			
			pList->SetItemText( 1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매수      
			pList->SetItemText( 0, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매도      
			pList->SetItemText( 3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드증감      
			pList->SetItemText( 2, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드순매수    
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1601OutBlock5 ) == 0 )
		{
			CListCtrl*		 pList		= (CListCtrl*)GetDlgItem( IDC_OUTBLOCK5 );
			LPt1601OutBlock5 pOutBlock	= (LPt1601OutBlock5)pRpData->lpData;
			
			pList->InsertItem( 0, "매    도" );
			pList->InsertItem( 1, "매    수" );
			pList->InsertItem( 2, "순 매 수" );
			pList->InsertItem( 3, "증    감" );
			
			pList->SetItemText( 1,  1, GetDispData( pOutBlock->ms_08     , sizeof( pOutBlock->ms_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매수          
			pList->SetItemText( 0,  1, GetDispData( pOutBlock->md_08     , sizeof( pOutBlock->md_08      ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인매도          
			pList->SetItemText( 3,  1, GetDispData( pOutBlock->rate_08   , sizeof( pOutBlock->rate_08    ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인증감          
			pList->SetItemText( 2,  1, GetDispData( pOutBlock->svolume_08, sizeof( pOutBlock->svolume_08 ), DATA_TYPE_LONG ) );    // [long  ,   12] 개인순매수        
			
			pList->SetItemText( 1,  2, GetDispData( pOutBlock->ms_17     , sizeof( pOutBlock->ms_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매수        
			pList->SetItemText( 0,  2, GetDispData( pOutBlock->md_17     , sizeof( pOutBlock->md_17      ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인매도        
			pList->SetItemText( 3,  2, GetDispData( pOutBlock->rate_17   , sizeof( pOutBlock->rate_17    ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인증감        
			pList->SetItemText( 2,  2, GetDispData( pOutBlock->svolume_17, sizeof( pOutBlock->svolume_17 ), DATA_TYPE_LONG ) );    // [long  ,   12] 외국인순매수      
			
			pList->SetItemText( 1,  3, GetDispData( pOutBlock->ms_18     , sizeof( pOutBlock->ms_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매수        
			pList->SetItemText( 0,  3, GetDispData( pOutBlock->md_18     , sizeof( pOutBlock->md_18      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계매도        
			pList->SetItemText( 3,  3, GetDispData( pOutBlock->rate_18   , sizeof( pOutBlock->rate_18    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계증감        
			pList->SetItemText( 2,  3, GetDispData( pOutBlock->svolume_18, sizeof( pOutBlock->svolume_18 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기관계순매수      
			
			pList->SetItemText( 1,  4, GetDispData( pOutBlock->ms_01     , sizeof( pOutBlock->ms_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매수          
			pList->SetItemText( 0,  4, GetDispData( pOutBlock->md_01     , sizeof( pOutBlock->md_01      ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권매도          
			pList->SetItemText( 3,  4, GetDispData( pOutBlock->rate_01   , sizeof( pOutBlock->rate_01    ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권증감          
			pList->SetItemText( 2,  4, GetDispData( pOutBlock->svolume_01, sizeof( pOutBlock->svolume_01 ), DATA_TYPE_LONG ) );    // [long  ,   12] 증권순매수        
			
			pList->SetItemText( 1,  5, GetDispData( pOutBlock->ms_03     , sizeof( pOutBlock->ms_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매수          
			pList->SetItemText( 0,  5, GetDispData( pOutBlock->md_03     , sizeof( pOutBlock->md_03      ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신매도          
			pList->SetItemText( 3,  5, GetDispData( pOutBlock->rate_03   , sizeof( pOutBlock->rate_03    ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신증감          
			pList->SetItemText( 2,  5, GetDispData( pOutBlock->svolume_03, sizeof( pOutBlock->svolume_03 ), DATA_TYPE_LONG ) );    // [long  ,   12] 투신순매수        
			
			pList->SetItemText( 1,  6, GetDispData( pOutBlock->ms_04     , sizeof( pOutBlock->ms_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매수          
			pList->SetItemText( 0,  6, GetDispData( pOutBlock->md_04     , sizeof( pOutBlock->md_04      ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행매도          
			pList->SetItemText( 3,  6, GetDispData( pOutBlock->rate_04   , sizeof( pOutBlock->rate_04    ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행증감          
			pList->SetItemText( 2,  6, GetDispData( pOutBlock->svolume_04, sizeof( pOutBlock->svolume_04 ), DATA_TYPE_LONG ) );    // [long  ,   12] 은행순매수        
			
			pList->SetItemText( 1,  7, GetDispData( pOutBlock->ms_02     , sizeof( pOutBlock->ms_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매수          
			pList->SetItemText( 0,  7, GetDispData( pOutBlock->md_02     , sizeof( pOutBlock->md_02      ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험매도          
			pList->SetItemText( 3,  7, GetDispData( pOutBlock->rate_02   , sizeof( pOutBlock->rate_02    ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험증감          
			pList->SetItemText( 2,  7, GetDispData( pOutBlock->svolume_02, sizeof( pOutBlock->svolume_02 ), DATA_TYPE_LONG ) );    // [long  ,   12] 보험순매수        
			
			pList->SetItemText( 1,  8, GetDispData( pOutBlock->ms_05     , sizeof( pOutBlock->ms_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매수          
			pList->SetItemText( 0,  8, GetDispData( pOutBlock->md_05     , sizeof( pOutBlock->md_05      ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금매도          
			pList->SetItemText( 3,  8, GetDispData( pOutBlock->rate_05   , sizeof( pOutBlock->rate_05    ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금증감          
			pList->SetItemText( 2,  8, GetDispData( pOutBlock->svolume_05, sizeof( pOutBlock->svolume_05 ), DATA_TYPE_LONG ) );    // [long  ,   12] 종금순매수        
			
			pList->SetItemText( 1,  9, GetDispData( pOutBlock->ms_06     , sizeof( pOutBlock->ms_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매수          
			pList->SetItemText( 0,  9, GetDispData( pOutBlock->md_06     , sizeof( pOutBlock->md_06      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금매도          
			pList->SetItemText( 3,  9, GetDispData( pOutBlock->rate_06   , sizeof( pOutBlock->rate_06    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금증감          
			pList->SetItemText( 2,  9, GetDispData( pOutBlock->svolume_06, sizeof( pOutBlock->svolume_06 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기금순매수        
			
			pList->SetItemText( 1, 10, GetDispData( pOutBlock->ms_11     , sizeof( pOutBlock->ms_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매수          
			pList->SetItemText( 0, 10, GetDispData( pOutBlock->md_11     , sizeof( pOutBlock->md_11      ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가매도          
			pList->SetItemText( 3, 10, GetDispData( pOutBlock->rate_11   , sizeof( pOutBlock->rate_11    ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가증감          
			pList->SetItemText( 2, 10, GetDispData( pOutBlock->svolume_11, sizeof( pOutBlock->svolume_11 ), DATA_TYPE_LONG ) );    // [long  ,   12] 국가순매수        
			
			pList->SetItemText( 1, 11, GetDispData( pOutBlock->ms_07     , sizeof( pOutBlock->ms_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매수          
			pList->SetItemText( 0, 11, GetDispData( pOutBlock->md_07     , sizeof( pOutBlock->md_07      ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타매도          
			pList->SetItemText( 3, 11, GetDispData( pOutBlock->rate_07   , sizeof( pOutBlock->rate_07    ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타증감          
			pList->SetItemText( 2, 11, GetDispData( pOutBlock->svolume_07, sizeof( pOutBlock->svolume_07 ), DATA_TYPE_LONG ) );    // [long  ,   12] 기타순매수        
			
			pList->SetItemText( 1, 12, GetDispData( pOutBlock->ms_00     , sizeof( pOutBlock->ms_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매수      
			pList->SetItemText( 0, 12, GetDispData( pOutBlock->md_00     , sizeof( pOutBlock->md_00      ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드매도      
			pList->SetItemText( 3, 12, GetDispData( pOutBlock->rate_00   , sizeof( pOutBlock->rate_00    ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드증감      
			pList->SetItemText( 2, 12, GetDispData( pOutBlock->svolume_00, sizeof( pOutBlock->svolume_00 ), DATA_TYPE_LONG ) );    // [long  ,   12] 사모펀드순매수    
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
LRESULT CDlg_t1601::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
