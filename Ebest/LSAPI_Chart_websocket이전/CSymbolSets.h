#pragma once
#pragma warning(disable:4996)

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
#include "../../Common/TimeUtils.h"
#include "CGlobals.h"
#include <set>

using namespace std;


constexpr int LEN_TM = 15;
enum class EN_STATUS { CREATED, CANBE_FIRED };

class CSymbolSets
{
public:
	explicit CSymbolSets(const int tf, const string& s) {
		m_timeframe = tf;
		m_symbol	= s;
	}

	void	update_base_candletm(string& tm_kor_ymd_hms);
	bool	is_time_api_qry(string& now_ymd_hms);

public:
	int			m_timeframe;
	std::string m_symbol;
	char		m_base_candletm_ydm_hms[LEN_TM + 1]{};
	char		m_last_qrytm_ydm_hms[LEN_TM + 1]{};
	char		m_next_qrytm_ydm_hms[LEN_TM + 1]{};
	EN_STATUS	m_status = EN_STATUS::CREATED;
};

using SymbolInfoPtr = std::shared_ptr<CSymbolSets>;
extern vector<SymbolInfoPtr>	__SymbolSets;
extern bool						__is_basetm_updated;

void		__update_candle_basetm(string& tf_s, string& symbol, string& tm_kor_ymd_hms);
set<int>	__get_timeframes();
set<string>	__get_symbols();

