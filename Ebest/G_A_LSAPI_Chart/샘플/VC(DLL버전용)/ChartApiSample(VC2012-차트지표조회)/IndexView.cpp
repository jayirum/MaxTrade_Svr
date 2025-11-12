// View01.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "IndexView.h"

#define COMSTR(st) CString(st, sizeof(st))


// CIndexView
IMPLEMENT_DYNCREATE(CIndexView, CFormView)

CIndexView::CIndexView()
	: CFormView(CIndexView::IDD)
{
    m_strIndexName	= "";
	m_nColCount		= 0;
	m_bExcelData	= FALSE;
	m_nCurrentPos	= 0;
}

CIndexView::~CIndexView()
{
}

void CIndexView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MARKET,					m_cmbMarket);
    DDX_Control(pDX, IDC_CODE,						m_edtCode);
    DDX_Control(pDX, IDC_PERIOD,					m_cmbPeriod);
    DDX_Control(pDX, IDC_COUNT,						m_edtCount);
    DDX_Control(pDX, IDC_DATETIMEPICKER1,			m_datEnd);
	DDX_Control(pDX, IDC_EDIT_EXCEL_FILE,			m_edtXlsFile);
    DDX_Control(pDX, IDC_LIST,						m_ctlList);
	DDX_Control(pDX, IDC_ST_MSG,					m_stMsg);
}

BEGIN_MESSAGE_MAP(CIndexView, CFormView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST,				&CIndexView::OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_CHECK_EXCEL,						&CIndexView::OnBnClickedCheckExcel)
    ON_BN_CLICKED(IDC_QUERY,							&CIndexView::OnBtnQuery)
    ON_MESSAGE(WM_USER + XM_RECEIVE_DATA,				OnXMReceiveData)
    ON_MESSAGE(WM_USER + XM_TIMEOUT_DATA,				OnXMTimeoutData)
    ON_MESSAGE(WM_USER + XM_RECEIVE_REAL_DATA_CHART,	OnXMReceiveRealData)  
END_MESSAGE_MAP()


// CIndexView 진단입니다.

#ifdef _DEBUG
void CIndexView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndexView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndexView 메시지 처리기입니다.

void CIndexView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

	// 시장구분 콤보 초기화
    InitMarketTypeCombo(&m_cmbMarket);

	// 주기구분 콤보 초기화
    InitPeriodCombo(&m_cmbPeriod);

    // 지표표시 리스트 초기화
	InitIndexList();

	// 조회갯수 초기화
	m_edtCount.SetWindowText("100");

	// 엑셀 파일 경로 초기화
    TCHAR szPath[ MAX_PATH ] = { 0, };
    GetModuleDir( szPath, sizeof( szPath ) );
	m_edtXlsFile.SetWindowText(CString(szPath)+"샘플\\엑셀VBA\\ChartExcelData.xls");

	UpdateData(FALSE);
}


int CIndexView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFormView::OnCreate(lpCreateStruct) == -1)
        return -1;

	return 0;
}


void CIndexView::OnDestroy()
{
    CFormView::OnDestroy();
}


void CIndexView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

	// 지표표시 리스트 사이즈 조절
    if (m_ctlList.GetSafeHwnd())
    {
        CRect rect;
        m_stMsg.GetWindowRect(&rect);
        ScreenToClient(rect);
        rect.bottom = cy - 1;
        rect.right	= cx - 1;
		rect.top = rect.bottom - 20;
		m_stMsg.SetWindowPos( NULL, 0, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);

		m_ctlList.GetWindowRect(&rect);
        ScreenToClient(rect);
        rect.bottom = cy - 1 - 20 - 5;
		rect.right	= cx - 5;
        m_ctlList.SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height(),  SWP_NOZORDER|SWP_NOMOVE);
    }
}


void CIndexView::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;
}


//----------------------------------------------------------------------------------------------------
// 엑셀데이터를 가져와서 지표데이터를 조회할지 판단합니다
//----------------------------------------------------------------------------------------------------
void CIndexView::OnBnClickedCheckExcel()
{
	UpdateData();

	//------------------------------------------------------------
	// 엑셀에 저장한 시고저종 데이터를 이용해 지표데이터를 가공할지 여부
	// false : ChartIndex TR 조회
	// true  : ChartExcel TR 조회  엑셀데이터를 이용해 지표데이터 조회
	m_bExcelData = ((CButton*)GetDlgItem(IDC_CHECK_EXCEL))->GetCheck();

	if (m_bExcelData) 
	{
		m_edtCount.EnableWindow(FALSE);
		m_datEnd.EnableWindow(FALSE);
		m_edtXlsFile.EnableWindow(TRUE);
	} 
	else 
	{
		m_edtCount.EnableWindow(TRUE);
		m_datEnd.EnableWindow(TRUE);
		m_edtXlsFile.EnableWindow(FALSE);
	}
}


