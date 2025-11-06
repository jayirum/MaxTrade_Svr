
#include "CSendPriceSvr.h"
#include <process.h>
#include "../../Common/StringUtils.h"
#include <assert.h>

#define __REF_1_HEAD__
#define __REF_1_TAIL__

#define __REF_2_HEAD__
#define __REF_2_TAIL__

extern vector<string>			g_vecStk;
extern map<string, CCircularQ*>	g_mapQ;


CSendPriceSvr::CSendPriceSvr()
{
}
CSendPriceSvr::~CSendPriceSvr()
{
	Finalize();
}


void CSendPriceSvr::Finalize()
{
	Set_ThreadDie();

	for (UINT i = 0; i < WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}

	CloseListenSock();

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csDelCK);

	WSACleanup();
}



BOOL CSendPriceSvr::Create_SendMarketDataThread()
{
	
	for( auto it=g_vecStk.begin(); it!=g_vecStk.end(); ++it)
	{
		TThrdArgc* pArgc = new TThrdArgc;
		pArgc->pThis	= this;
		pArgc->sStkCd	= (*it);
		unsigned int Id;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_SendData, this, 0, &Id);

		m_mapSendThrdIds[(*it)] = Id;
	}

	return TRUE;
}

BOOL CSendPriceSvr::Initialize( )
{
	InitializeCriticalSection(&m_csDelCK);
	//InitializeCriticalSection(&m_csConnections);

	if (!Create_SendMarketDataThread())
		return FALSE; 

	if (!InitListen()) {
		return FALSE;
	}
	LOGGING(INFO, TRUE, "Init Listen(IP:%s)(Port:%d)",m_zListenIP, m_nListenPort);

	Set_ThreadAlive();

	m_hThread_Listen	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);
	m_hParsing = (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unParsing);

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_dwThreadCount);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// client socket 을 담당할 IOCP Work thread	
	{
		UINT dwID;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CSendPriceSvr::InitListen()
{
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		LOGGING(ERR,TRUE, TEXT("create socket error: %d"), WSAGetLastError());
		return FALSE;
	}

	char zPort[32] = { 0 }, zMaxConnPerIp[32] = { 0 }, zConnDuration[32] = { 0 };
	if (!ReadConfig("CONFIG", "LISTEN_IP", m_zListenIP) ||
		!ReadConfig("CONFIG", "LISTEN_PORT", zPort)		||
		!ReadConfig("CONFIG", "MAXCONN_PER_IP", zMaxConnPerIp)		||
		!ReadConfig("CONFIG", "CONN_DURATION_SEC", zConnDuration)
		)
	{
		LOGGING(LOGTP_ERR, TRUE, "Listen IP, Port 조회 오류.ini 파일을 확인하세요");
		return FALSE;
	}

	m_nListenPort = atoi(zPort);
	//m_nMaxConnPerIP = atoi(zMaxConnPerIp);
	//m_nBlockDurationSec = atoi(zConnDuration);


	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr(m_zListenIP);
	InternetAddr.sin_port = htons(m_nListenPort);

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);


	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), m_zListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		LOGGING(LOGTP_ERR, TRUE, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

/*
BOOL WINAPI dQueuedCompletionStatus(
_In_     HANDLE       CompletionPort,
_In_     DWORD        dwNumberOfBytesTransferred,
_In_     ULONG_PTR    dwCompletionKey,
_In_opt_ LPOVERLAPPED lpOverlapped
);
*/


void CSendPriceSvr::CloseListenSock()
{
	SAFE_CLOSEHANDLE(m_hListenEvent);
	if (m_sockListen != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 1=> enable linger (means wait before close socket)
		ling.l_linger = 0;  // close session in this time
		setsockopt(m_sockListen, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(m_sockListen);
	}
	SAFE_CLOSESOCKET(m_sockListen);
}

VOID CSendPriceSvr::SendMessageToIocpThread(int Message)
{
	for( int i=0; i<WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			,0
			, (ULONG_PTR)Message
			, NULL
		);
	}
}

void CSendPriceSvr::DelClient_DelCK_DelList()
{
	EnterCriticalSection(&m_csDelCK);

	list<COMPLETION_KEY*>::iterator it;
	for (it = m_listDelCK.begin(); it != m_listDelCK.end(); )
	{
		if ((*it)->Is_BeingUsed()) {
			++it;
			continue;
		}

		COMPLETION_KEY* pCK = (*it);

		for (auto it = m_mapSendThrdIds.begin(); it != m_mapSendThrdIds.end(); it++)
		{
			TRegClient* pReg = CreateRegClientInfo(pCK->sock, "", "");
			PostThreadMessage((*it).second, WM_UNREG_CLIENT, (WPARAM)0, (LPARAM)pReg);
		}
		DelClient_CloseSock(pCK->sock);
		delete pCK;

		it = m_listDelCK.erase(it);
	}
	LeaveCriticalSection(&m_csDelCK);

}


void CSendPriceSvr::DelClient_CloseSock(SOCKET sock)
{
	shutdown(sock, SD_BOTH);
	// TIME-WAIT 없도록
	struct linger structLinger;
	structLinger.l_onoff = 1;
	structLinger.l_linger = 0;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (LPSTR)&structLinger, sizeof(structLinger));
	closesocket(sock);	
}

