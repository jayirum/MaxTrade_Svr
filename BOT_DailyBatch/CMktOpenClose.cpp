#include "CMktOpenClose.h"
#include "CGlobals.h"

extern HANDLE	g_hDieEvent;
extern CGlobals		gCommon;

CMktOpenClose::CMktOpenClose() : m_bStartCancel(false)
{ 
	cancelOrders_set_wait();

	m_thrdFlag.set_idle();
	m_odbcSel = std::make_unique<CODBC>(DBMS_TYPE::MYSQL);
}

CMktOpenClose::~CMktOpenClose()
{ 
	m_thrdFlag.set_stop();

	if (m_thrdSelect.joinable())	m_thrdSelect.join();
	if (m_thrdCnclOrd.joinable())	m_thrdCnclOrd.join();
}

bool CMktOpenClose::Initialize()
{ 
	//----------------------------------------------------------------------------------------------------//
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 }, zPingTimeout[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PING_TIMEOUT_SEC", zPingTimeout)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CMktOpenClose::Initialize]DB정보 조회 오류.INI파일을 확인하세요");
		return false;
	}
	char zConnStr[512] = { 0 };
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);
	m_sConnStr = zConnStr;

	m_dbPing_timeout = atoi(zPingTimeout);

	//----------------------------------------------------------------------------------------------------//	

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY", zDsn))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CMktOpenClose::Initialize]INI 에서 DB_RECONN_TRY 확인 필요");
		return false;
	}
	m_dbReconnTry = atoi(zDsn);

	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"TIMEOUT_MS_MARKET_SEL", zDsn))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CMktOpenClose::Initialize]INI 에서 TIMEOUT_MS_MARKET_SEL 확인 필요");
		return false;
	}
	m_timeout_ms_select = atoi(zDsn);

	//----------------------------------------------------------------------------------------------------//
	string arg = "Initailze";
	if (!dbConnect(m_odbcSel.get(), arg) )
		return false;

	m_thrdSelect = std::thread(&CMktOpenClose::threadFunc_Select, this);
	m_thrdCnclOrd = std::thread(&CMktOpenClose::threadFunc_CnclWorker, this);

	//----------------------------------------------------------------------------------------------------//

	m_thrdFlag.set_run();

	return true;
}

void CMktOpenClose::threadFunc_Select()
{
	__try
	{
		threadFunc_Select_Internal();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, TRUE, "--------------------------------------------");
		gCommon.log(ERR, TRUE, "EXCEPTION in CMktOpenClose::threadFunc_Select");
		gCommon.log(ERR, TRUE, "--------------------------------------------");
	}
}

void CMktOpenClose::threadFunc_Select_Internal()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));

	SYSTEMTIME st;
	char now_time[128];

	char zQ[1024];
	string symbol, start_tm, end_tm, event_tm;
	long mkt_tp;
	sprintf(zQ, " CALL spbatch_get_mkt_time();");

	while (!m_thrdFlag.is_stopped())
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(m_timeout_ms_select));

		//------------------------------------------------------------------------------------------------//

		bool bNeedReconn;
		m_odbcSel->Init_ExecQry(zQ);
		if (!m_odbcSel->Exec_Qry(bNeedReconn))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[장운영 시간 조회 실패](%s)(%s)", zQ, m_odbcSel->getMsg());
			m_odbcSel->DeInit_ExecQry();

			if (bNeedReconn) {
				string arg = "ThreadSelect";
				if (!reconnectDB(m_odbcSel.get(), arg))
				{
					gCommon.log(LOGTP_ERR, TRUE, "[장운영 시간 조회 실패 후 reconnect 실패!!!]");;
				}
				else
				{
					gCommon.log(LOGTP_ERR, TRUE, "[장운영 시간 조회 실패 후 reconnect 성공***]");
				}
			}
			continue;
		}

		//------------------------------------------------------------------------------------------------//

		while (m_odbcSel->GetNextData())
		{
			m_odbcSel->GetDataStr(1, &symbol);
			m_odbcSel->GetDataLong(2, &mkt_tp);
			m_odbcSel->GetDataStr(3, &start_tm);
			m_odbcSel->GetDataStr(4, &end_tm);

			GetLocalTime(&st); sprintf(now_time, "%02d:%02d", st.wHour, st.wMinute);

			if (start_tm == end_tm) {
				gCommon.log(ERR, TRUE, "장개시 시간과 장마감 시간이 동일(%s, %s). 처리하지 않음", start_tm.c_str(), end_tm.c_str());
				continue;
			}

			MARKET_ACTION mktAction = MARKET_ACTION::MARKET_NONE;

			if (mkt_tp == 1 && end_tm.compare(now_time) == 0)		// now market is open ==> close
			{
				mktAction = MARKET_ACTION::MARKET_CLOSE;
				event_tm = end_tm;
			}
			if (mkt_tp == 0 && start_tm.compare(now_time) == 0)		// now market is close ==> open
			{
				mktAction = MARKET_ACTION::MARKET_OPEN;
				event_tm = start_tm;
			}

			if (mktAction != MARKET_ACTION::MARKET_NONE)
			{
				m_deqMktFlags.push_back(std::make_unique<TMarketFlag>(symbol, mktAction));
			}
		} // while (m_odbcSel->GetNextData())
		m_odbcSel->DeInit_ExecQry();


		dbProc_marketFlag();	//------------------------------------------------------------------------// update mkt_tp

		m_deqMktFlags.clear();

		cancleOrders_set_start();


		//

	} // while (!m_thrdFlag.is_stopped())

	gCommon.log(INFO, TRUE, "CMktOpenClose::threadFunc_Select is exiting");
	SetEvent(g_hDieEvent);
}

