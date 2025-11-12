#include "stdafx.h"
#include "CQueryTime.h"

CQueryTime::CQueryTime()
{
	ZeroMemory(&m_tm, sizeof(m_tm));
}
CQueryTime::~CQueryTime()
{

}


void	CQueryTime::update(char* dt, char* tm)
{
	strcpy(m_tm.dt, dt);
	strcpy(m_tm.tm, tm);
}

void	CQueryTime::get(_Out_ std::string& dt, _Out_ std::string& tm)
{
	dt = m_tm.dt;
	tm = m_tm.tm;
}
