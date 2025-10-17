#include "CAPIClient.h"
#include "../Common/Util.h"
#include "../Common/MaxTradeInc.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CSenderList.h"


extern CGlobals		gCommon;
extern CSenderList	gSenders;;

CAPIClient::CAPIClient() :m_reconnTimeout(0)
{
	m_sock.emplace(m_ioContext);
	m_thrdFlag.is_idle();
	//m_threads[THRD_IDX::RECV] = std::thread(&CAPIClient::threadFunc_recv_apiData, this);
	//m_threads[THRD_IDX::TICK] = std::thread(&CAPIClient::threadFunc_save_marketData, this, THRD_IDX::TICK);
	//m_threads[THRD_IDX::QUOTE] = std::thread(&CAPIClient::threadFunc_save_marketData, this, THRD_IDX::QUOTE);
	//m_idTick = GetThreadId(m_threads[THRD_IDX::TICK].native_handle());
	//m_idQuote = GetThreadId(m_threads[THRD_IDX::QUOTE].native_handle());

	m_thrdRecv		= std::thread(&CAPIClient::threadFunc_recv_apiData, this);
	m_thrdParsing	= std::thread(&CAPIClient::threadfunc_parse_deploy, this);

}

CAPIClient::~CAPIClient()
{
	DeInitialize();
}

void CAPIClient::DeInitialize()
{
	m_thrdFlag.set_stop();

	if (m_sock.has_value())
	{
		if (m_sock->is_open())
			m_sock->cancel();
	}

	//for (int i = THRD_IDX::RECV; i <= THRD_IDX::QUOTE; i++)
	//{
	//	if (m_threads[i].joinable()) {
	//		m_threads[i].join();
	//	}
	//}

	if (m_thrdRecv.joinable()) {
		m_thrdRecv.join();
	}
	if (m_thrdParsing.joinable()) {
		m_thrdParsing.join();
	}

	if (m_sock.has_value()) {
		boost::system::error_code ec;
		m_sock->close(ec);
		m_sock.reset();
	}
}


bool CAPIClient::load_config()
{
	char zSection[] = "API_INFO";
	char zVal[128] = { 0 };
	
	if (gCommon.getConfig(zSection, (char*)"SERVER_IP", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "Failed to read config file - HOST(%s)", zSection);
		return false;
	}
	m_sServerIp = zVal;

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig(zSection, (char*)"SERVER_PORT", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "Failed to read config file - Port(%s)", zSection);
		return false;
	}
	m_sServerPort = zVal;

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig(zSection, (char*)"RECONNECT_TIMEOUT_SEC", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "Failed to read config file - RECONNECT_TIMEOUT_SEC(%s)", zSection);
		return false;
	}

	m_reconnTimeout = atoi(zVal);

	gCommon.log(INFO, TRUE, "OK to read config file - HOST(%s), Port(%s)",
		m_sServerIp.c_str(), m_sServerPort.c_str());

	return true;
}

bool CAPIClient::Initialize()
{
	if (!load_config())
		return false;


	//while (!Connect())
	//{
	//	if (m_thrdFlag.is_stopped())
	//		return false;

	//	std::this_thread::sleep_for(std::chrono::seconds(3));
	//}
	
	return true;
}

bool CAPIClient::Connect()
{
	
	bool bRet = true;
	boost::system::error_code ec;

	tcp::resolver resolver{ m_ioContext };
	tcp::resolver::results_type endpoints = resolver.resolve(m_sServerIp, m_sServerPort, ec);
	if (ec) {
		gCommon.log(ERR, TRUE, "[API연결오류]Resolve failed(%s)", ec.message().c_str());
		return false;
	}

	try
	{
		boost::asio::connect(*m_sock, endpoints, ec);

		if (ec) {
			gCommon.log(ERR, TRUE, "[API연결오류]Connect failed(%s)", ec.message().c_str());
			bRet = false;
		}
		else {
			gCommon.log(INFO, TRUE, "[API연결 성공]Connect OK(%s)(%s)", m_sServerIp.c_str(), m_sServerPort.c_str());
		}


	}
	catch (const boost::system::system_error& ec)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[API연결오류]client socket connect error:%s", ec.what());
	}
	catch (const std::exception& e)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[API연결오류]client socket unexpected error:%s", e.what());
	}
	catch (...)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[API연결오류]client socket unknown error");
	}

	if(bRet)
		m_thrdFlag.set_run();
	return bRet;
}


