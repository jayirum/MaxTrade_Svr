#include "CMarketData.h"
#include "../Common/CODBC.h"
#include "CGlobals.h"

extern CGlobals		gCommon;

CMarketData::CMarketData()
{
}

CMarketData::~CMarketData()
{
}

bool	CMarketData::Initialize()
{

	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "DB정보 조회 오류.INI파일을 확인하세요");
		return FALSE;
	}

	char zConnStr[512] = { 0 };
	sprintf(zConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	CODBC odbc(DBMS_TYPE::MSSQL);
	if (!odbc.Initialize())
	{
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}
	if (!odbc.Connect(zConnStr))
	{
		gCommon.log(LOGTP_ERR, TRUE, "DB Connect 오류:%s", odbc.getMsg());
		return false;
	}


	char zQ[1024];
	if (!gCommon.getConfigQuery((char*)"QUERY", (char*)"LOAD_SYMBOLS", zQ)) {
		gCommon.log(LOGTP_ERR, TRUE, "[종목정보 조회 쿼리 가져오기 오류!!!]ini 파일 확인(QUERY, LOAD_SYMBOLS)");
		return false;
	}
	bool bReconn;
	odbc.Init_ExecQry(zQ);
	if (!odbc.Exec_Qry(bReconn)) {
		gCommon.log(LOGTP_ERR, TRUE, "[종목정보 가져오기 오류!!!](%s)(%s)", zQ, odbc.getMsg());
		return false;
	}

	gCommon.log(INFO, TRUE, "[MarketData]Load Symbols Query(%s)", zQ);

	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zSymbol[128] = { 0, };
		long dotCnt = 0;

		odbc.GetDataStr(1, sizeof(zSymbol), zSymbol);
		odbc.GetDataLong(2, &dotCnt);

		m_mapTick[zSymbol] = std::make_unique<TTick>(dotCnt);
		m_mapQuote[zSymbol] = std::make_unique<TQuote>(dotCnt);

		gCommon.log(INFO, TRUE, "[MarketData저장소](symbol:%s)(DotCnt:%d)", zSymbol, dotCnt);
	}
	odbc.DeInit_ExecQry();

	if (m_mapTick.size() == 0) {
		gCommon.log(ERR, TRUE, "[CMarketData 종목정보 조회 오류!!!](%s)", zQ);
		return false;
	}

	return true;
}

bool CMarketData::save_tick(char* apiData)
{
	bool ret = false;

	__MAX::TRA001* pNew = (__MAX::TRA001*)apiData;

	std::string symbol	= __UTILS::trim(string(pNew->header.stk_cd, sizeof(pNew->header.stk_cd)));

	std::lock_guard<mutex>lock(m_mxTick);

	auto it = m_mapTick.find(symbol);
	if (it!=m_mapTick.end()) 
	{
		ret = true;
		std::unique_ptr<TTick>& tick = m_mapTick[symbol];
		bool bFilled = tick->isFilled();
		tick->o = __UTILS::stod_s(string(pNew->open, sizeof(pNew->open)));
		tick->h = __UTILS::stod_s(string(pNew->high, sizeof(pNew->high)));
		tick->l = __UTILS::stod_s(string(pNew->low, sizeof(pNew->low)));
		tick->c = __UTILS::stod_s(string(pNew->now_prc, sizeof(pNew->now_prc)));
		tick->time = __UTILS::trim(string(pNew->tm, sizeof(pNew->tm)));
		tick->update_lastTime();
		if (!bFilled) {
			gCommon.debug("[최초 TICK 저장(%s)](o:%.3f)(h:%.3f)(l:%.3f)(c:%.3f)(last time:%s)",
				symbol.c_str(), tick->o, tick->h, tick->l, tick->c, tick->lastUpdateTm.c_str());
		}
	}
	else
	{
		gCommon.log(ERR, TRUE, "[CMarketData::save_tick](%s)종목이 없음", symbol.c_str());
	}

	return ret;
}


