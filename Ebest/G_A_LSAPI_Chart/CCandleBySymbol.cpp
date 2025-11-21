#include "CIOCPServer.h"
#include "CCandleBySymbol.h"
#include <json.hpp>
#include "CBOTDbSaver.h"

map<string, ns_candle::CandleBySymbolPtr>	__CandleList;

ns_candle::CCandleBySymbol::CCandleBySymbol(string symbol, int dot_cnt, set<int>& tfs)
{
	m_symbol = symbol;

	for (const auto& tf : tfs)
	{
		m_candles.push_back(std::make_shared<CCandle>(tf, dot_cnt));
		__common.debug_fmt("[%s]Candle is created(TF:%d)", m_symbol.c_str(), tf);
	}

	m_thrd = std::thread(&ns_candle::CCandleBySymbol::thrdfunc_main, this);
}


ns_candle::CCandleBySymbol::~CCandleBySymbol()
{
	set_die();
	if(m_thrd.joinable())	m_thrd.join();
}

void ns_candle::CCandleBySymbol::push_data(DataUnitPtr p)
{
	m_ring_recv.push(p);
}

void ns_candle::CCandleBySymbol::thrdfunc_main()
{
	while (m_is_continue)
	{
		Sleep(1);
		
		if(is_passed_last_sent_tm()){
			check_fire_api_request();
		}

		
		DataUnitPtr data = m_ring_recv.pop();
		if( !data ) continue;

		//RING_Q_RET ret = m_ring_recv.pop(data);
		//if (ret!=RING_Q_RET::Succ){
		//	continue;
		//}

		if (data->symbol.compare(m_symbol))
			continue;

		switch (data->tp)
		{
			case DATA_TP::API:
				update_candle_by_api_data(data);
				break;
			case DATA_TP::CLOSE:
				update_candle_by_close_price(data);
				break;
		}
		
	}
}

bool	ns_candle::CCandleBySymbol::compare_candle_tm_for_fire(const string& now, const string& next_qry_tm)
{
	if(next_qry_tm.empty()) return false;

	int comp = std::strncmp(now.c_str(), m_last_sent_min.c_str(), LEN_TM_MIN );
	return (comp > 0 && now.compare(next_qry_tm) >= 0);
}

void	ns_candle::CCandleBySymbol::check_fire_api_request()
{
	CTimeUtils util; string now = util.sDateTime_yyyymmdd_hhmmss();
	
	for (auto& candle : m_candles)
	{
		Sleep(1);
		if( candle->m_status != CANDLE_STATUS::READY ){
			continue;
		}

		if (compare_candle_tm_for_fire(now, candle->m_next_qry_tm))
		{
			//__common.debug_fmt("[API REQUEST FIRE](%s)(%d)(now_min:%.*s)>(last_sent_min:%s) && (now:%s)>=(next_qry_tm:%s)", 
			//					m_symbol.c_str(),
			//					candle->m_tf,
			//					LEN_TM_MIN,
			//					now.c_str(),
			//					m_last_sent_min.c_str(),
			//					now.c_str(),
			//					candle->m_next_qry_tm.c_str()
			//					);

			char nul[]="";
			DataUnitPtr data = std::make_shared<TDataUnit>(DATA_TP::REQUEST);
			data->set(m_symbol, candle->m_tf, (char*)candle->m_next_qry_tm.c_str(),
				nul, nul, nul, nul, nul);

			candle->status_fired();

			//===== api request 를 할 수 있도록
			if(m_cb_req_api)	m_cb_req_api(data);

			m_last_sent_min = now.substr(0, LEN_TM_MIN);	// 
		}
	}
}

