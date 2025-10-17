#pragma once

#include "../../Common/MaxTradeInc.h"
#include "../../Common/CODBC.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>

using namespace std;

#define PARAM_ARRAY_CNT 10
struct TConvertedData
{
	string symbol;
	string interval;
	string time_s;
	string time_e;
	string o;
	string h;
	string l;
	string c;

	void init() { time_s = time_e = symbol = interval = o = h = l = c = ""; }
};

class CDBSave
{
public: 
	CDBSave();
	~CDBSave();

	bool	create_workers(int num);
	void	push_data(string& data);
	long	get_worker_cnt() { return m_nWorkerCnt; }
private:
	bool	connect_db(int idx);
	bool	reconnect_db(int idx);
	void	threadFunc_Save(int idx);
	bool	cvt_apiData(__MAX::TData* pAPIData, _Out_ TConvertedData& cvt);
	void	compose_sp_param(int nCnt);
private:
	std::deque < __MAX::TData* >	m_Q;
	std::mutex						m_mtxQ;
	std::condition_variable			m_cvQ;

	std::vector<thread>				m_vecWorkers;
	std::vector<CODBC*>				m_vecDBs;
	bool							m_bContinue;
	__MAX::CMemoryPool				m_memPool;

	unsigned int					m_nWorkerCnt;
	char							m_zConnStr[512];
	unsigned int					m_maxDataCnt;
	unsigned int					m_dbReconnTry;

	TConvertedData					m_Cvted[PARAM_ARRAY_CNT];
	string							m_query;

	char m_t[2048];
	char m_z[2048];
};

