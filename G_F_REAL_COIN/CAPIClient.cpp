#include "CAPIClient.h"
#include "../Common/Util.h"
#include "CSenderQList.h"
#include "../Common/MaxTradeInc.h"

#include <fstream>
#include <iostream>
#include <iomanip>


extern CGlobals		gCommon;
extern CSenderQList	gQList;

CAPIClient::CAPIClient():m_reconnTimeout(0)
{
	m_sock.emplace(m_ioContext);
	m_thrdFlag.setThreadRun();
	m_thrdRecv		= std::thread(&CAPIClient::threadFunc_recv_apiData, this);
	m_thrdEnqueue	= std::thread(&CAPIClient::threadFunc_enqueue_apiData, this);
}

CAPIClient::~CAPIClient()
{
	DeInitialize();
}


void CAPIClient::DeInitialize()
{
	m_thrdFlag.setThreadStop();

	if (m_sock.has_value())
	{
		if (m_sock->is_open())
			m_sock->cancel();
	}

	if (m_thrdRecv.joinable()) {
		m_thrdRecv.join();

		if (m_sock.has_value()){
			boost::system::error_code ec;
			m_sock->close(ec);
			m_sock.reset();
		}
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


	return true;
}

bool CAPIClient::Connect()
{
	
	bool bRet = true;
	boost::system::error_code ec;

	tcp::resolver resolver{ m_ioContext };
	tcp::resolver::results_type endpoints = resolver.resolve(m_sServerIp, m_sServerPort, ec);
	if (ec) {

		gCommon.log(ERR, TRUE, "Resolve failed(%s)", ec.message().c_str());
		return false;
	}

	try
	{
		boost::asio::connect(*m_sock, endpoints, ec);

		if (ec) {
			gCommon.log(ERR, TRUE, "Connect failed(%s)", ec.message().c_str());
			bRet = false;
		}
		else {
			gCommon.log(INFO, TRUE, "Connect OK(%s)(%s)", m_sServerIp.c_str(), m_sServerPort.c_str());
		}


	}
	catch (const boost::system::system_error& ec)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "client socket connect error:%s", ec.what());
	}
	catch (const std::exception& e)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "client socket unexpected error:%s", e.what());
	}
	catch (...)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "client socket unknown error");
	}

	if(bRet)
		m_thrdFlag.setThreadReady();

	return bRet;
}


void CAPIClient::threadFunc_recv_apiData()
{
	while (!m_thrdFlag.isReady())
	{
		if (!m_thrdFlag.isRunning()) return;
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	char recvBuff[1024];
	while (m_thrdFlag.isRunning() && m_thrdFlag.isReady() )
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		ZeroMemory(recvBuff, sizeof(recvBuff));
		boost::system::error_code ec;

		if (!m_sock.has_value())
		{
			gCommon.log(INFO, TRUE, "client socket is closed. Exiting thread...");
			break;
		}

		if (!m_sock->is_open())
			break;

		m_sock->read_some(boost::asio::buffer(recvBuff), ec);

		if (!m_thrdFlag.isRunning())
			break;

		if (ec)
		{
			if (ec == boost::asio::error::eof || ec.value()==10057 || ec.value()==10054) {
				gCommon.log(INFO, TRUE, "client socket is closed[%d][%s]", ec.value(), ec.message().c_str());
				if (!Reconnect())
					return;
			}
			else {
				gCommon.log(ERR, TRUE, "client socket receive error-[%d]%s", ec.value(), ec.message().c_str());
				Sleep(1000);
				continue;
			}
		}

		std::lock_guard<std::mutex> lock(m_mutexParsingBuffer);
		m_parsingBuffer += recvBuff;


		
	}

	gCommon.log(INFO, TRUE, "exiting api client socket read thread...");
}

void CAPIClient::threadFunc_enqueue_apiData()
{

	while (!m_thrdFlag.isReady()) {
		if (!m_thrdFlag.isRunning()) return;
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));

		if (!m_parsingBuffer.empty())
		{
			std::lock_guard<std::mutex> lock(m_mutexParsingBuffer);

			size_t pos;
			size_t len = m_parsingBuffer.size();
			// 123456789/r/n
			while ((pos = m_parsingBuffer.find("\r\n")) != std::string::npos)
			{
				std::string packet = m_parsingBuffer.substr(0, pos+2);  // 첫 번째 패킷 추출
				m_parsingBuffer.erase(0, pos + 2);  // 처리된 부분 삭제 (2는 "\r\n" 길이)

				gQList.enQueue_to_allSenders(packet);
			}
		}
	}
}

bool CAPIClient::Reconnect()
{
	while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));

		if (!Connect()) {
			Sleep(reconnTimeout());
		}
		else
			return true;
	}
	return false;
}