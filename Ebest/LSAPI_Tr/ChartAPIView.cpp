// View01.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSAPIApp.h"
#include "ChartAPIView.h"
#include "CSymbols.h"
#include "CSaveCandle.h"
#include "CGlobals.h"
#include "o3103.h"
#include <iostream>
#include <chrono>

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


// CChartAPIView 진단입니다.

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


// CChartAPIView 메시지 처리기입니다.

void CChartAPIView::OnInitialUpdate()
{
	// trcnt_per_sec:1, trcnt_base_sec:1, trcnt_rqst:-1, trcnt_limit:200
	api_get_limitation_for_logging();


    CFormView::OnInitialUpdate();

	// 시장구분 콤보 초기화
	InitSymbolCombo();
	gCommon.debug("[CChartAPIView::OnInitialUpdate]InitSymbolCombo()");

	// Timeframe 콤보 초기화
	InitTimeframeCombo();
	gCommon.debug("[CChartAPIView::OnInitialUpdate]InitTimeframeCombo()");
		
	m_thrdQuery = std::thread(&CChartAPIView::threadFunc_Query, this);
	m_thrdFlag.set_run();


	UpdateData(FALSE);

}

//bool CChartAPIView::get_time_config()
//{
//	char start_tm[32], end_tm[32], apiqry_often_sec[32], apiqry_seldom_min[32];
//
//	if (!gCommon.getConfig((char*)"API_TR", (char*)"TM_START", start_tm)) {
//		gCommon.log(LOGTP_ERR, "CChartAPIView::get_time_config]ini 파일에 [API_TR]TM_START 이 있는지 확인하세요");
//		return false;
//	}
//
//	if (!gCommon.getConfig((char*)"API_TR", (char*)"TM_END", end_tm)) {
//		gCommon.log(LOGTP_ERR, "CChartAPIView::get_time_config]ini 파일에 [API_TR]TM_END 이 있는지 확인하세요");
//		return false;
//	}
//
//	if (!gCommon.getConfig((char*)"API_TR", (char*)"APIQRY_OFTEN_SEC", end_tm)) {
//		gCommon.log(LOGTP_ERR, "CChartAPIView::get_time_config]ini 파일에 [API_TR]APIQRY_OFTEN_SEC 이 있는지 확인하세요");
//		return false;
//	}
//
//	if (!gCommon.getConfig((char*)"API_TR", (char*)"APIQRY_SELDOM_MIN", end_tm)) {
//		gCommon.log(LOGTP_ERR, "CChartAPIView::get_time_config]ini 파일에 [API_TR]APIQRY_SELDOM_MIN 이 있는지 확인하세요");
//		return false;
//	}
//
//	m_check_tm = new CCheckTime(start_tm, end_tm, apiqry_often_sec, apiqry_seldom_min);
//
//	return true;
//}
//
//
//bool	CChartAPIView::get_trcode()
//{
//	char code[32] = { 0 };
//	if (!gCommon.getConfig((char*)"API_TR", (char*)"TRCODE", code)) {
//		gCommon.log(LOGTP_ERR, "CChartAPIView::get_trcode]ini 파일에 [API_TR]TRCODE 이 있는지 확인하세요");
//		return false;
//	}
//	m_sTrCode = code;
//	return true;
//}

void	CChartAPIView::api_get_limitation_for_logging()
{
	int trcnt_per_sec = g_iXingAPI.GetTRCountPerSec(m_sTrCode.c_str());
	int trcnt_base_sec = g_iXingAPI.GetTRCountBaseSec(m_sTrCode.c_str());
	int trcnt_rqst = g_iXingAPI.GetTRCountRequest(m_sTrCode.c_str());
	int trcnt_limit = g_iXingAPI.GetTRCountLimit(m_sTrCode.c_str());

	gCommon.log(INFO, "TRCountPerSec:%d, TRCountBaseSec:%d, TRCountRequest:%d, TRCountRequest:%d",
		trcnt_per_sec, trcnt_base_sec, trcnt_rqst, trcnt_limit);
}

void CChartAPIView::InitSymbolCombo()
{
	m_cmbSymbols.ResetContent();
	
	std::deque<std::string> deq_symbols = gSymbol.get_symbol();

	for (std::string symbol : deq_symbols) {
		m_cmbSymbols.AddString(symbol.c_str());
	}

	
	m_cmbSymbols.SetCurSel( 0 );
}


