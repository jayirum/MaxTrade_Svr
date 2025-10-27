// View01.cpp : ���� �����Դϴ�.
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


// CIndexView �����Դϴ�.

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


// CIndexView �޽��� ó�����Դϴ�.

void CIndexView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

	// ���屸�� �޺� �ʱ�ȭ
    InitMarketTypeCombo(&m_cmbMarket);

	// �ֱⱸ�� �޺� �ʱ�ȭ
    InitPeriodCombo(&m_cmbPeriod);

    // ��ǥǥ�� ����Ʈ �ʱ�ȭ
	InitIndexList();

	// ��ȸ���� �ʱ�ȭ
	m_edtCount.SetWindowText("100");

	// ���� ���� ��� �ʱ�ȭ
    TCHAR szPath[ MAX_PATH ] = { 0, };
    GetModuleDir( szPath, sizeof( szPath ) );
	m_edtXlsFile.SetWindowText(CString(szPath)+"����\\����VBA\\ChartExcelData.xls");

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

	// ��ǥǥ�� ����Ʈ ������ ����
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    *pResult = 0;
}


//----------------------------------------------------------------------------------------------------
// ���������͸� �����ͼ� ��ǥ�����͸� ��ȸ���� �Ǵ��մϴ�
//----------------------------------------------------------------------------------------------------
void CIndexView::OnBnClickedCheckExcel()
{
	UpdateData();

	//------------------------------------------------------------
	// ������ ������ �ð����� �����͸� �̿��� ��ǥ�����͸� �������� ����
	// false : ChartIndex TR ��ȸ
	// true  : ChartExcel TR ��ȸ  ���������͸� �̿��� ��ǥ������ ��ȸ
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
// ��Ʈ ��ǥ������ ��ȸ�� ��û�մϴ�
//----------------------------------------------------------------------------------------------------
void CIndexView::OnBtnQuery()
{
	UpdateData();

	// ��ǥID�� Ȯ���Ͽ� �̹� �������� ��ǥ��� ������ ���� �� �� ��ǥ�� ����ȸ �Ѵ�
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

	// ǥ������ ����Ʈ�� �����Ѵ�
    m_ctlList.DeleteAllItems();

	// �����ڵ带 Ȯ���Ѵ�
	int nRQID = 0;
	CString strCode;
	m_edtCode.GetWindowText(strCode);
	if (strCode.GetLength() < 2) 
	{
		AfxMessageBox("������ �����ڵ尡 �����ϴ�.");
		m_edtCode.SetFocus();
		return;
	}

	// ���屸��, �ֱⱸ��
	int nType	= GetMarketType(&m_cmbMarket);
	int nPeriod = GetPeriodType(&m_cmbPeriod);
	
	// �ǽð� �ڵ����
	int nReal = ((CButton*)GetDlgItem(IDC_CHECK_REAL))->GetCheck();

	//------------------------------------------------------------
	// ���������͸� ������ ��ǥ�����͸� ��ȸ�Ѵ�
	// �鵥���ͷ� �׾Ƴ��� ���ŵ����Ͱ� �ִٸ�,
	// "xingAPI ��ġ����\��������\ChartExcelData.xls"�� ���˴�� ������ ���� ��
	// ���������� ��������� ��ǥ�����͸� ��ȸ�� �� �ִ�
	if (m_bExcelData) 
	{
		CString strExlFile;
		m_edtXlsFile.GetWindowText(strExlFile);
		if (strExlFile.IsEmpty()) 
		{
			AfxMessageBox("���� ȭ���� �����Ͽ� �ּ���.");
			return;
		}

		ChartExcelInBlock inblock;
		memset(&inblock, 0x20, sizeof(ChartExcelInBlock));
		strcpy(inblock.indexname, m_strIndexName);				// [string,   40] ��ǥ��
		inblock.indexouttype[0] = '1';							// [string,    1] ��� ������ ����(1:��ǥ)
		inblock.market[0]		= nType+'0';					// [string,    1] ���屸��(�ֽ�:1, ����:2, �����ɼ�:5)                      
		inblock.period[0]		= nPeriod+'0' ;					// [sting,     1] �ֱⱸ��(ƽ:0, ��:1, ��:2, ��:3, ��:4)                       
		strcpy(inblock.shcode, strCode);						// [string,    8] �����ڵ�													
		inblock.isexcelout[0]   = '1';							// [string,    1] ������ ��ǥ�����͸� ������ ���� ��� ���� (1:���, 0:��¾���)
		strcpy(inblock.excelfilename, strExlFile);				// [string,  256] ���������� ���ϸ�
		inblock.IsReal[0]		= nReal+'0';					// [string,    1] �ǽð� ������ �ڵ� ��� ���� (0:��ȸ��, 1:�ǽð� �ڵ� ���)
		nRQID = g_iXingAPI.RequestService(GetSafeHwnd(), NAME_CHARTEXCEL, (LPCTSTR)&inblock);
	} 
	//------------------------------------------------------------
	// ��Ʈ ���ʵ����͸� �̿��� ��ǥ�����͸� ��ȸ�Ѵ�
	else 
	{
		CString strCount;
		m_edtCount.GetWindowText(strCount);
		int nCount = atoi(strCount);

		CString strDate = GetDate(&m_datEnd);

		ChartIndexInBlock inblock;
		memset(&inblock, 0x20, sizeof(ChartIndexInBlock));
		strcpy(inblock.indexname, m_strIndexName);			// [string,   40] ��ǥ��
		inblock.market[0]		= nType+'0';				// [string,    1] ���屸��(�ֽ�:1, ����:2, �����ɼ�:5)
		inblock.period[0]		= nPeriod+'0' ;				// [sting,     1] �ֱⱸ��(ƽ:0,��:1,��:2,��:3,��:4)                       
		strcpy(inblock.shcode, strCode);					// [string,    8] �����ڵ�													
		sprintf(inblock.qrycnt, "%d", nCount);				// [long  ,    4] ��û�Ǽ�(�ִ� 500��)
		sprintf(inblock.ncnt, "%d", 1);						// [long  ,    4] ����(nƽ/n��)         
		strcpy(inblock.edate, strDate);					    // [string,    8] ��������(��/��/�� �ش�)  											
		inblock.Isamend[0]		= '1';						// [string,    1] �����ְ� �ݿ�����(0:�ݿ�����, 1:�ݿ�)
		inblock.Isgab[0]		= '1';						// [string,    1] ������ ����(0:��������, 1:����)
		inblock.IsReal[0]		= nReal+'0';				// [string,    1] �ǽð� ������ �ڵ� ��� ���� (0:��ȸ��, 1:�ǽð� �ڵ� ���)
		nRQID = g_iXingAPI.RequestService(GetSafeHwnd(), NAME_CHARTINDEX, (LPCTSTR)&inblock);
	}

    if (nRQID < 0)
    {
        TRACE("��ǥ ��ȸ ��û�� ���� �Ͽ����ϴ�.\n");
    }
}


//----------------------------------------------------------------------------------------------------
// ��Ʈ ��ǥ������ ��ȸ ����� ���Ź޽��ϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CIndexView::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//------------------------------------------------------------
	// Data�� ����
    if (wParam == REQUEST_DATA)
    {
		ReceiveChartIndex((LPRECV_PACKET)lParam);
    }

	//------------------------------------------------------------
	// �޽����� ����
    else if (wParam == MESSAGE_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		//AfxMessageBox(strMsg);

		m_stMsg.SetWindowText(strMsg);

        g_iXingAPI.ReleaseMessageData(lParam);
	}
	
	//------------------------------------------------------------
	// System Error�� ����
    else if (wParam == SYSTEM_ERROR_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
        CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		AfxMessageBox(strMsg);

        g_iXingAPI.ReleaseMessageData(lParam);
	}

	//------------------------------------------------------------
	// Release Data�� ����
    else if (wParam == RELEASE_DATA)
    {
		g_iXingAPI.ReleaseRequestData( (int)lParam);
	}

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// ��ȸ ������ ���� ������, Ÿ�Ӿƿ� �޽����� ���Ź޽��ϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CIndexView::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
    g_iXingAPI.ReleaseRequestData( ( int )lParam );

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// ��Ʈ ��ǥ������ �ǽð� ����� ���Ź޽��ϴ�
//----------------------------------------------------------------------------------------------------
// ChartIndex TR ��ȸ ��
// Inblock�� IsReal(�ǽð� ������ �ڵ� ��� ����) �ʵ尪�� '1'�� �����ϸ� 
// �ǽð� ������ ���Ž� API���ο��� �ǽð� ��ǥ�� �����Ͽ� �޽����� �����մϴ�.
// �� ��, ���Ź޴� �ǽð� �����ʹ� ChartIndex TR�� Outblock1�� ������ �����Դϴ�.
LRESULT CIndexView::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	// ��ȸ�� OutBlock�� indexid(indexid�� ������ ���� ��ǥ�̴� - ��ǥ�� �����ϴ� key�� �Ǵܰ���)
	UINT nIndexID = (UINT)wParam;	

	// ��ǥ �ǽð� �����ʹ� 
	// RECV_REAL_PACKET�� pszData���� OutBlock1 �� �������� ���Ź޴´�
    LPRECV_REAL_PACKET pRcvData = (LPRECV_REAL_PACKET)lParam;
	ChartIndexOutBlock1 *pBlock = (ChartIndexOutBlock1 *)(pRcvData->pszData);

	int nPos = atoi(COMSTR(pBlock->pos));
	int nCol = 0; 
	
	// ������ ��ȸ(�Ǵ� �ǽð�) ���� �������� ������ ǥ�� ��ġ�� ���Ͽ�
	// �űԷ� ǥ������, ������ ��ġ�� ������Ʈ ������ �Ǵ��� �� �ִ�
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
// ��ǥ�� �����մϴ�
//----------------------------------------------------------------------------------------------------
void CIndexView::SetIndexName(int iInexType, LPCTSTR pszName )
{
	m_strIndexName = pszName;
}


//----------------------------------------------------------------------------------------------------
// ��ǥǥ�� ����Ʈ �ʱ�ȭ�մϴ�
//----------------------------------------------------------------------------------------------------
void CIndexView::InitIndexList()
{
    COLUMNDATA ci[] = 
    {
        { 80, LVCFMT_CENTER, "�Ͻ�" },
        { 80, LVCFMT_CENTER, "�ð�" },
        { 80, LVCFMT_CENTER, "��" },
        { 80, LVCFMT_CENTER, "����" },
        { 80, LVCFMT_CENTER, "����" },
        { NULL, NULL, NULL }
    };

    for (int i = 0; ci[i].pText != NULL; i++)
        m_ctlList.InsertColumn(i, ci[i].pText, ci[i].nFormat, ci[i].nWidth);

    m_ctlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// ǥ�� �÷� ��
    m_nColCount = m_ctlList.GetHeaderCtrl()->GetItemCount();
}


//----------------------------------------------------------------------------------------------------
// ��ȸ ����� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
BOOL CIndexView::ReceiveChartIndex( LPRECV_PACKET pPKData )
{
	// ��ǥ�����ʹ� ��� ��忩��, Occurs �տ� �ݺ������� ���Եȴ�
	typedef struct
	{
		ChartIndexOutBlock	outBlock;
		char				sCountOutBlock[5];	// �ݺ������� ����
		ChartIndexOutBlock1	outBlock1[1];
	} ChartIndexOutWrap, *LPChartIndexOut;

    ChartIndexOutWrap*	pOut    = (ChartIndexOutWrap *)pPKData->lpData;
    int					nLength = pPKData->nDataLength;
    if (nLength < 0)
        return TRUE;

	// ��ȸ ��� ǥ�� (�ֱٰͺ��� ǥ��)
	m_arrTRCode.Add(CString(pPKData->szTrCode));
	m_arrIndexID.Add(COMSTR(pOut->outBlock.indexid));

	// ��ȸ ����� '0'�� �����ʹ� �÷������̴�
	int nColCnt = atoi(COMSTR(pOut->outBlock.validdata_cnt));
	InsertColumn(nColCnt, &pOut->outBlock1[0]);

	// ��ȸ ����� �����͸� ǥ���Ѵ�
	int nRecCnt = atoi(COMSTR(pOut->sCountOutBlock));
	m_ctlList.SetItemCount(nRecCnt);

	for (int i = 1; i < nRecCnt; i++)
    {
		 InsertData(i, &pOut->outBlock1[i]);
    }

    return TRUE;
}


//----------------------------------------------------------------------------------------------------
// ��ȿ�� �÷��� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
#define DEFAULT_COLUMN_WIDTH 80
void CIndexView::InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock)
{
	// ����Ʈ ��� ����
	m_ctlList.DeleteAllItems();

	// �÷� ����
	CHeaderCtrl *pHeaderCtrl = (CHeaderCtrl*)m_ctlList.GetDlgItem(0);
	if (pHeaderCtrl != NULL)
	{
		for (int i = pHeaderCtrl->GetItemCount(); i >=0; i--)
		{
			m_ctlList.DeleteColumn(0);
		}
	}

	// �÷� �߰�
	int nCol = 0;
	m_ctlList.InsertColumn( nCol++, "��ȣ", LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
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
// ��ǥ ǥ�� ����Ʈ�� �����͸� ǥ���մϴ�
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



