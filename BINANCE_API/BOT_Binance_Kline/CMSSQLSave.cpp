#include "CMSSQLSave.h"
#include "CGlobals.h"
#include <nlohmann/json.hpp>

extern HANDLE		g_hDieEvent;
extern CGlobals		gCommon;

CMSSQLSave::CMSSQLSave()
{ 
	m_pOdbc = NULL;
	m_thrdFlag.set_idle();
}

CMSSQLSave::~CMSSQLSave()
{
	m_thrdFlag.set_stop();

	m_cvQ.notify_all();
	for (int i = 0; i < m_vecWorkers.size(); i++)
	{
		if (m_vecWorkers[i].joinable())	m_vecWorkers[i].join();
	}
	gCommon.debug("All MSSQL Workers finished");

	delete m_pOdbc;
}

bool CMSSQLSave::create_workers(int num)
{
#ifdef __DEV
	return true;
#endif

	char zMaxCnt[128] = { 0 };
	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY", zMaxCnt)) {
		gCommon.log(ERR, TRUE, "Failed to read [DB_RECONN_TRY] in INI file");
		return false;
	}
	m_dbReconnTry = atoi(zMaxCnt);
	
	if (!connect_db())
		return false;

	//----------------------------------------------------------------------------------------------------//

	for (int i = 0; i < num; i++)
	{
		m_vecWorkers.emplace_back(&CMSSQLSave::threadFunc_Save, this, i);
	}
	m_nWorkerCnt = num;

	

	m_thrdFlag.set_run();

	//----------------------------------------------------------------------------------------------------//
	
	return true;
}


bool CMSSQLSave::connect_db()
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 }, zPingTimeout[128] = { 0 };

	if (!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"PWD", zPwd) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"PING_TIMEOUT_SEC", zPingTimeout)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "Failed to read DB Information in INI file");
		return FALSE;
	}

	sprintf(m_zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	m_pingTimeout_sec = atoi(zPingTimeout);

	m_pOdbc = new CODBC(DBMS_TYPE::MSSQL);
	if (!m_pOdbc->Initialize(m_pingTimeout_sec))
	{
		gCommon.log(LOGTP_ERR, TRUE, m_pOdbc->getMsg());
		return false;
	}
	if (!m_pOdbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CMSSQLSave::connect_db] Failed to connect DB:%s", m_pOdbc->getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[CMSSQLSave::connect_db]DB Connected successfully");
	return true;
}


bool CMSSQLSave::reconnect_db()
{
	m_pOdbc->DeInitialize();

	if (!m_pOdbc->Initialize(m_pingTimeout_sec))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[reconnect_db]Failed to Initialize-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return false;
	}
	if (!m_pOdbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[reconnect_db]Failed to Connect-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(30));
		return false;;
	}

	return true;
}


void CMSSQLSave::push_data(std::shared_ptr< TConvertedData> cvt)
{
	if (m_nWorkerCnt == 0) {
		gCommon.log(LOGTP_ERR, TRUE, "Terminate process as all the workers are not alive.");
		SetEvent(g_hDieEvent);
		return;
	}

	std::lock_guard<mutex>lock(m_mtxQ);
	m_Q.push_back(std::move(cvt));
	m_cvQ.notify_one();
}


void CMSSQLSave::threadFunc_Save(int idx)
{
	__try
	{
#ifndef __DEV
		threadFunc_Save_Internal(idx);
#endif
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
		gCommon.log(ERR, TRUE, "unexpected exception in CMSSQLSave::threadFunc_Save(%d)", idx);
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
	}
}
void CMSSQLSave::threadFunc_Save_Internal(int idx)
{
	deque<shared_ptr< TConvertedData>>	deqCvts;
	while ( !m_thrdFlag.is_stopped() )
	{
		std::this_thread::sleep_for(chrono::milliseconds(1));

		if (m_thrdFlag.is_idle()) continue;
		
		//m_vecDBs[idx]->PingConnection();

		//------------------------------------------------------------------------------------------------//

		std::unique_lock<std::mutex> lock(m_mtxQ);
		m_cvQ.wait(lock, [this] { return !m_Q.empty() || m_thrdFlag.is_stopped(); });

		if (m_thrdFlag.is_stopped())
			break;

		int nCnt = 0;
		while (m_Q.size() > 0)
		{
			deqCvts.push_back(std::move(m_Q.front()));
			m_Q.pop_front();
			nCnt++;
		}
		lock.unlock();
		
		//
		// sp »£√‚ ----------------------------------------------------------------------------------------//
		//
		Save_to_Db(deqCvts);
		//
		gCommon.log(INFO, FALSE, "[MSSQL Saving CNT:%d]", nCnt);
		deqCvts.clear();
		
		//------------------------------------------------------------------------------------------------//
		
	}
	gCommon.log(INFO, TRUE, "[MSSQL Thread exiting]");
	
	--m_nWorkerCnt;
}


void CMSSQLSave::Save_to_Db(deque<shared_ptr< TConvertedData>>& cvt)
{
	int nCnt = (int)cvt.size();

	char zQuery[2048];
	for (int i = 0; i < nCnt; i++)
	{
		if (m_thrdFlag.is_stopped())
			break;

		char zTableNm[128];
		sprintf(zTableNm, "CANDLE_%s", cvt[i]->symbol.c_str());

		ZeroMemory(zQuery, sizeof(zQuery));
		sprintf(zQuery,
			"EXEC sp_insert_candle "
			"'%s'"	//@i_table_nm      VARCHAR(128),
			",%d"	//@i_timeframe_cd  INT,
			",'%s'"	//@i_candle_tm     CHAR(13),
			",%.5f"	//@i_open_prc      DECIMAL(15, 5),
			",%.5f"	//@i_high_prc      DECIMAL(15, 5),
			",%.5f"	//@i_low_prc       DECIMAL(15, 5),
			",%.5f"	//@i_close_prc     DECIMAL(15, 5)
			",'%s'"	//@i_symbol_cd	varchar(20)
			, zTableNm
			, std::stoi(cvt[i]->interval)
			, cvt[i]->time_s.c_str()
			, std::stod(cvt[i]->o.c_str())
			, std::stod(cvt[i]->h.c_str())
			, std::stod(cvt[i]->l.c_str())
			, std::stod(cvt[i]->c.c_str())
			, cvt[i]->symbol.c_str()
		);

		bool bNeedReconn;
		m_pOdbc->Init_ExecQry(zQuery);
		if (!m_pOdbc->Exec_Qry(bNeedReconn))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[CMSSQLSave::Save_to_Db]DB Exec_Qry ERROR(%s)<%s>",
				m_pOdbc->getMsg(), zQuery);

			m_pOdbc->DeInit_ExecQry();

			if (!reconnect_db())
			{
				gCommon.log(LOGTP_ERR, TRUE, "[CMSSQLSave::Save_to_Db] Failed to reconnect_db");
				break;
			}
			else
			{
				gCommon.log(INFO, TRUE, "[CMSSQLSave::Save_to_Db] DB reconnected fuccessfully");
			}

			continue;
		}

		//if (m_pOdbc->GetNextData())
		//{
		//	long ret_code = 0;

		//	m_pOdbc->GetDataLong(1, &ret_code);

		//	if (ret_code != 0)
		//	{
		//		gCommon.log(LOGTP_ERR, TRUE, "[CMSSQLSave::Save_to_Db]SP return error(%s)", m_pOdbc->getMsg());

		//	}
		//}
		m_pOdbc->DeInit_ExecQry();
		gCommon.debug("[MSSQL SAVE OK](%s)", zQuery);

	}
}

