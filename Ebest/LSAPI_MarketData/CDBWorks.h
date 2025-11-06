#pragma once
#include "CDBConnector.h"
#include <string>
#include <set>
#include "CGlobals.h"

class CDBWorks
{
public:
	CDBWorks();
	~CDBWorks();

	bool	connect();

	//bool set_db_connection(CDBConnector* pDB);
	bool save_chartdata(const TAPIData& api	);

	std::set<int> load_timeframes();
	std::set<std::string> load_symbols();

	bool is_connected() { return m_pDB->is_connected(); }

private:
	CDBConnector* m_pDB;
};

extern CDBWorks __dbworks;