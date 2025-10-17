#include "CGlobals.h"


CGlobals	gCommon;

/// <summary>
/// 
/// 
/// </summary>

CGlobals::CGlobals()
{ 
}

CGlobals::~CGlobals()
{}

bool CGlobals::Initialize()
{
	GetCurrentDirectory(_MAX_PATH, m_zConDir);
	CUtil::GetCnfgFileNm(m_zConDir, (char*)EXENAME, m_zConfigFileName);
	sprintf(m_zLogDir, "%s\\Log", m_zConDir);

	if (!m_log.OpenLog(m_zLogDir, EXENAME))
	{
		printf("Open Log Error(%s)\n", m_zLogDir);
		return false;
	}

	return true;
}

bool CGlobals::read_config_all()
{
	std::string msg;
	try {
		msg = "APP_CONFIG";
		CHECK_BOOL(gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_IP",		m_cfg_app.listen_ip),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT",		m_cfg_app.listen_port),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY",	m_cfg_app.db_reconn_try),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"APP_CONFIG", (char*)"LOG_DEBUG",		m_cfg_app.log_debug),		msg);
		m_bDebugLog = (m_cfg_app.log_debug[0] == 'Y') ? true : false;
		gCommon.log(INFO, "[APP_CONFIG]Listen IP(%s)  Listen PORT(%s) LOG_DEBUG(%s)",
			m_cfg_app.listen_ip, m_cfg_app.listen_port, m_cfg_app.log_debug);

		msg = "API_TR";
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"TRCODE",				m_cfg_apitr.tr_code),			msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"READ_CNT",			m_cfg_apitr.read_cnt),			msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"TM_START",			m_cfg_apitr.tm_start),			msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"TM_END",				m_cfg_apitr.tm_end),			msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"APIQRY_OFTEN_SEC",	m_cfg_apitr.apiqry_often_sec),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"APIQRY_SELDOM_MIN",	m_cfg_apitr.apiqry_seldom_min), msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"APIQRY_TRINTERVAL_SEC", m_cfg_apitr.apiqry_trinterval_sec), msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_TR", (char*)"MAX_SYMBOL_CNT",		m_cfg_apitr.max_symbol_cnt),	msg);
		
		m_is_seldom_min_odd = (strncmp(m_cfg_apitr.apiqry_seldom_min, "ODD", 3) == 0);

		gCommon.log(INFO, "[API_TR] TRCODE(%s) READ_CNT(%s) TM_START(%s) TM_END(%s)"
			"APIQRY_OFTEN_SEC(%s) APIQRY_SELDOM_MIN(%s) APIQRY_TRINTERVAL_SEC(%s) "
			"APIQRY_SELDOM_MIN(%s)"
			, m_cfg_apitr.tr_code
			, m_cfg_apitr.read_cnt
			, m_cfg_apitr.tm_start
			, m_cfg_apitr.tm_end
			, m_cfg_apitr.apiqry_often_sec
			, m_cfg_apitr.apiqry_seldom_min
			, m_cfg_apitr.apiqry_trinterval_sec
			, m_cfg_apitr.max_symbol_cnt
		);

		msg = "DB_INFO";
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"DBMS",	m_cfg_db.dbms),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"IP",		m_cfg_db.ip),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"PORT",	m_cfg_db.port), msg);
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"DSN",	m_cfg_db.dsn),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"UID",	m_cfg_db.uid),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"DB_INFO", (char*)"PWD",	m_cfg_db.pwd),	msg);
		gCommon.log(INFO, "[DB_INFO] DBMS(%s) IP(%s) PORT(%s) DSN(%s) UID(%s) PWD(%s) "
			, m_cfg_db.dbms
			, m_cfg_db.ip
			, m_cfg_db.port
			, m_cfg_db.dsn
			, m_cfg_db.uid
			, m_cfg_db.pwd
		);

		msg = "API_INFO";	
		CHECK_BOOL(gCommon.getConfig((char*)"API_INFO", (char*)"SERVER_IP",		m_cfg_api.svr_ip),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_INFO", (char*)"SERVER_PORT",	m_cfg_api.svr_port),	msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_INFO", (char*)"USERID",		m_cfg_api.userid),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_INFO", (char*)"USERPWD",		m_cfg_api.userpwd),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"API_INFO", (char*)"CERTPWD",		m_cfg_api.certpwd),		msg);
		gCommon.log(INFO, "[API_INFO] SERVER_IP(%s) SERVER_PORT(%s) USERID(%s) USERPWD(%s) CERTPWD(%s)"
			, m_cfg_api.svr_ip
			, m_cfg_api.svr_port
			, m_cfg_api.userid
			, m_cfg_api.userpwd
			, m_cfg_api.certpwd
		);

		msg = "QUERY";
		CHECK_BOOL(gCommon.getConfig((char*)"QUERY", (char*)"GET_SYMBOL",		m_cfg_qry.load_symbol),		msg);
		CHECK_BOOL(gCommon.getConfig((char*)"QUERY", (char*)"GET_TIMEFRAME",	m_cfg_qry.load_timeframe),	msg);
		gCommon.log(INFO, "[API_INFO] SERVER_IP(%s) SERVER_PORT(%s) USERID(%s) USERPWD(%s) CERTPWD(%s)"
			, m_cfg_api.svr_ip
			, m_cfg_api.svr_port
			, m_cfg_api.userid
			, m_cfg_api.userpwd
			, m_cfg_api.certpwd
		);
	}
	catch (const std::exception& e)
	{
		gCommon.log(ERR, "Config 읽는 도중 오류:%s", e.what());
		return false;
	}

	return true;
}

void CGlobals::debug(const char* pMsg, ...)
{
	if (!m_bDebugLog)
		return;

	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	m_log.Log(INFO, szBuf, TRUE);
}
void CGlobals::log(LOGMSG_TP tp, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	m_log.Log(tp, szBuf, FALSE);
}

void CGlobals::logStart(const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	//m_log.Log(tp, "=============================================================", FALSE);
	m_log.Log(LOGTP_START, szBuf, FALSE);
}
bool CGlobals::getConfig(char* pzSection, char* pzKey, char* pValue)
{
	return CUtil::GetConfig(m_zConfigFileName, pzSection, pzKey, pValue);
}


bool CGlobals::getConfigQuery(char* pzSection, char* pzKey, char* pValue)
{
	return CUtil::GetConfigQuery(m_zConfigFileName, pzSection, pzKey, pValue);
}

// 앞으로 정렬. 남는 부분 0x20;
void CGlobals::format_api_str(const char* org, int size, _Out_ char* fmt)
{
	sprintf(fmt, "%-*s", size, org);	//[124.24    ]
}

