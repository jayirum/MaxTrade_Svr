#include "CTimeframeOfSymbols.h"
#include "CGlobals.h"

//==================== �������� ���� ====================//
std::mutex											__mtx_tfs_symbols;
std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;


//======================================================================//
//==================== CTimeframeOfSymbols ����Լ��� ====================//
//======================================================================//
CTimeframeOfSymbols::CTimeframeOfSymbols(int timeframe)
{
	m_timeframe = timeframe;
}

void CTimeframeOfSymbols::set_symbol(std::string symbol)
{
	m_map_symbols[symbol] = std::make_unique< CSymbol>(symbol, m_timeframe);
}


//===== ù ��ȸ�� ���� candle time �� base time �� ������Ʈ. ���� 1ȸ�� �Ѵ�.
bool CTimeframeOfSymbols::update_first_candle_tm(std::string& symbol, std::string& dt_kor, std::string& tm_kor_hms)
{
	auto it = m_map_symbols.find(symbol);
	if(it==m_map_symbols.end())
	{
		__common.log_fmt(ERR, "[update_first_candle_tm]ERROR(TF:%d)(Symbol:%s)", my_timeframe(), symbol.c_str());
		return false;
	}

	//===== �ش� ������ candle time ������Ʈ =====//
	it->second->update_first_candle_tm(dt_kor, tm_kor_hms);


	//===== �� ���� ��� ������Ʈ �Ǿ����� ���� =====//
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


//=====	��� symbol �鿡 ���� api ������ ��û�� �ð����� üũ
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
//==================== CSymbol ����Լ��� ====================//
//======================================================================//



//===== ù ��ȸ�� ���� candle time �� base time �� ������Ʈ. ���� 1ȸ�� �Ѵ�.
void CSymbol::update_first_candle_tm(const std::string& dt_kor, const std::string& tm_kor_hms)
{
	if( m_status!=EN_STATUS::CREATED)
		return ;

	//===== ���� �ð� =====//
	char now_ymd_hms[32]{0};
	SYSTEMTIME st; GetLocalTime(&st); 
	sprintf(now_ymd_hms, "%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//===== api �� ���� candle time	=====//
	char candle_ymd_hms[64];
	sprintf(candle_ymd_hms, "%s_%s", dt_kor.c_str(), tm_kor_hms.c_str());	//yyyymmdd_hhmmss


	//===== api �� ���� candle time ���� first_candle_time �� update	=====//
	// �ٸ�, ���� candle �� ������ �������� �� �����Ƿ�, ���� �ð��� �ʰ����� �ʴ� �ֱ� candle time �� ����ؼ� ����

	char curr_ymd_hms[32]{}; 
	char next_ymd_hms[32]{};
	char temp_ymd_hms[32]{};

	strcpy(curr_ymd_hms, candle_ymd_hms);

	for (;;)
	{
		// ���� candle time �� ����ð� ���� ũ�� ���� candle time �� �������� �Ѵ�.
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
		
		//===== ���� candle time �� ���̽��� ���� candle time ��� �� ����
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




//==========	�ش� �ɺ��� api ������ ��û�� �ð����� üũ ==========//
bool CSymbol::check_time_to_apiqry(const char* now_ymd_hms) //yyyymmdd_hhmmss
{
	// not initialized yet
	if (m_status != EN_STATUS::CANBE_FIRED) {
		return false;
	}

	//===== ���� ��ȸ�� �ð��� ����Ѵ�. =====//
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

//===== ���� ��ȸ�� �ð��� ����Ѵ�. =====//
bool CSymbol::calc_next_apiqry_tm(const char* now_ymd_hms)
{
	bool is_updated = true;

	if (m_status != EN_STATUS::CANBE_FIRED) {
		return (!is_updated);
	}

	// next qry time �� ���纸�� ������ ����, �ƴϸ� ���д�.
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

	sprintf(m_next_qry_ymd_hms, "%.8s_%.4s%.2s", temp_ymd_hms, temp_ymd_hms +8, __common.apiqry_on_sec());	// �ʸ� 02 �� ����

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
		__common.log_fmt(ERR, "[threadFunc_save] map �� �����Ͱ� ����(timeframe:%d)", tf);
		return false;
	}

	// �̹� ��� symbol �� ���� ù candle_tm �� ������ �� ��� �ٽ� �� �ʿ� ����.
	if( !it->second->is_first_updated_all() ){
		it->second->update_first_candle_tm(symbol, dt_kor, tm_kor_hms);
	}

	return true;
}