#pragma once


#include "../Common/MaxTradeInc.h"
#include "../Common/CODBC.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <deque>
#include <memory>
#include <atomic>
using namespace std;


enum class MARKET_ACTION { MARKET_CLOSE, MARKET_OPEN, MARKET_NONE };
inline char* market_action_s(MARKET_ACTION a, char* out) { 
	strcpy(out, "MarketNone"); 
	if (a == MARKET_ACTION::MARKET_CLOSE) strcpy(out, "장 마감");
	else  strcpy(out, "장 오픈");
	return out;
}

struct TMarketFlag
{
	string			symbol;
	MARKET_ACTION	action;

	TMarketFlag(string	s, MARKET_ACTION a) :action(a), symbol(s) {}
};

class CMktOpenClose
{
public:
	CMktOpenClose();
	~CMktOpenClose();

	bool Initialize();


private:
	void threadFunc_Select();
	void threadFunc_Select_Internal();
	void dbProc_marketFlag();
	void threadFunc_CnclWorker();
	void threadFunc_CnclWorker_Internal();
	bool dbConnect(CODBC* p, string thrdName);
	bool reconnectDB(CODBC* p, string thrdName);

	void cancleOrders_set_start() { m_bStartCancel = true; }
	void cancelOrders_set_wait() { m_bStartCancel = false; }
	bool cancelOrders_is_time_to_do() { return m_bStartCancel; }

private:
	string						m_sConnStr;
	std::unique_ptr<CODBC>		m_odbcSel;

	std::thread					m_thrdSelect;
	std::thread					m_thrdCnclOrd;
	
	deque<unique_ptr<TMarketFlag>>	m_deqMktFlags;
	deque<string>					m_deqCancelOrders;
	bool							m_bStartCancel;

	std::mutex					m_mtxCnclOrders;

	long						m_dbReconnTry;
	__MAX::CThreadFlag			m_thrdFlag;

	unsigned int				m_timeout_ms_select;
	int							m_dbPing_timeout;
};



