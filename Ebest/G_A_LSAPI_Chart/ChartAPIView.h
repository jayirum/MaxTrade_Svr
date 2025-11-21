#pragma once
#include "../../Common/CTcpClient.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <string>
#include <deque>
#include <map>
#include "o3103.h"
#include "CGlobals.h"
#include "CDBWorks.h"
#include "AppCommon.h"
#include "../../Common/CSPSCRing.h"
#include "../../Common/TimeUtils.h"
#include "CParser.h"

using namespace std;

constexpr int MIN_QRY_CNT = 2;

typedef int REQ_ID;
struct TReqInfo
{
	std::string sSymbol;
	std::string sTimeframe;
	std::string sTimeDiff;
};


struct TSendReq {
	string	symbol;
	int		tf;
	int		read_cnt;

	TSendReq(string s, int t, int r):symbol{s}, tf{t}, read_cnt{r}{}
};

#define API_TIMEOUT_MS	1500
class CAPIRqstTimeout {
public:
	
	void	pause() 
	{
		if(m_last_sent.empty()) return;

		CTimeUtils u;
		string now;
		string next = u.AddMiliseconds(m_last_sent.c_str(), API_TIMEOUT_MS);
		__common.debug_fmt("(m_last_sent:%s)(next:%s)", m_last_sent.c_str(), next.c_str());
		while(true)
		{
			now = u.sDateTime_yyyymmdd_hhmmssmmm();
			if(now.compare(next)>0){
				break;
			}
			Sleep(1);
		}
		m_last_sent = now;
		///__common.debug_fmt("[m_last_sent](%s)", m_last_sent.c_str());
	}

	void	set_last(){ 
		CTimeUtils u; m_last_sent= u.sDateTime_yyyymmdd_hhmmssmmm();
		//__common.debug_fmt("[set_last](%s)", m_last_sent.c_str());
	}
private:
	
	string	m_last_sent;
};

// CChartAPIView 폼 뷰입니다.

class CChartAPIView : public CFormView
{
	DECLARE_DYNCREATE(CChartAPIView)

public:
    //JAY void			SetIndexName(int iInexType,  LPCTSTR pszName);

private:
	enum {CHART_INDEX_ };

	//------------------------------------------------------------
	// 표시하는 헤더 컬럼 수
    int				m_nColCount;
	
	//------------------------------------------------------------
	// 조회(또는 실시간) 수신 데이터의 마지막 표시 위치
	// 신규로 표시할지, 기존의 위치에 업데이트 할지를 판단할 수 있다
	int				m_nCurrentPos;

	//------------------------------------------------------------
	// 기타
	//JAY CStringArray	m_arrTRCode;	// TR코드 리스트
	std::deque<std::string>		m_arrTRCode;
	CStringArray	m_arrIndexID;	// 지표ID 리스트

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Jay's Codes

	//===== Initialize =====/
	//void	InitSymbolCombo();
	//void	InitTimeframeCombo();
	
	//===== api query 관련 =====/
	void	api_get_limitation_for_logging();
	void	first_api_qry();
	bool	send_api_request(const std::string& symbol, int timeframe, int read_cnt);
	bool	send_api_request_wrapper();
	void	requestID_add(int nReqId, const char* symbol, const char* timeframe);
	void	cb_request_apidata_on_timing(DataUnitPtr&);
	void	cb_recv_sise_handler(ns_tcpclient::RET_BOOL, ns_tcpclient::RECV_LEN, const ns_tcpclient::RECV_BUF&, const ns_tcpclient::MSG_BUF*);
	void	cb_print_tcpmsg(string& msg, bool err);
	
	//===== api 수신 및 이후 처리 관련 =====/
	void	thrdfunc_save();
	void	thrdfunc_sise_parser();
	bool	recv_apidata_proc(LPRECV_PACKET pPKData);	// 조회 결과 표시
	bool	save_candle_data(std::string& sSymbol, std::string& sTimeframe, std::string& sTimeDiff, o3103OutBlock1* pBlock);
	//std::string		set_jsondata_for_client(const TAPIData& api);

private:
	CDBConnector*	m_dbConnector;
	std::map<REQ_ID, std::shared_ptr<TReqInfo>>		m_mapReqNo;	//  
	std::mutex										m_mtxReqNo;

	__MAX::CThreadFlag				m_thrdFlag;
	std::thread						m_thrd_save;
	std::thread						m_thrd_sise_parser;
	CSPSCRing<DataUnitPtr>			m_ring_db{1};
	
	vector<shared_ptr<TSendReq>>	m_rqst_queue;
	std::mutex						m_mtx_rqst_queue;

	shared_ptr<ns_tcpclient::CTcpClient>	m_sise_client;
	ns_parser::CParser						m_sise_parser;

	CAPIRqstTimeout							m_api_req_timeout;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	CChartAPIView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CChartAPIView();

public:
	enum { IDD = IDD_INDEX11 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
    virtual void	OnInitialUpdate();
    afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void	OnDestroy();
    afx_msg void	OnSize(UINT nType, int cx, int cy);
    //afx_msg void	OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnBtnQuery();
    afx_msg	LRESULT	OnXMReceiveData(WPARAM wParam, LPARAM lParam);
    afx_msg	LRESULT	OnXMTimeoutData(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnAddLog(WPARAM wParam, LPARAM lParam);

	//void AppendLogMessage(const std::string message);

	CComboBox		m_cmbSymbols;
    CComboBox		m_cmbTimeframe;
    CEdit			m_edtCount;
	CListBox 		m_lstLog;
	CStatic			m_stMsg;
	std::string		m_sTrCode;

};
