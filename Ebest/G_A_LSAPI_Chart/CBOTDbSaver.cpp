#include "stdafx.h"
#include "CBOTDbSaver.h"
#include "CGlobals.h"

ns_bot_db::CBOTDbManager __bot_manager;

ns_bot_db::COneBot::COneBot(DBMS_TYPE dbms, string company, string dsn, string user, string pwd)
{
	m_is_continue = true;

	m_dbms		= dbms;
	m_company	= company;
	m_dsn		= dsn;
	m_user		= user;
	m_pwd		= pwd;

	m_odbc = std::make_shared<CODBC>(m_dbms);	
}

ns_bot_db::COneBot::~COneBot(){
	set_die();
	if(m_thrd.joinable())	m_thrd.join();
}

bool ns_bot_db::COneBot::initialize()
{
	if(!connect())
		return false;

	m_thrd = std::thread(&ns_bot_db::COneBot::thrd_main, this);
	return true;
}

bool ns_bot_db::COneBot::connect()
{
	sprintf(m_conn_str, "DSN=%s;UID=%s;PWD=%s;", m_dsn.c_str(), m_user.c_str(), m_pwd.c_str());

	if (!m_odbc->Initialize(PING_TIMEOUT_SEC))
	{
		__common.log_fmt(ERR, "[COneBot::connect] Failed to Initialize DB:%s", m_odbc->getMsg());
		return false;
	}
	if (!m_odbc->Connect(m_conn_str))
	{
		__common.log_fmt(ERR, "[CDBConnector::connect_db] Failed to connect DB:%s", m_odbc->getMsg());
		return false;
	}

	__common.log_fmt(INFO, "[%s]DB Connected successfully", m_conn_str);
	return true;
}



bool ns_bot_db::COneBot::re_connect()
{
	m_odbc->DeInitialize();

	if (!m_odbc->Initialize(PING_TIMEOUT_SEC))
	{
		__common.log_fmt(LOGTP_ERR, "[reconnect_db]Failed to Initialize-(%s)", m_odbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return false;
	}
	if (!m_odbc->Connect(m_conn_str))
	{
		__common.log_fmt(LOGTP_ERR, "[reconnect_db]Failed to Connect-(%s)", m_odbc->getMsg());
		std::this_thread::sleep_for(std::chrono::seconds(30));
		return false;;
	}

	return true;
}


void ns_bot_db::COneBot::push(TBOTCandPtr& p)
{
	std::lock_guard<mutex> lock(m_mtx);
	m_queue.push_back(p);
}

void ns_bot_db::COneBot::thrd_main()
{
	char t[1024]{};
	while (m_is_continue)
	{
		_mm_pause();

		string query;
		int cnt = compose_query(query);
		if(cnt==0) continue;

		bool bNeedReconn;
		m_odbc->Init_ExecQry((char*)query.c_str());
		if (!m_odbc->Exec_Qry(bNeedReconn))
		{
			__common.log_fmt(ERR, "BOT Exec_Qry ERROR(%s)<%s>", m_odbc->getMsg(), (char*)query.c_str());
			m_odbc->DeInit_ExecQry();

			if (!re_connect())	break;
			else{
				__common.log_fmt(INFO, "COneBot DB reconnected fuccessfully");
			}
			continue;
		}

		if (m_odbc->GetNextData())
		{
			long ret_code = 0;

			m_odbc->GetDataLong(1, &ret_code);

			if (ret_code != 0)
			{
				__common.log_fmt(ERR, "[COneBot]SP return error(%s)", m_odbc->getMsg());

			}
		}
		m_odbc->DeInit_ExecQry();
		//__common.debug_fmt("[BOT SAVE OK](%s)", (char*)query.c_str());
	}
}

int	ns_bot_db::COneBot::compose_query(string& query)
{
	char t[512];
	std::lock_guard<mutex> lock(m_mtx);
	if(m_queue.empty()) return 0;

	int candle_cnt = 0;
	int queue_size = (m_queue.size()> MAX_CNADLE_CNT)? MAX_CNADLE_CNT: m_queue.size();

	sprintf(t, "CALL sp_save_candle_2(%d,", queue_size);
	query = t;

	for (candle_cnt = 0; candle_cnt < ns_bot_db::MAX_CNADLE_CNT; candle_cnt++)
	{
		if( !m_queue.empty() )
		{
			TBOTCandPtr candle = m_queue.front();
			m_queue.erase(m_queue.begin());

			sprintf(t,
				"'%s'"		// in i_symbol_cd varchar(10)
				",'%d'"		// in i_timeframe char(2)
				",'%.13s'"	// in i_candle_tm_s char(13)
				",'%.15s'"	// in i_candle_tm_e char(13)
				",'%.5f'"	// in i_o varchar(20)
				",'%.5f'"	// in i_h varchar(20)
				",'%.5f'"	// in i_c varchar(20)"
				",'%.5f'"	// in i_l varchar(20)
				, candle->symbol.c_str()
				, candle->tf
				, candle->candle_tm.c_str()
				, candle->candle_end_tm.c_str()
				, candle->o
				, candle->h
				, candle->l
				, candle->c
			);
		}
		else
		{
			sprintf(t, "'','','','','','','',''");
		}
		if (candle_cnt > 0)	query += ",";
		query.append(t, strlen(t));
	}
	query.append(");", 2);

	return queue_size;
}


//====================================================================================================//
// 
// 
//	CBOTDbManager
// 
// 
//====================================================================================================//


bool ns_bot_db::CBOTDbManager::create_bots()
{
	char val[128]{0};
	if (!__common.getConfig((char*)"BOT_INFO", (char*)"CNT", val)) {
		__common.log_fmt(ERR, "INI file ÀÇ BOT_INFO Á¡°Ë");
		return false;
	}

	char key_dbms[128], key_dsn[128], key_uid[128], key_pwd[128], key_company[128];
	int cnt = atoi(val);
	for (int i = 1; i < cnt + 1; i++)
	{
		sprintf(key_dbms,		"DBMS_%d",		i);
		sprintf(key_dsn,		"DSN_%d",		i);
		sprintf(key_uid,		"UID_%d",		i);
		sprintf(key_pwd,		"PWD_%d",		i);
		sprintf(key_company,	"COMPANY_%d",	i);

		if (!__common.getConfig((char*)"BOT_INFO", key_dbms, val)) return false;
		string dbms = val;

		if (!__common.getConfig((char*)"BOT_INFO", key_dsn, val)) return false;
		string dsn = val;

		if (!__common.getConfig((char*)"BOT_INFO", key_uid, val)) return false;
		string uid = val;

		if (!__common.getConfig((char*)"BOT_INFO", key_pwd, val)) return false;
		string pwd = val;

		if (!__common.getConfig((char*)"BOT_INFO", key_company, val)) return false;
		string company = val;


		DBMS_TYPE dbms_tp = (dbms.compare("MSSQL")==0)? DBMS_TYPE::MSSQL : DBMS_TYPE::MYSQL;	


		m_vec_bot.push_back(make_shared<COneBot>(dbms_tp, company, dsn, uid, pwd));

		auto it = m_vec_bot.back();
		if (!it->initialize()) {
			m_vec_bot.pop_back();
			return false;
		}
	}
	return true;
}

void ns_bot_db::CBOTDbManager::push_to_all(TBOTCandPtr& p)
{
	for (auto& it : m_vec_bot)
	{
		it->push(p);
	}
}