//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회를 요청합니다
//----------------------------------------------------------------------------------------------------
void CIndexView::OnBtnQuery()
{
	UpdateData();

	// 지표ID를 확인하여 이미 수신중인 지표라면 삭제를 먼저 한 후 지표를 재조회 한다
	CString strTRCode;
	CString strIndexID;
	for (int i = 0; i < m_arrIndexID.GetCount(); i++) 
	{
		strTRCode	= m_arrTRCode.GetAt(i);
		strIndexID	= m_arrIndexID.GetAt(i);

		if (strTRCode.CompareNoCase(NAME_CHARTINDEX) == 0)
		{
			g_iXingAPI.RemoveService(GetSafeHwnd(), NAME_CHARTINDEX, (LPCTSTR)strIndexID);
		}
		else if (strTRCode.CompareNoCase(NAME_CHARTEXCEL) == 0)
		{
			g_iXingAPI.RemoveService(GetSafeHwnd(), NAME_CHARTEXCEL, (LPCTSTR)strIndexID);
		}
	}

	// 표시중인 리스트를 삭제한다
    m_ctlList.DeleteAllItems();

	// 종목코드를 확인한다
	int nRQID = 0;
	CString strCode;
	m_edtCode.GetWindowText(strCode);
	if (strCode.GetLength() < 2) 
	{
		AfxMessageBox("적용할 종목코드가 없습니다.");
		m_edtCode.SetFocus();
		return;
	}

	// 시장구분, 주기구분
	int nType	= GetMarketType(&m_cmbMarket);
	int nPeriod = GetPeriodType(&m_cmbPeriod);
	
	// 실시간 자동등록
	int nReal = ((CButton*)GetDlgItem(IDC_CHECK_REAL))->GetCheck();

	//------------------------------------------------------------
	// 엑셀데이터를 가져와 지표데이터를 조회한다
	// 백데이터로 쌓아놓은 과거데이터가 있다면,
	// "xingAPI 설치폴더\엑셀샘플\ChartExcelData.xls"의 포맷대로 파일을 만든 후
	// 엑셀데이터 가져오기로 지표데이터를 조회할 수 있다
	if (m_bExcelData) 
	{
		CString strExlFile;
		m_edtXlsFile.GetWindowText(strExlFile);
		if (strExlFile.IsEmpty()) 
		{
			AfxMessageBox("엑셀 화일을 선택하여 주세요.");
			return;
		}

		ChartExcelInBlock inblock;
		memset(&inblock, 0x20, sizeof(ChartExcelInBlock));
		strcpy(inblock.indexname, m_strIndexName);				// [string,   40] 지표명
		inblock.indexouttype[0] = '1';							// [string,    1] 결과 데이터 구분(1:지표)
		inblock.market[0]		= nType+'0';					// [string,    1] 시장구분(주식:1, 업종:2, 선물옵션:5)                      
		inblock.period[0]		= nPeriod+'0' ;					// [sting,     1] 주기구분(틱:0, 분:1, 일:2, 주:3, 월:4)                       
		strcpy(inblock.shcode, strCode);						// [string,    8] 단축코드													
		inblock.isexcelout[0]   = '1';							// [string,    1] 가공한 지표데이터를 엑셀에 동시 출력 여부 (1:출력, 0:출력안함)
		strcpy(inblock.excelfilename, strExlFile);				// [string,  256] 엑셀데이터 파일명
		inblock.IsReal[0]		= nReal+'0';					// [string,    1] 실시간 데이터 자동 등록 여부 (0:조회만, 1:실시간 자동 등록)
		nRQID = g_iXingAPI.RequestService(GetSafeHwnd(), NAME_CHARTEXCEL, (LPCTSTR)&inblock);
	} 
	//------------------------------------------------------------
	// 차트 기초데이터를 이용해 지표데이터를 조회한다
	else 
	{
		CString strCount;
		m_edtCount.GetWindowText(strCount);
		int nCount = atoi(strCount);

		CString strDate = GetDate(&m_datEnd);

		ChartIndexInBlock inblock;
		memset(&inblock, 0x20, sizeof(ChartIndexInBlock));
		strcpy(inblock.indexname, m_strIndexName);			// [string,   40] 지표명
		inblock.market[0]		= nType+'0';				// [string,    1] 시장구분(주식:1, 업종:2, 선물옵션:5)
		inblock.period[0]		= nPeriod+'0' ;				// [sting,     1] 주기구분(틱:0,분:1,일:2,주:3,월:4)                       
		strcpy(inblock.shcode, strCode);					// [string,    8] 단축코드													
		sprintf(inblock.qrycnt, "%d", nCount);				// [long  ,    4] 요청건수(최대 500개)
		sprintf(inblock.ncnt, "%d", 1);						// [long  ,    4] 단위(n틱/n분)         
		strcpy(inblock.edate, strDate);					    // [string,    8] 종료일자(일/주/월 해당)  											
		inblock.Isamend[0]		= '1';						// [string,    1] 수정주가 반영여부(0:반영안함, 1:반영)
		inblock.Isgab[0]		= '1';						// [string,    1] 갭보정 여부(0:보정안함, 1:보정)
		inblock.IsReal[0]		= nReal+'0';				// [string,    1] 실시간 데이터 자동 등록 여부 (0:조회만, 1:실시간 자동 등록)
		nRQID = g_iXingAPI.RequestService(GetSafeHwnd(), NAME_CHARTINDEX, (LPCTSTR)&inblock);
	}

    if (nRQID < 0)
    {
        TRACE("지표 조회 요청에 실패 하였습니다.\n");
    }
}


