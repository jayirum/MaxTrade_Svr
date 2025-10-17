#include "CDBSave.h"
#include "CGlobals.h"
#include <nlohmann/json.hpp>

extern HANDLE		g_hDieEvent;
extern CGlobals		gCommon;

CDBSave::CDBSave() :m_bContinue(false)
{ }

CDBSave::~CDBSave()
{
	m_bContinue = false;
	gCommon.debug("m_bContinue false!!!");
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
		gCommon.log(ERR, TRUE, "MAX_DATACNT_IN_Q 가져오기 오류");
		return false;
	}
	m_maxDataCnt = atoi(zMaxCnt);

	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY", zMaxCnt)) {
		gCommon.log(ERR, TRUE, "DB_RECONN_TRY 가져오기 오류");
		return false;
	}
	m_dbReconnTry = atoi(zMaxCnt);
	m_bContinue = true;
	
	//----------------------------------------------------------------------------------------------------//

	for (int i = 0; i < num; i++)
	{
		m_vecWorkers.emplace_back(&CDBSave::threadFunc_Save, this, i);

		if (!connect_db(i))
			return false;
	}
	m_nWorkerCnt = num;

	//----------------------------------------------------------------------------------------------------//
	
	return true;
}


bool CDBSave::connect_db(int idx)
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::connect_db]DB정보 조회 오류.INI파일을 확인하세요");
		return FALSE;
	}

	sprintf(m_zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	CODBC* odbc = new CODBC(DBMS_TYPE::MYSQL);
	if (!odbc->Initialize())
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc->getMsg());
		return false;
	}
	if (!odbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::connect_db]DB Connect 오류:%s", odbc->getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[CDBSave::connect_db]DB Connect 성공[%d]", idx);
	m_vecDBs.push_back(odbc);
	return true;
}


bool CDBSave::reconnect_db(int idx)
{
	bool bReconn = false;

	for (unsigned int i = 0; i < m_dbReconnTry; i++)
	{
		m_vecDBs[idx]->DeInitialize();

		if (!m_vecDBs[idx]->Initialize())
		{
			gCommon.log(LOGTP_ERR, TRUE, "[reconnect_db]Initialize 오류(%s)", m_vecDBs[idx]->getMsg());
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		}
		if (!m_vecDBs[idx]->Connect(m_zConnStr))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[reconnect_db]Connect 오류(%s)", m_vecDBs[idx]->getMsg());
			std::this_thread::sleep_for(std::chrono::seconds(30));
			continue;
		}

		bReconn = true;
		break;
	}	
	return bReconn;
}


void CDBSave::push_data(string& data)
{
	if (m_nWorkerCnt == 0) {
		gCommon.log(LOGTP_ERR, TRUE, "Worder Thread 가 모두 종료되어서 프로세스를 종료합니다.");
		SetEvent(g_hDieEvent);
		return;
	}

	__MAX::TData* p = m_memPool.Alloc();
	memcpy(p->d, data.c_str(), data.size());

	std::lock_guard<mutex>lock(m_mtxQ);
	m_Q.push_back(p);
	m_cvQ.notify_one();
}


void CDBSave::compose_sp_param(int nCnt)
{
	m_query = "";

	for (int i = 0; i < PARAM_ARRAY_CNT; i++)
	{
		if (i < nCnt) {
			sprintf(m_t,
				"'%s'"	// in i_symbol_cd varchar(10)
				",'%s'"	// in i_timeframe char(2)
				",'%.13s'"	// in i_candle_tm_s char(13)
				",'%.15s'"	// in i_candle_tm_e char(13)
				",'%s'"	// in i_o varchar(20)
				",'%s'"	// in i_h varchar(20)
				",'%s'"	// in i_c varchar(20)"
				",'%s'"	// in i_l varchar(20)
				, m_Cvted[i].symbol.c_str()
				, m_Cvted[i].interval.c_str()
				, m_Cvted[i].time_s.c_str()
				, m_Cvted[i].time_e.c_str()
				, m_Cvted[i].o.c_str()
				, m_Cvted[i].h.c_str()
				, m_Cvted[i].l.c_str()
				, m_Cvted[i].c.c_str()
			);
		}
		else
		{
			sprintf(m_t, "'','','','','','','',''");
		}

		if (i == 0) {
			sprintf(m_z, "CALL sp_save_candle_2 (%d, %s", nCnt, m_t);
			m_query = m_z;
		}
		else {
			sprintf(m_z, ",%s", m_t);
			m_query += m_z;
		}
	}
	m_query += ");";	
}

