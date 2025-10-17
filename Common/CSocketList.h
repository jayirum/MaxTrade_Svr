
#pragma once

#pragma warning( disable : 4786 )
#pragma warning( disable : 6101 )
#pragma warning( disable : 4996 )

#include <WinSock2.h>
#include "Util.h"
#include <cassert>
#include <string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#define LIST_MINSIZE  10 //50
#define IDX_INIT_VAL  -1

enum class EN_EMPTY_PLACE { FAILED, SUCC };

struct TSockListUnit
{
	LONG	idx = IDX_INIT_VAL;
	HANDLE	hDuplicateSock = NULL;
	string	sSockOrg;
	string	sIP;
	string  sUserId;
	string  sNote;

	void Init() {
		idx = IDX_INIT_VAL; 
		SAFE_CLOSEHANDLE(hDuplicateSock); 
		sSockOrg = "";
		sIP = "";
		sNote = "";
		sUserId = "";
	}
};

/*
*	MUST Singleton
*/
class CSocketList
{
public:
	CSocketList();
	virtual ~CSocketList();

	BOOL	Create(LONG lListSize = LIST_MINSIZE);
	BOOL 	Save_NewSocket(_In_ TSockListUnit* unit, _Out_ LONG* plIdx);
	BOOL 	Del_ClosedSocket(string sSockOrg, _Out_ LONG* plIdx);
	EN_EMPTY_PLACE 	Idx_FirstEmptyPlace(LONG* pIdx, BOOL bLock);
	LONG 	Idx_SameSocket(string sSockOrg, BOOL bLock);
	BOOL	Is_AlreadyReg(string sSockOrg, BOOL bLock);

	// return found
	//BOOL	GetSocket(LONG lIdx, _Out_ SOCKET* sock,  _Out_ BOOL* bNext);
	BOOL	GetSocket(LONG lIdx, _Out_ HANDLE* hDupSock, _Out_ BOOL* bNext);
	LONG	GetTotNumOfSocket() { return m_lNumSocket; }
	const char* getMsg() const { return m_zMsg; }
private:
	void	Destroy();
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }
	void	ReCalc_LastIdx(LONG lIdx){ m_lLastIdx = (m_lLastIdx > lIdx) ? m_lLastIdx : lIdx; }
private:	
	static	UINT m_numInstance;
	CRITICAL_SECTION	m_cs;
	LONG	m_lLastIdx		= IDX_INIT_VAL;
	LONG	m_lNumSocket	= 0;
	LONG	M_lLastIdx		= 0;
	LONG 	m_lListSize		= 0;
	TSockListUnit*	m_list			= nullptr;
	char	m_zMsg[1024]	= { 0 };

};



/////////////////////////////////////////////////////////////////////////
////
////		Reader
////
/////////////////////////////////////////////////////////////////////////
enum class EN_RET_READDATA  { FAILED, SUCC_NO_DATA, SUCC_NEW_DATA };
class CSocketListReader
{
public:
	CSocketListReader(CSocketList* list) { m_list = list; }
	~CSocketListReader() {};

	// return : found
	BOOL GetSocket(LONG lIdx, _Out_ HANDLE* hSock,_Out_ BOOL* bNext) 
	{
		return m_list->GetSocket(lIdx, hSock, bNext);
	}

	LONG	GetTotNumOfSocket() { return m_list->GetTotNumOfSocket(); }

private:
	CSocketList* m_list = nullptr;
};