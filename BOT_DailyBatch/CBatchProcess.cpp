#include "CBatchProcess.h"
#include "CGlobals.h"
#include "../Common/Util.h"

extern bool			g_bDebug;
extern HANDLE		g_hDieEvent;
extern CGlobals		gCommon;

CBatchProcess::CBatchProcess()
{
	m_thrdFlag.set_idle();
	m_odbcSel = std::make_unique<CODBC>(DBMS_TYPE::MYSQL);
	//m_odbcWork = std::make_unique<CODBC>(DBMS_TYPE::MYSQL);
}

CBatchProcess::~CBatchProcess()
{
	m_thrdFlag.set_stop();

	if (m_thrdSelect.joinable())
		m_thrdSelect.join();
}

bool CBatchProcess::Initialize()
{ 
	//----------------------------------------------------------------------------------------------------//
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 }, zPingTimeout[128] = { 0 };
	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd) || 
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PING_TIMEOUT_SEC", zPingTimeout)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBatchProcess::Initialize]DB정보 조회 오류.INI파일을 확인하세요");
		return false;
	}

	char zConnStr[512] = { 0 };
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);
	m_sConnStr = zConnStr;

	m_dbPing_timeout = atoi(zPingTimeout);
	
	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY", zDsn))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBatchProcess::Initialize]INI 에서 DB_RECONN_TRY 확인 필요");
		return false;
	}
	m_dbReconnTry = atoi(zDsn);

	//----------------------------------------------------------------------------------------------------//

	if (!gCommon.getConfig((char*)"APP_CONFIG", (char*)"TIMEOUT_SEC_BATCH_SEL", zDsn))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBatchProcess::Initialize]INI 에서 TIMEOUT_SEC_BATCH_SEL 확인 필요");
		return false;
	}
	m_timeoutSelBatch = atoi(zDsn);

	//----------------------------------------------------------------------------------------------------//

	if (!dbConnect(m_odbcSel.get(), string(WORK_SEL)))
		return false;

	//if (!dbConnect(m_odbcWork.get(), string(WORK_BATCH)))
	//	return false;

	m_thrdSelect = std::thread(&CBatchProcess::threadFunc_Select, this);
	m_thrdFlag.set_run();
	//----------------------------------------------------------------------------------------------------//


	return true;
}

void CBatchProcess::threadFunc_Select()
{
	__try
	{ 
		threadFunc_Select_Internal();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		gCommon.log(ERR, TRUE, "---------------------------------------------");
		gCommon.log(ERR, TRUE, "Exception in CBatchProcess::threadFunc_Select");
		gCommon.log(ERR, TRUE, "---------------------------------------------");
	}
}

void CBatchProcess::threadFunc_Select_Internal()
{
	std::this_thread::sleep_for(std::chrono::seconds(3));

	SYSTEMTIME st;
	char now_time[128];

	char zQ[1024], batch_cd[32], exec_nm[128], start_tm[32];
	sprintf(zQ, "select batch_cd, exec_nm, start_tm from BATCH_SCL where use_yn = 'Y' and done = 'N' order by start_tm;");
	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::seconds(m_timeoutSelBatch));

		if (m_thrdFlag.is_idle()) continue;

		//m_odbcWork->PingConnection();

		//------------------------------------------------------------------------------------------------//
		bool bNeedReconn;
		m_odbcSel->Init_ExecQry(zQ);
		if (!m_odbcSel->Exec_Qry(bNeedReconn))
		{
			gCommon.log(LOGTP_ERR, TRUE, "[배치테이블 조회 오류!!!](%s)(%s)", zQ, m_odbcSel->getMsg());
			m_odbcSel->DeInit_ExecQry();

			if (bNeedReconn)
			{
				if (!reconnectDB(m_odbcSel.get(), WORK_SEL))
				{
					gCommon.log(LOGTP_ERR, TRUE, "[배치테이블 조회 오류 후 reconnect 실패!!!]");
				}
				else {
					gCommon.log(INFO, TRUE, "[배치테이블 조회 오류 후 reconnect 성공***]");
				}
			}
			continue;
		}

		//------------------------------------------------------------------------------------------------//

		while (m_odbcSel->GetNextData())
		{
			ZeroMemory(batch_cd, sizeof(batch_cd));
			ZeroMemory(start_tm, sizeof(start_tm));
			ZeroMemory(exec_nm, sizeof(exec_nm));

			m_odbcSel->GetDataStr(1, sizeof(batch_cd), batch_cd);
			m_odbcSel->GetDataStr(2, sizeof(exec_nm), exec_nm);
			m_odbcSel->GetDataStr(3, sizeof(start_tm), start_tm);

			GetLocalTime(&st); sprintf(now_time, "%02d:%02d", st.wHour, st.wMinute);

			gCommon.debug("[배치조회](batch_code:%s)(sp:%s)(time:%s)-(now:%s)", batch_cd, exec_nm, start_tm, now_time);


			if (strncmp(now_time, start_tm, 5) == 0)
			{
				gCommon.log(INFO, TRUE, "[배치조회](batch_code:%s)(sp:%s)(time:%s)", batch_cd, exec_nm, start_tm);
				//if (!do_batch(std::make_unique<TJob>(string(batch_cd), string(exec_nm))))
				//	return;
				m_deqJobs.push_back(std::make_unique<TJob>(batch_cd, exec_nm));
			}
		}
		m_odbcSel->DeInit_ExecQry();

		do_batch();


	} // while (!m_thrdFlag.is_stopped())

	gCommon.log(INFO, TRUE, "CBatchProcess::threadFunc_Select exiting....");
}

