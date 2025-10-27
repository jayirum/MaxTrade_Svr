
// Sample_xingAceRdSenderDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Sample_xingAceRdSender.h"
#include "Sample_xingAceRdSenderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TID_SEND			1000
#define BTN_TEXT_SEND		"(7) ����"
#define BTN_TEXT_STOP		"(7) �ߴ�"

CSample_xingAceRdSenderDlg::CSample_xingAceRdSenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSample_xingAceRdSenderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// �ʱ�ȭ
	m_bAttr				= FALSE;
}

void CSample_xingAceRdSenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TR, m_ctrlTrList);
	DDX_Control(pDX, IDC_LIST_SEND, m_ctrlSendList);
	DDX_Control(pDX, IDC_LIST_OUTBLOCK, m_ctrlOutBlockList);
	DDX_Control(pDX, IDC_LIST_INBLOCK, m_ctrlInBlockList);
}

BEGIN_MESSAGE_MAP(CSample_xingAceRdSenderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TR, &CSample_xingAceRdSenderDlg::OnItemchangedListTr)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_INBLOCK, &CSample_xingAceRdSenderDlg::OnClickedButtonUpdateInblock)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_OUTBLOCK, &CSample_xingAceRdSenderDlg::OnClickedButtonUpdateOutblock)
	ON_BN_CLICKED(IDC_BUTTON_INSERT_SENDLIST, &CSample_xingAceRdSenderDlg::OnClickedButtonInsertSendlist)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSample_xingAceRdSenderDlg::OnClickedButtonSend)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_UP, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListUp)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_DOWN, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListDown)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_CLEAR, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_DEL, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListDel)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_ALL_DEL, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListAllDel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_LOAD, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListLoad)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LIST_SAVE, &CSample_xingAceRdSenderDlg::OnClickedButtonSendListSave)
END_MESSAGE_MAP()

BOOL CSample_xingAceRdSenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// Control �ʱ�ȭ
	InitControl();

	// TR List �ʱ�ȭ
	InitTrData();

	m_iSender.Init();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.
