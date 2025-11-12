#include "CTimeframeOfSymbols.h"
#include "CGlobals.h"

//==================== 전역변수 선언 ====================//
std::mutex											__mtx_tfs_symbols;
std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;


//======================================================================//
//==================== CTimeframeOfSymbols 멤버함수들 ====================//
//======================================================================//
CTimeframeOfSymbols::CTimeframeOfSymbols(int timeframe)
{
	m_timeframe = timeframe;
}

void CTimeframeOfSymbols::set_symbol(std::string symbol)
{
	m_map_symbols[symbol] = std::make_unique< CSymbol>(symbol, m_timeframe);
}


//===== 첫 조회를 통해 candle time 의 base time 을 업데이트. 최초 1회만 한다.
bool CTimeframeOfSymbols::update_first_candle_tm(std::string& symbol, std::string& dt_kor, std::string& tm_kor_hms)
{
	auto it = m_map_symbols.find(symbol);
	if(it==m_map_symbols.end())
	{
		__common.log_fmt(ERR, "[update_first_candle_tm]ERROR(TF:%d)(Symbol:%s)", my_timeframe(), symbol.c_str());
		return false;
	}

	//===== 해당 종목의 candle time 업데이트 =====//
	it->second->update_first_candle_tm(dt_kor, tm_kor_hms);


	//===== 전 종목 모두 업데이트 되었는지 점검 =====//
	bool is_updated_all = true;
	for (auto& [symbol, symbolclass] : m_map_symbols)
	{
		if (symbolclass->curr_status() == EN_STATUS::CREATED)
		{
			is_updated_all = false;
			break;
		}
	}
	m_is_all_first_updated = is_updated_all;

	return true;
}


