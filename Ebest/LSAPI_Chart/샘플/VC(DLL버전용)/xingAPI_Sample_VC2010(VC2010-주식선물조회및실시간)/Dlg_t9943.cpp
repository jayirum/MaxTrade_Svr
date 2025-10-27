// Dlg_t9943.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "Dlg_t9943.h"

#include "./Packet/t9943.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t9943 dialog

IMPLEMENT_DYNCREATE(CDlg_t9943, CDialog)


CDlg_t9943::CDlg_t9943(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_t9943::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_t9943)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t9943::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t9943)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK, m_ctrlOutBlock);
}


BEGIN_MESSAGE_MAP(CDlg_t9943, CDialog)
	//{{AFX_MSG_MAP(CDlg_t9943)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t9943 message handlers

BOOL CDlg_t9943::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_t9943::OnButtonRequest() 
{
	RequestData( FALSE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t9943::InitCtrls()
{
	//-----------------------------------------------------------
	// OutBlock
	m_ctrlOutBlock.InsertColumn( 0, "종목명",   LVCFMT_LEFT,   150 );
	m_ctrlOutBlock.InsertColumn( 1, "단축코드", LVCFMT_CENTER,  60 );
	m_ctrlOutBlock.InsertColumn( 2, "확장코드", LVCFMT_CENTER, 100 );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t9943::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 주식마스터조회API용(t9943) ( block,headtype=A )
	t9943InBlock	pckInBlock;

	TCHAR			szTrNo[]	= "t9943";
	char			szNextKey[]	= "";
	
	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );
	
	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		szNextKey,					// 다음조회 Key : //		Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
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
LRESULT CDlg_t9943::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		int nDataLength = pRpData->nDataLength;

		LPt9943OutBlock pOutBlock = (LPt9943OutBlock)pRpData->lpData;

		// OutBlock 처리
		int nOutBlockCount = nDataLength / sizeof( t9943OutBlock );
		for( int i=0; i<nOutBlockCount; i++ )
		{
			m_ctrlOutBlock.InsertItem( i, "" );
			m_ctrlOutBlock.SetItemText( i,  0, GetDispData( pOutBlock[i].hname  , sizeof( pOutBlock[i].hname   ), DATA_TYPE_STRING ) );		// 종목명
			m_ctrlOutBlock.SetItemText( i,  1, GetDispData( pOutBlock[i].shcode , sizeof( pOutBlock[i].shcode  ), DATA_TYPE_STRING ) );		// 단축코드
			m_ctrlOutBlock.SetItemText( i,  2, GetDispData( pOutBlock[i].expcode, sizeof( pOutBlock[i].expcode ), DATA_TYPE_STRING ) );		// 확장코드
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
LRESULT CDlg_t9943::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );

	return 0L;
}