void CSendPriceSvr::DelClient_AddCK_DelList(COMPLETION_KEY* pCompletionKey)
{
	EnterCriticalSection(&m_csDelCK);
	m_listDelCK.push_back(pCompletionKey);
	LeaveCriticalSection(&m_csDelCK);
}


/*
1.	WSASend / WSARecv 를 호출한 경우는 사용한 socket 과 CK 가 연결되어 있으므로
pCompletionKey 를 통해서 CK 의 포인터가 나온다.

2.	PostQueuedCompletionStatus 를 호출한 경우는 socket 이 사용되지 않으므로
이때는 WM_MSG 를 보내도록 한다.

3.	확장된 OVERLAPPED 에 context 필드가 있으므로 여기에 CTX_DIE, CTX_RQST_SEND, CTX_RQST_RECV 를 채워서 보낸다.

*/
unsigned WINAPI CSendPriceSvr::Thread_Iocp(LPVOID lp)
{
	CSendPriceSvr* pThis = (CSendPriceSvr*)lp;

	COMPLETION_KEY	*pCK = NULL;
	IO_CONTEXT		*pIoContext = NULL;
	DWORD			dwBytesTransferred = 0;
	DWORD           dwIoSize = 0;
	DWORD           dwRecvNumBytes = 0;
	DWORD           dwSendNumBytes = 0;
	DWORD           dwFlags = 0;
	LPOVERLAPPED	pOverlap = NULL;
	BOOL bRet;
	char debug[1024];

	while (pThis->m_bRun)
	{
		bRet = GetQueuedCompletionStatus(pThis->m_hCompletionPort,
			&dwIoSize,
			(LPDWORD)&pCK,
			(LPOVERLAPPED *)&pOverlap,
			INFINITE);

		pIoContext = (IO_CONTEXT*)pOverlap;

		if (pCK == NULL)	return -1;		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력		
		if (pOverlap == NULL)	return -1;	// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pIoContext->context == CTX_DIE)	return -1;

		if (pIoContext->context == CTX_RQST_RECV)
		{
			if (bRet==FALSE || dwIoSize == 0)
			{
				pThis->DelClient_AddCK_DelList(pCK);
				continue;
			}

			__REF_1_HEAD__
			pCK->AddRefer();
			
			pThis->RequestRecvIO(pCK->sock);

			//FOR DEBUGGING
			sprintf(debug, "%.*s", dwIoSize, pIoContext->buf);
			debug[dwIoSize - 1] = 0x00;
			LOGGING(LOGTP_SUCC, FALSE, "[CLIENT](%s)", debug);

			pThis->m_parser.AddPacket(pCK->sock, pIoContext->buf, dwIoSize);

			PostThreadMessage(pThis->m_unParsing, __MAX::WM_RECEIVE_DATA, (WPARAM)0, (LPARAM)pCK);
		}

		if (pIoContext->context == CTX_RQST_SEND)
		{
			
		}
		delete pIoContext;

	} // while

	//mt4helper.disconnect();

	return 0;
}


