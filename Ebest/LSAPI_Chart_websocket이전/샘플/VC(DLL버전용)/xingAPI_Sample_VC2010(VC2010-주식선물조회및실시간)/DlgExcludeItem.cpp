// DlgExcludeItem.cpp : implementation file
//

#include "stdafx.h"
#include "xingapi_sample.h"
#include "DlgExcludeItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExcludeItem dialog


CDlgExcludeItem::CDlgExcludeItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExcludeItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExcludeItem)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgExcludeItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExcludeItem)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExcludeItem, CDialog)
	//{{AFX_MSG_MAP(CDlgExcludeItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExcludeItem message handlers

BOOL CDlgExcludeItem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
		 if( m_nData & 0x00000080 ) CheckDlgButton( IDC_CHECK1, BST_CHECKED );		// 관리종목
	else if( m_nData & 0x00000100 ) CheckDlgButton( IDC_CHECK2, BST_CHECKED );		// 시장경보  
	else if( m_nData & 0x00000200 ) CheckDlgButton( IDC_CHECK3, BST_CHECKED );		// 거래정지  
	else if( m_nData & 0x00004000 ) CheckDlgButton( IDC_CHECK4, BST_CHECKED );		// 우선주    
	else if( m_nData & 0x00200000 ) CheckDlgButton( IDC_CHECK5, BST_CHECKED );		// 증거금50  
	else if( m_nData & 0x01000000 ) CheckDlgButton( IDC_CHECK6, BST_CHECKED );		// 정리매매  
	else if( m_nData & 0x04000000 ) CheckDlgButton( IDC_CHECK7, BST_CHECKED );		// 투자유의  
	else if( m_nData & 0x80000000 ) CheckDlgButton( IDC_CHECK8, BST_CHECKED );		// 불성실공시
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExcludeItem::OnOK() 
{
	m_nData = 0;
		 if( IsDlgButtonChecked( IDC_CHECK1 ) ) m_nData |= 0x00000080;		// 관리종목
	else if( IsDlgButtonChecked( IDC_CHECK2 ) ) m_nData |= 0x00000100;		// 시장경보  
	else if( IsDlgButtonChecked( IDC_CHECK3 ) ) m_nData |= 0x00000200;		// 거래정지  
	else if( IsDlgButtonChecked( IDC_CHECK4 ) ) m_nData |= 0x00004000;		// 우선주    
	else if( IsDlgButtonChecked( IDC_CHECK5 ) ) m_nData |= 0x00200000;		// 증거금50  
	else if( IsDlgButtonChecked( IDC_CHECK6 ) ) m_nData |= 0x01000000;		// 정리매매  
	else if( IsDlgButtonChecked( IDC_CHECK7 ) ) m_nData |= 0x04000000;		// 투자유의  
	else if( IsDlgButtonChecked( IDC_CHECK8 ) ) m_nData |= 0x80000000;		// 불성실공시
	
	CDialog::OnOK();
}
