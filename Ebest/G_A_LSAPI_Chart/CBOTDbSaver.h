#pragma once
#include "CDBConnector.h"
#include <string>
#include <set>
#include <vector>
#include "CGlobals.h"
#include "AppCommon.h"

using namespace std;

namespace ns_bot_db
{

constexpr int PING_TIMEOUT_SEC = 30;
constexpr int MAX_CNADLE_CNT = 10;

struct TBOTCandle
{
	TBOTCandle(string& sb, int timeframe, string& tm, string& end_tm, double op, double hi, double lo, double cl, int vo)
			:symbol{sb}, tf{timeframe}, candle_tm{ tm }, candle_end_tm{end_tm}, o{ op }, h{ hi }, l{ lo }, c{ cl }, v{ vo } 
			{};
	string		symbol;
	int			tf;
	string		candle_tm;
	string		candle_end_tm;
	double		o{ 0 }, h{ 0 }, l{ 0 }, c{ 0 };
	int			v{ 0 };
};

using TBOTCandPtr = shared_ptr<TBOTCandle>;

class COneBot
{
public:
	COneBot(DBMS_TYPE dbms, string company, string ip, string port, string dbname, string user, string pwd);
	~COneBot();

	bool	initialize();
	void	push(TBOTCandPtr&);
	void	set_die(){ m_is_continue=false; }
	bool	is_connected() { return m_odbc->IsConnected(); }

private:
	bool	connect();
	void	thrd_main();
	int		compose_query(string& query);
	bool	re_connect();

private:
	DBMS_TYPE					m_dbms;
	string						m_company;
	string						m_ip, m_port, m_dbname;
	string						m_uid, m_pwd;
	std::shared_ptr <CODBC>		m_odbc;
	

	bool						m_is_continue;
	std::thread					m_thrd;
	std::mutex					m_mtx;
	vector<TBOTCandPtr>			m_queue;
};

class CBOTDbManager
{
public:
	CBOTDbManager(){};
	~CBOTDbManager(){
		//for (auto& it : m_vec_bot) { it->set_die(); }
		for (auto& it : m_vec_bot) { it->set_die(); }
	};

	bool create_bots();
	void push_to_all(TBOTCandPtr&);


private:
	vector<shared_ptr<COneBot>>		m_vec_bot;
};

}

extern ns_bot_db::CBOTDbManager __bot_manager;