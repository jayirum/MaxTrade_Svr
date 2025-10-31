#pragma once

#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include "../../Common/StringUtils.h"
#include "../../Common/CspscRing.h"
#include <atomic>

#define EXENAME			"LSAPI_Chart.exe"
#define EXE_VERSION		"v5.0.0"	// IOCP ž�� �� �ɺ������� ���� ����

constexpr int APIQRYCNT_FIRST = 3;
constexpr int APIQRYCNT_NEXT = 2;


struct TApp {
	char listen_ip		[32];
	char listen_port	[32];
	char db_reconn_try	[32];
};

struct TAPITr {
	char tr_code			[32];
	//char qry_cnt			[32];
	//char qry_cnt_first	[32];
	char tm_start			[32];
	char tm_end				[32];
	char apiqry_on_sec		[32];
	char apiqry_interval_ms [32];
	char max_symbol_cnt		[32];
};

struct TDBInfo{
	char dbms	[32];
	char ip		[32];
	char port	[32];
	char dsn	[32];
	char uid	[32];
	char pwd	[32];
	char ping_timeout[32];
};


struct TAPIInfo {
	char svr_ip		[32];
	char svr_port	[32];
	char userid		[32];
	char userpwd	[128];
	char certpwd	[128];
};


struct TQuery {
	char load_symbol[512];
	char load_timeframe[512];
};

struct TDebugging {
	char log_debug[32];
	char debug1[32];
	char debug2[32];
	char debug3[32];
	char debug4[32];
	char debug5[32];
	char assert[32];
};


struct TAPIData {
	std::string symbol, timeframe, timediff;
	std::string	tm_kor_ymd_hms;
	std::string	o, h, l, c, v;
	
	void set(std::string& s, 
			std::string& tf, 
			char* candle_kor_ymd_hms, // yyyymmdd_hhmmss 20251031_152700
			char* po, char* ph, char* pl, char* pc, char* pv
			)
	{
		symbol		= s;
		timeframe	= tf;
		tm_kor_ymd_hms = candle_kor_ymd_hms;

		CStringUtils u;
		o = std::string(po);
		h = std::string(ph);
		l = std::string(pl);
		c = std::string(pc);
		v = std::string(pv);
	}
};


class CGlobals
{
public:
	CGlobals();
	~CGlobals();

	bool Initialize();
	bool read_config_all();
	void logStart	(const char* pMsg, ...);
	void log_fmt	(LOGMSG_TP tp, const char* pMsg, ...);
	void log(LOGMSG_TP tp, const char* pMsg);
	
	bool getConfig		(char* pzSection, char* pzKey, char* pValue);
	bool getConfigQuery	(char* pzSection, char* pzKey, char* pValue);
	void format_api_str	(const char* org, int size, _Out_ char* fmt);


	//TApp
	char*		app_listen_ip()		{ return m_cfg_app.listen_ip;}
	uint16_t	app_listen_port()	{ return atoi(m_cfg_app.listen_port); }


	//TAPITr
	char* get_api_tr()			{ return m_cfg_apitr.tr_code;}
	//char* apiqry_qry_cnt()		{ return m_cfg_apitr.qry_cnt; }
	//char* apiqry_qry_cnt_first(){ return m_cfg_apitr.qry_cnt_first; }
	char* start_tm()			{ return m_cfg_apitr.tm_start; }
	char* end_tm()				{ return m_cfg_apitr.tm_end; }
	char* apiqry_on_sec()		{ return m_cfg_apitr.apiqry_on_sec; }
	int	  apiqry_interval_ms() { return atoi(m_cfg_apitr.apiqry_interval_ms); }
	int   max_symbol_cnt()		{ return atoi(m_cfg_apitr.max_symbol_cnt);}


	//TDBInfo
	char*	get_dsn(){ return m_cfg_db.dsn;}
	char*	get_db_uid() { return m_cfg_db.uid;}
	char*	get_db_pwd() { return m_cfg_db.pwd;}
	char*	get_db_ping_timeout() { return m_cfg_db.ping_timeout;}

	//TAPIInfo
	char*	get_userid(){ return m_cfg_api.userid;}
	char*	get_userpwd() { return m_cfg_api.userpwd; }
	char*	get_certpwd() { return m_cfg_api.certpwd; }

	//TQuery
	char* query_symbols() { return m_cfg_qry.load_symbol; }
	char* query_timeframes() { return m_cfg_qry.load_timeframe; }

	//TDebugging
	void debug_fmt(const char* pMsg, ...);
	void debug(const char* pMsg);
	bool is_debug1(){ return  (m_cfg_debug.debug1[0]=='Y'); }
	bool is_debug2() { return (m_cfg_debug.debug2[0] == 'Y'); }
	bool is_debug3() { return (m_cfg_debug.debug3[0] == 'Y'); }
	bool is_debug4() { return (m_cfg_debug.debug4[0] == 'Y'); }
	bool is_debug5() { return (m_cfg_debug.debug5[0] == 'Y'); }
	void assert_()  { if(m_cfg_debug.assert[0]=='Y') assert(false); }

public:
	TApp		m_cfg_app;
	TAPITr		m_cfg_apitr;
	TDBInfo		m_cfg_db;
	TAPIInfo	m_cfg_api;
	TQuery		m_cfg_qry;
	TDebugging	m_cfg_debug;

	CspscRing< TAPIData>	m_saveQ;
private:
	CLogMsg	m_log;
	char	m_zConDir[_MAX_PATH];
	char	m_zLogDir[_MAX_PATH];
	char	m_zConfigFileName[MAX_PATH];
	bool	m_bDebugLog;
};

extern CGlobals	__common;

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



