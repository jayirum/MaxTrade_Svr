#include "CGlobals.h"
#include "main.h"




/// <summary>
/// 
/// 
/// </summary>

CGlobals::CGlobals():m_bDebugLog(false)
{ 

}

CGlobals::~CGlobals()
{}

bool CGlobals::Initialize()
{
	//----------------------------------------------------------------------------------------------------//
	
	GetCurrentDirectory(_MAX_PATH, m_zConDir);
	CUtil::GetCnfgFileNm(m_zConDir, (char*)EXENAME, m_zConfigFileName);
	sprintf(m_zLogDir, "%s\\Log", m_zConDir);

	if (!m_log.OpenLog(m_zLogDir, EXENAME))
	{
		printf("Open Log Error(%s)\n", m_zLogDir);
		return false;     
	}

	
	//----------------------------------------------------------------------------------------------------//

	char z[128] = { 0 };
	if (!getConfig((char*)"APP_CONFIG", (char*)"LOG_DEBUG", z)) {
		log(ERR, TRUE, "INI 파일에 LOG_DEBUG 가 없다.");
		return false;
	}
	m_bDebugLog = (z[0] == 'Y') ? true : false;

	//----------------------------------------------------------------------------------------------------//

	return true;
}

void CGlobals::debug(const char* pMsg, ...)
{
	if (!m_bDebugLog)
		return;

	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	m_log.Log(INFO, szBuf, TRUE);
}

void CGlobals::log(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	m_log.Log(tp, szBuf, bPrintConsole);
}

bool CGlobals::getConfig(char* pzSection, char* pzKey, char* pValue)
{
	return CUtil::GetConfig(m_zConfigFileName, pzSection, pzKey, pValue);
}


bool CGlobals::getConfigQuery(char* pzSection, char* pzKey, char* pValue)
{
	return CUtil::GetConfigQuery(m_zConfigFileName, pzSection, pzKey, pValue);
}