bool CMarketData::save_quote(char* apiData)
{
	bool ret = false;

	__MAX::TRA002* pNew = (__MAX::TRA002*)apiData;

	string symbol	= __UTILS::trim(string(pNew->header.stk_cd, sizeof(pNew->header.stk_cd)));

	std::lock_guard<mutex>lock(m_mxQuote);

	auto it = m_mapQuote.find(symbol);
	if (it != m_mapQuote.end()) {
		ret = true;
		std::unique_ptr<TQuote>& quote = it->second;
		bool bFilled = quote->isFilled();

		quote->bid[0] = __UTILS::stod_s(string(pNew->b_hoga_1, sizeof(pNew->b_hoga_1)));
		quote->bid[1] = __UTILS::stod_s(string(pNew->b_hoga_2, sizeof(pNew->b_hoga_2)));
		quote->bid[2] = __UTILS::stod_s(string(pNew->b_hoga_3, sizeof(pNew->b_hoga_3)));
		quote->bid[3] = __UTILS::stod_s(string(pNew->b_hoga_4, sizeof(pNew->b_hoga_4)));
		quote->bid[4] = __UTILS::stod_s(string(pNew->b_hoga_5, sizeof(pNew->b_hoga_5)));

		quote->ask[0] = __UTILS::stod_s(string(pNew->s_hoga_1, sizeof(pNew->s_hoga_1)));
		quote->ask[1] = __UTILS::stod_s(string(pNew->s_hoga_2, sizeof(pNew->s_hoga_2)));
		quote->ask[2] = __UTILS::stod_s(string(pNew->s_hoga_3, sizeof(pNew->s_hoga_3)));
		quote->ask[3] = __UTILS::stod_s(string(pNew->s_hoga_4, sizeof(pNew->s_hoga_4)));
		quote->ask[4] = __UTILS::stod_s(string(pNew->s_hoga_5, sizeof(pNew->s_hoga_5)));

		quote->vol_bestBid = __UTILS::stod_s(string(pNew->b_vol_1, sizeof(pNew->b_vol_1)));
		quote->vol_bestAsk = __UTILS::stod_s(string(pNew->s_vol_1, sizeof(pNew->s_vol_1)));

		quote->time = __UTILS::trim(string(pNew->tm, sizeof(pNew->tm)));
		quote->update_lastTime();

		if (!bFilled) {
			gCommon.debug("[최초 QUOTE 저장(%s)](bid=>%.5f/%.5f/%.5f/%.5f/%.5f)(ask=/%.5f/%.5f/%.5f/%.5f/%.5f)(bid_vol:%.3f)(ask_vol:%.3f)(last time:%s)",
				symbol.c_str(), quote->bid[0], quote->bid[1], quote->bid[2], quote->bid[3], quote->bid[4],
				quote->ask[0], quote->ask[1], quote->ask[2], quote->ask[3], quote->ask[4], 
				quote->vol_bestBid, quote->vol_bestAsk, quote->lastUpdateTm.c_str());
			gCommon.debug(apiData);
		}
	}
	else
	{
		gCommon.log(ERR, TRUE, "[CMarketData::save_quote](%s)종목이 없음(%s)", symbol.c_str(), apiData);
	}
	return ret;
}


bool CMarketData::getBid_for_match(std::string& symbol, _Out_ double& bid, _Out_ double& match_prc, _Out_ double& vol, _Out_ string& lastTime)
{
	bool found = true;

	// lock 불필요 - map 에 변화가 없음
	auto it = m_mapQuote.find(symbol);
	if (it == m_mapQuote.end()){
		return (!found);
	}
	TQuote* quote = it->second.get();
	bid = quote->bid[0];
	vol = quote->vol_bestBid;

	double high = bid;

	auto itTick = m_mapTick.find(symbol);
	if (itTick != m_mapTick.end()) {
		TTick* tick = itTick->second.get();
		high = (tick->h==0)? high:tick->h;
	}
	match_prc = (bid > high) ? high : bid;
	lastTime = quote->lastUpdateTm;
	return (found);
}


bool CMarketData::getAsk_for_match(std::string& symbol, _Out_ double& ask, _Out_ double& match_prc, _Out_ double& vol, _Out_ string& lastTime)
{
	bool found = true;

	auto it = m_mapQuote.find(symbol);
	if (it == m_mapQuote.end()) {
		return (!found);
	}
	TQuote* quote = it->second.get();
	ask = quote->ask[0];
	vol = quote->vol_bestAsk;
	
	double low = ask;

	auto itTick = m_mapTick.find(symbol);
	if (itTick == m_mapTick.end()) {
		TTick* tick = itTick->second.get();
		low = (tick->l==0)? low : tick->l;
	}
		
	match_prc = (ask < low) ? low: ask;
	lastTime = quote->lastUpdateTm;

	return found;

}


bool CMarketData::get_quotes_tick(string& symbol, _Out_ TQuote*& quote, _Out_ TTick*& tick)
{
	bool found = true;

	auto it = m_mapQuote.find(symbol);
	if (it == m_mapQuote.end()) {
		return (!found);
	}

	auto itTick = m_mapTick.find(symbol);
	if (itTick == m_mapTick.end()) {
		return (!found);
	}

	quote = it->second.get();
	tick = itTick->second.get();
	return true;
}
