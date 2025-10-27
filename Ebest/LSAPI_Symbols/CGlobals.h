#pragma once

#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include <atomic>

#define EXENAME			"LSAPI_Symbols.exe"
#define EXE_VERSION		"v1.0.0"	// 


struct TApp {
	char listen_ip[128];
	char listen_port[128];
	char db_reconn_try[128];
	char log_debug[128];
};

struct TAPITr {
	char trcode_kf[128];
	char trcode_ov_master[128];
	char trcode_ov_info[128];
	char read_cnt[128];
	char tm_start[128];
	char tm_end[128];
	//char apiqry_often_sec[128];
	//char apiqry_seldom_min[128];
	//char apiqry_trinterval_sec[128];
	//char max_symbol_cnt[128];
};

struct TDBInfo{
	char dbms[128];
	char ip[128];
	char port[128];
	char dsn[128];
	char uid[128];
	char pwd[128];
};


struct TAPIInfo {
	char svr_ip[128];
	char svr_port[128];
	char userid[128];
	char userpwd[128];
	char certpwd[128];
};


//struct TQuery {
//	char load_symbol[512];
//	char load_timeframe[512];
//};


class CGlobals
{
public:
	CGlobals();
	~CGlobals();

	bool Initialize();
	bool read_config_all();
	void logStart	(const char* pMsg, ...);
	void log		(LOGMSG_TP tp, const char* pMsg, ...);
	void debug		(const char* pMsg, ...);
	
	bool getConfig		(char* pzSection, char* pzKey, char* pValue);
	bool getConfigQuery	(char* pzSection, char* pzKey, char* pValue);
	void format_api_str	(const char* org, int size, _Out_ char* fmt);

	char* trcode_kf() { return m_cfg_apitr.trcode_kf;}
	char* trcode_ov_master() { return m_cfg_apitr.trcode_ov_master; }
	char* trcode_ov_info() { return m_cfg_apitr.trcode_ov_info; }

	//char* query_symbols()	{ return m_cfg_qry.load_symbol;}
	//char* query_timeframes(){ return m_cfg_qry.load_timeframe; }
	//int   max_symbol_cnt()  { return atoi(m_cfg_apitr.max_symbol_cnt);}

	//char* apiqry_often_sec()		{ return m_cfg_apitr.apiqry_often_sec;}
	//bool	is_seldom_min_odd() { return m_is_seldom_min_odd;}
	//int	  apiqry_trinterval_sec()	{ return atoi(m_cfg_apitr.apiqry_trinterval_sec);}

	char* start_tm()	{ return m_cfg_apitr.tm_start;}
	char* end_tm()		{ return m_cfg_apitr.tm_end; }

	char*	apiqry_read_cnt()	{ return m_cfg_apitr.read_cnt; }
	
	char*	get_userid(){ return m_cfg_api.userid;}
	char*	get_userpwd() { return m_cfg_api.userpwd; }
	char*	get_certpwd() { return m_cfg_api.certpwd; }

public:
	TApp		m_cfg_app;
	TAPITr		m_cfg_apitr;
	TDBInfo		m_cfg_db;
	TAPIInfo	m_cfg_api;
	//TQuery		m_cfg_qry;
private:
	CLogMsg	m_log;
	char	m_zConDir[_MAX_PATH];
	char	m_zLogDir[_MAX_PATH];
	char	m_zConfigFileName[MAX_PATH];
	bool	m_bDebugLog;
	bool	m_is_seldom_min_odd;
};

extern CGlobals	gCommon;

class CThreadFlag
{
public:
	CThreadFlag() :m_threadRun(false), m_threadReady(false) {}

	void setThreadRun() { m_threadRun = true; }
	void setThreadStop() { m_threadRun = false; }
	void setThreadReady() { m_threadReady = true; }

	bool isRunning() { return m_threadRun; }
	bool isReady() { return m_threadReady; }

private:
	std::atomic<bool>			m_threadRun, m_threadReady;
	
};