void CSample_xingAceRdSenderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CSample_xingAceRdSenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//======================================================================================
// ��Ʈ�� �ʱ�ȭ
//======================================================================================
void CSample_xingAceRdSenderDlg::InitControl()
{
	// TR List �ʱ�ȭ
	m_ctrlTrList.InsertColumn( 0, "�ڵ�", LVCFMT_LEFT,  40 );
	m_ctrlTrList.InsertColumn( 1, "����", LVCFMT_LEFT, 200 );
	m_ctrlTrList.InsertColumn( 2, "����", LVCFMT_LEFT, 160 );

	// InBlock �ʱ�ȭ
	m_ctrlInBlockList.InsertColumn( 0, "�̸�",   LVCFMT_LEFT,   85 );
	m_ctrlInBlockList.InsertColumn( 1, "Ÿ��",   LVCFMT_LEFT,   55 );
	m_ctrlInBlockList.InsertColumn( 2, "����",   LVCFMT_RIGHT,  40 );
	m_ctrlInBlockList.InsertColumn( 3, "����",   LVCFMT_LEFT,  120 );
	m_ctrlInBlockList.InsertColumn( 4, "������", LVCFMT_LEFT,  100 );

	// OutBlock �ʱ�ȭ
	m_ctrlOutBlockList.InsertColumn( 0, "�̸�",   LVCFMT_LEFT,   85 );
	m_ctrlOutBlockList.InsertColumn( 1, "Ÿ��",   LVCFMT_LEFT,   55 );
	m_ctrlOutBlockList.InsertColumn( 2, "����",   LVCFMT_RIGHT,  40 );
	m_ctrlOutBlockList.InsertColumn( 3, "����",   LVCFMT_LEFT,  120 );
	m_ctrlOutBlockList.InsertColumn( 4, "������", LVCFMT_LEFT,  100 );

	// ���۸���Ʈ �ʱ�ȭ
	m_ctrlSendList.InsertColumn( 0, "TR",		LVCFMT_LEFT,  40 );
	m_ctrlSendList.InsertColumn( 1, "InBlock",  LVCFMT_LEFT, 100 );
	m_ctrlSendList.InsertColumn( 2, "OutBlock", LVCFMT_LEFT, 310 );
	m_ctrlSendList.InsertColumn( 3, "����",		LVCFMT_LEFT,  50 );

	// ���ý� �ϳ��� �ʵ尡 ���õǴ°� �ƴ϶� ��ü Row �� ���õǵ��� ����
	m_ctrlTrList	  .SetExtendedStyle( m_ctrlTrList		.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlInBlockList .SetExtendedStyle( m_ctrlInBlockList	.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlOutBlockList.SetExtendedStyle( m_ctrlOutBlockList	.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlSendList	  .SetExtendedStyle( m_ctrlSendList		.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	// ����Ʈ ��Ʈ�ѿ��� ���õ� �׸��� ��Ŀ���� �Ҿ �׽� ���̵��� ����
	::SetWindowLong( m_ctrlTrList	   .GetSafeHwnd(), GWL_STYLE, m_ctrlTrList		.GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlInBlockList .GetSafeHwnd(), GWL_STYLE, m_ctrlInBlockList .GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlOutBlockList.GetSafeHwnd(), GWL_STYLE, m_ctrlOutBlockList.GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlSendList	   .GetSafeHwnd(), GWL_STYLE, m_ctrlSendList	.GetStyle() | LVS_SHOWSELALWAYS );

	// �����ֱ⸦ 100ms �� �ʱ�ȭ�Ѵ�.
	SetDlgItemInt( IDC_EDIT_TIME, 100 );
}

//======================================================================================
// TR List�� TR ����� ǥ���Ѵ�.
//--------------------------------------------------------------------------------------
// ./res �������� TR ���ϵ��� �ִ�.
// �� ������ ����� �о�ͼ� ǥ���Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::InitTrData()
{
	//----------------------------------------------------------
	// �������� res ������ ã�Ƽ� �����Ѵ�.

	// res ������ ���������� �ִ� ������ �������� �̹Ƿ� ���������� ��ġ�� ���ؼ� res ������ ��ġ�� ����Ѵ�.
	CString strFileName;
	::GetModuleFileName( AfxGetInstanceHandle(), strFileName.GetBuffer(MAX_PATH), MAX_PATH );	// ���������� ��ġ�� ���ؼ�
	strFileName.ReleaseBuffer();
	int nFind = strFileName.ReverseFind( '\\' );												// ���ʺ��� \ �� �ִ� ��ġ�� �����Ŀ�
	strFileName = strFileName.Left( nFind );													// \ �� �ִ� ���� �κ��� ���ؼ� ���������� ���� ��ġ�� �����Ŀ�
	strFileName += "/res/*.res";																// res ������ ���δ�.

	// res ������ �˻��Ѵ�.
	CFileFind ff;
	BOOL bFind = ff.FindFile( strFileName );
	while( bFind )
	{
		bFind = ff.FindNextFile();

		// Directory �ϰ��� ���� ������ �˻��Ѵ�.
		if( ff.IsDirectory() )
		{
			continue;
		}

		// ������ �о TR Code�� TR Desc�� �����´�.
		CStdioFile file;
		CString strRead;
		if( file.Open( ff.GetFilePath(), CFile::modeRead, NULL ) )
		{
			while( TRUE )
			{
				// ������ �о
				if( file.ReadString( strRead ) != FALSE )
				{
					// �յ��� ������� ���ְ�
					strRead.Trim();

					// �ǽð� TR ������ ���
					//		�ĸ��� �������� .Feed, TR����, TR�ڵ�� �Ǿ��ִ�.
					CString strType = GetToken( strRead );
					if( strType.CompareNoCase( ".Feed" ) != 0 )
					{
						continue;
					}
					
					int nItem = m_ctrlTrList.InsertItem( m_ctrlTrList.GetItemCount(), "" );	// �����Է�
					m_ctrlTrList.SetItemText( nItem, 1, GetToken( strRead ) );	// TR ����
					m_ctrlTrList.SetItemText( nItem, 0, GetToken( strRead ) );	// TR �ڵ�
					m_ctrlTrList.SetItemText( nItem, 2, ff.GetFilePath() );		// ���ϸ�
				}
				break;
			}			
		}
		file.Close();
	}
}

//======================================================================================
// TR List���� TR�� �������� ���
//======================================================================================
void CSample_xingAceRdSenderDlg::OnItemchangedListTr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// ���õ� TR�� ó���Ѵ�.
	ProcessTR( GetSelectedItem( m_ctrlTrList ) );
	
	*pResult = 0;
}

//======================================================================================
// TR res ������ �о InBlock, OutBlock �� �ʵ带 �� List�� �߰��Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::ProcessTR( int nSelect )
{
	// TR ���� ��Ʈ�� �� ���� �ʱ�ȭ
	m_ctrlInBlockList.DeleteAllItems();
	m_ctrlOutBlockList.DeleteAllItems();
	m_bAttr = FALSE;

	// ���õȰ� ������ �۾����� �ʴ´�.
	if( nSelect < 0 )
	{
		return;
	}

	// res ������ �м��Ѵ�.
	CStdioFile file;
	CString strFile = m_ctrlTrList.GetItemText( nSelect, 2 );
	if( file.Open( strFile, CFile::modeRead, NULL ) == FALSE )
	{
		MessageBox( "���� ���¿� �����Ͽ����ϴ�.", "����", MB_ICONSTOP );
		return;
	}

	CListCtrl* pListCtrl = NULL;
	CString strRead, strTemp;
	BOOL bData = FALSE;
	while( TRUE )
	{
		// ������ �а�
		if( file.ReadString( strRead ) == FALSE )
		{
			break;
		}

		// �յ��� ���ʿ��� Space ���� ����
		strRead.Trim();

		// �����̾����� ���� ���� �д´�.
		if( strRead.IsEmpty() )
		{
			continue;
		}

		// �� �ڿ� ; ������ �����Ѵ�. �߰��� �ִ� ���� ����.
		strRead.Remove( ';' );

		// BEGIN_DATA_MAP ���ĺ��� InBlock/OutBlock �ʵ尡 �����ϹǷ� �ϴ� BEGIN_DATA_MAP �� ã�´�.
		if( bData == FALSE )
		{
			strTemp = GetToken( strRead );
			
			// BEGIN_DATA_MAP �� ã�Ҵ�.
			if( strTemp.CompareNoCase( "BEGIN_DATA_MAP" ) == 0 )
			{
				bData = TRUE;
			}
			// .feed �� Attribute �Ӽ��� �����Ƿ� ã�´�.
			else if( strTemp.CompareNoCase( ".feed" ) == 0 )
			{
				while( strRead.IsEmpty() == FALSE )
				{
					strTemp = GetToken( strRead );
					// Attribute �Ӽ��� �ִ�.
					if( strTemp.CompareNoCase( "attr" ) == 0 )
					{
						m_bAttr = TRUE;
						break;
					}
				}
			}
			continue;
		}

		// BEGIN_DATA_MAP �� ã�����Ƿ� ���⼭���ʹ� �ʵ带 ó���Ѵ�.

		// ListCtrl �� NULL �� ���� ���� InBlock/OutBlock ���� �𸣹Ƿ� �װͺ��� ó���Ѵ�.
		if( pListCtrl == NULL )
		{
			// , �� �������� 3��° ���� InBlock/OutBlock �����̴�.
			GetToken( strRead );
			GetToken( strRead );
			strTemp = GetToken( strRead );
			
			// InBlock
			if( strTemp.CompareNoCase( "input" ) == 0 )
			{
				pListCtrl = &m_ctrlInBlockList;
			}
			// OutBlock
			else if( strTemp.CompareNoCase( "output" ) == 0 )
			{
				pListCtrl = &m_ctrlOutBlockList;
			}
			continue;
		}

		// InBlock/OutBlock �� ���������� �� ������ begin ~ end ���̿� �ʵ� ������ �ִ�.
		if( strRead.CompareNoCase( "begin" ) == 0 )
		{
			continue;
		}
		else if( strRead.CompareNoCase( "end" ) == 0 )
		{
			// InBlock/OutBlock ������ ������.
			pListCtrl = NULL;
			continue;
		}
		else
		{
			// �ʵ� ����
			CString strDesc = GetToken( strRead );	// ����
							  GetToken( strRead );	// ������
			CString strName = GetToken( strRead );	// �̸�
			CString strType = GetToken( strRead );	// Ÿ��
			CString strLen  = GetToken( strRead );	// ����

			int nItem = pListCtrl->InsertItem( pListCtrl->GetItemCount(), "" );
			pListCtrl->SetItemText( nItem, 0, strName );
			pListCtrl->SetItemText( nItem, 1, strType );
			pListCtrl->SetItemText( nItem, 2, strLen  );
			pListCtrl->SetItemText( nItem, 3, strDesc );
		}
	}
	file.Close();
}

//======================================================================================
// InBlock �Է� ��ư�� �������� InBlock Edit �� ���� ��Ʈ�ѿ� �־��ش�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonUpdateInblock()
{
	int nSelect = GetSelectedItem( m_ctrlInBlockList );

	// ���õ� �ʵ尡 ����.
	if( nSelect < 0 )
	{
		MessageBox( "InBlock List ���� �Է��� �ʵ带 �����Ͻñ� �ٶ��ϴ�", "Ȯ��", MB_ICONWARNING );
		return;
	}

	// Edit ���� ���� �о�ͼ� List Control �� �־��ش�.
	CString strData;
	GetDlgItemText( IDC_EDIT_UPDATE_INBLOCK, strData );
	m_ctrlInBlockList.SetItemText( nSelect, 4, strData );

	// ���� �ʵ�� ������ �ٲ��ش�. �׷��� ���� �Է��� �����ϴ�.
	if( nSelect + 1 < m_ctrlInBlockList.GetItemCount() )
	{
		SelectItem( m_ctrlInBlockList, nSelect + 1, nSelect );
	}
}

//======================================================================================
// OutBlock �Է� ��ư�� �������� OutBlock Edit �� ���� ��Ʈ�ѿ� �־��ش�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonUpdateOutblock()
{
	int nSelect = GetSelectedItem( m_ctrlOutBlockList );

	// ���õ� �ʵ尡 ����.
	if( nSelect < 0 )
	{
		MessageBox( "OutBlock List ���� �Է��� �ʵ带 �����Ͻñ� �ٶ��ϴ�", "Ȯ��", MB_ICONWARNING );
		return;
	}

	// Edit ���� ���� �о�ͼ� List Control �� �־��ش�.
	CString strData;
	GetDlgItemText( IDC_EDIT_UPDATE_OUTBLOCK, strData );
	m_ctrlOutBlockList.SetItemText( nSelect, 4, strData );

	// ���� �ʵ�� ������ �ٲ��ش�. �׷��� ���� �Է��� �����ϴ�.
	if( nSelect + 1 < m_ctrlOutBlockList.GetItemCount() )
	{
		SelectItem( m_ctrlOutBlockList, nSelect + 1, nSelect );
	}
}

//======================================================================================
// Dialog Box ��𼭵� Enter�� ġ�� �� �Լ��� ���´�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnOK()
{
	// ���� Focus�� ���� �����찡 InBlock Edit ���
	if( GetFocus() == GetDlgItem( IDC_EDIT_UPDATE_INBLOCK ) )
	{
		// �Է¹�ư�� ������ó�� �ൿ�Ѵ�.
		OnClickedButtonUpdateInblock();
		return;
	}
	// ���� Focus�� ���� �����찡 OutBlock Edit ���
	else if( GetFocus() == GetDlgItem( IDC_EDIT_UPDATE_OUTBLOCK ) )
	{
		// �Է¹�ư�� ������ó�� �ൿ�Ѵ�.
		OnClickedButtonUpdateOutblock();
		return;
	}

	// OnOK()�� �¿�� Dialog �� �����Ѵ�. ����, �� �Լ��� ���´�.
	// CDialogEx::OnOK();
}

//======================================================================================
// ���۸���Ʈ�� �߰� ��ư�� ������ TR List, InBlock List, OutBlock List���� �����͸�
// ������ Send List�� �߰��Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonInsertSendlist()
{
	// TR List���� ���õ� Row�� �����´�.
	int nSelect = GetSelectedItem( m_ctrlTrList );
	if( nSelect < 0 )
	{
		MessageBox( "���õ� TR�� �����ϴ�.", "Ȯ��", MB_ICONWARNING );
		return;
	}

	int nCount;
	CString strType;
	CString strLen;
	CString strData;
	CString strInBlock;
	CString strOutBlock;

	// TR�� �����ͼ� Send List �� �߰��Ѵ�.
	int nItem = m_ctrlSendList.InsertItem( m_ctrlSendList.GetItemCount(), m_ctrlTrList.GetItemText( nSelect, 0 ) );

	// InBlock ���� �����͸� �����ͼ� ��������� �����ͷ� ����� Send List �� �߰��Ѵ�.
	nCount = m_ctrlInBlockList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// �ʵ������� ������
		strType = m_ctrlInBlockList.GetItemText( i, 1 );
		strLen  = m_ctrlInBlockList.GetItemText( i, 2 );
		strData = m_ctrlInBlockList.GetItemText( i, 4 );

		// �Էµ� �����͸� �뼱������ �����ͷ� �����.
		strInBlock += MakeBlockData( strType, strLen, strData, FALSE );
	}
	m_ctrlSendList.SetItemText( nItem, 1, strInBlock );

	// OutBlock ���� �����͸� �����ͼ� ��������� �����ͷ� ����� Send List �� �߰��Ѵ�.
	nCount = m_ctrlOutBlockList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// �ʵ������� ������
		strType = m_ctrlOutBlockList.GetItemText( i, 1 );
		strLen  = m_ctrlOutBlockList.GetItemText( i, 2 );
		strData = m_ctrlOutBlockList.GetItemText( i, 4 );

		// �Էµ� �����͸� �뼱������ �����ͷ� �����.
		strOutBlock += MakeBlockData( strType, strLen, strData, m_bAttr, strInBlock.IsEmpty() ? FALSE : TRUE );
	}
	m_ctrlSendList.SetItemText( nItem, 2, strOutBlock );
}

//======================================================================================
// ���� ��ư Ŭ���� �����͸� xingACE�� �����Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSend()
{
	CString strCaption;
	GetDlgItemText( IDC_BUTTON_SEND, strCaption );

	// ��ư�� Caption �� ���� Timer�� ������ų�� ������ �����Ѵ�.
	if( strCaption == BTN_TEXT_SEND )
	{
		// ������ ������ ������ ������ �ϴ� Ŭ�����Ѵ�.
		OnClickedButtonSendListClear();

		// �����ֱ⸦ ���Ѵ�.
		int nTime = GetDlgItemInt( IDC_EDIT_TIME );
		if( nTime <= 0 )
		{
			// �����ֱⰡ ����� �ƴϸ� �⺻������ 100ms �� �����Ѵ�.
			nTime = 100;
		}
		
		// ���۸���Ʈ�� Index ���� �ʱ�ȭ�ϰ� Timer�� ������Ų��.
		m_nSendIndex = 0;
		SetTimer( TID_SEND, nTime, NULL );

		// ��ư�� Caption �� �ߴ����� �����Ѵ�.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_STOP );
	}
	else
	{
		// Timer �� �����Ѵ�.
		KillTimer( TID_SEND );

		// ��ư�� Caption �� �������� �����Ѵ�.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );
	}
}

//======================================================================================
// ȭ���� ����ɶ� Uninit �� �Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClose()
{
	m_iSender.Uninit();

	CDialogEx::OnClose();
}

//======================================================================================
// ���� �ø��� ��ư
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListUp()
{
	int nItem = GetSelectedItem( m_ctrlSendList );
	if( 0 < nItem )			// ���õ� Row �� 0 ���� Ŀ������ ���� �ű� �� �ִ�.
	{
		CString strTemp;
		
		// ���õ� Row �� �� �ٷ� ���� Row�� Column ���� �ٲ�ġ�� �Ѵ�.
		strTemp = m_ctrlSendList.GetItemText( nItem, 0 );
		m_ctrlSendList.SetItemText( nItem, 0, m_ctrlSendList.GetItemText( nItem-1, 0 ) );
		m_ctrlSendList.SetItemText( nItem-1, 0, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 1 );
		m_ctrlSendList.SetItemText( nItem, 1, m_ctrlSendList.GetItemText( nItem-1, 1 ) );
		m_ctrlSendList.SetItemText( nItem-1, 1, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 2 );
		m_ctrlSendList.SetItemText( nItem, 2, m_ctrlSendList.GetItemText( nItem-1, 2 ) );
		m_ctrlSendList.SetItemText( nItem-1, 2, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 3 );
		m_ctrlSendList.SetItemText( nItem, 3, m_ctrlSendList.GetItemText( nItem-1, 3 ) );
		m_ctrlSendList.SetItemText( nItem-1, 3, strTemp );

		// �ٲ�ġ�� �����Ƿ� ���õ� Row �� �ٷ� ���� Row �� �ٲ۴�.
		SelectItem( m_ctrlSendList, nItem - 1, nItem );
	}
}

//======================================================================================
// �Ʒ��� ������ ��ư
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListDown()
{
	int nItem = GetSelectedItem( m_ctrlSendList );
	if( 0 <= nItem && nItem < m_ctrlSendList.GetItemCount() - 1 )			// ���õ� Row �� �� ������ Row�� �ƴϸ� �ű� �� �ִ�.
	{
		CString strTemp;
		
		// ���õ� Row �� �� �ٷ� �Ʒ��� Row�� Column ���� �ٲ�ġ�� �Ѵ�.
		strTemp = m_ctrlSendList.GetItemText( nItem, 0 );
		m_ctrlSendList.SetItemText( nItem, 0, m_ctrlSendList.GetItemText( nItem+1, 0 ) );
		m_ctrlSendList.SetItemText( nItem+1, 0, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 1 );
		m_ctrlSendList.SetItemText( nItem, 1, m_ctrlSendList.GetItemText( nItem+1, 1 ) );
		m_ctrlSendList.SetItemText( nItem+1, 1, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 2 );
		m_ctrlSendList.SetItemText( nItem, 2, m_ctrlSendList.GetItemText( nItem+1, 2 ) );
		m_ctrlSendList.SetItemText( nItem+1, 2, strTemp );

		strTemp = m_ctrlSendList.GetItemText( nItem, 3 );
		m_ctrlSendList.SetItemText( nItem, 3, m_ctrlSendList.GetItemText( nItem+1, 3 ) );
		m_ctrlSendList.SetItemText( nItem+1, 3, strTemp );

		// �ٲ�ġ�� �����Ƿ� ���õ� Row �� �ٷ� �Ʒ��� Row �� �ٲ۴�.
		SelectItem( m_ctrlSendList, nItem + 1, nItem );
	}
}

//======================================================================================
// ���۵� ������ ������ �ٽ� ������ �� �ְ�
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListClear()
{
	int nCount = m_ctrlSendList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// ���� ���� ����.
		m_ctrlSendList.SetItemText( i, 3, "" );
	}
}

