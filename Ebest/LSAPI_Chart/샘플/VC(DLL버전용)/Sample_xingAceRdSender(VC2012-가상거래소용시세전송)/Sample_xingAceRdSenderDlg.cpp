
// Sample_xingAceRdSenderDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Sample_xingAceRdSender.h"
#include "Sample_xingAceRdSenderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TID_SEND			1000
#define BTN_TEXT_SEND		"(7) 전송"
#define BTN_TEXT_STOP		"(7) 중단"

CSample_xingAceRdSenderDlg::CSample_xingAceRdSenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSample_xingAceRdSenderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// 초기화
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// Control 초기화
	InitControl();

	// TR List 초기화
	InitTrData();

	m_iSender.Init();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
void CSample_xingAceRdSenderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSample_xingAceRdSenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//======================================================================================
// 컨트롤 초기화
//======================================================================================
void CSample_xingAceRdSenderDlg::InitControl()
{
	// TR List 초기화
	m_ctrlTrList.InsertColumn( 0, "코드", LVCFMT_LEFT,  40 );
	m_ctrlTrList.InsertColumn( 1, "설명", LVCFMT_LEFT, 200 );
	m_ctrlTrList.InsertColumn( 2, "파일", LVCFMT_LEFT, 160 );

	// InBlock 초기화
	m_ctrlInBlockList.InsertColumn( 0, "이름",   LVCFMT_LEFT,   85 );
	m_ctrlInBlockList.InsertColumn( 1, "타입",   LVCFMT_LEFT,   55 );
	m_ctrlInBlockList.InsertColumn( 2, "길이",   LVCFMT_RIGHT,  40 );
	m_ctrlInBlockList.InsertColumn( 3, "설명",   LVCFMT_LEFT,  120 );
	m_ctrlInBlockList.InsertColumn( 4, "데이터", LVCFMT_LEFT,  100 );

	// OutBlock 초기화
	m_ctrlOutBlockList.InsertColumn( 0, "이름",   LVCFMT_LEFT,   85 );
	m_ctrlOutBlockList.InsertColumn( 1, "타입",   LVCFMT_LEFT,   55 );
	m_ctrlOutBlockList.InsertColumn( 2, "길이",   LVCFMT_RIGHT,  40 );
	m_ctrlOutBlockList.InsertColumn( 3, "설명",   LVCFMT_LEFT,  120 );
	m_ctrlOutBlockList.InsertColumn( 4, "데이터", LVCFMT_LEFT,  100 );

	// 전송리스트 초기화
	m_ctrlSendList.InsertColumn( 0, "TR",		LVCFMT_LEFT,  40 );
	m_ctrlSendList.InsertColumn( 1, "InBlock",  LVCFMT_LEFT, 100 );
	m_ctrlSendList.InsertColumn( 2, "OutBlock", LVCFMT_LEFT, 310 );
	m_ctrlSendList.InsertColumn( 3, "전송",		LVCFMT_LEFT,  50 );

	// 선택시 하나의 필드가 선택되는게 아니라 전체 Row 가 선택되도록 설정
	m_ctrlTrList	  .SetExtendedStyle( m_ctrlTrList		.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlInBlockList .SetExtendedStyle( m_ctrlInBlockList	.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlOutBlockList.SetExtendedStyle( m_ctrlOutBlockList	.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_ctrlSendList	  .SetExtendedStyle( m_ctrlSendList		.GetExtendedStyle() | LVS_EX_FULLROWSELECT );

	// 리스트 컨트롤에서 선택된 항목이 포커스를 잃어도 항시 보이도록 설정
	::SetWindowLong( m_ctrlTrList	   .GetSafeHwnd(), GWL_STYLE, m_ctrlTrList		.GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlInBlockList .GetSafeHwnd(), GWL_STYLE, m_ctrlInBlockList .GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlOutBlockList.GetSafeHwnd(), GWL_STYLE, m_ctrlOutBlockList.GetStyle() | LVS_SHOWSELALWAYS );
	::SetWindowLong( m_ctrlSendList	   .GetSafeHwnd(), GWL_STYLE, m_ctrlSendList	.GetStyle() | LVS_SHOWSELALWAYS );

	// 전송주기를 100ms 로 초기화한다.
	SetDlgItemInt( IDC_EDIT_TIME, 100 );
}

//======================================================================================
// TR List에 TR 목록을 표시한다.
//--------------------------------------------------------------------------------------
// ./res 폴더에는 TR 파일들이 있다.
// 이 파일의 목록을 읽어와서 표시한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::InitTrData()
{
	//----------------------------------------------------------
	// 폴더에서 res 파일을 찾아서 나열한다.

	// res 폴더는 실행파일이 있는 폴더의 서브폴더 이므로 실행파일의 위치를 구해서 res 폴더의 위치를 계산한다.
	CString strFileName;
	::GetModuleFileName( AfxGetInstanceHandle(), strFileName.GetBuffer(MAX_PATH), MAX_PATH );	// 실행파일의 위치를 구해서
	strFileName.ReleaseBuffer();
	int nFind = strFileName.ReverseFind( '\\' );												// 뒤쪽부터 \ 가 있는 위치를 구한후에
	strFileName = strFileName.Left( nFind );													// \ 가 있는 앞쪽 부분을 구해서 실행파일의 폴더 위치를 만든후에
	strFileName += "/res/*.res";																// res 폴더를 붙인다.

	// res 파일을 검색한다.
	CFileFind ff;
	BOOL bFind = ff.FindFile( strFileName );
	while( bFind )
	{
		bFind = ff.FindNextFile();

		// Directory 일경우는 다음 파일을 검색한다.
		if( ff.IsDirectory() )
		{
			continue;
		}

		// 파일을 읽어서 TR Code와 TR Desc를 가져온다.
		CStdioFile file;
		CString strRead;
		if( file.Open( ff.GetFilePath(), CFile::modeRead, NULL ) )
		{
			while( TRUE )
			{
				// 한줄을 읽어서
				if( file.ReadString( strRead ) != FALSE )
				{
					// 앞뒤의 빈공간을 없애고
					strRead.Trim();

					// 실시간 TR 파일일 경우
					//		컴마를 기준으로 .Feed, TR설명, TR코드로 되어있다.
					CString strType = GetToken( strRead );
					if( strType.CompareNoCase( ".Feed" ) != 0 )
					{
						continue;
					}
					
					int nItem = m_ctrlTrList.InsertItem( m_ctrlTrList.GetItemCount(), "" );	// 한줄입력
					m_ctrlTrList.SetItemText( nItem, 1, GetToken( strRead ) );	// TR 설명
					m_ctrlTrList.SetItemText( nItem, 0, GetToken( strRead ) );	// TR 코드
					m_ctrlTrList.SetItemText( nItem, 2, ff.GetFilePath() );		// 파일명
				}
				break;
			}			
		}
		file.Close();
	}
}

//======================================================================================
// TR List에서 TR을 선택했을 경우
//======================================================================================
void CSample_xingAceRdSenderDlg::OnItemchangedListTr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 선택된 TR을 처리한다.
	ProcessTR( GetSelectedItem( m_ctrlTrList ) );
	
	*pResult = 0;
}

//======================================================================================
// TR res 파일을 읽어서 InBlock, OutBlock 의 필드를 각 List에 추가한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::ProcessTR( int nSelect )
{
	// TR 관련 컨트롤 및 변수 초기화
	m_ctrlInBlockList.DeleteAllItems();
	m_ctrlOutBlockList.DeleteAllItems();
	m_bAttr = FALSE;

	// 선택된게 없으면 작업하지 않는다.
	if( nSelect < 0 )
	{
		return;
	}

	// res 파일을 분석한다.
	CStdioFile file;
	CString strFile = m_ctrlTrList.GetItemText( nSelect, 2 );
	if( file.Open( strFile, CFile::modeRead, NULL ) == FALSE )
	{
		MessageBox( "파일 오픈에 실패하였습니다.", "에러", MB_ICONSTOP );
		return;
	}

	CListCtrl* pListCtrl = NULL;
	CString strRead, strTemp;
	BOOL bData = FALSE;
	while( TRUE )
	{
		// 한줄을 읽고
		if( file.ReadString( strRead ) == FALSE )
		{
			break;
		}

		// 앞뒤의 불필요한 Space 등을 제거
		strRead.Trim();

		// 빈줄이었으면 다음 줄을 읽는다.
		if( strRead.IsEmpty() )
		{
			continue;
		}

		// 맨 뒤에 ; 있으면 제거한다. 중간에 있는 경우는 없다.
		strRead.Remove( ';' );

		// BEGIN_DATA_MAP 이후부터 InBlock/OutBlock 필드가 존재하므로 일단 BEGIN_DATA_MAP 을 찾는다.
		if( bData == FALSE )
		{
			strTemp = GetToken( strRead );
			
			// BEGIN_DATA_MAP 을 찾았다.
			if( strTemp.CompareNoCase( "BEGIN_DATA_MAP" ) == 0 )
			{
				bData = TRUE;
			}
			// .feed 에 Attribute 속성이 있으므로 찾는다.
			else if( strTemp.CompareNoCase( ".feed" ) == 0 )
			{
				while( strRead.IsEmpty() == FALSE )
				{
					strTemp = GetToken( strRead );
					// Attribute 속성이 있다.
					if( strTemp.CompareNoCase( "attr" ) == 0 )
					{
						m_bAttr = TRUE;
						break;
					}
				}
			}
			continue;
		}

		// BEGIN_DATA_MAP 을 찾았으므로 여기서부터는 필드를 처리한다.

		// ListCtrl 이 NULL 인 경우는 아직 InBlock/OutBlock 인지 모르므로 그것부터 처리한다.
		if( pListCtrl == NULL )
		{
			// , 를 기준으로 3번째 값이 InBlock/OutBlock 여부이다.
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

		// InBlock/OutBlock 이 정해졌으면 그 다음에 begin ~ end 사이에 필드 정보가 있다.
		if( strRead.CompareNoCase( "begin" ) == 0 )
		{
			continue;
		}
		else if( strRead.CompareNoCase( "end" ) == 0 )
		{
			// InBlock/OutBlock 정보가 끝났다.
			pListCtrl = NULL;
			continue;
		}
		else
		{
			// 필드 정보
			CString strDesc = GetToken( strRead );	// 설명
							  GetToken( strRead );	// 사용안함
			CString strName = GetToken( strRead );	// 이름
			CString strType = GetToken( strRead );	// 타입
			CString strLen  = GetToken( strRead );	// 길이

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
// InBlock 입력 버튼을 눌렀을때 InBlock Edit 의 값을 컨트롤에 넣어준다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonUpdateInblock()
{
	int nSelect = GetSelectedItem( m_ctrlInBlockList );

	// 선택된 필드가 없다.
	if( nSelect < 0 )
	{
		MessageBox( "InBlock List 에서 입력할 필드를 선택하시기 바랍니다", "확인", MB_ICONWARNING );
		return;
	}

	// Edit 에서 값을 읽어와서 List Control 에 넣어준다.
	CString strData;
	GetDlgItemText( IDC_EDIT_UPDATE_INBLOCK, strData );
	m_ctrlInBlockList.SetItemText( nSelect, 4, strData );

	// 다음 필드로 선택을 바꿔준다. 그래야 연속 입력이 가능하다.
	if( nSelect + 1 < m_ctrlInBlockList.GetItemCount() )
	{
		SelectItem( m_ctrlInBlockList, nSelect + 1, nSelect );
	}
}

//======================================================================================
// OutBlock 입력 버튼을 눌렀을때 OutBlock Edit 의 값을 컨트롤에 넣어준다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonUpdateOutblock()
{
	int nSelect = GetSelectedItem( m_ctrlOutBlockList );

	// 선택된 필드가 없다.
	if( nSelect < 0 )
	{
		MessageBox( "OutBlock List 에서 입력할 필드를 선택하시기 바랍니다", "확인", MB_ICONWARNING );
		return;
	}

	// Edit 에서 값을 읽어와서 List Control 에 넣어준다.
	CString strData;
	GetDlgItemText( IDC_EDIT_UPDATE_OUTBLOCK, strData );
	m_ctrlOutBlockList.SetItemText( nSelect, 4, strData );

	// 다음 필드로 선택을 바꿔준다. 그래야 연속 입력이 가능하다.
	if( nSelect + 1 < m_ctrlOutBlockList.GetItemCount() )
	{
		SelectItem( m_ctrlOutBlockList, nSelect + 1, nSelect );
	}
}

//======================================================================================
// Dialog Box 어디서든 Enter를 치면 이 함수로 들어온다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnOK()
{
	// 현재 Focus를 가진 윈도우가 InBlock Edit 라면
	if( GetFocus() == GetDlgItem( IDC_EDIT_UPDATE_INBLOCK ) )
	{
		// 입력버튼을 누른것처럼 행동한다.
		OnClickedButtonUpdateInblock();
		return;
	}
	// 현재 Focus를 가진 윈도우가 OutBlock Edit 라면
	else if( GetFocus() == GetDlgItem( IDC_EDIT_UPDATE_OUTBLOCK ) )
	{
		// 입력버튼을 누른것처럼 행동한다.
		OnClickedButtonUpdateOutblock();
		return;
	}

	// OnOK()를 태우면 Dialog 가 종료한다. 따라서, 이 함수를 막는다.
	// CDialogEx::OnOK();
}

//======================================================================================
// 전송리스트에 추가 버튼을 누르면 TR List, InBlock List, OutBlock List에서 데이터를
// 가져와 Send List에 추가한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonInsertSendlist()
{
	// TR List에서 선택된 Row를 가져온다.
	int nSelect = GetSelectedItem( m_ctrlTrList );
	if( nSelect < 0 )
	{
		MessageBox( "선택된 TR이 없습니다.", "확인", MB_ICONWARNING );
		return;
	}

	int nCount;
	CString strType;
	CString strLen;
	CString strData;
	CString strInBlock;
	CString strOutBlock;

	// TR을 가져와서 Send List 에 추가한다.
	int nItem = m_ctrlSendList.InsertItem( m_ctrlSendList.GetItemCount(), m_ctrlTrList.GetItemText( nSelect, 0 ) );

	// InBlock 에서 데이터를 가져와서 통신형태의 데이터로 만들어 Send List 에 추가한다.
	nCount = m_ctrlInBlockList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// 필드정보를 가져와
		strType = m_ctrlInBlockList.GetItemText( i, 1 );
		strLen  = m_ctrlInBlockList.GetItemText( i, 2 );
		strData = m_ctrlInBlockList.GetItemText( i, 4 );

		// 입력된 데이터를 통선형태의 데이터로 만든다.
		strInBlock += MakeBlockData( strType, strLen, strData, FALSE );
	}
	m_ctrlSendList.SetItemText( nItem, 1, strInBlock );

	// OutBlock 에서 데이터를 가져와서 통신형태의 데이터로 만들어 Send List 에 추가한다.
	nCount = m_ctrlOutBlockList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// 필드정보를 가져와
		strType = m_ctrlOutBlockList.GetItemText( i, 1 );
		strLen  = m_ctrlOutBlockList.GetItemText( i, 2 );
		strData = m_ctrlOutBlockList.GetItemText( i, 4 );

		// 입력된 데이터를 통선형태의 데이터로 만든다.
		strOutBlock += MakeBlockData( strType, strLen, strData, m_bAttr, strInBlock.IsEmpty() ? FALSE : TRUE );
	}
	m_ctrlSendList.SetItemText( nItem, 2, strOutBlock );
}

//======================================================================================
// 전송 버튼 클릭시 데이터를 xingACE로 전송한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSend()
{
	CString strCaption;
	GetDlgItemText( IDC_BUTTON_SEND, strCaption );

	// 버튼의 Caption 을 보고 Timer를 가동시킬지 죽일지 결정한다.
	if( strCaption == BTN_TEXT_SEND )
	{
		// 이전에 전송한 내역이 있으면 일단 클리어한다.
		OnClickedButtonSendListClear();

		// 전송주기를 구한다.
		int nTime = GetDlgItemInt( IDC_EDIT_TIME );
		if( nTime <= 0 )
		{
			// 전송주기가 양수가 아니면 기본값으로 100ms 를 설정한다.
			nTime = 100;
		}
		
		// 전송리스트의 Index 값을 초기화하고 Timer를 가동시킨다.
		m_nSendIndex = 0;
		SetTimer( TID_SEND, nTime, NULL );

		// 버튼의 Caption 을 중단으로 설정한다.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_STOP );
	}
	else
	{
		// Timer 를 종료한다.
		KillTimer( TID_SEND );

		// 버튼의 Caption 을 전송으로 설정한다.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );
	}
}

//======================================================================================
// 화면이 종료될때 Uninit 를 한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClose()
{
	m_iSender.Uninit();

	CDialogEx::OnClose();
}

//======================================================================================
// 위로 올리기 버튼
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListUp()
{
	int nItem = GetSelectedItem( m_ctrlSendList );
	if( 0 < nItem )			// 선택된 Row 가 0 보다 커야지만 위로 옮길 수 있다.
	{
		CString strTemp;
		
		// 선택된 Row 와 그 바로 위의 Row의 Column 값을 바꿔치기 한다.
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

		// 바꿔치기 했으므로 선택된 Row 를 바로 위의 Row 로 바꾼다.
		SelectItem( m_ctrlSendList, nItem - 1, nItem );
	}
}

//======================================================================================
// 아래로 내리기 버튼
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListDown()
{
	int nItem = GetSelectedItem( m_ctrlSendList );
	if( 0 <= nItem && nItem < m_ctrlSendList.GetItemCount() - 1 )			// 선택된 Row 가 맨 마지막 Row만 아니면 옮길 수 있다.
	{
		CString strTemp;
		
		// 선택된 Row 와 그 바로 아래의 Row의 Column 값을 바꿔치기 한다.
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

		// 바꿔치기 했으므로 선택된 Row 를 바로 아래의 Row 로 바꾼다.
		SelectItem( m_ctrlSendList, nItem + 1, nItem );
	}
}

//======================================================================================
// 전송된 내역을 지워서 다시 전송할 수 있게
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListClear()
{
	int nCount = m_ctrlSendList.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		// 상태 값을 비운다.
		m_ctrlSendList.SetItemText( i, 3, "" );
	}
}

//======================================================================================
// 선택된 Row를 지운다.
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
// 전체 삭제
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListAllDel()
{
	m_ctrlSendList.DeleteAllItems();
}

//======================================================================================
// 읽어오기
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListLoad()
{
	CFileDialog dlg( 
		TRUE, 
		"rdf", 
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"실시간 데이터 파일(*.rdf)|*.rdf|모든 파일(*.*)|*.*||", 
		this 
	);
	if( dlg.DoModal() != IDOK )
	{
		return;
	}

	// 리스트의 내용을 삭제
	m_ctrlSendList.DeleteAllItems();

	// 파일 오픈
	CStdioFile file;
	if( file.Open( dlg.GetPathName(), CFile::modeRead, NULL ) == FALSE )
	{
		MessageBox( "파일을 읽는데 실패하였습니다.", "에러", MB_ICONSTOP );
		return;
	}

	// 파일에서 한라인씩 읽어서 리스트에 저장
	CString strRead;
	int		nItem;
	while( file.ReadString( strRead ) )
	{
		nItem = m_ctrlSendList.InsertItem( m_ctrlSendList.GetItemCount(), "" );
		m_ctrlSendList.SetItemText( nItem, 0, GetToken( strRead ) );
		m_ctrlSendList.SetItemText( nItem, 1, GetToken( strRead ) );
		m_ctrlSendList.SetItemText( nItem, 2, GetToken( strRead ) );
	}

	// 파일 닫기
	file.Close();
}

//======================================================================================
// 저장하기
//======================================================================================
void CSample_xingAceRdSenderDlg::OnClickedButtonSendListSave()
{
	// 다른 이름으로 저장 Dialog
	CFileDialog dlg( 
		FALSE, 
		"rdf", 
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"실시간 데이터 파일(*.rdf)|*.rdf|모든 파일(*.*)|*.*||", 
		this 
	);
	if( dlg.DoModal() != IDOK )
	{
		return;
	}

	// 파일 오픈
	CStdioFile file;
	if( file.Open( dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate, NULL ) == FALSE )
	{
		MessageBox( "파일을 저장하는데 실패하였습니다.", "에러", MB_ICONSTOP );
		return;
	}

	// 리스트의 내용을 파일에 저장
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

	// 파일 닫기
	file.Close();
}

//======================================================================================
// 전송처리 Timer
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
// 데이터를 전송한다.
//======================================================================================
void CSample_xingAceRdSenderDlg::SendRealData()
{
	// 전송리스트의 갯수보다 Index가 더 크면 타이머를 중지하고 전송전 상태로 만든다
	int nCount = m_ctrlSendList.GetItemCount();
	if( m_nSendIndex >= nCount )
	{
		// Timer 를 종료한다.
		KillTimer( TID_SEND );

		// 버튼의 Caption 을 전송으로 설정한다.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );

		return;
	}

	// 전송할 데이터를 구하고
	CString strTr		= m_ctrlSendList.GetItemText( m_nSendIndex, 0 );
	CString strInBlock  = m_ctrlSendList.GetItemText( m_nSendIndex, 1 );
	CString strOutBlock	= m_ctrlSendList.GetItemText( m_nSendIndex, 2 );

	// 상태를 전송중으로 바꾸고
	m_ctrlSendList.SetItemText( m_nSendIndex, 3, "전송중" );
	
	// 데이터를 전송하고
	BOOL bResult = m_iSender.Send( 
		strTr,						// 실시간 TR Code
		strInBlock,					// InBlock Data
		strInBlock.GetLength(),		// InBlock 의 크기
		strOutBlock,				// OutBlock 의 Data
		strOutBlock.GetLength()		// OutBlock 의 크기
	);
	
	// 결과를 상태에 입력한다.
	m_ctrlSendList.SetItemText( m_nSendIndex, 3, bResult ? "전송" : "실패" );

	// 전송이 완료되었으므로 Index를 하나 늘린다.
	m_nSendIndex++;

	// 다음에 보낼 것이 없다면 여기서 중단한다.
	if( m_nSendIndex >= nCount )
	{
		// Timer 를 종료한다.
		KillTimer( TID_SEND );

		// 버튼의 Caption 을 전송으로 설정한다.
		SetDlgItemText( IDC_BUTTON_SEND, BTN_TEXT_SEND );

		return;
	}
}
