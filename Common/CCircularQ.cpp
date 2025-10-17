// GlobalList.cpp: implementation of the CCircularQ class.
//
//////////////////////////////////////////////////////////////////////

#include "CCircularQ.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CCircularQ::CCircularQ()
{
	InitializeCriticalSection(&m_csReader);
}

CCircularQ::~CCircularQ()
{
	Destroy();
	DeleteCriticalSection(&m_csReader);
}


void CCircularQ::Destroy()
{
	if (m_Q)
		delete[] m_Q;

	m_Q = nullptr;
}



BOOL CCircularQ::W_Create(int nQSize, _Out_ char* pMsg)
{
	*pMsg = 0x00;
	m_nQSize	= (nQSize > Q_MINSIZE) ? nQSize : Q_MINSIZE;
	m_Q			= new TCQUnit[m_nQSize];

	return TRUE;
}


LONG CCircularQ::W_Idx_For_AddData() const
{
	LONG lNewIdx = m_idxLastWrite + 1;
	if (lNewIdx == m_nQSize)
		lNewIdx = IDX_START_VAL;

	return lNewIdx;
}		



BOOL CCircularQ::W_AddData(const char* pData, int nDataSize, _Out_ char* pMsg)
{
	*pMsg = 0x00;
	if(!W_Is_Writer())
	{
		sprintf(pMsg, "[%d]는 Writer Thread 가 아닙니다. 저장할 권한이 없습니다.", GetCurrentThreadId());
		return FALSE;
	}

	LONG lNewIdx = W_Idx_For_AddData();

	TCQUnit* pUnit = (TCQUnit*)&m_Q[lNewIdx];
	pUnit->dataSize = nDataSize;
	ZeroMemory(pUnit->buf, UNIT_BUF_LEN);
	memcpy(pUnit->buf, pData, nDataSize);
	
	m_idxLastWrite = lNewIdx;

	sprintf(pMsg, "<%ld>(Data:%.*s)", m_idxLastWrite, nDataSize, pUnit->buf);
	
	return TRUE;
}

BOOL	CCircularQ::W_AddData(const TCQUnit* pUnit, _Out_ char* pMsg)
{
	*pMsg = 0x00;
	if (!W_Is_Writer())
	{
		sprintf(pMsg, "[%d]는 Writer Thread 가 아닙니다. 저장할 권한이 없습니다.", GetCurrentThreadId());
		return FALSE;
	}

	LONG lNewIdx = W_Idx_For_AddData();
	memcpy((TCQUnit*)&m_Q[lNewIdx], pUnit, sizeof(TCQUnit));
	m_idxLastWrite = lNewIdx;

	sprintf(pMsg, "<%ld>(Data:%.*s)", m_idxLastWrite, pUnit->dataSize, pUnit->buf);

	return TRUE;
}


void	CCircularQ::R_AddReader(UINT unThrdId)
{
	LONG idx = IDX_INIT_VAL;
	m_mapRIdx[unThrdId] = idx;
}

BOOL CCircularQ::R_Is_Reader_Added(UINT unThrdId) 
{ 
	BOOL bAdded = FALSE;
	LockReader(); 
	auto it = m_mapRIdx.find(unThrdId); 
	bAdded = (it == m_mapRIdx.end()) ? FALSE : TRUE;
	UnLockReader(); 
	
	return bAdded;
}



BOOL CCircularQ::R_Has_Reading_Started(UINT unThrdId)
{
	BOOL bStarted = FALSE;
	LockReader();
	auto it = m_mapRIdx.find(unThrdId);
	if (it != m_mapRIdx.end())
	{
		bStarted = ((*it).second == IDX_INIT_VAL) ? FALSE : TRUE;
	}
	UnLockReader();

	return bStarted;
}


