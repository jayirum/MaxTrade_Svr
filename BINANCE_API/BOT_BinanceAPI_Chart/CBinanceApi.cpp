#include "CBinanceApi.h"
#include "CGlobals.h"
#include "../../Common/Util.h"
#include "../../Common/MaxTradeInc.h"
#include "Certificate.h"
#include "CSenderQList.h"

#include <fstream>
#include <iostream>
#include <iomanip>

extern HANDLE	g_hDieEvent;				// event for terminating process
extern CGlobals		gCommon;
extern CSenderQList	gQList;

CBinanceApi::CBinanceApi(std::shared_ptr<CDBSave>& pDb) :m_sslctx(asio_ssl::context::tlsv12), m_dbSave(pDb)
{
	m_webSockets.emplace(m_ioContext, m_sslctx);

	m_thrdFlag.setThreadRun();
	m_thrdRecv = std::thread(&CBinanceApi::threadFunc_recv_apiData, this);
}

CBinanceApi::~CBinanceApi()
{
	DeInitialize();
}


void CBinanceApi::DeInitialize()
{
	m_thrdFlag.setThreadStop();

	if (m_webSockets.has_value())
	{
		if (m_webSockets->is_open())
		{
			boost::system::error_code ec;
			m_webSockets->next_layer().next_layer().cancel();
		}
	}

	if (m_thrdRecv.joinable()) 
	{
		m_thrdRecv.join();

		if (m_webSockets.has_value()) {
			boost::system::error_code ec;
			m_webSockets->close(beast_websocket::close_code::normal, ec);
			m_webSockets.reset();
		}
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
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols]DB정보 조회 오류.INI파일을 확인하세요");
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
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols]DB Connect 오류:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024] = { 0 };
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_SYMBOLS", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_symbols]Ini 파일에 쿼리가 없습니다.([QUERY][LOAD_SYMBOLS])");
		return false;
	}
	//sprintf(zQ,	"SELECT B.STK_CD, A.DOT_CNT FROM ARTC_MST A, STK_MST B "
	//			" WHERE A.ACNT_TP = '3' AND A.ARTC_CD = B.ARTC_CD "
	//			" AND A.ARTC_USE_YN = 'Y' AND B.STK_USE_YN = 'Y' ");

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
		odbc.GetDataLong(2, &dotCnt);

		//if (string(zSymbol) != "BTCUSDT")
		//	continue;

		m_mapSymbols[zSymbol] = dotCnt;

		gCommon.log(INFO, TRUE, "OK to load symbols from DB(%s)(DotCnt:%d)", zSymbol, dotCnt);
		
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
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes]DB정보 조회 오류.INI파일을 확인하세요");
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
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes]DB Connect 오류:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024] = { 0 };
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_TIMEFRAME", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "[CBinanceApi::load_timeframes]Ini 파일에 쿼리가 없습니다.([QUERY][LOAD_SYMBOLS])");
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
		char zTimeframe[128] = { 0, };

		odbc.GetDataStr(1, sizeof(zTimeframe), zTimeframe);
		
		m_vecTimeframe.push_back(zTimeframe);

		gCommon.log(INFO, TRUE, "OK to load timeframe from DB(%s)", zTimeframe);

		Sleep(1);

	}

	odbc.DeInit_ExecQry();

	return true;
}


bool CBinanceApi::load_rootCert()
{
	bool ret = true;

	boost::system::error_code ec;
	certificate::load_root_certificates(m_sslctx, ec);
	if (ec)
	{
		ret = false;
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_rootCert]Failed to load RootCert:%s", ec.message().c_str());
	}
	else
		gCommon.log(INFO, TRUE, "Success in loading RootCert");
	return ret;
}


