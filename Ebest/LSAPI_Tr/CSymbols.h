#pragma once

#include "CDBConnector.h"
#include <string>
#include <deque>
#include <set>


//struct TTimeFrame
//{
//	int timeframe;
//	int inteval_min;
//	bool bShort;
//};

class CSymbols
{
public:
	CSymbols();
	~CSymbols();

	bool							Initialize(CDBConnector* pDB);
	const std::deque<std::string>&  get_symbol();
	int								get_timeframe(_Out_ std::deque<int>& deq_tf);

	const std::deque<int>& get_timeframe_often()	{ return m_deq_timefram_often; }
	const std::deque<int>& get_timeframe_seldom()	{ return m_deq_timefram_seldom; }
	
	bool load_symbols();
	bool load_timeframes();
	//int	 get_read_cnt() { return m_nReadCnt; }
	//int	 get_interval_short() { return m_nIntervalShort; }
	//int	 get_interval_long() { return m_nIntervalLong; }
private:
	CDBConnector* m_pDB;
	std::deque<std::string>		m_deq_symbol;
	std::deque<int>				m_deq_timefram_often;
	std::deque<int>				m_deq_timefram_seldom;

	//bool						m_oddmin_seldom;
	//std::string					m_apiquery_sec;
	//int							m_max_symbol_cnt;

	//std::deque<std::unique_ptr<TTimeFrame>>
	//std::deque<std::unique_ptr<TTimeFrame>>	m_deqTFShort, m_deqTFLong;
	//int										m_nReadCnt;
	//int										m_nIntervalLong, m_nIntervalShort;
	//int										m_nIntervalBase_Timeframe;
};

extern CSymbols	gSymbol;
