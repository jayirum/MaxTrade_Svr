
#pragma once

#include <windows.h>
#include <stdio.h>
#include <cassert>
#pragma warning( disable : 4786 )
#pragma warning( disable : 6101 )
#pragma warning( disable : 4996 )

#define Q_MINSIZE		1000	// at least Q has 1000 units
#define UNIT_BUF_LEN	1024

#define IDX_INIT_VAL	-1
#define IDX_START_VAL	0

#define TURN_INIT_VAL	0
#define TURN_START_VAL	1

enum class EN_RET_STARTED { Q_INIT, Q_STARTED};

struct TUnit
{
	char buf[UNIT_BUF_LEN] = { 0 };
	int dataSize = 0;
};



/*
*	MUST Singleton
*/
class CCircularQ
{
public:
	CCircularQ();
	virtual ~CCircularQ();

	BOOL	Create(int nQSize = Q_MINSIZE);
	VOID	Add(const char* pData, int nDataSize);
	EN_RET_STARTED	Get_Idx_Turn(_Out_ LONG* idx, _Out_ ULONGLONG* turn);
	VOID	GetData(LONG lIdx, TUnit* out) const { memcpy(out, &m_Q[lIdx], sizeof(TUnit)); }

	char* getMsg() { return m_zMsg; }
	const int getQSize() const { return m_nQSize; }
	
	BOOL	Is_OutOfRange(LONG idx) { return (idx >= m_nQSize); }
private:
	LONG	GetIdx_ForNewData() const;
	void	Update_Idx_Turn(LONG lNewIdx);
	void	Destroy();
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }
private:
	static	UINT m_numInstance;
	TUnit*		m_Q				= nullptr;	// array of TUnit
	int			m_nQSize		= 0;
	char		m_zMsg[1024]	= { 0 };

	LONG		m_lIdxLastWrite	= IDX_INIT_VAL;		// idx where data has been saved lastly
	ULONGLONG	m_llCurrTurn	= TURN_INIT_VAL;

	CRITICAL_SECTION	m_cs;
};


///////////////////////////////////////////////////////////////////////
//
//		Writer - Singleton
//
///////////////////////////////////////////////////////////////////////
class CCQWriter
{
public:
	CCQWriter();
	~CCQWriter() {};

	BOOL SetQPtr(CCircularQ* Q) { 
		if (m_numInstance > 1)
			return FALSE;
		m_Q = Q; 
		return TRUE;
	}

	void Write(const char* pData, int nDataSize)
	{
		m_Q->Add(pData, nDataSize);
	}

	char* getMsg() { return m_Q->getMsg(); }
private:
	static	UINT	m_numInstance;
	CCircularQ* m_Q = nullptr;
};



///////////////////////////////////////////////////////////////////////
//
//		Reader
//
///////////////////////////////////////////////////////////////////////
enum class EN_RET_READDATA_Q  { FAILED, SUCC_NO_DATA, SUCC_NEW_DATA };
class CCQReader
{
public:
	CCQReader(CCircularQ* Q);
	CCQReader() {};
	~CCQReader() {};

	void SetQPtr(CCircularQ* Q) { m_Q = Q; }
	EN_RET_READDATA_Q ReadNewData( _Out_ TUnit* out, _Out_ BOOL* bMoreData, _Out_ BOOL* bMsg);

	char* getMsg() { return m_zMsg; }
private:
	//EN_RET_READDATA_Q ReadNewData_SameTurn(LONG lIdxW, LONGLONG llTurnW, _Out_ TUnit* out, _Out_ BOOL* bMoreData, _Out_ BOOL* bMsg);
	EN_RET_READDATA_Q GetData(LONG lIdxW, LONGLONG llTurnW, _Out_ TUnit* out, _Out_ BOOL* bMsg);
	BOOL	Is_Not_Initialized() { return (m_lIdxLastRead == IDX_INIT_VAL || m_llCurrTurn == TURN_INIT_VAL); }
	BOOL	Is_ReadingStarted() { return m_bReadingStarted; }
	VOID	Set_ReadingStarted(){ if(!m_bReadingStarted) m_bReadingStarted = TRUE; }
private:
	CCircularQ*		m_Q = nullptr;
	LONG			m_lIdxLastRead		= IDX_INIT_VAL;
	ULONGLONG		m_llCurrTurn	= TURN_INIT_VAL;
	char			m_zMsg[1024]	= { 0 };
	BOOL			m_bReadingStarted = FALSE;
};