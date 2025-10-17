// Dlg_t1857.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample_eSearch.h"
#include "Dlg_t1857.h"

#include "./Packet/t1857.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1857 dialog

IMPLEMENT_DYNCREATE(CDlg_t1857, CDialog)

// CDlg_t1857::CDlg_t1857(CWnd* pParent /*=NULL*/)
// : CDialog(CDlg_t1857::IDD, pParent)
CDlg_t1857::CDlg_t1857()
{
	//{{AFX_DATA_INIT(CDlg_t1857)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t1857::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t1857)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1,	m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_KEYLIST,	m_ctrlOutBlock2);
	DDX_Control(pDX, IDC_COMBO_FLAG,m_combo_Flag);
	DDX_Control(pDX, IDC_COMBO_REAL,m_combo_Real);	
}


BEGIN_MESSAGE_MAP(CDlg_t1857, CDialog)
	//{{AFX_MSG_MAP(CDlg_t1857)
	ON_NOTIFY(NM_CLICK, IDC_KEYLIST, OnClickKeylist)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST4,		  OnButtonRequest4	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_REAL_DATA_SEARCH, OnXMReceiveRealSearchData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1857 message handlers

BOOL CDlg_t1857::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
// 종목검색 조회
void CDlg_t1857::OnButtonRequest() 
{
	RequestData();
}

