#pragma once
#include "CDBConnector.h"
#include <string>

class CSaveCandle
{
public:
	CSaveCandle();
	~CSaveCandle();

	bool set_db_connection(CDBConnector* pDB);
	bool save(long timeframe
		, std::string symbol
		, char* zDt_Exch
		, char* zTm_Exch
		, int tm_diff
		, double	o, double h, double l, double c
		, int v
	);

private:
	CDBConnector* m_pDB;
};

extern CSaveCandle gSaveCandle;