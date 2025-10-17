#pragma once

#include <winsock2.h>
#include "CDBSave.h"

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/optional.hpp>
#include "CGlobals.h"

namespace beast_http		= boost::beast::http;		// from <boost/beast/http.hpp>
namespace beast_websocket	= boost::beast::websocket;	// from <boost/beast/websocket.hpp>
namespace asio				= boost::asio;				// from <boost/asio.hpp>
namespace asio_ssl			= boost::asio::ssl;			// from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;

using namespace std;


const string PATH_KLINE = "@kline_";



class CBinanceApi
{
public:
	//CBinanceApi();
	CBinanceApi(std::shared_ptr<CDBSave>& pDb);
	~CBinanceApi();

	bool Initialize();

private:
	void DeInitialize();

	bool load_rootCert();
	bool load_symbols();
	bool load_timeframes();
	bool load_config();
	bool request_apiData();
	void threadFunc_recv_apiData();

private:
	boost::optional<beast_websocket::stream<asio_ssl::stream<tcp::socket>>> m_webSockets;
	boost::asio::io_context		m_ioContext;
	asio_ssl::context			m_sslctx;
	std::map<std::string, long>	m_mapSymbols;
	std::vector<string>			m_vecTimeframe;
	string						m_sBinanceHost;
	string						m_sBinancePort;

	std::thread					m_thrdRecv ;
	CThreadFlag					m_thrdFlag;

	std::shared_ptr<CDBSave>	m_dbSave;

};

