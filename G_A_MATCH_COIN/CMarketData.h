#pragma once
#pragma warning(disable:4996)

//���ο� �ü� ���� map ����
//������	"OHLC
//���� �ü� �ð�
//BID, �ð�
//ASK, �ð�"
//getBid_for_match	bid �� �� �� ������
//getAsk_for_match	ask �� ���� �� ������
//���� ������	save_quote / save_tick
//COrder, CMatch, CAPIClient ���� ����


#include "../Common/MaxTradeInc.h"
#include <atomic>
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include "../Common/Util.h"
#include "../common/TimeUtils.h"

using namespace std;

struct TTick
{
	double o, h, l, c;
	std::string time;
	std::string lastUpdateTm;
	int dot_cnt;
	
	TTick(int dot): dot_cnt(dot) {}
	
	void update_lastTime() { CTimeUtils t; lastUpdateTm = t.Time_hhmmssmmm(); }
	bool isFilled() {
		return (o>0 && h>0 && l>0 && c>0);
	}
};

struct TQuote
{
	string time;
	double bid[5];
	double ask[5];
	double vol_bestBid;
	double vol_bestAsk;
	string lastUpdate;
	string lastUpdateTm;
	int dot_cnt;

	TQuote(int dot):dot_cnt(dot){}
	void update_lastTime() { CTimeUtils t; lastUpdateTm = t.Time_hhmmssmmm(); }
	bool isFilled() {
		return (bid[0] > 0 && ask[0] > 0);
	}
};

class CMarketData
{
public:
	CMarketData();
	~CMarketData();

	bool	Initialize();

	bool	save_tick(char* apiData);
	bool	save_quote(char* apiData);
	bool	getBid_for_match(std::string& symbol, _Out_ double& bid, _Out_ double& match_prc, _Out_ double& vol, _Out_ string& lastTime);
	bool	getAsk_for_match(std::string& symbol, _Out_ double& ask, _Out_ double& match_prc, _Out_ double& vol, _Out_ string& lastTime);
	bool	get_quotes_tick(string& symbol, _Out_ TQuote*& quote, _Out_ TTick*& tick);
	bool	is_started(string& symbol) {
		auto it = m_mapTick.find(symbol);
		if (it == m_mapTick.end())
			return false;
		if (it->second->lastUpdateTm.size() == 0)
			return false;

		
		auto itQ = m_mapQuote.find(symbol);
		if (itQ == m_mapQuote.end())
			return false;
		if (itQ->second->lastUpdateTm.size() == 0)
			return false;
		
		return true;
	}
private:

	std::mutex m_mxTick, m_mxQuote;
	std::map< std::string, std::unique_ptr<TTick>>	m_mapTick;
	std::map< std::string, std::unique_ptr<TQuote>>	m_mapQuote;
	
};

