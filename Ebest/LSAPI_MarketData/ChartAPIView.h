#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <string>
#include <deque>
#include <map>
#include "CGlobals.h"
#include "CDBWorks.h"
#include "CMarketDataHandler.h"

constexpr int MIN_QRY_CNT = 2;

enum class EN_DATA_TP { SISE, HOGA };

typedef int REQ_ID;
struct TReqInfo
{
	std::string sSymbol;
	std::string sTimeframe;
	std::string sTimeDiff;
};

//#define INTERVAL_FOR_EACH_QUERY	1500	//1.5SEC



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
	//std::deque<std::string>		m_arrTRCode;
	//CStringArray	m_arrIndexID;	// 지표ID 리스트

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Jay's Codes

	//===== Initialize =====/
	void	InitSymbolCombo();
	
		
	//===== api 수신 및 이후 처리 관련 =====/
	void	advise_apidata_all(EN_DATA_TP tp);
	bool	advise_apidata(EN_DATA_TP tp, const string& symbol);
	
	void	unadvise_apidata_all(EN_DATA_TP tp);
	bool	unadvise_apidata(EN_DATA_TP tp, const string& symbol);

	//std::string		set_jsondata_for_client(const TAPIData& api);

private:
	CDBConnector*	m_dbConnector;
	std::map<REQ_ID, std::shared_ptr<TReqInfo>>		m_mapReqNo;	//  
	std::mutex										m_mtxReqNo;

	CMarketDataHandler								m_md_handler;

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
//	DECLARE_MESSAGE_MAP()
public:
    virtual void	OnInitialUpdate();
    afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void	OnDestroy();
    afx_msg void	OnSize(UINT nType, int cx, int cy);
    //afx_msg void	OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnBtnQuery();
	afx_msg	LRESULT	OnXMReceiveRealData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//void AppendLogMessage(const std::string message);

	CComboBox		m_cmbSymbols;
    CComboBox		m_cmbTimeframe;
    CEdit			m_edtCount;
	CListBox 		m_lstLog;
	CStatic			m_stMsg;
	std::string		m_sTrCode;

};
