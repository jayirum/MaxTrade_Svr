/*
	LS 증권의 차트 시간은 

	10:00:00~10:00:59 까지가 1분봉으로 10:01 캔들이다.
*/

#pragma once
#pragma warning(disable:4996)

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
#include "../../Common/TimeUtils.h"
#include "CGlobals.h"
//#include "../../Common/CSPSCRing.h"
#include <set>
#include <map>
#include "AppCommon.h"


using namespace std;

namespace ns_candle
{

constexpr int MAX_CANDLE_CNT	= 3;
constexpr int LEN_TM			= 15;
enum class CANDLE_STATUS	{ CREATED, READY, FIRED };
enum class COMP_TIME		{ API_EARLY=-1, SAME, API_LATE};


string	calc_ongoing_candle_tm	(const string& now, long tf, const string& ongoing_candle);
string	calc_next_qry_tm		(const string& now, long tf, const string& base_candle_tm);
string	calc_prev_candle_tm		(long tf, const string& ongoing_candle);
string	calc_candle_end_tm		(const string& candle_tm);

struct TCandle
{
	TCandle(string& tm, double op, double hi, double lo, double cl, int vo)
		:candle_tm{tm}, o{op}, h{hi}, l{lo}, c{cl}, v{vo}{
			candle_end_tm = calc_candle_end_tm(tm);
		};

	string		candle_tm;
	string		candle_end_tm;
	double		o{0}, h{ 0 }, l{ 0 }, c{ 0 };
	int			v{0};
};

class CCandle
{
public:

	CCandle(int timeframe, int dot_cnt) {
		m_tf = timeframe;
		m_dot_cnt = dot_cnt;
	}
	void status_fired() { m_status = CANDLE_STATUS::FIRED; }
	void status_ready() { m_status = CANDLE_STATUS::READY; }
	void remove_old_candle()
	{
		while (m_map_by_candletm.size() > MAX_CANDLE_CNT) 
		{
			auto it = m_map_by_candletm.begin();
			m_map_by_candletm.erase(it);
		}
	};

public:
	int				m_tf{ 0 };
	string			m_next_qry_tm;
	int				m_dot_cnt{ 0 };
	CANDLE_STATUS	m_status{ CANDLE_STATUS::CREATED };

	map<string, shared_ptr< TCandle>>	m_map_by_candletm;
};

using CandlePtr		= shared_ptr <CCandle>;
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

	void	send_candle_to_client(const char* caller, int tf, string& candle_tm, double o, double h, double l, double c, int v);


	
	int		compare_candle_tm(string l, string r) {
		return std::strncmp(l.c_str(), r.c_str(), LEN_TM_MIN);	//yyyymmdd_hhmmss 중 분까지만 비교
	}

	bool	is_passed_last_sent_tm() {
		CTimeUtils t;
		string now = t.sDateTime_yyyymmdd_hhmm();
		return (m_last_sent_min.compare(now)<0);
	}

public:
	vector<CandlePtr>			m_candles;

private:
	
	std::string								m_symbol;
	//CSPSCRing<DataUnitPtr>					m_ring_recv{4};	// chart data, close price 수신
	CSimpleQueue<DataUnitPtr>				m_ring_recv;	// chart data, close price 수신
	CALLBACK_RQST							m_cb_req_api;
	std::thread								m_thrd;
	bool									m_is_continue{true};

	string									m_last_sent_min;
};

using CandleBySymbolPtr = std::shared_ptr<CCandleBySymbol>;

} // namespace ns_candle

extern map<string, ns_candle::CandleBySymbolPtr>	__CandleList;
