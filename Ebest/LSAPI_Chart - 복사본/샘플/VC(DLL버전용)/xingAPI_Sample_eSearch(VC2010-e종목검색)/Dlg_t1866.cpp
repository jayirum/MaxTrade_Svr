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
// ����˻� ����Ʈ ��ȸ
void CDlg_t1866::OnButtonRequest() 
{
	RequestData();
}
// ����˻� ���� ��ȸ
void CDlg_t1866::OnButtonRequest1()
{
	RequestData(TRUE);
}

//--------------------------------------------------------------------------------------
// ��Ʈ�� �ʱ�ȭ
//--------------------------------------------------------------------------------------
void CDlg_t1866::InitCtrls()
{
	m_ctrlOutBlock.InsertColumn( 0, "�������Ǽ�",	LVCFMT_LEFT, 60 );
	m_ctrlOutBlock.InsertColumn( 1, "���ӿ���",		LVCFMT_LEFT, 60 );
	m_ctrlOutBlock.InsertColumn( 2, "����Ű",		LVCFMT_LEFT, 300 );

	m_ctrlOutBlock1.InsertColumn( 0, "���������ε���",	LVCFMT_LEFT, 200 );
	m_ctrlOutBlock1.InsertColumn( 1, "�׷��",			LVCFMT_LEFT, 200 );
	m_ctrlOutBlock1.InsertColumn( 2, "���������",		LVCFMT_LEFT, 300 );
}

//--------------------------------------------------------------------------------------
// ������ ��ȸ
//--------------------------------------------------------------------------------------
void CDlg_t1866::RequestData( BOOL bNext )
{
	//-----------------------------------------------------------
	// �����������Ǹ���Ʈ��ȸ(API/MTS)(t1866) ( ENCRYPT,block,headtype=A )
	t1866InBlock	pckInBlock;


	TCHAR			szTrNo[]	= "t1866";
	char			szNextKey[]	= "";

	//-----------------------------------------------------------
	// ������ ���
	CString str_ID;			GetDlgItemText( IDC_INBLOCK_ID, str_ID );			// �α���ID
	CString str_Cont;		GetDlgItemText( IDC_INBLOCK_CONT, str_Cont );		// ���ӿ���
	CString str_Index;		GetDlgItemText( IDC_INBLOCK_INDEX1866, str_Index );	// ����Ű

	//-----------------------------------------------------------
	// ������ �ʱ�ȭ
	FillMemory( &pckInBlock, sizeof( pckInBlock ), ' ' );

	//-----------------------------------------------------------
	// ������ �Է�
	SetPacketData( pckInBlock.user_id		, sizeof( pckInBlock.user_id		), str_ID      , DATA_TYPE_STRING );	// �α���ID
	SetPacketData( pckInBlock.gb			, sizeof( pckInBlock.gb				), "0"		   , DATA_TYPE_STRING );	// ��ȸ����
	SetPacketData( pckInBlock.group_name	, sizeof( pckInBlock.group_name		), ""	       , DATA_TYPE_STRING );	// �׷��
	SetPacketData( pckInBlock.cont			, sizeof( pckInBlock.cont			), str_Cont    , DATA_TYPE_STRING );	// ���ӿ���
	SetPacketData( pckInBlock.cont_key		, sizeof( pckInBlock.cont_key		), str_Index   , DATA_TYPE_STRING );	// ����Ű

	//-----------------------------------------------------------
	// ������ ����
	int nRqID = g_iXingAPI.Request( 
		GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_DATA ���� �´�.
		szTrNo,						// TR ��ȣ
		&pckInBlock,				// InBlock ������
		sizeof( pckInBlock ),		// InBlock ������ ũ��
		bNext,						// ������ȸ ���� : ������ȸ�� ��쿡 �����Ѵ�.
		szNextKey,					// ������ȸ Key : Header Type�� B �� ��쿣 ���� ��ȸ�� ���� Next Key�� �־��ش�.
		30							// Timeout(��) : �ش� �ð�(��)���� �����Ͱ� ���� ������ Timeout�� �߻��Ѵ�. XM_TIMEOUT_DATA �޽����� �߻��Ѵ�.
	);
	
	if( bNext == FALSE )
	{
		m_ctrlOutBlock.DeleteAllItems();
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
LRESULT CDlg_t1866::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//-------------------------------------------------------------------------------------
	// Data�� ����
	if( wParam == REQUEST_DATA )
	{

		LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;

		if( strcmp( pRpData->szBlockName, NAME_t1866OutBlock ) == 0 )
		{
			LPt1866OutBlock p1866OutBlock = (LPt1866OutBlock)pRpData->lpData;

			m_ctrlOutBlock.InsertItem( 0, "" );
			m_ctrlOutBlock.SetItemText( 0,  0, GetDispData( p1866OutBlock[0].result_count 	, sizeof( p1866OutBlock[0].result_count	), DATA_TYPE_STRING ) );	// ���������ε���          
			m_ctrlOutBlock.SetItemText( 0,  1, GetDispData( p1866OutBlock[0].cont			, sizeof( p1866OutBlock[0].cont	), DATA_TYPE_STRING ) );			// �׷��                 
			m_ctrlOutBlock.SetItemText( 0,  2, GetDispData( p1866OutBlock[0].cont_key		, sizeof( p1866OutBlock[0].cont_key	), DATA_TYPE_STRING ) );		// ���������             
		}
		else if( strcmp( pRpData->szBlockName, NAME_t1866OutBlock1 ) == 0 )
		{
			LPt1866OutBlock1 p1866OutBlock1 = (LPt1866OutBlock1)pRpData->lpData;

			int nCount = pRpData->nDataLength/sizeof(t1866OutBlock1);
			
			for( int i=0; i< nCount ; i++ )
			{
				m_ctrlOutBlock1.InsertItem( i, "" );
				m_ctrlOutBlock1.SetItemText( i,  0, GetDispData( p1866OutBlock1[i].query_index 	, sizeof( p1866OutBlock1[i].query_index	), DATA_TYPE_STRING ) );		// ���������ε���          
				m_ctrlOutBlock1.SetItemText( i,  1, GetDispData( p1866OutBlock1[i].group_name  	, sizeof( p1866OutBlock1[i].group_name	), DATA_TYPE_STRING ) );		// �׷��                 
				m_ctrlOutBlock1.SetItemText( i,  2, GetDispData( p1866OutBlock1[i].query_name  	, sizeof( p1866OutBlock1[i].query_name	), DATA_TYPE_STRING ) );		// ���������             
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
// Timeout �߻�
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