#pragma once
#include "CDBConnector.h"
#include <string>
#include "t9943.h"
#include "o3101.h"
#include "o3105.h"

class CSaveSymbols
{
public:
	CSaveSymbols();
	~CSaveSymbols();

	bool set_db_connection(CDBConnector* pDB);
	bool save_kf_master(char* code, char* name);
	bool save_ov_master(char* code, char* name, char* lstng_y, char* lstng_m);
	bool save_ov_info(char* code, char* name, char* mtrt_dt);


private:
	CDBConnector* m_pDB;
};

extern CSaveSymbols gSaveSymbols;