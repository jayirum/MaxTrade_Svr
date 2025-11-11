// TcpSock.cpp: implementation of the CTcpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "CTcpClient.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ns_tcpclient::CTcpClient::CTcpClient(CLogMsg* log)
{
	m_log = log;
}
ns_tcpclient::CTcpClient::~CTcpClient()
{
	m_is_continue = false;
	if(m_thrd_recv.joinable())	m_thrd_recv.join();
	disconnect();
}

void ns_tcpclient::CTcpClient::disconnect()
{
	if (m_sock != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(m_sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(m_sock);
	}
	m_sock = INVALID_SOCKET;
	m_conn_flag = false;
}

bool ns_tcpclient::CTcpClient::begin(string svr_ip, int svr_port, int timeout_ms)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		strcpy(m_msg, "WSAStartup error");
		return false;
	}

	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			dump("create socket", GetLastError() );
			return false;
		}
	}

	m_svr_ip	= svr_ip;
	m_svr_port	= svr_port;
	m_timeout_ms= timeout_ms;

	////	remote address
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = htons(m_svr_port);
	m_sin.sin_addr.s_addr = inet_addr(m_svr_ip.c_str());
	
	m_thrd_recv = std::thread(&ns_tcpclient::CTcpClient::thrdfunc_recv, this);

	return true;
}


bool ns_tcpclient::CTcpClient::connect_svr()
{
	disconnect();

	if( m_sock == INVALID_SOCKET)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			dump("socket create", GetLastError() );	
			return false;
		}
	}

	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_conn_flag = false;
		dump("connect", GetLastError() );
		disconnect();
		return false;
	}

	// send timeout
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_timeout_ms, sizeof(m_timeout_ms));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_timeout_ms, sizeof(m_timeout_ms));

	m_conn_flag = true;

	if (m_log) m_log->log(LOGTP_SUCC, "connect server ok (%ip:%s)(port:%d)", m_svr_ip.c_str(), m_svr_port);

	return true;
}

void ns_tcpclient::CTcpClient::thrdfunc_recv()
{
	while (m_is_continue)
	{
		_mm_pause();
		if(!m_cb_recv_handler) continue;

		if (!is_conneced())
		{
			connect_svr();
			continue;
		}

		recv_proc();
	}
}


void ns_tcpclient::CTcpClient::recv_proc()
{
	int		recv_ret	= 0;
	bool	b_ret		= true;
	int		total_size	= 0;
	int		err_no		= 0;
	string	s_buff;
	char	rcv_buff[BUFF_SIZE];

	//ZeroMemory(rcv_buff, sizeof(rcv_buff));
	recv_ret = recv(m_sock, rcv_buff, sizeof(rcv_buff), 0);
	//if(recv_ret>0)	m_log->log(LOGTP_SUCC, "[recv](%.*s)", recv_ret, rcv_buff);
	while (recv_ret >0)
	{
		strcpy(m_msg, "데이터 수신");

		s_buff.append(rcv_buff, recv_ret);
		total_size	+= recv_ret;

		//ZeroMemory(rcv_buff, sizeof(rcv_buff));
		recv_ret = recv(m_sock, rcv_buff, sizeof(rcv_buff), 0);
		//if (recv_ret > 0)	m_log->log(LOGTP_SUCC, "[recv](%.*s)", recv_ret, rcv_buff);
	}

	if(recv_ret <=0)
	{
		if (recv_ret == SOCKET_ERROR)
		{
			err_no = WSAGetLastError();
			if (err_no != WSAETIMEDOUT)
			{
				dump("recv", err_no);
				disconnect();
				b_ret = false;
			}
		}
		
		if (recv_ret == 0) {
			sprintf(m_msg, "[%d] socket is closed by server", m_sock);
			disconnect();
			m_log->log(LOGTP_ERR, "Server 에 의해 disconnect 됨");
			b_ret = false;
		}
	}

	if (total_size > 0) {		
		m_cb_recv_handler(b_ret, total_size, s_buff, m_msg);
	}
}


int ns_tcpclient::CTcpClient::send_data( char* pInBuf, int nBufLen,  int *o_ErrCode )
{
	if (!is_conneced())
	{
		if (!connect_svr())
			return -1;
	}
	
	int Ret=0;
	int nRetryCnt = 0, nRetryBlock=0;
	
	while(m_is_continue)
	{
		Ret = send(m_sock, pInBuf, nBufLen, 0);
		if (Ret > 0) {
			*o_ErrCode = 0;
			break;
		}
		
		int nErr = WSAGetLastError();
		*o_ErrCode = nErr;
		if (nErr == WSAETIMEDOUT || nErr == WSAEWOULDBLOCK)
		{
			if (++nRetryCnt <= DEF_SEND_RETRY)	
				continue;
		
			sprintf( m_msg, "WSAETIMEDOUT or WSAEWOULDBLOCK %d회 반복으로 에러 리턴", nRetryCnt);
		}
		else
		{
			sprintf(m_msg, "Send Errr (%d)", nErr);
		}
		disconnect();
		break;
	}
	
	return Ret;
}


void ns_tcpclient::CTcpClient::dump( const char* pSrc, int nErr )
{
	LPVOID lpMsgBuf=NULL;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );

	sprintf( m_msg, "[%s] %s", pSrc, (char*)lpMsgBuf );
	LocalFree( lpMsgBuf );

	if(m_log) m_log->log(LOGTP_ERR, m_msg);
}