void	ns_candle::CCandleBySymbol::update_candle_by_close_price(DataUnitPtr& data)
{
	CStringUtils su;
	if (!su.is_number(data->c)) {
		__common.log_fmt(ERR, "[CCandleBySymbol::update_candle_by_close_price] close is not number:%s", data->c.c_str());
		return ;
	}
	double price = std::stod(data->c);

	for (auto& candle : m_candles)
	{
		Sleep(1);
		if (candle->m_status == CANDLE_STATUS::CREATED)
		{
			continue;
		}
		

		// base candle time 필요		
		string price_tm			= data->candle_tm_kor;
		string price_candle_tm;
		
		if (candle->m_map_by_candletm.empty()) {			
			price_candle_tm = calc_ongoing_candle_tm(price_tm, candle->m_tf, price_tm);
			__common.debug_fmt("[update_candle_by_close_price]map is empty. price_tm:%s, price_candle_tm:%s",
				price_tm, price_candle_tm);
		}
		else
		{
			map<string, shared_ptr<TCandle>>::reverse_iterator itLast = candle->m_map_by_candletm.rbegin();
			price_candle_tm = calc_ongoing_candle_tm(price_tm, candle->m_tf, itLast->first);
		}		

		auto it = candle->m_map_by_candletm.find(price_candle_tm);

		//========== 아직 API TR 로 새로운 Candle 생성 전. close price 를 이용해서 만든다. ==========//
		if(it== candle->m_map_by_candletm.end())
		{
			candle->m_map_by_candletm[price_candle_tm] = std::make_shared<TCandle>(
				price_candle_tm
				, std::stod(data->c)
				, std::stod(data->c)
				, std::stod(data->c)
				, std::stod(data->c)
				, std::stol(data->v)
			);

			//candle->m_next_qry_tm = calc_next_qry_tm(price_tm, candle->m_tf, price_candle_tm);

			__common.debug_fmt("[update_candle_by_close_price-1](%s)(%d)아직 TR로 새 캔들 생성 전. close price 정보를 이용해서 만든다."
									"(price_time:%s)(price candle:%s)(close price:%s)(next_qry:%s)"
									,m_symbol.c_str()
									,candle->m_tf
									,price_tm.c_str()
									, price_candle_tm.c_str()
									, data->c.c_str()
									, candle->m_next_qry_tm.c_str()
									);

			
		}
		//========== 이미 해당 candle 이 있는 경우 - update ==========//
		else
		{
			// h,l,c 다시 계산
			shared_ptr<TCandle> &p = it->second;

			p->h = (p->h < price)? price : p->h;
			p->l = (p->l > price)? price : p->l;
			p->c = price;
			//candle->v += std::stoi(data->v);

			//===== Send to client
			send_candle_to_client("close price", candle->m_tf, price_candle_tm, p->o, p->h, p->l, p->c, p->v);

			//__common.log_fmt(INFO, "[update_candle_by_close_price-2](%s)(%d)(%.5f)(price_time:%s)price candle:%s)"
			//	"같은 시간 candle 이 이미 있으므로 h,l,c 갱신"
			//	, m_symbol.c_str()
			//	, candle->m_tf
			//	, p->c
			//	, price_tm.c_str()
			//	, price_candle_tm.c_str()
			//);
		}
	}
}


void	ns_candle::CCandleBySymbol::send_candle_to_client(
							const char* caller, int tf, string& candle_tm, double o, double h, double l, double c, int v)
{
	nlohmann::json j;
	j["code"] = "CHART";
	j["symbol"] = m_symbol;
	j["timeframe"] = tf;
	//j["candle_tm"] = candle_tm;
	j["o"] = o;
	j["h"] = h;
	j["l"] = l;
	j["c"] = c;
	j["v"] = v;

	CTimeUtils ut;
	long utxTimestamp = ut.toUnixTimestamp(candle_tm);
	j["candle_tm"] = utxTimestamp;
	
	std::string data = j.dump();
	if (data.size() == 0) {
		__common.assert_();
	}
	data += "\r\n";
	
	__common.debug_send("(%s)[CLIENT SEND](%s)", caller, data.c_str());

	__iocpSvr.broadcast_all_clients(m_symbol, tf, data);	

	string candle_end_tm = calc_candle_end_tm(candle_tm);
	ns_bot_db::TBOTCandPtr bot = make_shared< ns_bot_db::TBOTCandle>(m_symbol, tf, candle_tm, candle_end_tm, o, h, l, c, v);
	__bot_manager.push_to_all(bot);
}

