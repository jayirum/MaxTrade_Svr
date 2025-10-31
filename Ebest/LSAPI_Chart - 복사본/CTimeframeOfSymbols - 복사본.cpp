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

bool CTimeframeOfSymbols::update_candle_tm(const char* symbol, const char* dt_kor, const char* tm_kor)
{
	auto it = m_map_symbols.find(symbol);
	if(it==m_map_symbols.end()){
		__common.debug_fmt("[UPDATE CANDLE TM-1.5]ERROR(TF:%d)(Symbol:%s)", my_timeframe(), symbol);
		return false;
	}

	char kor_tm[32]{};
	sprintf(kor_tm, "%.8s_%.4s", dt_kor, tm_kor);	//yyyymmdd_hhmm
	it->second->update_candle_tm(kor_tm);
	return true;
}

int	CTimeframeOfSymbols::check_time_to_apiqry_symbols(const char* now_tm_sec)	//yyyymmdd_hhmmss
{
	int num = 0;

	for (auto& [symbol, symbolclass] : m_map_symbols)
	{
		if(symbolclass->check_time_to_apiqry(now_tm_sec))
			num++;
	}

	return num;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// after receive api data (not from db)
// yyyymmdd_hhmm
void CSymbol::update_candle_tm(const char* candle_tm_kor)
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


bool CSymbol::check_time_to_apiqry(const char* now_tm_sec) //yyyymmdd_hhmmss
{
	// not initialized yet
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return false;
	}

	//
	calc_next_apiqry_tm();
	//

	char qry_tm[32];
	sprintf(qry_tm, "%s%.2s", m_next_candle_tm_kor, __common.apiqry_on_sec());	// 초를 02초로
	bool is_time = (strncmp(qry_tm, now_tm_sec, LEN_QRY_TM) == 0);
	if (is_time) {
		if (__common.is_debug1()) {
			__common.debug_fmt("[check_time_to_apiqry][FIRE](TF:%d)(%s)(status:%d)(now:%.*s)(fire:%s)"
				, m_tf, m_symbol.c_str(), (int)m_status, LEN_QRY_TM, now_tm_sec, qry_tm);
		}
		m_status = EN_STATUS::FIRED;
	}

	return is_time;
}


// 
void CSymbol::calc_next_candle_tm()
{
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return;
	}
	
	CTimeUtils util;
	char t[128]; 
	strcpy(t, util.DateTime_yyyymmddhhmmss());
	std::string now = t; 
	for(;;)
	{
		
		char dt[32], tm[32];
		sprintf(dt, "%.8s", m_last_candle_tm_kor);
		sprintf(tm, "%.6s", m_last_candle_tm_kor+8);

		char next_candle_tm[32]={0};
		util.AddMins(dt, tm, m_tf, next_candle_tm);	//yyyymmddhhmmss

		std::string candle_tm = next_candle_tm;

		long diff = util.diff_mins(
			candle_tm.substr(0,4),	//yyyy
			candle_tm.substr(4,2),	//mm
			candle_tm.substr(6,2),	//dd
			candle_tm.substr(8,2),	//hh
			candle_tm.substr(10,2),	//min
			now.substr(0, 4),	//yyyy
			now.substr(4, 2),	//mm
			now.substr(6, 2),	//dd
			now.substr(8, 2),	//hh
			now.substr(10, 2)	//min
			);

		if( diff<0 || (diff > m_tf) )
	}
	sprintf(m_next_candle_tm_kor, "%.8s_%.4", next_candle_tm, next_candle_tm + 8);
}

//void CSymbol::calc_next_apiqry_tm() 
//{
//	if (m_status != EN_STATUS::CANBE_FIRED) {
//		return;
//	}
//
//	CTimeUtils util;
//	char prev_qry_tm[32]; 
//	char dt[32], tm[32];
//	sprintf(dt, "%.8s", m_last_candle_tm_kor);
//	sprintf(tm, "%.6s", m_last_candle_tm_kor+8);
//
//	strcpy(prev_qry_tm, m_next_qry_tm);
//	util.AddMins(dt, tm, m_tf, m_next_qry_tm);
//
//	sprintf(m_next_qry_tm+12, "%.2s", __common.apiqry_on_sec());	// 초를 01 로 변경
//
//	if (__common.is_debug1()) {
//		if(strcmp(prev_qry_tm, m_next_qry_tm)!=0 )
//		{
//			__common.debug_fmt("\t<calc_next_qry_tm>(TF:%d)(%s)(status:%d)(prev:%s)(next:%s)"
//				, m_tf, m_symbol.c_str(), (int)m_status, prev_qry_tm, m_next_qry_tm);
//		}
//	}
//}




char* __get_next_qry_time(int tf, std::string& symbol, _Out_ bool& is_first)
{
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);
	auto it = __map_tfs_symbols.find(tf);
	if (it == __map_tfs_symbols.end())
	{
		__common.log_fmt(ERR, "[__get_next_qry_time] map 에 Timeframe데이터가 없음(timeframe:%d)", tf);
		return nullptr;
	}

	auto it2 = it->second->m_map_symbols.find(symbol);
	if (it2 == it->second->m_map_symbols.end())
	{
		__common.log_fmt(ERR, "[__get_next_qry_time] map 에 Symbol데이터가 없음(%s)", symbol.c_str());
		return nullptr;
	}

	is_first = it2->second->is_just_created();
	return it2->second->next_qry_tm();
}

bool __update_candle_tm(std::string& timeframe, std::string& symbol, std::string& dt_kor, std::string& tm_kor)
{
	long tf = std::stol(timeframe);
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);
	auto it = __map_tfs_symbols.find(tf);
	if (it == __map_tfs_symbols.end())
	{
		__common.log_fmt(ERR, "[threadFunc_save] map 에 데이터가 없음(timeframe:%d)", tf);
		return false;
	}
	it->second->update_candle_tm(symbol.c_str(), dt_kor.c_str(), tm_kor.c_str());

	return true;
}