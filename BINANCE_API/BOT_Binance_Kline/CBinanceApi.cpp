#include "CBinanceApi.h"
#include "CGlobals.h"
#include "../../Common/Util.h"
#include "../../Common/MaxTradeInc.h"

#include <fstream>
#include <iostream>
#include <iomanip>

extern HANDLE		g_hDieEvent;				// event for terminating process
extern CGlobals		gCommon;

CBinanceApi::CBinanceApi(std::shared_ptr<CDBSave> pDb, std::shared_ptr<CMSSQLSave> pMssql)
	:m_dbSave(std::move(pDb)), m_mssqlSave(std::move(pMssql))
{
	m_thrdFlag.set_idle();
}

CBinanceApi::~CBinanceApi()
{
	DeInitialize();
}


void CBinanceApi::DeInitialize()
{
	m_thrdFlag.set_stop();

	for (auto &it : m_vecThreads) {
		if (it.joinable())	it.join();
	}
}

bool CBinanceApi::load_symbols()
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid)  ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols] Failed to read DB information in INI file");
		return FALSE;
	}

	char zConnStr[512] = { 0 };
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	CODBC odbc(DBMS_TYPE::MYSQL);
	if (!odbc.Initialize())
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}
	if (!odbc.Connect(zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols]DB Connect failed:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024] = { 0 };
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_SYMBOLS", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols]Ini does not have query.([QUERY][LOAD_SYMBOLS])");
		return false;
	}

	odbc.Init_ExecQry(zQ);

	bool bNeedReconn;
	if (!odbc.Exec_Qry(bNeedReconn)) {
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		odbc.DeInit_ExecQry();
		return false;
	}

	gCommon.log(INFO, TRUE, "Load Symbols Query(%s)", zQ);

	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		//TSymbol symbolInfo;

		char zSymbol[128] = { 0, };
		long dotCnt = 0;

		odbc.GetDataStr(1, sizeof(zSymbol), zSymbol);

		//TODO
		//if (string(zSymbol) != "BTCUSDT")
		//	continue;

		// thread 생성 --------------------------------------------------------------------------------------
		m_vecThreads.push_back(thread(&CBinanceApi::threadFunc_fetch_data, this, zSymbol));

		gCommon.log(INFO, TRUE, "OK to load symbols from DB(%s)", zSymbol);
		
		Sleep(1);

	}
	odbc.DeInit_ExecQry();

	return true;
}


bool CBinanceApi::load_timeframes()
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes] There is no DB Info in INI file");
		return FALSE;
	}

	char zConnStr[512] = { 0 }; 
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	CODBC odbc(DBMS_TYPE::MYSQL);
	if (!odbc.Initialize())
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}
	if (!odbc.Connect(zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes]DB Connect failed:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024] = { 0 };
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_TIMEFRAME", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes]Ini does not have query.([QUERY][LOAD_SYMBOLS])");
		return false;
	}

	odbc.Init_ExecQry(zQ);

	bool bNeedReconn;
	if (!odbc.Exec_Qry(bNeedReconn)) {
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		odbc.DeInit_ExecQry();
		return false;
	}

	gCommon.log(INFO, TRUE, "Load Timeframes Query(%s)", zQ);

	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char nm[128] = { 0 };
		long cd;

		odbc.GetDataLong(1, &cd);
		odbc.GetDataStr(2, sizeof(nm), nm);
		
		//TODO
		//if (cd != 3 && cd!=5 )
		//	continue;

		m_vecIntervals.push_back(std::make_unique<TInterval>(cd,nm));

		gCommon.log(INFO, TRUE, "OK to load timeframe from DB(Nm:%s)(Code:%d)", nm, cd);

		Sleep(1);

		//TODO
		//break;
	}

	odbc.DeInit_ExecQry();

	return true;
}



