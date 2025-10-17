#include "CMatchProcess.h"
#include "CGlobals.h"
#include "../Common/TimeUtils.h"
#include "../Common/CODBC.h"
#include <algorithm>
#include "CMarketOrderList.h"
#include "CMarketData.h"

extern CGlobals gCommon;
extern CMarketData				gMarketData;
extern CLimitOrderListWrapper	gLimitOrdList;
extern CMarketOrderListWrapper	gMarketOrdList;

CMatchProcess::CMatchProcess(string symbol, int dotCnt) :m_symbol(symbol), m_nDotCnt(dotCnt)
{
	m_thrdFlag.set_idle();
	m_thrdLimit[(int)__MAX::BSTP_IDX::BUY] = thread(&CMatchProcess::thrdFunc_matchproc_limitOrders, this, __MAX::BSTP_IDX::BUY);
	m_thrdLimit[(int)__MAX::BSTP_IDX::SELL] = thread(&CMatchProcess::thrdFunc_matchproc_limitOrders, this, __MAX::BSTP_IDX::SELL);
	m_thrdMarket[(int)__MAX::BSTP_IDX::BUY] = thread(&CMatchProcess::thrdFunc_matchproc_marketorders, this, __MAX::BSTP_IDX::BUY);
	m_thrdMarket[(int)__MAX::BSTP_IDX::SELL] = thread(&CMatchProcess::thrdFunc_matchproc_marketorders, this, __MAX::BSTP_IDX::SELL);
	m_thrdFlag.set_run();
}
CMatchProcess::~CMatchProcess()
{
	m_thrdFlag.set_stop();

	for (int i = 0; i < 2; i++)
	{
		if (m_thrdLimit[i].joinable()) m_thrdLimit[i].join();
		if (m_thrdMarket[i].joinable()) m_thrdMarket[i].join();
	}
}


void CMatchProcess::thrdFunc_matchproc_limitOrders(__MAX::BSTP_IDX idx_bstp)
{
	string sBsTp = (idx_bstp == __MAX::BSTP_IDX::BUY) ? "매수" : "매도";

	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (gLimitOrdList.isEmpty(m_symbol))
			continue;

		TQuote* pQuote = nullptr;
		TTick* pTick = nullptr;

		if (!gMarketData.get_quotes_tick(m_symbol, pQuote, pTick))
		{
			gCommon.log(ERR, TRUE, 
				"[CMatchProcess::thrdFunc_CheckLimits][%s지정가체결처리]해당종목(%s) 의 시세/호가가 없어서 오류!!!",
				sBsTp.c_str(), m_symbol);
			Sleep(3000);
			continue;
		}

		if (!pQuote->isFilled() || !pTick->isFilled())
			continue;

		CLimitOrderList* pList;
		if (!gLimitOrdList.get_ordList_of_symbol(m_symbol, pList))
		{
			gCommon.log(ERR, TRUE, 
				"[CMatchProcess::thrdFunc_CheckLimits][%s지정가체결처리] 지정가주문리스트 가져오기 오류(get_ordList_of_symbol)(%s)", 
				sBsTp.c_str(), m_symbol.c_str());
			continue;
		}

		if (idx_bstp == __MAX::BSTP_IDX::BUY)
		{
			std::lock_guard<mutex> lock(pList->m_mutexBuysPrc);
			if (pList->isEmpty(__MAX::BUY_S))	continue;

			// 제일 높은 가격의 주문
			auto itPrcMap = pList->m_buys_byPrice.begin();//------------------------------------------------map<price, map<ord_no, orderinfo>>
			while (itPrcMap != pList->m_buys_byPrice.end())
			{
				string sOrdPrc = itPrcMap->first;
				double dOrdPrc = stod(sOrdPrc);

				if (dOrdPrc > pQuote->ask[0] ||							//---------------------------------- 반대호가 점검. 반대호가와 같거나 크면 체결
					__UTILS::is_equal_double(dOrdPrc, pQuote->ask[0])	//---------------------------------- 매수주문가 >= ask : 체결조건
					)
				{
					// 해당 가격에 달려있는 모든 주문 체결처리 (??? 지정가면 주문한 가격에 체결이 되어야 한다.)
					itPrcMap = processMatch_limitOrder_by_priceCondition(__MAX::BSTP_IDX::BUY, pQuote->ask[0], itPrcMap, pList, pQuote);
					continue;
				}
				
				// 반대호가 체결조건이 아니면, 같은 방향 호가 체결율 점검
				if (__UTILS::is_equal_double(dOrdPrc, pQuote->bid[0]))
				{
					itPrcMap = processMatch_limitOrder_by_matchRate(__MAX::BSTP_IDX::BUY, pQuote->bid[0], pQuote->vol_bestBid, itPrcMap, pList, pQuote);
				}
			}
		}
		else if (idx_bstp == __MAX::BSTP_IDX::SELL)
		{
			std::lock_guard<mutex> lock(pList->m_mutexSellsPrc);
			if (pList->isEmpty(__MAX::SELL_S))	continue;

			// 제일 높은 가격의 주문
			auto itPrcMap = pList->m_sells_byPrice.begin();//--------------------------------- map<price, map<ord_no, orderinfo>>
			while (itPrcMap != pList->m_sells_byPrice.end())
			{
				string sOrdPrc = itPrcMap->first;
				double dOrdPrc = stod(sOrdPrc);

				if (dOrdPrc < pQuote->bid[0] ||							//------------------- 반대호가 점검.
					__UTILS::is_equal_double(dOrdPrc, pQuote->bid[0])	//------------------- 매도주문가 <= bid : 체결조건
					)
				{
					// 해당 가격에 달려있는 모든 주문 체결처리
					itPrcMap = processMatch_limitOrder_by_priceCondition(__MAX::BSTP_IDX::SELL, pQuote->bid[0], itPrcMap, pList, pQuote);
					continue;
				}
				
				// 반대호가 체결조건이 아니면, 같은 방향 호가 체결율 점검
				if (__UTILS::is_equal_double(dOrdPrc, pQuote->ask[0]))
				{
					itPrcMap = processMatch_limitOrder_by_matchRate(__MAX::BSTP_IDX::SELL, pQuote->ask[0], pQuote->vol_bestAsk, itPrcMap, pList, pQuote);
				}
			}
		}	
	}
}


