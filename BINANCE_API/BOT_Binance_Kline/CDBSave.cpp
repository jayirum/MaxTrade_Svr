#include "CDBSave.h"
#include "CGlobals.h"
#include <nlohmann/json.hpp>

extern HANDLE		g_hDieEvent;
extern CGlobals		gCommon;

CDBSave::CDBSave()
{ 
	m_thrdFlag.set_idle();
}

CDBSave::~CDBSave()
{
	m_thrdFlag.set_stop();

	m_cvQ.notify_all();
	for (int i = 0; i < m_vecWorkers.size(); i++)
	{
		if (m_vecWorkers[i].joinable())	m_vecWorkers[i].join();
	}
	gCommon.debug("All DB Workers finished");
}

bool CDBSave::create_workers(int num)
{
	//----------------------------------------------------------------------------------------------------//

	char zMaxCnt[128] = { 0 };
	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"MAX_DATACNT_IN_Q", zMaxCnt)) {
		gCommon.log(ERR, TRUE, "Failed to read [MAX_DATACNT_IN_Q] in INI file");
		return false;
	}
	m_maxDataCnt = atoi(zMaxCnt);

	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY", zMaxCnt)) {
		gCommon.log(ERR, TRUE, "Failed to read [DB_RECONN_TRY] in INI file");
		return false;
	}
	m_dbReconnTry = atoi(zMaxCnt);
	
	
	//----------------------------------------------------------------------------------------------------//

	for (int i = 0; i < num; i++)
	{
		m_vecWorkers.emplace_back(&CDBSave::threadFunc_Save, this, i);

		if (!connect_db(i))
			return false;
	}
	m_nWorkerCnt = num;

	m_thrdFlag.set_run();

	//----------------------------------------------------------------------------------------------------//
	
	return true;
}


bool CDBSave::connect_db(int idx)
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 }, zPingTimeout[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PING_TIMEOUT_SEC", zPingTimeout)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "Failed to read DB Information in INI file");
		return FALSE;
	}

	sprintf(m_zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	m_pingTimeout_sec = atoi(zPingTimeout);

	CODBC* odbc = new CODBC(DBMS_TYPE::MYSQL);
	if (!odbc->Initialize(m_pingTimeout_sec))
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc->getMsg());
		return false;
	}
	if (!odbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::connect_db] Failed to connect DB:%s", odbc->getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[CDBSave::connect_db]DB Connected successfully[%d]", idx);
	m_vecDBs.push_back(odbc);
	return true;
}


bool CDBSave::reconnect_db(int idx)
{
	bool bReconn = false;

	for (unsigned int i = 0; i < m_dbReconnTry; i++)
	{
		m_vecDBs[idx]->DeInitialize();

		if (!m_vecDBs[idx]->Initialize(m_pingTimeout_sec))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[IDX:%d][reconnect_db]Failed to Initialize-(%s)", idx, m_vecDBs[idx]->getMsg());
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		}
		if (!m_vecDBs[idx]->Connect(m_zConnStr))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[IDX:%d[[reconnect_db]Failed to Connect-(%s)", idx, m_vecDBs[idx]->getMsg());
			std::this_thread::sleep_for(std::chrono::seconds(30));
			continue;
		}

		bReconn = true;
		break;
	}	
	return bReconn;
}


void CDBSave::push_data(std::shared_ptr< TConvertedData> cvt)
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


void CDBSave::compose_sp_param(deque<shared_ptr< TConvertedData>>& cvt, _Out_ string& query)
{
	int nCnt = (int)cvt.size();
	char t[2048], z[2048];
	for (int i = 0; i < PARAM_ARRAY_CNT; i++)
	{
		if (i < nCnt) {
			sprintf(t,
				"'%s'"	// in i_symbol_cd varchar(10)
				",'%s'"	// in i_timeframe char(2)
				",'%.13s'"	// in i_candle_tm_s char(13)
				",'%.15s'"	// in i_candle_tm_e char(13)
				",'%s'"	// in i_o varchar(20)
				",'%s'"	// in i_h varchar(20)
				",'%s'"	// in i_c varchar(20)"
				",'%s'"	// in i_l varchar(20)
				, cvt[i]->symbol.c_str()
				, cvt[i]->interval.c_str()
				, cvt[i]->time_s.c_str()
				, cvt[i]->time_e.c_str()
				, cvt[i]->o.c_str()
				, cvt[i]->h.c_str()
				, cvt[i]->l.c_str()
				, cvt[i]->c.c_str()
			);
		}
		else
		{
			sprintf(t, "'','','','','','','',''");
		}

		if (i == 0) {
			sprintf(z, "CALL sp_save_candle_2 (%d, %s", (int)nCnt, t);
			query = z;
		}
		else {
			sprintf(z, ",%s", t);
			query += z;
		}
	}
	query += ");";	
}


