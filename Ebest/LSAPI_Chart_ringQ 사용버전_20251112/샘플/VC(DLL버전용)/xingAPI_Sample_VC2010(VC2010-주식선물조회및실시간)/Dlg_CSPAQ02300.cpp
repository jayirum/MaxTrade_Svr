// Dlg_CSPAQ02300.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "Dlg_CSPAQ02300.h"

#include "./Packet/CSPAQ12300.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ02300 dialog


IMPLEMENT_DYNCREATE(CDlg_CSPAQ02300, CDialog)

CDlg_CSPAQ02300::CDlg_CSPAQ02300(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_CSPAQ02300::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_CSPAQ02300)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_CSPAQ02300::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_CSPAQ02300)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OUTBLOCK1, m_ctrlOutBlock1);
	DDX_Control(pDX, IDC_OUTBLOCK2, m_ctrlOutBlock2);
	DDX_Control(pDX, IDC_OUTBLOCK3, m_ctrlOutBlock3);
}


BEGIN_MESSAGE_MAP(CDlg_CSPAQ02300, CDialog)
	//{{AFX_MSG_MAP(CDlg_CSPAQ02300)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,		  OnButtonRequest	  )
	ON_BN_CLICKED( IDC_BUTTON_REQUEST_NEXT,	  OnButtonRequestNext )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_DATA, OnXMReceiveData	  )
	ON_MESSAGE	 ( WM_USER + XM_TIMEOUT_DATA, OnXMTimeoutData	  )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_CSPAQ02300 message handlers

BOOL CDlg_CSPAQ02300::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_CSPAQ02300::OnButtonRequest() 
{
	RequestData();
}

void CDlg_CSPAQ02300::OnButtonRequestNext() 
{
	RequestData( TRUE );
}

