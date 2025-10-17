#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "Comm.h"
#include "../Common/Util.h"

TCHAR	g_zConDir[_MAX_PATH] = { 0 };
TCHAR	g_zLogDir[_MAX_PATH] = { 0 };
char	g_zConfigFileName[MAX_PATH] = { 0 };

CLogMsg		g_log;
CStkInfo	g_stkInfo;

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

BOOL	MakeDBConnStr(_Out_ char* pzConnStr)
{
	char zDsn[128] = { 0 }, zUid[128] = { 0 }, zPwd[128] = { 0 };
	if (!ReadConfig((char*)"DB", (char*)"DSN", zDsn) ||
		!ReadConfig((char*)"DB", (char*)"UID", zUid) ||
		!ReadConfig((char*)"DB", (char*)"PWD", zPwd)
		)
	{
		LOGGING(LOGTP_ERR, TRUE, "DB정보 조회 오류.INI파일을 확인하세요");
		return FALSE;
	}

	sprintf(pzConnStr, "DSN=%s;UID=%s;PWD=%s;", zDsn, zUid, zPwd);

	return TRUE;
}


//
//void RequestRecvIO(SOCKET sock)
//{
//	IO_CONTEXT* pRecv = NULL;
//	DWORD dwNumberOfBytesRecvd = 0;
//	DWORD dwFlags = 0;
//
//	BOOL bRet = TRUE;
//	try {
//		pRecv = new IO_CONTEXT;
//		pRecv->wsaBuf.buf = pRecv->buf;
//		pRecv->wsaBuf.len = __MAX::BUFLEN_1K;
//		pRecv->context = CTX_RQST_RECV;
//
//		int nRet = WSARecv(sock
//			, &(pRecv->wsaBuf)
//			, 1, &dwNumberOfBytesRecvd, &dwFlags
//			, &(pRecv->overLapped)
//			, NULL);
//		if (nRet == SOCKET_ERROR) {
//			if (WSAGetLastError() != WSA_IO_PENDING) {
//				//LOGGING(LOGTP_ERR, TRUE, TEXT("WSARecv error : %d"), WSAGetLastError());
//				bRet = FALSE;
//			}
//		}
//	}
//	catch (...) {
//		//LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend TRY CATCH"));
//		bRet = FALSE;
//	}
//
//	if (!bRet)
//		delete pRecv;
//
//	//printf("RequestRecvIO ok\n");
//	return;
//}
//
//VOID RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen)
//{
//
//	BOOL  bRet = TRUE;
//	DWORD dwOutBytes = 0;
//	DWORD dwFlags = 0;
//	IO_CONTEXT* pSend = NULL;
//
//	try {
//		pSend = new IO_CONTEXT;
//
//		//ZeroMemory(pSend, sizeof(IO_CONTEXT));
//		CopyMemory(pSend->buf, pSendBuf, nSendLen);
//		pSend->wsaBuf.buf = pSend->buf;
//		pSend->wsaBuf.len = nSendLen;
//		pSend->context = CTX_RQST_SEND;
//
//		int nRet = WSASend(sock
//			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
//			, 1					// wsaBuf 포인터 갯수
//			, &dwOutBytes		// 전송된 바이트 수
//			, dwFlags
//			, &pSend->overLapped	// overlapped 포인터
//			, NULL);
//		if (nRet == SOCKET_ERROR) {
//			if (WSAGetLastError() != WSA_IO_PENDING) {
//				//LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend error : %d"), WSAGetLastError);
//				bRet = FALSE;
//			}
//		}
//		//printf("WSASend ok..................\n");
//	}
//	catch (...) {
//		//LOGGING(ERR, TRUE, TEXT("WSASend try catch error [CIocp]"));
//		bRet = FALSE;
//	}
//	if (!bRet)
//		delete pSend;
//	//else
//	//	LOGGING(INFO, FALSE, "[SEND](sock:%d)(%s)", sock, pSendBuf);
//	return;
//}
//TRegClient* CreateRegClientInfo(SOCKET sock, string sIP, string sUserID, string sStkCd1, string sStkCd2, string sStkCd3)
TRegClient* CreateRegClientInfo(LPVOID sock, string sIP, string sUserID, string sStkCd1, string sStkCd2, string sStkCd3)
{
	TRegClient* p = new TRegClient;

	if (!p->Convert_Duplicate_Socket(sock))
	{
		delete p;
		return NULL;
	}

	//if (!DuplicateHandle(GetCurrentProcess(), reinterpret_cast<HANDLE>(sock),
	//	GetCurrentProcess(), &p->hSock, 0, FALSE, DUPLICATE_SAME_ACCESS))
	//{
	//	return FALSE;
	//}
	//char z[32]; sprintf(z, "%d", sock); 
	//p->sSockOrg = z;

	p->sIP = sIP; p->sUserID = sUserID; p->sStkCd1 = sStkCd1; p->sStkCd2 = sStkCd2; p->sStkCd3 = sStkCd3;


	return p;
}