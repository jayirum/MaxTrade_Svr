// GlobalList.cpp: implementation of the CSocketList class.
//
//////////////////////////////////////////////////////////////////////

#include "CSocketList.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT CSocketList::m_numInstance = 0;

CSocketList::CSocketList()
{
	++m_numInstance;
	ASSERT_FUNC((m_numInstance == 1), "MUST Single instance");

	InitializeCriticalSection(&m_cs);
}

CSocketList::~CSocketList()
{
	Destroy();
	DeleteCriticalSection(&m_cs);
}


BOOL CSocketList::Create(LONG lListSize)
{
	if (m_numInstance > 1)
	{
		sprintf(m_zMsg, "Instance 가 이미 생성되어 있음(%d)", m_numInstance);
		return FALSE;
	}

	m_lListSize = (lListSize > LIST_MINSIZE) ? lListSize : LIST_MINSIZE;

	m_list = new TSockListUnit[m_lListSize];

	return TRUE;
}

EN_EMPTY_PLACE CSocketList::Idx_FirstEmptyPlace(LONG* pIdx, BOOL bLock) 
{
	EN_EMPTY_PLACE ret = EN_EMPTY_PLACE::FAILED;

	if(bLock) Lock();
	LONG lLoop = 0;
	for (lLoop = 0; lLoop < m_lListSize; lLoop++)
	{
		if (m_list[lLoop].hDuplicateSock == NULL)
		{
			*pIdx = lLoop;
			ret = EN_EMPTY_PLACE::SUCC;
			break;
		}
	}
	if (bLock) UnLock();

	if (lLoop >= m_lListSize)
	{
		sprintf(m_zMsg, "List 에 빈 공간이 없음. List Size 를 늘려야 함");
		ret = EN_EMPTY_PLACE::FAILED;
	}

	return ret;
}

BOOL CSocketList::Save_NewSocket(_In_ TSockListUnit* unit, _Out_ LONG* plIdx)
{
	BOOL bSucc = FALSE;
	Lock();

	LONG lIdx = IDX_INIT_VAL;
	EN_EMPTY_PLACE ret = Idx_FirstEmptyPlace(&lIdx, FALSE);
	if (ret == EN_EMPTY_PLACE::SUCC)
	{
		bSucc = TRUE;

		memcpy(&m_list[lIdx], unit, sizeof(TSockListUnit));
		m_list[lIdx].idx	= lIdx;

		m_lNumSocket++;
		ReCalc_LastIdx(lIdx);

		*plIdx = lIdx;
		sprintf(m_zMsg, "[A] IDX:%ld,SOCKET:%s,TotNum:%d", lIdx, m_list[lIdx].sSockOrg.c_str(), GetTotNumOfSocket());
	}
	UnLock();	

	return bSucc;
}

BOOL CSocketList::Del_ClosedSocket(string sSockOrg, _Out_ LONG* plIdx)
{
	BOOL bSucc = FALSE;
	Lock();

	LONG lIdx = Idx_SameSocket(sSockOrg, FALSE);
	if (lIdx == IDX_INIT_VAL)
	{
		sprintf(m_zMsg, "Delet 하기 위한 socket 을 찾지 못함.SOCKET[%s] - NumOfSocket[%ld]", sSockOrg.c_str(), m_lNumSocket);
	}
	else
	{
		m_lNumSocket--;
		*plIdx = lIdx;

		m_list[lIdx].Init();

		sprintf(m_zMsg, "[D] IDX:%ld,SOCKET:%s,TotNum:%d", lIdx, sSockOrg.c_str(), GetTotNumOfSocket());
		bSucc = TRUE;
	}
	UnLock();

	return bSucc;
}

LONG CSocketList::Idx_SameSocket(string sSockOrg, BOOL bLock)
{
	if(bLock) Lock();

	LONG idx = IDX_INIT_VAL;
	LONG lLoop = 0;
	for (lLoop = 0; lLoop < m_lLastIdx+1; lLoop++)
	{
		if (m_list[lLoop].sSockOrg == sSockOrg)
		{
			idx = m_list[lLoop].idx;
			break;
		}
	}

	if (bLock) UnLock();

	return idx;
}


BOOL CSocketList::Is_AlreadyReg(string sSockOrg, BOOL bLock)
{
	return (Idx_SameSocket(sSockOrg, bLock) != IDX_INIT_VAL);
}


// return : bFound
BOOL CSocketList::GetSocket(LONG lIdx, _Out_  HANDLE* hDupSock, _Out_ BOOL* bNext)
{
	*bNext = FALSE;
	
	if (lIdx < m_lLastIdx)
		*bNext = TRUE;

	//*sock = m_list[lIdx].sockOrg;
	*hDupSock = m_list[lIdx].hDuplicateSock;
	return (*hDupSock != NULL);
}

void CSocketList::Destroy()
{
	Lock();

	if (m_list)
		delete[] m_list;
	m_list = nullptr;

	UnLock();
}



/////////////////////////////////////////////////////////////////////////
////
////		writer
////
/////////////////////////////////////////////////////////////////////////

//UINT CSocketListWriter::m_numInstance = 0;
//CSocketListWriter::CSocketListWriter()
//{
//	m_list = NULL;
//	m_numInstance++;
//	ASSERT_FUNC((m_numInstance == 1), "MUST Single instance");
//}
