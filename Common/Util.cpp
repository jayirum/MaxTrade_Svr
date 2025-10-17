
//#include <afx.h>
#include <stdio.h>
#include <stdarg.h>
#include "Util.h"
//#include "Prop.h"
#include "Screwdb.h"
//#include "StringUtils.h"
#pragma warning(disable : 4530)
#pragma warning(disable : 4267)

//void __CheckBool(bool res, char* msg)
//{
//	if (!res)
//		throw msg;
//}



//3F2504E0-4F89-11D3-9A0C0305E82C3301
char* __UTILS::MakeGUID(char *pzGUID)
{
	_GUID TestGUID;

	// CoCreateGuid �����ϱ�
	CoCreateGuid(&TestGUID);

	// ������ GUID�� 829C1584-C57B-4dac-BCE7-6F33455F747A �� ���� �������� ��ȯ.
	sprintf(pzGUID, "%.8X-%.4X-%.4X-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X",
		TestGUID.Data1, TestGUID.Data2, TestGUID.Data3, TestGUID.Data4[0],
		TestGUID.Data4[1], TestGUID.Data4[2], TestGUID.Data4[3], TestGUID.Data4[4],
		TestGUID.Data4[5], TestGUID.Data4[6], TestGUID.Data4[7]
	);

	return pzGUID;
}

