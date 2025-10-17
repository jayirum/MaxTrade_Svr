#include "COrderProcess.h"
#include "CGlobals.h"
#include "../Common/TimeUtils.h"
#include <algorithm>
#include "CLimitOrders_bySymbol.h"
#include "CMarketOrders_bySymbol.h"
#include "CMarketData.h"

extern CGlobals							gCommon;
extern CMarketData						gMarketData;
extern map< string, CLimitOrders_bySymbol*>		gLimitOrdList;
extern map< string, CMarketOrders_bySymbol*>	gMarketOrdList;


COrderProcess::COrderProcess()
{
	m_thrdFlag.set_idle();
	m_thrdLimit = std::thread(&COrderProcess::threadFunc_Limit, this);
	m_thrdMarket = std::thread(&COrderProcess::threadFunc_Market, this);
	m_thrdFlag.set_run();

	m_idLimit = GetThreadId(m_thrdLimit.native_handle());
	m_idMarket = GetThreadId(m_thrdMarket.native_handle());
}

COrderProcess::~COrderProcess()
{
	if (m_thrdLimit.joinable())		m_thrdLimit.join();
	if (m_thrdMarket.joinable())	m_thrdMarket.join();
}

bool COrderProcess::is_marketdata_started(_In_ string& sPacket)
{
	__MAX::TTC001* p = (__MAX::TTC001*)sPacket.c_str();
	string symbol = __UTILS::trim(std::string(p->symbol, sizeof(p->symbol)));
	
	return gMarketData.is_started(symbol);
}

bool COrderProcess::AcceptOrder(string& ordPacket) // depoloy order by order type(market, limit, mit)
{
	__MAX::TData* pData = gCommon.memPool.Alloc();
	pData->init();

	if (!is_marketdata_started(ordPacket))
	{
		gCommon.log(ERR, TRUE, "[COrderProcess::AcceptOrder] 시세 저장전이라 주문접수 불가");
		compose_reject(ordPacket, "9999", "아직 시세 수신 전이라 거래 불가합니다. 잠시 후 다시 시도해주세요", pData);
		gCommon.returnQ.push(pData);
		return false;
	}

	memcpy(pData->d, ordPacket.c_str(), ordPacket.size());
	__MAX::TTC001* p = (__MAX::TTC001*)ordPacket.c_str();
	

	// 시장가, 지정가를 각기 다른 스레드에서 접수하도록
	string ord_tp = __UTILS::trim(std::string(p->ord_tp, sizeof(p->ord_tp)));
	if (ord_tp == __MAX::ORD_TP_MARKET		||
		ord_tp == __MAX::ORD_TP_CLR_SYMBOL	||
		ord_tp == __MAX::ORD_TP_CLR_ALL		)
	{
		PostThreadMessage(m_idMarket, __MAX::WM_ORDER_SEND, (WPARAM)0, (LPARAM)pData);
	}
	else if (ord_tp == __MAX::ORD_TP_LIMIT || 
			ord_tp == __MAX::ORD_TP_CANCEL || 
			ord_tp == __MAX::ORD_TP_MODIFY ||
			ord_tp == __MAX::ORD_TP_CNCL_SYMBOL ||
			ord_tp == __MAX::ORD_TP_CNCL_ALL
		) 
	{
		PostThreadMessage(m_idLimit, __MAX::WM_ORDER_SEND, (WPARAM)0, (LPARAM)pData);
	}
	//else if (ord_tp == __MAX::ORD_TP_MIT) {
	//	PostThreadMessage(m_idMit, WM_ORDER_SEND, (WPARAM)0, (LPARAM)pData);
	//}
	return true;
}