bool CBinanceApi::load_config()
{
	char zVal[128] = { 0 };
	
	//----------------------------------------------------------------------------------------------------//

	if (gCommon.getConfig((char*)"BINANCE_INFO", (char*)"HOST", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST");
		return false;
	}
	m_sBinanceHost = zVal;

	//----------------------------------------------------------------------------------------------------//

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig((char*)"BINANCE_INFO", (char*)"PORT", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST");
		return false;
	}
	m_sBinancePort = zVal;

	//----------------------------------------------------------------------------------------------------//

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig((char*)"BINANCE_INFO", (char*)"KLINE_ENDPOINT", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read KLINE_ENDPOINT");
		return false;
	}
	m_sBinanceEndpoint = zVal;

	//----------------------------------------------------------------------------------------------------//

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig((char*)"APP_CONFIG", (char*)"REQUEST_SEC", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read REQUEST_SEC");
		return false;
	}
	sprintf(m_rqstSec, "%.2s", zVal);


	gCommon.log(INFO, TRUE, "OK to read config file - HOST(%s), Port(%s)",
		m_sBinanceHost.c_str(), m_sBinancePort.c_str());

	return true;
}

bool CBinanceApi::Initialize()
{
	if (!load_config())
		return false;

	if (!load_timeframes())
		return false;

	if (!load_symbols())
		return false;

	m_thrdFlag.set_run();

	return true;
}

void CBinanceApi::threadFunc_fetch_data(char* pzSymbol)
{
	__try
	{
		threadFunc_fetch_data_Internal(pzSymbol);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
		gCommon.log(ERR, TRUE, "unexpected exception in CBinanceApi::threadFunc_fetch_data");
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
	}
	gCommon.log(INFO, TRUE, "Exiting threadFunc_fetch_data...==> kill this process");
	SetEvent(g_hDieEvent);

}

void CBinanceApi::threadFunc_fetch_data_Internal(char* pzSymbol)
{
	string symbol(pzSymbol);
	char timeStamp[128];
	
	char hhmmss[128];
	string lastTime;
	bool bOnce = false;

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (m_thrdFlag.is_idle())	continue;

		if (!bOnce) {
			request_api_data(symbol);
			bOnce = true;
			continue;
		}


		ZeroMemory(timeStamp, sizeof(timeStamp));
		__UTILS::timestamp_yyyymmdd_hhmmssmmm(timeStamp);
		sprintf(hhmmss, "%.6s", &timeStamp[9]);


		// 이미 실행했으면 pass
		if (lastTime.compare(hhmmss) == 0)
			continue;

		// 02초에 수행
		if (strncmp(&hhmmss[4], m_rqstSec, 2) != 0)
			continue;

		lastTime = string(hhmmss);

		gCommon.debug("[데이터요청시작](%s)(%s)", lastTime.c_str(), symbol.c_str());

		request_api_data(symbol);
	}

}

void CBinanceApi::request_api_data(string& symbol)
{
	for (auto& interval : m_vecIntervals)
	{
		json::value data = std::move(fetch_data(symbol, interval));

		if (!data.is_null())
		{
			std::shared_ptr <TConvertedData> cvt = parsingJson(symbol, interval->name, data);
			if (cvt) {
				m_dbSave->push_data(cvt);

#ifndef __DEV
				m_mssqlSave->push_data(cvt);
#endif
			}
		}
	}
}


json::value CBinanceApi::fetch_data(string& symbol, unique_ptr<TInterval>& interval)
{
	net::io_context ioc;
	ssl::context ctx(ssl::context::tlsv12_client);
	ctx.set_default_verify_paths();

	tcp::resolver resolver(ioc);
	beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

	boost::system::error_code ec;
	try
	{
		auto const results = resolver.resolve(m_sBinanceHost, m_sBinancePort, ec);
		if (ec) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] Host resolution failed(%d)(%s)",ec.value(), ec.message());
			return json::value();  // 빈 JSON 반환
		}

		beast::get_lowest_layer(stream).connect(results, ec);
		if (ec) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] Connection to Binance failed(%d)(%s)", ec.value(), ec.message());
			return json::value();  // 빈 JSON 반환
		}

		// Perform SSL handshake
		if (!SSL_set_tlsext_host_name(stream.native_handle(), m_sBinanceHost.c_str())) {
			throw beast::system_error(
				beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()),
				"Failed to set SNI hostname");
		}
		stream.handshake(ssl::stream_base::client, ec);
		if (ec) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] handshake to Binance failed(%d)(%s)", ec.value(), ec.message());
			return json::value();  // 빈 JSON 반환
		}

		// Create HTTP request
		char start_time[128] = { 0 };
		long long timestamp_ms = get_aligned_timestamp_ms(interval->code, start_time);
		string req_str = m_sBinanceEndpoint 
						+ "?symbol=" + symbol 
						+ "&interval=" + interval->name
						+ "&limit=" + "1"
						+ "&startTime="+ std::to_string(timestamp_ms);

		http::request<http::string_body> req(http::verb::get, req_str, 11);
		req.set(http::field::host, m_sBinanceHost);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		gCommon.debug("\t[REQUEST](%s)(%s)(%s)(%s)", symbol.c_str(), interval->name.c_str(), start_time, req_str.c_str());

		// Send request
		http::write(stream, req, ec);
		if (ec) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] Request write failed(%d)(%s)", ec.value(), ec.message());
			return json::value();  // 빈 JSON 반환
		}

		// Read response
		beast::flat_buffer buffer;
		http::response<http::dynamic_body> res;
		http::read(stream, buffer, res, ec);
		if (ec) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] Response read failed(%d)(%s)", ec.value(), ec.message());
			return json::value();  // 빈 JSON 반환
		}

		beast::error_code ec;
		stream.shutdown(ec);
		if (ec == net::error::eof || ec == ssl::error::stream_truncated) {
			ec = {};
		}
		if (ec) throw beast::system_error{ ec };

		/// Parse and return the JSON response
		std::string body = beast::buffers_to_string(res.body().data());

		if (!is_valid_json_response(res, body)){
			return json::value();
		}
		gCommon.debug( "\t<RESPONS>(%s)(%s)(%s)(%s)", symbol.c_str(), interval->name.c_str(), start_time, body.c_str());

		try {
			return std::move(json::parse(body));
		}
		catch (const std::exception& e) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] JSON Parsing failed(%s)", e.what());
			return json::value();  // 빈 JSON 반환
		}
	}
	catch (const std::exception& e) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::fetch] Exception caught failed(%s)", e.what());
		return json::value();  // 빈 JSON 반환
	}
}

