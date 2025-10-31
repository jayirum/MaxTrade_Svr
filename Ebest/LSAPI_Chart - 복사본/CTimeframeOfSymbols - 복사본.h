#pragma once
#pragma warning(disable:4996)

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include "../../Common/TimeUtils.h"
#include "CGlobals.h"

using SYMBOL_STR = std::string;

constexpr int LEN_CANDLE_TM = 13;
constexpr int LEN_QRY_TM = 15;


enum class EN_STATUS { CREATED, CANBE_FIRED, FIRED };

class CSymbol
{
public:
	explicit CSymbol(std::string symbol, int tf)
		:m_symbol(symbol), 
		m_tf(tf),
		m_last_candle_tm_kor{},
		m_next_candle_tm_kor{},
		m_status{ EN_STATUS::CREATED } 
	{
	}
	CSymbol(const CSymbol&)				= delete;	// 복사 금지
	CSymbol& operator=(const CSymbol&)	= delete;	// 대입금지

	// after receive api data (not from db)
	void	update_candle_tm(const char* candle_tm_kor );
	
	bool	check_time_to_apiqry(const char* now_tm_sec);	//yyyymmdd_hhmmss

	//void	calc_next_apiqry_tm();
	void	calc_next_candle_tm();
	
	bool	is_time_to_apiqry() { return (m_status==EN_STATUS::FIRED); }

	void	reset_status(){ m_status=EN_STATUS::CANBE_FIRED;
		__common.debug_fmt("[CSymbol][timeframe:%d][%s] reset status", m_tf, m_symbol.c_str());
	
	}
	//char*	next_qry_tm() { return m_next_qry_tm;}
	bool	is_just_created() { return (m_status==EN_STATUS::CREATED); }
private:
	std::string m_symbol;
	int			m_tf;
	char		m_last_candle_tm_kor[LEN_CANDLE_TM +1];	// yyyymmdd_hhmm
	char		m_next_candle_tm_kor[LEN_CANDLE_TM + 1];	// yyyymmdd_hhmm
	//char		m_next_qry_tm[LEN_QRY_TM +1];			// yyyymmdd_hhmmss
	EN_STATUS	m_status;
};

class CTimeframeOfSymbols
{
public:
	CTimeframeOfSymbols(int timeframe);
	CTimeframeOfSymbols(const CTimeframeOfSymbols&) = delete;				// 복사 금지
	CTimeframeOfSymbols& operator=(const CTimeframeOfSymbols&) = delete;	// 대입금지


	void	set_symbol(std::string symbol);
	bool	update_candle_tm(const char* symbol, const char* dt_kor, const char* tm_kor);
		
	int		check_time_to_apiqry_symbols(const char* now_tm_sec);	//yyyymmdd_hhmmss

	int		my_timeframe(){ return m_timeframe;}
	
public:
	std::mutex										m_mtx;
	std::map<SYMBOL_STR, std::unique_ptr<CSymbol>>	m_map_symbols;	//

	int												m_timeframe;
	int												m_num_tobe_fired;

};

extern std::mutex											__mtx_tfs_symbols;
extern std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;

char* __get_next_qry_time(int tf, std::string& symbol, _Out_ bool& is_first);
bool __update_candle_tm(std::string& timeframe, std::string& symbol, std::string& dt_kor, std::string& tm_kor);