void CChartAPIView::InitTimeframeCombo()
{
	m_cmbTimeframe.ResetContent();

	std::deque<int> deq_tf;
	gSymbol.get_timeframe(deq_tf);
	for (int tf : deq_tf) {
		char z[32];
		sprintf(z, "%ld", tf);
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

	// 지표표시 리스트 사이즈 조절
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


//void CChartAPIView::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//    *pResult = 0;
//}

//void CChartAPIView::first_query()
//{
//	if (!is_query_time())
//		return;
//
//	int cnt = 0;
//
//	long timeframe = 0;
//	int idx = gSymbol.get_timeframe_short(timeframe, -1);
//	while (idx > -1) {
//		fetch_api_data(timeframe);
//		idx = gSymbol.get_timeframe_short(timeframe, idx);
//		cnt++;
//	}
//
//	idx = gSymbol.get_timeframe_long(timeframe, -1);
//	while (idx > -1) {
//		fetch_api_data(timeframe);
//		idx = gSymbol.get_timeframe_long(timeframe, idx);
//		cnt++;
//	}
//
//	char z[256]; sprintf(z, "[%d]개의 Request 를 최초로 전송했습니다.", cnt);
//	gCommon.log(INFO, z);
//}




//bool CChartAPIView::is_query_time()
//{
//	SYSTEMTIME st; char zNow[128]; GetLocalTime(&st);
//	sprintf(zNow, "%02d:%02d", st.wHour, st.wMinute);
//
//	bool b_market_time = true;
//	if (strcmp(m_tm_end, zNow) <= 0 && strcmp(zNow, m_tm_start) < 0)
//		b_market_time = false;
//
//	return b_market_time;
//}


void CChartAPIView::threadFunc_Query()
{
	std::this_thread::sleep_for(std::chrono::seconds(3));

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (m_thrdFlag.is_idle()) continue;

		//
		bool is_often_sec = false, is_seldom_min = false;
		bool b_go = m_check_tm.check_time(_Out_ is_often_sec, _Out_ is_seldom_min);
		//

		if (!b_go) continue;

		if (is_often_sec)
		{
			const std::deque<int>& often = gSymbol.get_timeframe_often();
			for( int tf : often){
				fetch_api_data(tf);
			}
		}

		if (is_seldom_min)
		{
			const std::deque<int>& seldom = gSymbol.get_timeframe_seldom();
			for (int tf : seldom) {
				fetch_api_data(tf);
			}
		}

		if (is_often_sec || is_seldom_min)
			m_check_tm.set_exec_time();

	} // while (!m_thrdFlag.is_stopped())

}


void CChartAPIView::fetch_api_data(int timeframe)
{
	std::deque<std::string> deq_symbols = gSymbol.get_symbol();

	int inteval = gCommon.apiqry_trinterval_sec();
	for (std::string symbol : deq_symbols) {
		requestData(symbol, timeframe);
		Sleep(inteval);
	}
}

//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회를 요청합니다
//----------------------------------------------------------------------------------------------------
void CChartAPIView::OnBtnQuery()
{
	UpdateData();

}


bool	CChartAPIView::requestData(std::string symbol, int timeframe)
{
	o3103InBlock	inBlock;
	char			t[256];

	// 데이터 초기화
	memset(&inBlock, 0x20, sizeof(inBlock));

	SetPacketData(inBlock.shcode, sizeof(inBlock.shcode), symbol.c_str(), DATA_TYPE_STRING);

	sprintf(t, "%d", timeframe);
	SetPacketData(inBlock.ncnt, sizeof(inBlock.ncnt), t, DATA_TYPE_LONG);
		
	SetPacketData(inBlock.readcnt, sizeof(inBlock.readcnt), gCommon.apiqry_read_cnt(), DATA_TYPE_LONG);

	//JAY SetPacketData(inBlock.cts_date, sizeof(inBlock.cts_date), m_tm.get_dt(), DATA_TYPE_STRING);
	//JAY SetPacketData(inBlock.cts_time, sizeof(inBlock.cts_time), m_tm.get_tm(), DATA_TYPE_STRING);
	

	//gCommon.debug("[inBlock](%.*s)", sizeof(inBlock),(char*)&inBlock);
	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = g_iXingAPI.Request(
		GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
		NAME_o3103,						// TR 번호
		(LPVOID)&inBlock,
		sizeof(inBlock)
	);
	gCommon.log(INFO, "Request(TR:%s)(symbol:%.*s)(timeframe:%.*s)(readcnt:%.*s)(dt:%.*s)(tm:%.*s)",
		NAME_o3103,
		sizeof(inBlock.shcode), inBlock.shcode, 
		sizeof(inBlock.ncnt),inBlock.ncnt, 
		sizeof(inBlock.readcnt), inBlock.readcnt, 
		sizeof(inBlock.cts_date), inBlock.cts_date, 
		sizeof(inBlock.cts_time), inBlock.cts_time
	);

	bool ret = true;
	//-----------------------------------------------------------
	// Request ID가 0보다 작을 경우에는 에러이다.
	if (nRqID < 0)
	{
		gCommon.log(ERR, "g_iXingAPI.Request 실패.API 조회 신청 에러");
		ret = false;
	}
	else
	{
		gCommon.log(INFO, "Request 성공(REQ ID:%d)", nRqID);

		char code[32], ncnt[32];
		sprintf(code, "%.*s", sizeof(inBlock.shcode), inBlock.shcode);
		sprintf(ncnt, "%.*s", sizeof(inBlock.ncnt), inBlock.ncnt);

		//
		requestID_add(nRqID, code, ncnt);
		//
	}

	return ret;
}

void	CChartAPIView::requestID_add(int nReqId, std::string symbol, std::string timeframe)
{
	auto p = std::make_unique< TReqInfo>();
	p->sSymbol = symbol;
	p->sTimeframe = timeframe;
	std::lock_guard<std::mutex> lock(m_mtxReqNo);
	m_mapReqNo[nReqId] = std::move(p);
}

//LRESULT CChartAPIView::OnAddLog(WPARAM wParam, LPARAM lParam)
//{
//	std::unique_ptr<CString> pMsg(reinterpret_cast<CString*>(lParam));
//	m_lstLog.InsertString(0, *pMsg);
//
//	// 너무 많아지면 오래된 항목 삭제 (예: 1000줄 제한)
//	const int maxLines = 100;
//	if (m_lstLog.GetCount() > maxLines)
//		m_lstLog.DeleteString(m_lstLog.GetCount() - 1);
//
//	return 0;
//}

//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회 결과를 수신받습니다
//----------------------------------------------------------------------------------------------------
LRESULT CChartAPIView::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
{
	//------------------------------------------------------------
	// Data를 받음
    if (wParam == REQUEST_DATA)
    {
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		gCommon.log(INFO, "[OnXMReceiveData](REQUEST_DATA)");
		receive_candle((LPRECV_PACKET)lParam);
    }

	//------------------------------------------------------------
	// 메시지를 받음
    else if (wParam == MESSAGE_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		//gCommon.log(INFO, "[OnXMReceiveData](MESSAGE_DATA)(%.512s)", (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}
	
	//------------------------------------------------------------
	// System Error를 받음
    else if (wParam == SYSTEM_ERROR_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
        CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		//gCommon.log(ERR, "[OnXMReceiveData](SYSTEM_ERROR_DATA)(%.*s)",pMsg->nMsgLength, (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}

	//------------------------------------------------------------
	// Release Data를 받음
    else if (wParam == RELEASE_DATA)
    {
		gCommon.log(INFO, "[OnXMReceiveData](RELEASE_DATA)(%d)", (int)lParam);
		g_iXingAPI.ReleaseRequestData( (int)lParam);
	}

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// 조회 결과를 표시합니다
//----------------------------------------------------------------------------------------------------
bool CChartAPIView::receive_candle(LPRECV_PACKET pPKData)
{
	int reqId				= pPKData->nRqID;
	int	nDataLength			= pPKData->nDataLength;
	int nDataMode			= pPKData->nDataMode;
	std::string sBlockName	= pPKData->szBlockName;
	char cCont				= pPKData->cCont[0];

	gCommon.debug("[receive_candle]reqid:%d, DataLen:%d, DataMode:%d, BlockName:%s, cCont:%c",
		reqId, nDataLength, pPKData->nDataMode, pPKData->szBlockName, cCont);

	if (nDataLength < 0)
		return true;

	char zTimeDiff[32] = { 0 };


	if (sBlockName.compare(NAME_o3103OutBlock) == 0)
	{
		if (cCont == '0')	// 다음조회 없음
		{
			gCommon.log(ERR, "[receive_candle] o3103OutBlock 인데 다음조회가 없다.");
			return false;
		}
		
		o3103OutBlock* p = (o3103OutBlock*)pPKData->lpData;
		sprintf(zTimeDiff, "%.*s", sizeof(p->timediff), p->timediff);	// 시차

		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				gCommon.log(ERR, "[receive_candle]수신한 데이터의 REQ ID(%d) 가 map 에 없다.", reqId);
				return false;
			}
			itReq->second->sTimeDiff = zTimeDiff;
		}		
	}
	else if (sBlockName.compare(NAME_o3103OutBlock1) == 0)
	{
		int nLoop = nDataLength / sizeof(o3103OutBlock1);
		o3103OutBlock1* pOut = (o3103OutBlock1*)pPKData->lpData;

		std::string sSymbol, sTimeframe, sTimeDiff;
		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				gCommon.log(ERR, "[receive_candle]수신한 데이터의 REQ ID(%d) 가 map 에 없다.", reqId);
				return false;
			}
			sSymbol = itReq->second->sSymbol;
			sTimeframe = itReq->second->sTimeframe;
			sTimeDiff = itReq->second->sTimeDiff;

			m_mapReqNo.erase(itReq);
		}

		for (int i = 0; i < nLoop; i++)
		{
			save_candle_data(sSymbol, sTimeframe, sTimeDiff, pOut);
			pOut++;
		}
	}
	return true;
}