void CMktOpenClose::dbProc_marketFlag()
{
	if (m_deqMktFlags.empty())
		return;

	char zQ[1024];
	char action_s[128];
	for (auto& it : m_deqMktFlags)
	{
		if (m_thrdFlag.is_stopped())	return;

		string symbol = it->symbol;
		MARKET_ACTION action = it->action;
		market_action_s(action, action_s);
		sprintf(zQ, "call spbatch_set_market_open_close('%s', %d)", symbol.c_str(), (int)action);
		gCommon.log(INFO, TRUE, zQ);


		bool bNeedReconn;
		m_odbcSel->Init_ExecQry(zQ);
		if (!m_odbcSel->Exec_Qry(bNeedReconn))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[%s 오류!!!](%s)(%s)", action_s, zQ, m_odbcSel->getMsg());
			m_odbcSel->DeInit_ExecQry();
			Sleep(1000);

			if (bNeedReconn) {
				if (!reconnectDB(m_odbcSel.get(), string("threadFunc_MktWorker"))) {
					gCommon.log(ERR, TRUE, "[%s 오류 후 reconnect 오류!!!");
				}
				else {
					gCommon.log(INFO, TRUE, "[%s 오류 후 reconnect 성공***");
				}
			}
			continue;
		}
		if (action == MARKET_ACTION::MARKET_CLOSE) 
		{
			gCommon.log(INFO, TRUE, "[장마감 처리 성공](%s)(%s)", symbol.c_str(), zQ);
			std::lock_guard<mutex> lock(m_mtxCnclOrders);

			m_deqCancelOrders.push_back(symbol);	//--------------------------------------------------------// 장마감 취소 처리
		}
		else if (action == MARKET_ACTION::MARKET_OPEN) 
		{
			gCommon.log(INFO, TRUE, "[장개시 처리 성공](%s)(%s)", symbol.c_str(), zQ);
		}
		m_odbcSel->DeInit_ExecQry();

	} // for (auto& it : m_deqMktFlags)
	
}


void CMktOpenClose::threadFunc_CnclWorker()
{
	__try
	{
		threadFunc_CnclWorker_Internal();
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, TRUE, "---------------------------------------------");
		gCommon.log(ERR, TRUE, "Exception in CMktOpenClose::threadFunc_CnclWorker");
		gCommon.log(ERR, TRUE, "---------------------------------------------");
	}
}


void CMktOpenClose::threadFunc_CnclWorker_Internal()
{
	//CODBC odbc(DBMS_TYPE::MYSQL);
	//if (!reconnectDB(&odbc, string("threadFunc_CnclWorker"))) {
	//	return;
	//}


	char zQ[1024];
	while (!m_thrdFlag.is_stopped())
	{
		//odbc.PingConnection();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_thrdFlag.is_idle()) continue;

		if (!cancelOrders_is_time_to_do())	continue;

		std::lock_guard<std::mutex> lock(m_mtxCnclOrders);
		if (m_deqCancelOrders.empty())
		{
			continue;
		}

		CODBC odbc(DBMS_TYPE::MYSQL);
		if (!reconnectDB(&odbc, string("threadFunc_CnclWorker"))) {
			Sleep(5000);
			continue;
		}


		for (auto& it : m_deqCancelOrders)
		{
			if (m_thrdFlag.is_stopped())
				return;

			string symbol = it;

			sprintf(zQ, "call spbatch_market_close_cancel_orders('%s')", symbol.c_str());
			gCommon.log(INFO, TRUE, "[장마감취소주문쿼리](%s)", zQ);

			bool bNeedReconn;
			odbc.Init_ExecQry(zQ);
			int loop = 0;
			while (!odbc.Exec_Qry(bNeedReconn))
			{
				gCommon.log(LOGTP_ERR, TRUE, "[장마감취소 오류!!!-%s](%s)(%s)", zQ, odbc.getMsg());
				odbc.DeInit_ExecQry();
				Sleep(1000);

				if (bNeedReconn)
				{
					if (reconnectDB(&odbc, string("threadFunc_CnclWorker")))
					{
						gCommon.log(INFO, TRUE, "[장마감취소 오류 후 재연결 시도 성공***-%s]", symbol.c_str());
					}
					else {
						gCommon.log(ERR, TRUE, "[장마감취소 오류 후 재연결 시도 오류!!!-%s]", symbol.c_str());
					}
				}

				if (++loop > 10)
					break;
				continue;
			}
			gCommon.log(INFO, TRUE, "[장마감 주문취소 성공***](%s)(%s)", symbol.c_str(), zQ);
			odbc.DeInit_ExecQry();
		} // for (auto& it : m_deqCancelOrders)

		m_deqCancelOrders.clear();

		cancelOrders_set_wait();


	} // while (!m_thrdFlag.is_stopped())

	gCommon.log(INFO, TRUE, "CMktOpenClose::threadFunc_CnclWorker exiting...");
}

bool CMktOpenClose::dbConnect(CODBC* p, string thrdName)
{
	if (!p->Initialize(m_dbPing_timeout))
	{
		gCommon.log(LOGTP_ERR, TRUE, p->getMsg());
		return false;
	}
	if (!p->Connect(m_sConnStr.c_str()))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[%s]DB Connect 오류:%s", thrdName.c_str(), p->getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[CMktOpenClose::dbConnectt] (%s) DB Connect 성공", thrdName.c_str());
	return true;
}

bool CMktOpenClose::reconnectDB(CODBC* p, string thrdName)
{
	bool conn = false;
	for (int i = 0; i < m_dbReconnTry; i++)
	{
		p->DeInitialize();

		if (dbConnect(p, thrdName)) {
			conn = true;
			break;
		}
		gCommon.log(ERR, TRUE, "[%s]DB 재연결 실패(%d)", thrdName.c_str(), i);
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	return conn;
}