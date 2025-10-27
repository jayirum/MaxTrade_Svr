// Dlg_H01.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_H01.h"

#include "./Packet/H01.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_H01 dialog

IMPLEMENT_DYNCREATE(CDlg_H01, CDialog)

CDlg_H01::CDlg_H01(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_H01::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_H01)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_H01::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_H01)
	DDX_Control(pDX, IDC_OUTBLOCK, m_ctrlOutBlock);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_H01, CDialog)
	//{{AFX_MSG_MAP(CDlg_H01)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_REAL_DATA,	OnXMReceiveRealData	)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_H01 message handlers

BOOL CDlg_H01::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 주문에 관련된 실시간TR은 입력값이 없으므로 화면이 오픈될때 등록한다.
	AdviseData();

	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_H01::OnDestroy() 
{
	UnadviseData();

	CDialog::OnDestroy();
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_H01::InitCtrls()
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
	m_ctrlOutBlock.InsertItem( nRow++, "매도수구분"			);
	m_ctrlOutBlock.InsertItem( nRow++, "정정취소구분"		);
	m_ctrlOutBlock.InsertItem( nRow++, "계좌번호1"			);
	m_ctrlOutBlock.InsertItem( nRow++, "호가수량"			);
	m_ctrlOutBlock.InsertItem( nRow++, "호가가격"			);
	m_ctrlOutBlock.InsertItem( nRow++, "주문유형"			);
	m_ctrlOutBlock.InsertItem( nRow++, "호가구분"			);
	m_ctrlOutBlock.InsertItem( nRow++, "시장조성호가구분"	);
	m_ctrlOutBlock.InsertItem( nRow++, "자사주신고서ID"		);
	m_ctrlOutBlock.InsertItem( nRow++, "자사주매매방법"		);
	m_ctrlOutBlock.InsertItem( nRow++, "매도유형코드"		);
	m_ctrlOutBlock.InsertItem( nRow++, "신용구분코드"		);
	m_ctrlOutBlock.InsertItem( nRow++, "위탁자기구분"		);
	m_ctrlOutBlock.InsertItem( nRow++, "위탁사번호"			);
	m_ctrlOutBlock.InsertItem( nRow++, "프로그램구분"		);
	m_ctrlOutBlock.InsertItem( nRow++, "대용주권계좌번호"	);
	m_ctrlOutBlock.InsertItem( nRow++, "계좌구분코드"		);
	m_ctrlOutBlock.InsertItem( nRow++, "계좌증거금코드"		);
	m_ctrlOutBlock.InsertItem( nRow++, "국가코드"			);
	m_ctrlOutBlock.InsertItem( nRow++, "투자자구분"			);
	m_ctrlOutBlock.InsertItem( nRow++, "외국인코드"			);
	m_ctrlOutBlock.InsertItem( nRow++, "주문매체구분"		);
	m_ctrlOutBlock.InsertItem( nRow++, "주문식별자번호"		);
	m_ctrlOutBlock.InsertItem( nRow++, "MAC주소"			);
	m_ctrlOutBlock.InsertItem( nRow++, "호가일자"			);
	m_ctrlOutBlock.InsertItem( nRow++, "회원사주문시각"		);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_filler"			);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_accno"			);
	m_ctrlOutBlock.InsertItem( nRow++, "mem_filler1"		);
	m_ctrlOutBlock.InsertItem( nRow++, "매칭접수시간"		);
	m_ctrlOutBlock.InsertItem( nRow++, "실정정취소수량"		);
	m_ctrlOutBlock.InsertItem( nRow++, "자동취소구분"		);
	m_ctrlOutBlock.InsertItem( nRow++, "거부사유"			);
	m_ctrlOutBlock.InsertItem( nRow++, "프로그램호가신고"	);
}