//======================================================================================
// ���õ� Row�� �����.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListDel()
{
	int nItem = GetSelectedItem( m_ctrlSendList );
	if( nItem >= 0 )
	{
		m_ctrlSendList.DeleteItem( nItem );
	}
}

//======================================================================================
// ��ü ����
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListAllDel()
{
	m_ctrlSendList.DeleteAllItems();
}

//======================================================================================
// �о����
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListLoad()
{
	CFileDialog dlg( 
		TRUE, 
		"rdf", 
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"�ǽð� ������ ����(*.rdf)|*.rdf|��� ����(*.*)|*.*||", 
		this 
	);
	if( dlg.DoModal() != IDOK )
	{
		return;
	}

	// ����Ʈ�� ������ ����
	m_ctrlSendList.DeleteAllItems();

	// ���� ����
	CStdioFile file;
	if( file.Open( dlg.GetPathName(), CFile::modeRead, NULL ) == FALSE )
	{
		MessageBox( "������ �дµ� �����Ͽ����ϴ�.", "����", MB_ICONSTOP );
		return;
	}

	// ���Ͽ��� �Ѷ��ξ� �о ����Ʈ�� ����
	CString strRead;
	int		nItem;
	while( file.ReadString( strRead ) )
	{
		nItem = m_ctrlSendList.InsertItem( m_ctrlSendList.GetItemCount(), "" );
		m_ctrlSendList.SetItemText( nItem, 0, GetToken( strRead ) );
		m_ctrlSendList.SetItemText( nItem, 1, GetToken( strRead ) );
		m_ctrlSendList.SetItemText( nItem, 2, GetToken( strRead ) );
	}

	// ���� �ݱ�
	file.Close();
}

