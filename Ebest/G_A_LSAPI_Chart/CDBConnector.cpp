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
	m_pingTimeout_sec = atoi(__common.get_db_ping_timeout());

	bool ok = m_pOdbc->Initialize(
		__common.get_db_ip_base()
		, __common.get_db_port_base()
		, __common.get_db_dbname_base()
		, __common.get_db_uid_base()
		, __common.get_db_pwd_base()
		, m_pingTimeout_sec
		);
	if (!ok)
	{
		__common.log_fmt(LOGTP_ERR, "[CDBConnector::connect_db] Failed to Initialize DB:%s", m_pOdbc->getMsg());
		__common.log(LOGTP_ERR, m_pOdbc->getMsg());
		return false;
	}
		

	if (!m_pOdbc->Connect())
	{
		__common.log_fmt(LOGTP_ERR, "[%s] Failed to connect DB:%s", m_pOdbc->get_conn_str(), m_pOdbc->getMsg());
		return false;
	}

	__common.log(INFO, "[CDBConnector::connect_db]DB Connected successfully");
	return true;
}


bool CDBConnector::reconnect_db()
{
	m_pOdbc->DeInitialize();

	if (!m_pOdbc->Initialize(__common.get_db_ip_base()
		, __common.get_db_port_base()
		, __common.get_db_dbname_base()
		, __common.get_db_uid_base()
		, __common.get_db_pwd_base()
		, m_pingTimeout_sec))
	{
		__common.log_fmt(LOGTP_ERR, "[reconnect_db]Failed to Initialize-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return false;
	}
	if (!m_pOdbc->Connect())
	{
		__common.log_fmt(LOGTP_ERR, "[reconnect_db]Failed to Connect-(%s)", m_pOdbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(30));
		return false;;
	}

	return true;
}


