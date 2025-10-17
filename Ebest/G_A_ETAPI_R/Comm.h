#pragma once


#pragma warning( disable : 26495)

#include "CSendPriceSvr.h"
#include "../../Common/MaxTradeInc.h"
#include "../../common/LogMsg.h"
#include "../../common/CODBC.h"
#include "../../common/Util.h"
#include "../../common/CCircularQ.h"

#include <string>
using namespace std;

#ifdef DEF_KF
	#define EXENAME	"G_A_ETAPI_R_KF"
	#define DEF_ACNT_TP	'1'
#elif DEF_GF
	#define EXENAME	"G_A_ETAPI_R_GF"
	#define DEF_ACNT_TP	'2'
#endif
#define EXEC_VERSION	"v1.0"

#define WM_MARKETDATA		WM_USER + 3434
#define WM_REG_CLIENT		WM_MARKETDATA+1
#define WM_UNREG_CLIENT		WM_MARKETDATA+2


struct TRegClient
{
	string sSockOrg;
	HANDLE hSock;
	string sIP;
	string sUserID;

	TRegClient() { hSock = NULL; }
	void Convert_Socket_Str(SOCKET sock) { char z[32]; sprintf(z, "%d", sock); sSockOrg = z; }
	BOOL Convert_Duplicate_Socket(SOCKET sock)
	{
		if (!DuplicateHandle(GetCurrentProcess(), reinterpret_cast<HANDLE>(sock),
			GetCurrentProcess(), &hSock, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			return FALSE;
		}
		Convert_Socket_Str(sock);
		return TRUE;
	}
};
TRegClient* CreateRegClientInfo(SOCKET sock, string sIP, string sUserID);

struct RECV_DATA
{
	SOCKET	sock;
	char	data[__MAX::BUFLEN_1K];
	int		len;
};

void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg);
void	RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen);
void 	RequestRecvIO(SOCKET sock);
void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
BOOL ReadConfig(char* pzSection, char* pzKey, _Out_ char* pVal);