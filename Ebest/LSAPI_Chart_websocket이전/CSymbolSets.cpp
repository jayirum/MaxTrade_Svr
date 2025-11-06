#include "CSymbolSets.h"


vector<SymbolInfoPtr>	__SymbolSets;
bool					__is_basetm_updated = false;


void CSymbolSets::update_base_candletm(string& tm_kor_ymd_hms)
{
	if (m_status != EN_STATUS::CREATED){
		__common.debug_fmt("\t<update_base_candletm> 이미 update 했다.(TF:%d)(%s)(base tm:%s)(last qry:%s)(수신:%s)",
						m_timeframe,
						m_symbol.c_str(),
						m_base_candletm_ydm_hms,
						m_last_qrytm_ydm_hms,
						tm_kor_ymd_hms.c_str()
						);
		return;
	}

	//===== 현재 시각 =====//
	CTimeUtils util;
	string now_ymd_hms = util.sDateTime_yyyymmdd_hhmmss();

	//===== api 로 받은 candle time	=====//
	char candle_ymd_hms[64];
	sprintf(candle_ymd_hms, tm_kor_ymd_hms.c_str());


	//===== api 로 받은 candle time 으로 candle base time 을 update	=====//
	// 다만, 받은 candle 이 오래된 데이터일 수 있으므로, 현재 시각을 초과하지 않는 최근 candle time 을 계산해서 저장

	char curr_ymd_hms[32]{};
	char next_ymd_hms[32]{};

	strcpy(curr_ymd_hms, candle_ymd_hms);

	for (;;)
	{
		// next candle time 이 현재시간 보다 크면 curr candle time 을 기준으로 한다.
		if (strlen(next_ymd_hms) > 0)
		{
			if(now_ymd_hms.compare(next_ymd_hms) < 0 )
			{
				strcpy(m_base_candletm_ydm_hms, curr_ymd_hms);
				break;
			}

			// update curr_candle
			strcpy(curr_ymd_hms, next_ymd_hms);
		}

		CTimeUtils util;
		char dt[32]{}, tm[32]{};

		sprintf(dt, "%.8s", curr_ymd_hms);
		sprintf(tm, "%.6s", curr_ymd_hms + 9);	//yyyymmdd_hhmmss

		//===== 현재 candle time 을 베이스로 다음 candle time 계산 및 저장
		util.AddMins_(dt, tm, m_timeframe, next_ymd_hms);

		_mm_pause();

		//__common.debug_fmt("\t<update_base_candletm-계산중> (TF:%d)(%s)(curr candle:%s)(next candle:%s)(now:%s)",
		//					m_timeframe, 
		//					m_symbol.c_str(), 
		//					curr_ymd_hms, 
		//					next_ymd_hms, 
		//					now_ymd_hms.c_str()
		//					);
	}

	//======= update last_time, next_time and status =============/
	strcpy(m_last_qrytm_ydm_hms, m_base_candletm_ydm_hms);
	sprintf(m_last_qrytm_ydm_hms + 13, "%.2s", __common.apiqry_on_sec()); //yyyymmdd_hhmm02 
	
	strcpy(m_next_qrytm_ydm_hms, next_ymd_hms);
	sprintf(m_next_qrytm_ydm_hms + 13, "%.2s", __common.apiqry_on_sec()); //yyyymmdd_hhmm02 

	m_status = EN_STATUS::CANBE_FIRED;

	__common.debug_fmt("\t<update_base_candletm> (TF:%d)(%s)(base candle tm:%s)(last qry tm:%s)(next qry tm:%s)",
		m_timeframe, m_symbol.c_str(), m_base_candletm_ydm_hms, m_last_qrytm_ydm_hms, m_next_qrytm_ydm_hms);
}


bool CSymbolSets::is_time_api_qry(string& now_ymd_hms)
{
	if(m_status==EN_STATUS::CREATED)
		return false;

	//===== 현재시각이 더 크면 오류 =====//
	if (now_ymd_hms.compare(m_next_qrytm_ydm_hms) > 0) {
		__common.log_fmt(ERR, "[is_time_api_qry](tf:%d) 현재시각(%s) > next qry tm(%s)",m_timeframe, now_ymd_hms.c_str(), m_next_qrytm_ydm_hms);
		__common.assert_();
		return false;
	}

	if (now_ymd_hms.compare(m_next_qrytm_ydm_hms) < 0) {
		return false;
	}


	//===== 현재시각과 같으면 fire =====//
	string backup = m_last_qrytm_ydm_hms;
	strcpy(m_last_qrytm_ydm_hms, m_next_qrytm_ydm_hms);

	CTimeUtils util;
	char dt[32]{}, tm[32]{};
	sprintf(dt, "%.8s", m_last_qrytm_ydm_hms);
	sprintf(tm, "%.6s", m_last_qrytm_ydm_hms + 9);

	// next 재계산
	util.AddMins_ (dt, tm, m_timeframe, m_next_qrytm_ydm_hms);
	
	__common.debug_fmt("[is_time_api_qry] FIRE (TF:%d)(%s)(last:%s)(fire:%s)(next:%s)",
						m_timeframe, 
						m_symbol.c_str(), 
						backup.c_str(),
						m_last_qrytm_ydm_hms, 
						m_next_qrytm_ydm_hms
						);

	return true;
}



//========== Global Functions ==========//

void __update_candle_basetm(string& tf_s, string& symbol, string& tm_kor_ymd_hms)
{
	if(__is_basetm_updated)	return;
		
	int tf = std::stol(tf_s);
	for (auto& sym_ptr : __SymbolSets)
	{
		if (sym_ptr->m_timeframe == tf && sym_ptr->m_symbol.compare(symbol) == 0)
		{
			sym_ptr->update_base_candletm(tm_kor_ymd_hms);
		}
	}

	__is_basetm_updated = true;

	//===== 모든 symbolset 이 base time update 되었나 =====/
	for (auto& sym_ptr : __SymbolSets)
	{
		if (sym_ptr->m_status==EN_STATUS::CREATED)
		{
			__is_basetm_updated = false;
			break;
		}
	}

	if (__is_basetm_updated) {
		__common.debug_fmt("모든 SymbolSet 의 base time 업데이트 되었다");
	}
}









set<int>	__get_timeframes()
{
	set<int> tfs;
	for (auto& sym_ptr : __SymbolSets)
	{
		tfs.insert(sym_ptr->m_timeframe);
	}
	return tfs;
}

set<string>	__get_symbols()
{
	set<string> symbols;
	for (auto& sym_ptr : __SymbolSets)
	{
		symbols.insert(sym_ptr->m_symbol);
	}
	return symbols;
}