//
// 매수주문이 매수 1호가와 같아서 체결율에 따른 체결 처리
//
map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator
CMatchProcess::processMatch_limitOrder_by_matchRate(
	__MAX::BSTP_IDX idx_bstp, double dBestQuotePrc, double dBestQuoteVol,
	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator& itPrcMap,
	CLimitOrderList* pList,
	TQuote* pQuote
)
{
	string sBsTp = (idx_bstp == __MAX::BSTP_IDX::BUY) ? "매수" : "매도";

	auto itOrdNoMap = itPrcMap->second.begin();//-------------------------------------------- map<ord_no, orderinfo>
	while (itOrdNoMap != itPrcMap->second.end())
	{
		TLimitOrdInfo* pOrdInfo = itOrdNoMap->second.get();
		if (!pOrdInfo->match_rt.bRegistered)//----------------------------------------------- 처음으로 1호가와 같은 가격
		{
			pOrdInfo->match_rt.Register(m_match_rt, pQuote->vol_bestAsk);
			gCommon.log(INFO, TRUE,
				"[%s지정가 체결율 등록]<%s><%s>(주문번호:%d)(주문가격:%.*f)"
				"(%s 1호가:%.*f)(1호가수량:%.3f)(체결율:%.2f)(타겟감소량:%.3f)",
				sBsTp.c_str()
				, pOrdInfo->userid.c_str(), m_symbol.c_str(), pOrdInfo->no, pQuote->dot_cnt, pOrdInfo->price
				, sBsTp.c_str(), pQuote->dot_cnt, dBestQuotePrc, dBestQuoteVol, m_match_rt, pOrdInfo->match_rt.vol_target
			);

			++itOrdNoMap;
			continue;
		}
		else//------------------------------------------------------------------------------- 이미 등록되어서 체결율에 따른 점검
		{
			if (pOrdInfo->match_rt.Update(pQuote->vol_bestAsk))
			{
				gCommon.log(INFO, TRUE,
					"[%s지정가 체결율 체결]<%s><%s>(주문번호:%d)(주문가격:%.*f)"
					"(%s 1호가:%.*f)(1호가수량:%.3f)(체결율:%.2f)(타겟감소량:%.3f)",
					sBsTp.c_str()
					, pOrdInfo->userid.c_str(), m_symbol.c_str(), pOrdInfo->no, pQuote->dot_cnt, pOrdInfo->price
					, sBsTp.c_str(), pQuote->dot_cnt, dBestQuotePrc, dBestQuoteVol, m_match_rt, pOrdInfo->match_rt.vol_target
				);

				//TA001 구성
				__MAX::TData* pSendData = gCommon.memPool.Alloc();
				char sPrc[32]; sprintf(sPrc, "%.*f", m_nDotCnt, dBestQuotePrc);
				compose_returnPacket_of_match(sPrc, pOrdInfo->ordPacket, pSendData);
				gCommon.returnQ.push(pSendData);

				itOrdNoMap = itPrcMap->second.erase(itOrdNoMap);
			}
			else {
				++itOrdNoMap;
			}
		}
	} // while (itOrdNoMap != itPrcMap->second.end())

	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator returnIt;
	returnIt = (idx_bstp == __MAX::BSTP_IDX::BUY) ?
		pList->m_buys_byPrice.erase(itPrcMap) :
		pList->m_sells_byPrice.erase(itPrcMap)
		;
	return returnIt;
}

