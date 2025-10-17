#pragma once

#include "../Common/MaxTradeInc.h"
#include <atomic>
#include <map>
#include <string>
#include <memory>
#include "../Common/Util.h"
#include "../Common/TimeUtils.h"
#include "CGlobals.h"
#include "CLimitOrderList.h"
#include "CMarketOrderList.h"
#include "CMarketData.h"

using namespace std;

struct TLastTime
{
	string quote, tick;
	string marketBuy, marketSell;
	string limitBuy, limitSell;

	void updateLastTime(_In_ string& tm) {
		CTimeUtils t; tm = t.Time_hhmmssmmm();
	}
};

class CMatchProcessBySymbol
{
public:
	CMatchProcessBySymbol(string symbol, int dotCnt);
	~CMatchProcessBySymbol();

private:
	void thrdFunc_matchproc_limitOrders(__MAX::BSTP_IDX idx_bstp);
	void thrdFunc_matchproc_marketorders(__MAX::BSTP_IDX idx_bstp);


	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator
	processMatch_limitOrder_by_priceCondition(
		__MAX::BSTP_IDX idx_bstp, double dBestQuotePrc,
		map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator& itPrcMap,
		CLimitOrderList* pList,
		TQuote* pQuote
	);

	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator
		processMatch_limitOrder_by_matchRate(
			__MAX::BSTP_IDX idx_bstp, double dBestQuotePrc, double dBestQuoteVol,
			map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator& itPrcMap,
			CLimitOrderList* pList,
			TQuote* pQuote
		);

	//void compose_match_limitOrder(_In_ string cntrPrc, _In_ __MAX::TData* pTC001, _Out_ __MAX::TData* pTA001);
	void compose_returnPacket_of_match(_In_ const string& cntrPrc, _In_ string& sTC001, _Out_ __MAX::TData* pTA001);
	void compose_rejectPacket(_In_ const string& sTC001, string& rjctCode, string& rjct_msg, _Out_ __MAX::TData* pTA001);

private:
	string		m_symbol;
	int			m_nDotCnt;

	CLimitOrderList*	m_limitList;
	CMarketOrderList*	m_marketList;

	thread		m_thrdLimit[2];
	thread		m_thrdMarket[2];
	__MAX::CThreadFlag	m_thrdFlag;

	TLastTime	m_lastTime;;
	double		m_match_rt;
};

class CMatchProcessWrapper
{
public:
	bool create_matchprocesses();

private:
	map<string, unique_ptr<CMatchProcess>>	m_mapMatches;
};