bool CBatchProcess::do_batch()
{
	if (m_deqJobs.empty())
		return true;


	int retry = 0;
	bool bSucc = false;
	bool bNeedReconn;
	char zQ[1024];

	
	CODBC odbc(DBMS_TYPE::MYSQL);
	while (!dbConnect(&odbc, string(WORK_BATCH)))
		Sleep(3000);

	for (auto& it : m_deqJobs)
	{
		sprintf(zQ, "call %s()", it->sp_name.c_str());

		odbc.Init_ExecQry(zQ);
		for (int retry = 0; retry < 5; retry++)
		{
			bSucc = odbc.Exec_Qry(bNeedReconn);
			odbc.DeInit_ExecQry();
			if (bSucc)
				break;

			gCommon.log(LOGTP_ERR, TRUE, "[DB 배치 실행 오류!!!(%s)](%s)(%s)", it->batch_code.c_str(), zQ, odbc.getMsg());
			if (bNeedReconn) {
				Sleep(3000);
				reconnectDB(&odbc, WORK_BATCH);
			}
		}
		if (!bSucc) {
			gCommon.log(LOGTP_ERR, TRUE, "\n\n배치실행 실패로 스레드 종료\n\n");
			return false;
		}

		gCommon.log(INFO, TRUE, "[BATCH 실행성공***(%s)](%s)", it->batch_code.c_str(), zQ);

		//----------------------------------------------------------------------------------------------------//

		sprintf(zQ, "call spbatch_set_dailybatch_done('%s')", it->batch_code.c_str());
		bSucc = false;
		odbc.Init_ExecQry(zQ);
		for (int retry = 0; retry < 5; retry++)
		{
			bSucc = odbc.Exec_Qry(bNeedReconn);
			odbc.DeInit_ExecQry();
			if (bSucc)
				break;

			gCommon.log(LOGTP_ERR, TRUE, "[DB 배치 완료 업데이트 오류!!!](%s)(%s)", zQ, odbc.getMsg());
			if (bNeedReconn) {
				reconnectDB(&odbc, WORK_BATCH);
			}
		}
		if (!bSucc) {
			gCommon.log(LOGTP_ERR, TRUE, "\n\n배치완료 업데이트 실패로 스레드 종료\n\n");
			return false;
		}
		gCommon.log(INFO, TRUE, "[BATCH 완료 업데이트 성공***(%s)](%s)", it->batch_code.c_str(), zQ);
		//----------------------------------------------------------------------------------------------------//
	}

	m_deqJobs.clear();
	return true;
}

bool CBatchProcess::dbConnect(CODBC* p, string work)
{
	if (!p->Initialize(m_dbPing_timeout))
	{
		gCommon.log(LOGTP_ERR, TRUE, p->getMsg());
		return false;
	}
	if (!p->Connect(m_sConnStr.c_str()))
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CBatchProcess::dbConnect]DB Connect 오류:%s", p->getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[%s]DB Connect 성공", work.c_str());
	return true;
}

bool CBatchProcess::reconnectDB(CODBC* p, string work)
{
	bool conn = false;
	for (int i = 0; i < m_dbReconnTry; i++)
	{
		if (dbConnect(p, work)) {
			conn = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	return conn;
}
