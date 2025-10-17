// Dlg_t1452.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_t1452.h"

#include "./Packet/t1452.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1452 dialog

IMPLEMENT_DYNCREATE(CDlg_t1452, CDialog)

CDlg_t1452::CDlg_t1452(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_t1452::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_t1452)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_t1452::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_t1452)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_t1452, CDialog)
	//{{AFX_MSG_MAP(CDlg_t1452)
	ON_BN_CLICKED(IDC_BUTTON_EXCLUDE_ITEM, OnButtonExcludeItem)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_t1452 message handlers

BOOL CDlg_t1452::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_t1452::OnButtonRequest() 
{
	RequestData();
}

void CDlg_t1452::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_t1452::InitCtrls()
{
	//-----------------------------------------------------------
	// OutBlock
	InsertColumn4List( this, IDC_OUTBLOCK ,  0, "IDX           ", FALSE,  4 );    // [long  ,    4] CNT   
	
	//-----------------------------------------------------------
	// OutBlock1
	InsertColumn4List( this, IDC_OUTBLOCK1,  0, "종목명      ", TRUE , 20 );    // [string,   20] 종목명      
	InsertColumn4List( this, IDC_OUTBLOCK1,  1, "현재가      ", FALSE,  8 );    // [long  ,    8] 현재가      
	InsertColumn4List( this, IDC_OUTBLOCK1,  2, "전일대비구분", FALSE,  1 );    // [string,    1] 전일대비구분
	InsertColumn4List( this, IDC_OUTBLOCK1,  3, "전일대비    ", FALSE,  8 );    // [long  ,    8] 전일대비    
	InsertColumn4List( this, IDC_OUTBLOCK1,  4, "등락율      ", FALSE,  6 );    // [float ,  6.2] 등락율      
	InsertColumn4List( this, IDC_OUTBLOCK1,  5, "누적거래량  ", TRUE , 12 );    // [long  ,   12] 누적거래량  
	InsertColumn4List( this, IDC_OUTBLOCK1,  6, "회전율      ", FALSE,  6 );    // [float ,  6.2] 회전율      
	InsertColumn4List( this, IDC_OUTBLOCK1,  7, "전일거래량  ", FALSE, 12 );    // [long  ,   12] 전일거래량  
	InsertColumn4List( this, IDC_OUTBLOCK1,  8, "전일비      ", FALSE, 10 );    // [float , 10.2] 전일비      
	InsertColumn4List( this, IDC_OUTBLOCK1,  9, "종목코드    ", FALSE,  6 );    // [string,    6] 종목코드    
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_t1452::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// 기간별주가(t1452) ( attr,block,headtype=A )
	t1452InBlock	pckInBlock;

	TCHAR			szTrNo[]	= "t1452";

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );
	
	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.gubun    , sizeof( pckInBlock.gubun     ), this, IDC_INBLOCK_GUBUN    , DATA_TYPE_STRING );	// [string,    1] 구분      
	SetPacketData( pckInBlock.jnilgubun, sizeof( pckInBlock.jnilgubun ), this, IDC_INBLOCK_JNILGUBUN, DATA_TYPE_STRING );	// [string,    1] 전일구분  
	SetPacketData( pckInBlock.sdiff    , sizeof( pckInBlock.sdiff     ), this, IDC_INBLOCK_SDIFF    , DATA_TYPE_LONG   );	// [long  ,    3] 시작등락율
	SetPacketData( pckInBlock.ediff    , sizeof( pckInBlock.ediff     ), this, IDC_INBLOCK_EDIFF    , DATA_TYPE_LONG   );	// [long  ,    3] 종료등락율
	SetPacketData( pckInBlock.jc_num   , sizeof( pckInBlock.jc_num    ), this, IDC_INBLOCK_JC_NUM   , DATA_TYPE_LONG   );	// [long  ,   12] 대상제외  
	SetPacketData( pckInBlock.sprice   , sizeof( pckInBlock.sprice    ), this, IDC_INBLOCK_SPRICE   , DATA_TYPE_LONG   );	// [long  ,    8] 시작가격  
	SetPacketData( pckInBlock.eprice   , sizeof( pckInBlock.eprice    ), this, IDC_INBLOCK_EPRICE   , DATA_TYPE_LONG   );	// [long  ,    8] 종료가격  
	SetPacketData( pckInBlock.volume   , sizeof( pckInBlock.volume    ), this, IDC_INBLOCK_VOLUME   , DATA_TYPE_LONG   );	// [long  ,   12] 거래량    
	SetPacketData( pckInBlock.idx      , sizeof( pckInBlock.idx       ), this, IDC_INBLOCK_IDX      , DATA_TYPE_LONG   );	// [long  ,    4] IDX       

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부
		"",							// 다음조회 Key
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);
	
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK  ))->DeleteAllItems();
	((CListCtrl*)GetDlgItem( IDC_OUTBLOCK1 ))->DeleteAllItems();

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
LRESULT CDlg_t1452::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		// Block Mode 이다.
		if( strcmp( pRpData->szBlockName, NAME_t1452OutBlock ) == 0 )
		{
			CListCtrl*		pList	  = (CListCtrl*)GetDlgItem( IDC_OUTBLOCK );
			LPt1452OutBlock pOutBlock = (LPt1452OutBlock)pRpData->lpData;

			int nItem = pList->InsertItem( pList->GetItemCount(), "" );
			pList->SetItemText( nItem,  0, GetDispData( pOutBlock->idx , sizeof( pOutBlock->idx  ), DATA_TYPE_STRING ) );		// [long  ,    4] IDX   
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1452OutBlock1 ) == 0 )
		{
			CListCtrl*		 pList	   = (CListCtrl*)GetDlgItem( IDC_OUTBLOCK1 );
			LPt1452OutBlock1 pOutBlock = (LPt1452OutBlock1)pRpData->lpData;
			int				 nCount	   = pRpData->nDataLength / sizeof( t1452OutBlock1 );		// Block Mode 시엔 전체크기 / 하나의 Record 크기 로 갯수를 구한다.
			for( int i=0; i<nCount; i++ )
			{
				int nItem = pList->InsertItem( pList->GetItemCount(), "" );
				pList->SetItemText( nItem,  0, GetDispData( pOutBlock[i].hname     , sizeof( pOutBlock[i].hname      ), DATA_TYPE_STRING    ) );		// [string,   20] 종목명      
				pList->SetItemText( nItem,  1, GetDispData( pOutBlock[i].price     , sizeof( pOutBlock[i].price      ), DATA_TYPE_LONG      ) );		// [long  ,    8] 현재가      
				pList->SetItemText( nItem,  2, GetDispData( pOutBlock[i].sign      , sizeof( pOutBlock[i].sign       ), DATA_TYPE_STRING    ) );		// [string,    1] 전일대비구분
				pList->SetItemText( nItem,  3, GetDispData( pOutBlock[i].change    , sizeof( pOutBlock[i].change     ), DATA_TYPE_LONG      ) );		// [long  ,    8] 전일대비    
				pList->SetItemText( nItem,  4, GetDispData( pOutBlock[i].diff      , sizeof( pOutBlock[i].diff       ), DATA_TYPE_DOUBLE, 2 ) );		// [float ,  6.2] 등락율      
				pList->SetItemText( nItem,  5, GetDispData( pOutBlock[i].volume    , sizeof( pOutBlock[i].volume     ), DATA_TYPE_LONG      ) );		// [long  ,   12] 누적거래량  
				pList->SetItemText( nItem,  6, GetDispData( pOutBlock[i].vol       , sizeof( pOutBlock[i].vol        ), DATA_TYPE_DOUBLE, 2 ) );		// [float ,  6.2] 회전율      
				pList->SetItemText( nItem,  7, GetDispData( pOutBlock[i].jnilvolume, sizeof( pOutBlock[i].jnilvolume ), DATA_TYPE_LONG      ) );		// [long  ,   12] 전일거래량  
				pList->SetItemText( nItem,  8, GetDispData( pOutBlock[i].bef_diff  , sizeof( pOutBlock[i].bef_diff   ), DATA_TYPE_DOUBLE, 2 ) );		// [float , 10.2] 전일비      
				pList->SetItemText( nItem,  9, GetDispData( pOutBlock[i].shcode    , sizeof( pOutBlock[i].shcode     ), DATA_TYPE_STRING    ) );		// [string,    6] 종목코드    
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
LRESULT CDlg_t1452::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );

	return 0L;
}

#include "DlgExcludeItem.h"
void CDlg_t1452::OnButtonExcludeItem() 
{
	CDlgExcludeItem dlg;
	dlg.SetData( GetDlgItemInt( IDC_INBLOCK_JC_NUM ) );
	if( dlg.DoModal() == IDOK )
	{
		SetDlgItemInt( IDC_INBLOCK_JC_NUM, dlg.GetData() );
	}	
}
