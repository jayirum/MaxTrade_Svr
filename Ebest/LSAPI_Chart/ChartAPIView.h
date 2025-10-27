#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <string>
#include <deque>
#include <map>
#include "CQueryTime.h"
#include "o3103.h"
#include "CGlobals.h"
#include "CDBWorks.h"


typedef int REQ_ID;
struct TReqInfo
{
	std::string sSymbol;
	std::string sTimeframe;
	std::string sTimeDiff;
};

//#define INTERVAL_FOR_EACH_QUERY	1500	//1.5SEC


class CCheckTime
{
public:
	CCheckTime()
	{
		m_last_exec[0] = 0;
		m_now[0] = 0;
	}

	~CCheckTime() {};

	bool	check_time(_Out_ bool& is_often_sec, _Out_ bool& is_seldom_min) 
	{
		//
		now_time();
		//

		if (strcmp(m_now, m_last_exec) == 0)
			return false;
	
		// compare hh:mm ==> end(05:00) ~ start(09:00)
		bool bigger_end		= (strncmp(__common.end_tm(), m_now, 5)	<=0);
		bool smaller_start	= (strncmp(m_now, __common.start_tm(),5)< 0);

		if ( bigger_end && smaller_start )
			return false;

		is_often_sec = 0;	//TODO (strncmp(&m_now[6], __common.apiqry_often_sec(), 2) == 0);	// 12:07:01 에서 01초

		char z[32]; sprintf(z, "%.02s", &m_now[3]);	// 12:07:02 에서 07분
		int n = atoi(z);
		//if (__common.is_seldom_min_odd())
		//	is_seldom_min = (n % 2 != 0);
		//else
		//	is_seldom_min = (n % 2 == 0);

		return true;
	}

	void set_exec_time()
	{
		strcpy(m_last_exec, m_now);
	}

private:
	void	now_time() { GetLocalTime(&m_st); sprintf(m_now, "%02d:%02d:%02d", m_st.wHour, m_st.wMinute, m_st.wSecond); }
	
private:
	//char	m_start_tm[32];
	//char	m_end_tm[32];
	//char	m_apiqry_often_sec[32];
	//bool	m_is_seldom_min_odd;

	char	m_last_exec[8+1];	// hh:mm:ss

	SYSTEMTIME	m_st;
	char		m_now[8+1];			// hh:mm:ss
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

	bool	recv_apidata_proc(LPRECV_PACKET pPKData);	// 조회 결과 표시
	bool	save_candle_data(std::string sSymbol, std::string sTimeframe, std::string sTimeDiff, o3103OutBlock1* pBlock, bool b_update_candle_tm);

	void	InitSymbolCombo();
	void	InitTimeframeCombo();
	
	void	api_get_limitation_for_logging();
	
	void	threadFunc_Query();
	void	first_api_qry();
	int		check_qrytime_all();
	void	fetch_candles_apidata();
	bool	fetch_apidata(std::string symbol, int timeframe, bool is_first);
	void	requestID_add(int nReqId, const char* symbol, const char* timeframe);
	

private:
	CDBConnector* m_dbConnector;

	//
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

	std::map<REQ_ID, std::shared_ptr<TReqInfo>>		m_mapReqNo;	//  
	std::mutex										m_mtxReqNo;
	
	CQueryTime				m_tm;
	std::thread				m_thrdQuery;
	__MAX::CThreadFlag		m_thrdFlag;
	//char 					m_tm_start[6], m_tm_end[6];
	
	CCheckTime				m_check_tm;
};
