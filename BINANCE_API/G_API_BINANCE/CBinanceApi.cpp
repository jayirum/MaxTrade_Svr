#include "CBinanceApi.h"
#include "CGlobals.h"
#include "../../Common/Util.h"
#include "../../Common/CODBC.h"
#include "../../Common/MaxTradeInc.h"
#include "Certificate.h"
#include "CSenderQList.h"

#include <fstream>
#include <iostream>
#include <iomanip>


extern CGlobals		gCommon;
extern CSenderQList	gQList;

CBinanceApi::CBinanceApi():m_sslctx(asio_ssl::context::tlsv12)
{
	for (int i = 0; i < NUM_DATA_TYPE; i++)
	{
		m_webSockets[i].emplace(m_ioContext, m_sslctx);
	}

	m_thrdFlag.set_idle();
	for (int i = 0; i < NUM_DATA_TYPE; i++){
		m_thrdRecv[i] = std::thread(&CBinanceApi::threadFunc_recv_apiData, this, static_cast<DATA_TYPE>(i));
	}
}

CBinanceApi::~CBinanceApi()
{
	DeInitialize();
}


void CBinanceApi::DeInitialize()
{
	m_thrdFlag.set_stop();

	for (int i = 0; i < NUM_DATA_TYPE; i++)
	{
		if (m_webSockets[i].has_value())
		{
			if (m_webSockets[i]->is_open())
			{
				boost::system::error_code ec;
				m_webSockets[i]->next_layer().next_layer().cancel();
			}
		}
	}
	for (int i = 0; i < NUM_DATA_TYPE; i++)
	{
		if (m_thrdRecv[i].joinable()) {
			m_thrdRecv[i].join();

			if (m_webSockets[i].has_value()) {
				boost::system::error_code ec;
				m_webSockets[i]->close(beast_websocket::close_code::normal, ec);
				m_webSockets[i].reset();
			}
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

	odbc.Init_ExecQry(zQ);
	bool bNeedConn;
	if (!odbc.Exec_Qry(bNeedConn)) {
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
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

		m_mapSymbols[zSymbol] = dotCnt;

		gCommon.log(INFO, TRUE, "[load_symbols](%s)(DotCnt:%d)", zSymbol, dotCnt);
		
		Sleep(1);
	}
	odbc.DeInit_ExecQry();

	if (m_mapSymbols.size() == 0)
	{
		gCommon.log(ERR, TRUE, "[load_symbols 실패](%s)", zQ);
		return false;
	}

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
		gCommon.log(INFO, TRUE, "[load_rootCert 성공]");
	return ret;
}


bool CBinanceApi::load_config()
{
	char zSection[] = "BINANCE_INFO";
	char zVal[128] = { 0 };
	
	if (gCommon.getConfig(zSection, (char*)"HOST", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST(%s)", zSection);
		return false;
	}
	m_sBinanceHost = zVal;

	ZeroMemory(zVal, sizeof(zVal));
	if (gCommon.getConfig(zSection, (char*)"PORT", zVal) == NULL) {
		gCommon.log(ERR, TRUE, "[CBinanceApi::load_config]Failed to read config file - HOST(%s)", zSection);
		return false;
	}
	m_sBinancePort = zVal;

	gCommon.log(INFO, TRUE, "[load_config 성공]HOST(%s), Port(%s)",
		m_sBinanceHost.c_str(), m_sBinancePort.c_str());

	return true;
}

bool CBinanceApi::Initialize()
{
	if (!load_config())
		return false;

	if (!load_symbols())
		return false;

	if( !load_rootCert() )
		return false;


	boost::system::error_code ec;


	asio_ssl::host_name_verification verifier(m_sBinanceHost);
	m_sslctx.set_verify_mode(asio_ssl::verify_peer);
	m_sslctx.set_verify_callback(verifier);
	
	for (int i=0; i< NUM_DATA_TYPE; i++)
	{
		if (!request_apiData(static_cast<DATA_TYPE>(i))) {
			return false;
		}
	}
	
	m_thrdFlag.set_run();

	return true;
}


/*
/ws/adausdt@ticker/bnbusdt@ticker/btcusdt@ticker/dogeusdt@ticker/ethusdt@ticker/solusdt@ticker/trxusdt@ticker/xrpusdt@ticker
*/
bool CBinanceApi::request_apiData(DATA_TYPE type)
{
	string path = "/ws/";
	char tempType[128];
	
	std::map<std::string, long>::iterator it;
	int i = 0;
	for (it=m_mapSymbols.begin(); it!=m_mapSymbols.end(); ++it,i++)
	{
		string symbol = (*it).first; 

		std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);

		if(type== DATA_TYPE::TICK)
			path += symbol + PATH_TICK;
		else 
			path += symbol + PATH_BOOK;

		if (i < m_mapSymbols.size() - 1) {
			path += "/";  // 여러 개의 심볼을 하나의 WebSocket에서 구독
		}

	}

	gCommon.log(INFO, TRUE, "[%s]Symbol Path for Binance(%s)", type_s(type, tempType), path.c_str());


	boost::system::error_code ec;

	tcp::resolver resolver{ m_ioContext };
	auto const results = resolver.resolve(m_sBinanceHost, m_sBinancePort, ec);
	if (ec) {
		gCommon.log(ERR, TRUE, "[%s][CBinanceApi::request_apiData]WebSocket resolve error:%s", type_s(type, tempType), ec.message().c_str());
		return false;
	}

	bool bRet = true;

	try
	{
		asio::connect(m_webSockets[(int)type]->next_layer().next_layer(), results.begin(), results.end());
		gCommon.log(INFO, TRUE, "[%s][Connect Binane 성공](%s)(%s)", type_s(type, tempType), m_sBinanceHost.c_str(), m_sBinancePort.c_str());

		m_webSockets[(int)type]->next_layer().handshake(asio_ssl::stream_base::client, ec);

		m_webSockets[(int)type]->set_option(beast_websocket::stream_base::decorator
		(
			[](beast_websocket::request_type& req)
			{
				req.set(beast_http::field::user_agent,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-client-coro");
			})
		);

		boost::system::error_code ec;
		m_webSockets[(int)type]->handshake(m_sBinanceHost, path, ec);
		if (ec)
		{
			bRet = false;
			gCommon.log(ERR, TRUE, "[%s][CBinanceApi::request_apiData]WebSocket handshake error:%s",type_s(type,tempType), ec.message().c_str());
		}
		else
		{
			gCommon.log(INFO, TRUE, "[%s][WebSocket handshake 성공]", type_s(type, tempType));

			m_webSockets[(int)type]->control_callback(
				[&](boost::beast::websocket::frame_type kind, boost::beast::string_view payload)
				{
					if (kind == boost::beast::websocket::frame_type::ping)
					{
						try {
							std::string receivedPayload(payload.data(), payload.size());
							boost::beast::websocket::ping_data pongPayload;
							pongPayload.assign(payload.data(), payload.size());
							m_webSockets[(int)type]->pong(pongPayload);  // 동일한 payload로 PONG 응답
						}
						catch (const std::exception& e) {
							gCommon.log(ERR, TRUE, "[%s][Binance WebSocket] Error sending PONG: %s", type_s(type, tempType), e.what());
						}
					}
				});
			gCommon.log(INFO, TRUE, "[%s][Binance WebSocket] Received_webSockets->control_callback", type_s(type, tempType));
		}
	}
	catch (const boost::system::system_error& ec)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[%s][CBinanceApi::request_apiData]WebSocket connect error:%s", type_s(type, tempType), ec.what());
	}
	catch (const std::exception& e)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[%s][CBinanceApi::request_apiData]WebSocket unexpected error:%s", type_s(type, tempType), e.what());
	}
	catch (...)
	{
		bRet = false;
		gCommon.log(ERR, TRUE, "[%s][CBinanceApi::request_apiData]WebSocket unknown error", type_s(type, tempType));
	}

	if (bRet)
		gCommon.log(INFO, TRUE, "[%s][Binance 로 WebSocket 통한 요청 성공](%s)(%s)", type_s(type, tempType), m_sBinanceHost.c_str(), path.c_str());


	return bRet;
}




void CBinanceApi::threadFunc_recv_apiData(DATA_TYPE type)
{
	boost::beast::flat_buffer buffRecv;
	char zPacket[MSG_SIZE];

	int cnt_tick = 0, cnt_quote = 0;
	char tempType[128];

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_thrdFlag.is_idle()) continue;

		buffRecv.clear();
		boost::system::error_code ec;

		if (!m_webSockets[(int)type].has_value())
		{
			gCommon.log(INFO, TRUE, "[%s][CBinanceApi::threadFunc_recv_apiData]WebSocket is closed. Exiting thread...", type_s(type, tempType));
			break;
		}

		//  비동기 읽기 실행 (핸들러를 일반 함수로 변경)
		if (!m_webSockets[(int)type]->is_open())
			break;

		m_webSockets[(int)type]->read(buffRecv, ec);
		if (ec)
		{
			if (ec == beast_websocket::error::closed)
			{
				gCommon.log(INFO, TRUE, "[%s][CBinanceApi::threadFunc_recv_apiData]WebSocket closed, exiting thread...", type_s(type, tempType));
				break;
			}
			continue;
		}
		if (m_thrdFlag.is_stopped()) break;

		std::string sRecvData(boost::beast::buffers_to_string(buffRecv.data()));
		ZeroMemory(zPacket, sizeof(zPacket));

		if (type == DATA_TYPE::TICK) 
		{
			if (++cnt_tick == 1000) {
				gCommon.debug("[RECV_TICK](%s)", sRecvData.c_str());
				cnt_tick = 0;
			}
			__MAX::compose_ra001_from_binance(sRecvData, m_mapSymbols, zPacket);
		}
		else
		{
			__MAX::compose_ra002_from_binance(sRecvData, zPacket);
			if (++cnt_quote == 10000) {
				gCommon.debug("[RECV_QUOTE](%s)", sRecvData.c_str());
				cnt_quote = 0;
			}
		}
		gQList.enQueue_to_allSenders(zPacket);
	}

	gCommon.log(INFO, TRUE, "[%s]Exiting WebSocket read thread...", type_s(type, tempType));
}

