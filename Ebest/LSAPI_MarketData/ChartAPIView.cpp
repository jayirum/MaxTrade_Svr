

#include "stdafx.h"
#include "AppCommon.h"
#include "LSAPIApp.h"
#include "ChartAPIView.h"
#include "CGlobals.h"
#include <iostream>
#include <chrono>
#include "CDBWorks.h"
#include "../../Common/StringUtils.h"
#include "CIOCPServer.h"
#include <json.hpp>
#include <algorithm>
#include "KF_Hoga_FH0.h"
#include "KF_Sise_FC0.h"
#include "OF_Hoga_OVH.h"
#include "OF_Sise_OVC.h"

#define COMSTR(st) CString(st, sizeof(st))


// CChartAPIView
IMPLEMENT_DYNCREATE(CChartAPIView, CFormView)

CChartAPIView::CChartAPIView()
	: CFormView(CChartAPIView::IDD)
{
	m_nColCount		= 0;
	m_nCurrentPos	= 0;
}

CChartAPIView::~CChartAPIView()
{
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
	ON_MESSAGE(WM_USER + XM_RECEIVE_REAL_DATA,			OnXMReceiveRealData)
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
    CFormView::OnInitialUpdate();

	// 시장구분 콤보 초기화
	InitSymbolCombo();
	
	// market data 패킷 구성 및 broadcasting
	m_md_handler.Initialize();


	std::thread	thrd_api_rqst([this]() {
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		advise_apidata_all(EN_DATA_TP::SISE);
		
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		advise_apidata_all(EN_DATA_TP::HOGA);
		return;
	});

	thrd_api_rqst.detach();
	
	UpdateData(FALSE);

}


void CChartAPIView::InitSymbolCombo()
{
	m_cmbSymbols.ResetContent();
		
	for (auto& s : __common.symbols) {
		m_cmbTimeframe.AddString(s.c_str());
	}

	m_cmbSymbols.SetCurSel( 0 );
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


void CChartAPIView::advise_apidata_all(EN_DATA_TP tp)
{
	for (const auto& symbol : __common.symbols)
	{
		//unadvise_apidata(tp, symbol);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		advise_apidata(tp, symbol);
	}
}

bool CChartAPIView::advise_apidata(EN_DATA_TP tp, const string& symbol)
{
	char zTrCode[32] = { 0 };
	char zMsg[128] = { 0 };
	int nSize{0};

	if (tp==EN_DATA_TP::SISE)
	{
		strcpy(zTrCode, DEF_TR_SISE);
#ifdef __DEF_OVERSEAS
		nSize = sizeof(OVC_InBlock);
#else 
		nSize = sizeof(FC0_InBlock);
#endif
		strcpy(zMsg, "시세");
	}
	else
	{
		strcpy(zTrCode, DEF_TR_HOGA);
#ifdef __DEF_OVERSEAS
		nSize = sizeof(OVH_InBlock);
#else
		nSize = sizeof(FH0_InBlock);
#endif
		strcpy(zMsg, "호가");
	}

	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.AdviseRealData(
		GetSafeHwnd(),	// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		zTrCode,		// TR 번호
		symbol.c_str(),	// 종목코드
		nSize			// 종목코드 길이
	);

	//-----------------------------------------------------------
	// 에러체크
	if (bSuccess == FALSE)
	{
		__common.log_fmt(ERR, "[시세요청 오류][symbol:%s][%s] (g_iXingAPI.AdviseRealData)", symbol.c_str(), zMsg);
		return false;
	}

	__common.log_fmt(INFO, "[시세요청 성공][TR:%s][symbol:%s][Size:%d]", zTrCode, symbol.c_str(), nSize);
	return true;
}



LRESULT CChartAPIView::OnXMReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	TDataUnit* msg = __common.mempool->get<TDataUnit>();
	memcpy(msg->data, pRealPacket->pszData, pRealPacket->nDataLength);
	msg->data_size = pRealPacket->nDataLength;

	if (strcmp(pRealPacket->szTrCode, DEF_TR_SISE) == 0)
	{
		m_md_handler.push_sise(msg);
	}
	if (strcmp(pRealPacket->szTrCode, DEF_TR_HOGA) == 0)
	{
		m_md_handler.push_hoga(msg);
	}


	return 0L;
}


void CChartAPIView::unadvise_apidata_all(EN_DATA_TP tp)
{
	for (const auto& symbol : __common.symbols)
	{
		unadvise_apidata(tp, symbol);
	}
}

bool CChartAPIView::unadvise_apidata(EN_DATA_TP tp, const string& symbol)
{

	char zTrCode[32] = { 0 };
	char zMsg[128] = { 0 };
	int nSize{0};

	if (tp == EN_DATA_TP::SISE)
	{
		strcpy(zTrCode, DEF_TR_SISE);
#ifdef __DEF_OVERSEAS
		nSize = sizeof(OVC_OutBlock);
#else
		nSize = sizeof(FC0_OutBlock);
#endif
		strcpy(zMsg, "시세");
	}
	else
	{
		strcpy(zTrCode, DEF_TR_HOGA);
#ifdef __DEF_OVERSEAS
		nSize = sizeof(OVH_OutBlock);
#else 
		nSize = sizeof(FH0_OutBlock);
#endif
		strcpy(zMsg, "호가");
	}

	//-----------------------------------------------------------
	// 데이터 전송
	BOOL bSuccess = g_iXingAPI.UnadviseRealData(
		GetSafeHwnd(),	// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
		zTrCode,		// TR 번호
		symbol.c_str(),	// 종목코드
		nSize			// 종목코드 길이
	);

	//-----------------------------------------------------------
	// 에러체크
	if (bSuccess == FALSE)
	{
		__common.log_fmt(ERR, "[시세취소 오류][symbol:%s][%s] (g_iXingAPI.UnadviseRealData)", symbol.c_str(), zMsg);
		return false;
	}

	__common.log_fmt(ERR, "[시세취소 성공][symbol:%s][%s] (g_iXingAPI.UnadviseRealData)", symbol.c_str(), zMsg);
	return true;
}
//std::string	CChartAPIView::set_jsondata_for_client(const TAPIData& api)
//{
//	nlohmann::json j;
//	j["symbol"]		= api.symbol;
//	j["timeframe"]	= api.timeframe;
//	j["candle_tm"]	= api.tm_kor_ymd_hms.c_str();
//	j["o"]			= api.o;
//	j["h"]			= api.h;
//	j["l"]			= api.l;
//	j["c"]			= api.c;
//	j["v"]			= api.v;
//	return std::move(j.dump());
//}




//----------------------------------------------------------------------------------------------------
// 차트 지표데이터 조회를 요청합니다
//----------------------------------------------------------------------------------------------------
void CChartAPIView::OnBtnQuery()
{
	UpdateData();

}
