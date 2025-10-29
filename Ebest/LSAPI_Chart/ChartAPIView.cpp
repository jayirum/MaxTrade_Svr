// View01.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LSAPIApp.h"
#include "ChartAPIView.h"
#include "CGlobals.h"
#include "o3103.h"
#include <iostream>
#include <chrono>
#include "CTimeframeOfSymbols.h"
#include "CDBWorks.h"
#include "../../Common/StringUtils.h"


#define COMSTR(st) CString(st, sizeof(st))


// CChartAPIView
IMPLEMENT_DYNCREATE(CChartAPIView, CFormView)

CChartAPIView::CChartAPIView()
	: CFormView(CChartAPIView::IDD)
{
	m_nColCount		= 0;
	m_nCurrentPos	= 0;
	m_thrdFlag.set_idle();
}

CChartAPIView::~CChartAPIView()
{
	m_thrdFlag.set_stop();

	if (m_thrdQuery.joinable())
		m_thrdQuery.join();
}

void CChartAPIView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MARKET,		m_cmbSymbols);
    DDX_Control(pDX, IDC_PERIOD,		m_cmbTimeframe);
    DDX_Control(pDX, IDC_COUNT,			m_edtCount);
	DDX_Control(pDX, IDC_ST_MSG,		m_stMsg);
	DDX_Control(pDX, IDC_LIST_LOG,		m_lstLog);
}

BEGIN_MESSAGE_MAP(CChartAPIView, CFormView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    //ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST,				&CChartAPIView::OnLvnItemchangedList)
    ON_BN_CLICKED(IDC_QUERY,							&CChartAPIView::OnBtnQuery)
    ON_MESSAGE(WM_USER + XM_RECEIVE_DATA,				OnXMReceiveData)
    ON_MESSAGE(WM_USER + XM_TIMEOUT_DATA,				OnXMTimeoutData)   
END_MESSAGE_MAP()


// CChartAPIView �����Դϴ�.

#ifdef _DEBUG
void CChartAPIView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CChartAPIView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



// CChartAPIView �޽��� ó�����Դϴ�.

void CChartAPIView::OnInitialUpdate()
{
	bool	connect_db();
	bool	load_symbols_timeframes();


	// trcnt_per_sec:1, trcnt_base_sec:1, trcnt_rqst:-1, trcnt_limit:200
	api_get_limitation_for_logging();


    CFormView::OnInitialUpdate();

	// ���屸�� �޺� �ʱ�ȭ
	//InitSymbolCombo();
	//__common.debug("[CChartAPIView::OnInitialUpdate]InitSymbolCombo()");

	// Timeframe �޺� �ʱ�ȭ
	InitTimeframeCombo();
	__common.debug("[CChartAPIView::OnInitialUpdate]InitTimeframeCombo()");
		
	m_thrdQuery = std::thread(&CChartAPIView::threadFunc_Query, this);
	m_thrdFlag.set_run();


	UpdateData(FALSE);

}


void	CChartAPIView::api_get_limitation_for_logging()
{
	int trcnt_per_sec = g_iXingAPI.GetTRCountPerSec(__common.get_api_tr());
	int trcnt_base_sec = g_iXingAPI.GetTRCountBaseSec(__common.get_api_tr());
	int trcnt_rqst = g_iXingAPI.GetTRCountRequest(__common.get_api_tr());
	int trcnt_limit = g_iXingAPI.GetTRCountLimit(__common.get_api_tr());

	__common.log_fmt(INFO, "[%s]TRCountPerSec:%d, TRCountBaseSec:%d, TRCountRequest:%d, TRCountRequest:%d",
		__common.get_api_tr(), trcnt_per_sec, trcnt_base_sec, trcnt_rqst, trcnt_limit);
}

void CChartAPIView::InitSymbolCombo()
{
	m_cmbSymbols.ResetContent();
	
	//TODO std::deque<std::string> deq_symbols = gSymbol.get_symbol();

	//TODO for (std::string symbol : deq_symbols) {
	//TODO 	m_cmbSymbols.AddString(symbol.c_str());
	//TODO }

	
	m_cmbSymbols.SetCurSel( 0 );
}


void CChartAPIView::InitTimeframeCombo()
{
	m_cmbTimeframe.ResetContent();

	for (auto& kv : __map_tfs_symbols) {
		char z[32];
		sprintf(z, "%ld", kv.first);
		m_cmbTimeframe.AddString(z);
	}

	m_cmbTimeframe.SetCurSel(0);
}

int CChartAPIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFormView::OnCreate(lpCreateStruct) == -1)
        return -1;

	return 0;
}


