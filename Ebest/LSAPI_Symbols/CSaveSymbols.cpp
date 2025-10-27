#include "stdafx.h"
#include "CSaveSymbols.h"
#include "CGlobals.h"


CSaveSymbols gSaveCandle;


CSaveSymbols::CSaveSymbols()
{
	
}

CSaveSymbols::~CSaveSymbols()
{

}

bool CSaveSymbols::set_db_connection(CDBConnector* pDB)
{
	m_pDB = pDB;
	return m_pDB->is_connected();
}

bool CSaveSymbols::save_kf_master(char* code, char* name)
{
	//char zQ[1024];
	//sprintf(zQ, 
	//	"CHART_SAVE "
	//	"%d"	//		@I_TIMEFRAME	INT-- 1:1min, 60 : 1hour,
	//	",'%s'"	//@I_STK_CD		VARCHAR(10)
	//	",'%s'"	//@I_DT_EXCH		CHAR(8)
	//	",'%s'"	//@I_TM_EXCH		char(6)
	//	",%d"	// @I_TM_DIFF		INT
	//	",%.9f"	// @I_O			DECIMAL(15, 9)
	//	",%.9f"	// @I_H			DECIMAL(15, 9)
	//	",%.9f"	// @I_L			DECIMAL(15, 9)
	//	",%.9f"	// @I_C			DECIMAL(15, 9)
	//	",%d"	// @I_V			INT)
	//	,timeframe
	//	, symbol.c_str()
	//	, zDt_Exch
	//	, zTm_Exch
	//	, tm_diff
	//	, o
	//	, h
	//	, l
	//	, c
	//	, v
	//);
	//bool bNeedReconn;
	//m_pDB->m_pOdbc->Init_ExecQry(zQ);
	//bool ret = m_pDB->m_pOdbc->Exec_Qry(bNeedReconn);
	//m_pDB->m_pOdbc->DeInit_ExecQry();
	//if (!ret)
	//{
	//	gCommon.log(LOGTP_ERR, "[Save Chart Error](%s)(%s)", zQ, m_pDB->m_pOdbc->getMsg());
	//	return false;
	//}
	
	gCommon.debug("[save_kf_master] Code(%s) Name(%s)", code, name);
	return true;
}

bool CSaveSymbols::save_ov_master(char* code, char* name, char* lstng_y, char* lstng_m)
{
	gCommon.debug("[save_ov_master] Code(%s) Name(%s) Listing Y(%s) Listing M(%s)", code, name, lstng_y, lstng_m);
	return true;
}

bool CSaveSymbols::save_ov_info(char* code, char* name, char* mtrt_dt)
{
	gCommon.debug("[save_ov_info] Code(%s) Name(%s) 최종거래일(%s)", code, name, mtrt_dt);
	return true;
}