void COrderProcess::threadFunc_Limit()
{
	string ret_code, ret_msg;
	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		
		MSG PeekMsg;
		while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (PeekMsg.message != __MAX::WM_ORDER_SEND){
				break;
			}

			__MAX::TData* pRecvData = reinterpret_cast<__MAX::TData*>(PeekMsg.lParam);
			__MAX::TTC001* tc001 = (__MAX::TTC001*)pRecvData->d;
			string symbol = __UTILS::trim(std::string(tc001->symbol, sizeof(tc001->symbol)));
			__MAX::TData* pSendData = gCommon.memPool.Alloc();

			gCommon.debug("[threadFunc_Limit 데이터 수신](%s)", pRecvData->d);

			auto it = gLimitOrdList.find(symbol);
			if (it != gLimitOrdList.end())
			{
				CLimitOrders_bySymbol* pList = it->second;

				string ordTp = __UTILS::trim(std::string(tc001->ord_tp, sizeof(tc001->ord_tp)));
				if (ordTp == __MAX::ORD_TP_LIMIT)
				{
					gCommon.debug("[COrderProcess::threadFunc_Limit-3] call CLimitOrdList::AddOrder");
					if (pList->AddOrder(pRecvData->d, ret_code, ret_msg))
					{
						compose_confirm_limitOrder(ordTp, pRecvData, pSendData);
					}
					else
					{
						gCommon.log(ERR, TRUE, "[지정가주문접수오류]주문리스트 가져오기 실패(%s)(%s)", symbol.c_str(), pRecvData->d);
						compose_reject(pRecvData, ret_code, ret_msg, pSendData);
					}
				}
				else if (ordTp == __MAX::ORD_TP_MODIFY)
				{
					gCommon.debug("[COrderProcess::threadFunc_Limit-3] call CLimitOrdList::ModifyOrder");
					if (pList->ModifyOrder(pRecvData->d, ret_code, ret_msg))
					{
						compose_confirm_limitOrder(ordTp, pRecvData, pSendData);
						gCommon.debug("[Modify Order-RETURN CLIENT](%s)", pSendData->d);
					}
					else
					{
						compose_reject(pRecvData, ret_code, ret_msg, pSendData);
						gCommon.log(ERR, TRUE, "[정정주문 접수 오류]정정주문 처리 중 오류(%s)(%s)", symbol.c_str(), pRecvData->d);
					}
				}
				else if (ordTp == __MAX::ORD_TP_CANCEL ||
					ordTp == __MAX::ORD_TP_CNCL_SYMBOL ||
					ordTp == __MAX::ORD_TP_CNCL_ALL)
				{
					gCommon.debug("[COrderProcess::threadFunc_Limit-3](ordTp:%s) call CLimitOrdList::DelOrder", ordTp.c_str());
					if (pList->DelOrder(pRecvData->d, ret_code, ret_msg))
					{
						compose_confirm_limitOrder(ordTp, pRecvData, pSendData);
					}
					else
					{
						compose_reject(pRecvData, ret_code, ret_msg, pSendData);
						gCommon.log(ERR, TRUE, "[취소주문 접수 오류]취소주문 처리 중 오류(%s)(%s)", symbol.c_str(), pRecvData->d);
					}
				}
			}
			else
			{
				gCommon.log(ERR, TRUE, "[지정가/정정/취소주문접수오류]주문리스트 가져오기 실패(%s)(%s)", symbol.c_str(), pRecvData->d);
				string code(__MAX::ERR_WITH_MSG);
				string msg("체결서버 처리중 예기치 못한 오류 입니다.(지정가주문 리스트 오류)");
				compose_reject(pRecvData, code, msg, pSendData);
			}
			gCommon.returnQ.push(pSendData);

			gCommon.memPool.release(pRecvData);
		} // while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
	}
}


