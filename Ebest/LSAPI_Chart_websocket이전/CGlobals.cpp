#include "CGlobals.h"


CGlobals	__common;

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
		CHECK_BOOL(__common.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_IP",		m_cfg_app.listen_ip),		msg);
		CHECK_BOOL(__common.getConfig((char*)"APP_CONFIG", (char*)"LISTEN_PORT",	m_cfg_app.listen_port),		msg);
		CHECK_BOOL(__common.getConfig((char*)"APP_CONFIG", (char*)"DB_RECONN_TRY",	m_cfg_app.db_reconn_try),	msg);
		__common.log_fmt(INFO, "[APP_CONFIG]Listen IP(%s)  Listen PORT(%s)", m_cfg_app.listen_ip, m_cfg_app.listen_port);

		msg = "API_TR";
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"TRCODE",				m_cfg_apitr.tr_code),			msg);
		//CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"QRY_CNT",			m_cfg_apitr.qry_cnt),			msg);
		//CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"QRY_CNT_FIRST",		m_cfg_apitr.qry_cnt_first),		msg);
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"TM_START",			m_cfg_apitr.tm_start),			msg);
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"TM_END",				m_cfg_apitr.tm_end),			msg);
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"APIQRY_ON_SEC",		m_cfg_apitr.apiqry_on_sec),		msg);
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"APIQRY_INTERVAL_MS", m_cfg_apitr.apiqry_interval_ms), msg);
		CHECK_BOOL(__common.getConfig((char*)"API_TR", (char*)"MAX_SYMBOL_CNT",		m_cfg_apitr.max_symbol_cnt),	msg);
		
		__common.log_fmt(INFO, "[API_TR] TRCODE(%s) QRY_CNT_FIRST(%d) QRY_CNT_NEXT(%d) TM_START(%s) TM_END(%s) "
							"APIQRY_ON_SEC(%s) APIQRY_INTERVAL_SEC(%s) MAX_SYMBOL_CNT(%s)"
			, m_cfg_apitr.tr_code
			, APIQRYCNT_FIRST
			, APIQRYCNT_NEXT
			, m_cfg_apitr.tm_start
			, m_cfg_apitr.tm_end
			, m_cfg_apitr.apiqry_on_sec
			, m_cfg_apitr.apiqry_interval_ms
			, m_cfg_apitr.max_symbol_cnt
		);

		msg = "DB_INFO_BASE";
		CHECK_BOOL(__common.getConfig((char*)"DB_INFO_BASE", (char*)"DBMS",	m_cfg_db.dbms),	msg);
		CHECK_BOOL(__common.getConfig((char*)"DB_INFO_BASE", (char*)"DSN",	m_cfg_db.dsn),	msg);
		CHECK_BOOL(__common.getConfig((char*)"DB_INFO_BASE", (char*)"UID",	m_cfg_db.uid),	msg);
		CHECK_BOOL(__common.getConfig((char*)"DB_INFO_BASE", (char*)"PWD",	m_cfg_db.pwd),	msg);
		CHECK_BOOL(__common.getConfig((char*)"DB_INFO_BASE", (char*)"PING_TIMEOUT_SEC", m_cfg_db.ping_timeout), msg);
		__common.log_fmt(INFO, "[DB_INFO_BASE] DBMS(%s) DSN(%s) UID(%s) PING_TIMEOUT(%s)"
			, m_cfg_db.dbms
			, m_cfg_db.dsn
			, m_cfg_db.uid
			, m_cfg_db.ping_timeout
		);

		msg = "API_INFO";	
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"XINGAPI_PATH", m_cfg_api.xingapi_path), msg);
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"SERVER_IP",	m_cfg_api.svr_ip),		msg);
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"SERVER_PORT",	m_cfg_api.svr_port),	msg);
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"USERID",		m_cfg_api.userid),		msg);
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"USERPWD",		m_cfg_api.userpwd),		msg);
		CHECK_BOOL(__common.getConfig((char*)"API_INFO", (char*)"CERTPWD",		m_cfg_api.certpwd),		msg);
		__common.log_fmt(INFO, "[API_INFO] XINGAPI_PATH(%s) SERVER_IP(%s) SERVER_PORT(%s) USERID(%s)"
			, m_cfg_api.xingapi_path
			, m_cfg_api.svr_ip
			, m_cfg_api.svr_port
			, m_cfg_api.userid
		);

		msg = "QUERY";
		CHECK_BOOL(__common.getConfig((char*)"QUERY", (char*)"GET_SYMBOL",		m_cfg_qry.load_symbol),		msg);
		CHECK_BOOL(__common.getConfig((char*)"QUERY", (char*)"GET_TIMEFRAME",	m_cfg_qry.load_timeframe),	msg);


		msg = "DEBUGGING";
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"LOG_DEBUG", m_cfg_debug.log_debug), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"DEBUG_1", m_cfg_debug.debug1), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"DEBUG_2", m_cfg_debug.debug2), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"DEBUG_3", m_cfg_debug.debug3), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"DEBUG_4", m_cfg_debug.debug4), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"DEBUG_5", m_cfg_debug.debug5), msg);
		CHECK_BOOL(__common.getConfig((char*)"DEBUGGING", (char*)"ASSERT", m_cfg_debug.assert), msg);

		m_bDebugLog = (m_cfg_debug.log_debug[0] == 'Y') ? true : false;

	}
	catch (const std::exception& e)
	{
		__common.log_fmt(ERR, "Config 읽는 도중 오류:%s", e.what());
		return false;
	}

	return true;
}

void CGlobals::debug( const char* pMsg)
{
	m_log.Log(INFO, pMsg, FALSE);
}

void CGlobals::debug_fmt(const char* pMsg, ...)
{
	if (!m_bDebugLog)
		return;

	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, size, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	m_log.Log(INFO, szBuf, TRUE);
}

void CGlobals::log(LOGMSG_TP tp, const char* pMsg)
{
	m_log.Log(tp, pMsg, FALSE);
}

void CGlobals::log_fmt(LOGMSG_TP tp, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, size, pMsg, argptr);
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
	vsprintf_s(szBuf, size, pMsg, argptr);
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

