#pragma once


#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <set>
#include "CGlobals.h"

using namespace std;

struct TCandle
{
	int		tf;
	string	tm_ymd_hms;
	string	o,h,l,c;
	int		decimal_cnt;
};

class CCandle
{
public:

	explicit CCandle(set<int> tfs);

	void	queue_add();
private:
	void	set_die() { m_is_finished.store(true); }
	void	thread_worker();
private:
	std::thread						m_worker;
	vector<unique_ptr<TCandle>>		m_candles;
	std::atomic_bool				m_is_finished{false};
	CSafeQueue<TCandle>			m_queue;
};

extern vector<shared_ptr<CCandle>>	__Candles;