void COrderProcess::threadFunc_Market()
{
	while (!m_thrdFlag.is_stopped())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));

		MSG PeekMsg;
		while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (PeekMsg.message != __MAX::WM_ORDER_SEND) {
				break;
			}

			__MAX::TData* pRecvData = reinterpret_cast<__MAX::TData*>(PeekMsg.lParam);
			__MAX::TTC001* tc001 = (__MAX::TTC001*)pRecvData->d;
			__MAX::TData* pSendData = gCommon.memPool.Alloc();

			gCommon.debug("[COrderProcess::threadFunc_Market-1]pop data(%s)", pRecvData->d);
			string symbol = __UTILS::trim(std::string(tc001->symbol, sizeof(tc001->symbol)));

			bool bSucc = false;
			auto it = gMarketOrdList.find(symbol);
			if (it != gMarketOrdList.end())
			{
				CMarketOrders_bySymbol* pList = it->second;

				string ret_code, ret_msg;
				string ordTp = __UTILS::trim(std::string(tc001->ord_tp, sizeof(tc001->ord_tp)));
				gCommon.debug("[COrderProcess::threadFunc_Market-2]ordTp:%s", ordTp.c_str());

				if (pList->AddOrder(pRecvData->d, ret_code, ret_msg))
				{
					bSucc = true;
					gCommon.debug("[New Market Order-Added](%s)", pSendData->d);
				}
				else
				{
					compose_reject(pRecvData, ret_code, ret_msg, pSendData);
					gCommon.log(ERR, TRUE, "[COrderProcess::threadFunc_Market]시장가 신규주문 오류(%s)(%s)", ret_msg.c_str(), pRecvData->d);
				}
			}
			else
			{
				gCommon.log(ERR, TRUE, "[시장가주문접수오류]주문리스트 가져오기 실패(%s)(%s)", symbol.c_str(), pRecvData->d);
				string code(__MAX::ERR_WITH_MSG);
				string msg("체결서버 처리중 예기치 못한 오류 입니다.(시장가주문 리스트 오류)");
				compose_reject(pRecvData, code, msg, pSendData);
			}
			if(!bSucc)
				gCommon.returnQ.push(pSendData);

			gCommon.memPool.release(pRecvData);
		} // while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
	}
}


void COrderProcess::compose_confirm_limitOrder(_In_ string ordTp, _In_ __MAX::TData* pTC001, _Out_ __MAX::TData* pTA001)
{
	__MAX::TTC001* tc001 = (__MAX::TTC001*)pTC001->d;
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

	if (ordTp == __MAX::ORD_TP_LIMIT) {
		memcpy(ta001->tr_cd, "1", 1);	// 1:신규주문, 2:정정 3:취소 4,체결 5:거부
	}
	else if (ordTp == __MAX::ORD_TP_MODIFY){
		memcpy(ta001->tr_cd, "2", 1);	// 1:신규주문, 2:정정 3:취소 4,체결 5:거부
	}
	if (ordTp == __MAX::ORD_TP_CANCEL ||
		ordTp == __MAX::ORD_TP_CNCL_SYMBOL ||
		ordTp == __MAX::ORD_TP_CNCL_ALL
		) {
		memcpy(ta001->tr_cd, "3", 1);	// 1:신규주문, 2:정정 3:취소 4,체결 5:거부
	}
	memcpy(ta001->api_tp, "V", 1);
	memcpy(ta001->stk_cd, tc001->symbol, sizeof(tc001->symbol));
	//ta001->api_ord_no, ta001->api_cntr_no,
	memcpy(ta001->ord_no, tc001->ord_no, sizeof(tc001->ord_no));
	memcpy(ta001->ord_prc, tc001->ord_prc, sizeof(tc001->ord_prc));
	memcpy(ta001->ord_qty, tc001->ord_vol, sizeof(tc001->ord_vol));
	ta001->cntr_prc[sizeof(ta001->cntr_prc) - 1] = '0';
	ta001->cntr_qty[sizeof(ta001->cntr_qty) - 1] = '0';
	memcpy(ta001->remn_qty, tc001->ord_vol, sizeof(tc001->ord_vol));
	//ta001->api_rslt_cd, ta001->api_msg

	sprintf(t, util.DateTime_yyyymmdd_hhmmssmmm());
	memcpy(ta001->api_dt, t, sizeof(ta001->api_dt));
	memcpy(ta001->api_tm, t + 9, sizeof(ta001->api_tm));

	ta001->Enter[0] = '\r';
	ta001->Enter[1] = '\n';
	//ta001->Enter[2] = 0x00;
}

void COrderProcess::compose_reject(_In_ __MAX::TData* pTC001, string rjctCode, string rjct_msg, _Out_ __MAX::TData* pTA001)
{
	string packet = string(pTC001->d);
	compose_reject(packet, rjctCode, rjct_msg, pTA001);
}

void COrderProcess::compose_reject(_In_ string& sPacket, string rjctCode, string rjct_msg, _Out_ __MAX::TData* pTA001)
{
	__MAX::TTC001* tc001 = (__MAX::TTC001*)sPacket.c_str();
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