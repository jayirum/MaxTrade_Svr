#pragma warning( disable:4477)

#include "MaxTradeInc.h"
#include <string.h>
#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>
#include "../Common/Util.h"

bool __MAX::Is_SisePacket(const char* pData)
{
	__MAX::TRealHeader* header = (__MAX::TRealHeader*)pData;
	int comp = strncmp(header->packet_cd, __MAX::CD_SISE, sizeof(header->packet_cd));
	return (comp == 0);
}
bool __MAX::Is_HogaPacket(const char* pData)
{
	__MAX::TRealHeader* header = (__MAX::TRealHeader*)pData;
	int comp = strncmp(header->packet_cd, __MAX::CD_HOGA, sizeof(header->packet_cd));
	return (comp == 0);
}

bool __MAX::Is_RegStkPacket(const char* pData)
{
	__MAX::TPC003* p = (__MAX::TPC003*)pData;
	return (strncmp(p->packet_cd, __MAX::CD_REG_STK, __MAX::PACKET_CD_LEN) == 0);
}


char* __MAX::FormatPrc(const double dPrc, _Out_ char* pzPrc)
{
	sprintf(pzPrc, "%010.5f", dPrc);	// 1.14389 ==>  "0001.43890"
	return pzPrc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


__MAX::CPacketBuffer::CPacketBuffer()
{
	InitializeCriticalSectionAndSpinCount(&m_cs, 1000);
}

__MAX::CPacketBuffer::~CPacketBuffer()
{
	RemoveAll(true);
	DeleteCriticalSection(&m_cs);
}

bool __MAX::CPacketBuffer::Add(_In_ char* pInBuf, int nSize)
{
	if (!pInBuf)
		return false;

	bool bRet = true;
	Lock();

	try {
		m_sBuffer += string(pInBuf, nSize);
	}
	catch (...)
	{
		Unlock();
		m_sMsg = "Exception while Add";
		bRet = false;
	}
	Unlock();

	return bRet;
}

bool __MAX::CPacketBuffer::GetOnePacketLock(int* pnLen, char* pOutBuf)
{
	bool bRemain;
	Lock();
	__try
	{
		bRemain = GetOnePacket(pnLen, pOutBuf);
	}
	__finally
	{
		Unlock();
	}
	return bRemain;
}

/*
	return true : buffer has more packet

	Just check STX & ETX
*/

bool __MAX::CPacketBuffer::GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf)
{
	*pnLen = 0;
	*pOutBuf = 0;

	if (m_sBuffer.size() == 0) {
		return false;
	}

	basic_string <char>::size_type find;
	const basic_string <char>::size_type npos = -1;

	// [abcdef\n\rABCDEF\n\r]
	// find = 6
	find = m_sBuffer.find_first_of(PACKET_DELIMITER); 
	if (find == npos)
		return false;

	string sOneData = m_sBuffer.substr(0, find);	// without \n\r
	m_sBuffer.erase(0, find + __MAX::DELIMITER_LEN);

	*pnLen = (int)sOneData.size();
	memcpy(pOutBuf, sOneData.c_str(), sOneData.size());

	return (m_sBuffer.size() > 0);
}




void __MAX::CPacketBuffer::RemoveAll(bool bLock)
{
	if (bLock)	Lock();
	m_sBuffer.erase(m_sBuffer.begin(), m_sBuffer.end());
	if (bLock)	Unlock();
}



void  __MAX::CPacketBuffer::Erase(int nStartPos, int nLen)
{
	m_sBuffer.erase(nStartPos, nLen);
}


__MAX::CPacketBufferIocp::CPacketBufferIocp()
{
	InitializeCriticalSection(&m_cs);
}

__MAX::CPacketBufferIocp::~CPacketBufferIocp()
{
	map<SOCKET, CPacketBuffer*>::iterator it;
	Lock();
	for (it = m_mapBuffer.begin(); it != m_mapBuffer.end(); ++it)
		delete (*it).second;

	m_mapBuffer.clear();
	UnLock();

	DeleteCriticalSection(&m_cs);
}

void __MAX::CPacketBufferIocp::AddSocket(SOCKET sock)
{
	Lock();

	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it != m_mapBuffer.end())
		delete (*it).second;

	CPacketBuffer* parser = new CPacketBuffer;
	m_mapBuffer[sock] = parser;

	UnLock();
}

int	__MAX::CPacketBufferIocp::AddPacket(SOCKET sock, char* pBuf, int nSize)
{
	Lock();

	int nLen = 0;
	CPacketBuffer* pPacket = NULL;
	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		pPacket = new CPacketBuffer;
	}
	else
	{
		pPacket = (*it).second;
	}

	pPacket->Add(pBuf, nSize);
	nLen = pPacket->GetBuffLen();

	UnLock();

	return nLen;
}

BOOL __MAX::CPacketBufferIocp::GetOnePacket(SOCKET sock, _Out_ int* pnLen, _Out_ char* pOutBuf)
{
	*pnLen = 0;
	*pOutBuf = 0;
	BOOL bResult = FALSE;

	Lock();
	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		return bResult;
	}

	CPacketBuffer* pPacket = (*it).second;

	bResult = pPacket->GetOnePacket(pnLen, pOutBuf);


	m_mapBuffer[sock] = pPacket;

	UnLock();

	return bResult;
}