map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator
CMatchProcess::processMatch_limitOrder_by_priceCondition(
	__MAX::BSTP_IDX idx_bstp, double dBestQuotePrc,
	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator& itPrcMap,
	CLimitOrderList* pList,
	TQuote* pQuote
)
{
	string sBsTp = (idx_bstp == __MAX::BSTP_IDX::BUY) ? "매수" : "매도";

	auto itOrdNoMap = itPrcMap->second.begin();//-------------------------------------------- 주문번호 기반 내부 map loop
	while (itOrdNoMap != itPrcMap->second.end())
	{
		TLimitOrdInfo* pOrdInfo = itOrdNoMap->second.get();

		// 체결 로직
		gCommon.log(INFO, TRUE, "[%s지정가 체결처리]<%s><%s>(주문번호:%d)(주문가격:%.*f)(매도1호가:%.*f)(주문수량:%.3f)",
			sBsTp.c_str(),
			pOrdInfo->userid.c_str(),
			pOrdInfo->no,
			pQuote->dot_cnt, pOrdInfo->price,
			pQuote->dot_cnt, dBestQuotePrc,
			pOrdInfo->vol
		);

		// TA001 구성 및 메시지 큐에 추가
		__MAX::TData* pSendData = gCommon.memPool.Alloc();
		compose_returnPacket_of_match(itPrcMap->first, pOrdInfo->ordPacket, pSendData);
		gCommon.returnQ.push(pSendData);

		// 개별 주문 삭제
		itOrdNoMap = itPrcMap->second.erase(itOrdNoMap);
	}

	// 가격 레벨 삭제 후 새로운 반복자 반환
	map<string, map<long, unique_ptr<TLimitOrdInfo>>>::iterator returnIt; 
	returnIt = (idx_bstp == __MAX::BSTP_IDX::BUY) ?
				pList->m_buys_byPrice.erase(itPrcMap):
				pList->m_sells_byPrice.erase(itPrcMap)
				;
	return returnIt;
}

void CMatchProcess::thrdFunc_matchproc_marketorders(__MAX::BSTP_IDX idx_bstp)
{
	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		string last_quoted_time;
		double quote, match_prc, vol;

		if (idx_bstp == __MAX::BSTP_IDX::BUY)
		{
			if (!gMarketData.getAsk_for_match(m_symbol, quote, match_prc, vol, last_quoted_time))
			{
				gCommon.log(ERR, TRUE, "[CMatchProcess::thrdFunc_matchproc_marketorders][매수시장가체결처리]해당종목(%s) 의 시세(Ask)가 없어서 오류!!!", m_symbol);
				continue;
			}
		}
		else
		{
			if (!gMarketData.getBid_for_match(m_symbol, quote, match_prc, vol, last_quoted_time))
			{
				gCommon.log(ERR, TRUE, "[CMatchProcess::thrdFunc_matchproc_marketorders][매도시장가체결처리]해당종목(%s) 의 시세(Bid)가 없어서 오류!!!", m_symbol);
				continue;
			}
		}

		if (quote == 0 || match_prc == 0 )
			continue;

		CMarketOrderList* pList;
		if (!gMarketOrdList.get_ordList_of_symbol(m_symbol, pList))
		{
			gCommon.log(ERR, TRUE, "[CMatchProcess::thrdFunc_matchproc_marketorders][시장가체결처리] 시장가주문리스트 가져오기 오류(get_ordList_of_symbol)(%s)", m_symbol.c_str());
			continue;
		}


		std::lock_guard<mutex> lock(pList->m_mutex[(int)idx_bstp]);
		auto it = pList->m_ordList[(int)idx_bstp].begin();
		while (it != pList->m_ordList[(int)idx_bstp].end())
		{
			__MAX::TData* pSendData = gCommon.memPool.Alloc();

			if (idx_bstp == __MAX::BSTP_IDX::BUY)
			{
				gCommon.log(INFO, TRUE, "[시장가체결처리]매수주문(주문번호:%d)(종목:%s)(수량:%s)(ID:%s)(매도호가:%.*f)(체결가:%.*f)",
					(*it)->no, m_symbol.c_str(), (*it)->vol.c_str(), (*it)->userid.c_str(), m_nDotCnt, quote, m_nDotCnt, match_prc);
			}
			else
			{
				gCommon.log(INFO, TRUE, "[시장가체결처리]매도주문(주문번호:%d)(종목:%s)(수량:%s)(ID:%s)(매수호가:%.*f)(체결가:%.*f)",
					(*it)->no, m_symbol.c_str(), (*it)->vol.c_str(), (*it)->userid.c_str(), m_nDotCnt, quote, m_nDotCnt, match_prc);
			}

			//TA001 구성
			char temp[32]; sprintf(temp, "%.*f", m_nDotCnt, match_prc);
			compose_returnPacket_of_match(temp, it->get()->ordPacket, pSendData);
			gCommon.returnQ.push(pSendData);

			it = pList->m_ordList[(int)idx_bstp].erase(it);
		}
	}
}



