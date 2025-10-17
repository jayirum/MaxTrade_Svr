// GlobalList.cpp: implementation of the CCircularQ class.
//
//////////////////////////////////////////////////////////////////////

#include "CCircularQ.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT CCircularQ::m_numInstance = 0;

CCircularQ::CCircularQ()
{
	++m_numInstance;
	ASSERT_FUNC((m_numInstance==1),"MUST Single instance");
	InitializeCriticalSection(&m_cs);
}

CCircularQ::~CCircularQ()
{
	Destroy();
	DeleteCriticalSection(&m_cs);
}


BOOL CCircularQ::Create(int nQSize)
{
	if(m_numInstance>1 )
	{
		sprintf(m_zMsg, "Instance 가 이미 생성되어 있음(%d)", m_numInstance);
		return FALSE;
	}

	m_nQSize = (nQSize > Q_MINSIZE) ? nQSize : Q_MINSIZE;

	m_Q = new TUnit[m_nQSize];

	return TRUE;
}

LONG CCircularQ::GetIdx_ForNewData() const
{
	LONG lNewIdx = m_lIdxLastWrite + 1;
	if (lNewIdx == m_nQSize)
		lNewIdx = IDX_START_VAL;

	return lNewIdx;
}		



VOID CCircularQ::Add(const char* pData, int nDataSize)
{
	LONG lNewIdx = GetIdx_ForNewData();

	TUnit* pUnit = &m_Q[lNewIdx];

	pUnit->dataSize = nDataSize;
	ZeroMemory(pUnit->buf, UNIT_BUF_LEN);
	memcpy(pUnit->buf, pData, nDataSize);
	
	Update_Idx_Turn(lNewIdx);

	sprintf(m_zMsg, "[IdxLastWrite:%ld][Turn:%lld](Data:%.*s)", m_lIdxLastWrite, m_llCurrTurn, nDataSize, pUnit->buf);
	//printf("[1]%s\n", m_zMsg);

}

void CCircularQ::Update_Idx_Turn(LONG lNewIdx)
{
	Lock();

	m_lIdxLastWrite = lNewIdx;

	if (m_lIdxLastWrite == IDX_START_VAL)
		m_llCurrTurn++;

	UnLock();
}

/*
	get last idx & current turn

	return value: 
		- true
*/
EN_RET_STARTED	CCircularQ::Get_Idx_Turn(_Out_ LONG* idx, _Out_ ULONGLONG* turn)
{
	Lock();

	*idx = m_lIdxLastWrite;
	*turn = m_llCurrTurn;

	UnLock();

	EN_RET_STARTED ret = EN_RET_STARTED::Q_INIT;
	if (*idx > IDX_INIT_VAL && *turn > TURN_INIT_VAL)
		ret = EN_RET_STARTED::Q_STARTED;

	return ret;
}