//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ02300::InitCtrls()
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

	((CComboBox*)GetDlgItem( IDC_INBLOCK_ACNTNO         ))->SetCurSel( 0 );	// 계좌번호
	((CComboBox*)GetDlgItem( IDC_INBLOCK_BALCRETP       ))->SetCurSel( 0 );	// 잔고생성구분             
	((CComboBox*)GetDlgItem( IDC_INBLOCK_CMSNAPPTPCODE  ))->SetCurSel( 0 );	// 수수료적용구분         
	((CComboBox*)GetDlgItem( IDC_INBLOCK_D2BALBASEQRYTP ))->SetCurSel( 0 );	// D2잔고기준조회구분         
	((CComboBox*)GetDlgItem( IDC_INBLOCK_UPRCTPCODE     ))->SetCurSel( 0 );	// 단가구분         

	//-------------------------------------------------------------------------
	// OutBlock1
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "레코드갯수        ", FALSE,    5 );    // [long  ,    5] 레코드갯수        
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "계좌번호          ", TRUE ,   20 );    // [string,   20] 계좌번호          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "비밀번호          ", TRUE ,    8 );    // [string,    8] 비밀번호          
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "잔고생성구분      ", TRUE ,    1 );    // [string,    1] 잔고생성구분      
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "수수료적용구분    ", TRUE ,    1 );    // [string,    1] 수수료적용구분    
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "D2잔고기준조회구분", TRUE ,    1 );    // [string,    1] D2잔고기준조회구분
	InsertColumn4List( this, IDC_OUTBLOCK1, nCol++, "단가구분          ", TRUE ,    1 );    // [string,    1] 단가구분          
	
	//-------------------------------------------------------------------------
	// OutBlock2
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "레코드갯수                   ", FALSE,    5 );    // [long  ,    5] 레코드갯수                   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "지점명                       ", TRUE ,   40 );    // [string,   40] 지점명                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "계좌명                       ", TRUE ,   40 );    // [string,   40] 계좌명                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금주문가능금액             ", FALSE,   16 );    // [long  ,   16] 현금주문가능금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "출금가능금액                 ", FALSE,   16 );    // [long  ,   16] 출금가능금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "거래소금액                   ", FALSE,   16 );    // [long  ,   16] 거래소금액                   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "코스닥금액                   ", FALSE,   16 );    // [long  ,   16] 코스닥금액                   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "HTS주문가능금액              ", FALSE,   16 );    // [long  ,   16] HTS주문가능금액              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "증거금률100퍼센트주문가능금액", FALSE,   16 );    // [long  ,   16] 증거금률100퍼센트주문가능금액
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "잔고평가금액                 ", FALSE,   16 );    // [long  ,   16] 잔고평가금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매입금액                     ", FALSE,   16 );    // [long  ,   16] 매입금액                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "미수금액                     ", FALSE,   16 );    // [long  ,   16] 미수금액                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "손익율                       ", FALSE, 18.6 );    // [double, 18.6] 손익율                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "투자원금                     ", FALSE,   20 );    // [long  ,   20] 투자원금                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "투자손익금액                 ", FALSE,   16 );    // [long  ,   16] 투자손익금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "신용담보주문금액             ", FALSE,   16 );    // [long  ,   16] 신용담보주문금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예수금                       ", FALSE,   16 );    // [long  ,   16] 예수금                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1예수금                     ", FALSE,   16 );    // [long  ,   16] D1예수금                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2예수금                     ", FALSE,   16 );    // [long  ,   16] D2예수금                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "주문일                       ", TRUE ,    8 );    // [string,    8] 주문일                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "현금증거금액                 ", FALSE,   16 );    // [long  ,   16] 현금증거금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용증거금액                 ", FALSE,   16 );    // [long  ,   16] 대용증거금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대용금액                     ", FALSE,   16 );    // [long  ,   16] 대용금액                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일매수체결금액             ", FALSE,   16 );    // [long  ,   16] 전일매수체결금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일매도체결금액             ", FALSE,   16 );    // [long  ,   16] 전일매도체결금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일매수체결금액             ", FALSE,   16 );    // [long  ,   16] 금일매수체결금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일매도체결금액             ", FALSE,   16 );    // [long  ,   16] 금일매도체결금액             
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "평가손익합계                 ", FALSE,   15 );    // [long  ,   15] 평가손익합계                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예탁자산총액                 ", FALSE,   16 );    // [long  ,   16] 예탁자산총액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "제비용                       ", FALSE,   19 );    // [long  ,   19] 제비용                       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "재사용금액                   ", FALSE,   16 );    // [long  ,   16] 재사용금액                   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "기타대여금액                 ", FALSE,   16 );    // [long  ,   16] 기타대여금액                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "가정산금액                   ", FALSE,   16 );    // [long  ,   16] 가정산금액                   
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1수수료                     ", FALSE,   16 );    // [long  ,   16] D1수수료                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2수수료                     ", FALSE,   16 );    // [long  ,   16] D2수수료                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1제세금                     ", FALSE,   16 );    // [long  ,   16] D1제세금                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2제세금                     ", FALSE,   16 );    // [long  ,   16] D2제세금                     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D1결제예정금액               ", FALSE,   16 );    // [long  ,   16] D1결제예정금액               
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "D2결제예정금액               ", FALSE,   16 );    // [long  ,   16] D2결제예정금액               
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일KSE현금증거금            ", FALSE,   16 );    // [long  ,   16] 전일KSE현금증거금            
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일KSE대용증거금            ", FALSE,   16 );    // [long  ,   16] 전일KSE대용증거금            
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일KSE신용현금증거금        ", FALSE,   16 );    // [long  ,   16] 전일KSE신용현금증거금        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일KSE신용대용증거금        ", FALSE,   16 );    // [long  ,   16] 전일KSE신용대용증거금        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일KSE현금증거금            ", FALSE,   16 );    // [long  ,   16] 금일KSE현금증거금            
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일KSE대용증거금            ", FALSE,   16 );    // [long  ,   16] 금일KSE대용증거금            
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일KSE신용현금증거금        ", FALSE,   16 );    // [long  ,   16] 금일KSE신용현금증거금        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일KSE신용대용증거금        ", FALSE,   16 );    // [long  ,   16] 금일KSE신용대용증거금        
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일코스닥현금증거금         ", FALSE,   16 );    // [long  ,   16] 전일코스닥현금증거금         
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일코스닥대용증거금         ", FALSE,   16 );    // [long  ,   16] 전일코스닥대용증거금         
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일코스닥신용현금증거금     ", FALSE,   16 );    // [long  ,   16] 전일코스닥신용현금증거금     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일코스닥신용대용증거금     ", FALSE,   16 );    // [long  ,   16] 전일코스닥신용대용증거금     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일코스닥현금증거금         ", FALSE,   16 );    // [long  ,   16] 금일코스닥현금증거금         
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일코스닥대용증거금         ", FALSE,   16 );    // [long  ,   16] 금일코스닥대용증거금         
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일코스닥신용현금증거금     ", FALSE,   16 );    // [long  ,   16] 금일코스닥신용현금증거금     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일코스닥신용대용증거금     ", FALSE,   16 );    // [long  ,   16] 금일코스닥신용대용증거금     
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일프리보드현금증거금       ", FALSE,   16 );    // [long  ,   16] 전일프리보드현금증거금       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일프리보드대용증거금       ", FALSE,   16 );    // [long  ,   16] 전일프리보드대용증거금       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일프리보드현금증거금       ", FALSE,   16 );    // [long  ,   16] 금일프리보드현금증거금       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일프리보드대용증거금       ", FALSE,   16 );    // [long  ,   16] 금일프리보드대용증거금       
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일장외현금증거금           ", FALSE,   16 );    // [long  ,   16] 전일장외현금증거금           
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "전일장외대용증거금           ", FALSE,   16 );    // [long  ,   16] 전일장외대용증거금           
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일장외현금증거금           ", FALSE,   16 );    // [long  ,   16] 금일장외현금증거금           
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "금일장외대용증거금           ", FALSE,   16 );    // [long  ,   16] 금일장외대용증거금           
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "예탁담보수량                 ", FALSE,   16 );    // [long  ,   16] 예탁담보수량                 
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매수정산금(D+2)              ", FALSE,   16 );    // [long  ,   16] 매수정산금(D+2)              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "매도정산금(D+2)              ", FALSE,   16 );    // [long  ,   16] 매도정산금(D+2)              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "변제소요금(D+1)              ", FALSE,   16 );    // [long  ,   16] 변제소요금(D+1)              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "변제소요금(D+2)              ", FALSE,   16 );    // [long  ,   16] 변제소요금(D+2)              
	InsertColumn4List( this, IDC_OUTBLOCK2, nCol++, "대출금액                     ", FALSE,   16 );    // [long  ,   16] 대출금액                     
	
	//-------------------------------------------------------------------------
	// OutBlock3
	nCol = 0;
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "종목번호            ", TRUE ,   12 );    // [string,   12] 종목번호            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "종목명              ", TRUE ,   40 );    // [string,   40] 종목명              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "유가증권잔고유형코드", TRUE ,    2 );    // [string,    2] 유가증권잔고유형코드
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "유가증권잔고유형명  ", TRUE ,   40 );    // [string,   40] 유가증권잔고유형명  
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "잔고수량            ", FALSE,   16 );    // [long  ,   16] 잔고수량            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매매기준잔고수량    ", FALSE,   16 );    // [long  ,   16] 매매기준잔고수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "금일매수체결수량    ", FALSE,   16 );    // [long  ,   16] 금일매수체결수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "금일매도체결수량    ", FALSE,   16 );    // [long  ,   16] 금일매도체결수량    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도가              ", FALSE, 21.4 );    // [double, 21.4] 매도가              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매수가              ", FALSE, 21.4 );    // [double, 21.4] 매수가              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도손익금액        ", FALSE,   16 );    // [long  ,   16] 매도손익금액        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "손익율              ", FALSE, 18.6 );    // [double, 18.6] 손익율              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "현재가              ", FALSE, 15.2 );    // [double, 15.2] 현재가              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "신용금액            ", FALSE,   16 );    // [long  ,   16] 신용금액            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "만기일              ", TRUE ,    8 );    // [string,    8] 만기일              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매도체결가      ", FALSE, 13.2 );    // [double, 13.2] 전일매도체결가      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매도수량        ", FALSE,   16 );    // [long  ,   16] 전일매도수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매수체결가      ", FALSE, 13.2 );    // [double, 13.2] 전일매수체결가      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매수수량        ", FALSE,   16 );    // [long  ,   16] 전일매수수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "대출일              ", TRUE ,    8 );    // [string,    8] 대출일              
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "평균단가            ", FALSE, 13.2 );    // [double, 13.2] 평균단가            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도가능수량        ", FALSE,   16 );    // [long  ,   16] 매도가능수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도주문수량        ", FALSE,   16 );    // [long  ,   16] 매도주문수량        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "금일매수체결금액    ", FALSE,   16 );    // [long  ,   16] 금일매수체결금액    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "금일매도체결금액    ", FALSE,   16 );    // [long  ,   16] 금일매도체결금액    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매수체결금액    ", FALSE,   16 );    // [long  ,   16] 전일매수체결금액    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일매도체결금액    ", FALSE,   16 );    // [long  ,   16] 전일매도체결금액    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "잔고평가금액        ", FALSE,   16 );    // [long  ,   16] 잔고평가금액        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "평가손익            ", FALSE,   16 );    // [long  ,   16] 평가손익            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "현금주문가능금액    ", FALSE,   16 );    // [long  ,   16] 현금주문가능금액    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "주문가능금액        ", FALSE,   16 );    // [long  ,   16] 주문가능금액        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도미체결수량      ", FALSE,   16 );    // [long  ,   16] 매도미체결수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매도미결제수량      ", FALSE,   16 );    // [long  ,   16] 매도미결제수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매수미체결수량      ", FALSE,   16 );    // [long  ,   16] 매수미체결수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매수미결제수량      ", FALSE,   16 );    // [long  ,   16] 매수미결제수량      
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "미결제수량          ", FALSE,   16 );    // [long  ,   16] 미결제수량          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "미체결수량          ", FALSE,   16 );    // [long  ,   16] 미체결수량          
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "전일종가            ", FALSE, 15.2 );    // [double, 15.2] 전일종가            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "매입금액            ", FALSE,   16 );    // [long  ,   16] 매입금액            
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "등록시장코드        ", TRUE ,    2 );    // [string,    2] 등록시장코드        
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "대출상세분류코드    ", TRUE ,    2 );    // [string,    2] 대출상세분류코드    
	InsertColumn4List( this, IDC_OUTBLOCK3, nCol++, "예탁담보대출수량    ", FALSE,   16 );    // [long  ,   16] 예탁담보대출수량    

	ZeroMemory( m_szContKey, sizeof( m_szContKey ) );
}