//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회 결과를 수신받습니다
//----------------------------------------------------------------------------------------------------
LRESULT CIndexView::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//------------------------------------------------------------
	// Data를 받음
    if (wParam == REQUEST_DATA)
    {
		ReceiveChartIndex((LPRECV_PACKET)lParam);
    }

	//------------------------------------------------------------
	// 메시지를 받음
    else if (wParam == MESSAGE_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		//AfxMessageBox(strMsg);

		m_stMsg.SetWindowText(strMsg);

        g_iXingAPI.ReleaseMessageData(lParam);
	}
	
	//------------------------------------------------------------
	// System Error를 받음
    else if (wParam == SYSTEM_ERROR_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
        CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		AfxMessageBox(strMsg);

        g_iXingAPI.ReleaseMessageData(lParam);
	}

	//------------------------------------------------------------
	// Release Data를 받음
    else if (wParam == RELEASE_DATA)
    {
		g_iXingAPI.ReleaseRequestData( (int)lParam);
	}

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// 조회 응답이 오지 않으면, 타임아웃 메시지를 수신받습니다
//----------------------------------------------------------------------------------------------------
LRESULT CIndexView::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
    g_iXingAPI.ReleaseRequestData( ( int )lParam );

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 실시간 결과를 수신받습니다
//----------------------------------------------------------------------------------------------------
// ChartIndex TR 조회 시
// Inblock의 IsReal(실시간 데이터 자동 등록 여부) 필드값을 '1'로 설정하면 
// 실시간 데이터 수신시 API내부에서 실시간 지표를 가공하여 메시지를 전송합니다.
// 이 때, 수신받는 실시간 데이터는 ChartIndex TR의 Outblock1과 동일한 구조입니다.
LRESULT CIndexView::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	// 조회시 OutBlock의 indexid(indexid가 같으면 같은 지표이다 - 지표를 구분하는 key로 판단가능)
	UINT nIndexID = (UINT)wParam;	

	// 지표 실시간 데이터는 
	// RECV_REAL_PACKET의 pszData내에 OutBlock1 의 포맷으로 수신받는다
    LPRECV_REAL_PACKET pRcvData = (LPRECV_REAL_PACKET)lParam;
	ChartIndexOutBlock1 *pBlock = (ChartIndexOutBlock1 *)(pRcvData->pszData);

	int nPos = atoi(COMSTR(pBlock->pos));
	int nCol = 0; 
	
	// 이전의 조회(또는 실시간) 수신 데이터의 마지막 표시 위치와 비교하여
	// 신규로 표시할지, 기존의 위치에 업데이트 할지를 판단할 수 있다
	if (nPos == m_nCurrentPos) 
	{
	} 
	else if (nPos > m_nCurrentPos) 
	{
		m_ctlList.InsertItem(0,"");
		m_nCurrentPos = nPos;
	} 
	else 
	{
		return FALSE;
	}

	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->pos)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->date)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->time)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->open)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->high)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->low)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->close)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->volume)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value1)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value2)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value3)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value4)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value5)); 

	return 1L;
}