//======================================================================================
// �����ϱ�
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListSave()
{
	// �ٸ� �̸����� ���� Dialog
	CFileDialog dlg( 
		FALSE, 
		"rdf", 
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"�ǽð� ������ ����(*.rdf)|*.rdf|��� ����(*.*)|*.*||", 
		this 
	);
	if( dlg.DoModal() != IDOK )
	{
		return;
	}

	// ���� ����
	CStdioFile file;
	if( file.Open( dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate, NULL ) == FALSE )
	{
		MessageBox( "������ �����ϴµ� �����Ͽ����ϴ�.", "����", MB_ICONSTOP );
		return;
	}

	// ����Ʈ�� ������ ���Ͽ� ����
	CString strWrite;
	int nCount = m_ctrlSendList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		strWrite.Format( "%s,%s,%s\n", 
			m_ctrlSendList.GetItemText( i, 0 ),
			m_ctrlSendList.GetItemText( i, 1 ),
			m_ctrlSendList.GetItemText( i, 2 )
		);
		file.WriteString( strWrite );
	}

	// ���� �ݱ�
	file.Close();
}

//======================================================================================
// ����ó�� Timer
//======================================================================================
void CSample_xingAceRdSenderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TID_SEND )
	{
		SendRealData();
	}

	CDialogEx::OnTimer(nIDEvent);
}

