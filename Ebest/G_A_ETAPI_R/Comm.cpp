#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "Comm.h"
#include "../../Common/Util.h"
#include <vector>
#include <map>
using namespace std;

TCHAR	g_zConfigDir[_MAX_PATH] = { 0 };
TCHAR	g_zLogDir[_MAX_PATH] = { 0 };
char	g_zConfigFileName[MAX_PATH] = { 0 };

CSendPriceSvr				g_SendPrcSvr;
CLogMsg						g_log;
vector<string>				g_vecStk;
map<string, CCircularQ*>	g_mapQ;

BOOL ReadConfig(char* pzSection, char* pzKey, _Out_ char* pVal)
{
	return(CUtil::GetConfig(g_zConfigFileName, pzSection, pzKey, pVal) != NULL);
}

void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	g_log.Log(tp, szBuf, bPrintConsole);
}


TRegClient* CreateRegClientInfo(SOCKET sock, string sIP, string sUserID)
{
	TRegClient* p = new TRegClient;
	if (!p->Convert_Duplicate_Socket(sock))
	{
		delete p;
		return NULL;
	}
	p->sIP = sIP; p->sUserID = sUserID; 
	return p;
}

void ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg)
{
#if 0
	char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, pCode);
	set.SetVal(FDS_SUCC_YN, "N");
	set.SetVal(FDN_ERR_CODE, nErrCode);
	set.SetVal(FDS_MSG, pzMsg);

	int nLen = set.Complete(zSendBuff);

	LOGGING(INFO, TRUE, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(sock, zSendBuff, nLen);
#endif
}

void RequestRecvIO(SOCKET sock)
{
#if 0
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

#endif
	return;
}

VOID RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen)
{
#if 0
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

#endif
	return;
}