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

struct TBOTCandle
{
	TBOTCandle(string& tm, string& end_tm, double op, double hi, double lo, double cl, int vo)
			:candle_tm{ tm }, candle_end_tm{end_tm}, o{ op }, h{ hi }, l{ lo }, c{ cl }, v{ vo } 
			{};

	string		candle_tm;
	string		candle_end_tm;
	double		o{ 0 }, h{ 0 }, l{ 0 }, c{ 0 };
	int			v{ 0 };
};

using TCandPtr = shared_ptr<TBOTCandle>;

class COneBot
{
public:
	COneBot(string company, string dsn, string user, string pwd);
	void	push(TCandPtr&);

	bool is_connected() { return m_connector->is_connected(); }

private:
	void	thrd_main();

private:
	string						m_company;
	string						m_dsn;
	string						m_user, m_pwd;
	std::thread					m_thrd;
	std::mutex					m_mtx;
	shared_ptr<CDBConnector>	m_connector;
};

class CBOTDbSaver
{
public:
	CBOTDbSaver();
	~CBOTDbSaver();

	bool create_bots() {

	}

	void push_to_all(TCandPtr&);


private:
	vector<shared_ptr<COneBot>>		m_vec_bot;
};

}

extern ns_bot_db::CBOTDbSaver __bot_db;