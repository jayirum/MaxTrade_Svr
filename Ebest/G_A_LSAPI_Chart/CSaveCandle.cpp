#include "stdafx.h"
#include "CSaveCandle.h"
#include "CGlobals.h"


CSaveCandle gSaveCandle;


CSaveCandle::CSaveCandle()
{
	
}

CSaveCandle::~CSaveCandle()
{

}

bool CSaveCandle::set_db_connection(CDBConnector* pDB)
{
	m_pDB = pDB;
	return m_pDB->is_connected();
}

bool CSaveCandle::save(long timeframe
	, std::string symbol
	, char* zDt_Exch
	, char* zTm_Exch
	, int tm_diff
	, double	o, double h, double l, double c, int v
)
{
	char zQ[1024];
	sprintf(zQ, 
		"CHART_SAVE "
		"%d"	//		@I_TIMEFRAME	INT-- 1:1min, 60 : 1hour,
		",'%s'"	//@I_STK_CD		VARCHAR(10)
		",'%s'"	//@I_DT_EXCH		CHAR(8)
		",'%s'"	//@I_TM_EXCH		char(6)
		",%d"	// @I_TM_DIFF		INT
		",%.9f"	// @I_O			DECIMAL(15, 9)
		",%.9f"	// @I_H			DECIMAL(15, 9)
		",%.9f"	// @I_L			DECIMAL(15, 9)
		",%.9f"	// @I_C			DECIMAL(15, 9)
		",%d"	// @I_V			INT)
		,timeframe
		, symbol.c_str()
		, zDt_Exch
		, zTm_Exch
		, tm_diff
		, o
		, h
		, l
		, c
		, v
	);
	bool bNeedReconn;
	m_pDB->m_pOdbc->Init_ExecQry(zQ);
	bool ret = m_pDB->m_pOdbc->Exec_Qry(bNeedReconn);
	m_pDB->m_pOdbc->DeInit_ExecQry();
	if (!ret)
	{
		gCommon.log(LOGTP_ERR, "[Save Chart Error](%s)(%s)", zQ, m_pDB->m_pOdbc->getMsg());
		return false;
	}
	//gCommon.debug("[Save Chart OK](%s)", zQ);
	return true;
}