//=====	모든 symbol 들에 대해 api 쿼리를 요청할 시간인지 체크
int	CTimeframeOfSymbols::check_time_to_apiqry_symbols(const char* now_ymd_hms)	//yyyymmdd_hhmmss
{
	int num = 0;

	for (auto& [symbol, symbolclass] : m_map_symbols)
	{
		if(symbolclass->check_time_to_apiqry(now_ymd_hms))
			num++;
	}

	return num;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


//======================================================================//
//==================== CSymbol 멤버함수들 ====================//
//======================================================================//



//===== 첫 조회를 통해 candle time 의 base time 을 업데이트. 최초 1회만 한다.
void CSymbol::update_first_candle_tm(const std::string& dt_kor, const std::string& tm_kor_hms)
{
	if( m_status!=EN_STATUS::CREATED)
		return ;

	//===== 현재 시각 =====//
	char now_ymd_hms[32]{0};
	SYSTEMTIME st; GetLocalTime(&st); 
	sprintf(now_ymd_hms, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//===== api 로 받은 candle time	=====//
	char candle_ymd_hms[64];
	sprintf(candle_ymd_hms, "%s_%s", dt_kor.c_str(), tm_kor_hms.c_str());	//yyyymmdd_hhmmss


	//===== api 로 받은 candle time 으로 first_candle_time 을 update	=====//
	// 다만, 받은 candle 이 오래된 데이터일 수 있으므로, 현재 시각을 초과하지 않는 최근 candle time 을 계산해서 저장

	char curr_ymd_hms[32]{}; 
	char next_ymd_hms[32]{};
	char temp_ymd_hms[32]{};

	strcpy(curr_ymd_hms, candle_ymd_hms);

	for (;;)
	{
		// 다음 candle time 이 현재시간 보다 크면 현재 candle time 을 기준으로 한다.
		if (strlen(next_ymd_hms) > 0)
		{
			if (strcmp(next_ymd_hms, now_ymd_hms) > 0) 
			{
				strcpy(m_first_candle_ymd_hms, curr_ymd_hms);
				break;
			}
			
			// update curr_candle
			strcpy(curr_ymd_hms, next_ymd_hms);
		}

		CTimeUtils util;
		char dt[32]{}, tm[32]{};
		ZeroMemory(temp_ymd_hms,sizeof(temp_ymd_hms));

		sprintf(dt, "%.8s", curr_ymd_hms);
		sprintf(tm, "%.4s", curr_ymd_hms + 8);
		
		//===== 현재 candle time 을 베이스로 다음 candle time 계산 및 저장
		util.AddMins(dt, tm, m_tf, temp_ymd_hms);
		sprintf(next_ymd_hms, "%.8s_%.6s", temp_ymd_hms, temp_ymd_hms +8);

		__common.debug_fmt("\t<update_first_candle_tm> (TF:%d)(%s)(curr candle:%s)(next candle:%s)(now:%s)",
			m_tf, m_symbol.c_str(), curr_ymd_hms, next_ymd_hms, now_ymd_hms);
	}

	//======= update last time and status =============/
	strcpy(m_last_qry_ymd_hms, m_first_candle_ymd_hms);
	sprintf(m_last_qry_ymd_hms+13, "%.2s", __common.apiqry_on_sec()); //yyyymmdd_hhmm02 

	m_status = EN_STATUS::CANBE_FIRED;

	__common.debug_fmt("\t<update last qry tm> (TF:%d)(%s)(first candle:%s)(last qry:%s)",
		m_tf, m_symbol.c_str(), m_first_candle_ymd_hms, m_last_qry_ymd_hms);

}




//==========	해당 심볼의 api 쿼리를 요청할 시간인지 체크 ==========//
bool CSymbol::check_time_to_apiqry(const char* now_ymd_hms) //yyyymmdd_hhmmss
{
	// not initialized yet
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return false;
	}

	//===== 다음 조회할 시간을 계산한다. =====//
	bool is_updated = calc_next_apiqry_tm(now_ymd_hms);
	//

	bool is_time = (strncmp(m_next_qry_ymd_hms, now_ymd_hms, LEN_TM) == 0);
	if (is_time) {
		if (__common.is_debug1()) {
			__common.debug_fmt("[check_time_to_apiqry][FIRE](TF:%d)(%s)(status:%d)(now:%.*s)(fire:%s)"
				, m_tf, m_symbol.c_str(), (int)m_status, LEN_TM, now_ymd_hms, m_next_qry_ymd_hms);
		}
		m_status = EN_STATUS::FIRED;
	}

	return is_time;
}

//===== 다음 조회할 시간을 계산한다. =====//
bool CSymbol::calc_next_apiqry_tm(const char* now_ymd_hms)
{
	bool is_updated = true;

	if (m_status != EN_STATUS::CANBE_FIRED) {
		return (!is_updated);
	}

	// next qry time 이 현재보다 작으면 재계산, 아니면 놔둔다.
	if (strncmp(m_next_qry_ymd_hms, now_ymd_hms, LEN_TM) >= 0) {
		return (!is_updated);
	}

	// switch
	strcpy(m_last_qry_ymd_hms, m_next_qry_ymd_hms);

	CTimeUtils util;
	char dt[32]{}, tm[32]{}, temp_ymd_hms[64]{};
	sprintf(dt, "%.8s", m_last_qry_ymd_hms);
	sprintf(tm, "%.6s", m_last_qry_ymd_hms + 8);

	util.AddMins(dt, tm, m_tf, temp_ymd_hms);

	sprintf(m_next_qry_ymd_hms, "%.8s_%.4s%.2s", temp_ymd_hms, temp_ymd_hms +8, __common.apiqry_on_sec());	// 초를 02 로 변경

	__common.debug_fmt("\t<calc_next_qry_tm>(TF:%d)(%s)(status:%d)(last:%s)(next:%s)"
				, m_tf, m_symbol.c_str(), (int)m_status, m_last_qry_ymd_hms, m_next_qry_ymd_hms);

	return is_updated;
}


void	CSymbol::reset_status() 
{
	m_status = EN_STATUS::CANBE_FIRED;
	__common.debug_fmt("[CSymbol][timeframe:%d][%s] reset status", m_tf, m_symbol.c_str());
}


bool __update_first_candle_tm(std::string& timeframe, std::string& symbol, std::string& dt_kor, std::string& tm_kor_hms)
{
	long tf = std::stol(timeframe);
	std::lock_guard<std::mutex> lock(__mtx_tfs_symbols);
	auto it = __map_tfs_symbols.find(tf);
	if (it == __map_tfs_symbols.end())
	{
		__common.log_fmt(ERR, "[threadFunc_save] map 에 데이터가 없음(timeframe:%d)", tf);
		return false;
	}

	// 이미 모든 symbol 에 대해 첫 candle_tm 이 저장이 된 경우 다시 할 필요 없다.
	if( !it->second->is_first_updated_all() ){
		it->second->update_first_candle_tm(symbol, dt_kor, tm_kor_hms);
	}

	return true;
}