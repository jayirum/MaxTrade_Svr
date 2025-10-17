// Dlg_t8411.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "Dlg_t8411.h"

#include "./Packet/t8411.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t8411 dialog

IMPLEMENT_DYNCREATE(CDlg_t8411, CDialog)

CDlg_t8411::CDlg_t8411(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_t8411::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_t8411)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t8411::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t8411)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK, m_ctrlOutBlock);
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
}


BEGIN_MESSAGE_MAP(CDlg_t8411, CDialog)
	//{{AFX_MSG_MAP(CDlg_t8411)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t8411 message handlers

BOOL CDlg_t8411::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bCompress = FALSE;
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_t8411::OnButtonRequest() 
{
	RequestData( FALSE );
}

void CDlg_t8411::OnButtonRequestNext() 
{
	RequestData( TRUE );	
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t8411::InitCtrls()
{
	//-----------------------------------------------------------
	// OutBlock
	m_ctrlOutBlock.InsertColumn( 0, "단축코드",		LVCFMT_LEFT, 50 );
	m_ctrlOutBlock.InsertColumn( 1, "전일시가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 2, "전일고가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 3, "전일저가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 4, "전일종가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 5, "전일거래량",	LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 6, "당일시가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 7, "당일고가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 8, "당일저가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 9, "당일종가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 10, "상한가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 11, "하한가",		LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 12, "연속일자",	LVCFMT_LEFT,  50 );
	m_ctrlOutBlock.InsertColumn( 13, "연속시간",	LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 14, "장시작시간",	LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 15, "장종료시간",	LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 16, "동시호가처리시간", LVCFMT_RIGHT, 50 );
	m_ctrlOutBlock.InsertColumn( 17, "레코드카운트", LVCFMT_RIGHT, 50 );

	//-----------------------------------------------------------
	// OutBlock1
	m_ctrlOutBlock1.InsertColumn(  0, "날짜",		LVCFMT_LEFT  , 50 );
	m_ctrlOutBlock1.InsertColumn(  1, "시간",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  2, "시가",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  3, "고가",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  4, "저가",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  5, "종가",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  6, "거래량",		LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  7, "수정구분",	LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  8, "수정비율",	LVCFMT_RIGHT , 50 );
	m_ctrlOutBlock1.InsertColumn(  9, "수정주가반영한목", LVCFMT_RIGHT , 50 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t8411::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 주식현재가호가조회(t1101) ( attr,block,headtype=A )
	t8411InBlock	pckInBlock;

	TCHAR			szTrNo[]	= "t8411";
	char			szNextKey[]	= "";

	//-----------------------------------------------------------
	// 데이터 취득
	CString str_shcode   ; GetDlgItemText( IDC_INBLOCK_SHCODE   , str_shcode    );		// 단축코드 
	CString str_ncnt     ; GetDlgItemText( IDC_INBLOCK_NCNT		, str_ncnt		);		// 단위(n틱)                     
	CString str_qrycnt   ; GetDlgItemText( IDC_INBLOCK_QRYCNT   , str_qrycnt    );		// 요청건수(최대-압축:2000비압축:500
	CString str_nday     ; GetDlgItemText( IDC_INBLOCK_NDAY		, str_nday		);		// 조회영업일수(0:미사용1>=사용)  드   
	CString str_sdate    ; GetDlgItemText( IDC_INBLOCK_SDATE	, str_sdate		);		// 시작일자                       
	CString str_edate    ; GetDlgItemText( IDC_INBLOCK_EDATE	, str_edate		);		// 종료일자 
	CString str_cts_date ; GetDlgItemText( IDC_INBLOCK_CTS_DATE , str_cts_date	);		// 연속일자 
	CString str_cts_time ; GetDlgItemText( IDC_INBLOCK_CTS_TIME , str_cts_time	);		// 연속시간

	// 압축여부(Y:압축N:비압축)       
	m_bCompress = GetCheckedRadioButton( IDC_RD_COMPRESS, IDC_RD_DECOMPRESS ) == IDC_RD_COMPRESS ? TRUE : FALSE;

	CString str_comp_yn; 
	str_comp_yn = ( m_bCompress == TRUE ? "Y" : "N" );


	// 다음 조회일경우에 다음 조회시 사용하는 Field는 데이터를 넣으면 안된다.
	if( bNext == FALSE )
	{
		str_cts_date = "";
		str_cts_time = "";
	}
	
	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );
	
	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.shcode	, sizeof( pckInBlock.shcode		), str_shcode   , DATA_TYPE_STRING );	// 단축코드   
	SetPacketData( pckInBlock.ncnt		, sizeof( pckInBlock.ncnt		), str_ncnt		, DATA_TYPE_LONG );		// 단위   
	SetPacketData( pckInBlock.qrycnt	, sizeof( pckInBlock.qrycnt		), str_qrycnt   , DATA_TYPE_LONG );		// 요청건수   
	SetPacketData( pckInBlock.nday		, sizeof( pckInBlock.nday		), str_nday		, DATA_TYPE_STRING );	// 조회영업일수   
	SetPacketData( pckInBlock.sdate		, sizeof( pckInBlock.sdate		), str_sdate	, DATA_TYPE_STRING );	// 시작일자   
	SetPacketData( pckInBlock.edate		, sizeof( pckInBlock.edate		), str_edate	, DATA_TYPE_STRING );	// 종료일자   
	SetPacketData( pckInBlock.cts_date  , sizeof( pckInBlock.cts_date   ), str_cts_date , DATA_TYPE_STRING );	// 연속일자   
	SetPacketData( pckInBlock.cts_time  , sizeof( pckInBlock.cts_time	), str_cts_time , DATA_TYPE_STRING );	// 연속시간   
	SetPacketData( pckInBlock.comp_yn   , sizeof( pckInBlock.comp_yn    ), str_comp_yn  , DATA_TYPE_STRING );	// 압축여부   

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
	
	if( bNext == FALSE )
	{
		m_ctrlOutBlock.DeleteAllItems();
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
LRESULT CDlg_t8411::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		// Header가 A Type 일 경우에는 Block 명이 들어온다.
		if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock ) == 0 )
		{
			LPt8411OutBlock pOutBlock = (LPt8411OutBlock)pRpData->lpData;
			m_ctrlOutBlock.InsertItem( 0, "" );
			m_ctrlOutBlock.SetItemText(  0, 0, GetDispData( pOutBlock->shcode ,		sizeof( pOutBlock->shcode		), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 1, GetDispData( pOutBlock->jisiga ,		sizeof( pOutBlock->jisiga		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 2, GetDispData( pOutBlock->jihigh ,		sizeof( pOutBlock->jihigh		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 3, GetDispData( pOutBlock->jilow ,		sizeof( pOutBlock->jilow		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 4, GetDispData( pOutBlock->jiclose ,	sizeof( pOutBlock->jiclose      ), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 5, GetDispData( pOutBlock->jivolume ,	sizeof( pOutBlock->jivolume     ), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 6, GetDispData( pOutBlock->disiga ,		sizeof( pOutBlock->disiga		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 7, GetDispData( pOutBlock->dihigh ,		sizeof( pOutBlock->dihigh		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 8, GetDispData( pOutBlock->dilow ,		sizeof( pOutBlock->dilow		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 9, GetDispData( pOutBlock->diclose ,	sizeof( pOutBlock->diclose      ), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 10, GetDispData( pOutBlock->highend ,	sizeof( pOutBlock->highend      ), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 11, GetDispData( pOutBlock->lowend ,	sizeof( pOutBlock->lowend		), DATA_TYPE_LONG ) );
			m_ctrlOutBlock.SetItemText(  0, 12, GetDispData( pOutBlock->cts_date ,	sizeof( pOutBlock->cts_date     ), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 13, GetDispData( pOutBlock->cts_time ,	sizeof( pOutBlock->cts_time     ), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 14, GetDispData( pOutBlock->s_time ,	sizeof( pOutBlock->s_time		), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 15, GetDispData( pOutBlock->e_time ,	sizeof( pOutBlock->e_time		), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 16, GetDispData( pOutBlock->dshmin ,	sizeof( pOutBlock->dshmin		), DATA_TYPE_STRING ) );
			m_ctrlOutBlock.SetItemText(  0, 17, GetDispData( pOutBlock->rec_count , sizeof( pOutBlock->rec_count    ), DATA_TYPE_LONG ) );
		}
		else if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock1 ) == 0 )
		{
			LPt8411OutBlock1 pOutBlock1 = (LPt8411OutBlock1)pRpData->lpData;

			int nDataLength = pRpData->nDataLength;
			int nCount = 0;

			t8411OutBlock1 szOutBlock1[2000];

			if( m_bCompress == TRUE)
			{
				nDataLength = g_iXingAPI.Decompress( (char *)pOutBlock1, (char *)&szOutBlock1[0], nDataLength );
				if (nDataLength  > 0)
				{
					nCount = nDataLength / sizeof( t8411OutBlock1 );
					pOutBlock1 = &szOutBlock1[0];
				}
			}
			else
			{
					nCount = nDataLength / sizeof( t8411OutBlock1 );
			}

			// Occurs 일 경우
			// Header가 'A' 이면 전체길이에서 OutBlock의 길이를 나눠서 갯수를 구한다.
			for( int i=0; i< nCount ; i++ )
			{
				m_ctrlOutBlock1.InsertItem( i, "" );
				m_ctrlOutBlock1.SetItemText( i,  0, GetDispData( pOutBlock1[i].date		, sizeof( pOutBlock1[i].date       ), DATA_TYPE_STRING ) );		// 날짜               
				m_ctrlOutBlock1.SetItemText( i,  1, GetDispData( pOutBlock1[i].time		, sizeof( pOutBlock1[i].time       ), DATA_TYPE_STRING ) );		// 시간               
				m_ctrlOutBlock1.SetItemText( i,  2, GetDispData( pOutBlock1[i].open		, sizeof( pOutBlock1[i].open       ), DATA_TYPE_LONG ) );		// 시가               
				m_ctrlOutBlock1.SetItemText( i,  3, GetDispData( pOutBlock1[i].high		, sizeof( pOutBlock1[i].high       ), DATA_TYPE_LONG ) );		// 고가               
				m_ctrlOutBlock1.SetItemText( i,  4, GetDispData( pOutBlock1[i].low		, sizeof( pOutBlock1[i].low			), DATA_TYPE_LONG ) );		// 저가               
				m_ctrlOutBlock1.SetItemText( i,  5, GetDispData( pOutBlock1[i].close	, sizeof( pOutBlock1[i].close       ), DATA_TYPE_LONG ) );		// 종가               
				m_ctrlOutBlock1.SetItemText( i,  6, GetDispData( pOutBlock1[i].jdiff_vol, sizeof( pOutBlock1[i].jdiff_vol   ), DATA_TYPE_LONG ) );		// 거래량             
				m_ctrlOutBlock1.SetItemText( i,  7, GetDispData( pOutBlock1[i].jongchk  , sizeof( pOutBlock1[i].jongchk     ), DATA_TYPE_LONG ) );		// 수정구분           
				m_ctrlOutBlock1.SetItemText( i,  8, GetDispData( pOutBlock1[i].rate		, sizeof( pOutBlock1[i].rate		), DATA_TYPE_FLOAT ) );		// 수정비율           
				m_ctrlOutBlock1.SetItemText( i,  9, GetDispData( pOutBlock1[i].pricechk , sizeof( pOutBlock1[i].pricechk    ), DATA_TYPE_LONG ) );		// 수정주가반영항목   
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
LRESULT CDlg_t8411::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );

	return 0L;
}