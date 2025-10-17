#pragma once

#include <winsock2.h>
#include "../../Common/MaxTradeInc.h"

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

#define NUM_DATA_TYPE	2
enum class DATA_TYPE { TICK,QUOTE};
inline char* type_s(DATA_TYPE tp, char* out) {
	strcpy(out, "TICK_DATA");
	if(tp==DATA_TYPE::QUOTE)
		strcpy(out, "BOOK_DATA");
	return out;
}

const string PATH_TICK = "@ticker";
const string PATH_BOOK = "@depth5@100ms";

struct TSymbol
{
	std::string	symbol;
	long dotCnt;
};

class CBinanceApi
{
public:
	//CBinanceApi();
	CBinanceApi();
	~CBinanceApi();

	bool Initialize();

private:
	void DeInitialize();

	bool load_rootCert();
	bool load_symbols();
	bool load_config();
	bool request_apiData(DATA_TYPE type);
	void threadFunc_recv_apiData(DATA_TYPE type);

private:
	boost::optional<beast_websocket::stream<asio_ssl::stream<tcp::socket>>> m_webSockets[NUM_DATA_TYPE] ;
	boost::asio::io_context		m_ioContext;
	asio_ssl::context			m_sslctx;
	//std::vector<TSymbol>		m_Symbols;
	std::map<std::string, long>	m_mapSymbols;
	string						m_sBinanceHost;
	string						m_sBinancePort;

	std::thread					m_thrdRecv[NUM_DATA_TYPE] ;
	__MAX::CThreadFlag			m_thrdFlag;
};

