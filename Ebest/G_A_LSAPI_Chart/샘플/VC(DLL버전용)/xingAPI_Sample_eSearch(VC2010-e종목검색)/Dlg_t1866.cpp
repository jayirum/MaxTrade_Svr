// Dlg_t1866.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample_eSearch.h"
#include "Dlg_t1866.h"

#include "./Packet/t1866.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1866 dialog

IMPLEMENT_DYNCREATE(CDlg_t1866, CDialog)

// CDlg_t1866::CDlg_t1866(CWnd* pParent /*=NULL*/)
// : CDialog(CDlg_t1866::IDD, pParent)
CDlg_t1866::CDlg_t1866()
{
	//{{AFX_DATA_INIT(CDlg_t1866)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t1866::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t1866)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK,	m_ctrlOutBlock);
	DDX_Control(pDX, IDC_OUTBLOCK1,	m_ctrlOutBlock1);
}


BEGIN_MESSAGE_MAP(CDlg_t1866, CDialog)
	//{{AFX_MSG_MAP(CDlg_t1866)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST1,		  OnButtonRequest1	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1866 message handlers

BOOL CDlg_t1866::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
// 종목검색 리스트 조회
void CDlg_t1866::OnButtonRequest() 
{
	RequestData();
}
// 종목검색 연속 조회
void CDlg_t1866::OnButtonRequest1()
{
	RequestData(TRUE);
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t1866::InitCtrls()
{
	m_ctrlOutBlock.InsertColumn( 0, "저장조건수",	LVCFMT_LEFT, 60 );
	m_ctrlOutBlock.InsertColumn( 1, "연속여부",		LVCFMT_LEFT, 60 );
	m_ctrlOutBlock.InsertColumn( 2, "연속키",		LVCFMT_LEFT, 300 );

	m_ctrlOutBlock1.InsertColumn( 0, "서버저장인덱스",	LVCFMT_LEFT, 200 );
	m_ctrlOutBlock1.InsertColumn( 1, "그룹명",			LVCFMT_LEFT, 200 );
	m_ctrlOutBlock1.InsertColumn( 2, "조건저장명",		LVCFMT_LEFT, 300 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1866::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 서버저장조건리스트조회(API/MTS)(t1866) ( ENCRYPT,block,headtype=A )
	t1866InBlock	pckInBlock;


	TCHAR			szTrNo[]	= "t1866";
	char			szNextKey[]	= "";

	//-----------------------------------------------------------
	// 데이터 취득
	CString str_ID;			GetDlgItemText( IDC_INBLOCK_ID, str_ID );			// 로그인ID
	CString str_Cont;		GetDlgItemText( IDC_INBLOCK_CONT, str_Cont );		// 연속여부
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_INDEX1866, str_Index );	// 연속키

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.user_id		, sizeof( pckInBlock.user_id		), str_ID      , DATA_TYPE_STRING );	// 로그인ID
	SetPacketData( pckInBlock.gb			, sizeof( pckInBlock.gb				), "0"		   , DATA_TYPE_STRING );	// 조회구분
	SetPacketData( pckInBlock.group_name	, sizeof( pckInBlock.group_name		), ""	       , DATA_TYPE_STRING );	// 그룹명
	SetPacketData( pckInBlock.cont			, sizeof( pckInBlock.cont			), str_Cont    , DATA_TYPE_STRING );	// 연속여부
	SetPacketData( pckInBlock.cont_key		, sizeof( pckInBlock.cont_key		), str_Index   , DATA_TYPE_STRING );	// 연속키

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
LRESULT CDlg_t1866::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{

		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		if( strcmp( pRpData->szBlockName, NAME_t1866OutBlock ) == 0 )
		{
			LPt1866OutBlock p1866OutBlock = (LPt1866OutBlock)pRpData->lpData;

			m_ctrlOutBlock.InsertItem( 0, "" );
			m_ctrlOutBlock.SetItemText( 0,  0, GetDispData( p1866OutBlock[0].result_count 	, sizeof( p1866OutBlock[0].result_count	), DATA_TYPE_STRING ) );	// 서버저장인덱스          
			m_ctrlOutBlock.SetItemText( 0,  1, GetDispData( p1866OutBlock[0].cont			, sizeof( p1866OutBlock[0].cont	), DATA_TYPE_STRING ) );			// 그룹명                 
			m_ctrlOutBlock.SetItemText( 0,  2, GetDispData( p1866OutBlock[0].cont_key		, sizeof( p1866OutBlock[0].cont_key	), DATA_TYPE_STRING ) );		// 조건저장명             
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1866OutBlock1 ) == 0 )
		{
			LPt1866OutBlock1 p1866OutBlock1 = (LPt1866OutBlock1)pRpData->lpData;

			int nCount = pRpData->nDataLength/sizeof(t1866OutBlock1);
			
			for( int i=0; i< nCount ; i++ )
			{
				m_ctrlOutBlock1.InsertItem( i, "" );
				m_ctrlOutBlock1.SetItemText( i,  0, GetDispData( p1866OutBlock1[i].query_index 	, sizeof( p1866OutBlock1[i].query_index	), DATA_TYPE_STRING ) );		// 서버저장인덱스          
				m_ctrlOutBlock1.SetItemText( i,  1, GetDispData( p1866OutBlock1[i].group_name  	, sizeof( p1866OutBlock1[i].group_name	), DATA_TYPE_STRING ) );		// 그룹명                 
				m_ctrlOutBlock1.SetItemText( i,  2, GetDispData( p1866OutBlock1[i].query_name  	, sizeof( p1866OutBlock1[i].query_name	), DATA_TYPE_STRING ) );		// 조건저장명             
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
LRESULT CDlg_t1866::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );

	return 0L;
}

BOOL CDlg_t1866::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->wParam == VK_RETURN )
	{
		return 0;
	}
	else if( pMsg->wParam == VK_TAB )
	{
		CDialog::PreTranslateMessage(pMsg);
	}

	return 0;
}