//======================================================================================
// �����͸� �����Ѵ�.
//======================================================================================
void CSample_xingAceRdSenderDlg::SendRealData()
{
	// ���۸���Ʈ�� �������� Index�� �� ũ�� Ÿ�̸Ӹ� �����ϰ� ������ ���·� �����
	int nCount = m_ctrlSendList.GetItemCount();
	if( m_nSendIndex >= nCount )
	{
		// Timer �� �����Ѵ�.
		KillTimer( TID_SEND );

		// ��ư�� Caption �� �������� �����Ѵ�.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );

		return;
	}

	// ������ �����͸� ���ϰ�
	CString strTr		= m_ctrlSendList.GetItemText( m_nSendIndex, 0 );
	CString strInBlock  = m_ctrlSendList.GetItemText( m_nSendIndex, 1 );
	CString strOutBlock	= m_ctrlSendList.GetItemText( m_nSendIndex, 2 );

	// ���¸� ���������� �ٲٰ�
	m_ctrlSendList.SetItemText( m_nSendIndex, 3, "������" );
	
	// �����͸� �����ϰ�
	BOOL bResult = m_iSender.Send( 
		strTr,						// �ǽð� TR Code
		strInBlock,					// InBlock Data
		strInBlock.GetLength(),		// InBlock �� ũ��
		strOutBlock,				// OutBlock �� Data
		strOutBlock.GetLength()		// OutBlock �� ũ��
	);
	
	// ����� ���¿� �Է��Ѵ�.
	m_ctrlSendList.SetItemText( m_nSendIndex, 3, bResult ? "����" : "����" );

	// ������ �Ϸ�Ǿ����Ƿ� Index�� �ϳ� �ø���.
	m_nSendIndex++;

	// ������ ���� ���� ���ٸ� ���⼭ �ߴ��Ѵ�.
	if( m_nSendIndex >= nCount )
	{
		// Timer �� �����Ѵ�.
		KillTimer( TID_SEND );

		// ��ư�� Caption �� �������� �����Ѵ�.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );

		return;
	}
}
