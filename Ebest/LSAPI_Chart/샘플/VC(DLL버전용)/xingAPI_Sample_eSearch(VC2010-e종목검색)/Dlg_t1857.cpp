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
// ����˻� ��ȸ
void CDlg_t1857::OnButtonRequest() 
{
	RequestData();
}

// ����˻� �ǽð����
void CDlg_t1857::OnButtonRequest4()
{
	RequestData2();
}

//--------------------------------------------------------------------------------------
// ��Ʈ�� �ʱ�ȭ
//--------------------------------------------------------------------------------------
void CDlg_t1857::InitCtrls()
{
	m_ctrlOutBlock1.InsertColumn( 0, "�����ڵ�",		LVCFMT_LEFT, 70 );
	m_ctrlOutBlock1.InsertColumn( 1, "�����",			LVCFMT_LEFT, 100 );
	m_ctrlOutBlock1.InsertColumn( 2, "���簡",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 3, "���ϴ�񱸺�",	LVCFMT_LEFT, 30 );
	m_ctrlOutBlock1.InsertColumn( 4, "���ϴ��",		LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 5, "���",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 6, "�ŷ���",			LVCFMT_LEFT, 50 );
	m_ctrlOutBlock1.InsertColumn( 7, "�������",		LVCFMT_LEFT, 50 );

	m_ctrlOutBlock2.InsertColumn( 0, "Key",	LVCFMT_LEFT, 100 );
	m_combo_Flag.SetCurSel(0);
	m_combo_Real.SetCurSel(0);
}

//--------------------------------------------------------------------------------------
// ������ ��ȸ
//--------------------------------------------------------------------------------------
void CDlg_t1857::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// ����˻�(�Ź���API��)(t1857) ( attr,block,headtype=A )
	t1857InBlock	pckInBlock;


	TCHAR			szTrNo[]	= "t1857";
	char			szNextKey[]	= "";

	//-----------------------------------------------------------
	// ������ ���
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_INDEX1857, str_Index );		// ����˻��Է°�
	CString str_Flag;		m_combo_Flag.GetLBText(m_combo_Flag.GetCurSel(),str_Flag);// ��ȸ���а�
	str_Flag = str_Flag.Left(1);
	CString str_Real;		m_combo_Real.GetLBText(m_combo_Real.GetCurSel(),str_Real);// �ǽð� ����
	str_Real = str_Real.Left(1);

	//-----------------------------------------------------------
	// ������ �ʱ�ȭ
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// ������ �Է�
	SetPacketData( pckInBlock.sRealFlag		, sizeof( pckInBlock.sRealFlag		), str_Real	   , DATA_TYPE_STRING );	// �ǽð� ���� 1:��� 0:��ȸ��
	SetPacketData( pckInBlock.sSearchFlag	, sizeof( pckInBlock.sSearchFlag	), str_Flag	   , DATA_TYPE_STRING );	// ��ȸ���а� S:���� F:����
	SetPacketData( pckInBlock.query_index	, sizeof( pckInBlock.query_index	), str_Index   , DATA_TYPE_STRING );	// ����˻��Է°�

	//-----------------------------------------------------------
	// ������ ����
	int nRqID = g_iXingAPI.RequestService( 
		GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_DATA ���� �´�.
		szTrNo,						// TR ��ȣ
		(LPCTSTR)&pckInBlock
	);

	if( bNext == FALSE )
	{
		m_ctrlOutBlock1.DeleteAllItems();
	}

	//-----------------------------------------------------------
	// Request ID�� 0���� ���� ��쿡�� �����̴�.
	if( nRqID < 0 )
	{
		MessageBox( "��ȸ����", "����", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// �����͸� ����
//--------------------------------------------------------------------------------------
LRESULT CDlg_t1857::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data�� ����
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
					m_ctrlOutBlock1.SetItemText( i,  0, GetDispData( p1857OutBlock1[i].shcode 	, sizeof( p1857OutBlock1[i].shcode	), DATA_TYPE_STRING ) );		// �����ڵ�          
					m_ctrlOutBlock1.SetItemText( i,  1, GetDispData( p1857OutBlock1[i].hname  	, sizeof( p1857OutBlock1[i].hname	), DATA_TYPE_STRING ) );		// �����            
					m_ctrlOutBlock1.SetItemText( i,  2, GetDispData( p1857OutBlock1[i].price   	, sizeof( p1857OutBlock1[i].price	), DATA_TYPE_LONG ) );			// ���簡            
					m_ctrlOutBlock1.SetItemText( i,  3, GetDispData( p1857OutBlock1[i].sign		, sizeof( p1857OutBlock1[i].sign	), DATA_TYPE_STRING ) );		// ���ϴ�񱸺�      
					m_ctrlOutBlock1.SetItemText( i,  4, GetDispData( p1857OutBlock1[i].change  	, sizeof( p1857OutBlock1[i].change	), DATA_TYPE_LONG ) );			// ���ϴ��          
					m_ctrlOutBlock1.SetItemText( i,  5, GetDispData( p1857OutBlock1[i].diff 	, sizeof( p1857OutBlock1[i].diff	), DATA_TYPE_FLOAT_DOT,2 ) );	// �����            
					m_ctrlOutBlock1.SetItemText( i,  6, GetDispData( p1857OutBlock1[i].volume	, sizeof( p1857OutBlock1[i].volume	), DATA_TYPE_LONG ) );			// �ŷ���            
					CString strJobFlag;
					strJobFlag = GetDispData( p1857OutBlock1[i].JobFlag	, sizeof( p1857OutBlock1[i].JobFlag	), DATA_TYPE_STRING );
					if(strcmp( strJobFlag, "N" ) == 0)		strJobFlag = "����";
					else if(strcmp( strJobFlag, "R" ) == 0)	strJobFlag = "������";
					else if(strcmp( strJobFlag, "O" ) == 0)	strJobFlag = "��Ż";
					m_ctrlOutBlock1.SetItemText( i,  7, strJobFlag );			// �������
				}
			}
		}
	}
	
	//-------------------------------------------------------------------------------------
	// �޽����� ����
	else if( wParam == MESSAGE_DATA )
	{
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;

		CString strMsg( (char*)pMsg->lpszMessageData, pMsg->nMsgLength );
		GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg );		
		
		g_iXingAPI.ReleaseMessageData( lParam );
	}
	
	//-------------------------------------------------------------------------------------
	// System Error�� ����
	else if( wParam == SYSTEM_ERROR_DATA )
	{
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		
		CString strMsg( (char*)pMsg->lpszMessageData, pMsg->nMsgLength );
		GetParent()->SendMessage( WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg );		

		g_iXingAPI.ReleaseMessageData( lParam );
	}

	//-------------------------------------------------------------------------------------
	// Release Data�� ����
	else if( wParam == RELEASE_DATA )
	{
		g_iXingAPI.ReleaseRequestData( (int)lParam );
	}

	return 0L;
}