void CChartAPIView::OnDestroy()
{
    CFormView::OnDestroy();
}


void CChartAPIView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

	// ��ǥǥ�� ����Ʈ ������ ����
    if (m_lstLog.GetSafeHwnd())
    {
        CRect rect;
        m_stMsg.GetWindowRect(&rect);
        ScreenToClient(rect);
        rect.bottom = cy - 1;
        rect.right	= cx - 1;
		rect.top = rect.bottom - 20;
		m_stMsg.SetWindowPos( NULL, 0, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);

		m_lstLog.GetWindowRect(&rect);
        ScreenToClient(rect);
        rect.bottom = cy - 1 - 20 - 5;
		rect.right	= cx - 5;
		m_lstLog.SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height(),  SWP_NOZORDER|SWP_NOMOVE);
    }
}




void CChartAPIView::threadFunc_Query()
{
	std::this_thread::sleep_for(std::chrono::seconds(3));

	//#
	first_api_qry();

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (m_thrdFlag.is_idle()) continue;
				
		int fired_num = check_qrytime_all();

		if(fired_num>0){
			__common.log_fmt(INFO, "[APIQRY�ð�üũ]�� [%d] ���� ��Ʈ�� ��û�ؾ� �Ѵ�.(Timeframe X Symbol ����)", fired_num);
			fetch_candles_apidata();
		}

	} // while (!m_thrdFlag.is_stopped())

}

void	CChartAPIView::fetch_candles_apidata()
{
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);

	for (auto& [tf, tfclass] : __map_tfs_symbols)
	{
		for (auto& [symbol, symbolclass] : tfclass->m_map_symbols)
		{
			if (symbolclass->is_time_to_apiqry())
			{
				//__common.debug_fmt("[API REQUEST](timeframe:%d)(symbol:%s)", tf, symbol.c_str());
				fetch_apidata(symbol, tf, false);
				std::this_thread::sleep_for(std::chrono::milliseconds(__common.apiqry_interval_ms()));

			}
		}
	}
}

void	CChartAPIView::first_api_qry()
{
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);

	for (auto& [tf, tfclass] : __map_tfs_symbols)					//std::map<int, std::unique_ptr<CTimeframeOfSymbols>>
	{
		for (auto& [symbol, symbolclass] : tfclass->m_map_symbols)	//std::map<SYMBOL_STR, std::unique_ptr<CSymbol>>
		{
			if (!fetch_apidata(symbol, tf, true))
			{
				//TODO
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(__common.apiqry_interval_ms()));
		}
	}
}