//void CMatchProcess::compose_match_limitOrder(_In_ string ordTp, _In_ __MAX::TData* pTC001, _Out_ __MAX::TData* pTA001)
//{//
//}

void CMatchProcess::compose_returnPacket_of_match(_In_ const string& cntrPrc, _In_ string& sTC001, _Out_ __MAX::TData* pTA001)
{
	__MAX::TTC001* tc001 = (__MAX::TTC001*)sTC001.c_str();
	__MAX::TTA001* ta001 = (__MAX::TTA001*)pTA001->d;
	memset(ta001, 0x20, sizeof(__MAX::TTA001));
	*(pTA001->d + sizeof(__MAX::TTA001)) = 0x00;

	char t[128];
	sprintf(t, "%03d", (int)sizeof(__MAX::TTA001) - 2);	//Enter 길이는 들어가지 않는다.
	memcpy(ta001->header.len, t, strlen(t));
	memcpy(ta001->header.packet_cd, "TA001", 5);
	memcpy(ta001->header.userid, tc001->header.userid, sizeof(tc001->header.userid));
	memcpy(ta001->header.acnt_tp, "2", 1);
	memcpy(ta001->header.err_cd, "0000", 4);

	CTimeUtils util; sprintf(t, "%.9s", util.Time_hhmmssmmm());
	memcpy(ta001->header.tm, t, strlen(t));

	memcpy(ta001->tr_cd, "4", 1);	// 1:신규주문, 2:정정 3:취소 4,체결 5:거부
	memcpy(ta001->api_tp, "V", 1);
	memcpy(ta001->stk_cd, tc001->symbol, sizeof(tc001->symbol));
	//ta001->api_ord_no, ta001->api_cntr_no,
	memcpy(ta001->ord_no, tc001->ord_no, sizeof(tc001->ord_no));
	memcpy(ta001->ord_prc, tc001->ord_prc, sizeof(tc001->ord_prc));
	memcpy(ta001->ord_qty, tc001->ord_vol, sizeof(tc001->ord_vol));
	
	sprintf(t, "%*.*f", sizeof(ta001->cntr_prc), m_nDotCnt, stod(cntrPrc));
	memcpy(ta001->cntr_prc, t, sizeof(ta001->cntr_prc));

	memcpy(ta001->cntr_qty, ta001->ord_qty, sizeof(ta001->cntr_qty));
	ta001->remn_qty[sizeof(ta001->remn_qty) - 1] = '0';
	//ta001->api_rslt_cd, ta001->api_msg

	sprintf(t, util.DateTime_yyyymmdd_hhmmssmmm());
	memcpy(ta001->api_dt, t, sizeof(ta001->api_dt));
	memcpy(ta001->api_tm, t + 9, sizeof(ta001->api_tm));

	ta001->Enter[0] = '\r';
	ta001->Enter[1] = '\n';
}