void CDBSave::threadFunc_Save(int idx)
{
	__try
	{
		threadFunc_Save_Internal(idx);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
		gCommon.log(ERR, TRUE, "unexpected exception in CDBSave::threadFunc_Save(%d)", idx);
		gCommon.log(ERR, FALSE, "----------------------------------------------------------------");
	}
}
void CDBSave::threadFunc_Save_Internal(int idx)
{
	deque<shared_ptr< TConvertedData>>	deqCvts;
	while ( !m_thrdFlag.is_stopped() )
	{
		std::this_thread::sleep_for(chrono::milliseconds(1));

		if (m_thrdFlag.is_idle()) continue;
		
		m_vecDBs[idx]->PingConnection();

		//------------------------------------------------------------------------------------------------//

		std::unique_lock<std::mutex> lock(m_mtxQ);
		m_cvQ.wait(lock, [this] { return !m_Q.empty() || m_thrdFlag.is_stopped(); });

		if (m_thrdFlag.is_stopped())
			break;

		int nCnt = 0;
		while (m_Q.size() > 0 && nCnt < PARAM_ARRAY_CNT)
		{
			deqCvts.push_back(std::move(m_Q.front()));
			m_Q.pop_front();
			nCnt++;
		}
		lock.unlock();
		
		// sp ±¸¼º ----------------------------------------------------------------------------------------//
		string query;
		compose_sp_param(deqCvts, query);
		gCommon.log(INFO, FALSE, "[IDX:%d][Saving CNT:%d](%s)", idx, nCnt, query.c_str());
		deqCvts.clear();
		
		//------------------------------------------------------------------------------------------------//
		
		bool bNeedReconn;
		m_vecDBs[idx]->Init_ExecQry( (char*)query.c_str());
		if (!m_vecDBs[idx]->Exec_Qry(bNeedReconn)) 
		{
			gCommon.log(LOGTP_ERR, TRUE, "[IDX:%d][CDBSave::threadFunc_Save]DB Exec_Qry ERROR(%s)<%s>", 
				idx, m_vecDBs[idx]->getMsg(), (char*)query.c_str());

			m_vecDBs[idx]->DeInit_ExecQry();

			if (!reconnect_db(idx))
			{
				gCommon.log(LOGTP_ERR, TRUE, "[IDX:%d][CDBSave::threadFunc_Save] Failed to reconnect_db", idx);
				break;
			}
			else
			{
				gCommon.log(INFO, TRUE, "[IDX:%d][CDBSave::threadFunc_Save] DB reconnected fuccessfully", idx);
			}

			continue;
		}

		if (m_vecDBs[idx]->GetNextData())
		{
			long ret_code = 0;

			m_vecDBs[idx]->GetDataLong(1, &ret_code);

			if (ret_code != 0)
			{
				gCommon.log(LOGTP_ERR, TRUE, "[IDX:%d][CDBSave::threadFunc_Save]SP return error(%s)", idx, m_vecDBs[idx]->getMsg());

			}
		}
		m_vecDBs[idx]->DeInit_ExecQry();
		gCommon.debug("[IDX:%d][SAVE OK](%s)", idx, (char*)query.c_str());
		
		//------------------------------------------------------------------------------------------------//
		
		
	}
	gCommon.log(INFO, TRUE, "[IDX:%d][DB Thread exiting]", idx);
	
	--m_nWorkerCnt;
}
