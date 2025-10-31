#include "CSymbolSets.h"


vector<SymbolInfoPtr>	__SymbolSets;
bool					__is_basetm_updated = false;


void CSymbolSets::update_base_candletm(string& tm_kor_ymd_hms)
{
	if (m_status != EN_STATUS::CREATED){
		__common.debug_fmt("\t<update_base_candletm> �̹� update �ߴ�.(TF:%d)(%s)(base tm:%s)(last qry:%s)(����:%s)",
						m_timeframe,
						m_symbol.c_str(),
						m_base_candletm_ydm_hms,
						m_last_qrytm_ydm_hms,
						tm_kor_ymd_hms.c_str()
						);
		return;
	}

	//===== ���� �ð� =====//
	CTimeUtils util;
	string now_ymd_hms = util.sDateTime_yyyymmdd_hhmmss();

	//===== api �� ���� candle time	=====//
	char candle_ymd_hms[64];
	sprintf(candle_ymd_hms, tm_kor_ymd_hms.c_str());


	//===== api �� ���� candle time ���� candle base time �� update	=====//
	// �ٸ�, ���� candle �� ������ �������� �� �����Ƿ�, ���� �ð��� �ʰ����� �ʴ� �ֱ� candle time �� ����ؼ� ����

	char curr_ymd_hms[32]{};
	char next_ymd_hms[32]{};

	strcpy(curr_ymd_hms, candle_ymd_hms);

	for (;;)
	{
		// next candle time �� ����ð� ���� ũ�� curr candle time �� �������� �Ѵ�.
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

		//===== ���� candle time �� ���̽��� ���� candle time ��� �� ����
		util.AddMins_(dt, tm, m_timeframe, next_ymd_hms);

		_mm_pause();

		//__common.debug_fmt("\t<update_base_candletm-�����> (TF:%d)(%s)(curr candle:%s)(next candle:%s)(now:%s)",
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

	//===== ����ð��� �� ũ�� ���� =====//
	if (now_ymd_hms.compare(m_next_qrytm_ydm_hms) > 0) {
		__common.log_fmt(ERR, "[is_time_api_qry](tf:%d) ����ð�(%s) > next qry tm(%s)",m_timeframe, now_ymd_hms.c_str(), m_next_qrytm_ydm_hms);
		__common.assert_();
		return false;
	}

	if (now_ymd_hms.compare(m_next_qrytm_ydm_hms) < 0) {
		return false;
	}


	//===== ����ð��� ������ fire =====//
	string backup = m_last_qrytm_ydm_hms;
	strcpy(m_last_qrytm_ydm_hms, m_next_qrytm_ydm_hms);

	CTimeUtils util;
	char dt[32]{}, tm[32]{};
	sprintf(dt, "%.8s", m_last_qrytm_ydm_hms);
	sprintf(tm, "%.6s", m_last_qrytm_ydm_hms + 9);

	// next ����
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

	//===== ��� symbolset �� base time update �Ǿ��� =====/
	for (auto& sym_ptr : __SymbolSets)
	{
		if (sym_ptr->m_status==EN_STATUS::CREATED)
		{
			__is_basetm_updated = false;
			break;
		}
	}

	if (__is_basetm_updated) {
		__common.debug_fmt("��� SymbolSet �� base time ������Ʈ �Ǿ���");
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


