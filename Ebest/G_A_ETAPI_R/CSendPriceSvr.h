/*
	- receive data from Master / Slave

	- Use only 1 thread

	- Dispatch to CMasterWithSlaves thread
*/

#pragma once

#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 26496)
#pragma warning( disable : 26495)

#include "../../Common/MaxTradeInc.h"
#include <windows.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include "comm.h"



using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10



#define WM_MARKETDATA		WM_USER + 3434
#define WM_REG_CLIENT		WM_MARKETDATA+1
#define WM_UNREG_CLIENT		WM_MARKETDATA+2

#define CIRCULAR_Q_SIZE	1024*10

enum { CTX_DIE = 990, /*CTX_MT4PING,*/ CTX_RQST_SEND, CTX_RQST_RECV };
enum { CK_TYPE_NORMAL, CK_TYPE_COMMUNICATION };

#define CVT_SOCKET(sock,out) { sprintf(out, "%d", sock);}



struct IO_CONTEXT
{
	WSAOVERLAPPED	overLapped;
	WSABUF			wsaBuf;
	char			buf[__MAX::BUFLEN_1K];
	int				context;
	DWORD           dwIoSize;
	IO_CONTEXT()
	{
		ZeroMemory(&overLapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&wsaBuf, sizeof(wsaBuf));
		ZeroMemory(&buf, sizeof(buf));
	}
};
#define CONTEXT_SIZE sizeof(IO_CONTEXT)


struct COMPLETION_KEY
{
	SOCKET	sock;
	string	sUserID;
	string  sClientIp;
	long	lRefCnt;
	time_t	acptTime;
	COMPLETION_KEY()
	{
		sock = INVALID_SOCKET;
		lRefCnt = 0;
	}

	void	AddRefer() { InterlockedIncrement(&lRefCnt); }
	void	Release() { InterlockedDecrement(&lRefCnt); }
	bool	Is_BeingUsed() { return (lRefCnt > 0); }
};
#define CONTEXT_SIZE sizeof(IO_CONTEXT)


struct TThrdArgc
{
	void*	pThis;
	string	sStkCd;
};

class CSendPriceSvr
{
public:
	CSendPriceSvr();
	virtual ~CSendPriceSvr();

	BOOL Initialize();

private:
	void Finalize();

	BOOL InitListen();
	BOOL Create_SendMarketDataThread();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	void DelClient_AddCK_DelList(COMPLETION_KEY* pCompletionKey);
	void DelClient_DelCK_DelList();
	void DelClient_CloseSock(SOCKET sock);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_SendData(LPVOID lp);

	void	RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(SOCKET sock);


	VOID	Set_ThreadAlive() { m_bRun = TRUE; }
	VOID	Set_ThreadDie() { m_bRun = FALSE; }

	//BOOL	Is_ConnAllowed(string sClientIP, _Out_ int* pnCurrConnCnt);


private:
	
	map<string, unsigned>	m_mapSendThrdIds;
	 
	DWORD			m_dwThreadCount=0;
	HANDLE			m_hCompletionPort=0;
	SOCKET			m_sockListen=INVALID_SOCKET;
	char			m_zListenIP[128] = { 0 };
	int				m_nListenPort = 0;
	HANDLE			m_hThread_Listen=NULL, m_hParsing = NULL;
	unsigned int	m_unThread_Listen = 0, m_unParsing = 0;
	WSAEVENT		m_hListenEvent=NULL;

	__MAX::CPacketBufferIocp	m_parser;

	list<COMPLETION_KEY*>	m_listDelCK;
	CRITICAL_SECTION		m_csDelCK;


	//vector<TConnections>	m_vecConnections;
	//CRITICAL_SECTION		m_csConnections;
	//UINT					m_nMaxConnPerIP;
	//UINT					m_nBlockDurationSec;


	BOOL m_bRun = FALSE;
	char m_zMsg[1024] = { 0 };

};