void	ns_candle::CCandleBySymbol::update_candle_by_api_data(DataUnitPtr& data)
{
	CTimeUtils util;
	string now = util.sDateTime_yyyymmdd_hhmmss();

	for (auto& candle : m_candles)
	{
		Sleep(1);
		if (candle->m_tf == data->tf)
		{
			if(candle->m_status == CANDLE_STATUS::CREATED)
			{
				update_candle_by_first(now, data, candle);
				m_last_sent_min = now.substr(0, LEN_TM_MIN);
			}
			else
			{
				update_candle_by_next(now, data, candle);
			}
			candle->status_ready();
			break;
		}
	}
}

/*
	api 데이터의 candle time 을 이용해서 현재 진행중인 candle time (ongoing_candle_tm) 계산

	api      now         ongoing_candle_tm
	10:03   10:04   ==>   10:06
	10:06   10:04   ==>   10:06
*/
void ns_candle::CCandleBySymbol::update_candle_by_first(const string& now, DataUnitPtr& api, CandlePtr& candle)
{
	CStringUtils su;
	if (!su.is_number(api->o) ||
		!su.is_number(api->h) ||
		!su.is_number(api->l) ||
		!su.is_number(api->c) ||
		!su.is_number(api->v)
		)
	{
		__common.log_fmt(ERR, "[CCandleBySymbol::update_candle_by_first]convert to number error:%s,%s,%s,%s,%s",
			api->o.c_str(), api->h.c_str(), api->l.c_str(), api->c.c_str(), api->v.c_str());
		return;
	}

	//========== 해당 candle time 의 candle 정보 저장
	candle->m_map_by_candletm[api->candle_tm_kor] = std::make_shared<TCandle>(
																		api->candle_tm_kor
																		,std::stod(api->o)
																		,std::stod(api->h)
																		,std::stod(api->l)
																		,std::stod(api->c)
																		,std::stol(api->v)
																		);

	__common.debug_fmt("[update_candle_by_first-1] 캔들 생성.(%s)(%d)(now:%s), (candle_tm:%s)(ohlcv:%s/%s/%s/%s/%s)"
					,m_symbol.c_str()
					,candle->m_tf
					,now.c_str()
					,api->candle_tm_kor.c_str()
					,api->o.c_str()
					,api->h.c_str()
					,api->l.c_str()
					,api->c.c_str()
					,api->v.c_str()
					);

	//========== 다음 쿼리 시간 계산
	candle->m_next_qry_tm = calc_next_qry_tm(now, candle->m_tf, api->candle_tm_kor);
	candle->m_status = CANDLE_STATUS::READY;
	__common.debug_fmt("[update_candle_by_first-2] 다음 쿼리 시간 저장.(%s)(%d)(now:%s)(next_qry_tm:%s)"
		, m_symbol.c_str()
		, candle->m_tf
		, now.c_str()
		, candle->m_next_qry_tm.c_str()
		);
}


/*
	
*/
void ns_candle::CCandleBySymbol::update_candle_by_next(const string& now, DataUnitPtr& api_data, CandlePtr& candle)
{
	string api_tm	= api_data->candle_tm_kor;
	

	CStringUtils su;
	if (!su.is_number(api_data->o) ||
		!su.is_number(api_data->h) ||
		!su.is_number(api_data->l) ||
		!su.is_number(api_data->c) ||
		!su.is_number(api_data->v)
		)
	{
		__common.log_fmt(ERR, "[update_candle_by_next]convert to number error:%s,%s,%s,%s,%s",
			api_data->o.c_str(), api_data->h.c_str(), api_data->l.c_str(), api_data->c.c_str(), api_data->v.c_str());
		return;
	}
	
	double o = std::stod(api_data->o);
	double h = std::stod(api_data->h);
	double l = std::stod(api_data->l);
	double c = std::stod(api_data->c);
	int v = std::stoi(api_data->v);

	//===== 같은 시간 candle 찾는다.
	map<string, shared_ptr<TCandle>>::iterator it = candle->m_map_by_candletm.find(api_tm);

	// 새로운 candle
	if (it == candle->m_map_by_candletm.end())
	{
		candle->m_map_by_candletm[api_tm] = std::make_shared<TCandle>(api_tm,o,h,l,c,v);

		//API 정보를 이용해서 전송
		send_candle_to_client("api tr", candle->m_tf, api_tm, o, h, l, c, v);

		candle->m_next_qry_tm = calc_next_qry_tm(now, candle->m_tf, api_tm);
		
		__common.debug_fmt("[update_candle_by_next-새로운candle](%s)(%d)(now:%s)(API:%s)(next qry:%s)"
								"새로운 candle 생성, client 전송, next qry time재계산",
								m_symbol.c_str(), candle->m_tf, now.c_str(), api_tm.c_str(), candle->m_next_qry_tm.c_str());

		// 오래된 캔들정보 삭제
		candle->remove_old_candle();
	}

	// 기존 candle update
	else
	{
		shared_ptr<TCandle>& p = it->second;
		p->o = o;
		p->h = h;
		p->l = l;
		p->c = c;
		p->v = v;

		//API 정보를 이용해서 전송
		send_candle_to_client("api tr", candle->m_tf, api_tm, o,h,l,c,v );

		// 다음조회시각이 candle time 보다 작으면 next query 다시 계산
		if (api_tm.compare(candle->m_next_qry_tm) > 0) {
			candle->m_next_qry_tm = calc_next_qry_tm(now, candle->m_tf, api_tm);
		}

		__common.debug_fmt("[update_candle_by_next-기존candle](%s)(%d)(now:%s)(API:%s)(next qry:%s)"
			"기존 candle update, client 전송",
			m_symbol.c_str(), candle->m_tf, now.c_str(), api_tm.c_str(), candle->m_next_qry_tm.c_str());

	}
}