DWORD	__UTILS::ReportException(DWORD dExitCode, const char* psPos, _Out_ char* pzMsgBuff) // 20120510
{
	switch (dExitCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: strcpy(pzMsgBuff, "EXCEPTION_ACCESS_VIOLATION");		break;
	case EXCEPTION_BREAKPOINT: strcpy(pzMsgBuff, "EXCEPTION_BREAKPOINT");			break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: strcpy(pzMsgBuff, "EXCEPTION_DATATYPE_MISALIGNMENT"); break;
	case EXCEPTION_SINGLE_STEP: strcpy(pzMsgBuff, "EXCEPTION_SINGLE_STEP");			break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: strcpy(pzMsgBuff, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: strcpy(pzMsgBuff, "EXCEPTION_FLT_DENORMAL_OPERAND");	break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: strcpy(pzMsgBuff, "EXCEPTION_FLT_DIVIDE_BY_ZERO");	break;
	case EXCEPTION_FLT_INEXACT_RESULT: strcpy(pzMsgBuff, "EXCEPTION_FLT_INEXACT_RESULT");	break;
	case EXCEPTION_FLT_INVALID_OPERATION: strcpy(pzMsgBuff, "EXCEPTION_FLT_INVALID_OPERATION"); break;
	case EXCEPTION_FLT_OVERFLOW: strcpy(pzMsgBuff, "EXCEPTION_FLT_OVERFLOW");			break;
	case EXCEPTION_FLT_STACK_CHECK: strcpy(pzMsgBuff, "EXCEPTION_FLT_STACK_CHECK");		break;
	case EXCEPTION_FLT_UNDERFLOW: strcpy(pzMsgBuff, "EXCEPTION_FLT_UNDERFLOW");		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: strcpy(pzMsgBuff, "EXCEPTION_INT_DIVIDE_BY_ZERO");	break;
	case EXCEPTION_INT_OVERFLOW: strcpy(pzMsgBuff, "EXCEPTION_INT_OVERFLOW");			break;
	case EXCEPTION_PRIV_INSTRUCTION: strcpy(pzMsgBuff, "EXCEPTION_PRIV_INSTRUCTION");		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: strcpy(pzMsgBuff, "EXCEPTION_NONCONTINUABLE_EXCEPTION"); break;
	default:sprintf(pzMsgBuff, "[except code:%d]undefined error", dExitCode); break;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


BOOL __UTILS::ReadConfig(char* pzCnfgfileName, char* pzSection, char* pzKey, _Out_ char* pVal)
{
	return(CUtil::GetConfig(pzCnfgfileName, pzSection, pzKey, pVal) != NULL);
}



// yyyymmdd-hhmmss
void __UTILS::UnixTimestamp_to_GmtTime(int64_t timestamp, char* out)
{
	// �и��ʸ� �� ������ ��ȯ
	std::time_t event_time_sec = timestamp / 1000;

	// ��ȯ�� �ð��� ���
	std::tm* timeinfo = std::gmtime(&event_time_sec); // UTC ����

	sprintf(out, "%04d%02d%02d_%02d%02d%02d",
		timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}


// yyyymmdd-hhmmss
void __UTILS::UnixTimestamp_to_KoreanTime(int64_t timestamp, char* out)
{
	// �и��ʸ� �� ������ ��ȯ
	std::time_t event_time_sec = timestamp / 1000;

	// ��ȯ�� �ð��� ���
	std::tm* timeinfo = std::gmtime(&event_time_sec); // UTC ����

	// 9�ð� �߰��Ͽ� KST ��ȯ
	timeinfo->tm_hour += 9;
	std::mktime(timeinfo);  // ��ȯ ����

	sprintf(out, "%04d%02d%02d_%02d%02d%02d",
		timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}


std::string __UTILS::trimRight(const std::string& str)
{
	size_t end = str.find_last_not_of(' ');
	return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string __UTILS::trimLeft(const std::string& str) {
	size_t start = str.find_first_not_of(' ');
	return (start == std::string::npos) ? "" : str.substr(start);
}

// ���� ���� ����
std::string __UTILS::trim(const std::string& str) {
	return trimRight(trimLeft(str));
}


char* __UTILS::RTrim(char* pBuf, int nScope)
{
	if (strlen(pBuf) == 0)
		return NULL;

	for (int i = nScope - 1; i >= 0; i--)
	{
		if (*(pBuf + i) == 0x20 ||
			*(pBuf + i) == '\t' ||
			*(pBuf + i) == '\r' ||
			*(pBuf + i) == '\n'
			)
		{
			*(pBuf + i) = 0x00;
		}
		else
		{
			break;
		}
	}
	return pBuf;
}

char* __UTILS::LTrim(char* pBuf)
{
	int nLen = lstrlen(pBuf);
	if (nLen == 0)
		return NULL;

	char* pTmp = new char[nLen + 1];
	lstrcpy(pTmp, pBuf);

	int nPos = 0;
	for (int i = 0; i < nLen; i++) {
		if (*(pTmp + nPos) == 0x20 ||
			*(pTmp + nPos) == '\n' ||
			*(pTmp + nPos) == '\r' ||
			*(pTmp + nPos) == '\t'
			)
		{
			nPos++;
		}
		else
		{
			break;
		}
	}

	lstrcpy(pBuf, pTmp + nPos);
	delete[] pTmp;
	return pBuf;
}



CUtil::CUtil(){}
CUtil::~CUtil(){}




/**	\brief	double �� �������� �Ҽ��� ���ϸ� ����� �����κи� �����Ѵ�.

  \param	dSrc	a parameter of type double
  
	\return	static inline double
*/
double CUtil::TruncDbl(double dSrc)
{
	double dInt = 0;
	modf(dSrc, &dInt);
	return dInt;
}


/**	\brief	double �� �������� ���� �ڸ��� ������ �Ҽ��� ������.

  \param	dSrc	��ȯ�ϰ��� �ϴ� ��
  \param	dPos	�ڸ��� ��ġ
  
	\return	static inline double
	
	  ex) 123.123 �� �Ҽ��� 2�ڸ� ���ϴ� ������.
	  => TruncDbl2(123.123, 2)
*/
double CUtil::TruncDbl2(double dSrc, double dPos)
{
	double dInt = 0;
	double dMultiple = pow(10, dPos);	//	100
	double dSrcCopy = dSrc;				//	123.123
	
	dSrcCopy = dSrcCopy * dMultiple;	//	dSrcCopy = 12312.3
	modf(dSrcCopy, &dInt);				//	dInt = 12312
	
	double dResult = dInt / dMultiple;	//	dResult = 12312 / 100 = 123.12
	return dResult;
}


/**	\brief	double �� �������� �Ҽ��� �̻��� ���� �ڸ��� ������.

  \param	src			��ȯ�ϰ��� �ϴ� ��
  \param	nOffSet		�������� �ϴ� �ڸ���
  
	\return	static inline double
	
	  ex) 12345 ==> 10000
	  Round(12345, 4)
	  
*/
double CUtil::Round(double src, int nOffSet)
{
	double dMultiple = pow((double)10, (double)nOffSet);	//	10000
	
	//	floor : �ش� �� ���� �ִ� ���� ex) 1.2345 => 1
	double dRet = floor( src / dMultiple );	//	dRet = 1
	dRet *= dMultiple;						//	dRet = 1 * 10000 = 10000
	return dRet;
}


/*
	�ݿø�.

	56.349  => �Ҽ����ڸ����� �ڸ��� : 56.34

	roundoff(56.349, 2)
*/
double CUtil::roundoff(double src, int offset)
{
	//double dMultiple = pow((double)10, (double)offset);	//	100

	//int nRet = (int)((src * dMultiple)+0.5);	//	nRet = 56.349*100 + 0.5 = 5634.9 + 0.5 = 5635.4 => 5635
	//double dRet = (double)(nRet / dMultiple);	//	dRet = 5635 / 100 = 56.35

	double dMultiple = pow((double)10, (double)offset);
	double dRet = floor(src * dMultiple + 0.5) / dMultiple;
	return dRet;
}


// VOID CUtil::CopyRAlign(TCHAR* pDest, const TCHAR *pSrc, long destSize, long srcSize,TCHAR cFiller)
// {
// 	//	Filler �� �ʱ�ȭ
// 	FillMemory(pDest, destSize, cFiller);
// 	
// 	//	�Ҵ��� �޸� block ���� copy�� block �� �� ũ�� 
// 	//	�Ҵ�� �޸� block ũ��� �����.
// 	if(srcSize>destSize)
// 		srcSize = destSize;
// 	long lPos = destSize - srcSize;
// 	CopyMemory( pDest+lPos, pSrc, srcSize );
// }







VOID CUtil::FormatErrMsg(_In_ int nErrNo, _Out_ char* pzMsg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErrNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	strcpy(pzMsg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//VOID CUtil::PrintErr(CLog* pLog, BOOL bDebug, int nErrNo)
//{
//	LPVOID lpMsgBuf=NULL;
//	FormatMessage( 
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//		FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL,
//		nErrNo,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//		(LPTSTR) &lpMsgBuf,
//		0,
//		NULL 
//		);
//	if (bDebug) 
//	{
//		printf("[Error](%d)(%s)",nErrNo, (LPCTSTR)lpMsgBuf);
//	}
//	
//	if(pLog)
//	{
//		pLog->Log("[Error](%d)(%s)",nErrNo, (LPSTR)lpMsgBuf);
//	}
//	
//	LocalFree( lpMsgBuf );
//}



#define	DEF_BUF_LEN	4096
//
//void CUtil::LogMsg( CLog *log, BOOL bSucc, char* pMsg, ...)
//{
//	char buff1[DEF_BUF_LEN];
//	char buff2[DEF_BUF_LEN];
//	va_list argptr;
//	SYSTEMTIME	st;
//	
//	if(lstrlen(pMsg)>=DEF_BUF_LEN)
//		*(pMsg+DEF_BUF_LEN-1) = 0x00;
//
//	va_start(argptr, pMsg);
//	vsprintf_s(buff1, pMsg, argptr);
//	va_end(argptr);
//	
//	
//	GetLocalTime(&st);
//	if(bSucc)
//		sprintf(buff2, "[I][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//	else
//		sprintf(buff2, "[F][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//
//	log->LogEx(buff2);
//}



void CUtil::logOutput(char* pMsg, ...)
{
	char buff1[512], buff2[512];
	va_list argptr;
	SYSTEMTIME	st;


	va_start(argptr, pMsg);
	vsprintf_s(buff1, pMsg, argptr);
	va_end(argptr);


	GetLocalTime(&st);
	sprintf(buff2, "[%02d:%02d:%02d.%03d]%s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);

	//OutputDebugString(buff2);
}


//void CUtil::LogPrint( CLog *log, BOOL bSucc, char* pMsg, ...)
//{
//	char buff1[DEF_BUF_LEN];
//	char buff2[DEF_BUF_LEN];
//	va_list argptr;
//	SYSTEMTIME	st;
//	
//	if(lstrlen(pMsg)>=DEF_BUF_LEN)
//		*(pMsg+DEF_BUF_LEN-1) = 0x00;
//
//	va_start(argptr, pMsg);
//	vsprintf_s(buff1, pMsg, argptr);
//	va_end(argptr);
//	
//	
//	GetLocalTime(&st);
//	if(bSucc)
//		sprintf(buff2, "[I][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//	else
//		sprintf(buff2, "[F][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//
//	log->LogEx(buff2);
//	printf(buff2);
//}
//
//
//char* CUtil::LogMsgEx( CLog *log, BOOL bSucc, char* pOut, char* pMsg, ...)
//{
//	char buff1[DEF_BUF_LEN];
//	va_list argptr;
//	SYSTEMTIME	st;
//	
//	if(lstrlen(pMsg)>=DEF_BUF_LEN)
//		*(pMsg+DEF_BUF_LEN-1) = 0x00;
//
//	va_start(argptr, pMsg);
//	vsprintf_s(buff1, pMsg, argptr);
//	va_end(argptr);
//	
//	
//	GetLocalTime(&st);
//	if(bSucc)
//		sprintf(pOut, "[I][%02d:%02d:%02d.%03d]%s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//	else
//		sprintf(pOut, "[F][%02d:%02d:%02d.%03d]%s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//
//	log->log(pOut);
//	return pOut;
//}



//
//void CUtil::LogEmergency( CLog *log, BOOL bSucc, char* pMsg, ...)
//{
//	char buff1[DEF_BUF_LEN];
//	char buff2[DEF_BUF_LEN];
//	va_list argptr;
//	SYSTEMTIME	st;
//	
//	if(lstrlen(pMsg)>=DEF_BUF_LEN)
//		*(pMsg+DEF_BUF_LEN-1) = 0x00;
//
//	va_start(argptr, pMsg);
//	vsprintf_s(buff1, pMsg, argptr);
//	va_end(argptr);
//	
//	
//	GetLocalTime(&st);
//	sprintf(buff2, "[E][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//
//	log->LogEx(buff2);
//}



///************************************************************************
//	���� �ð��� ������ �ð�(��)�� �������� ����
//
//	-- i_psLastTime : hhmmss ����
//************************************************************************/
//BOOL CUtil::IsPassedMin( TCHAR* i_psLastTime, int i_nBaseMin )
//{
//	TCHAR	b_szNowH[32];
//	TCHAR	b_szNowM[32];
//
//	TCHAR	b_szLastH[32];
//	TCHAR	b_szLastM[32];
//
//	SYSTEMTIME	st;
//
//	GetLocalTime(&st);
//
//	sprintf_s( b_szLastH, "%.2s", i_psLastTime);
//	sprintf_s( b_szNowH, "%02d", st.wHour );
//
//	//	HOUR �� �������� �̹� ���� ����
//	if( atoi(b_szNowH) > atoi(b_szLastH) )	
//		return TRUE;
//
//	sprintf_s( b_szLastM, "%.2s", i_psLastTime+2);
//	sprintf_s( b_szNowM, "%02d", st.wMinute );
//
//	int nGapMin = atoi( b_szNowM ) - atoi(b_szLastM);
//
//	return ( nGapMin>i_nBaseMin );
//
//}
//
//



///************************************************************************
//	���� ���翡 �ش��ϴ� KS200 ����, �ɼ��� Year, Month ��ȣ ��ȯ
//************************************************************************/
//VOID CUtil::GetYearMonSymbol_KS200(TCHAR* o_psYearSymbol, TCHAR* o_psMonSymbol)
//{
//	TCHAR	szYear[5], szMon[3];
//	SYSTEMTIME st;
//	GetLocalTime(&st);
//	sprintf_s( szYear, "%04d", st.wYear );
//	sprintf_s( szMon, "%02d", st.wMonth );
//
//	if( strcmp( szYear, "2010")==0 )		lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_10 );
//	else if( strcmp( szYear, "2011")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_11 );
//	else if( strcmp( szYear, "2012")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_12 );
//	else if( strcmp( szYear, "2013")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_13 );
//	else if( strcmp( szYear, "2014")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_14 );
//	else if( strcmp( szYear, "2015")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_15 );
//	else if( strcmp( szYear, "2016")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_16 );
//	else if( strcmp( szYear, "2017")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_17 );
//	else if( strcmp( szYear, "2018")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_18 );
//	else if( strcmp( szYear, "2019")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_19 );
//	else if( strcmp( szYear, "2020")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_20 );
//	else if( strcmp( szYear, "2021")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_21 );
//	else if( strcmp( szYear, "2022")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_22 );
//	else if( strcmp( szYear, "2023")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_23 );
//	else if( strcmp( szYear, "2024")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_24 );
//	else if( strcmp( szYear, "2025")==0 )	lstrcpy( o_psYearSymbol, DEF_KS200_YEAR_25 );
//	else sprintf_s( o_psYearSymbol, 2,  "%.1s", szYear+3 );
//
//	if( strcmp(szMon, "10") )		lstrcpy( o_psMonSymbol, DEF_KS200_MON_10 );
//	else if( strcmp(szMon, "11") )	lstrcpy( o_psMonSymbol, DEF_KS200_MON_11 );
//	else if( strcmp(szMon, "12") )	lstrcpy( o_psMonSymbol, DEF_KS200_MON_12 );
//	else sprintf_s( o_psMonSymbol, 2, "%.1s", szMon+1 );
//
//	return ;
//
//}




void CUtil::GetMyModuleAndDir(char *o_psDir, char* o_psModule, char* o_psConfig)
{

	// config file
	char szFullName[_MAX_PATH];
	GetModuleFileName(NULL, szFullName, _MAX_PATH);

	int nLen = strlen(szFullName);
	for (int i = nLen - 1; i > 0; i--)
	{
		if (szFullName[i] == '\\')
		{
			sprintf(o_psDir, "%.*s", i, szFullName);
			strcpy(o_psModule, &(szFullName[i + 1]));
			strcpy(o_psConfig, GetCnfgFileNm(o_psDir, o_psModule, o_psConfig));
			return;
		}
	}
}


//
//TCHAR* CUtil::GetCnfgFileNmOfSvc(TCHAR* i_psSvcNm, char* o_pzDir, TCHAR* o_pzCfgFileNm)
//{
//	TCHAR zRegistry[512], szFullName[1024], szModuleNm[512];
//	sprintf(zRegistry, TEXT("SYSTEM\\CurrentControlSet\\services\\%s"), i_psSvcNm);
//
//	CProp prop;
//	prop.SetBaseKey(HKEY_LOCAL_MACHINE, zRegistry);
//	strcpy(szFullName, prop.GetValue("ImagePath"));
//	
//	int nLen = strlen(szFullName);
//	for (int i = nLen - 1; i > 0; i--)
//	{
//		if (szFullName[i] == '\\')
//		{
//			sprintf(o_pzDir, "%.*s", i, szFullName);
//			strcpy(szModuleNm, &(szFullName[i + 1]));
//			char zTemp[512] = { 0 };
//			for (UINT k = 0; k < strlen(szModuleNm); k++)
//			{
//				zTemp[k] = toupper(szModuleNm[k]);
//			}
//			
//			char* pos = strstr(zTemp, ".EXE");
//			if (pos == 0)
//			{
//				sprintf(o_pzCfgFileNm, "%s\\%s.ini", o_pzDir, szModuleNm);
//			}
//			else
//			{
//				int nLen = strlen(szModuleNm) - strlen(pos);
//				sprintf(o_pzCfgFileNm, "%s\\%.*s.ini", o_pzDir, nLen, szModuleNm);
//			}
//
//
//			return o_pzCfgFileNm;
//		}
//	}
//	return o_pzCfgFileNm;
//}

/*
	config file �̸�
*/
char* CUtil::GetCnfgFileNm(char *i_psDir, char* i_psFileNm, char* o_psValue)
{
	BOOL bSameDir = FALSE;
	char szDir[MAX_PATH], szFileNm[MAX_PATH];

	strcpy(szDir, i_psDir);

	__UTILS::LTrim(szDir);	__UTILS::RTrim(szDir, strlen(szDir));
	if (szDir[0] == '.') {
		bSameDir = TRUE;
	}
	if(strlen(szDir) == 0)
		bSameDir = TRUE;

	// ���� ������ �ƴ� ���
	if (bSameDir == TRUE)
	{
		GetCurrentDirectory(_MAX_PATH, szDir);
	}
	
	if (i_psDir[strlen(szDir) - 1] != '\\')
		strcat(szDir, "\\");
	

	char temp[1024];
	strcpy(temp, i_psFileNm);
	_strupr(temp);
	char* pos = strstr(temp, ".EXE");
	if (pos == 0)
	{
		sprintf(szFileNm, "%s%s.ini", szDir, i_psFileNm);
	}
	else
	{
		int nLen = strlen(i_psFileNm) - strlen(pos);
		sprintf(szFileNm, "%s%.*s.ini", szDir, nLen, i_psFileNm);
	}

	strcpy(o_psValue, szFileNm);
	return o_psValue;
}


/*
config file �̸�
*/
char* CUtil::GetCnfgXMLFileNm(char *i_psDir, char* i_psFileNm, char* o_psValue)
{
	char szDir[MAX_PATH], szFileNm[MAX_PATH];
	BOOL bSameDir = FALSE;

	strcpy(szDir, i_psDir);
	
	__UTILS::LTrim(szDir);	__UTILS::RTrim(szDir, strlen(szDir));
	if (szDir[0] == '.') {
		bSameDir = TRUE;
	}
	if (strlen(szDir) == 0)
		bSameDir = TRUE;

	// ���� ������ �ƴ� ���
	if (bSameDir == TRUE)
	{
		GetCurrentDirectory(_MAX_PATH, szDir);
	}

	if (i_psDir[strlen(szDir) - 1] != '\\')
		strcat(szDir, "\\");

	char temp[1024];
	strcpy(temp, i_psFileNm);
	_strupr(temp);
	char* pos = strstr(temp, ".EXE");
	if (pos == 0)
	{
		sprintf(szFileNm, "%s%s.xml", szDir, i_psFileNm);
	}
	else
	{
		int nLen = strlen(i_psFileNm) - strlen(pos);
		sprintf(szFileNm, "%s%.*s.xml", szDir, nLen, i_psFileNm);
	}

	strcpy(o_psValue, szFileNm);
	return o_psValue;
}

BOOL CUtil::SetConfigValue(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue)
{ 
	if (!WritePrivateProfileStringA(i_psSectionNm, i_psKeyNm, o_psValue, i_psCnfgFileNm))
	{
		//sprintf(m_zMsg, "WritePrivateProfileStringA error(%d)", GetLastError());
		return FALSE;
	}
	return TRUE; 
}

char* CUtil::GetConfig(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue)
{
	*o_psValue = 0x00;
	DWORD dwRet = GetPrivateProfileString(i_psSectionNm, i_psKeyNm, NULL, o_psValue, 1024, (LPCSTR)i_psCnfgFileNm);
	if (dwRet == 0)
		return NULL;

	// �ּ��� ����
	// http:// �Ǵ� https:// �� ����

	char* http = strstr(o_psValue, "http://");
	char* https = strstr(o_psValue, "https://");

	if (!http && !https)
	{
		char* pComment = strstr(o_psValue, "//");
		if (pComment)	*(pComment) = 0x00;

		// tab
		char* pTab = strstr(o_psValue, "\t");
		if (pTab)	*(pTab) = 0x00;

		return o_psValue;
	}
	if( http )
	{
		char* pComment = strstr(http+6, "//");
		if (pComment)
			*(pComment) = 0x00;
	}
	if (https)
	{
		char* pComment = strstr(https + 7, "//");
		if (pComment)
			*(pComment) = 0x00;
	}
	return o_psValue;
}


char* CUtil::GetConfigQuery(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue)
{
	*o_psValue = 0x00;
	DWORD dwRet = GetPrivateProfileString(i_psSectionNm, i_psKeyNm, NULL, o_psValue, 1024, (LPCSTR)i_psCnfgFileNm);
	if (dwRet == 0)
		return NULL;



	// �ּ��� ����
	// http:// �Ǵ� https:// �� ����

	//char* http = strstr(o_psValue, "http://");
	//char* https = strstr(o_psValue, "https://");

	//if (!http && !https)
	//{
	//	char* pComment = strstr(o_psValue, "//");
	//	if (pComment)	*(pComment) = 0x00;

	//	// tab
	//	char* pTab = strstr(o_psValue, "\t");
	//	if (pTab)	*(pTab) = 0x00;

	//	return o_psValue;
	//}
	//if (http)
	//{
	//	char* pComment = strstr(http + 6, "//");
	//	if (pComment)
	//		*(pComment) = 0x00;
	//}
	//if (https)
	//{
	//	char* pComment = strstr(https + 7, "//");
	//	if (pComment)
	//		*(pComment) = 0x00;
	//}

	// Remove line continuation characters('\')
	std::string result(o_psValue);
	size_t pos;
	while ((pos = result.find("\\\n")) != std::string::npos) {
		result.replace(pos, 2, " ");// Replace '\\n' with space
	}
	strcpy(o_psValue, result.c_str());
	return (o_psValue);
}

//
//BOOL CUtil::GetNextConfigData(char* pzCnfgFileNm, char* pzSectionNm, char* pzPrevKeyNm, char* o_pzNextKeyNm, char* o_pzNextValue)
//{
//	char zTemp[1024] = { 0, };
//	DWORD dwRet;
//
//	dwRet = GetPrivateProfileString(pzSectionNm, NULL, NULL, zTemp, sizeof(zTemp), (LPCSTR)pzCnfgFileNm);
//	if (dwRet <= 0)
//		return FALSE;
//
//	std::list<std::string> listKey;
//	SplitDataEx(zTemp, NULL, dwRet, &listKey);
//	if (listKey.empty())
//		return FALSE;
//
//	// ���� ��ȸ
//	if (pzPrevKeyNm[0] == NULL) {
//		// ù��° key �� value ��ȯ
//		strcpy(o_pzNextKeyNm, (*listKey.begin()).c_str());
//		listKey.pop_front();
//		GetPrivateProfileString(pzSectionNm, o_pzNextKeyNm, NULL, o_pzNextValue, sizeof(zTemp), (LPCSTR)pzCnfgFileNm);
//		return TRUE;
//	}
//	else
//	{
//		std::list<std::string>::iterator it;
//		for (it = listKey.begin(); it != listKey.end(); it++)
//		{
//			strcpy(zTemp, (*it).c_str());
//			if (strcmp(zTemp, pzPrevKeyNm) == 0)
//			{
//				it++;
//				if (it == listKey.end())
//					return FALSE;
//
//				strcpy(o_pzNextKeyNm, (*it).c_str());
//				GetPrivateProfileString(pzSectionNm, o_pzNextKeyNm, NULL, o_pzNextValue, sizeof(zTemp), (LPCSTR)pzCnfgFileNm);
//				return TRUE;
//			}
//		}
//	}
//	return FALSE;
//}
//	

bool CUtil::Load_MsgFile(char* pzMsgFile, map<string, string>& map)
{
	char line[128] = { 0, };
	char buf[128] = { 0, };

	bool bFoundSection = false;


	FILE* fp = fopen(pzMsgFile, "r");
	if (fp == NULL)
		return false;

	map.clear();
	while (fgets(line, 100, fp) != NULL)
	{
		char* pEqual = strchr(line, '=');
		if (pEqual == NULL)
			continue;

		char* pComment = strstr(line, "//");
		if (pComment)
			*(pComment) = 0x00;

		/*
		EURUSD=EURUSD.g // this is test
		*/
		int nLenOfLine = strlen(line);	// 15
		int nLenOfEqual = strlen(pEqual);	// 9

		
		// ACNT=12345
		sprintf(buf, "%.*s", nLenOfLine - nLenOfEqual, line);
		std::string sKey = buf;

		sprintf(buf, "%.*s", nLenOfEqual - 1, pEqual + 1);
		std::string sValue = buf;

		map[sKey] = sValue;
	}

	return true;

}


int CUtil::GetTickGap(double dFirstPrc, double dSndPrc, int nDotCnt, double dTickSize)
{
	double dPow = pow(10., (double)nDotCnt);

	dFirstPrc = dFirstPrc * dPow;
	dSndPrc = dSndPrc * dPow;
	int nGapTick = 0;
	nGapTick = (int)((dFirstPrc - dSndPrc) / dTickSize / dPow);
	return nGapTick;
}

double CUtil::GetPrcByTick(char* pzOrigPrc, double dTickCnt, double dTickSize, char cPlusMinus)
{
	double dRsltPrc = atof(pzOrigPrc);
	if(cPlusMinus=='+') dRsltPrc += dTickCnt* dTickSize;
	else				dRsltPrc -= dTickCnt* dTickSize;

	return dRsltPrc;
}

/*
#define	FORMAT_PRC(prc,dotcnt,out) { sprintf(out, "%0*.*f", LEN_PRC, dotcnt, prc); }
*/
int CUtil::CompPrc(const char* pPrc1, const int nLen1, const char* pPrc2, const int nLen2, const int nDotCnt, const int nFormatLen)
{
	char zPrc1[32], zPrc2[32];
	sprintf(zPrc1, "%.*s", nLen1, pPrc1);
	sprintf(zPrc1, "%0*.*f", nFormatLen, nDotCnt, atof(zPrc1));

	sprintf(zPrc2, "%.*s", nLen2, pPrc2);
	sprintf(zPrc2, "%0*.*f", nFormatLen, nDotCnt, atof(zPrc2));
	
	return strncmp(zPrc1, zPrc2, nFormatLen);

}

int CUtil::CompPrc(const double pPrc1, const double pPrc2, const int nDotCnt, const int nFormatLen)
{
	char zPrc1[32], zPrc2[32];
	sprintf(zPrc1, "%0*.*f", nFormatLen, nDotCnt, pPrc1);

	sprintf(zPrc2, "%0*.*f", nFormatLen, nDotCnt, pPrc2);

	return strncmp(zPrc1, zPrc2, nFormatLen);

}


BOOL CUtil::IsSamePrice( char* pPrc1, int nLen1, char* pPrc2, int nLen2, int nDotCnt, int nFormatLen)
{
	char zPrc1[32], zPrc2[32];
	sprintf(zPrc1, "%.*s", nLen1, pPrc1);
	sprintf(zPrc1, "%0*.*f", nFormatLen, nDotCnt, atof(zPrc1));

	sprintf(zPrc2, "%.*s", nLen2, pPrc2);
	sprintf(zPrc2, "%0*.*f", nFormatLen, nDotCnt, atof(zPrc2));

	return (strncmp(zPrc1, zPrc2, nFormatLen)==0);

}




char* __UTILS::GetCnfgValue(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue)
{
	*o_psValue = 0x00;
	DWORD dwRET = GetPrivateProfileString(i_psSectionNm, i_psKeyNm, NULL, o_psValue, 1024, (LPCSTR)i_psCnfgFileNm);
	// �ּ��� ����
	char* pComment = strstr(o_psValue, "//");
	if (pComment)
		*(pComment) = 0x00;
	return o_psValue;
}

void CUtil::SeperateFileName(_In_ char* pzFullName, _Out_ char* pDir, _Out_ char* pPureFileName)
{
	string sFullName = pzFullName;
	size_t idx = sFullName.find_last_of("\\");
	if (idx == -1)
	{
		strcpy(pDir, ".");
		strcpy(pPureFileName, pzFullName);
	}
	else
	{
		int nDirLen = idx;	// ������ \\ ����
		sprintf(pDir, "%.*s", nDirLen, pzFullName);

		int nNameLen = strlen(pzFullName) - idx - 1;
		sprintf(pPureFileName, "%.*s", nNameLen, pzFullName + (idx + 1));
	}
}
