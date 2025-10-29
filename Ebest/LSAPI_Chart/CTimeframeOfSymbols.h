#pragma once
#pragma warning(disable:4996)

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include "../../Common/TimeUtils.h"
#include "CGlobals.h"

typedef std::string SYMBOL_STR;

enum class EN_STATUS { CREATED, CANBE_FIRED, FIRED };

class CSymbol
{
public:
	explicit CSymbol(std::string symbol, int tf)
		:m_symbol(symbol), 
		m_tf(tf),
		m_last_candle_tm_kor{},
		m_next_qry_tm{},
		m_status{ EN_STATUS::CREATED } 
	{
	}
	CSymbol(const CSymbol&)				= delete;	// 복사 금지
	CSymbol& operator=(const CSymbol&)	= delete;	// 대입금지

	// after receive api data (not from db)
	void	update_candle_tm(const char* candle_tm_kor );			//yyyymmddhhmmss
	
	bool	check_time_to_apiqry(const char* now_tm);	//yyyymmddhhmmss

	void	calc_next_apiqry_tm();
	
	bool	is_time_to_apiqry() { return (m_status==EN_STATUS::FIRED); }

	void	reset_status(){ m_status=EN_STATUS::CANBE_FIRED;
		__common.debug_fmt("[CSymbol][timeframe:%d][%s] reset status", m_tf, m_symbol.c_str());
	
	}

private:
	std::string m_symbol;
	int			m_tf;
	//char		m_last_candle_dt[8 + 1];	// yyyymmdd
	//char		m_last_candle_tm[6 + 1];	// hhmmss
	char		m_last_candle_tm_kor[14+1];
	char		m_next_qry_tm[14+1];		// yyyymmddhhmmss
	EN_STATUS	m_status;
};

class CTimeframeOfSymbols
{
public:
	CTimeframeOfSymbols(int timeframe);
	CTimeframeOfSymbols(const CTimeframeOfSymbols&) = delete;				// 복사 금지
	CTimeframeOfSymbols& operator=(const CTimeframeOfSymbols&) = delete;	// 대입금지


	void	set_symbol(std::string symbol);
	bool	update_candle_tm(const char* symbol, const char* dt, const char* tm, const char* timediff);
		
	int		check_time_to_apiqry_symbols(const char* now_tm);	//yyyymmddhhmmss

	int		my_timeframe(){ return m_timeframe;}
	

public:
	std::mutex										m_mtx;
	std::map<SYMBOL_STR, std::unique_ptr<CSymbol>>	m_map_symbols;	//

	int												m_timeframe;
	int												m_num_tobe_fired;

};

extern std::mutex											__mtx_tfs_symbols;
extern std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;