void CAPIClient::threadFunc_recv_apiData()
{
	char recvBuff[1024];
	while (!m_thrdFlag.is_stopped() )
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (!m_thrdFlag.is_running())
			continue;

		boost::system::error_code ec;

		if (!m_sock.has_value())
		{
			gCommon.log(INFO, TRUE, "client socket is closed. Exiting thread...");
			break;
		}

		if (!m_sock->is_open())
			break;

		ZeroMemory(recvBuff, sizeof(recvBuff));
		m_sock->read_some(boost::asio::buffer(recvBuff), ec);

		if (m_thrdFlag.is_stopped())
			break;

		if (ec)
		{
			if (ec == boost::asio::error::eof || ec.value()==10057) {
				gCommon.log(INFO, TRUE, "[API데이터수신]Socket is closed!!![%d](%s)", ec.value(), ec.message().c_str());
				if (!Reconnect())
					break;
			}
			else {
				gCommon.log(ERR, TRUE, "[API데이터수신]Receive error!!![%d](%s)", ec.value(), ec.message().c_str());
				Sleep(1000);
				continue;
			}
		}

		std::lock_guard<std::mutex> lock(m_mtxBuff);
		m_parsingBuffer += recvBuff;
	}

	gCommon.log(INFO, TRUE, "exiting api client socket read thread...");
}


void CAPIClient::threadfunc_parse_deploy()
{
	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!m_parsingBuffer.empty())
		{
			std::unique_lock <std::mutex> lock(m_mtxBuff);

			string send_buff;
			size_t pos;
			size_t len = m_parsingBuffer.size();
			while ((pos = m_parsingBuffer.find("\r\n")) != std::string::npos)								// IPWorks 에서 enter 붙여서 보낸다.
			{
				send_buff += m_parsingBuffer.substr(0, pos+1);												// \r\n 모두 추출(클라이언트 역시 IPWorks)
				m_parsingBuffer.erase(0, pos + 2);															// 처리된 부분 삭제 (2는 "\r\n" 길이)
			}
			lock.unlock();

			gSenders.sendData_all_clients(send_buff);
		}
	}
}


//void CAPIClient::threadFunc_save_marketData(int idx)
//{
//	while (!m_thrdFlag.is_stopped())
//	{
//		MSG PeekMsg;
//		while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
//		{
//			if (PeekMsg.message == WM_QUIT)
//				return;
//
//			if (PeekMsg.message == __MAX::WM_MARKET_DATA)
//			{
//				__MAX::TData* pRecvData = reinterpret_cast<__MAX::TData*>(PeekMsg.lParam);
//				
//				if (idx == THRD_IDX::TICK) {
//					gMarketData.save_tick(pRecvData->d);
//				}
//				else {
//					gMarketData.save_quote(pRecvData->d);
//				}
//
//				gCommon.memPool.release(pRecvData);
//			}
//		}
//	}
//
//	gCommon.log(INFO, TRUE, "exiting save thread[%d]...", idx);
//}

bool CAPIClient::Reconnect()
{
	while (!m_thrdFlag.is_stopped())
	{
		if (!Connect()) {
			gCommon.log(ERR, TRUE, "[Reconnect 에러]API 연결 오류 입니다.");
			Sleep(reconnTimeout());
		}
		else
			return true;
	}
	return false;
}