void CMatchProcess::compose_rejectPacket(_In_ const string& sTC001, string& rjctCode, string& rjct_msg, _Out_ __MAX::TData* pTA001)
{
	__MAX::TTC001* tc001 = (__MAX::TTC001*)sTC001.c_str();
	__MAX::TTA001* ta001 = (__MAX::TTA001*)pTA001->d;
	memset(ta001, 0x20, sizeof(__MAX::TTA001));
	*(pTA001->d + sizeof(__MAX::TTA001)) = 0x00;

	char t[128];
	sprintf(t, "%03d", (int)sizeof(__MAX::TTA001) - 2);	//Enter 길이는 들어가지 않는다.
	memcpy(ta001->header.len, t, strlen(t));
	memcpy(ta001->header.packet_cd, "TA001", 5);
	memcpy(ta001->header.userid, tc001->header.userid, sizeof(tc001->header.userid));
	memcpy(ta001->header.acnt_tp, "2", 1);
	memcpy(ta001->header.err_cd, "0000", 4);

	CTimeUtils util; sprintf(t, "%.9s", util.Time_hhmmssmmm());
	memcpy(ta001->header.tm, t, strlen(t));
	memcpy(ta001->tr_cd, "5", 1);	// 1:신규주문, 2:정정 3:취소 4,체결 5:거부
	memcpy(ta001->api_tp, "V", 1);
	memcpy(ta001->stk_cd, tc001->symbol, sizeof(tc001->symbol));
	//ta001->api_ord_no, ta001->api_cntr_no,
	memcpy(ta001->ord_no, tc001->ord_no, sizeof(tc001->ord_no));
	memcpy(ta001->ord_prc, tc001->ord_prc, sizeof(tc001->ord_prc));
	memcpy(ta001->ord_qty, tc001->ord_vol, sizeof(tc001->ord_vol));
	ta001->cntr_prc[sizeof(ta001->cntr_prc) - 1] = '0';
	ta001->cntr_qty[sizeof(ta001->cntr_qty) - 1] = '0';
	memcpy(ta001->remn_qty, tc001->ord_vol, sizeof(tc001->ord_vol));
	memcpy(ta001->api_rslt_cd, rjctCode.c_str(), MIN(sizeof(ta001->api_rslt_cd), rjctCode.size()));
	memcpy(ta001->api_msg, rjct_msg.c_str(), MIN(sizeof(ta001->api_msg), rjct_msg.size()));

	sprintf(t, util.DateTime_yyyymmdd_hhmmssmmm());
	memcpy(ta001->api_dt, t, sizeof(ta001->api_dt));
	memcpy(ta001->api_tm, t + 9, sizeof(ta001->api_tm));

	ta001->Enter[0] = '\r';
	ta001->Enter[1] = '\n';
	//ta001->Enter[2] = 0x00;
}


////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////

bool CMatchProcessWrapper::create_matchprocesses()
{

	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };

	if (!gCommon.getConfig((char*)"DB_INFO", (char*)"DSN", zDsn) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"UID", zUid) ||
		!gCommon.getConfig((char*)"DB_INFO", (char*)"PWD", zPwd)
		)
	{
		gCommon.log(LOGTP_ERR, TRUE, "[CMatchProcessWrapper::create_matchprocesses]DB정보 조회 오류.INI파일을 확인하세요");
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
		gCommon.log(LOGTP_ERR, TRUE, "[CMatchProcessWrapper::create_matchprocesses]DB Connect 오류:%s", odbc.getMsg());
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
		gCommon.log(LOGTP_ERR, TRUE, odbc.getMsg());
		return false;
	}

	int row = 0;
	BOOL bSendSise = TRUE;
	while (odbc.GetNextData())
	{
		char zSymbol[128] = { 0, };
		long dotCnt;

		odbc.GetDataStr(1, sizeof(zSymbol), zSymbol);
		odbc.GetDataLong(2, &dotCnt);


		//TODO
		if (std::string(zSymbol).compare("BTCUSDT") != 0)
			continue;


		m_mapMatches[zSymbol] = std::make_unique<CMatchProcess>(zSymbol, dotCnt);

		gCommon.log(INFO, TRUE, "[%s]CMatchProcess 생성", zSymbol);
	}
	odbc.DeInit_ExecQry();

	if (m_mapMatches.size() == 0) {
		gCommon.log(ERR, TRUE, "[CMarketData 종목정보 조회 오류!!!](%s)", zQ);
		return false;
	}

	return true;
}