#include "stdafx.h"
#include "CDBConnector.h"
#include "CGlobals.h"

CDBConnector::CDBConnector()
{
	m_dbReconnTry = 0;
	m_pingTimeout_sec = 0;

	m_pOdbc = std::make_unique<CODBC>(DBMS_TYPE::MSSQL);
}


CDBConnector::~CDBConnector()
{
}


bool CDBConnector::connect_db()
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 }, zPingTimeout[128] = { 0 };

	if (!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"PWD", zPwd) ||
		!gCommon.getConfig((char*)"MSSQL_INFO", (char*)"PING_TIMEOUT_SEC", zPingTimeout)
		)
	{
		gCommon.log(LOGTP_ERR, "Failed to read DB Information in INI file");
		return false;
	}

	sprintf(m_zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	m_pingTimeout_sec = atoi(zPingTimeout);

	if (!m_pOdbc->Initialize(m_pingTimeout_sec))
	{
		gCommon.log(LOGTP_ERR, m_pOdbc->getMsg());
		return false;
	}
	if (!m_pOdbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, "[CDBConnector::connect_db] Failed to connect DB:%s", m_pOdbc->getMsg());
		return false;
	}

	gCommon.log(INFO, "[CDBConnector::connect_db]DB Connected successfully");
	return true;
}


bool CDBConnector::reconnect_db()
{
	m_pOdbc->DeInitialize();

	if (!m_pOdbc->Initialize(m_pingTimeout_sec))
	{
		gCommon.log(LOGTP_ERR, "[reconnect_db]Failed to Initialize-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return false;
	}
	if (!m_pOdbc->Connect(m_zConnStr))
	{
		gCommon.log(LOGTP_ERR, "[reconnect_db]Failed to Connect-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(30));
		return false;;
	}

	return true;
}


