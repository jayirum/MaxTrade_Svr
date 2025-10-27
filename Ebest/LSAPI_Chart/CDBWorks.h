#pragma once
#include "CDBConnector.h"
#include <string>
#include <set>

class CDBWorks
{
public:
	CDBWorks();
	~CDBWorks();

	bool	connect();

	//bool set_db_connection(CDBConnector* pDB);
	bool save_chartdata(long timeframe
		, std::string symbol
		, char* zDt_Exch
		, char* zTm_Exch
		, int tm_diff
		, double	o, double h, double l, double c
		, int v
	);

	std::set<int> load_timeframes();
	std::set<std::string> load_symbols();

	bool is_connected() { return m_pDB->is_connected(); }

private:
	CDBConnector* m_pDB;
};

extern CDBWorks __dbworks;