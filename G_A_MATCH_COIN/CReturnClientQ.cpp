#include "CReturnClientQ.h"
#include "CGlobals.h"

extern CGlobals		gCommon;


CReturnClientQ::CReturnClientQ():m_lockfreeQ(RETURNCLIENT_Q::Q_SIZE)
{
	
}

CReturnClientQ::~CReturnClientQ()
{
	//DeleteCriticalSection(&m_cs);
}

void CReturnClientQ::push(__MAX::TData* pData)
{
	gCommon.debug("[BEFORE PUSH](%s)", pData->d);
	m_lockfreeQ.push(pData);
}

__MAX::TData* CReturnClientQ::pop()
{
	__MAX::TData* data = nullptr;

	return(m_lockfreeQ.pop(data) == true) ? data : nullptr;
}



//void CReturnClientQ::push(string& data)
//{
//	//lock();
//	//m_Q.push_back(data);
//	//unlock();
//}
//bool CReturnClientQ::pop(string& out)
//{
//	bool exist = false;
//	lock();
//	if (!m_Q.empty())
//	{
//		exist = true;
//		out = std::move(m_Q.front());
//		m_Q.pop_front();
//		unlock();
//	}
//	return exist;
//}