bool	CChartAPIView::fetch_apidata(std::string symbol, int timeframe, bool is_first)
{
	o3103InBlock	inBlock;
	char			t[256];

	// ������ �ʱ�ȭ
	memset(&inBlock, 0x20, sizeof(inBlock));

	SetPacketData(inBlock.shcode, sizeof(inBlock.shcode), symbol.c_str(), DATA_TYPE_STRING);

	sprintf(t, "%d", timeframe);
	SetPacketData(inBlock.ncnt, sizeof(inBlock.ncnt), t, DATA_TYPE_LONG);
	
	if(is_first)
		SetPacketData(inBlock.readcnt, sizeof(inBlock.readcnt), __common.apiqry_qry_cnt_first(), DATA_TYPE_LONG);
	else
		SetPacketData(inBlock.readcnt, sizeof(inBlock.readcnt), __common.apiqry_qry_cnt(), DATA_TYPE_LONG);

	//JAY SetPacketData(inBlock.cts_date, sizeof(inBlock.cts_date), m_tm.get_dt(), DATA_TYPE_STRING);
	//JAY SetPacketData(inBlock.cts_time, sizeof(inBlock.cts_time), m_tm.get_tm(), DATA_TYPE_STRING);
	

	//__common.log_fmt(INFO, "[fetch_apidata](%.*s)", sizeof(inBlock),(char*)&inBlock);
	
	
	
	//-----------------------------------------------------------
	// ������ ����
	int nRqID = 0;
	bool ret = true;

	for( int i=0; i<3; i++)
	{
		nRqID = g_iXingAPI.Request(
			GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_DATA ���� �´�.
			NAME_o3103,						// TR ��ȣ
			(LPVOID)&inBlock,
			sizeof(inBlock)
		);
	
		//-----------------------------------------------------------
		// Request ID�� 0���� ���� ��쿡�� �����̴�.
		if (nRqID < 0)
		{
			__common.log_fmt(ERR, "g_iXingAPI.Request ����.API ��ȸ ��û ����(REQID:%d)(%s)", nRqID, g_iXingAPI.GetErrorMessage(nRqID));
			__common.log_fmt(ERR, "Request����(TR:%s)(symbol:%.*s)(timeframe:%.*s)(qry_cnt:%.*s)(dt:%.*s)(tm:%.*s)",
				NAME_o3103,
				sizeof(inBlock.shcode), inBlock.shcode,
				sizeof(inBlock.ncnt), inBlock.ncnt,
				sizeof(inBlock.readcnt), inBlock.readcnt,
				sizeof(inBlock.cts_date), inBlock.cts_date,
				sizeof(inBlock.cts_time), inBlock.cts_time
			);
			ret = false;
			Sleep(2000);
		}
		else{
			ret = true;
			break;
		}
	}
	if (nRqID > 0)
	{
		__common.log_fmt(INFO, "[Request����](TR:%s)(symbol:%.*s)(timeframe:%.*s)(qry_cnt:%.*s)(dt:%.*s)(tm:%.*s)",
			NAME_o3103,
			sizeof(inBlock.shcode), inBlock.shcode,
			sizeof(inBlock.ncnt), inBlock.ncnt,
			sizeof(inBlock.readcnt), inBlock.readcnt,
			sizeof(inBlock.cts_date), inBlock.cts_date,
			sizeof(inBlock.cts_time), inBlock.cts_time
		);

		char code[32], ncnt[32];
		sprintf(code, "%.*s", sizeof(inBlock.shcode), inBlock.shcode);
		CStringUtils util; util.trim_all(code);

		sprintf(ncnt, "%.*s", sizeof(inBlock.ncnt), inBlock.ncnt);

		//#
		requestID_add(nRqID, code, ncnt);
	}
	
	return ret;
}

int	CChartAPIView::check_qrytime_all()
{
	char now[15 + 1];	//yyyymmdd_hhmmss
	CTimeUtils time_util;
	strcpy(now, time_util.DateTime_yyyymmddhhmmss());

	int num = 0;
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);
	for (auto& [tf, tfclass] : __map_tfs_symbols)					//std::map<int, std::unique_ptr<CTimeframeOfSymbols>>
	{
		num += tfclass->check_time_to_apiqry_symbols(now);
	}
	return num;
}


void	CChartAPIView::requestID_add(int nReqId, const char* symbol, const char* timeframe)
{
	auto p = std::make_shared< TReqInfo>();
	p->sSymbol		= symbol;
	p->sTimeframe	= timeframe;

	std::lock_guard<std::mutex> lock(m_mtxReqNo);
	m_mapReqNo[nReqId] = p;
}