void CDBSave::threadFunc_Save(int idx)
{
	
	while (m_bContinue)
	{
		std::this_thread::sleep_for(chrono::milliseconds(1));
		
		//------------------------------------------------------------------------------------------------//

		std::unique_lock<std::mutex> lock(m_mtxQ);
		m_cvQ.wait(lock, [this] { return !m_Q.empty() || !m_bContinue; });

		if (!m_bContinue) break;

		int nCnt = 0;
		while (m_Q.size() > 0 && nCnt < PARAM_ARRAY_CNT)
		{
			__MAX::TData* pData = m_Q.front();
			m_Q.pop_front();

			m_Cvted[nCnt].init();
			if (!cvt_apiData(pData, m_Cvted[nCnt]))
			{
				m_memPool.release(pData);
				break;
			}
			nCnt++;

			m_memPool.release(pData);
		}
		lock.unlock();
				
		compose_sp_param(nCnt);

		gCommon.debug("[CNT:%d](%s)", nCnt, m_query.c_str());
		//------------------------------------------------------------------------------------------------//

		
		bool bNeedReconn;
		m_vecDBs[idx]->Init_ExecQry( (char*)m_query.c_str());
		if (!m_vecDBs[idx]->Exec_Qry(bNeedReconn)) 
		{
			gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::threadFunc_Save]DB Exec_Qry ERROR-%d(%s)%s", idx, m_vecDBs[idx]->getMsg(), (char*)m_query.c_str());
			m_vecDBs[idx]->DeInit_ExecQry();

			if (!reconnect_db(idx))
			{
				gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::threadFunc_Save] DB와 연결이 끊어져 스레드(%d) 종료", idx);
				break;
			}

			continue;
		}

		if (m_vecDBs[idx]->GetNextData())
		{
			long ret_code = 0;

			m_vecDBs[idx]->GetDataLong(1, &ret_code);

			if (ret_code != 0)
			{
				gCommon.log(LOGTP_ERR, TRUE, "[CDBSave::threadFunc_Save]SP return error-%d(%s)", idx, m_vecDBs[idx]->getMsg());

			}
		}
		m_vecDBs[idx]->DeInit_ExecQry();
		//gCommon.debug("[AFTER EXEC(%d)](%s)", idx, (char*)m_query.c_str());
		
		//------------------------------------------------------------------------------------------------//
		
		
	}
	gCommon.log(INFO, TRUE, "[DB Thread 종료-%d]", idx);
	
	--m_nWorkerCnt;
}


bool CDBSave::cvt_apiData(__MAX::TData* pAPIData, _Out_ TConvertedData& cvt)
{
	std::string jsonData(pAPIData->d);
	nlohmann::json binance = nlohmann::json::parse(jsonData);

	char zKoreanTime[128] = { 0 };
	
	cvt.symbol = binance["data"]["s"];

	if (binance["data"].contains("k"))
	{
		nlohmann::json kline = binance["data"]["k"];

		cvt.interval = kline["i"];
		cvt.o = kline["o"];
		cvt.h = kline["h"].get<string>();
		cvt.l = kline["l"].get<string>();
		cvt.c = kline["c"].get<string>();

		int64_t time_s = kline["t"];
		__UTILS::UnixTimestamp_to_KoreanTime(time_s, zKoreanTime);	// yyyymmdd-hhmmss
		cvt.time_s = zKoreanTime;

		int64_t time_e = kline["T"];
		__UTILS::UnixTimestamp_to_KoreanTime(time_e, zKoreanTime);	// yyyymmdd-hhmmss
		cvt.time_e = zKoreanTime;
	}
	else {
		return false;
	}

	return true;
}