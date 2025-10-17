#pragma once

#include <winsock2.h>
#include <boost/beast/core.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/optional.hpp>
#include "CGlobals.h"


namespace asio = boost::asio;
using boost::asio::ip::tcp;

namespace THRD_IDX {
	const int RECV = 0;
	const int TICK = 1;
	const int QUOTE = 2;
};

class CAPIClient
{
public:
	CAPIClient();
	~CAPIClient();

	bool Initialize();
	bool Connect();
	void DeInitialize();
	int reconnTimeout() { return (m_reconnTimeout == 0) ? 3000 : m_reconnTimeout * 100; }

private:
	bool load_config();
	void threadFunc_recv_apiData();
	void threadfunc_parse_deploy();
	//void threadFunc_save_marketData(int idx);
	bool Reconnect();
	

private:
	boost::optional<tcp::socket> m_sock;
	boost::asio::io_context		m_ioContext;
	std::string					m_sServerIp;
	std::string					m_sServerPort;

	std::thread					m_thrdRecv;
	//std::thread					m_threads[3];
	//DWORD						m_idTick, m_idQuote;
	__MAX::CThreadFlag			m_thrdFlag;
	int							m_reconnTimeout;

	std::mutex      m_mtxBuff;
	std::string     m_parsingBuffer;
	std::thread     m_thrdParsing;

};

