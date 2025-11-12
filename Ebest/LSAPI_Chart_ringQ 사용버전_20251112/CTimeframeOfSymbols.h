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

constexpr int LEN_TM = 15;


enum class EN_STATUS { CREATED, CANBE_FIRED, FIRED };

class CSymbol
{
public:
	explicit CSymbol(std::string symbol, int tf)
		:m_symbol(symbol), 
		m_tf(tf),
		m_last_qry_ymd_hms{},
		m_next_qry_ymd_hms{},
		m_first_candle_ymd_hms{},
		m_status{ EN_STATUS::CREATED }
		
	{
	}
	CSymbol(const CSymbol&)				= delete;	// 복사 금지
	CSymbol& operator=(const CSymbol&)	= delete;	// 대입금지

	//===== 첫 조회를 통해 candle time 의 base time 을 업데이트. 최초 1회만 한다.
	void	update_first_candle_tm(const std::string& dt_kor, const std::string& tm_kor_hms);
	
	//=====	해당 심볼의 api 쿼리를 요청할 시간인지 체크
	bool	check_time_to_apiqry(const char* now_ymd_hms);	//yyyymmdd_hhmmss
	bool	calc_next_apiqry_tm(const char* now_ymd_hms);	//yyyymmdd_hhmmss
	

	//================== util 함수들 ==================//
	bool		is_time_to_apiqry() { return (m_status==EN_STATUS::FIRED); }
	void		reset_status();	
	bool		is_just_created()	{ return (m_status==EN_STATUS::CREATED); }	
	EN_STATUS	curr_status()		{ return m_status;}

private:
	std::string m_symbol;
	int			m_tf;

	char		m_last_qry_ymd_hms	[LEN_TM + 1];	// yyyymmdd_hhmmss
	char		m_next_qry_ymd_hms	[LEN_TM + 1];	// yyyymmdd_hhmmss

	char		m_first_candle_ymd_hms[LEN_TM +1];	// yyyymmdd_hhmmss
	EN_STATUS	m_status;
};

class CTimeframeOfSymbols
{
public:
	CTimeframeOfSymbols(int timeframe);
	CTimeframeOfSymbols(const CTimeframeOfSymbols&) = delete;				// 복사 금지
	CTimeframeOfSymbols& operator=(const CTimeframeOfSymbols&) = delete;	// 대입금지

	//=====	모든 symbol 들에 대해 api 쿼리를 요청할 시간인지 체크
	int		check_time_to_apiqry_symbols(const char* now_ymd_hms);	//yyyymmdd_hhmmss

	//===== 첫 조회를 통해 candle time 의 base time 을 업데이트. 최초 1회만 한다.
	bool	update_first_candle_tm(std::string& symbol, std::string& dt_kor, std::string& tm_kor_hms);

	//================== util 함수들 ==================//
	void	set_symbol(std::string symbol);
	bool	is_first_updated_all()	{ return m_is_all_first_updated; }
	int		my_timeframe()			{ return m_timeframe;}
	
public:
	std::mutex										m_mtx;
	std::map<SYMBOL_STR, std::unique_ptr<CSymbol>>	m_map_symbols;	//

	int												m_timeframe{0};
	int												m_num_tobe_fired{ 0 };

	bool											m_is_all_first_updated{false};

};

//==================== 전역변수 선언 ====================//

extern std::mutex											__mtx_tfs_symbols;
extern std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;

//==================== 전역함수 선언 ====================//
bool __update_first_candle_tm(std::string& timeframe, std::string& symbol, 
								std::string& dt_kor, std::string& tm_kor_hms);