//{
//	"e": "24hrTicker",  // Event type
//		"E" : 123456789,     // Event time
//		"s" : "BTCUSDT",     // Symbol
//		"p" : "0.0015",      // Price change
//		"P" : "250.00",      // Price change percent
//		"w" : "0.0018",      // Weighted average price
//		"c" : "0.0025",      // Last price
//		"Q" : "10",          // Last quantity
//		"o" : "0.0010",      // Open price
//		"h" : "0.0025",      // High price
//		"l" : "0.0010",      // Low price
//		"v" : "10000",       // Total traded base asset volume
//		"q" : "18",          // Total traded quote asset volume
//		"O" : 0,             // Statistics open time
//		"C" : 86400000,      // Statistics close time
//		"F" : 0,             // First trade ID
//		"L" : 18150,         // Last trade Id
//		"n" : 18151          // Total number of trades
//}
void __MAX::compose_ra001_from_binance(std::string& jsonData, std::map<std::string, long>& mapSymbol, char* out)
{
	nlohmann::json binance = nlohmann::json::parse(jsonData);
	std::string s;

	TRA001* pOut = (TRA001*)out;
	memset(out, 0x20, sizeof(TRA001));
	out[sizeof(TRA001)] = 0;


	// header
	memcpy(pOut->header.packet_cd, CD_SISE, sizeof(pOut->header.packet_cd));
	
	s = binance["s"];
	memcpy(pOut->header.stk_cd, s.c_str(), std::min<size_t>(sizeof(pOut->header.stk_cd), s.size()) );
	std::map<std::string, long>::iterator it = mapSymbol.find(s);
	if (it == mapSymbol.end())
		return;
	long dotCnt = (*it).second;
	
	pOut->header.acnt_tp[0] = '2';

	
	// body
	int64_t utcTime = binance["E"];
	char zKoreanTime[128] = { 0 };
	__UTILS::UnixTimestamp_to_KoreanTime(utcTime, zKoreanTime);	// yyyymmdd-hhmmss
	memcpy(pOut->tm, &zKoreanTime[9], 6);

	memset(pOut->kp200, 0x20, sizeof(pOut->kp200));
	pOut->kp200[sizeof(pOut->kp200) - 1] = '0';

	pOut->bsTp[0] = ' ';	//체결구분

	char t[128];
	double prc;

	s = binance["c"];
	prc = std::stod(s);
	sprintf(t, "%*.*f", (int)sizeof(pOut->now_prc), dotCnt, prc);
	//sprintf(t, "%*s", (unsigned int)sizeof(pOut->now_prc), s.c_str());
	memcpy(&pOut->now_prc, t, sizeof(pOut->now_prc));
	//memcpy(pOut->now_prc, s.c_str(), std::min<size_t>(sizeof(pOut->now_prc), s.size()));
	
	s = binance["p"];
	sprintf(t, "%*s", (unsigned int)sizeof(pOut->chg), s.c_str());
	memcpy(pOut->chg, t, sizeof(pOut->chg));

	s = binance["v"];
	sprintf(t, "%*s", (unsigned int)sizeof(pOut->acml_cntr_vol), s.c_str());
	memcpy(pOut->acml_cntr_vol, t, sizeof(pOut->acml_cntr_vol));

	memset(pOut->acml_amt, 0x20, sizeof(pOut->acml_amt));
	pOut->acml_amt[sizeof(pOut->acml_amt) - 1] = '0';

	s = binance["o"];
	prc = std::stod(s);
	sprintf(t, "%*.*f", (int)sizeof(pOut->open), dotCnt, prc);
	memcpy(pOut->open, t, sizeof(pOut->open));

	//sprintf(t, "%*s", (unsigned int)sizeof(pOut->open), s.c_str());


	s = binance["h"];
	prc = std::stod(s);
	sprintf(t, "%*.*f", (int)sizeof(pOut->high), dotCnt, prc);
	memcpy(pOut->high, t, sizeof(pOut->high));
	//sprintf(t, "%*s", (unsigned int)sizeof(pOut->high), s.c_str());

	s = binance["l"];
	prc = std::stod(s);
	sprintf(t, "%*.*f", (int)sizeof(pOut->low), dotCnt, prc);
	memcpy(pOut->low, t, sizeof(pOut->low));


	s = binance["Q"];
	sprintf(t, "%*s", (unsigned int)sizeof(pOut->cntr_vol), s.c_str());
	memcpy(pOut->cntr_vol, t, sizeof(pOut->cntr_vol));
	
	pOut->fluc_rt[sizeof(pOut->fluc_rt)-1]			= '0'; //등락율
	pOut->mkt_basis[sizeof(pOut->mkt_basis) - 1]	= '0';	// 시장BASIS
	pOut->ncntr_fluc[sizeof(pOut->ncntr_fluc) - 1]	= '0';	// 미결제약정증감
	pOut->ncntr_vol[sizeof(pOut->ncntr_vol) - 1]	= '0';		// 미결제약정수량(단위계약)

	pOut->Enter[0] = '\r';
	pOut->Enter[1] = '\n';
}

