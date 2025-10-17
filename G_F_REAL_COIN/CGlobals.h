#pragma once

#include "../Common/MaxTradeInc.h"
#include "../Common/LogMsg.h"
#include "../Common/util.h"
#include <atomic>

class CClientNum
{
public:
	CClientNum();
	~CClientNum();
	int IncNum();
	void DecNum() { m_nCurrClientNum--; }
	
	void SetSendersNum(int n) {m_nSendersNum = n;}
private:
	std::atomic<unsigned int>	m_nCurrClientNum;
	int							m_nSendersNum;
};


class CGlobals
{
public:
	CGlobals();
	~CGlobals();

	bool Initialize();
	void log(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
	void debug(const char* pMsg, ...);
	bool getConfig(char* pzSection, char* pzKey, char* pValue);
	int  getSendersNum() { return m_nSendersNum; }

public:
	CClientNum	gClientNum;
	__MAX::CMemoryPool	memPool;

private:
	CLogMsg	m_log;
	char	m_zConDir[_MAX_PATH];
	char	m_zLogDir[_MAX_PATH];
	char	m_zConfigFileName[MAX_PATH];

	int		m_nSendersNum;
	bool	m_bDebugLog;

};


class CThreadFlag
{
public:
	CThreadFlag() :m_threadRun(false), m_threadReady(false) {}

	void setThreadRun() { m_threadRun = true; }
	void setThreadStop() { m_threadRun = false; }
	void setThreadReady() { m_threadReady = true; }

	bool isRunning() { return m_threadRun;}
	bool isReady() { return m_threadReady; }

private:
	std::atomic<bool>			m_threadRun, m_threadReady;
};
