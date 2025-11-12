#pragma once
#include "CDBConnector.h"
#include <string>
#include <set>
#include <vector>
#include "CGlobals.h"
#include "AppCommon.h"

using namespace std;

class CDBWorks
{
public:
	CDBWorks();
	~CDBWorks();

	bool	connect();

	//bool set_db_connection(CDBConnector* pDB);
	bool save_chartdata(const DataUnitPtr& api	);

	std::set<int> load_timeframes();
	std::vector<shared_ptr<TSymbol>> load_symbols();

	bool is_connected() { return m_pDB->is_connected(); }

private:
	CDBConnector* m_pDB;
};

extern CDBWorks __dbworks;