bool	CChartAPIView::save_candle_data(std::string sSymbol, std::string sTimeframe, std::string sTimeDiff, o3103OutBlock1* pBlock)
{
	char dt[32], tm[32], o[32], h[32], l[32], c[32], v[32];
	sprintf(dt, "%.*s", sizeof(pBlock->date), pBlock->date);
	sprintf(tm, "%.*s", sizeof(pBlock->time), pBlock->time);
	sprintf(o, "%.*s", sizeof(pBlock->open), pBlock->open);
	sprintf(h, "%.*s", sizeof(pBlock->high), pBlock->high);
	sprintf(l, "%.*s", sizeof(pBlock->low), pBlock->low);
	sprintf(c, "%.*s", sizeof(pBlock->close), pBlock->close);
	sprintf(v, "%.*s", sizeof(pBlock->volume), pBlock->volume);

	gCommon.debug("[save_candle_data](symbol:%s)(timeframe:%s)(timediff:%s)(dt:%s)(tm:%s)(o:%s)(h:%s)(l:%s)(c:%s)(v:%s)",
		sSymbol.c_str(), sTimeframe.c_str(), sTimeDiff.c_str(),
		dt, tm, o, h, l, c, v)
		;

	gSaveCandle.save(std::stol(sTimeframe), sSymbol, dt, tm, std::stoi(sTimeDiff), atof(o), atof(h), atof(l), atof(c), atoi(v));
	return true;
}


