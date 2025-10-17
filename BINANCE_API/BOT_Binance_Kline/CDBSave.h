#pragma once

#include "../../Common/MaxTradeInc.h"
#include "../../Common/CODBC.h"
#include "main.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>

using namespace std;


class CDBSave
{
public: 
	CDBSave();
	~CDBSave();

	bool	create_workers(int num);
	void	push_data(std::shared_ptr< TConvertedData> cvt);
	long	get_worker_cnt() { return m_nWorkerCnt; }
private:
	bool	connect_db(int idx);
	bool	reconnect_db(int idx);
	void	threadFunc_Save(int idx);
	void	threadFunc_Save_Internal(int idx);
	void	compose_sp_param(deque<shared_ptr< TConvertedData>>& cvt, _Out_ string& query);
private:
	std::deque < std::shared_ptr< TConvertedData> >	m_Q;
	std::mutex						m_mtxQ;
	std::condition_variable			m_cvQ;

	std::vector<thread>				m_vecWorkers;
	std::vector<CODBC*>				m_vecDBs;
	__MAX::CMemoryPool				m_memPool;
	__MAX::CThreadFlag				m_thrdFlag;

	unsigned int					m_nWorkerCnt;
	char							m_zConnStr[512];
	unsigned int					m_maxDataCnt;
	unsigned int					m_dbReconnTry;

	int								m_pingTimeout_sec;

	//deque<unique_ptr< TConvertedData>>	m_deqCvts;
	//string								m_query;

	//char m_t[2048];
	//char m_z[2048];
};

