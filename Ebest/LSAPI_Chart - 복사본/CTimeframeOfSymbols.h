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
	CSymbol(const CSymbol&)				= delete;	// ���� ����
	CSymbol& operator=(const CSymbol&)	= delete;	// ���Ա���

	//===== ù ��ȸ�� ���� candle time �� base time �� ������Ʈ. ���� 1ȸ�� �Ѵ�.
	void	update_first_candle_tm(const std::string& dt_kor, const std::string& tm_kor_hms);
	
	//=====	�ش� �ɺ��� api ������ ��û�� �ð����� üũ
	bool	check_time_to_apiqry(const char* now_ymd_hms);	//yyyymmdd_hhmmss
	bool	calc_next_apiqry_tm(const char* now_ymd_hms);	//yyyymmdd_hhmmss
	

	//================== util �Լ��� ==================//
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
	CTimeframeOfSymbols(const CTimeframeOfSymbols&) = delete;				// ���� ����
	CTimeframeOfSymbols& operator=(const CTimeframeOfSymbols&) = delete;	// ���Ա���

	//=====	��� symbol �鿡 ���� api ������ ��û�� �ð����� üũ
	int		check_time_to_apiqry_symbols(const char* now_ymd_hms);	//yyyymmdd_hhmmss

	//===== ù ��ȸ�� ���� candle time �� base time �� ������Ʈ. ���� 1ȸ�� �Ѵ�.
	bool	update_first_candle_tm(std::string& symbol, std::string& dt_kor, std::string& tm_kor_hms);

	//================== util �Լ��� ==================//
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

//==================== �������� ���� ====================//

extern std::mutex											__mtx_tfs_symbols;
extern std::map<int, std::shared_ptr<CTimeframeOfSymbols>>	__map_tfs_symbols;

//==================== �����Լ� ���� ====================//
bool __update_first_candle_tm(std::string& timeframe, std::string& symbol, 
								std::string& dt_kor, std::string& tm_kor_hms);