//----------------------------------------------------------------------------------------------------
// 조회 응답이 오지 않으면, 타임아웃 메시지를 수신받습니다
//----------------------------------------------------------------------------------------------------
LRESULT CChartAPIView::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	gCommon.log(INFO, "[OnXMTimeoutData]수신");
    g_iXingAPI.ReleaseRequestData( ( int )lParam );

    return 1L;
}

//
//
//void CChartAPIView::AppendLogMessage( std::string message)
//{
//	auto pStr = new CString(message.c_str()); // PostMessage라 포인터 수명 보장 필요
//	PostMessage(WM_ADD_LOG, 0, reinterpret_cast<LPARAM>(pStr));
//}


//----------------------------------------------------------------------------------------------------
// 유효한 컬럼을 표시합니다
//----------------------------------------------------------------------------------------------------
//#define DEFAULT_COLUMN_WIDTH 80
//void CChartAPIView::InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock)
//{
//	// 리스트 모두 삭제
//	//JAY m_ctlList.DeleteAllItems();
//
//	// 컬럼 삭제
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
//	//// 컬럼 추가
//	//int nCol = 0;
//	//m_ctlList.InsertColumn( nCol++, "번호", LVCFMT_CENTER, DEFAULT_COLUMN_WIDTH); 
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
// 지표 표시 리스트에 데이터를 표시합니다
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