long long CBinanceApi::get_aligned_timestamp_ms(int interval_min, _Out_ char* yyyymmdd_hhmm)
{
	using namespace std::chrono;

	// 기준 시간: 현재 - 1분 (실제 캔들이 생성되어 있는 시점)
	auto ts = system_clock::now() - minutes(1);	//BUG수정. 모든 분봉에 대해 1분전 것 요청 minutes(interval_min);
	auto ms = duration_cast<milliseconds>(ts.time_since_epoch()).count();

	long long interval_ms = interval_min * 60LL * 1000;
	long long aligned_ms = (ms / interval_ms) * interval_ms;

	std::time_t t = aligned_ms / 1000;
	std::tm local_tm;
	localtime_s(&local_tm, &t);
	std::sprintf(yyyymmdd_hhmm, "%04d%02d%02d_%02d%02d%02d",
		local_tm.tm_year + 1900,
		local_tm.tm_mon + 1,
		local_tm.tm_mday,
		local_tm.tm_hour,
		local_tm.tm_min,
		local_tm.tm_sec
	);

	return aligned_ms;
}

bool CBinanceApi::is_valid_json_response(const http::response<http::dynamic_body>& res, const std::string& body) 
{
	if (res.result_int() != 200) {
		gCommon.log(ERR, TRUE, "[HTTP Error] Status(%d)", res.result_int());
		return false;
	}
	if (body.empty()) {
		gCommon.log(ERR, TRUE, "[Empty Body] Body of the response is empty");
		return false;
	}
	if (body.find("<html>") != std::string::npos || body.find("CloudFront") != std::string::npos) {
		gCommon.log(ERR, TRUE, "[HTML/CloudFront Error] Recognized HTML response");
		return false;
	}
	try {
		auto json_data = json::parse(body);
		if (!json_data.is_array()) {
			gCommon.log(ERR, TRUE, "[Invalid JSON] This JSON is not array.");
			return false;
		}
	}
	catch (const std::exception& e) {
		gCommon.log(ERR, TRUE, "[JSON Parse Error] ");
		return false;
	}

	return true;
}


/*
[
  [
	1499040000000,      // Open time
	"0.01634790",       // Open
	"0.80000000",       // High
	"0.01575800",       // Low
	"0.01577100",       // Close
	"148976.11427815",  // Volume
	1499644799999,      // Close time
	"2434.19055334",    // Quote asset volume
	308,                // Number of trades
	"1756.87402397",    // Taker buy base asset volume
	"28.46694368",      // Taker buy quote asset volume
	"17928899.62484339" // Ignore.
  ]
]
*/
std::unique_ptr <TConvertedData> CBinanceApi::parsingJson(string& symbol, string& interval_nm, json::value& data)
{
	if (!data.is_array())
	{
		gCommon.log(ERR, TRUE, "[CBinanceApi::parsingJson] Invalid JSON response!");
		return nullptr;
	}

	std::unique_ptr <TConvertedData> cvt = std::make_unique< TConvertedData>();

	char zKoreanTime[128];

	for (const auto& candle : data.as_array())
	{
		cvt->symbol = symbol;
		cvt->interval = interval_nm;

		int64_t time = candle.at(0).as_int64();
		__UTILS::UnixTimestamp_to_KoreanTime(time, zKoreanTime);	// yyyymmdd-hhmmss
		cvt->time_s = zKoreanTime;

		time = candle.at(6).as_int64();
		__UTILS::UnixTimestamp_to_KoreanTime(time, zKoreanTime);	// yyyymmdd-hhmmss
		cvt->time_e = zKoreanTime;

		cvt->o = candle.at(1).as_string();
		cvt->h = candle.at(2).as_string();
		cvt->l = candle.at(3).as_string();
		cvt->c = candle.at(4).as_string();

		gCommon.debug("[Convert](symbol:%s)(interval:%s)(time_s:%s)(time_e:%s)(o:%s)(h:%s)(l:%s)(c:%s)",
			cvt->symbol.c_str(), cvt->interval.c_str(), cvt->time_s.c_str(), cvt->time_e.c_str(), cvt->o.c_str(),
			cvt->h.c_str(), cvt->l.c_str(), cvt->c.c_str());
	}

	return cvt;
}