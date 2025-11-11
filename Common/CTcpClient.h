#pragma once
#pragma warning(disable:4996)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <functional>
#include <string>
#include <algorithm>
#include <thread>
#include "LogMsg.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

namespace ns_tcpclient
{
	using RET_BOOL = bool;
	using RECV_LEN = int;
	using RECV_BUF = std::string;
	using MSG_BUF = char;
	using CALLBACK_RECV = std::function<void(RET_BOOL, RECV_LEN, const RECV_BUF&, const MSG_BUF*)>;

	constexpr int DEF_SEND_RETRY	= 3;
	constexpr int BUFF_SIZE			= 1024;

	class CTcpClient
	{
	public:
	
		CTcpClient(CLogMsg* log);
		~CTcpClient();

		bool	begin(string svr_ip, int svr_port, int timeout_ms);
		void	setcallback_recv_handler(CALLBACK_RECV f) {
			m_cb_recv_handler = std::move(f);
		}

		bool	connect_svr();
		bool 	is_conneced() { return m_conn_flag; }
		int		send_data(char* pInBuf, int nBufLen, int *o_ErrCode);
	
		char*	get_msg() { return m_msg; };

	private:
		void	thrdfunc_recv();
		void	recv_proc();
		void	disconnect();
		void	dump( const char* pSrc, int nErr );

		SOCKET			m_sock{ INVALID_SOCKET };
		string			m_svr_ip;
		int				m_svr_port{0};
		SOCKADDR_IN		m_sin;
		bool			m_conn_flag{ false };
		int				m_timeout_ms{ 10 };
		CALLBACK_RECV	m_cb_recv_handler;

		std::thread		m_thrd_recv;
		bool			m_is_continue{ true };
		char			m_msg[512]{0};
		CLogMsg			*m_log{nullptr};
	};
}