//--------------------------------------------------------------------------------------
// ������ ��ȸ
//--------------------------------------------------------------------------------------
void CDlg_t1857::RequestData2( BOOL bNext )
{
	TCHAR			szTrNo[]	= "t1857";

	//-----------------------------------------------------------
	// ������ ���
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_REALKEY, str_Index );		// ���������ε���

	//-----------------------------------------------------------
	// ������ ����
	int nRqID1 = g_iXingAPI.RemoveService(
		GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_DATA ���� �´�.
		szTrNo,						// TR ��ȣ
		(LPCTSTR)str_Index
	);

	//-----------------------------------------------------------
	// Request ID�� 0���� ���� ��쿡�� �����̴�.
	if( nRqID1 < 0 )
	{
		MessageBox( "��ȸ����", "����", MB_ICONSTOP );
	}
}

//--------------------------------------------------------------------------------------
// Timeout �߻�
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
// �����͸� ����
//--------------------------------------------------------------------------------------
LRESULT CDlg_t1857::OnXMReceiveRealSearchData( WPARAM wParam, LPARAM lParam )
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	LPt1857OutBlock1 pOutBlock = (LPt1857OutBlock1)pRealPacket->pszData;
	int nRowCount = m_ctrlOutBlock1.GetItemCount();
	m_ctrlOutBlock1.InsertItem( 0, "" );
	m_ctrlOutBlock1.SetItemText( 0,  0, GetDispData( pOutBlock->shcode	, sizeof( pOutBlock->shcode	), DATA_TYPE_STRING ) );		// �����ڵ�          
	m_ctrlOutBlock1.SetItemText( 0,  1, GetDispData( pOutBlock->hname  	, sizeof( pOutBlock->hname 	), DATA_TYPE_STRING ) );		// �����            
	m_ctrlOutBlock1.SetItemText( 0,  2, GetDispData( pOutBlock->price   , sizeof( pOutBlock->price 	), DATA_TYPE_LONG ) );			// ���簡            
	m_ctrlOutBlock1.SetItemText( 0,  3, GetDispData( pOutBlock->sign	, sizeof( pOutBlock->sign	), DATA_TYPE_STRING ) );		// ���ϴ�񱸺�      
	m_ctrlOutBlock1.SetItemText( 0,  4, GetDispData( pOutBlock->change 	, sizeof( pOutBlock->change	), DATA_TYPE_LONG ) );			// ���ϴ��          
	m_ctrlOutBlock1.SetItemText( 0,  5, GetDispData( pOutBlock->diff	, sizeof( pOutBlock->diff	), DATA_TYPE_FLOAT_DOT,2 ) );	// �����            
	m_ctrlOutBlock1.SetItemText( 0,  6, GetDispData( pOutBlock->volume	, sizeof( pOutBlock->volume	), DATA_TYPE_LONG ) );			// �ŷ���            
	CString strJobFlag;
	strJobFlag = GetDispData( pOutBlock->JobFlag	, sizeof( pOutBlock->JobFlag	), DATA_TYPE_STRING );
	if(strcmp( strJobFlag, "N" ) == 0)		strJobFlag = "����";
	else if(strcmp( strJobFlag, "R" ) == 0)	strJobFlag = "������";
	else if(strcmp( strJobFlag, "O" ) == 0)	strJobFlag = "��Ż";
	m_ctrlOutBlock1.SetItemText( 0,  7, strJobFlag );			// �������

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