//{
//	"e": "depthUpdate", // Event type
//		"E" : 1571889248277, // Event time
//		"T" : 1571889248276, // Transaction time
//		"s" : "BTCUSDT",
//		"U" : 390497796,     // First update ID in event
//		"u" : 390497878,     // Final update ID in event
//		"pu" : 390497794,    // Final update Id in last stream(ie `u` in last stream)
//		"b" : [              // Bids to be updated
//			[
//				"7403.89",      // Price Level to be updated
//				"0.002"         // Quantity
//			],
//				[
//					"7403.90",
//					"3.906"
//				],
//				[
//					"7404.00",
//					"1.428"
//				],
//				[
//					"7404.85",
//					"5.239"
//				],
//				[
//					"7405.43",
//					"2.562"
//				]
//		],
//		"a": [              // Asks to be updated
//			[
//				"7405.96",      // Price level to be
//				"3.340"         // Quantity
//			],
//				[
//					"7406.63",
//					"4.525"
//				],
//				[
//					"7407.08",
//					"2.475"
//				],
//				[
//					"7407.15",
//					"4.800"
//				],
//				[
//					"7407.20",
//					"0.175"
//				]
//		]
//}
void __MAX::compose_ra002_from_binance(std::string& jsonData, char* out)
{
	nlohmann::json binance = nlohmann::json::parse(jsonData);
	std::string s;

	TRA002* pOut = (TRA002*)out;
	memset(out, 0x20, sizeof(TRA002));
	out[sizeof(TRA002)] = 0;


	// header
	memcpy(pOut->header.packet_cd, CD_HOGA, sizeof(pOut->header.packet_cd));

	s = binance["s"];
	memcpy(pOut->header.stk_cd, s.c_str(), std::min<size_t>(sizeof(pOut->header.stk_cd), s.size()));
	pOut->header.acnt_tp[0] = __MAX::ACNT_TP_CC;

	// body
	int64_t utcTime = binance["E"];
	char zKoreanTime[128] = { 0 };
	__UTILS::UnixTimestamp_to_KoreanTime(utcTime, zKoreanTime);	// yyyymmdd-hhmmss
	memcpy(pOut->tm, &zKoreanTime[9], 6);

	// ask
	int i = 1;
	char t[128];
	for (const auto& ask : binance["a"])
	{
		std::string price = ask[0];
		std::string vol = ask[1];

		int hogalen = sizeof(pOut->s_hoga_1);
		int vollen = sizeof(pOut->s_vol_1);
		if (i == 1) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->s_hoga_1, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->s_vol_1, t, vollen);

			pOut->s_cnt_1[sizeof(pOut->s_cnt_1) - 1] = '0';
		}
		else if (i == 2) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->s_hoga_2, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->s_vol_2, t, vollen);

			pOut->s_cnt_2[sizeof(pOut->s_cnt_2) - 1] = '0';
		}
		else if (i == 3) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->s_hoga_3, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->s_vol_3, t, vollen);

			pOut->s_cnt_3[sizeof(pOut->s_cnt_3) - 1] = '0';
		}
		else if (i == 4) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->s_hoga_4, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->s_vol_4, t, vollen);

			pOut->s_cnt_4[sizeof(pOut->s_cnt_4) - 1] = '0';
		}
		else if (i == 5) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->s_hoga_5, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->s_vol_5, t, vollen);

			pOut->s_cnt_5[sizeof(pOut->s_cnt_5) - 1] = '0';
		}
		i++;
	}
	
	i = 1;
	for (const auto& bid : binance["b"])
	{
		std::string price = bid[0];
		std::string vol = bid[1];

		int hogalen = sizeof(pOut->b_hoga_1);
		int vollen = sizeof(pOut->b_vol_1);

		if (i == 1) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->b_hoga_1, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->b_vol_1, t, vollen);

			pOut->b_cnt_1[sizeof(pOut->b_cnt_1) - 1] = '0';
		}
		else if (i == 2) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->b_hoga_2, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->b_vol_2, t, vollen);

			pOut->b_cnt_2[sizeof(pOut->b_cnt_2) - 1] = '0';
		}
		else if (i == 3) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->b_hoga_3, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->b_vol_3, t, vollen);

			pOut->b_cnt_3[sizeof(pOut->b_cnt_3) - 1] = '0';
		}
		else if (i == 4) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->b_hoga_4, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->b_vol_4, t, vollen);

			pOut->b_cnt_4[sizeof(pOut->b_cnt_4) - 1] = '0';
		}
		else if (i == 5) {
			sprintf(t, "%*s", hogalen, price.c_str());
			memcpy(pOut->b_hoga_5, t, hogalen);


			sprintf(t, "%*s", vollen, vol.c_str());
			memcpy(pOut->b_vol_5, t, vollen);

			pOut->b_cnt_5[sizeof(pOut->b_cnt_5) - 1] = '0';
		}
		i++;
	}

	pOut->Enter[0] = '\r';
	pOut->Enter[1] = '\n';
}

