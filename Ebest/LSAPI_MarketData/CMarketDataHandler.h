#pragma once


#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <set>
#include "CGlobals.h"
#include "KF_Hoga_FH0.h"
#include "KF_Sise_FC0.h"
#include "OF_Hoga_OVH.h"
#include "OF_Sise_OVC.h"


using namespace std;

constexpr int _BUF_LEN = 1024;

class CMarketDataHandler
{
public:
	CMarketDataHandler(){};
	~CMarketDataHandler();
	
	bool	Initialize();

	void	push_hoga(TDataUnit* p);
	void	push_sise(TDataUnit* p);

private:
	string	compose_pack_sise_kf(TDataUnit*, string symbol);
	string	compose_pack_hoga_kf(TDataUnit*, string symbol);

	string	compose_pack_sise_of(TDataUnit* p);
	string	compose_pack_hoga_of(TDataUnit* p);

private:
	void	set_die() { 
		m_is_continue.store(false); 
		m_queue_hoga.close();
		m_queue_sise.close();
	}
	void	thread_sise();
	void	thread_hoga();
private:
	std::thread				m_thrd_sise, m_thrd_hoga;
	std::atomic_bool		m_is_continue{ true };

	CSafeQueue<TDataUnit>	m_queue_hoga, m_queue_sise;

	char					m_buf_sise[_BUF_LEN]{};
	char					m_buf_hoga[_BUF_LEN]{};
};