//----------------------------------------------------------------------------------------------------
// ��Ʈ ��ǥ������ ��ȸ ����� ���Ź޽��ϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CChartAPIView::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//------------------------------------------------------------
	// Data�� ����
    if (wParam == REQUEST_DATA)
    {
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		//__common.log(INFO, "[OnXMReceiveData](REQUEST_DATA)");
		recv_apidata_proc((LPRECV_PACKET)lParam);
    }

	//------------------------------------------------------------
	// �޽����� ����
    else if (wParam == MESSAGE_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		//__common.log(INFO, "[OnXMReceiveData](MESSAGE_DATA)(%.512s)", (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}
	
	//------------------------------------------------------------
	// System Error�� ����
    else if (wParam == SYSTEM_ERROR_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
        CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		//__common.log(ERR, "[OnXMReceiveData](SYSTEM_ERROR_DATA)(%.*s)",pMsg->nMsgLength, (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}

	//------------------------------------------------------------
	// Release Data�� ����
    else if (wParam == RELEASE_DATA)
    {
		//__common.log_fmt(INFO, "[OnXMReceiveData](RELEASE_DATA)(%d)", (int)lParam);
		g_iXingAPI.ReleaseMessageData( (int)lParam);
	}

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// ��ȸ ����� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
bool CChartAPIView::recv_apidata_proc(LPRECV_PACKET pPKData)
{
	int reqId				= pPKData->nRqID;
	int	nDataLength			= pPKData->nDataLength;
	int nDataMode			= pPKData->nDataMode;
	std::string sBlockName	= pPKData->szBlockName;
	char cCont				= pPKData->cCont[0];

	//__common.debug_fmt("[recv_apidata_proc]reqid:%d, DataLen:%d, DataMode:%d, BlockName:%s, cCont:%c",
	//	reqId, nDataLength, pPKData->nDataMode, pPKData->szBlockName, cCont);

	if (nDataLength < 0)
		return true;

	char zTimeDiff[32] = { 0 };


	if (sBlockName.compare(NAME_o3103OutBlock) == 0)
	{
		if (cCont == '0')	// ������ȸ ����
		{
			__common.log(ERR, "[recv_apidata_proc] o3103OutBlock �ε� ������ȸ�� ����.");
			return false;
		}
		
		o3103OutBlock* p = (o3103OutBlock*)pPKData->lpData;
		sprintf(zTimeDiff, "%.*s", sizeof(p->timediff), p->timediff);	// ����

		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				__common.log_fmt(ERR, "[recv_apidata_proc]������ �������� REQ ID(%d) �� map �� ����.", reqId);
				return false;
			}
			itReq->second->sTimeDiff = zTimeDiff;
		}
	}
	else if (sBlockName.compare(NAME_o3103OutBlock1) == 0)
	{
		int block_cnt = nDataLength / sizeof(o3103OutBlock1);
		o3103OutBlock1* pOut = (o3103OutBlock1*)pPKData->lpData;

		std::string symbol, timeframe, timediff;

		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				__common.log_fmt(ERR, "[recv_apidata_proc]������ �������� REQ ID(%d) �� map �� ����.", reqId);
				return false;
			}
			CStringUtils utils;
			symbol		= utils.trim_str(itReq->second->sSymbol);
			timeframe	= itReq->second->sTimeframe;
			timediff	= itReq->second->sTimeDiff;

			m_mapReqNo.erase(itReq);
			//__common.debug_fmt("[MAP REQ ����](REQ ID:%d)(Timeframe:%s)(Symbol:%s)", reqId, timeframe.c_str(), symbol.c_str());
		}

		
		for(int i=0; i< block_cnt; i++){
			if (!save_candle_data(symbol, timeframe, timediff, pOut, (i==0)))
			{
				//TODO
			}
			pOut++;
		}
	}
	return true;
}



bool	CChartAPIView::save_candle_data(std::string sSymbol, std::string sTimeframe, std::string sTimeDiff, o3103OutBlock1* pBlock, bool b_update_candle_tm)
{
	char dt[32], tm[32], o[32], h[32], l[32], c[32], v[32];
	sprintf(dt, "%.*s", sizeof(pBlock->date), pBlock->date);
	sprintf(tm, "%.*s", sizeof(pBlock->time), pBlock->time);
	sprintf(o, "%.*s", sizeof(pBlock->open), pBlock->open);
	sprintf(h, "%.*s", sizeof(pBlock->high), pBlock->high);
	sprintf(l, "%.*s", sizeof(pBlock->low), pBlock->low);
	sprintf(c, "%.*s", sizeof(pBlock->close), pBlock->close);
	sprintf(v, "%.*s", sizeof(pBlock->volume), pBlock->volume);

	__common.debug_fmt("\t<save_candle_data>(symbol:%.s)(timeframe:%s)(timediff:%.2s)(dt:%s)(tm:%s)(o:%.8s)(h:%.8s)(l:%.8s)(c:%.8s)(v:%.8s)",
		sSymbol.c_str(), sTimeframe.c_str(), sTimeDiff.c_str(),
		dt, tm, o, h, l, c, v)
		;

	int tf = std::stol(sTimeframe);
	bool ret = __dbworks.save_chartdata(
				tf, 
				sSymbol, 
				dt, 
				tm, 
				std::stoi(sTimeDiff), 
				atof(o), atof(h), atof(l), atof(c), atoi(v)
				);
	if (!ret) {
		return false;
	}

	// map �� ����
	if(b_update_candle_tm){
		std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);
		auto it = __map_tfs_symbols.find(tf);
		if (it == __map_tfs_symbols.end()) {
			__common.log_fmt(ERR, "[save_candle_data] map �� �����Ͱ� ����(timeframe:%d)", tf);
			return false;
		}
		it->second->update_candle_tm(sSymbol.c_str(), (const char*)dt, (const char*)tm, sTimeDiff.c_str());
	}

	return true;
}


