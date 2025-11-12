#pragma once
#include <string>

struct TQUERY_TM
{
	char dt[8 + 1];
	char tm[6 + 1];
};


class CQueryTime
{
public:
	CQueryTime();
	~CQueryTime();


	void	update(char* dt, char* tm);
	void	get(_Out_ std::string& dt, _Out_ std::string& tm);
	char* get_dt() { return m_tm.dt; }
	char* get_tm() { return m_tm.tm; }

private:
	TQUERY_TM		m_tm;
};

