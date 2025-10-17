#pragma once

//#include <deque>
#include "../Common/MaxTradeInc.h"
#include <string>
#include <memory>
#include <windows.h>
#include <boost/lockfree/queue.hpp>
using namespace std;

namespace RETURNCLIENT_Q {
	const int Q_SIZE = 1000;
}

class CReturnClientQ
{
public:
	CReturnClientQ();
	~CReturnClientQ();

	void push(__MAX::TData* pData);
	__MAX::TData* pop();

private:
	//void lock() { EnterCriticalSection(&m_cs); }
	//void unlock() { LeaveCriticalSection(&m_cs); }
private:
	//deque<string>		m_Q;
	//CRITICAL_SECTION	m_cs;

	boost::lockfree::queue<__MAX::TData*>	m_lockfreeQ;
};