//----------------------------------------------------------------------------------------------------
// ��ȸ ������ ���� ������, Ÿ�Ӿƿ� �޽����� ���Ź޽��ϴ�
//----------------------------------------------------------------------------------------------------
LRESULT CChartAPIView::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	__common.log(INFO, "[OnXMTimeoutData]����");
    g_iXingAPI.ReleaseMessageData( ( int )lParam );

    return 1L;
}




//----------------------------------------------------------------------------------------------------
// ��Ʈ ��ǥ������ ��ȸ�� ��û�մϴ�
//----------------------------------------------------------------------------------------------------
void CChartAPIView::OnBtnQuery()
{
	UpdateData();

}


//
//
//void CChartAPIView::AppendLogMessage( std::string message)
//{
//	auto pStr = new CString(message.c_str()); // PostMessage�� ������ ���� ���� �ʿ�
//	PostMessage(WM_ADD_LOG, 0, reinterpret_cast<LPARAM>(pStr));
//}


//----------------------------------------------------------------------------------------------------
// ��ȿ�� �÷��� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
//#define DEFAULT_COLUMN_WIDTH 80
//void CChartAPIView::InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock)
//{
//	// ����Ʈ ��� ����
//	//JAY m_ctlList.DeleteAllItems();
//
//	// �÷� ����
//	//JAY 
//	//CHeaderCtrl *pHeaderCtrl = (CHeaderCtrl*)m_ctlList.GetDlgItem(0);
//	//if (pHeaderCtrl != NULL)
//	//{
//	//	for (int i = pHeaderCtrl->GetItemCount(); i >=0; i--)
//	//	{
//	//		m_ctlList.DeleteColumn(0);
//	//	}
//	//}
//
//	//// �÷� �߰�
//	//int nCol = 0;
//	//m_ctlList.InsertColumn( nCol++, "��ȣ", LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->date), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->time), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->open), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->high), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->low), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->close), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->volume), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//
//	//if (nColCnt >= 8)
//	//	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value1), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//
//	//if (nColCnt >= 9)
//	//	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value2), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//
//	//if (nColCnt >= 10)
//	//	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value3), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//	//
//	//if (nColCnt >= 11)
//	//	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value4), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//
//	//if (nColCnt >= 12)
//	//	m_ctlList.InsertColumn( nCol++, COMSTR(pBlock->value5), LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
//}


//----------------------------------------------------------------------------------------------------
// ��ǥ ǥ�� ����Ʈ�� �����͸� ǥ���մϴ�
//----------------------------------------------------------------------------------------------------
//void CChartAPIView::InsertData(int i, ChartIndexOutBlock1 *pBlock)
//{
//	int nCol = 0;
//	//JAY m_ctlList.InsertItem(nCol, COMSTR(pBlock->pos)); 
//	//JAY 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->pos)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->date)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->time)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->open)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->high)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->low)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->close)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->volume)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value1)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value2)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value3)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value4)); 
//	//m_ctlList.SetItemText(0, nCol++, COMSTR(pBlock->value5)); 
//
//	//m_nCurrentPos = atoi(COMSTR(pBlock->pos));
//}