//----------------------------------------------------------------------------------------------------
// 지표를 설정합니다
//----------------------------------------------------------------------------------------------------
void CIndexView::SetIndexName(int iInexType, LPCTSTR pszName )
{
	m_strIndexName = pszName;
}


//----------------------------------------------------------------------------------------------------
// 지표표시 리스트 초기화합니다
//----------------------------------------------------------------------------------------------------
void CIndexView::InitIndexList()
{
    COLUMNDATA ci[] = 
    {
        { 80, LVCFMT_CENTER, "일시" },
        { 80, LVCFMT_CENTER, "시가" },
        { 80, LVCFMT_CENTER, "고가" },
        { 80, LVCFMT_CENTER, "저가" },
        { 80, LVCFMT_CENTER, "종가" },
        { NULL, NULL, NULL }
    };

    for (int i = 0; ci[i].pText != NULL; i++)
        m_ctlList.InsertColumn(i, ci[i].pText, ci[i].nFormat, ci[i].nWidth);

    m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// 표시 컬럼 수
    m_nColCount = m_ctlList.GetHeaderCtrl()->GetItemCount();
}


//----------------------------------------------------------------------------------------------------
// 조회 결과를 표시합니다
//----------------------------------------------------------------------------------------------------
BOOL CIndexView::ReceiveChartIndex( LPRECV_PACKET pPKData )
{
	// 지표데이터는 비블럭 모드여서, Occurs 앞에 반복갯수가 포함된다
	typedef struct
	{
		ChartIndexOutBlock	outBlock;
		char				sCountOutBlock[5];	// 반복데이터 갯수
		ChartIndexOutBlock1	outBlock1[1];
	} ChartIndexOutWrap, *LPChartIndexOut;

    ChartIndexOutWrap*	pOut    = (ChartIndexOutWrap *)pPKData->lpData;
    int					nLength = pPKData->nDataLength;
    if (nLength < 0)
        return TRUE;

	// 조회 결과 표시 (최근것부터 표시)
	m_arrTRCode.Add(CString(pPKData->szTrCode));
	m_arrIndexID.Add(COMSTR(pOut->outBlock.indexid));

	// 조회 결과의 '0'번 데이터는 컬럼제목이다
	int nColCnt = atoi(COMSTR(pOut->outBlock.validdata_cnt));
	InsertColumn(nColCnt, &pOut->outBlock1[0]);

	// 조회 결과의 데이터를 표시한다
	int nRecCnt = atoi(COMSTR(pOut->sCountOutBlock));
	m_ctlList.SetItemCount(nRecCnt);

	for (int i = 1; i < nRecCnt; i++)
    {
		 InsertData(i, &pOut->outBlock1[i]);
    }

    return TRUE;
}


//----------------------------------------------------------------------------------------------------
// 유효한 컬럼을 표시합니다
//----------------------------------------------------------------------------------------------------
#define DEFAULT_COLUMN_WIDTH 80
void CIndexView::InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock)
{
	// 리스트 모두 삭제
	m_ctlList.DeleteAllItems();

	// 컬럼 삭제
	CHeaderCtrl *pHeaderCtrl = (CHeaderCtrl*)m_ctlList.GetDlgItem(0);
	if (pHeaderCtrl != NULL)
	{
		for (int i = pHeaderCtrl->GetItemCount(); i >=0; i--)
		{
			m_ctlList.DeleteColumn(0);
		}
	}

	// 컬럼 추가
	int nCol = 0;
	m_ctlList.InsertColumn( nCol++, "번호", LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->date), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->time), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->open), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->high), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->low), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->close), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->volume), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 

	if (nColCnt >= 8)
		m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value1), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	
	if (nColCnt >= 9)
		m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value2), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	
	if (nColCnt >= 10)
		m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value3), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
	
	if (nColCnt >= 11)
		m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value4), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 

	if (nColCnt >= 12)
		m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value5), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
}


//----------------------------------------------------------------------------------------------------
// 지표 표시 리스트에 데이터를 표시합니다
//----------------------------------------------------------------------------------------------------
void CIndexView::InsertData(int i, ChartIndexOutBlock1 *pBlock)
{
	int nCol = 0;
	m_ctlList.InsertItem(nCol, COMSTR(pBlock->pos)); 

	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->pos)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->date)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->time)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->open)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->high)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->low)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->close)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->volume)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value1)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value2)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value3)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value4)); 
	m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value5)); 

	m_nCurrentPos = atoi(COMSTR(pBlock->pos));
}



