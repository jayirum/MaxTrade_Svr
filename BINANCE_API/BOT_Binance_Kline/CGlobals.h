#pragma once

#include "../../Common/MaxTradeInc.h"
#include "../../Common/LogMsg.h"
#include "../../Common/util.h"



class CGlobals
{
public:
	CGlobals();
	~CGlobals();

	bool Initialize();
	void log(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
	void debug(const char* pMsg, ...);
	bool getConfig(char* pzSection, char* pzKey, char* pValue);
	bool getConfigQuery(char* pzSection, char* pzKey, char* pValue);

public:
	__MAX::CMemoryPool	memPool;
private:
	CLogMsg	m_log;
	char	m_zConDir[_MAX_PATH];
	char	m_zLogDir[_MAX_PATH];
	char	m_zConfigFileName[MAX_PATH];

	int		m_nSendersNum;
	bool	m_bDebugLog;
};



