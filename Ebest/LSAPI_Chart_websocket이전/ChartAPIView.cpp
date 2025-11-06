/*========================================================
3분봉이 10:42, 10:45, 10:48 과 같이 구성이 되는 상황이면
10:42:01~10:44:59 까지 차트시간은 10:45 가 된다. 
따라서, 10:45 분봉이 완료된 후에 그 데이터를 읽어내려면 api 로 2개의 데이터를 요청해서 그 중 맞는 것을 선택해야 한다.
========================================================*/

#include "stdafx.h"
#include "LSAPIApp.h"
#include "ChartAPIView.h"
#include "CGlobals.h"
#include "o3103.h"
#include <iostream>
#include <chrono>
#include "CSymbolSets.h"
#include "CDBWorks.h"
#include "../../Common/StringUtils.h"
#include "CIOCPServer.h"
#include <json.hpp>

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

	if(m_thrdQuery.joinable())	m_thrdQuery.join(); 	
	if(m_thrd_save.joinable())	m_thrd_save.join();
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
	//InitSymbolCombo();
	//__common.debug("[CChartAPIView::OnInitialUpdate]InitSymbolCombo()");

	// Timeframe 콤보 초기화
	InitTimeframeCombo();
	__common.debug("[CChartAPIView::OnInitialUpdate]InitTimeframeCombo()");
		
	m_thrdQuery = std::thread(&CChartAPIView::threadFunc_api_query, this);
	m_thrdFlag.set_run();

	m_thrd_save = std::thread(&CChartAPIView::threadFunc_save, this);

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
	
	set<string> symbols = __get_symbols();

	for (auto& s : symbols) {
		m_cmbTimeframe.AddString(s.c_str());
	}

	m_cmbSymbols.SetCurSel( 0 );
}


