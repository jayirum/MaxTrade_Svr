#include "CTimeframeOfSymbols.h"
#include "CGlobals.h"

std::mutex											__mtx_tfs_symbols;
std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;

CTimeframeOfSymbols::CTimeframeOfSymbols(int timeframe)
{
	m_timeframe = timeframe;
}

void CTimeframeOfSymbols::set_symbol(std::string symbol)
{
	m_map_symbols[symbol] = std::make_unique< CSymbol>(symbol, m_timeframe);
}

bool CTimeframeOfSymbols::update_candle_tm(const char* symbol, const char* dt, const char* tm, const char* timediff)
{
	auto it = m_map_symbols.find(symbol);
	if(it==m_map_symbols.end()){
		__common.debug_fmt("[UPDATE CANDLE TM-1.5]ERROR(TF:%d)(Symbol:%s)", my_timeframe(), symbol);
		return false;
	}

	char kor_tm[32]{};

	CTimeUtils time_util;

	int diff_hour = atoi(timediff)*(-1) * 60;

	time_util.AddMins((char*)dt, (char*)tm, diff_hour, kor_tm);
	it->second->update_candle_tm(kor_tm);
	return true;
}

int	CTimeframeOfSymbols::check_time_to_apiqry_symbols(const char* now_tm)	//yyyymmddhhmmss
{
	int num = 0;

	for (auto& [symbol, symbolclass] : m_map_symbols)
	{
		if(symbolclass->check_time_to_apiqry(now_tm))
			num++;
	}

	return num;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// after receive api data (not from db)
void CSymbol::update_candle_tm(const char* candle_tm_kor)			//yyyymmddhhmmss
{
	char prev[32]; strcpy(prev, m_last_candle_tm_kor);

	// candle 이 생성되지 않은 경우 => 한 단계 증가시킨다.
	if (strcmp(m_last_candle_tm_kor, candle_tm_kor) == 0)
	{
		CTimeUtils util;
		char dt[32], tm[32];
		sprintf(dt, "%.8s", m_last_candle_tm_kor);
		sprintf(tm, "%.6s", m_last_candle_tm_kor + 8);
		util.AddMins(dt, tm, m_tf, m_next_qry_tm);
		__common.debug_fmt("\t<Update Candle Time-1> (TF:%d)(%s)(prev:%s)(new:%s)(calc:%s)", 
			m_tf, m_symbol.c_str(), prev, candle_tm_kor, m_last_candle_tm_kor);
	}
	else {
		strcpy(m_last_candle_tm_kor, candle_tm_kor);
		m_status = EN_STATUS::CANBE_FIRED;

		__common.debug_fmt("\t<Update Candle Time-2> (TF:%d)(%s)(prev:%s)(new:%s)", m_tf, m_symbol.c_str(), prev, m_last_candle_tm_kor);
	}
}


bool CSymbol::check_time_to_apiqry(const char* now_tm) //yyyymmddhhmmss
{
	// not initialized yet
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return false;
	}

	//
	calc_next_apiqry_tm();
	//

	bool is_time = (strcmp(m_next_qry_tm, now_tm) == 0);
	if (is_time) {
		if (__common.is_debug1()) {
			__common.debug_fmt("[check_time_to_apiqry][FIRE](TF:%d)(%s)(status:%d)(now:%s)(next:%s)"
				, m_tf, m_symbol.c_str(), (int)m_status, now_tm, m_next_qry_tm);
		}
		m_status = EN_STATUS::FIRED;
	}

	return is_time;
}

void CSymbol::calc_next_apiqry_tm() 
{
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return;
	}

	CTimeUtils util;
	char prev_qry_tm[32]; 
	char dt[32], tm[32];
	sprintf(dt, "%.8s", m_last_candle_tm_kor);
	sprintf(tm, "%.6s", m_last_candle_tm_kor+8);

	strcpy(prev_qry_tm, m_next_qry_tm);
	util.AddMins(dt, tm, m_tf, m_next_qry_tm);

	sprintf(m_next_qry_tm+12, "%.2s", __common.apiqry_on_sec());	// 초를 01 로 변경

	if (__common.is_debug1()) {
		if(strcmp(prev_qry_tm, m_next_qry_tm)!=0 )
		{
			__common.debug_fmt("\t<calc_next_qry_tm>(TF:%d)(%s)(status:%d)(prev:%s)(next:%s)"
				, m_tf, m_symbol.c_str(), (int)m_status, prev_qry_tm, m_next_qry_tm);
		}
	}
}