string	ns_candle::calc_next_qry_tm(const string& now, long tf, const string& base_candle_tm)
{
	string base = base_candle_tm;
	while (now.compare(base) < 0)
	{
		base = calc_prev_candle_tm(tf, base_candle_tm);
	}

	string rslt = calc_ongoing_candle_tm(now, tf, base);
	char firetime[32]{};
	sprintf(firetime, "%.*s%.2s", LEN_TM_MIN, rslt.c_str(), __common.apiqry_on_sec());

	//__common.debug_fmt("[calc_next_qry_tm](now:%s)(최초base:%s)(수정base:%s)(next_qry:%s)"
	//	, now.c_str(), base_candle_tm.c_str(), base.c_str(), firetime);


	return string(firetime);
}

string	ns_candle::calc_ongoing_candle_tm(const string& now, long tf, const string& i_ongoing_candle)
{
	string result;
	char ongoing[32]{};	//yyyymmdd_hhmmss
	strcpy(ongoing, i_ongoing_candle.c_str());

	for (;;)
	{
		Sleep(1);
		if (now.compare(ongoing) < 0)	// 현재 10:04,  candle 10:06
		{
			result = ongoing;
			break;
		}

		CTimeUtils util;
		char dt[32]{}, tm[32]{};

		sprintf(dt, "%.8s", ongoing);
		sprintf(tm, "%.6s", ongoing + 9);	//yyyymmdd_hhmmss

		//===== 현재 candle time 을 베이스로 다음 candle time 계산 및 저장
		util.AddMins_(dt, tm, tf, ongoing);

		strcpy(ongoing + LEN_TM_MIN, "00");	// candle 은 second 가 0 이다.
	}

	return result;
}


string	ns_candle::calc_prev_candle_tm(long tf, const string& i_ongoing_candle)
{
	char prev[32]{};
	char ongoing[32]{};	//yyyymmdd_hhmmss
	strcpy(ongoing, i_ongoing_candle.c_str());

	CTimeUtils util;
	char dt[32]{}, tm[32]{};

	sprintf(dt, "%.8s", ongoing);
	sprintf(tm, "%.6s", ongoing + 9);	//yyyymmdd_hhmmss

	util.AddMins_(dt, tm, tf * -1, prev);


	return string(prev);
}


string	ns_candle::calc_candle_end_tm(const string& candle_tm)
{
	char end_tm[32]{};	 

	CTimeUtils util;
	char dt[32]{}, tm[32]{};

	sprintf(dt, "%.8s", candle_tm.data());
	sprintf(tm, "%.6s", candle_tm.data() + 9);	//yyyymmdd_hhmmss

	util.AddSeconds_(dt, tm, -1, end_tm);	// candle time 의 1초 이전이 종료시간이다.


	return string(end_tm);

}
