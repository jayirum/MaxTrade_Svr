#include "stdafx.h"
#include "CDBWorks.h"
#include "CGlobals.h"


CDBWorks __dbworks;


CDBWorks::CDBWorks()
{
	
}

CDBWorks::~CDBWorks()
{

}

bool CDBWorks::connect()
{
	m_pDB = new CDBConnector();
	return m_pDB->connect_db();
}

bool CDBWorks::save_chartdata(const TAPIData& api)
{
	char zQ[1024];
	sprintf(zQ, 
		"SP_CHART_F_SAVE "
		"%d"	//		@I_TIMEFRAME	INT-- 1:1min, 60 : 1hour,
		",'%s'"	//@I_STK_CD		VARCHAR(10)
		",'%s'"	//@I_KOR_ymd_hms char(15)
		",%.9f"	// @I_O			DECIMAL(15, 9)
		",%.9f"	// @I_H			DECIMAL(15, 9)
		",%.9f"	// @I_L			DECIMAL(15, 9)
		",%.9f"	// @I_C			DECIMAL(15, 9)
		",%d"	// @I_V			INT)
		, std::stol(api.timeframe)
		, api.symbol.c_str()
		, api.tm_kor_ymd_hms.c_str()
		, std::stod(api.o)
		, std::stod(api.h)
		, std::stod(api.l)
		, std::stod(api.c)
		, std::stol(api.v)
	);
	bool bNeedReconn;
	m_pDB->m_pOdbc->Init_ExecQry(zQ);
	bool ret = m_pDB->m_pOdbc->Exec_Qry(bNeedReconn);
	m_pDB->m_pOdbc->DeInit_ExecQry();
	if (!ret)
	{
		__common.log_fmt(LOGTP_ERR, "[Save Chart Error](%s)(%s)", zQ, m_pDB->m_pOdbc->getMsg());
		return false;
	}
	
	return true;
}


std::set<int> CDBWorks::load_timeframes()
{
	std::set<int> set_tf{};

	__common.log_fmt(INFO, "Try to Load Timeframes Query(%s)", __common.query_timeframes());

	m_pDB->m_pOdbc->Init_ExecQry(__common.query_timeframes());
	bool bNeedReconn;
	if (!m_pDB->m_pOdbc->Exec_Qry(bNeedReconn)) {
		__common.log(LOGTP_ERR, m_pDB->m_pOdbc->getMsg());
		m_pDB->m_pOdbc->DeInit_ExecQry();
		return set_tf;
	}

	int row = 0;
	BOOL bSendSise = TRUE;
	
	while (m_pDB->m_pOdbc->GetNextData())
	{
		long timeframe;
		m_pDB->m_pOdbc->GetDataLong(1, &timeframe);

		set_tf.insert(timeframe);
		__common.log_fmt(INFO, "Load timeframe from DB(%d)", timeframe);
	}	
	m_pDB->m_pOdbc->DeInit_ExecQry();

	return std::move(set_tf);
}


std::set<std::string> CDBWorks::load_symbols()
{
	__common.log_fmt(INFO, "Try to load Symbols Query(%s)", __common.query_symbols());

	std::set<std::string> symbols{};

	m_pDB->m_pOdbc->Init_ExecQry(__common.query_symbols());
	bool bNeedReconn;
	if (!m_pDB->m_pOdbc->Exec_Qry(bNeedReconn)) {
		__common.log(LOGTP_ERR, m_pDB->m_pOdbc->getMsg());
		m_pDB->m_pOdbc->DeInit_ExecQry();
		return std::set<std::string>{};
	}	

	BOOL bSendSise = TRUE;
	while (m_pDB->m_pOdbc->GetNextData())
	{
		char zSymbol[128] = { 0, };

		m_pDB->m_pOdbc->GetDataStr(1, sizeof(zSymbol), zSymbol);
		symbols.insert(zSymbol);

		__common.log_fmt(INFO, "Load symbols from DB(%s)", zSymbol);

		Sleep(1);
	}
	m_pDB->m_pOdbc->DeInit_ExecQry();

	//if (symbols.size() > (unsigned int)__common.max_symbol_cnt())
	//{
	//	__common.log_fmt(ERR, "Symbol Number is greater than (%d)", __common.max_symbol_cnt());
	//	return std::set<std::string>{};
	//}

	return std::move(symbols);
}