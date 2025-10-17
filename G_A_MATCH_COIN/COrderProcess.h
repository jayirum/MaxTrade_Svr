#pragma once

#include "../Common/MaxTradeInc.h"
#include "CGlobals.h"
#include <thread>
#include <string>
#include <boost/lockfree/queue.hpp>

using namespace std;


class COrderProcess
{
public:
	COrderProcess();
	~COrderProcess();

	bool AcceptOrder(string& ordPacket); // depoloy order by order type(market, limit, mit)

private:
	void threadFunc_Limit();
	void threadFunc_Market();
	void threadFunc_Mit();
	bool is_marketdata_started(_In_ string& sPacket);

	void compose_confirm_limitOrder(_In_ string ordTp,  _In_ __MAX::TData* pTC001, _Out_ __MAX::TData* pTA001);
	void compose_reject(_In_ __MAX::TData* pTC001,	string rjctCode, string rjct_msg, _Out_ __MAX::TData* pTA001);
	void compose_reject(_In_ string& sPacket,		string rjctCode, string rjct_msg, _Out_ __MAX::TData* pTA001);
private:

	thread					m_thrdLimit, m_thrdMarket;	//, m_thrdMit;
	unsigned int			m_idLimit, m_idMarket;		//, m_idMit;
	__MAX::CThreadFlag		m_thrdFlag;
}
;