bool CBinanceApi::load_config()
{
	char zSection[] = "BINANCE_INFO";
	char zVal[128] = { 0 };
	
	//----------------------------------------------------------------------------------------------------//

	if (gCommon.getConfig(zSection, (char*)"HOST", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST(%s)", zSection);
		return false;
	}
	m_sBinanceHost = zVal;

	//----------------------------------------------------------------------------------------------------//

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig(zSection, (char*)"PORT", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST(%s)", zSection);
		return false;
	}
	m_sBinancePort = zVal;

	//----------------------------------------------------------------------------------------------------//

	//ZeroMemory(zVal, sizeof(zVal));
	//if (gCommon.getConfig((char*)"APP_CONFIG", (char*)"TIMEOUT_MS_RECV_APIDATA", zVal) == NULL) {
	//	gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read TIMEOUT_MS_RECV_APIDATA");
	//	return false;
	//}
	//m_timeout_recvapi_ms = (unsigned int)atoi(zVal);

	//----------------------------------------------------------------------------------------------------//

	//ZeroMemory(zVal, sizeof(zVal));
	//if (gCommon.getConfig((char*)"APP_CONFIG", (char*)"TIMEOUT_SEC_PING", zVal) == NULL) {
	//	gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read TIMEOUT_SEC_PING");
	//	return false;
	//}
	//m_timeout_ping_sec = (unsigned int)atoi(zVal);

	//----------------------------------------------------------------------------------------------------//

	gCommon.log(INFO, TRUE, "OK to read config file - HOST(%s), Port(%s)",
		m_sBinanceHost.c_str(), m_sBinancePort.c_str());

	return true;
}

bool CBinanceApi::Initialize()
{
	if (!load_config())
		return false;

	if (!load_symbols())
		return false;

	if (!load_timeframes())
		return false;

	if (!load_rootCert())
		return false;

	boost::system::error_code ec;


	asio_ssl::host_name_verification verifier(m_sBinanceHost);
	m_sslctx.set_verify_mode(asio_ssl::verify_peer);
	m_sslctx.set_verify_callback(verifier);

	if (!request_apiData()) {
		return false;
	}

	m_thrdFlag.setThreadReady();


	return true;
}



bool CBinanceApi::request_apiData()
{
	string path = "/stream?streams=";
	
	std::map<std::string, long>::iterator it;
	int i = 0;
	for (it=m_mapSymbols.begin(); it!=m_mapSymbols.end(); ++it,i++)
	{
		string symbol = (*it).first; 

		std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);

		for (auto itTimeframe : m_vecTimeframe)
		{
			path += symbol + PATH_KLINE + itTimeframe + "/";
		}
	}

	path.pop_back();


	gCommon.log(INFO, TRUE, "Stream Path(%s)", path.c_str());


	boost::system::error_code ec;

	tcp::resolver resolver{ m_ioContext };
	auto const results = resolver.resolve(m_sBinanceHost, m_sBinancePort, ec);
	if (ec) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::request_apiData]WebSocket resolve error:%s", ec.message().c_str());
		return false;
	}

	bool bRet = true;

	try
	{
		asio::connect(m_webSockets->next_layer().next_layer(), results.begin(), results.end());
		gCommon.log(INFO, TRUE, "connect to binance(%s)(%s)", m_sBinanceHost.c_str(), m_sBinancePort.c_str());

		m_webSockets->next_layer().handshake(asio_ssl::stream_base::client, ec);

		m_webSockets->set_option(beast_websocket::stream_base::decorator
		(
			[](beast_websocket::request_type& req)
			{
				req.set(beast_http::field::user_agent,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-client-coro");
			})
		);

		
		boost::system::error_code ec;
		m_webSockets->handshake(m_sBinanceHost, path, ec);
		if (ec)
		{
			bRet = false;
			gCommon.log(ERR, TRUE, "[CBinanceApi::request_apiData]WebSocket handshake error:%s", ec.message().c_str());
		}
		else
		{
			gCommon.log(INFO, TRUE, "WebSocket handshake OK");
		}


		m_webSockets->control_callback(
			[&](boost::beast::websocket::frame_type kind, boost::beast::string_view payload)
			{
				if (kind == boost::beast::websocket::frame_type::ping)
				{
					try {
						std::string receivedPayload(payload.data(), payload.size());
						gCommon.log(INFO, TRUE, "[Binance WebSocket] Received PING with payload(%s)", receivedPayload.c_str());

						boost::beast::websocket::ping_data pongPayload;
						pongPayload.assign(payload.data(), payload.size());
						m_webSockets->pong(pongPayload);  // 동일한 payload로 PONG 응답
						gCommon.log(INFO, TRUE, "[Binance WebSocket] Received PING, Sent PONG...");
					}
					catch (const std::exception& e) {
						gCommon.log(ERR, TRUE, "[Binance WebSocket] Error sending PONG: %s", e.what());
					}
				}
			});

		gCommon.log(INFO, TRUE, "[Binance WebSocket] Receivedm_webSockets->control_callback");
	}
	catch (const boost::system::system_error& ec)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[CBinanceApi::request_apiData]WebSocket connect error:%s", ec.what());
	}
	catch (const std::exception& e)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[CBinanceApi::request_apiData]WebSocket unexpected error:%s", e.what());
	}
	catch (...)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[CBinanceApi::request_apiData]WebSocket unknown error");
	}

	if (bRet)
		gCommon.log(INFO, TRUE, "WebSocket succeeded in requesting(%s)(%s)", m_sBinanceHost.c_str(), path.c_str());


	return bRet;
}




void CBinanceApi::threadFunc_recv_apiData()
{
	while (!m_thrdFlag.isReady())
	{
		if (!m_thrdFlag.isRunning()) return;
		std::this_thread::yield();
	}

	boost::beast::flat_buffer buffRecv;
	char zPacket[MSG_SIZE];

	int cnt_tick = 0, cnt_quote = 0;

	while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_dbSave->get_worker_cnt() == 0)
		{
			gCommon.log(ERR, TRUE, "유효한 DB Worker 가 없어서 프로세스를 종료합니다.");
			SetEvent(g_hDieEvent);	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Terminate Process
			return;
		}
		

		buffRecv.clear();
		boost::system::error_code ec;

		if (!m_webSockets.has_value())
		{
			gCommon.log(ERR, TRUE, "[CBinanceApi::threadFunc_recv_apiData]m_webSockets.has_value=fasle. Exiting thread...");
			break;
		}

		//  비동기 읽기 실행 (핸들러를 일반 함수로 변경)
		if (!m_webSockets->is_open()) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::threadFunc_recv_apiData]m_webSockets->is_open()=false. Exiting thread...");
			break;
		}

		m_webSockets->read(buffRecv, ec);
		if (ec)
		{
			if (ec == beast_websocket::error::closed)
			{
				gCommon.log(INFO, TRUE, "[CBinanceApi::threadFunc_recv_apiData]WebSocket closed, exiting thread...");
				break;
			}
			continue;
		}
		if (!m_thrdFlag.isRunning() || !m_thrdFlag.isReady()) {
			gCommon.log(ERR, TRUE, "[CBinanceApi::threadFunc_recv_apiData]스레드 종료 Flag-2");
			break;
		}
			

		std::string sRecvData(boost::beast::buffers_to_string(buffRecv.data()));
		ZeroMemory(zPacket, sizeof(zPacket));

		//gCommon.log(INFO, TRUE, "[RECV](%s)", sRecvData.c_str());

		//
		m_dbSave->push_data(sRecvData);
		//
	}

	gCommon.log(INFO, TRUE, "Exiting WebSocket read thread...==> kill this process");
	SetEvent(g_hDieEvent);
}