// 종목검색 실시간등록
void CDlg_t1857::OnButtonRequest4()
{
	RequestData2();
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t1857::InitCtrls()
{
	m_ctrlOutBlock1.InsertColumn( 0, "종목코드",		LVCFMT_LEFT, 70 );
	m_ctrlOutBlock1.InsertColumn( 1, "종목명",			LVCFMT_LEFT, 100 );
	m_ctrlOutBlock1.InsertColumn( 2, "현재가",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 3, "전일대비구분",	LVCFMT_LEFT, 30 );
	m_ctrlOutBlock1.InsertColumn( 4, "전일대비",		LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 5, "등략율",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 6, "거래량",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 7, "종목상태",		LVCFMT_LEFT, 50 );

	m_ctrlOutBlock2.InsertColumn( 0, "Key",	LVCFMT_LEFT, 100 );
	m_combo_Flag.SetCurSel(0);
	m_combo_Real.SetCurSel(0);
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1857::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 종목검색(신버전API용)(t1857) ( attr,block,headtype=A )
	t1857InBlock	pckInBlock;


	TCHAR			szTrNo[]	= "t1857";
	char			szNextKey[]	= "";

	//-----------------------------------------------------------
	// 데이터 취득
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_INDEX1857, str_Index );		// 종목검색입력값
	CString str_Flag;		m_combo_Flag.GetLBText(m_combo_Flag.GetCurSel(),str_Flag);// 조회구분값
	str_Flag = str_Flag.Left(1);
	CString str_Real;		m_combo_Real.GetLBText(m_combo_Real.GetCurSel(),str_Real);// 실시간 여부
	str_Real = str_Real.Left(1);

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.sRealFlag		, sizeof( pckInBlock.sRealFlag		), str_Real	   , DATA_TYPE_STRING );	// 실시간 여부 1:등록 0:조회만
	SetPacketData( pckInBlock.sSearchFlag	, sizeof( pckInBlock.sSearchFlag	), str_Flag	   , DATA_TYPE_STRING );	// 조회구분값 S:서버 F:파일
	SetPacketData( pckInBlock.query_index	, sizeof( pckInBlock.query_index	), str_Index   , DATA_TYPE_STRING );	// 종목검색입력값

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.RequestService( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		(LPCTSTR)&pckInBlock
	);

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
LRESULT CDlg_t1857::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{

		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		if( strcmp( pRpData->szTrCode , "t1857" ) == 0 )
		{
			if( strcmp( pRpData->szBlockName, "t1857OutBlock" ) == 0 )
			{
				LPt1857OutBlock p1857OutBlock = (LPt1857OutBlock)pRpData->lpData;
				m_ctrlOutBlock2.InsertItem( 0, "" );
				m_ctrlOutBlock2.SetItemText( 0, 0, GetDispData( p1857OutBlock->AlertNum 	, sizeof( p1857OutBlock->AlertNum ), DATA_TYPE_STRING ) );
			}
			else if( strcmp( pRpData->szBlockName, "t1857OutBlock1" ) == 0 )
			{
				LPt1857OutBlock1 p1857OutBlock1 = (LPt1857OutBlock1)pRpData->lpData;

				int nCount = pRpData->nDataLength/sizeof(t1857OutBlock1);
				
				for( int i=0; i< nCount ; i++ )
				{
					m_ctrlOutBlock1.InsertItem( i, "" );
					m_ctrlOutBlock1.SetItemText( i,  0, GetDispData( p1857OutBlock1[i].shcode 	, sizeof( p1857OutBlock1[i].shcode	), DATA_TYPE_STRING ) );		// 종목코드          
					m_ctrlOutBlock1.SetItemText( i,  1, GetDispData( p1857OutBlock1[i].hname  	, sizeof( p1857OutBlock1[i].hname	), DATA_TYPE_STRING ) );		// 종목명            
					m_ctrlOutBlock1.SetItemText( i,  2, GetDispData( p1857OutBlock1[i].price   	, sizeof( p1857OutBlock1[i].price	), DATA_TYPE_LONG ) );			// 현재가            
					m_ctrlOutBlock1.SetItemText( i,  3, GetDispData( p1857OutBlock1[i].sign		, sizeof( p1857OutBlock1[i].sign	), DATA_TYPE_STRING ) );		// 전일대비구분      
					m_ctrlOutBlock1.SetItemText( i,  4, GetDispData( p1857OutBlock1[i].change  	, sizeof( p1857OutBlock1[i].change	), DATA_TYPE_LONG ) );			// 전일대비          
					m_ctrlOutBlock1.SetItemText( i,  5, GetDispData( p1857OutBlock1[i].diff 	, sizeof( p1857OutBlock1[i].diff	), DATA_TYPE_FLOAT_DOT,2 ) );	// 등락율            
					m_ctrlOutBlock1.SetItemText( i,  6, GetDispData( p1857OutBlock1[i].volume	, sizeof( p1857OutBlock1[i].volume	), DATA_TYPE_LONG ) );			// 거래량            
					CString strJobFlag;
					strJobFlag = GetDispData( p1857OutBlock1[i].JobFlag	, sizeof( p1857OutBlock1[i].JobFlag	), DATA_TYPE_STRING );
					if(strcmp( strJobFlag, "N" ) == 0)		strJobFlag = "진입";
					else if(strcmp( strJobFlag, "R" ) == 0)	strJobFlag = "재진입";
					else if(strcmp( strJobFlag, "O" ) == 0)	strJobFlag = "이탈";
					m_ctrlOutBlock1.SetItemText( i,  7, strJobFlag );			// 종목상태
				}
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
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1857::RequestData2( BOOL bNext )
{
	TCHAR			szTrNo[]	= "t1857";

	//-----------------------------------------------------------
	// 데이터 취득
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_REALKEY, str_Index );		// 서버저장인덱스

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID1 = g_iXingAPI.RemoveService(
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		(LPCTSTR)str_Index
	);

	//-----------------------------------------------------------
	// Request ID가 0보다 작을 경우에는 에러이다.
	if( nRqID1 < 0 )
	{
		MessageBox( "조회실패", "에러", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// Timeout 발생
//--------------------------------------------------------------------------------------
LRESULT CDlg_t1857::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );

	return 0L;
}

BOOL CDlg_t1857::PreTranslateMessage(MSG* pMsg) 
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

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_t1857::OnXMReceiveRealSearchData( WPARAM wParam, LPARAM lParam )
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	LPt1857OutBlock1 pOutBlock = (LPt1857OutBlock1)pRealPacket->pszData;
	int nRowCount = m_ctrlOutBlock1.GetItemCount();
	m_ctrlOutBlock1.InsertItem( 0, "" );
	m_ctrlOutBlock1.SetItemText( 0,  0, GetDispData( pOutBlock->shcode	, sizeof( pOutBlock->shcode	), DATA_TYPE_STRING ) );		// 종목코드          
	m_ctrlOutBlock1.SetItemText( 0,  1, GetDispData( pOutBlock->hname  	, sizeof( pOutBlock->hname 	), DATA_TYPE_STRING ) );		// 종목명            
	m_ctrlOutBlock1.SetItemText( 0,  2, GetDispData( pOutBlock->price   , sizeof( pOutBlock->price 	), DATA_TYPE_LONG ) );			// 현재가            
	m_ctrlOutBlock1.SetItemText( 0,  3, GetDispData( pOutBlock->sign	, sizeof( pOutBlock->sign	), DATA_TYPE_STRING ) );		// 전일대비구분      
	m_ctrlOutBlock1.SetItemText( 0,  4, GetDispData( pOutBlock->change 	, sizeof( pOutBlock->change	), DATA_TYPE_LONG ) );			// 전일대비          
	m_ctrlOutBlock1.SetItemText( 0,  5, GetDispData( pOutBlock->diff	, sizeof( pOutBlock->diff	), DATA_TYPE_FLOAT_DOT,2 ) );	// 등락율            
	m_ctrlOutBlock1.SetItemText( 0,  6, GetDispData( pOutBlock->volume	, sizeof( pOutBlock->volume	), DATA_TYPE_LONG ) );			// 거래량            
	CString strJobFlag;
	strJobFlag = GetDispData( pOutBlock->JobFlag	, sizeof( pOutBlock->JobFlag	), DATA_TYPE_STRING );
	if(strcmp( strJobFlag, "N" ) == 0)		strJobFlag = "진입";
	else if(strcmp( strJobFlag, "R" ) == 0)	strJobFlag = "재진입";
	else if(strcmp( strJobFlag, "O" ) == 0)	strJobFlag = "이탈";
	m_ctrlOutBlock1.SetItemText( 0,  7, strJobFlag );			// 종목상태

	return 0L;
}

void CDlg_t1857::OnClickKeylist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nIndex = pNMItemActivate->iItem;

	if(nIndex >= 0 && nIndex < m_ctrlOutBlock2.GetItemCount())
	{
		CString strKey = m_ctrlOutBlock2.GetItemText(nIndex,0);
		SetDlgItemText(IDC_INBLOCK_REALKEY,strKey);
	}

	*pResult = 0;
}
