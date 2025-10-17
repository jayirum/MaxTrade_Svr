#pragma once

#include "../Common/MaxTradeInc.h"
#include "../Common/CODBC.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <string>
#include <atomic>
using namespace std;

#define WORK_SEL	"배치조회"
#define WORK_BATCH	"배치실행"

struct TJob
{
	string batch_code;
	string sp_name;

	TJob(string code, string name) :batch_code(code), sp_name(name)
	{}
};

class CBatchProcess
{
public:
	CBatchProcess();
	~CBatchProcess();

	bool Initialize();


private:
	//void threadFunc_Worker();
	void threadFunc_Select();
	void threadFunc_Select_Internal();
	bool do_batch();
	bool dbConnect(CODBC* p, string work);
	bool reconnectDB(CODBC* p, string work);

private:
	string							m_sConnStr;
	
	std::unique_ptr<CODBC>			m_odbcSel;
	std::thread						m_thrdSelect;

	//std::unique_ptr<CODBC>			m_odbcWork;
	//std::thread						m_thrdWork;
	
	deque<std::unique_ptr<TJob>>	m_deqJobs;
	//std::mutex						m_mtxJobs;

	long							m_dbReconnTry;
	long							m_timeoutSelBatch;

	__MAX::CThreadFlag				m_thrdFlag;
	int								m_dbPing_timeout;
};


