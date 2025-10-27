#include "stdafx.h"
#include "CSymbols.h"
#include "CGlobals.h"

CSymbols	gSymbol;


CSymbols::CSymbols()
{
	//m_oddmin_seldom = false;
}


CSymbols::~CSymbols()
{
}

bool CSymbols::Initialize(CDBConnector* pDB)
{
	m_pDB = pDB;
	return m_pDB->is_connected();
}



bool CSymbols::load_symbols()
{
	m_pDB->m_pOdbc->Init_ExecQry(gCommon.query_symbols());
	bool bNeedReconn;
	if (!m_pDB->m_pOdbc->Exec_Qry(bNeedReconn)) {
		gCommon.log(LOGTP_ERR, m_pDB->m_pOdbc->getMsg());
		m_pDB->m_pOdbc->DeInit_ExecQry();
		return false;
	}
	gCommon.log(INFO, "Load Symbols Query(%s)", gCommon.query_symbols());

	BOOL bSendSise = TRUE;
	while (m_pDB->m_pOdbc->GetNextData())
	{
		char zSymbol[128] = { 0, };

		m_pDB->m_pOdbc->GetDataStr(1, sizeof(zSymbol), zSymbol);
		m_deq_symbol.push_back(zSymbol);

		gCommon.log(INFO, "OK to load symbols from DB(%s)", zSymbol);

		Sleep(1);

	}
	m_pDB->m_pOdbc->DeInit_ExecQry();

	//JAY
	m_deq_symbol.push_back("MCAV25");
	m_deq_symbol.push_back("HSIV25");

	if(m_deq_symbol.size()==0)
	{ 
		gCommon.log(ERR, "Symbol Number is 0. Error.");
		return false;
	}

	if (m_deq_symbol.size() > (unsigned int)gCommon.max_symbol_cnt() )
	{
		gCommon.log(ERR, "Symbol Number is greater than (%d)", gCommon.max_symbol_cnt());
		return false;
	}

	return true;
}


bool CSymbols::load_timeframes()
{
	m_pDB->m_pOdbc->Init_ExecQry(gCommon.query_timeframes());
	bool bNeedReconn;
	if (!m_pDB->m_pOdbc->Exec_Qry(bNeedReconn)) {
		gCommon.log(LOGTP_ERR, m_pDB->m_pOdbc->getMsg());
		m_pDB->m_pOdbc->DeInit_ExecQry();
		return false;
	}

	gCommon.log(INFO, "Load Timeframes Query(%s)", gCommon.query_timeframes());

	int row = 0;
	BOOL bSendSise = TRUE;
	std::set<long>	set_tf;
	while (m_pDB->m_pOdbc->GetNextData())
	{
		long timeframe;
		m_pDB->m_pOdbc->GetDataLong(1, &timeframe);

		set_tf.insert(timeframe);
	}

	int loop = 0;
	for (long tf : set_tf) {
		if (loop == 0)
			m_deq_timefram_often.push_back(tf);
		else
			m_deq_timefram_seldom.push_back(tf);

		gCommon.log(INFO, "OK to load timeframe from DB(%d)", tf);

		loop++;
	}

	m_pDB->m_pOdbc->DeInit_ExecQry();

	if (m_deq_timefram_often.size() == 0 || m_deq_timefram_seldom.size() == 0)
	{
		gCommon.log(ERR, "TimeFrame Number is 0. Error.");
		return false;
	}

	return true;
}

const std::deque<std::string>&  CSymbols::get_symbol()
{
	return m_deq_symbol;
}

int  CSymbols::get_timeframe( std::deque<int>& deq_tf)
{
	deq_tf.clear();

	for (unsigned int i = 0; i < m_deq_timefram_often.size(); i++) {
		deq_tf.push_back(m_deq_timefram_often[i]);
	}

	for (unsigned int i = 0; i < m_deq_timefram_seldom.size(); i++) {
		deq_tf.push_back(m_deq_timefram_seldom[i]);
	}

	return deq_tf.size();
}

//
//int  CSymbols::get_timeframe_short(_Out_ long& timeframe, int nPrevIdx)	// 置段 -1
//{
//	int idx = nPrevIdx + 1;
//
//	int size = m_deqTFShort.size();
//
//	if (size == 0 || idx >= size)
//		return -1;
//
//	timeframe = m_deqTFShort[idx]->timeframe;
//	return idx;
//}
//
//
//int  CSymbols::get_timeframe_long(_Out_ long& timeframe, int nPrevIdx)	// 置段 -1
//{
//	int idx = nPrevIdx + 1;
//
//	int size = m_deqTFLong.size();
//
//	if (size == 0 || idx >= size)
//		return -1;
//
//	timeframe = m_deqTFLong[idx]->timeframe;
//	return idx;
//}