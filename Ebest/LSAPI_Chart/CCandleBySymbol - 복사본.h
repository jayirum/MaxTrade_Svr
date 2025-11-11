#pragma once
#pragma warning(disable:4996)

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
#include "../../Common/TimeUtils.h"
#include "CGlobals.h"
#include "../../Common/CNoLockRingQueue.h"
#include <set>
#include "AppCommon.h"

using namespace std;

namespace ns_candle
{

constexpr int LEN_TM = 15;
enum class CANDLE_STATUS { CREATED, READY, FIRED };
enum class COMP_TIME { API_EARLY=-1, SAME, API_LATE};


struct TCandle
{
	TCandle(int timeframe, int dot_cnt) {
		tf = timeframe;
		dot_cnt = dot_cnt;
	}
	int			tf{0};
	string		ongoing_candle_tm;
	string		prev_candle_tm;
	double		o{0}, h{ 0 }, l{ 0 }, c{ 0 };
	int			v{0};
	int			dot_cnt{0};
	CANDLE_STATUS	status{ CANDLE_STATUS::CREATED};

	void reset() {o=0;h=0;l=0;c=0;v=0;}
	void status_fired() { status = CANDLE_STATUS::FIRED; }
	void status_ready() { status = CANDLE_STATUS::READY; }

	string candle_tm_bak;
	double o_bak{ 0 }, h_bak{ 0 }, l_bak{ 0 }, c_bak{ 0 };
	void backup(){ o_bak=o; h_bak=h; l_bak=l; c_bak=c;}
	void reset_bak(){ o_bak = 0; h_bak = 0; l_bak = 0; c_bak = 0; }
};

using CandlePtr		= shared_ptr < TCandle>;
using CALLBACK_RQST	= std::function<void(DataUnitPtr&)>	;

class CCandleBySymbol
{
public:
	explicit CCandleBySymbol(string symbol, int dot_cnt, set<int>& tfs);
	~CCandleBySymbol();

	void	push_data(DataUnitPtr p);

	void	setcallback_req_api(std::function<void(DataUnitPtr&)> f) {
		m_cb_req_api = std::move(f);
	}

	void	set_die(){ m_is_continue=false;}

private:
	bool	is_my_symbol(string& sb){ return (m_symbol==sb); }
	void	thrdfunc_main();
	void	check_fire_api_request();

	bool	compare_candle_tm_for_fire(const string& now, const string& candle_tm);

	void	update_candle_by_api_data(DataUnitPtr&);
	void	update_candle_by_first(const string& now, DataUnitPtr&, CandlePtr&);
	void	update_candle_by_next(const string& now, DataUnitPtr&, CandlePtr&);

	void	update_candle_by_close_price(DataUnitPtr&);

	void	send_candle_to_client_byCandle(const char* caller, const ns_candle::CandlePtr& candle);
	void	send_candle_to_client_byApi(const char* caller, const DataUnitPtr& api);

	string	calc_ongoing_candle_tm(const string& now, long tf, const string& ongoing_candle);
	string	calc_prev_candle_tm(long tf, const string& ongoing_candle);

	
	int		compare_candle_tm(string l, string r) {
		return std::strncmp(l.c_str(), r.c_str(), LEN_TM_MIN);	//yyyymmdd_hhmmss 중 분까지만 비교
	}

	bool	is_passed_last_sent_tm() {
		CTimeUtils t;
		string now = t.sDateTime_yyyymmdd_hhmm();
		return (m_last_sent_min.compare(now)<0);
	}

public:
	vector<CandlePtr>					m_candles;

private:
	
	std::string							m_symbol;
	CNoLockRingQueue<DataUnitPtr>		m_rcv_queue;	// chart data, close price 수신
	CALLBACK_RQST						m_cb_req_api;
	std::thread							m_thrd;
	bool								m_is_continue{true};

	string								m_last_sent_min;
};

using SymbolInfoPtr = std::shared_ptr<CCandleBySymbol>;

} // namespace ns_candle

extern map<string, ns_candle::SymbolInfoPtr>	__CandleList;