void CChartAPIView::InitTimeframeCombo()
{
	m_cmbTimeframe.ResetContent();

	set<int> tfs = __get_timeframes();

	for (auto& tf : tfs) {
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




void CChartAPIView::threadFunc_api_query()
{
	std::this_thread::sleep_for(std::chrono::seconds(3));

	//===== 첫번째 데이터를 조회한다 =====/
	first_api_qry();

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (m_thrdFlag.is_idle()) continue;
				
		//
		fetch_candles_apidata();
		//

	}
}

void	CChartAPIView::fetch_candles_apidata()
{
	CTimeUtils util;
	string now_ymd_hms = util.sDateTime_yyyymmdd_hhmmss();

	for (auto& sym_ptr : __SymbolSets)
	{
		if (sym_ptr->is_time_api_qry(now_ymd_hms))
		{
			//__common.debug_fmt("[API REQUEST](timeframe:%d)(symbol:%s)", tf, symbol.c_str());
			send_api_request(sym_ptr->m_symbol, sym_ptr->m_timeframe, false);
			std::this_thread::sleep_for(std::chrono::milliseconds(__common.apiqry_interval_ms()));
		}
	}
	
}

void	CChartAPIView::first_api_qry()
{
	__common.debug("[First Query] Start...");
	for (auto& symPtr : __SymbolSets)
	{		
		if (!send_api_request(symPtr->m_symbol, symPtr->m_timeframe, true))
		{
			//TODO
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(__common.apiqry_interval_ms()));
	}
}



bool	CChartAPIView::send_api_request(std::string& symbol, int timeframe, bool is_first)
{
	o3103InBlock	inBlock;
	char			t[256];

	// 데이터 초기화
	memset(&inBlock, 0x20, sizeof(inBlock));

	SetPacketData(inBlock.shcode, sizeof(inBlock.shcode), symbol.c_str(), DATA_TYPE_STRING);

	sprintf(t, "%d", timeframe);
	SetPacketData(inBlock.ncnt, sizeof(inBlock.ncnt), t, DATA_TYPE_LONG);
	
	int read_cnt = (is_first)? APIQRYCNT_FIRST : APIQRYCNT_NEXT;
	if(read_cnt > MIN_QRY_CNT ) read_cnt = MIN_QRY_CNT;
	sprintf(t, "%d", read_cnt);
	SetPacketData(inBlock.readcnt, sizeof(inBlock.readcnt), t, DATA_TYPE_LONG);
		
	//JAY SetPacketData(inBlock.cts_date, sizeof(inBlock.cts_date), m_tm.get_dt(), DATA_TYPE_STRING);
	//JAY SetPacketData(inBlock.cts_time, sizeof(inBlock.cts_time), m_tm.get_tm(), DATA_TYPE_STRING);
		
	
	//-----------------------------------------------------------
	// 데이터 전송
	int nRqID = 0;
	bool ret = true;

	for( int i=0; i<3; i++)
	{
		nRqID = g_iXingAPI.Request(
			GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
			NAME_o3103,						// TR 번호
			(LPVOID)&inBlock,
			sizeof(inBlock)
		);
	
		//-----------------------------------------------------------
		// Request ID가 0보다 작을 경우에는 에러이다.
		if (nRqID < 0)
		{
			__common.log_fmt(ERR, "g_iXingAPI.Request 실패.API 조회 신청 에러(REQID:%d)(%s)", nRqID, g_iXingAPI.GetErrorMessage(nRqID));
			__common.log_fmt(ERR, "Request실패(TR:%s)(symbol:%.*s)(timeframe:%.*s)(qry_cnt:%.*s)(dt:%.*s)(tm:%.*s)",
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
		__common.log_fmt(INFO, "[Request성공](TR:%s)(symbol:%.*s)(timeframe:%.*s)(qry_cnt:%.*s)(dt:%.*s)(tm:%.*s)",
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


void	CChartAPIView::requestID_add(int nReqId, const char* symbol, const char* timeframe)
{
	auto p = std::make_shared< TReqInfo>();
	p->sSymbol		= symbol;
	p->sTimeframe	= timeframe;

	std::lock_guard<std::mutex> lock(m_mtxReqNo);
	m_mapReqNo[nReqId] = p;
}


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
		//__common.log(INFO, "[OnXMReceiveData](REQUEST_DATA)");
		recv_apidata_proc((LPRECV_PACKET)lParam);
    }

	//------------------------------------------------------------
	// 메시지를 받음
    else if (wParam == MESSAGE_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
		//__common.log(INFO, "[OnXMReceiveData](MESSAGE_DATA)(%.512s)", (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}
	
	//------------------------------------------------------------
	// System Error를 받음
    else if (wParam == SYSTEM_ERROR_DATA)
    {
        LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;
        CString strMsg((char *)pMsg->lpszMessageData, pMsg->nMsgLength);
		//__common.log(ERR, "[OnXMReceiveData](SYSTEM_ERROR_DATA)(%.*s)",pMsg->nMsgLength, (char*)pMsg->lpszMessageData);

        g_iXingAPI.ReleaseMessageData(lParam);
	}

	//------------------------------------------------------------
	// Release Data를 받음
    else if (wParam == RELEASE_DATA)
    {
		//__common.log_fmt(INFO, "[OnXMReceiveData](RELEASE_DATA)(%d)", (int)lParam);
		g_iXingAPI.ReleaseRequestData( (int)lParam);
	}

    return 1L;
}


//----------------------------------------------------------------------------------------------------
// 조회 결과를 표시합니다
//----------------------------------------------------------------------------------------------------
bool CChartAPIView::recv_apidata_proc(LPRECV_PACKET pPKData)
{
	int reqId				= pPKData->nRqID;
	int	nDataLength			= pPKData->nDataLength;
	int nDataMode			= pPKData->nDataMode;
	std::string sBlockName	= pPKData->szBlockName;
	char cCont				= pPKData->cCont[0];

	if (nDataLength < 0)
		return true;

	char zTimeDiff[32] = { 0 };


	if (sBlockName.compare(NAME_o3103OutBlock) == 0)
	{	
		o3103OutBlock* pOut = (o3103OutBlock*)pPKData->lpData;
		
		__common.debug_fmt("\t<NAME_o3103OutBlock> (reqid:%d)(DataLen:%d)(DataMode:%d)(BlockName:%s)(cCont:%c)(dt:%.8s)(tm:%.6s)",
			reqId, nDataLength, pPKData->nDataMode, pPKData->szBlockName, cCont, pOut->cts_date, pOut->cts_time);

		sprintf(zTimeDiff, "%.*s", sizeof(pOut->timediff), pOut->timediff);	// 시차

		char read_cnt[32]; sprintf(read_cnt, "%.*s", sizeof(pOut->readcnt), pOut->readcnt);
		int cnt = atoi(read_cnt);
		if (cnt == 0) {
			__common.log(ERR, "[recv_apidata_proc] o3103OutBlock 인데 read cnt 가 없다.");
			return false;
		}

		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				__common.log_fmt(ERR, "[recv_apidata_proc]수신한 데이터의 REQ ID(%d) 가 map 에 없다.", reqId);
				return false;
			}
			itReq->second->sTimeDiff = zTimeDiff;
		}
	}
	else if (sBlockName.compare(NAME_o3103OutBlock1) == 0)
	{
		int block_cnt = nDataLength / sizeof(o3103OutBlock1);
		o3103OutBlock1* pOut = (o3103OutBlock1*)pPKData->lpData;

		__common.debug_fmt("\t<NAME_o3103OutBlock1> (reqid:%d)(DataLen:%d)(DataMode:%d)(BlockName:%s)(cCont:%c)(dt:%.8s)(tm:%.6s)",
			reqId, nDataLength, pPKData->nDataMode, pPKData->szBlockName, cCont, pOut->date, pOut->time);

		std::string symbol, timeframe, timediff;

		{
			std::lock_guard<std::mutex> lock(m_mtxReqNo);
			auto itReq = m_mapReqNo.find(reqId);
			if (itReq == m_mapReqNo.end()) {
				__common.log_fmt(ERR, "[recv_apidata_proc]수신한 데이터의 REQ ID(%d) 가 map 에 없다.", reqId);
				return false;
			}
			CStringUtils utils;
			symbol		= utils.trim_str(itReq->second->sSymbol);
			timeframe	= itReq->second->sTimeframe;
			timediff	= itReq->second->sTimeDiff;

			m_mapReqNo.erase(itReq);
			//__common.debug_fmt("[MAP REQ 삭제](REQ ID:%d)(Timeframe:%s)(Symbol:%s)", reqId, timeframe.c_str(), symbol.c_str());
		}

		
		for(int i=0; i< block_cnt; i++)
		{
			if (save_candle_data(symbol, timeframe, timediff, pOut))
			{
				break;
			}
			pOut++;
		}
	}
	return true;
}



bool	CChartAPIView::save_candle_data(std::string& sSymbol, std::string& sTimeframe, std::string& sTimeDiff, 
										o3103OutBlock1* pBlock)
{
	bool is_saved = true;

	char dt[32], tm[32], o[32], h[32], l[32], c[32], v[32];
	sprintf(dt, "%.*s", sizeof(pBlock->date), pBlock->date);
	sprintf(tm, "%.*s", sizeof(pBlock->time), pBlock->time);
	sprintf(o, "%.*s", sizeof(pBlock->open), pBlock->open);
	sprintf(h, "%.*s", sizeof(pBlock->high), pBlock->high);
	sprintf(l, "%.*s", sizeof(pBlock->low), pBlock->low);
	sprintf(c, "%.*s", sizeof(pBlock->close), pBlock->close);
	sprintf(v, "%.*s", sizeof(pBlock->volume), pBlock->volume);


	//===== timediff 를 이용해서 한국 candle time 계산 =====//
	int diff_hour = std::stol(sTimeDiff) * (-1) * 60;

	char		candle_kor_ymd_hms	[64]{ 0 };
	CTimeUtils	time_util;	

	time_util.AddMins_((char*)dt, (char*)tm, diff_hour, candle_kor_ymd_hms); // yyyymmdd_hhmmss


	//===== trimming =====//
	CStringUtils u;
	strcpy(o, u.trim_all(o));
	strcpy(h, u.trim_all(h));
	strcpy(l, u.trim_all(l));
	strcpy(c, u.trim_all(c));
	strcpy(v, u.trim_all(v));

	__common.debug_fmt("\t<save_candle_data>(symbol:%s)(timeframe:%s)(dt:%s)[[TM:%s]](diff:%s)(tm_kor:%s)"
						"(o:%.8s)(h:%.8s)(l:%.8s)(c:%.8s)(v:%.8s)",
		sSymbol.c_str(), sTimeframe.c_str(), dt, tm, sTimeDiff.c_str(), candle_kor_ymd_hms,
		o, h, l, c, v)
		;
	
	//===== 현재시간보다 큰 candle 은 다음에 처리한다. (완료된 candle만 처리) =====//
	if(!is_finished_candle(candle_kor_ymd_hms))
		return (!is_saved);

	//===== 이후 작업을 위해 thread 에게 전달 =====//
	TAPIData data;
	data.set(sSymbol, sTimeframe, candle_kor_ymd_hms, o,h,l,c,v);
	if (!__common.m_saveQ.push(data))
	{
		__common.log(ERR, "Save Q is full. Failed to save API data");
		return (!is_saved);
	}

	return is_saved;
}


// 조회하는 시점보다 큰 캔들타임의 데이터는 처리하지 않는다.
bool CChartAPIView::is_finished_candle(char* candle_kor_ymd_hms)
{
	CTimeUtils util;
	string now_ymd_hms = util.sDateTime_yyyymmdd_hhmmss();

	int comp = now_ymd_hms.compare(candle_kor_ymd_hms);
	if (comp < 0) {
		//__common.log_fmt(INFO, 
		//				"CANDLE TIME 이 현재 시각보다 크다. 이건 사용하지 않는다.(현재:%s)(Candle Time:%s)", 
		//				now_ymd_hms.c_str(), candle_kor_ymd_hms);
		//return false;

		__common.log_fmt(INFO, 
						"CANDLE TIME 이 현재 시각보다 크다. 그래도 저장.(현재:%s)(Candle Time:%s)", 
						now_ymd_hms.c_str(), candle_kor_ymd_hms);

	}
	return true;
}


void CChartAPIView::threadFunc_save()
{
	CTimeUtils util;
	while (!m_thrdFlag.is_stopped())
	{
		_mm_pause();

		if (m_thrdFlag.is_idle()) continue;

		TAPIData data;
		if( !__common.m_saveQ.pop(data) )
			continue;

		//__common.debug_fmt("\t[POP TAPIData](symbol:%s)(timeframe:%s)(dt_kor:%s)(tm_kor:%s)(o:%.8s)(h:%.8s)(l:%.8s)(c:%.8s)(v:%.8s)",
		//					data.symbol.c_str(), data.timeframe.c_str(), 
		//					data.dt_kor.c_str(), data.tm_kor.c_str(),
		//					data.o.c_str(), data.h.c_str(), data.l.c_str(), data.c.c_str(), data.v.c_str());

		
		//=============== update candle time of vector ==============//
		
		__update_candle_basetm(data.timeframe, data.symbol, data.tm_kor_ymd_hms);


		//=============== broad cast to all clients ==============//
		string now_ydm_hms = util.sDateTime_yyyymmdd_hhmmss();
		if( now_ydm_hms.compare(data.tm_kor_ymd_hms)>0){
			string json_data = set_jsondata_for_client(data);			
			__iocpSvr.broadcast_all_clients(json_data);
			__common.debug_fmt("\t<broadcase>(%s)",json_data.c_str());
		}
		
		//=============== save into db ==============//
		bool ret = __dbworks.save_chartdata(data);
	}
}


std::string	CChartAPIView::set_jsondata_for_client(const TAPIData& api)
{
	nlohmann::json j;
	j["symbol"]		= api.symbol;
	j["timeframe"]	= api.timeframe;
	j["candle_tm"]	= api.tm_kor_ymd_hms.c_str();
	j["o"]			= api.o;
	j["h"]			= api.h;
	j["l"]			= api.l;
	j["c"]			= api.c;
	j["v"]			= api.v;
	return std::move(j.dump());
}

//----------------------------------------------------------------------------------------------------
// 조회 응답이 오지 않으면, 타임아웃 메시지를 수신받습니다
//----------------------------------------------------------------------------------------------------
LRESULT CChartAPIView::OnXMTimeoutData( WPARAM wParam, LPARAM lParam )
{
	__common.log(INFO, "[OnXMTimeoutData]수신");
    g_iXingAPI.ReleaseMessageData( ( int )lParam );

    return 1L;
}




//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회를 요청합니다
//----------------------------------------------------------------------------------------------------
void CChartAPIView::OnBtnQuery()
{
	UpdateData();

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


