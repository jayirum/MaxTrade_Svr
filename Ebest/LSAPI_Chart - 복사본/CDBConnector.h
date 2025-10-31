#pragma once

#include <thread>
#include "../../Common/CODBC.h"

class CDBConnector
{
public:
	CDBConnector();
	~CDBConnector();

	bool	is_connected() { return m_pOdbc->IsConnected(); }

	bool connect_db();
	bool reconnect_db();

public:
	std::unique_ptr <CODBC>	m_pOdbc;
	char					m_zConnStr[512];
	unsigned int			m_dbReconnTry;
	int						m_pingTimeout_sec;
};

