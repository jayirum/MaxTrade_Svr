#include "CIOCPServer.h"
#include "CCandleBySymbol.h"
#include <json.hpp>


map<string, ns_candle::SymbolInfoPtr>	__CandleList;

ns_candle::CCandleBySymbol::CCandleBySymbol(string symbol, int dot_cnt, set<int>& tfs)
{ 
	m_symbol = symbol;

	for (const auto& tf : tfs)
	{
		m_candles.push_back(std::make_shared<TCandle>(tf, dot_cnt));
		__common.debug_fmt("[%s]Candle is created(TF:%d)", m_symbol.c_str(), tf);
	}

	m_thrd = std::thread(&ns_candle::CCandleBySymbol::thrdfunc_main, this);
}


ns_candle::CCandleBySymbol::~CCandleBySymbol()
{
	for (auto& [sb, candles] : __CandleList)
	{
		candles->set_die();
	}

	m_is_continue = false;
	if(m_thrd.joinable())	m_thrd.join();
}

void ns_candle::CCandleBySymbol::push_data(DataUnitPtr p)
{
	m_rcv_queue.push(p);
}

void ns_candle::CCandleBySymbol::thrdfunc_main()
{
	while (m_is_continue)
	{
		_mm_pause();

		if(is_passed_last_sent_tm()){
			check_fire_api_request();
		}


		DataUnitPtr data;
		bool is_exist = m_rcv_queue.pop(data);
		if (!is_exist)	continue;

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

bool	ns_candle::CCandleBySymbol::compare_candle_tm_for_fire(const string& now, const string& candle_tm)
{
	char fire_tm[32];
	sprintf(fire_tm, "%.*s%.2s", LEN_TM_MIN, candle_tm.c_str(), __common.apiqry_on_sec());

	int comp = std::strncmp(now.c_str(), m_last_sent_min.c_str(), LEN_TM_MIN );
	return (comp > 0 && now.compare(fire_tm) >= 0);
}

void	ns_candle::CCandleBySymbol::check_fire_api_request()
{
	CTimeUtils util; string now = util.sDateTime_yyyymmdd_hhmmss();
	
	for (auto& candle : m_candles)
	{
		if( candle->status != CANDLE_STATUS::READY ){
			_mm_pause();
			continue;
		}

		if (compare_candle_tm_for_fire(now, candle->ongoing_candle_tm))
		{
			__common.debug_fmt("[API REQUEST FIRE](%s)(%d)(now_min:%.*s)>(last_sent_min:%s) && (now:%s)>=(ongoing:%s)", 
								m_symbol.c_str(),
								candle->tf,
								LEN_TM_MIN,
								now.c_str(),
								m_last_sent_min.c_str(),
								now.c_str(),
								candle->ongoing_candle_tm.c_str()
								);

			char nul[]="";
			DataUnitPtr data = std::make_shared<TDataUnit>(DATA_TP::REQUEST);
			data->set(m_symbol, candle->tf, (char*)candle->ongoing_candle_tm.c_str(),
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
		if (candle->status == CANDLE_STATUS::CREATED)
		{
			continue;
		}
		string price_tm = data->candle_tm_kor;
		////===== 가격의 시간은 prev ~ ongoing 사이에 있어야 한다.
		//string price_tm = data->candle_tm_kor;
		//if (candle->prev_candle_tm.compare(price_tm) >= 0 )
		//{
		//	__common.log_fmt(ERR, "[update_candle_by_close_price-1](%s)(%d) (ongoing:%s), (prev:%s)>=(price time:%s)"
		//		, m_symbol.c_str()
		//		, candle->tf
		//		, candle->ongoing_candle_tm.c_str()
		//		, candle->prev_candle_tm.c_str()
		//		, price_tm.c_str()
		//	);

		//	__common.assert_();
		//	continue;
		//}
		
		//========== 아직 API TR 로 새로운 Candle 생성 전. 
		if(candle->ongoing_candle_tm.compare(price_tm) < 0	)
		{
			__common.log_fmt(INFO, "[update_candle_by_close_price-2](%s)(%d) (ongoing:%s)<=(price time:%s)"
									"여기서는 아무것도 하지 않고 skip(TR로 새캔들 생성전)"
									,m_symbol.c_str()
									,candle->tf
									,candle->ongoing_candle_tm.c_str()
									,price_tm.c_str()
									);

			//candle->backup();
			//candle->reset();
			//candle->o = candle->h = candle->l = candle->c = price;
			//
			//candle->ongoing_candle_tm	= calc_ongoing_candle_tm(price_tm, candle->tf, candle->ongoing_candle_tm);
			//candle->prev_candle_tm		= calc_prev_candle_tm(candle->tf, candle->ongoing_candle_tm);

			//__common.log_fmt(INFO, "[update_candle_by_close_price-3](%s)(%d) candle reset (price time:%s)(ongoing:%s)(prev:%s)"
			//	"새로운 candle 생성해야 함."
			//	, m_symbol.c_str()
			//	, candle->tf
			//	, price_tm.c_str()
			//	, candle->ongoing_candle_tm.c_str()
			//	, candle->prev_candle_tm.c_str()
			//);

			continue;
		}
		else if (candle->ongoing_candle_tm.compare(price_tm) == 0) {
			__common.debug_fmt("(ongoing:%s)==(price_time:%s) 이 close 데이터 까지는 ongoing 에 포함", 
			candle->ongoing_candle_tm.c_str(), price_tm.c_str());
		}

		//===== h,l,c 다시 계산
		
		candle->h = (candle->h < price)? price : candle->h;
		candle->l = (candle->l > price)? price : candle->l;
		candle->c = price;
		//candle->v += std::stoi(data->v);

		//===== Send to client
		send_candle_to_client_byCandle("close price", candle);
	}
}


void	ns_candle::CCandleBySymbol::send_candle_to_client_byApi(const char* caller, const DataUnitPtr& api)
{
	nlohmann::json j;
	j["code"]	= "CHART";
	j["symbol"] = m_symbol;
	j["timeframe"] = api->tf;
	j["candle_tm"] = api->candle_tm_kor;
	j["o"] = api->o;
	j["h"] = api->h;
	j["l"] = api->l;
	j["c"] = api->c;
	j["v"] = api->v;

	std::string data = std::move(j.dump());
	__common.debug_fmt("(%s)[CLIENT SEND-API](%s)", caller, data.c_str());

	__iocpSvr.broadcast_all_clients(data);	
}

void	ns_candle::CCandleBySymbol::send_candle_to_client_byCandle(const char* caller, const ns_candle::CandlePtr& candle)
{
	nlohmann::json j;
	j["code"] = "CHART";
	j["symbol"] = m_symbol;
	j["timeframe"] = candle->tf;
	j["candle_tm"] = candle->ongoing_candle_tm;
	j["o"] = candle->o;
	j["h"] = candle->h;
	j["l"] = candle->l;
	j["c"] = candle->c;
	j["v"] = candle->v;
	
	std::string data = std::move(j.dump());
	if (data.size() == 0) {
		int a = 0;
	}

	try{
		__common.debug_fmt("(%s)[CLIENT SEND-CANDLE](%s)", caller, data.c_str());
	}
	catch(...)
	{
		int a = 0;
	}

	__iocpSvr.broadcast_all_clients(data);	
}

void	ns_candle::CCandleBySymbol::update_candle_by_api_data(DataUnitPtr& data)
{
	CTimeUtils util;
	string now = util.sDateTime_yyyymmdd_hhmmss();

	for (auto& candle : m_candles)
	{
		if (candle->tf == data->tf)
		{
			if(candle->status == CANDLE_STATUS::CREATED)
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


string	ns_candle::CCandleBySymbol::calc_ongoing_candle_tm(const string& now, long tf, const string& i_ongoing_candle)
{
	string result;
	char ongoing[32]{};	//yyyymmdd_hhmmss
	strcpy(ongoing, i_ongoing_candle.c_str());

	for (;;)
	{
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

		_mm_pause();
	}

	return result;
}


string	ns_candle::CCandleBySymbol::calc_prev_candle_tm(long tf, const string& i_ongoing_candle)
{
	char prev[32]{};
	char ongoing[32]{};	//yyyymmdd_hhmmss
	strcpy(ongoing, i_ongoing_candle.c_str());

	CTimeUtils util;
	char dt[32]{}, tm[32]{};

	sprintf(dt, "%.8s", ongoing);
	sprintf(tm, "%.6s", ongoing + 9);	//yyyymmdd_hhmmss

	util.AddMins_(dt, tm, tf*-1, prev);


	return string(prev);
}

/*
	api 데이터의 candle time 을 이용해서 현재 진행중인 candle time (ongoing_candle_tm) 계산

	api      now         ongoing_candle_tm
	10:03   10:04   ==>   10:06
	10:06   10:04   ==>   10:06
*/
void ns_candle::CCandleBySymbol::update_candle_by_first(const string& now, DataUnitPtr& api, CandlePtr& candle)
{
	//===== 이미 현재시간 보다 큰 캔들이 오면 시고저종 저장
	if(compare_candle_tm(api->candle_tm_kor, now)>0)
	{
		candle->ongoing_candle_tm = api->candle_tm_kor;;
		
		CStringUtils su;
		if (!su.is_number(api->o) ||
			!su.is_number(api->h) ||
			!su.is_number(api->l) ||
			!su.is_number(api->c) ||
			!su.is_number(api->v)
			) 
		{
			__common.log_fmt(ERR, "[CCandleBySymbol::update_candle_by_first]convert to number error:%s,%s,%s,%s,%s", 
				api->o.c_str(), api->h.c_str(), api->l.c_str(), api->c.c_str(), api->v.c_str() );
			return;
		}
		candle->o = std::stod(api->o);
		candle->h = std::stod(api->h);
		candle->l = std::stod(api->l);
		candle->c = std::stod(api->c);
		candle->v = std::stol(api->v);

		__common.debug_fmt("[update_candle_by_first]현재시간 이후 캔들 수신.(%s)(%d)(now:%s)<(api:%s)(ohlcv:%s/%s/%s/%s/%s)"
						,m_symbol.c_str()
						,candle->tf
						,now.c_str()
						,api->candle_tm_kor.c_str()
						,api->o.c_str()
						,api->h.c_str()
						,api->l.c_str()
						,api->c.c_str()
						,api->v.c_str()
						);
	}
	else
	{
		candle->ongoing_candle_tm = calc_ongoing_candle_tm(now, candle->tf, api->candle_tm_kor);

		__common.debug_fmt("[update_candle_by_first]현재시간 이전 캔들 수신.ongoing계산(%s)(%d)(now:%s)(api:%s)(ongoing:%s)"
			, m_symbol.c_str()
			, candle->tf
			, now.c_str()
			, api->candle_tm_kor.c_str()
			, candle->ongoing_candle_tm.c_str()
		);
	}

	candle->prev_candle_tm = calc_prev_candle_tm(candle->tf, api->candle_tm_kor);
	candle->status = CANDLE_STATUS::READY;
	
}


/*
	api 를 통해서 수신한 데이터를 이용해서 시고저종 조정
	api 캔들시간이 ongoing 과 같거나 이전이면 client 전송

	now 1004 >= api 1003
	now 1004 <  api 1006
			    api 1006 < ongoing 1009  => ongoing 조정.
			   api == ongoing => save, send
			   api > ongoing  => ongoing 조정, save, send
*/
void ns_candle::CCandleBySymbol::update_candle_by_next(const string& now, DataUnitPtr& api_data, CandlePtr& candle)
{
	string api_tm	= api_data->candle_tm_kor;
	string ongoing	= candle->ongoing_candle_tm;

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

	//// 현재보다 이전 캔들이 오는 건 캔들시간만 재계산한다.
	//int comp = compare_candle_tm(now, api_tm);
	//if(comp>0)
	//{
	//	__common.log_fmt(INFO, "[update_candle_by_next-1](%s)(%d)(now:%s)>(API:%s) ongoing 재계산",
	//		m_symbol.c_str(), candle->tf, now.c_str(), api_tm.c_str());

	//	candle->ongoing_candle_tm	= calc_ongoing_candle_tm(now, candle->tf, api_tm);
	//	candle->prev_candle_tm		= calc_prev_candle_tm(candle->tf, api_tm);
	//}
	//else
	{
		ns_candle::COMP_TIME api_ongoing = (ns_candle::COMP_TIME)compare_candle_tm(api_tm, ongoing);
		switch(api_ongoing)
		{
		// api 캔들시간이 ongoing 이전이다. ==> 전송
		case ns_candle::COMP_TIME::API_EARLY:
		{
			//API 정보를 이용해서 전송
			send_candle_to_client_byApi("api tr", api_data);

			string new_ongoing = calc_ongoing_candle_tm(now, candle->tf, api_tm);
			__common.log_fmt(INFO, "[update_candle_by_next-2](%s)(%d)(now:%s), (API:%s)<(ongoing:%s). API데이터 Client전송. ongoing 재계산(%s)",
				m_symbol.c_str(), candle->tf, now.c_str(), api_tm.c_str(), ongoing.c_str(), new_ongoing.c_str());

			candle->ongoing_candle_tm	= new_ongoing;
			//candle->prev_candle_tm		= calc_prev_candle_tm(candle->tf, api_tm);
			break;
		}
		case ns_candle::COMP_TIME::SAME:
		{
			__common.log_fmt(INFO, "[update_candle_by_next-3](%s)(%d)(now:%s), (API:%s)==(ongoing:%s) candle 저장 후 client 전송. ongoing time 조정",
				m_symbol.c_str(), candle->tf, now.c_str(), api_tm.c_str(), ongoing.c_str());

			candle->o = std::stod(api_data->o);
			candle->h = std::stod(api_data->h);
			candle->l = std::stod(api_data->l);
			candle->c = std::stod(api_data->c);
			candle->v = std::stoi(api_data->v);

			//ongoing 전송
			send_candle_to_client_byCandle("api tr", candle);

			//========== 새로운 ongoing ==========//
			candle->ongoing_candle_tm	= calc_ongoing_candle_tm(now, candle->tf, api_tm);
			//candle->prev_candle_tm		= calc_prev_candle_tm(candle->tf, candle->ongoing_candle_tm);

			candle->o = 0;
			candle->h = 0;
			candle->l = 0;
			candle->c = 0;
			candle->v = 0;

			__common.log_fmt(INFO, "[update_candle_by_next-4]새로운 ongoing 초기화.(%s)(%d)(now:%s)(ongoing:%s)",
				m_symbol.c_str(), candle->tf, now.c_str(), candle->ongoing_candle_tm.c_str());

			break;
		}			
		case ns_candle::COMP_TIME::API_LATE:
		{
			__common.log_fmt(INFO, "[update_candle_by_next-5](%s)(%d)(now:%s), (API:%s)>(ongoing:%s) ongoing time 조정.candle 저장 및 전송",
				m_symbol.c_str(), candle->tf, now.c_str(), api_tm.c_str(), ongoing.c_str());

			candle->ongoing_candle_tm = api_tm;

			candle->o = std::stod(api_data->o);
			candle->h = std::stod(api_data->h);
			candle->l = std::stod(api_data->l);
			candle->c = std::stod(api_data->c);
			candle->v = std::stoi(api_data->v);

			//candle->prev_candle_tm = calc_prev_candle_tm(candle->tf, api_tm);

			//API 정보를 이용해서 전송
			send_candle_to_client_byCandle("api tr", candle);
			break;
		}
		} // switch(api_ongoing)
	}
}
