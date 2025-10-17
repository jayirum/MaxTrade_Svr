
#pragma once
#pragma warning( disable : 4786 )
#pragma warning( disable : 6101 )
#pragma warning( disable : 4996 )

#include <windows.h>
#include <stdio.h>
#include <cassert>
#include <map>
#include <atomic>
using namespace std;



#define Q_MINSIZE		10000	// at least Q has 10,000 units
#define UNIT_BUF_LEN	1024 
#define RESERVED_LEN	128
#define MAX_READ_SIZE	10

#define IDX_INIT_VAL	-1
#define IDX_START_VAL	0


//enum class EN_RET_STARTED { Q_INIT, Q_STARTED};
enum class RET_READ  { FAILED, SUCC_NO_DATA, SUCC_NEW_DATA };

struct TCQUnit
{
	char buf[UNIT_BUF_LEN] = { 0 };
	int dataSize = 0;
	LONG lIdx;
	char reserved1[RESERVED_LEN] = { 0 };
	char reserved2[RESERVED_LEN] = { 0 };
	char reserved3[RESERVED_LEN] = { 0 };
};



/*
*	MUST Singleton
*/
class CCircularQ
{
public:
	CCircularQ();
	virtual ~CCircularQ();


	// FOR Writer	///////////////////////////////////////////////////////////////////////////////////
public:	
	BOOL	W_Create(int nQSize, _Out_ char* pMsg);
	VOID	W_SetWriter(UINT unWThrdId)  { m_unWThrdId= unWThrdId;}
	BOOL	W_AddData(const char* pData, int nDataSize, _Out_ char* pMsg);
	BOOL	W_AddData(const TCQUnit* pUnit, _Out_ char* pMsg);
private:
	LONG	W_Idx_For_AddData() const;
	BOOL	W_Is_Writer() { return (GetCurrentThreadId() == m_unWThrdId); }

	//FOR Reader	////////////////////////////////////////////////////////////////////////////////////
public:
	void	 R_AddReader(UINT unThrdId);
	RET_READ R_ReadNewData(UINT unThrdId,  _Out_ TCQUnit* out, _Out_ BOOL* bMoreData, _Out_ char* pMsg, _Out_ LONG* plIdx);
	TCQUnit* R_ReadNewData_Array(UINT unThrdId, _Out_ BOOL* pbSucc, _Out_ int* pnArraySize, _Out_ char* pMsg);
private:
	BOOL	R_Has_Data_Saved(LONG idxLastWrite) { return (idxLastWrite > IDX_INIT_VAL); }
	BOOL	R_Is_Reader_Added(UINT unThrdId);
	BOOL	R_Has_Reading_Started(UINT unThrdId);

private:

	void	Destroy();
	void	LockReader() { EnterCriticalSection(&m_csReader); }
	void	UnLockReader() { LeaveCriticalSection(&m_csReader); }
private:
	TCQUnit*		m_Q			= nullptr;	// array of TCQUnit
	int				m_nQSize	= 0;

	UINT			m_unWThrdId;
	map<UINT,LONG>	m_mapRIdx;
	LONG			m_idxLastWrite	= IDX_INIT_VAL;		// idx where data has been saved lastly

	CRITICAL_SECTION	m_csReader;
};

