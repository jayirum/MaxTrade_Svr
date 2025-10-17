#pragma once




#include <winsock2.h>
#include "../Common/MaxTradeInc.h"
#include "../common/LogMsg.h"
#include "CStkInfo.h"
#include <string>
using namespace std;


#define WM_CNTR				WM_USER + 3534
#define WM_MKT_CLOSE		WM_CNTR+1
#define WM_UNREG_CLIENT		WM_CNTR+2
#define WM_INOUT			WM_CNTR+3

struct TData
{
	char data[__MAX::BUFLEN_1K];
	TData() { ZeroMemory(data, sizeof(data)); }
};

void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg);
void	RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen);
void 	RequestRecvIO(SOCKET sock);
void	LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
BOOL	ReadConfig(char* pzSection, char* pzKey, _Out_ char* pVal);
BOOL	MakeDBConnStr(_Out_ char* pzConnStr);