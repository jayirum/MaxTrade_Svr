// Dlg_CEXAT11200.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CEXAT11200.h"

#include "./Packet/CEXAT11200.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CEXAT11200 dialog


IMPLEMENT_DYNCREATE(CDlg_CEXAT11200, CDialog)

CDlg_CEXAT11200::CDlg_CEXAT11200(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CEXAT11200::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CEXAT11200)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CEXAT11200::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CEXAT11200)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
}


BEGIN_MESSAGE_MAP(CDlg_CEXAT11200, CDialog)
	//{{AFX_MSG_MAP(CDlg_CEXAT11200)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CEXAT11200 message handlers

BOOL CDlg_CEXAT11200::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CEXAT11200::OnButtonRequest() 
{
	RequestData();
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CEXAT11200::InitCtrls()
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
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수            ", FALSE,    5 );    // [long  ,    5] 레코드갯수            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "원주문번호            ", FALSE,   10 );    // [long  ,   10] 원주문번호            
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호              ", TRUE ,   20 );    // [string,   20] 계좌번호              
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "비밀번호              ", TRUE ,    8 );    // [string,    8] 비밀번호              
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "선물옵션종목번호      ", TRUE ,   12 );    // [string,   12] 선물옵션종목번호      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "매매구분코드          ", TRUE ,    1 );    // [string,    1] 매매구분코드          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "유렉스가격조건구분코드", FALSE,    1 );    // [string,    1] 유렉스가격조건구분코드
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문가격              ", FALSE, 15.2 );    // [double, 15.2] 주문가격              
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "정정수량              ", FALSE,   16 );    // [long  ,   16] 정정수량              
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "주문조건가격          ", FALSE, 25.8 );    // [double, 25.8] 주문조건가격          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "통신매체코드          ", TRUE ,    2 );    // [string,    2] 통신매체코드          

	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수      ", FALSE,    5 );    // [long  ,    5] 레코드갯수             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문번호        ", FALSE,   10 );    // [long  ,   10] 주문번호               
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "지점명          ", TRUE ,   40 );    // [string,   40] 지점명                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명          ", TRUE ,   40 );    // [string,   40] 계좌명                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "종목명          ", TRUE ,   50 );    // [string,   50] 종목명                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문가능금액    ", FALSE,   16 );    // [long  ,   16] 주문가능금액           
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문가능금액", FALSE,   16 );    // [long  ,   16] 현금주문가능금액       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문증거금액    ", FALSE,   16 );    // [long  ,   16] 주문증거금             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문증거금액", FALSE,   16 );    // [long  ,   16] 현금주문증거금액         
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문가능수량    ", FALSE,   16 );    // [long  ,   16] 주문가능수량           
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CEXAT11200::RequestData( BOOL bNext )
{
	CEXAT11200InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CEXAT11200";
	char			szNextKey[]		= "";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strOrgOrdNo;	GetDlgItemText( IDC_INBLOCK_ORGORDNO, strOrgOrdNo );    // [long  ,   10] 원주문번호       
    CString strAcntNo  ;	GetDlgItemText( IDC_INBLOCK_ACNTNO  , strAcntNo   );    // [string,   20] 계좌번호         
    CString strPwd     ;	GetDlgItemText( IDC_INBLOCK_PWD     , strPwd      );    // [string,    8] 비밀번호         
    CString strFnoIsuNo;	GetDlgItemText( IDC_INBLOCK_FNOISUNO, strFnoIsuNo );    // [string,   12] 선물옵션종목번호 
    CString strOrdPrc  ;	GetDlgItemText( IDC_INBLOCK_ORDPRC  , strOrdPrc   );    // [double, 15.2] 주문가격         

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.OrgOrdNo, sizeof( pckInBlock.OrgOrdNo ), strOrgOrdNo, DATA_TYPE_LONG   );    // [long  ,   10] 원주문번호       
	SetPacketData( pckInBlock.AcntNo  , sizeof( pckInBlock.AcntNo   ), strAcntNo  , DATA_TYPE_STRING );    // [string,   20] 계좌번호         
	SetPacketData( pckInBlock.Pwd     , sizeof( pckInBlock.Pwd      ), strPwd     , DATA_TYPE_STRING );    // [string,    8] 비밀번호         
	SetPacketData( pckInBlock.FnoIsuNo, sizeof( pckInBlock.FnoIsuNo ), strFnoIsuNo, DATA_TYPE_STRING );    // [string,   12] 선물옵션종목번호 
	SetPacketData( pckInBlock.OrdPrc  , sizeof( pckInBlock.OrdPrc   ), strOrdPrc  , DATA_TYPE_DOT, 2 );    // [double, 15.2] 주문가격         

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
		CString strMsg;
		strMsg.Format( "[%d] %s", nRqID, g_iXingAPI.GetErrorMessage( nRqID ) );
		MessageBox( strMsg, "조회실패", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_CEXAT11200::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		typedef struct
		{
			CEXAT11200OutBlock1	outBlock1;
			CEXAT11200OutBlock2	outBlock2;
		} CEXAT11200AllOutBlock, *LPCEXAT11200AllOutBlock;

		LPRECV_PACKET			pRpData		 = (LPRECV_PACKET)lParam;
		LPCEXAT11200AllOutBlock pAllOutBlock = (LPCEXAT11200AllOutBlock)pRpData->lpData;
		int						nDataLength  = pRpData->nDataLength;
		int						nCol;
		
		//------------------------------------------------
		// OutBlock1
		nDataLength -= sizeof( CEXAT11200OutBlock1 );
		if( nDataLength < 0 )
		{
			return 0L;
		}

		nCol = 0;
		m_ctrlOutBlock1.InsertItem( 0, "" );
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt          , sizeof( pAllOutBlock->outBlock1.RecCnt           ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수            
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrgOrdNo        , sizeof( pAllOutBlock->outBlock1.OrgOrdNo         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 원주문번호            
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo          , sizeof( pAllOutBlock->outBlock1.AcntNo           ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호              
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.Pwd             , sizeof( pAllOutBlock->outBlock1.Pwd              ), DATA_TYPE_STRING ) );    // [string,    8] 비밀번호              
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.FnoIsuNo        , sizeof( pAllOutBlock->outBlock1.FnoIsuNo         ), DATA_TYPE_STRING ) );    // [string,   12] 선물옵션종목번호      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BnsTpCode       , sizeof( pAllOutBlock->outBlock1.BnsTpCode        ), DATA_TYPE_STRING ) );    // [string,    1] 매매구분코드            
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.ErxPrcCndiTpCode, sizeof( pAllOutBlock->outBlock1.ErxPrcCndiTpCode ), DATA_TYPE_STRING ) );    // [string,    1] 유렉스가격조건구분코드
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdPrc          , sizeof( pAllOutBlock->outBlock1.OrdPrc           ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 주문가격              
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.MdfyQty         , sizeof( pAllOutBlock->outBlock1.MdfyQty          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 정정수량              
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.OrdCndiPrc      , sizeof( pAllOutBlock->outBlock1.OrdCndiPrc       ), DATA_TYPE_DOT, 8 ) );    // [double, 25.8] 주문조건가격          
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CommdaCode      , sizeof( pAllOutBlock->outBlock1.CommdaCode       ), DATA_TYPE_LONG   ) );    // [string,    2] 통신매체코드          

		//------------------------------------------------
		// OutBlock1
		nDataLength -= sizeof( CEXAT11200OutBlock2 );
		if( nDataLength < 0 )
		{
			return 0L;
		}
		
		nCol = 0;
		m_ctrlOutBlock2.InsertItem( 0, "" );
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt       , sizeof( pAllOutBlock->outBlock2.RecCnt        ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수         
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdNo        , sizeof( pAllOutBlock->outBlock2.OrdNo         ), DATA_TYPE_LONG   ) );    // [long  ,   10] 주문번호           
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.BrnNm        , sizeof( pAllOutBlock->outBlock2.BrnNm         ), DATA_TYPE_STRING ) );    // [string,   40] 지점명             
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm       , sizeof( pAllOutBlock->outBlock2.AcntNm        ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명             
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.IsuNm        , sizeof( pAllOutBlock->outBlock2.IsuNm         ), DATA_TYPE_STRING ) );    // [string,   50] 종목명             
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAbleAmt   , sizeof( pAllOutBlock->outBlock2.OrdAbleAmt    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문가능금액       
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAbleAmt, sizeof( pAllOutBlock->outBlock2.MnyOrdAbleAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문가능금액   
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdMgn       , sizeof( pAllOutBlock->outBlock2.OrdMgn        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문증거금액       
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdMgn    , sizeof( pAllOutBlock->outBlock2.MnyOrdMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문증거금액   
		m_ctrlOutBlock2.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdAbleQty   , sizeof( pAllOutBlock->outBlock2.OrdAbleQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문가능수량       
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
LRESULT CDlg_CEXAT11200::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