unsigned WINAPI CSendPriceSvr::Thread_Parsing(LPVOID lp)
{
	CSendPriceSvr*	pThis = (CSendPriceSvr*)lp;
	char	zRecvBuff[__MAX::BUFLEN_4K];
	int		nLen = 0;

	BOOL	bContinue = FALSE;

	while (pThis->m_bRun)
	{
		Sleep(1);
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == FALSE)
			continue;
		if (msg.message != __MAX::WM_RECEIVE_DATA)
		{
			((COMPLETION_KEY*)msg.lParam)->Release();
			continue;
		}

		COMPLETION_KEY* pCK = (COMPLETION_KEY*)msg.lParam;
		bContinue = TRUE;
		while (bContinue)
		{
			ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
			bContinue = pThis->m_parser.GetOnePacket(pCK->sock, &nLen, zRecvBuff);
			if (nLen <= 0)
			{
				break;	//--------------------------------//
			}

			//TODO
			Sleep(0);

		} // while (bContinue)

		__REF_1_TAIL__
		pCK->Release(); /******************/


	} // while(pThis->m_bRun)
	
	
	return 0;
}





unsigned WINAPI CSendPriceSvr::Thread_Listen(LPVOID lp)
{
	CSendPriceSvr *pThis = (CSendPriceSvr*)lp;

	SOCKADDR_IN			sinClient;
	int	sinSize			= sizeof(sinClient);
	long nLoop			= 0;
	int nHearbeatCnt	= 0;

	while (pThis->m_bRun)
	{
		DWORD dw = WSAWaitForMultipleEvents(1, &pThis->m_hListenEvent, TRUE, 10, FALSE);
		if (dw != WSA_WAIT_EVENT_0) 
		{
			if (dw == WSA_WAIT_TIMEOUT)
			{
				// conductinog this function while being idle
				pThis->DelClient_DelCK_DelList();
				continue;
			}
		}

		WSAResetEvent(pThis->m_hListenEvent);		
		
		SOCKET sockClient = accept(pThis->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (sockClient == INVALID_SOCKET) 
		{
			int nErr = WSAGetLastError();
			LOGGING(LOGTP_ERR, TRUE, TEXT("accept error:%d"), nErr);
			{ // Socket operation on nonsocket.
				pThis->InitListen();
				Sleep(3000);
			}
			continue;
		}

		int nZero = 0;
		if (SOCKET_ERROR == setsockopt(sockClient, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof(int)))
		{
			shutdown(sockClient, SD_SEND);
			closesocket(sockClient);
			LOGGING(LOGTP_ERR, TRUE, TEXT("setsockopt error : %d"), WSAGetLastError);
			continue;;
		}

		char zIp[32];
		strcpy(zIp, inet_ntoa(sinClient.sin_addr));


		// CLIENT 동작을 정확히 알아야 한다.
		//int nCnt = 0;
		//if (!pThis->Is_ConnAllowed(zIp, &nCnt))
		//{
		//	LOGGING(LOGTP_ERR, TRUE, "[%s] IP 가 (%d)초 내에 (%d) 회 이상 연결 시도. 연결 거부 한다.", zIp
		//		, pThis->m_nBlockDurationSec, pThis->m_nMaxConnPerIP);
		//	shutdown(sockClient, SD_SEND);
		//	closesocket(sock Client);
		//	continue;
		//}

		//	CK 와 IOCP 연결
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;
		pCK->sClientIp	= zIp;
		pCK->acptTime	= time(nullptr);

		HANDLE h = CreateIoCompletionPort((HANDLE)pCK->sock,
			pThis->m_hCompletionPort,
			(DWORD)pCK,
			0);
		if (h == NULL)
		{
			delete pCK;
			closesocket(sockClient);
			continue;
		}

		// PacketParser
		pThis->m_parser.AddSocket(sockClient);

		// reg socket to all threads
		for (auto it = pThis->m_mapSendThrdIds.begin(); it != pThis->m_mapSendThrdIds.end(); it++)
		{
			TRegClient* pReg = CreateRegClientInfo(pCK->sock, zIp, "");
			PostThreadMessage((*it).second, WM_REG_CLIENT, (WPARAM)0, (LPARAM)pReg);
		}

		LOGGING(INFO, TRUE, TEXT("Connected[socket:%d][IP:%s]"), sockClient, zIp);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK->sock);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}


//BOOL CSendPriceSvr::Is_ConnAllowed(string sClientIP, _Out_ int* pnCurrConnCnt)
//{
//	int count = 0;
//	time_t currentTime = time(nullptr);
//
//	EnterCriticalSection(&m_csConnections);
//	for (const auto& conn : m_vecConnections) {
//		if (conn.sIP == sClientIP && (currentTime - conn.acptTime) < m_nBlockDurationSec) {
//			++count;
//		}
//	}
//	LeaveCriticalSection(&m_csConnections);
//
//	*pnCurrConnCnt = count;
//	return (count < m_nMaxConnPerIP);
//}


void CSendPriceSvr::RequestRecvIO(SOCKET sock)
{
	IO_CONTEXT* pRecv = NULL;
	DWORD dwNumberOfBytesRecvd = 0;
	DWORD dwFlags = 0;

	BOOL bRet = TRUE;
	try {
		pRecv = new IO_CONTEXT;
		pRecv->wsaBuf.buf = pRecv->buf;
		pRecv->wsaBuf.len = __MAX::BUFLEN_1K;
		pRecv->context = CTX_RQST_RECV;

		int nRet = WSARecv(sock
			, &(pRecv->wsaBuf)
			, 1, &dwNumberOfBytesRecvd, &dwFlags
			, &(pRecv->overLapped)
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				//LOGGING(LOGTP_ERR, TRUE, TEXT("WSARecv error : %d"), WSAGetLastError());
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		//LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	//printf("RequestRecvIO ok\n");
	return;
}

VOID CSendPriceSvr::RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen)
{

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		//ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				//LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		//LOGGING(ERR, TRUE, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;
	//else
	//	LOGGING(INFO, FALSE, "[SEND](sock:%d)(%s)", sock, pSendBuf);
	return;
}

unsigned WINAPI CSendPriceSvr::Thread_SendData(LPVOID lp)
{
	map<string, TRegClient*> mapSock;

	TThrdArgc* pArgc		= (TThrdArgc*)lp;	
	CSendPriceSvr* pThis	= (CSendPriceSvr*)pArgc->pThis;
	string sStkCd			= pArgc->sStkCd;
	
	auto findQ = g_mapQ.find(sStkCd);
	if (findQ == g_mapQ.end())
	{
		LOGGING(LOGTP_ERR, FALSE, "[%s]의 큐가 생성되지 않았습니다.", sStkCd.c_str());
		return FALSE;
	}
	CCircularQ* pQ = (*findQ).second;
	pQ->R_AddReader(GetCurrentThreadId());

	char zMsg[1024];

	while (pThis->m_bRun)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			if (msg.message == WM_REG_CLIENT)
			{
				TRegClient* pClient = new TRegClient;
				memcpy(pClient, (TRegClient*)msg.lParam, sizeof(TRegClient));
				mapSock[pClient->sSockOrg] = pClient;
			}
			else if (msg.message == WM_UNREG_CLIENT)
			{
				TRegClient* p = (TRegClient*)msg.lParam;
				auto it = mapSock.find(p->sSockOrg);
				if (it != mapSock.end()) 
				{
					TRegClient* pClient = (TRegClient*)(*it).second;
					CloseHandle(pClient->hSock);
					delete pClient;
				}
			}
			delete(char*)msg.lParam;
		}

		//READ Q & Send Data
		BOOL bSucc			= FALSE;
		int nArraySize		= 0;
		TCQUnit* pArrUnit	= pQ->R_ReadNewData_Array(GetCurrentThreadId(), &bSucc, &nArraySize, zMsg);
		if (!bSucc) {
			LOGGING(LOGTP_ERR, FALSE, "(%s)", zMsg);
			continue;
		}

		if (nArraySize > 0)
		{
			for (int k = 0; k < nArraySize; k++)
			{
				for (auto itMap = mapSock.begin(); itMap != mapSock.end(); ++itMap)
				{
					TRegClient* pUnit = (TRegClient*)(*itMap).second;
					SOCKET sock = reinterpret_cast<SOCKET>(pUnit->hSock);
					pThis->RequestSendIO(sock, pArrUnit[k].buf, pArrUnit[k].dataSize);
				}
			}
			delete[] pArrUnit;
		}
	}

	delete pArgc;
	return 0;
}