//--------------------------------------------------------------------------------------
// 데이터 Advise
//--------------------------------------------------------------------------------------
void CDlg_H01::AdviseData()
{
	TCHAR	szTrCode[]		= "H01";

	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.AdviseRealData( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		szTrCode,					// TR 번호
		"",							// H01는 빈 데이터를 넘겨야 한다.
		0							// H01의 데이터 길이는 0이다.
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
void CDlg_H01::UnadviseData()
{
	TCHAR	szTrCode[]		= "H01";
	
	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.UnadviseRealData( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		szTrCode,					// TR 번호
		"",							// H01는 빈 데이터를 넘겨야 한다.
		0							// H01의 데이터 길이는 0이다.
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
LRESULT CDlg_H01::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	if( strcmp( pRealPacket->szTrCode, "H01" ) == 0 )
	{
		LPH01_OutBlock pOutBlock = (LPH01_OutBlock)pRealPacket->pszData;
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
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->dosugb      , sizeof( pOutBlock->dosugb              ), DATA_TYPE_STRING      ) );    // 매도수구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mocagb      , sizeof( pOutBlock->mocagb              ), DATA_TYPE_STRING      ) );    // 정정취소구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->accno1      , sizeof( pOutBlock->accno1              ), DATA_TYPE_STRING      ) );    // 계좌번호1
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->qty2        , sizeof( pOutBlock->qty2				 ), DATA_TYPE_LONG        ) );    // 호가수량
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->price       , sizeof( pOutBlock->price				 ), DATA_TYPE_FLOAT       ) );    // 호가가격
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ordgb       , sizeof( pOutBlock->ordgb               ), DATA_TYPE_STRING      ) );    // 주문유형
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->hogagb      , sizeof( pOutBlock->hogagb              ), DATA_TYPE_STRING      ) );    // 호가구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->sihogagb    , sizeof( pOutBlock->sihogagb            ), DATA_TYPE_STRING      ) );    // 시장조성호가구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->tradid      , sizeof( pOutBlock->tradid              ), DATA_TYPE_STRING      ) );    // 자사주신고서ID
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->treacode    , sizeof( pOutBlock->treacode            ), DATA_TYPE_STRING      ) );    // 자사주매매방법
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->askcode     , sizeof( pOutBlock->askcode             ), DATA_TYPE_STRING      ) );    // 매도유형코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->creditcode  , sizeof( pOutBlock->creditcode          ), DATA_TYPE_STRING      ) );    // 신용구분코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->jakigb      , sizeof( pOutBlock->jakigb              ), DATA_TYPE_STRING      ) );    // 위탁자기구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->trustnum    , sizeof( pOutBlock->trustnum            ), DATA_TYPE_STRING      ) );    // 위탁사번호
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ptgb        , sizeof( pOutBlock->ptgb				 ), DATA_TYPE_STRING      ) );    // 프로그램구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->substonum   , sizeof( pOutBlock->substonum           ), DATA_TYPE_STRING      ) );    // 대용주권계좌번호
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->accgb       , sizeof( pOutBlock->accgb				 ), DATA_TYPE_STRING      ) );    // 계좌구분코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->accmarggb   , sizeof( pOutBlock->accmarggb           ), DATA_TYPE_STRING      ) );    // 계좌증거금코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->nationcode  , sizeof( pOutBlock->nationcode          ), DATA_TYPE_STRING      ) );    // 국가코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->investgb    , sizeof( pOutBlock->investgb            ), DATA_TYPE_STRING      ) );    // 투자자구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->forecode    , sizeof( pOutBlock->forecode            ), DATA_TYPE_STRING      ) );    // 외국인코드
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->medcode     , sizeof( pOutBlock->medcode             ), DATA_TYPE_STRING      ) );    // 주문매체구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ordid       , sizeof( pOutBlock->ordid				 ), DATA_TYPE_STRING      ) );    // 주문식별자번호
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->macid       , sizeof( pOutBlock->macid				 ), DATA_TYPE_STRING      ) );    // MAC주소
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->orddate     , sizeof( pOutBlock->orddate             ), DATA_TYPE_STRING      ) );    // 호가일자
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->rcvtime     , sizeof( pOutBlock->rcvtime             ), DATA_TYPE_STRING      ) );    // 회원사주문시각
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_filler  , sizeof( pOutBlock->mem_filler          ), DATA_TYPE_STRING      ) );    // mem_filler
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_accno   , sizeof( pOutBlock->mem_accno           ), DATA_TYPE_STRING      ) );    // mem_accno
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->mem_filler1 , sizeof( pOutBlock->mem_filler1         ), DATA_TYPE_STRING      ) );    // mem_filler1
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->ordacpttm   , sizeof( pOutBlock->ordacpttm			 ), DATA_TYPE_STRING      ) );    // 매칭접수시간
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->qty         , sizeof( pOutBlock->qty				 ), DATA_TYPE_LONG	      ) );    // 실정정취소수량
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->autogb      , sizeof( pOutBlock->autogb				 ), DATA_TYPE_STRING      ) );    // 자동취소구분
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->rejcode     , sizeof( pOutBlock->rejcode			 ), DATA_TYPE_STRING      ) );    // 거부사유
		m_ctrlOutBlock.SetItemText(  nRow++, 1, GetDispData( pOutBlock->prgordde   , sizeof( pOutBlock->prgordde			 ), DATA_TYPE_STRING      ) );    // 프로그램호가신고
	}

   	return 0L;
}