TCQUnit* CCircularQ::R_ReadNewData_Array(UINT unThrdId, _Out_ BOOL* pbSucc, _Out_ int* pnArraySize, _Out_ char* pMsg)
{
	*pbSucc = TRUE;
	*pnArraySize = 0;
	*pMsg = 0;

	LONG idxLastWrite = m_idxLastWrite;

	if (!R_Has_Data_Saved(idxLastWrite))
		return NULL;

	/////////////////////////////////////////////////////////////////////////
	LockReader();
	auto it = m_mapRIdx.find(unThrdId);
	if (it == m_mapRIdx.end())
	{
		*pbSucc = FALSE;

		UnLockReader();
		sprintf(pMsg, "먼저 Reader를 등록하고 사용하세요.(R_AddReader()함수)");
		return NULL;
	}

	LONG idxLastRead = (*it).second;

	// Reader를 최초로 실행하는 경우
	if (idxLastRead == IDX_INIT_VAL) {
		m_mapRIdx[unThrdId] = idxLastWrite;
		UnLockReader();

		sprintf(pMsg, "Reader 최초 실행한 경우.");
		return NULL;
	}
	UnLockReader();
	/////////////////////////////////////////////////////////////////////////

	int nReadingNum = 0;
	if (idxLastRead == idxLastWrite)
	{
		return NULL;
	}
	else if (idxLastRead < idxLastWrite)
	{
		nReadingNum = idxLastWrite - idxLastRead;
	}
	else
	{
		LONG lFinalIdx = m_nQSize - 1;
		nReadingNum = lFinalIdx - idxLastRead;
		if (nReadingNum == 0)
			nReadingNum = idxLastWrite;
	}

	nReadingNum = (nReadingNum > MAX_READ_SIZE) ? MAX_READ_SIZE : nReadingNum;
	
	TCQUnit* pUnit = new TCQUnit[nReadingNum];

	for (int i = 0; i < nReadingNum; i++)
	{
		if (++idxLastRead > (m_nQSize - 1))
			idxLastRead = IDX_START_VAL;

		memcpy( &pUnit[i], &m_Q[idxLastRead], sizeof(TCQUnit));
		pUnit[i].lIdx = idxLastRead;
	}
	LockReader();
	m_mapRIdx[unThrdId] = idxLastRead;
	UnLockReader();

	*pnArraySize = nReadingNum;

	return pUnit;
}

/*
	[0|data][1|data][2|data][3|data][4|data]


*/
RET_READ CCircularQ::R_ReadNewData(UINT unThrdId, _Out_ TCQUnit* out, _Out_ BOOL* bMoreData, _Out_ char* pMsg, _Out_ LONG* plIdx)
{
	*pMsg = 0x00;
	LONG idxLastWrite = m_idxLastWrite;

	if (!R_Has_Data_Saved(idxLastWrite))
		return RET_READ::SUCC_NO_DATA;

	/////////////////////////////////////////////////////////////////////////
	LockReader();
	auto it = m_mapRIdx.find(unThrdId);
	if (it == m_mapRIdx.end())
	{
		UnLockReader();

		sprintf(pMsg, "먼저 Reader를 등록하고 사용하세요.(R_AddReader()함수)");
		return RET_READ::FAILED;
	}

	LONG idxLastRead = (*it).second;

	// Reader를 최초로 실행하는 경우
	if (idxLastRead == IDX_INIT_VAL) {
		m_mapRIdx[unThrdId] = idxLastWrite;
		UnLockReader();

		sprintf(pMsg, "Reader 최초 실행한 경우.");
		return RET_READ::SUCC_NO_DATA;
	}
	UnLockReader();
	/////////////////////////////////////////////////////////////////////////

	if (idxLastRead == idxLastWrite)
	{
		return RET_READ::SUCC_NO_DATA;
	}


	ZeroMemory(out, sizeof(TCQUnit));
	*bMoreData = FALSE;

	if (++idxLastRead == m_nQSize)
		idxLastRead = IDX_START_VAL;


	*plIdx = idxLastRead;

	// DATA COPY	/////////////////////////////////////////////////
	memcpy(out, &m_Q[idxLastRead], sizeof(TCQUnit));

	LockReader();
	m_mapRIdx[unThrdId] = idxLastRead;
	UnLockReader();

	*bMoreData = (idxLastRead == idxLastWrite) ? FALSE : TRUE;

	return RET_READ::SUCC_NEW_DATA;
}