void CCircularQ::Destroy()
{
	if(m_Q)
		delete[] m_Q;

	m_Q = nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// CCQWriter
// 
///////////////////////////////////////////////////////////////////////////////////////////



UINT CCQWriter::m_numInstance = 0;

CCQWriter::CCQWriter()
{
	++m_numInstance;
	ASSERT_FUNC((m_numInstance == 1), "MUST Single instance");
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// CCQReader
// 
///////////////////////////////////////////////////////////////////////////////////////////
CCQReader::CCQReader(CCircularQ* Q)
{
	m_Q = Q;
}


/*
	[0|data][1|data][2|data][3|data][4|data]


*/
EN_RET_READDATA_Q CCQReader::ReadNewData(_Out_ TUnit* out, _Out_ BOOL* bMoreData, _Out_ BOOL* bMsg)
{
	ZeroMemory(out, sizeof(TUnit));
	*bMoreData	= FALSE;
	*bMsg		= FALSE;

	LONG lIdxW			= IDX_INIT_VAL;
	ULONGLONG llTurnW	= TURN_INIT_VAL;

	EN_RET_STARTED retStarted = m_Q->Get_Idx_Turn(&lIdxW, &llTurnW);
	if (retStarted == EN_RET_STARTED::Q_INIT) {
		return (EN_RET_READDATA_Q::SUCC_NO_DATA);
	}

	// Reader를 최초로 실행하는 경우
	if (Is_Not_Initialized())
	{
		m_lIdxLastRead	= lIdxW;
		m_llCurrTurn = llTurnW;
		*bMsg = TRUE;
		sprintf(m_zMsg, "[1]Reader 최초 실행한 경우. IdxLastRead:%ld, CurrTurn:%lld", m_lIdxLastRead, m_llCurrTurn);
		return (EN_RET_READDATA_Q::SUCC_NO_DATA);
	}


	EN_RET_READDATA_Q ret;

	if (m_llCurrTurn == llTurnW)
	{
		if (m_lIdxLastRead > lIdxW) // reader 마지막 4, 데이터는 2 
		{
			sprintf(m_zMsg, "[2][ERROR][m_llCurrTurn(%lld) == llTurnW(%lld)] [m_lIdxLastRead(%ld) > lIdxW(%ld)]",
				m_llCurrTurn, llTurnW, m_lIdxLastRead, lIdxW);
			*bMsg = TRUE;
			ret = EN_RET_READDATA_Q::FAILED;
		}
		else //(m_lIdxLastRead <= lIdxW) // reader 마지막 2, 데이터는 4
		{
			if (m_lIdxLastRead == lIdxW && Is_ReadingStarted())
			{
				ret = EN_RET_READDATA_Q::SUCC_NO_DATA;
			}
			else
			{
				ret = GetData(lIdxW, llTurnW, out, bMsg);

				if (m_lIdxLastRead < lIdxW)
					*bMoreData = TRUE;
			}
		}
	}
	else if (m_llCurrTurn < llTurnW)
	{
		*bMoreData = TRUE;
		ret = GetData(lIdxW, llTurnW, out, bMsg);
	}
	else
	{
		sprintf(m_zMsg, "[3][ERROR]m_llCurrTurn(%lld) > llTurnW(%lld)", m_llCurrTurn, llTurnW);
		*bMsg = TRUE;
		ret = (EN_RET_READDATA_Q::FAILED);
	}

	return (ret);
}


EN_RET_READDATA_Q CCQReader::GetData(LONG lIdxW, LONGLONG llTurnW, _Out_ TUnit* out,_Out_ BOOL* bMsg)
{
	LONG		lBakIdx = m_lIdxLastRead;
	LONGLONG	llBakTurn = m_llCurrTurn;

	if (Is_ReadingStarted())
		m_lIdxLastRead++;

	if ( m_Q->Is_OutOfRange(m_lIdxLastRead)) {
		m_lIdxLastRead = IDX_START_VAL;
		m_llCurrTurn++;
	}

	m_Q->GetData(m_lIdxLastRead, out);

	sprintf(m_zMsg, "[4][m_llCurrTurn(%lld->%lld) : llTurnW(%lld)] [m_lIdxLastRead(%ld->%ld) : lIdxW(%ld)]",
		llBakTurn, m_llCurrTurn, llTurnW, lBakIdx, m_lIdxLastRead, lIdxW);

	Set_ReadingStarted();


	return EN_RET_READDATA_Q::SUCC_NEW_DATA;
	
}

//
//EN_RET_READDATA CCQReader::ReadNewData_SameTurn(LONG lIdxW, LONGLONG llTurnW, _Out_ TUnit* out, _Out_ BOOL* bMoreData, _Out_ BOOL* bMsg)
//{
//	EN_RET_READDATA ret;
//
//	if (m_lIdxLastRead > lIdxW) // reader 마지막 4, 데이터는 2 
//	{
//		sprintf(m_zMsg, "[3][ERROR][m_llCurrTurn(%lld) == llTurnW(%lld)] [m_lIdxLastRead(%ld) > lIdxW(%ld)]",
//			m_llCurrTurn, llTurnW, m_lIdxLastRead, lIdxW);
//		*bMsg = TRUE;
//		ret = EN_RET_READDATA::FAILED;
//	}
//	else //(m_lIdxLastRead <= lIdxW) // reader 마지막 2, 데이터는 4
//	{
//		if (m_lIdxLastRead == lIdxW && Is_ReadingStarted())
//		{
//			ret = EN_RET_READDATA::SUCC_NO_DATA;
//		}
//		else
//		{
//			LONG		lBakIdx = m_lIdxLastRead;
//			LONGLONG	llBakTurn = m_llCurrTurn;
//
//			if (Is_ReadingStarted())
//				m_lIdxLastRead++;
//
//			if (m_Q->Is_LastIdx(m_lIdxLastRead)) {
//				m_lIdxLastRead = IDX_START_VAL;
//				m_llCurrTurn++;
//			}
//
//			m_Q->GetData(m_lIdxLastRead, out);
//
//			sprintf(m_zMsg, "[2][m_llCurrTurn(%lld->%lld) == llTurnW(%lld)] [m_lIdxLastRead(%ld->%ld) < lIdxW(%ld)]",
//				llBakTurn, m_llCurrTurn, llTurnW, lBakIdx, m_lIdxLastRead, lIdxW);
//
//			Set_ReadingStarted();
//
//			if (m_lIdxLastRead < lIdxW)
//				*bMoreData = TRUE;
//
//			ret = EN_RET_READDATA::SUCC_NEW_DATA;
//			*bMsg = TRUE;
//		}
//	}
//	
//	return ret;
//}

#if 0
EN_RET_READDATA CCQReader::ReadNewData(_Out_ TUnit* out, _Out_ BOOL* bMoreData, _Out_ BOOL* bMsg)
{
	ZeroMemory(out, sizeof(TUnit));
	*bMoreData	= FALSE;
	*bMsg		= FALSE;

	LONG lLastWriteIdx			= IDX_INIT_VAL;
	ULONGLONG lCurrWriteTurn	= TURN_INIT_VAL;

	EN_RET_STARTED retStarted = m_Q->Get_Idx_Turn(&lLastWriteIdx, &lCurrWriteTurn);
	if (retStarted== EN_RET_STARTED::Q_STARTED_NO)
		return (EN_RET_READDATA::SUCC_NO_DATA);

	// Reader를 최초로 실행하는 경우
	if (Is_Not_Initialized())
	{
		m_lLastIdx		= lLastWriteIdx;
		m_llLastTurn	= lCurrWriteTurn;
		*bMsg = TRUE;
		sprintf(m_zMsg, "[1]Reader 최초 실행한 경우. LastIdx:%ld, LastTurn:%lld", m_lLastIdx, m_llLastTurn);
		return (EN_RET_READDATA::SUCC_NO_DATA);
	}

	// No new data
	if (m_lLastIdx == lLastWriteIdx)
	{
		if (m_llLastTurn != lCurrWriteTurn)
		{
			sprintf(m_zMsg, "[2][ERROR]Q IDX 는 같은데(%ld,%ld) TURN 이 다르다.(%lld, %lld)", 
											m_lLastIdx, lLastWriteIdx, m_llLastTurn, lCurrWriteTurn);
			*bMsg = TRUE;
			return (EN_RET_READDATA::FAILED);
		}
		return (EN_RET_READDATA::SUCC_NO_DATA);
	}
	else if (m_lLastIdx < lLastWriteIdx) // reader 마지막 2, 데이터는 4
	{
		if (m_llLastTurn == lCurrWriteTurn)
		{
			m_lLastIdx++;

			m_Q->GetData(m_lLastIdx, out);

			sprintf(m_zMsg, "[3][m_lLastIdx < lLastWriteIdx:%ld/%ld-%ld][m_llLastTurn == lCurrWriteTurn:%lld-%lld]",
				m_lLastIdx-1, m_lLastIdx, lLastWriteIdx, m_llLastTurn, lCurrWriteTurn);


			if (m_lLastIdx < lLastWriteIdx)
				*bMoreData = TRUE;

			*bMsg = TRUE;
		}
		else // 내 커서가 데이터 커서보다 뒤에 있는데, turn 이 다르다면 이건 너무 늦는 경우이다.
		{
			sprintf(m_zMsg, "[5][ERROR]Reader Idx(%d)가 Data Idx(%d) 보다 늦는데, Turn이 다르다.(%lld, %lld)",
				m_lLastIdx, lLastWriteIdx, m_llLastTurn, lCurrWriteTurn);
			*bMsg = TRUE;
			m_lLastIdx = lLastWriteIdx;
			m_llLastTurn = lCurrWriteTurn;
			return (EN_RET_READDATA::SUCC_NO_DATA);
		}
	}
	else  if (m_lLastIdx > lLastWriteIdx) // reader 마지막 4, 데이터는 2 (한바퀴 돈 경우)
	{
		if (m_llLastTurn + 1 == lCurrWriteTurn)
		{
			LONG lBackup = m_lLastIdx;

			if (m_Q->Is_LastIdx(m_lLastIdx))
				m_lLastIdx = IDX_START_VAL;
			else
				m_lLastIdx++;

			m_Q->GetData(m_lLastIdx, out);

			sprintf(m_zMsg, "[6][m_lLastIdx > lLastWriteIdx:%ld/%ld-%ld][m_llLastTurn == lCurrWriteTurn:%lld-%lld]",
				lBackup, m_lLastIdx, lLastWriteIdx, m_llLastTurn, lCurrWriteTurn);

			if (m_lLastIdx == IDX_START_VAL)
				m_llLastTurn++;

			*bMoreData = TRUE;
			*bMsg = TRUE;
		}
		else // 데이터는 한바퀴 돌았는데, turn이 안맞다.
		{
			sprintf(m_zMsg, "[8][ERROR]Reader Idx(%d)가 Data Idx(%d) 보다 빠른면, Reader Turn(%lld) + 1 = Data Turn(%lld) 이어야 한다.",
				m_lLastIdx, lLastWriteIdx, m_llLastTurn, lCurrWriteTurn);

			m_lLastIdx = lLastWriteIdx;
			m_llLastTurn = lCurrWriteTurn;
			*bMsg = TRUE;
			return (EN_RET_READDATA::SUCC_NO_DATA);
		}
	}
	
	return (EN_RET_READDATA::SUCC_NEW_DATA);
}
#endif