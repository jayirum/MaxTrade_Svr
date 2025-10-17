// Dlg_C01.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_C01.h"

#include "./Packet/C01.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_C01 dialog

IMPLEMENT_DYNCREATE(CDlg_C01, CDialog)

CDlg_C01::CDlg_C01(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_C01::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_C01)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_C01::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_C01)
	DDX_Control(pDX, IDC_OUTBLOCK, m_ctrlOutBlock);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_C01, CDialog)
	//{{AFX_MSG_MAP(CDlg_C01)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_REAL_DATA,	OnXMReceiveRealData	)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_C01 message handlers

BOOL CDlg_C01::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 주문에 관련된 실시간TR은 입력값이 없으므로 화면이 오픈될때 등록한다.
	AdviseData();

	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_C01::OnDestroy() 
{
	UnadviseData();

	CDialog::OnDestroy();
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_C01::InitCtrls()
{
	//-------------------------------------------------------------------------
	// OutBlock
	m_ctrlOutBlock.InsertColumn( 0, "필드  ", LVCFMT_LEFT, 150 );
	m_ctrlOutBlock.InsertColumn( 1, "데이터", LVCFMT_LEFT, 200 );

	int nRow = 0;
	m_ctrlOutBlock.InsertItem( nRow++, "라인일련번호"		);
	m_ctrlOutBlock.InsertItem( nRow++, "계좌번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "조작자ID"			);
	m_ctrlOutBlock.InsertItem( nRow++, "일련번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "trcode"				);
	m_ctrlOutBlock.InsertItem( nRow++, "매칭그룹번호"		);
	m_ctrlOutBlock.InsertItem( nRow++, "보드ID"				);
	m_ctrlOutBlock.InsertItem( nRow++, "회원번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "지점번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "주문번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "원주문번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "종목코드"			);
	m_ctrlOutBlock.InsertItem( nRow++, "약정번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "체결가격"			);
	m_ctrlOutBlock.InsertItem( nRow++, "체결수량"			);
	m_ctrlOutBlock.InsertItem( nRow++, "세션ID"				);
	m_ctrlOutBlock.InsertItem( nRow++, "체결일자"			);
	m_ctrlOutBlock.InsertItem( nRow++, "체결시각"			);
	m_ctrlOutBlock.InsertItem( nRow++, "최근월체결가격"		);
	m_ctrlOutBlock.InsertItem( nRow++, "차근월체결가격"		);
	m_ctrlOutBlock.InsertItem( nRow++, "매도수구분"			);
	m_ctrlOutBlock.InsertItem( nRow++, "계좌번호1"			);
	m_ctrlOutBlock.InsertItem( nRow++, "시장조성호가구분"	);
	m_ctrlOutBlock.InsertItem( nRow++, "위탁사번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "대용주권계좌번호"	);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_filler"			);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_accno"			);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_filler1"		);
}

//--------------------------------------------------------------------------------------
// 데이터 Advise
//--------------------------------------------------------------------------------------
void CDlg_C01::AdviseData()
{
	TCHAR	szTrCode[]		= "C01";

	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.AdviseRealData( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		szTrCode,					// TR 번호
		"",							// C01는 빈 데이터를 넘겨야 한다.
		0							// C01의 데이터 길이는 0이다.
		);

	//-----------------------------------------------------------
	// 에러체크
	if( bSuccess == FALSE )
	{
		MessageBox( "조회실패", "에러", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터 Unadvise
//--------------------------------------------------------------------------------------
void CDlg_C01::UnadviseData()
{
	TCHAR	szTrCode[]		= "C01";
	
	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.UnadviseRealData( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		szTrCode,					// TR 번호
		"",							// C01는 빈 데이터를 넘겨야 한다.
		0							// C01의 데이터 길이는 0이다.
		);
	
	//-----------------------------------------------------------
	// 에러체크
	if( bSuccess == FALSE )
	{
		MessageBox( "조회실패", "에러", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_C01::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	if( strcmp( pRealPacket->szTrCode, "C01" ) == 0 )
	{
		LPC01_OutBlock pOutBlock = (LPC01_OutBlock)pRealPacket->pszData;
		int nRow = 0;
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->lineseq		, sizeof( pOutBlock->lineseq             ), DATA_TYPE_LONG        ) );    // 라인일련번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->accno		, sizeof( pOutBlock->accno				 ), DATA_TYPE_STRING      ) );    // 계좌번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->user        , sizeof( pOutBlock->user				 ), DATA_TYPE_STRING      ) );    // 조작자ID        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->seq         , sizeof( pOutBlock->seq				 ), DATA_TYPE_LONG        ) );    // 일련번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->trcode      , sizeof( pOutBlock->trcode              ), DATA_TYPE_STRING      ) );    // trcode        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->megrpno     , sizeof( pOutBlock->megrpno             ), DATA_TYPE_STRING      ) );    // 매칭그룹번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->boardid     , sizeof( pOutBlock->boardid             ), DATA_TYPE_STRING      ) );    // 보드ID        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->memberno    , sizeof( pOutBlock->memberno            ), DATA_TYPE_STRING      ) );    // 회원번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->bpno        , sizeof( pOutBlock->bpno				 ), DATA_TYPE_STRING      ) );    // 지점번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ordno       , sizeof( pOutBlock->ordno               ), DATA_TYPE_STRING      ) );    // 주문번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->orgordno    , sizeof( pOutBlock->orgordno            ), DATA_TYPE_STRING      ) );    // 원주문번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->expcode     , sizeof( pOutBlock->expcode             ), DATA_TYPE_STRING      ) );    // 종목코드        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->yakseq      , sizeof( pOutBlock->yakseq              ), DATA_TYPE_STRING      ) );    // 약정번호        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->cheprice    , sizeof( pOutBlock->cheprice            ), DATA_TYPE_FLOAT       ) );    // 체결가격        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->chevol      , sizeof( pOutBlock->chevol              ), DATA_TYPE_LONG        ) );    // 체결수량        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->sessionid   , sizeof( pOutBlock->sessionid           ), DATA_TYPE_STRING      ) );    // 세션ID        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->chedate     , sizeof( pOutBlock->chedate             ), DATA_TYPE_STRING      ) );    // 체결일자        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->chetime     , sizeof( pOutBlock->chetime             ), DATA_TYPE_STRING      ) );    // 체결시각        
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->spdprc1     , sizeof( pOutBlock->spdprc1             ), DATA_TYPE_FLOAT       ) );    // 최근월체결가격
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->spdprc2     , sizeof( pOutBlock->spdprc2             ), DATA_TYPE_FLOAT       ) );    // 차근월체결가격
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->dosugb      , sizeof( pOutBlock->dosugb              ), DATA_TYPE_STRING      ) );    // 매도수구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->accno1      , sizeof( pOutBlock->accno1              ), DATA_TYPE_STRING      ) );    // 계좌번호1
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->sihogagb    , sizeof( pOutBlock->sihogagb            ), DATA_TYPE_STRING      ) );    // 시장조성호가구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->jakino      , sizeof( pOutBlock->jakino              ), DATA_TYPE_STRING      ) );    // 위탁사번호
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->daeyong     , sizeof( pOutBlock->daeyong             ), DATA_TYPE_STRING      ) );    // 대용주권계좌번호
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_filler  , sizeof( pOutBlock->mem_filler          ), DATA_TYPE_STRING      ) );    // mem_filler
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_accno   , sizeof( pOutBlock->mem_accno           ), DATA_TYPE_STRING      ) );    // mem_accno
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_filler1 , sizeof( pOutBlock->mem_filler1         ), DATA_TYPE_STRING      ) );    // mem_filler1
	}

   	return 0L;
}