//--------------------------------------------------------------------------------------
// 데이터 조회
//--------------------------------------------------------------------------------------
void CDlg_CSPAQ02300::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
// 현물정상주문(CSPAQ02300) ( base21=SONAT000,headtype=B )
	CSPAQ12300InBlock1	pckInBlock;

	TCHAR			szTrNo[]		= "CSPAQ12300";

	//-----------------------------------------------------------
	// 데이터 취득
    CString strAcntNo        ;		GetDlgItemText( IDC_INBLOCK_ACNTNO        , strAcntNo         );    // [long  ,    5] 레코드갯수        
    CString strPwd           ;		GetDlgItemText( IDC_INBLOCK_PWD           , strPwd            );    // [string,   20] 계좌번호          
    CString strBalCreTp      ;		GetDlgItemText( IDC_INBLOCK_BALCRETP      , strBalCreTp       );    // [string,    8] 비밀번호          
    CString strCmsnAppTpCode ;		GetDlgItemText( IDC_INBLOCK_CMSNAPPTPCODE , strCmsnAppTpCode  );    // [string,    1] 잔고생성구분      
    CString strD2balBaseQryTp;		GetDlgItemText( IDC_INBLOCK_D2BALBASEQRYTP, strD2balBaseQryTp );    // [string,    1] 수수료적용구분    
    CString strUprcTpCode    ;		GetDlgItemText( IDC_INBLOCK_UPRCTPCODE    , strUprcTpCode     );    // [string,    1] D2잔고기준조회구분

	//-----------------------------------------------------------
	// 데이터 초기화
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// 데이터 입력
	SetPacketData( pckInBlock.RecCnt        , sizeof( pckInBlock.RecCnt         ), "1"              , DATA_TYPE_LONG         );    // [long  ,    5] 레코드갯수        
	SetPacketData( pckInBlock.AcntNo        , sizeof( pckInBlock.AcntNo         ), strAcntNo        , DATA_TYPE_STRING       );    // [string,   20] 계좌번호          
	SetPacketData( pckInBlock.Pwd           , sizeof( pckInBlock.Pwd            ), strPwd           , DATA_TYPE_STRING       );    // [string,    8] 비밀번호          
	SetPacketData( pckInBlock.BalCreTp      , sizeof( pckInBlock.BalCreTp       ), strBalCreTp      , DATA_TYPE_STRING       );    // [string,    1] 잔고생성구분      
	SetPacketData( pckInBlock.CmsnAppTpCode , sizeof( pckInBlock.CmsnAppTpCode  ), strCmsnAppTpCode , DATA_TYPE_STRING       );    // [string,    1] 수수료적용구분    
	SetPacketData( pckInBlock.D2balBaseQryTp, sizeof( pckInBlock.D2balBaseQryTp ), strD2balBaseQryTp, DATA_TYPE_STRING       );    // [string,    1] D2잔고기준조회구분
	SetPacketData( pckInBlock.UprcTpCode    , sizeof( pckInBlock.UprcTpCode     ), strUprcTpCode    , DATA_TYPE_STRING       );    // [string,    1] 단가구분          

	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		szTrNo,						// TR 번호
		&pckInBlock,				// InBlock 데이터
		sizeof( pckInBlock ),		// InBlock 데이터 크기
		bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
		bNext ? m_szContKey : "",	// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
		30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
	);

	m_ctrlOutBlock1.DeleteAllItems();
	m_ctrlOutBlock2.DeleteAllItems();
	if( bNext == FALSE )
	{
		m_ctrlOutBlock3.DeleteAllItems();
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
LRESULT CDlg_CSPAQ02300::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if( wParam == REQUEST_DATA )
	{
		typedef struct
		{
			CSPAQ12300OutBlock1	outBlock1;
			CSPAQ12300OutBlock2	outBlock2;
			char				sCountOutBlock3[5];
			CSPAQ12300OutBlock3	outBlock3[1];
		} CSPAQ12300AllOutBlock, *LPCSPAQ12300AllOutBlock;

		LPRECV_PACKET			pRpData		 = (LPRECV_PACKET)lParam;
		LPCSPAQ12300AllOutBlock pAllOutBlock = (LPCSPAQ12300AllOutBlock)pRpData->lpData;
		int						nDataLength  = pRpData->nDataLength;
		int						nCol;
		int						nRow;
		int						nCount;
		char					szCount[6] = { 0 };

		// 연속키를 저장한다.
		strcpy_s( m_szContKey, pRpData->szContKey );

		//------------------------------------------------
		// OutBlock1

		// 받은 데이터 크기가 Block 크기보다 작다면 처리하지 않는다.
		nDataLength -= sizeof( CSPAQ12300OutBlock1 );
		if( nDataLength < 0 )
		{
			return 0L;
		}

		nCol = 0;
		m_ctrlOutBlock1.InsertItem( 0, "" );
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.RecCnt        , sizeof( pAllOutBlock->outBlock1.RecCnt         ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수        
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.AcntNo        , sizeof( pAllOutBlock->outBlock1.AcntNo         ), DATA_TYPE_STRING ) );    // [string,   20] 계좌번호          
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.Pwd           , sizeof( pAllOutBlock->outBlock1.Pwd            ), DATA_TYPE_STRING ) );    // [string,    8] 비밀번호          
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.BalCreTp      , sizeof( pAllOutBlock->outBlock1.BalCreTp       ), DATA_TYPE_STRING ) );    // [string,    1] 잔고생성구분      
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.CmsnAppTpCode , sizeof( pAllOutBlock->outBlock1.CmsnAppTpCode  ), DATA_TYPE_STRING ) );    // [string,    1] 수수료적용구분    
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.D2balBaseQryTp, sizeof( pAllOutBlock->outBlock1.D2balBaseQryTp ), DATA_TYPE_STRING ) );    // [string,    1] D2잔고기준조회구분          
		m_ctrlOutBlock1.SetItemText(  0, nCol++, GetDispData( pAllOutBlock->outBlock1.UprcTpCode    , sizeof( pAllOutBlock->outBlock1.UprcTpCode     ), DATA_TYPE_STRING ) );    // [string,    1] 단가구분          

		//------------------------------------------------
		// OutBlock2
		nDataLength -= sizeof( CSPAQ12300OutBlock2 );
		if( nDataLength < 0 )
		{
			return 0L;
		}

		nCol = 0;
		m_ctrlOutBlock2.InsertItem( 0, "" );
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.RecCnt                , sizeof( pAllOutBlock->outBlock2.RecCnt                 ), DATA_TYPE_LONG   ) );    // [long  ,    5] 레코드갯수                   
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.BrnNm                 , sizeof( pAllOutBlock->outBlock2.BrnNm                  ), DATA_TYPE_STRING ) );    // [string,   40] 지점명                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.AcntNm                , sizeof( pAllOutBlock->outBlock2.AcntNm                 ), DATA_TYPE_STRING ) );    // [string,   40] 계좌명                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyOrdAbleAmt         , sizeof( pAllOutBlock->outBlock2.MnyOrdAbleAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문가능금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyoutAbleAmt         , sizeof( pAllOutBlock->outBlock2.MnyoutAbleAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 출금가능금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.SeOrdAbleAmt          , sizeof( pAllOutBlock->outBlock2.SeOrdAbleAmt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 거래소금액                   
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.KdqOrdAbleAmt         , sizeof( pAllOutBlock->outBlock2.KdqOrdAbleAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 코스닥금액                   
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.HtsOrdAbleAmt         , sizeof( pAllOutBlock->outBlock2.HtsOrdAbleAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] HTS주문가능금액              
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt, sizeof( pAllOutBlock->outBlock2.MgnRat100pctOrdAbleAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 증거금률100퍼센트주문가능금액
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.BalEvalAmt            , sizeof( pAllOutBlock->outBlock2.BalEvalAmt             ), DATA_TYPE_LONG   ) );    // [long  ,   16] 잔고평가금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PchsAmt               , sizeof( pAllOutBlock->outBlock2.PchsAmt                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매입금액                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.RcvblAmt              , sizeof( pAllOutBlock->outBlock2.RcvblAmt               ), DATA_TYPE_LONG   ) );    // [long  ,   16] 미수금액                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PnlRat                , sizeof( pAllOutBlock->outBlock2.PnlRat                 ), DATA_TYPE_DOT, 6 ) );    // [double, 18.6] 손익율                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.InvstOrgAmt           , sizeof( pAllOutBlock->outBlock2.InvstOrgAmt            ), DATA_TYPE_LONG   ) );    // [long  ,   20] 투자원금                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.InvstPlAmt            , sizeof( pAllOutBlock->outBlock2.InvstPlAmt             ), DATA_TYPE_LONG   ) );    // [long  ,   16] 투자손익금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdtPldgOrdAmt        , sizeof( pAllOutBlock->outBlock2.CrdtPldgOrdAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 신용담보주문금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.Dps                   , sizeof( pAllOutBlock->outBlock2.Dps                    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 예수금                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1Dps                 , sizeof( pAllOutBlock->outBlock2.D1Dps                  ), DATA_TYPE_LONG   ) );    // [long  ,   16] D1예수금                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2Dps                 , sizeof( pAllOutBlock->outBlock2.D2Dps                  ), DATA_TYPE_LONG   ) );    // [long  ,   16] D2예수금                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.OrdDt                 , sizeof( pAllOutBlock->outBlock2.OrdDt                  ), DATA_TYPE_STRING ) );    // [string,    8] 주문일                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.MnyMgn                , sizeof( pAllOutBlock->outBlock2.MnyMgn                 ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금증거금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstMgn              , sizeof( pAllOutBlock->outBlock2.SubstMgn               ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용증거금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.SubstAmt              , sizeof( pAllOutBlock->outBlock2.SubstAmt               ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대용금액                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayBuyExecAmt       , sizeof( pAllOutBlock->outBlock2.PrdayBuyExecAmt        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매수체결금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdaySellExecAmt      , sizeof( pAllOutBlock->outBlock2.PrdaySellExecAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매도체결금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayBuyExecAmt       , sizeof( pAllOutBlock->outBlock2.CrdayBuyExecAmt        ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매수체결금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdaySellExecAmt      , sizeof( pAllOutBlock->outBlock2.CrdaySellExecAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매도체결금액             
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.EvalPnlSum            , sizeof( pAllOutBlock->outBlock2.EvalPnlSum             ), DATA_TYPE_LONG   ) );    // [long  ,   15] 평가손익합계                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.DpsastTotamt          , sizeof( pAllOutBlock->outBlock2.DpsastTotamt           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 예탁자산총액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.Evrprc                , sizeof( pAllOutBlock->outBlock2.Evrprc                 ), DATA_TYPE_LONG   ) );    // [long  ,   19] 제비용                       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.RuseAmt               , sizeof( pAllOutBlock->outBlock2.RuseAmt                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 재사용금액                   
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.EtclndAmt             , sizeof( pAllOutBlock->outBlock2.EtclndAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 기타대여금액                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrcAdjstAmt           , sizeof( pAllOutBlock->outBlock2.PrcAdjstAmt            ), DATA_TYPE_LONG   ) );    // [long  ,   16] 가정산금액                   
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1CmsnAmt             , sizeof( pAllOutBlock->outBlock2.D1CmsnAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] D1수수료                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2CmsnAmt             , sizeof( pAllOutBlock->outBlock2.D2CmsnAmt              ), DATA_TYPE_LONG   ) );    // [long  ,   16] D2수수료                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1EvrTax              , sizeof( pAllOutBlock->outBlock2.D1EvrTax               ), DATA_TYPE_LONG   ) );    // [long  ,   16] D1제세금                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2EvrTax              , sizeof( pAllOutBlock->outBlock2.D2EvrTax               ), DATA_TYPE_LONG   ) );    // [long  ,   16] D2제세금                     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D1SettPrergAmt        , sizeof( pAllOutBlock->outBlock2.D1SettPrergAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] D1결제예정금액               
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.D2SettPrergAmt        , sizeof( pAllOutBlock->outBlock2.D2SettPrergAmt         ), DATA_TYPE_LONG   ) );    // [long  ,   16] D2결제예정금액               
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKseMnyMgn        , sizeof( pAllOutBlock->outBlock2.PrdayKseMnyMgn         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일KSE현금증거금            
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKseSubstMgn      , sizeof( pAllOutBlock->outBlock2.PrdayKseSubstMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일KSE대용증거금            
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKseCrdtMnyMgn    , sizeof( pAllOutBlock->outBlock2.PrdayKseCrdtMnyMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일KSE신용현금증거금        
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKseCrdtSubstMgn  , sizeof( pAllOutBlock->outBlock2.PrdayKseCrdtSubstMgn   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일KSE신용대용증거금        
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKseMnyMgn        , sizeof( pAllOutBlock->outBlock2.CrdayKseMnyMgn         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일KSE현금증거금            
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKseSubstMgn      , sizeof( pAllOutBlock->outBlock2.CrdayKseSubstMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일KSE대용증거금            
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKseCrdtMnyMgn    , sizeof( pAllOutBlock->outBlock2.CrdayKseCrdtMnyMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일KSE신용현금증거금        
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKseCrdtSubstMgn  , sizeof( pAllOutBlock->outBlock2.CrdayKseCrdtSubstMgn   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일KSE신용대용증거금        
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKdqMnyMgn        , sizeof( pAllOutBlock->outBlock2.PrdayKdqMnyMgn         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일코스닥현금증거금         
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKdqSubstMgn      , sizeof( pAllOutBlock->outBlock2.PrdayKdqSubstMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일코스닥대용증거금         
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKdqCrdtMnyMgn    , sizeof( pAllOutBlock->outBlock2.PrdayKdqCrdtMnyMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일코스닥신용현금증거금     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayKdqCrdtSubstMgn  , sizeof( pAllOutBlock->outBlock2.PrdayKdqCrdtSubstMgn   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일코스닥신용대용증거금     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKdqMnyMgn        , sizeof( pAllOutBlock->outBlock2.CrdayKdqMnyMgn         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일코스닥현금증거금         
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKdqSubstMgn      , sizeof( pAllOutBlock->outBlock2.CrdayKdqSubstMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일코스닥대용증거금         
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKdqCrdtMnyMgn    , sizeof( pAllOutBlock->outBlock2.CrdayKdqCrdtMnyMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일코스닥신용현금증거금     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayKdqCrdtSubstMgn  , sizeof( pAllOutBlock->outBlock2.CrdayKdqCrdtSubstMgn   ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일코스닥신용대용증거금     
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayFrbrdMnyMgn      , sizeof( pAllOutBlock->outBlock2.PrdayFrbrdMnyMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일프리보드현금증거금       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayFrbrdSubstMgn    , sizeof( pAllOutBlock->outBlock2.PrdayFrbrdSubstMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일프리보드대용증거금       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayFrbrdMnyMgn      , sizeof( pAllOutBlock->outBlock2.CrdayFrbrdMnyMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일프리보드현금증거금       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayFrbrdSubstMgn    , sizeof( pAllOutBlock->outBlock2.CrdayFrbrdSubstMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일프리보드대용증거금       
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayCrbmkMnyMgn      , sizeof( pAllOutBlock->outBlock2.PrdayCrbmkMnyMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일장외현금증거금           
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.PrdayCrbmkSubstMgn    , sizeof( pAllOutBlock->outBlock2.PrdayCrbmkSubstMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일장외대용증거금           
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayCrbmkMnyMgn      , sizeof( pAllOutBlock->outBlock2.CrdayCrbmkMnyMgn       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일장외현금증거금           
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.CrdayCrbmkSubstMgn    , sizeof( pAllOutBlock->outBlock2.CrdayCrbmkSubstMgn     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일장외대용증거금           
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.DpspdgQty             , sizeof( pAllOutBlock->outBlock2.DpspdgQty              ), DATA_TYPE_LONG   ) );    // [long  ,   16] 예탁담보수량                 
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.BuyAdjstAmtD2         , sizeof( pAllOutBlock->outBlock2.BuyAdjstAmtD2          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수정산금(D+2)              
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.SellAdjstAmtD2        , sizeof( pAllOutBlock->outBlock2.SellAdjstAmtD2         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도정산금(D+2)              
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.RepayRqrdAmtD1        , sizeof( pAllOutBlock->outBlock2.RepayRqrdAmtD1         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 변제소요금(D+1)              
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.RepayRqrdAmtD2        , sizeof( pAllOutBlock->outBlock2.RepayRqrdAmtD2         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 변제소요금(D+2)              
		m_ctrlOutBlock2.SetItemText( 0, nCol++, GetDispData( pAllOutBlock->outBlock2.LoanAmt               , sizeof( pAllOutBlock->outBlock2.LoanAmt                ), DATA_TYPE_LONG   ) );    // [long  ,   16] 대출금액                     
		
		
		//------------------------------------------------
		// OutBlock3
		
		// Count 계산
		nDataLength -= 5;
		if( nDataLength < 0 )
		{
			return 0L;
		}

		CopyMemory( szCount, pAllOutBlock->sCountOutBlock3, 5 );
		nCount = atoi( szCount );

		// Data
		nDataLength -= sizeof( CSPAQ12300OutBlock3 ) * nCount;
		if( nDataLength < 0 )
		{
			return 0L;
		}

		for( int i=0; i<nCount; i++ )
		{
			nCol = 0;
			nRow = m_ctrlOutBlock3.InsertItem( m_ctrlOutBlock3.GetItemCount(), "" );
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].IsuNo           , sizeof( pAllOutBlock->outBlock3[i].IsuNo            ), DATA_TYPE_STRING ) );    // [string,   12] 종목번호            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].IsuNm           , sizeof( pAllOutBlock->outBlock3[i].IsuNm            ), DATA_TYPE_STRING ) );    // [string,   40] 종목명              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SecBalPtnCode   , sizeof( pAllOutBlock->outBlock3[i].SecBalPtnCode    ), DATA_TYPE_STRING ) );    // [string,    2] 유가증권잔고유형코드
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SecBalPtnNm     , sizeof( pAllOutBlock->outBlock3[i].SecBalPtnNm      ), DATA_TYPE_STRING ) );    // [string,   40] 유가증권잔고유형명  
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BalQty          , sizeof( pAllOutBlock->outBlock3[i].BalQty           ), DATA_TYPE_LONG   ) );    // [long  ,   16] 잔고수량            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BnsBaseBalQty   , sizeof( pAllOutBlock->outBlock3[i].BnsBaseBalQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매매기준잔고수량    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CrdayBuyExecQty , sizeof( pAllOutBlock->outBlock3[i].CrdayBuyExecQty  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매수체결수량    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CrdaySellExecQty, sizeof( pAllOutBlock->outBlock3[i].CrdaySellExecQty ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매도체결수량    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellPrc         , sizeof( pAllOutBlock->outBlock3[i].SellPrc          ), DATA_TYPE_DOT, 4 ) );    // [double, 21.4] 매도가              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BuyPrc          , sizeof( pAllOutBlock->outBlock3[i].BuyPrc           ), DATA_TYPE_DOT, 4 ) );    // [double, 21.4] 매수가              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellPnlAmt      , sizeof( pAllOutBlock->outBlock3[i].SellPnlAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도손익금액        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PnlRat          , sizeof( pAllOutBlock->outBlock3[i].PnlRat           ), DATA_TYPE_DOT, 6 ) );    // [double, 18.6] 손익율              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].NowPrc          , sizeof( pAllOutBlock->outBlock3[i].NowPrc           ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 현재가              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CrdtAmt         , sizeof( pAllOutBlock->outBlock3[i].CrdtAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 신용금액            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].DueDt           , sizeof( pAllOutBlock->outBlock3[i].DueDt            ), DATA_TYPE_STRING ) );    // [string,    8] 만기일              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdaySellExecPrc, sizeof( pAllOutBlock->outBlock3[i].PrdaySellExecPrc ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 전일매도체결가      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdaySellQty    , sizeof( pAllOutBlock->outBlock3[i].PrdaySellQty     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매도수량        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdayBuyExecPrc , sizeof( pAllOutBlock->outBlock3[i].PrdayBuyExecPrc  ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 전일매수체결가      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdayBuyQty     , sizeof( pAllOutBlock->outBlock3[i].PrdayBuyQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매수수량        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].LoanDt          , sizeof( pAllOutBlock->outBlock3[i].LoanDt           ), DATA_TYPE_STRING ) );    // [string,    8] 대출일              
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].AvrUprc         , sizeof( pAllOutBlock->outBlock3[i].AvrUprc          ), DATA_TYPE_DOT, 2 ) );    // [double, 13.2] 평균단가            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellAbleQty     , sizeof( pAllOutBlock->outBlock3[i].SellAbleQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도가능수량        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellOrdQty      , sizeof( pAllOutBlock->outBlock3[i].SellOrdQty       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도주문수량        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CrdayBuyExecAmt , sizeof( pAllOutBlock->outBlock3[i].CrdayBuyExecAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매수체결금액    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].CrdaySellExecAmt, sizeof( pAllOutBlock->outBlock3[i].CrdaySellExecAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 금일매도체결금액    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdayBuyExecAmt , sizeof( pAllOutBlock->outBlock3[i].PrdayBuyExecAmt  ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매수체결금액    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdaySellExecAmt, sizeof( pAllOutBlock->outBlock3[i].PrdaySellExecAmt ), DATA_TYPE_LONG   ) );    // [long  ,   16] 전일매도체결금액    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BalEvalAmt      , sizeof( pAllOutBlock->outBlock3[i].BalEvalAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 잔고평가금액        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].EvalPnl         , sizeof( pAllOutBlock->outBlock3[i].EvalPnl          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 평가손익            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].MnyOrdAbleAmt   , sizeof( pAllOutBlock->outBlock3[i].MnyOrdAbleAmt    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 현금주문가능금액    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].OrdAbleAmt      , sizeof( pAllOutBlock->outBlock3[i].OrdAbleAmt       ), DATA_TYPE_LONG   ) );    // [long  ,   16] 주문가능금액        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellUnercQty    , sizeof( pAllOutBlock->outBlock3[i].SellUnercQty     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도미체결수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].SellUnsttQty    , sizeof( pAllOutBlock->outBlock3[i].SellUnsttQty     ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매도미결제수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BuyUnercQty     , sizeof( pAllOutBlock->outBlock3[i].BuyUnercQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수미체결수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].BuyUnsttQty     , sizeof( pAllOutBlock->outBlock3[i].BuyUnsttQty      ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매수미결제수량      
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].UnsttQty        , sizeof( pAllOutBlock->outBlock3[i].UnsttQty         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 미결제수량          
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].UnercQty        , sizeof( pAllOutBlock->outBlock3[i].UnercQty         ), DATA_TYPE_LONG   ) );    // [long  ,   16] 미체결수량          
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PrdayCprc       , sizeof( pAllOutBlock->outBlock3[i].PrdayCprc        ), DATA_TYPE_DOT, 2 ) );    // [double, 15.2] 전일종가            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].PchsAmt         , sizeof( pAllOutBlock->outBlock3[i].PchsAmt          ), DATA_TYPE_LONG   ) );    // [long  ,   16] 매입금액            
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].RegMktCode      , sizeof( pAllOutBlock->outBlock3[i].RegMktCode       ), DATA_TYPE_STRING ) );    // [string,    2] 등록시장코드        
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].LoanDtlClssCode , sizeof( pAllOutBlock->outBlock3[i].LoanDtlClssCode  ), DATA_TYPE_STRING ) );    // [string,    2] 대출상세분류코드    
			m_ctrlOutBlock3.SetItemText( nRow, nCol++, GetDispData( pAllOutBlock->outBlock3[i].DpspdgLoanQty   , sizeof( pAllOutBlock->outBlock3[i].DpspdgLoanQty    ), DATA_TYPE_LONG   ) );    // [long  ,   16] 예탁담보대출수량    
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
LRESULT CDlg_CSPAQ02300::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	g_iXingAPI.ReleaseRequestData( (int)lParam );
	GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)"", (LPARAM)"Timeout" );		

	return 0L;
}
