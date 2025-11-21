#pragma once

#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include "../../Common/StringUtils.h"
#include <atomic>
#include "AppCommon.h"
#include <deque>

using namespace std;

struct TApp {
	char listen_ip		[32]{};
	char listen_port	[32]{};
	char db_reconn_try	[32]{};
};

struct TSiseSvr{
	char ip			[32]{};
	char port		[32]{};
	char timeout_ms	[32]{};
};

struct TAPITr {
	char tr_code			[32]{};
	//char qry_cnt			[32]{};
	//char qry_cnt_first	[32]{};
	char tm_start			[32]{};
	char tm_end				[32]{};
	char apiqry_on_sec		[32]{};
	char apiqry_interval_ms [32]{};
	char max_symbol_cnt		[32]{};
};

struct TDBInfoBase{
	char dbms	[64]{};
	char ip		[64]{};
	char port	[64]{};
	char dbname [64]{};
	char uid	[64]{};
	char pwd	[64]{};
	char ping_timeout[32]{};
};

struct TBOTDb
{
	char company[64]{};
	char dbms[64]{};
	char ip[64]{};
	char port[64]{};
	char uid[64]{};
	char pwd[64]{};
};

struct TAPIInfo {
	char xingapi_path	[_MAX_PATH]{};
	char svr_ip		[32]{};
	char svr_port	[32]{};
	char userid		[32]{};
	char userpwd	[128]{};
	char certpwd	[128]{};
};


struct TQuery {
	char load_symbol[512];
	char load_timeframe[512];
};

struct TDebugging {
	char debug_log[32];
	char debug_recv[32];
	char debug_send[32];
	char assert[32];
};




class CGlobals
{
public:
	CGlobals();
	~CGlobals();

	bool Initialize();
	bool read_config_all();
	bool read_config_debug();
	void logStart	(const char* pMsg, ...);
	void log_fmt	(LOGMSG_TP tp, const char* pMsg, ...);
	void log(LOGMSG_TP tp, const char* pMsg);
	
	bool getConfig		(char* pzSection, char* pzKey, char* pValue);
	bool getConfigQuery	(char* pzSection, char* pzKey, char* pValue);
	void format_api_str	(const char* org, int size, _Out_ char* fmt);


	//TApp
	char*		app_listen_ip()		{ return m_cfg_app.listen_ip;}
	uint16_t	app_listen_port()	{ return atoi(m_cfg_app.listen_port); }

	// sise server
	char*		sise_svr_ip()			{ return m_cfg_sise_svr.ip; }
	int			sise_svr_port()			{ return atoi(m_cfg_sise_svr.port);}
	int			sise_recv_timeout_ms()	{ return atoi(m_cfg_sise_svr.timeout_ms); }

	//TAPITr
	char* get_api_tr()			{ return m_cfg_apitr.tr_code;}
	//char* apiqry_qry_cnt()		{ return m_cfg_apitr.qry_cnt; }
	//char* apiqry_qry_cnt_first(){ return m_cfg_apitr.qry_cnt_first; }
	char* start_tm()			{ return m_cfg_apitr.tm_start; }
	char* end_tm()				{ return m_cfg_apitr.tm_end; }
	char* apiqry_on_sec()		{ return m_cfg_apitr.apiqry_on_sec; }
	int	  apiqry_interval_ms() { return atoi(m_cfg_apitr.apiqry_interval_ms); }
	int   max_symbol_cnt()		{ return atoi(m_cfg_apitr.max_symbol_cnt);}


	//TDBInfoBase
	char*	get_db_ip_base() { return m_cfg_db.ip;}
	char*	get_db_port_base() { return m_cfg_db.port; }
	char*	get_db_dbname_base() { return m_cfg_db.dbname; }
	char*	get_db_uid_base() { return m_cfg_db.uid; }
	char*	get_db_pwd_base() { return m_cfg_db.pwd;}
	char*	get_db_ping_timeout() { return m_cfg_db.ping_timeout;}

	//TAPIInfo
	char*	get_xingapi_path()	{ return m_cfg_api.xingapi_path;}
	char*	get_userid()		{ return m_cfg_api.userid;}
	char*	get_userpwd()		{ return m_cfg_api.userpwd; }
	char*	get_certpwd()		{ return m_cfg_api.certpwd; }

	//TQuery
	char* query_symbols() { return m_cfg_qry.load_symbol; }
	char* query_timeframes() { return m_cfg_qry.load_timeframe; }

	//TDebugging
	void debug_fmt(const char* pMsg, ...);
	void debug(const char* pMsg);
	
	void debug_recv(const char* pMsg, ...);
	void debug_send(const char* pMsg, ...);
	void assert_();//  { if(m_cfg_debug.assert[0]=='Y') assert(false); }

private:
	void	thrd_read_debug();
public:
	TApp		m_cfg_app;
	TSiseSvr	m_cfg_sise_svr;
	TAPITr		m_cfg_apitr;
	TDBInfoBase	m_cfg_db;
	TAPIInfo	m_cfg_api;
	TQuery		m_cfg_qry;
	TDebugging	m_cfg_debug;
	CLogMsg		m_log;
	
private:
	
	char	m_zConDir[_MAX_PATH];
	char	m_zLogDir[_MAX_PATH];
	char	m_zConfigFileName[MAX_PATH];
	
	std::thread		_thrd_read_debug;
	bool			_thrd_continue{true};
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




template<typename T>
class CSimpleQueue
{
public:
	void push(T& t) {
		std::lock_guard<std::mutex>lock(_mtx);
		_q.push_back(t);
	}

	T pop() {
		std::lock_guard<std::mutex> lock(_mtx);
		if (_q.empty()) return nullptr;

		T t = _q.front();
		_q.pop_front();
		return t;
	}

private:
	deque< T>	_q;
	std::mutex	_mtx;
};