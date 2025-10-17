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



class CMSSQLSave
{
public: 
	CMSSQLSave();
	~CMSSQLSave();

	bool	create_workers(int num=1);
	void	push_data(std::shared_ptr< TConvertedData> cvt);
	long	get_worker_cnt() { return m_nWorkerCnt; }
private:
	bool	connect_db();
	bool	reconnect_db();
	void	threadFunc_Save(int idx);
	void	threadFunc_Save_Internal(int idx);
	void	Save_to_Db(deque<shared_ptr< TConvertedData>>& cvt);
private:
	std::deque < std::shared_ptr< TConvertedData> >	m_Q;
	std::mutex						m_mtxQ;
	std::condition_variable			m_cvQ;

	std::vector<thread>				m_vecWorkers;
	CODBC*							m_pOdbc;
	__MAX::CMemoryPool				m_memPool;
	__MAX::CThreadFlag				m_thrdFlag;

	unsigned int					m_nWorkerCnt;
	char							m_zConnStr[512];
	unsigned int					m_maxDataCnt;
	unsigned int					m_dbReconnTry;

	int								m_pingTimeout_sec;

};

