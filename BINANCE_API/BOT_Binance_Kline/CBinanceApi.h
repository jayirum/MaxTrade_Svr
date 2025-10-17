#pragma once

#include "../../common/MaxTradeInc.h"
#include "CDBSave.h"
#include "CMSSQLSave.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/optional.hpp>
#include <boost/json.hpp>
#include <boost/beast/http.hpp>
#include "CGlobals.h"
#include <thread>


namespace net = boost::asio;
namespace beast = boost::beast;
namespace http	= beast::http;
namespace json	= boost::json;
namespace ssl	= boost::asio::ssl;
using tcp = net::ip::tcp;

using namespace std;


struct TInterval
{
	long code;
	string name;

	TInterval(int c, string n) :code(c), name(n) {}
};

class CBinanceApi
{
public:
	//CBinanceApi();
	CBinanceApi(std::shared_ptr<CDBSave> pDb, std::shared_ptr<CMSSQLSave> pMssql);
	~CBinanceApi();

	bool Initialize();

private:
	void DeInitialize();

	bool load_config();
	bool load_symbols();
	bool load_timeframes();

	//void connect_api();
	//void disconnect_api() { m_stream.close(); }

	void threadFunc_fetch_data(char* pzSymbol);
	void threadFunc_fetch_data_Internal(char* pzSymbol);
	void request_api_data(string& symbol);
	json::value fetch_data(string& symbol, unique_ptr<TInterval>& interval);
	std::unique_ptr <TConvertedData> parsingJson(string& symbol, string& interval_nm, json::value& data);
	bool is_valid_json_response(const http::response<http::dynamic_body>& res, const std::string& body);
	long long get_aligned_timestamp_ms(int interval_min, _Out_ char* yyyymmdd_hhmm);

private:
	//asio::io_context			m_ioContext;
	//asio::ip::tcp::resolver		m_resolver;
	//beast::tcp_stream			m_stream;
	string						m_sBinanceHost;
	string						m_sBinancePort;
	string						m_sBinanceEndpoint;

	std::vector<std::unique_ptr<TInterval>>		m_vecIntervals;
	std::vector<thread>			m_vecThreads;
	std::shared_ptr<CDBSave>	m_dbSave;
	std::shared_ptr<CMSSQLSave>	m_mssqlSave;


	__MAX::CThreadFlag			m_thrdFlag;
	char						m_rqstSec[3];
};

