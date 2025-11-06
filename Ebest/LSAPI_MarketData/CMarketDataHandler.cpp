#include "CIOCPServer.h"
#include "CMarketDataHandler.h"
#include "OF_Sise_OVC.h"
#include "OF_Hoga_OVH.h"
#include "KF_Sise_FC0.h"
#include "KF_Hoga_FH0.h"
#include "../../Common/MaxTradeInc.h"


void print_of_sise(OVC_OutBlock* api, __MAX::TRA001* ra001);
void print_of_hoga(OVH_OutBlock* api, __MAX::TRA002* ra002);

void print_kf_sise(FC0_OutBlock* api, __MAX::TRA001* ra001);
void print_kf_hoga(FH0_OutBlock* api, __MAX::TRA002* ra002);

CMarketDataHandler::~CMarketDataHandler()
{
	set_die();

	if( m_thrd_hoga.joinable() )	m_thrd_hoga.join();
	if (m_thrd_sise.joinable())		m_thrd_sise.join();
}

bool	CMarketDataHandler::Initialize()
{
	m_thrd_hoga = std::thread(&CMarketDataHandler::thread_hoga, this);
	m_thrd_sise = std::thread(&CMarketDataHandler::thread_sise, this);

	return true;
}

void	CMarketDataHandler::push_hoga(TDataUnit* p)
{ 
	m_queue_hoga.push(p);
}
void	CMarketDataHandler::push_sise(TDataUnit* p)
{ 
	m_queue_sise.push(p);
}


void	CMarketDataHandler::thread_sise()
{
	__common.debug("CMarketDataHandler::thread_sise starts...");

	while (m_is_continue.load())
	{
		_mm_pause();

		TDataUnit* p = m_queue_sise.wait_and_pop();
		if (!m_is_continue.load())
			break;

		if (p == nullptr) {
			__common.log(ERR, "[CMarketDataHandler::thread_sise] m_queue_sise returned nullptr");
			continue;
		}

		//__common.debug_fmt("[thread_sise]recv sise(%d)(%s)", p->data_size, p->data);

		string pack;
		#ifdef __DEF_OVERSEAS
		pack = compose_pack_sise_of(p);
		#else
		pack = compose_pack_sise_kf(p, *__common.symbols.begin());
		#endif

		__common.mempool->release<TDataUnit>(p);

		__iocpSvr.broadcast_all_clients(pack);
	}
}

void	CMarketDataHandler::thread_hoga()
{
	__common.debug("CMarketDataHandler::thread_hoga starts...");
	while (m_is_continue.load())
	{
		_mm_pause();

		TDataUnit* p = m_queue_hoga.wait_and_pop();
		if( !m_is_continue.load())
			break;

		if( p==nullptr){
			__common.log(ERR, "[CMarketDataHandler::thread_hoga] m_queue_hoga returned nullptr");
			continue;
		}
		
		//__common.debug_fmt("[thread_hoga]recv hoga(%d)(%s)", p->data_size, p->data);
			
		string pack;

#ifdef __DEF_OVERSEAS
		pack = compose_pack_hoga_of(p);
#else
		pack = compose_pack_hoga_kf(p, *__common.symbols.begin());
#endif

		__common.mempool->release<TDataUnit>(p);

		__iocpSvr.broadcast_all_clients(pack);
	}
}



string	CMarketDataHandler::compose_pack_sise_kf(TDataUnit* p, string symbol)
{
	int len_a, len_r;
	char t[128];
	double close, diff;

	FC0_OutBlock* api = (FC0_OutBlock*)p->data;

	ZeroMemory(m_buf_sise, _BUF_LEN);
	__MAX::TRA001* ra001 = (__MAX::TRA001*)m_buf_sise;
	memset(ra001, 0x20, sizeof(__MAX::TRA001));

	// header
	memcpy(ra001->header.packet_cd, __MAX::CD_SISE, sizeof(ra001->header.packet_cd));

	len_a = symbol.size(); len_r = sizeof(ra001->header.stk_cd);
	memcpy(ra001->header.stk_cd, symbol.c_str(), std::min<size_t>(len_a, len_r));

	ra001->header.acnt_tp[0] = '1';

	//// body
	//int64_t utcTime = binance["E"];
	//char zKoreanTime[128] = { 0 };
	//__UTILS::UnixTimestamp_to_KoreanTime(utcTime, zKoreanTime);	// yyyymmdd-hhmmss
	//memcpy(ra001->tm, &zKoreanTime[9], 6);

	len_a = sizeof(api->chetime); len_r = sizeof(ra001->tm);
	memcpy(ra001->tm, api->chetime, std::min<size_t>(len_a, len_r));

	len_a = sizeof(api->k200jisu); len_r = sizeof(ra001->kp200);	
	memcpy(ra001->kp200, api->k200jisu, std::min<size_t>(len_a, len_r));

	ra001->bsTp[0] = api->cgubun[0];	//체결구분

	// 체결가
	sprintf(t, "%.*s", sizeof(api->price), api->price);
	close = atof(t);
	sprintf(t, "%*.5f", sizeof(ra001->now_prc), close);	// [   1.12345]
	memcpy(ra001->now_prc, t, sizeof(ra001->now_prc));

	// 전일대비. 1,2 상승, 3 보합, 4,5 하락
	sprintf(t, "%.*s", sizeof(api->change), api->change);	
	diff = atof(t);
	diff = (api->sign[0] == '4' || api->sign[0] == '5') ? diff * -1. : diff * 1.;
	sprintf(t, "%*.2f", sizeof(ra001->chg), diff);	// [   1.12345]
	memcpy(ra001->chg, t, sizeof(ra001->chg));

	// 누적거래량
	sprintf(t, "%.*s", sizeof(api->volume), api->volume);
	sprintf(t, "%*d", sizeof(ra001->acml_cntr_vol), atoi(t));
	memcpy(ra001->acml_cntr_vol, t, sizeof(ra001->acml_cntr_vol));

	// 누적거래대금
	sprintf(t, "%.*s", sizeof(api->value), api->value);
	sprintf(t, "%*d", sizeof(ra001->acml_amt), atoi(t));
	memcpy(ra001->acml_amt, t, sizeof(ra001->acml_amt));

	// open
	sprintf(t, "%.*s", sizeof(api->open), api->open);
	sprintf(t, "%*.5f", sizeof(ra001->open), atof(t));	// [   1.12345]
	memcpy(ra001->open, t, sizeof(ra001->open));

	// high
	sprintf(t, "%.*s", sizeof(api->high), api->high);
	sprintf(t, "%*.5f", sizeof(ra001->high), atof(t));	// [   1.12345]
	memcpy(ra001->high, t, sizeof(ra001->high));

	// low
	sprintf(t, "%.*s", sizeof(api->low), api->low);
	sprintf(t, "%*.5f", sizeof(ra001->low), atof(t));	// [   1.12345]
	memcpy(ra001->low, t, sizeof(ra001->low));

	// 체결수량
	sprintf(t, "%.*s", sizeof(api->cvolume), api->cvolume);
	sprintf(t, "%*d", sizeof(ra001->cntr_vol), atoi(t));
	memcpy(ra001->cntr_vol, t, sizeof(ra001->cntr_vol));

	// 등락율 
	sprintf(t, "%.*s", sizeof(api->drate), api->drate);
	double rt = atof(t);
	sprintf(t, "%*.2f", sizeof(ra001->fluc_rt), rt);	// [   1.12345]
	memcpy(ra001->fluc_rt, t, sizeof(ra001->fluc_rt));

	sprintf(t, "%.*s", sizeof(api->sbasis), api->sbasis);
	sprintf(t, "%*.2f", sizeof(ra001->mkt_basis), atof(t));	
	memcpy(ra001->mkt_basis, t, sizeof(ra001->mkt_basis));

	sprintf(t, "%.*s", sizeof(api->openyakcha), api->openyakcha);
	sprintf(t, "%*d", sizeof(ra001->ncntr_fluc), atoi(t));
	memcpy(ra001->ncntr_fluc, t, sizeof(ra001->ncntr_fluc));

	sprintf(t, "%.*s", sizeof(api->openyak), api->openyak);
	sprintf(t, "%*d", sizeof(ra001->ncntr_vol), atoi(t));
	memcpy(ra001->ncntr_vol, t, sizeof(ra001->ncntr_vol));

	ra001->Enter[0] = '\r';
	ra001->Enter[1] = '\n';

	//print_kf_sise(api, ra001);

	return string(m_buf_sise);
}


string	CMarketDataHandler::compose_pack_sise_of(TDataUnit* p)
{
	int len_a, len_r;
	char t[128];
	double close, diff;

	OVC_OutBlock* api = (OVC_OutBlock*)p->data;

	ZeroMemory(m_buf_sise, _BUF_LEN);
	__MAX::TRA001* ra001 = (__MAX::TRA001*)m_buf_sise;
	memset(ra001, 0x20, sizeof(__MAX::TRA001));

	// header
	memcpy(ra001->header.packet_cd, __MAX::CD_SISE, sizeof(ra001->header.packet_cd));

	len_a = sizeof(api->symbol); len_r = sizeof(ra001->header.stk_cd);
	memcpy(ra001->header.stk_cd, api->symbol, std::min<size_t>(len_a, len_r));

	ra001->header.acnt_tp[0] = '2';

	//// body
	//int64_t utcTime = binance["E"];
	//char zKoreanTime[128] = { 0 };
	//__UTILS::UnixTimestamp_to_KoreanTime(utcTime, zKoreanTime);	// yyyymmdd-hhmmss
	//memcpy(ra001->tm, &zKoreanTime[9], 6);

	len_a = sizeof(api->kortm); len_r = sizeof(ra001->tm);
	memcpy(ra001->tm, api->kortm, std::min<size_t>(len_a, len_r));

	memset(ra001->kp200, 0x20, sizeof(ra001->kp200));
	ra001->kp200[sizeof(ra001->kp200) - 1] = '0';

	ra001->bsTp[0] = ' ';	//체결구분

	// 체결가
	sprintf(t, "%.*s", sizeof(api->curpr), api->curpr);
	close = atof(t);
	sprintf(t, "%*.5f", sizeof(ra001->now_prc), close);	// [   1.12345]
	memcpy(ra001->now_prc, t, sizeof(ra001->now_prc));

	// 전일대비
	sprintf(t, "%.*s", sizeof(api->ydiffpr), api->ydiffpr);
	diff = atof(t);
	diff = (api->ydiffSign[0]=='-' )? diff *-1. : diff *1.;
	sprintf(t, "%*.2f", sizeof(ra001->chg), diff);	// [   1.12345]
	memcpy(ra001->chg, t, sizeof(ra001->chg));

	// 누적거래량
	sprintf(t, "%.*s", sizeof(api->totq), api->totq);
	sprintf(t, "%*d", sizeof(ra001->acml_cntr_vol), atoi(t));	
	memcpy(ra001->acml_cntr_vol, t, sizeof(ra001->acml_cntr_vol));

	// 누적거래대금 : 없음
	

	// open
	sprintf(t, "%.*s", sizeof(api->open), api->open);
	sprintf(t, "%*.5f", sizeof(ra001->open), atof(t));	// [   1.12345]
	memcpy(ra001->open, t, sizeof(ra001->open));

	// high
	sprintf(t, "%.*s", sizeof(api->high), api->high);
	sprintf(t, "%*.5f", sizeof(ra001->high), atof(t));	// [   1.12345]
	memcpy(ra001->high, t, sizeof(ra001->high));

	// low
	sprintf(t, "%.*s", sizeof(api->low), api->low);
	sprintf(t, "%*.5f", sizeof(ra001->low), atof(t));	// [   1.12345]
	memcpy(ra001->low, t, sizeof(ra001->low));

	// 체결수량
	sprintf(t, "%.*s", sizeof(api->trdq), api->trdq);
	sprintf(t, "%*d", sizeof(ra001->cntr_vol), atoi(t));
	memcpy(ra001->cntr_vol, t, sizeof(ra001->cntr_vol));

	// 등락율 
	sprintf(t, "%.*s", sizeof(api->chgrate), api->chgrate);
	double rt = atof(t);
	sprintf(t, "%*.2f", sizeof(ra001->fluc_rt), rt);	// [   1.12345]
	memcpy(ra001->fluc_rt, t, sizeof(ra001->fluc_rt));

	ra001->Enter[0] = '\r';
	ra001->Enter[1] = '\n';

	print_of_sise(api, ra001);

	return string(m_buf_sise);	
}



string	CMarketDataHandler::compose_pack_hoga_kf(TDataUnit* p, string symbol)
{
	int len_a, len_r;
	char t[128];

	FH0_OutBlock* api = (FH0_OutBlock*)p->data;

	ZeroMemory(m_buf_sise, _BUF_LEN);
	__MAX::TRA002* ra002 = (__MAX::TRA002*)m_buf_hoga;
	memset(ra002, 0x20, sizeof(__MAX::TRA002));

	// header
	memcpy(ra002->header.packet_cd, __MAX::CD_HOGA, sizeof(ra002->header.packet_cd));

	len_a = symbol.size();	len_r = sizeof(ra002->header.stk_cd);
	memcpy(ra002->header.stk_cd, symbol.c_str(), std::min<size_t>(len_a, len_r));

	ra002->header.acnt_tp[0] = '1';

	//// body
	
	len_a = sizeof(api->hotime); len_r = sizeof(ra002->tm);
	memcpy(ra002->tm, api->hotime, std::min<size_t>(len_a, len_r));

	// 매도수량 1
	sprintf(t, "%.*s", sizeof(api->offerrem1), api->offerrem1);
	sprintf(t, "%*d", sizeof(ra002->s_vol_1), atoi(t));
	memcpy(ra002->s_vol_1, t, sizeof(ra002->s_vol_1));

	// 매도수량 2
	sprintf(t, "%.*s", sizeof(api->offerrem2), api->offerrem2);
	sprintf(t, "%*d", sizeof(ra002->s_vol_2), atoi(t));
	memcpy(ra002->s_vol_2, t, sizeof(ra002->s_vol_2));

	// 매도수량 3
	sprintf(t, "%.*s", sizeof(api->offerrem3), api->offerrem3);
	sprintf(t, "%*d", sizeof(ra002->s_vol_3), atoi(t));
	memcpy(ra002->s_vol_3, t, sizeof(ra002->s_vol_3));

	// 매도수량 4
	sprintf(t, "%.*s", sizeof(api->offerrem4), api->offerrem4);
	sprintf(t, "%*d", sizeof(ra002->s_vol_4), atoi(t));
	memcpy(ra002->s_vol_4, t, sizeof(ra002->s_vol_4));

	// 매도수량 5
	sprintf(t, "%.*s", sizeof(api->offerrem5), api->offerrem5);
	sprintf(t, "%*d", sizeof(ra002->s_vol_1), atoi(t));
	memcpy(ra002->s_vol_5, t, sizeof(ra002->s_vol_5));

	//============================================================//

	// 매도호가 1
	sprintf(t, "%.*s", sizeof(api->offerho1), api->offerho1);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_1), atof(t));
	memcpy(ra002->s_hoga_1, t, sizeof(ra002->s_hoga_1));

	// 매도호가 2
	sprintf(t, "%.*s", sizeof(api->offerho2), api->offerho2);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_2), atof(t));
	memcpy(ra002->s_hoga_2, t, sizeof(ra002->s_hoga_2));

	// 매도호가 3
	sprintf(t, "%.*s", sizeof(api->offerho3), api->offerho3);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_3), atof(t));
	memcpy(ra002->s_hoga_3, t, sizeof(ra002->s_hoga_3));

	// 매도호가 4
	sprintf(t, "%.*s", sizeof(api->offerho4), api->offerho4);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_4), atof(t));
	memcpy(ra002->s_hoga_4, t, sizeof(ra002->s_hoga_4));

	// 매도호가 5
	sprintf(t, "%.*s", sizeof(api->offerho5), api->offerho5);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_5), atof(t));
	memcpy(ra002->s_hoga_5, t, sizeof(ra002->s_hoga_5));


	//============================================================//

	// 매도건수 1
	sprintf(t, "%.*s", sizeof(api->offercnt1), api->offercnt1);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_1), atoi(t));
	memcpy(ra002->s_cnt_1, t, sizeof(ra002->s_cnt_1));

	// 매도건수 2
	sprintf(t, "%.*s", sizeof(api->offercnt2), api->offercnt2);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_2), atoi(t));
	memcpy(ra002->s_cnt_2, t, sizeof(ra002->s_cnt_2));

	// 매도건수 3
	sprintf(t, "%.*s", sizeof(api->offercnt3), api->offercnt3);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_3), atoi(t));
	memcpy(ra002->s_cnt_3, t, sizeof(ra002->s_cnt_3));

	// 매도건수 4
	sprintf(t, "%.*s", sizeof(api->offercnt4), api->offercnt4);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_4), atoi(t));
	memcpy(ra002->s_cnt_4, t, sizeof(ra002->s_cnt_4));

	// 매도건수 5
	sprintf(t, "%.*s", sizeof(api->offercnt5), api->offercnt5);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_5), atoi(t));
	memcpy(ra002->s_cnt_5, t, sizeof(ra002->s_cnt_5));



	//*********************************************************************************//

	// 매수수량 1
	sprintf(t, "%.*s", sizeof(api->bidrem1), api->bidrem1);
	sprintf(t, "%*d", sizeof(ra002->b_vol_1), atoi(t));
	memcpy(ra002->b_vol_1, t, sizeof(ra002->b_vol_1));

	// 매수수량 2
	sprintf(t, "%.*s", sizeof(api->bidrem2), api->bidrem2);
	sprintf(t, "%*d", sizeof(ra002->b_vol_2), atoi(t));
	memcpy(ra002->b_vol_2, t, sizeof(ra002->b_vol_2));

	// 매수수량 3
	sprintf(t, "%.*s", sizeof(api->bidrem3), api->bidrem3);
	sprintf(t, "%*d", sizeof(ra002->b_vol_3), atoi(t));
	memcpy(ra002->b_vol_3, t, sizeof(ra002->b_vol_3));

	// 매수수량 4
	sprintf(t, "%.*s", sizeof(api->bidrem4), api->bidrem4);
	sprintf(t, "%*d", sizeof(ra002->b_vol_4), atoi(t));
	memcpy(ra002->b_vol_4, t, sizeof(ra002->b_vol_4));

	// 매수수량 5
	sprintf(t, "%.*s", sizeof(api->bidrem5), api->bidrem5);
	sprintf(t, "%*d", sizeof(ra002->b_vol_5), atoi(t));
	memcpy(ra002->b_vol_5, t, sizeof(ra002->b_vol_5));

	//============================================================//

	// 매수호가 1
	sprintf(t, "%.*s", sizeof(api->bidho1), api->bidho1);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_1), atof(t));
	memcpy(ra002->b_hoga_1, t, sizeof(ra002->b_hoga_1));

	// 매수호가 2
	sprintf(t, "%.*s", sizeof(api->bidho2), api->bidho2);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_2), atof(t));
	memcpy(ra002->b_hoga_2, t, sizeof(ra002->b_hoga_2));

	// 매수호가 3
	sprintf(t, "%.*s", sizeof(api->bidho3), api->bidho3);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_3), atof(t));
	memcpy(ra002->b_hoga_3, t, sizeof(ra002->b_hoga_3));

	// 매수호가 4
	sprintf(t, "%.*s", sizeof(api->bidho4), api->bidho4);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_4), atof(t));
	memcpy(ra002->b_hoga_4, t, sizeof(ra002->b_hoga_4));

	// 매수호가 5
	sprintf(t, "%.*s", sizeof(api->bidho5), api->bidho5);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_5), atof(t));
	memcpy(ra002->b_hoga_5, t, sizeof(ra002->b_hoga_5));


	//============================================================//

	// 매수건수 1
	sprintf(t, "%.*s", sizeof(api->bidcnt1), api->bidcnt1);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_1), atoi(t));
	memcpy(ra002->b_cnt_1, t, sizeof(ra002->b_cnt_1));

	// 매수건수 2
	sprintf(t, "%.*s", sizeof(api->bidcnt2), api->bidcnt2);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_2), atoi(t));
	memcpy(ra002->b_cnt_2, t, sizeof(ra002->b_cnt_2));

	// 매수건수 3
	sprintf(t, "%.*s", sizeof(api->bidcnt3), api->bidcnt3);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_3), atoi(t));
	memcpy(ra002->b_cnt_3, t, sizeof(ra002->b_cnt_3));

	// 매수건수 4
	sprintf(t, "%.*s", sizeof(api->bidcnt4), api->bidcnt4);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_4), atoi(t));
	memcpy(ra002->b_cnt_4, t, sizeof(ra002->b_cnt_4));

	// 매수건수 5
	sprintf(t, "%.*s", sizeof(api->bidcnt5), api->bidcnt5);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_5), atoi(t));
	memcpy(ra002->b_cnt_5, t, sizeof(ra002->b_cnt_5));

	//********************************************************************//

	sprintf(t, "%.*s", sizeof(api->totofferrem), api->totofferrem);
	int s_vol = atoi(t);
	sprintf(t, "%*d", sizeof(ra002->s_tot_vol), s_vol);
	memcpy(ra002->s_tot_vol, t, sizeof(ra002->s_tot_vol));

	sprintf(t, "%.*s", sizeof(api->totoffercnt), api->totoffercnt);
	sprintf(t, "%*d", sizeof(ra002->s_tot_cnt), atoi(t));
	memcpy(ra002->s_tot_cnt, t, sizeof(ra002->s_tot_cnt));

	sprintf(t, "%.*s", sizeof(api->totbidrem), api->totbidrem);
	int b_vol = atoi(t);
	sprintf(t, "%*d", sizeof(ra002->b_tot_vol), b_vol);
	memcpy(ra002->b_tot_vol, t, sizeof(ra002->b_tot_vol));

	sprintf(t, "%.*s", sizeof(api->totbidcnt), api->totbidcnt);
	sprintf(t, "%*d", sizeof(ra002->b_tot_cnt), atoi(t));
	memcpy(ra002->b_tot_cnt, t, sizeof(ra002->b_tot_cnt));

	sprintf(t, "%*d", sizeof(ra002->exce_vol), (b_vol - s_vol));
	memcpy(ra002->exce_vol, t, sizeof(ra002->exce_vol));

	ra002->Enter[0] = '\r';
	ra002->Enter[1] = '\n';

	//print_kf_hoga(api, ra002);

	return string(m_buf_hoga);
}


string	CMarketDataHandler::compose_pack_hoga_of(TDataUnit* p)
{
	__common.debug("CMarketDataHandler::compose_pack_hoga is called");
	

	int len_a, len_r;
	char t[128];

	OVH_OutBlock* api = (OVH_OutBlock*)p->data;

	ZeroMemory(m_buf_sise, _BUF_LEN);
	__MAX::TRA002* ra002 = (__MAX::TRA002*)m_buf_hoga;
	memset(ra002, 0x20, sizeof(__MAX::TRA002));

	// header
	memcpy(ra002->header.packet_cd, __MAX::CD_HOGA, sizeof(ra002->header.packet_cd));

	len_a = sizeof(api->symbol); len_r = sizeof(ra002->header.stk_cd);
	memcpy(ra002->header.stk_cd, api->symbol, std::min<size_t>(len_a, len_r));

	ra002->header.acnt_tp[0] = '2';

	//// body
	//int64_t utcTime = binance["E"];
	//char zKoreanTime[128] = { 0 };
	//__UTILS::UnixTimestamp_to_KoreanTime(utcTime, zKoreanTime);	// yyyymmdd-hhmmss
	//memcpy(ra002->tm, &zKoreanTime[9], 6);

	len_a = sizeof(api->hotime); len_r = sizeof(ra002->tm);
	memcpy(ra002->tm, api->hotime, std::min<size_t>(len_a, len_r));

	// 매도수량 1
	sprintf(t, "%.*s", sizeof(api->offerrem1), api->offerrem1);
	sprintf(t, "%*d", sizeof(ra002->s_vol_1), atoi(t));	
	memcpy(ra002->s_vol_1, t, sizeof(ra002->s_vol_1));

	// 매도수량 2
	sprintf(t, "%.*s", sizeof(api->offerrem2), api->offerrem2);
	sprintf(t, "%*d", sizeof(ra002->s_vol_2), atoi(t));
	memcpy(ra002->s_vol_2, t, sizeof(ra002->s_vol_2));

	// 매도수량 3
	sprintf(t, "%.*s", sizeof(api->offerrem3), api->offerrem3);
	sprintf(t, "%*d", sizeof(ra002->s_vol_3), atoi(t));
	memcpy(ra002->s_vol_3, t, sizeof(ra002->s_vol_3));

	// 매도수량 4
	sprintf(t, "%.*s", sizeof(api->offerrem4), api->offerrem4);
	sprintf(t, "%*d", sizeof(ra002->s_vol_4), atoi(t));
	memcpy(ra002->s_vol_4, t, sizeof(ra002->s_vol_4));

	// 매도수량 5
	sprintf(t, "%.*s", sizeof(api->offerrem5), api->offerrem5);
	sprintf(t, "%*d", sizeof(ra002->s_vol_1), atoi(t));
	memcpy(ra002->s_vol_5, t, sizeof(ra002->s_vol_5));

	//============================================================//

	// 매도호가 1
	sprintf(t, "%.*s", sizeof(api->offerho1), api->offerho1);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_1), atof(t));	
	memcpy(ra002->s_hoga_1, t, sizeof(ra002->s_hoga_1));

	// 매도호가 2
	sprintf(t, "%.*s", sizeof(api->offerho2), api->offerho2);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_2), atof(t));
	memcpy(ra002->s_hoga_2, t, sizeof(ra002->s_hoga_2));

	// 매도호가 3
	sprintf(t, "%.*s", sizeof(api->offerho3), api->offerho3);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_3), atof(t));
	memcpy(ra002->s_hoga_3, t, sizeof(ra002->s_hoga_3));

	// 매도호가 4
	sprintf(t, "%.*s", sizeof(api->offerho4), api->offerho4);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_4), atof(t));
	memcpy(ra002->s_hoga_4, t, sizeof(ra002->s_hoga_4));

	// 매도호가 5
	sprintf(t, "%.*s", sizeof(api->offerho5), api->offerho5);
	sprintf(t, "%*.5f", sizeof(ra002->s_hoga_5), atof(t));
	memcpy(ra002->s_hoga_5, t, sizeof(ra002->s_hoga_5));


	//============================================================//
	 
	// 매도건수 1
	sprintf(t, "%.*s", sizeof(api->offerno1), api->offerno1);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_1), atoi(t));
	memcpy(ra002->s_cnt_1, t, sizeof(ra002->s_cnt_1));

	// 매도건수 2
	sprintf(t, "%.*s", sizeof(api->offerno2), api->offerno2);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_2), atoi(t));
	memcpy(ra002->s_cnt_2, t, sizeof(ra002->s_cnt_2));

	// 매도건수 3
	sprintf(t, "%.*s", sizeof(api->offerno3), api->offerno3);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_3), atoi(t));
	memcpy(ra002->s_cnt_3, t, sizeof(ra002->s_cnt_3));

	// 매도건수 4
	sprintf(t, "%.*s", sizeof(api->offerno4), api->offerno4);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_4), atoi(t));
	memcpy(ra002->s_cnt_4, t, sizeof(ra002->s_cnt_4));

	// 매도건수 5
	sprintf(t, "%.*s", sizeof(api->offerno5), api->offerno5);
	sprintf(t, "%*d", sizeof(ra002->s_cnt_5), atoi(t));
	memcpy(ra002->s_cnt_5, t, sizeof(ra002->s_cnt_5));



	//*********************************************************************************//

	// 매수수량 1
	sprintf(t, "%.*s", sizeof(api->bidrem1), api->bidrem1);
	sprintf(t, "%*d", sizeof(ra002->b_vol_1), atoi(t));
	memcpy(ra002->b_vol_1, t, sizeof(ra002->b_vol_1));

	// 매수수량 2
	sprintf(t, "%.*s", sizeof(api->bidrem2), api->bidrem2);
	sprintf(t, "%*d", sizeof(ra002->b_vol_2), atoi(t));
	memcpy(ra002->b_vol_2, t, sizeof(ra002->b_vol_2));

	// 매수수량 3
	sprintf(t, "%.*s", sizeof(api->bidrem3), api->bidrem3);
	sprintf(t, "%*d", sizeof(ra002->b_vol_3), atoi(t));
	memcpy(ra002->b_vol_3, t, sizeof(ra002->b_vol_3));

	// 매수수량 4
	sprintf(t, "%.*s", sizeof(api->bidrem4), api->bidrem4);
	sprintf(t, "%*d", sizeof(ra002->b_vol_4), atoi(t));
	memcpy(ra002->b_vol_4, t, sizeof(ra002->b_vol_4));

	// 매수수량 5
	sprintf(t, "%.*s", sizeof(api->bidrem5), api->bidrem5);
	sprintf(t, "%*d", sizeof(ra002->b_vol_5), atoi(t));
	memcpy(ra002->b_vol_5, t, sizeof(ra002->b_vol_5));

	//============================================================//

	// 매수호가 1
	sprintf(t, "%.*s", sizeof(api->bidho1), api->bidho1);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_1), atof(t));
	memcpy(ra002->b_hoga_1, t, sizeof(ra002->b_hoga_1));

	// 매수호가 2
	sprintf(t, "%.*s", sizeof(api->bidho2), api->bidho2);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_2), atof(t));
	memcpy(ra002->b_hoga_2, t, sizeof(ra002->b_hoga_2));

	// 매수호가 3
	sprintf(t, "%.*s", sizeof(api->bidho3), api->bidho3);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_3), atof(t));
	memcpy(ra002->b_hoga_3, t, sizeof(ra002->b_hoga_3));

	// 매수호가 4
	sprintf(t, "%.*s", sizeof(api->bidho4), api->bidho4);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_4), atof(t));
	memcpy(ra002->b_hoga_4, t, sizeof(ra002->b_hoga_4));

	// 매수호가 5
	sprintf(t, "%.*s", sizeof(api->bidho5), api->bidho5);
	sprintf(t, "%*.5f", sizeof(ra002->b_hoga_5), atof(t));
	memcpy(ra002->b_hoga_5, t, sizeof(ra002->b_hoga_5));


	//============================================================//

	// 매수건수 1
	sprintf(t, "%.*s", sizeof(api->bidno1), api->bidno1);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_1), atoi(t));
	memcpy(ra002->b_cnt_1, t, sizeof(ra002->b_cnt_1));

	// 매수건수 2
	sprintf(t, "%.*s", sizeof(api->bidno2), api->bidno2);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_2), atoi(t));
	memcpy(ra002->b_cnt_2, t, sizeof(ra002->b_cnt_2));

	// 매수건수 3
	sprintf(t, "%.*s", sizeof(api->bidno3), api->bidno3);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_3), atoi(t));
	memcpy(ra002->b_cnt_3, t, sizeof(ra002->b_cnt_3));

	// 매수건수 4
	sprintf(t, "%.*s", sizeof(api->bidno4), api->bidno4);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_4), atoi(t));
	memcpy(ra002->b_cnt_4, t, sizeof(ra002->b_cnt_4));

	// 매수건수 5
	sprintf(t, "%.*s", sizeof(api->bidno5), api->bidno5);
	sprintf(t, "%*d", sizeof(ra002->b_cnt_5), atoi(t));
	memcpy(ra002->b_cnt_5, t, sizeof(ra002->b_cnt_5));

	//********************************************************************//

	sprintf(t, "%.*s", sizeof(api->totofferrem), api->totofferrem);
	int s_vol = atoi(t);
	sprintf(t, "%*d", sizeof(ra002->s_tot_vol), s_vol);
	memcpy(ra002->s_tot_vol, t, sizeof(ra002->s_tot_vol));

	sprintf(t, "%.*s", sizeof(api->totoffercnt), api->totoffercnt);
	sprintf(t, "%*d", sizeof(ra002->s_tot_cnt), atoi(t));
	memcpy(ra002->s_tot_cnt, t, sizeof(ra002->s_tot_cnt));

	sprintf(t, "%.*s", sizeof(api->totbidrem), api->totbidrem);
	int b_vol = atoi(t);
	sprintf(t, "%*d", sizeof(ra002->b_tot_vol), b_vol);
	memcpy(ra002->b_tot_vol, t, sizeof(ra002->b_tot_vol));

	sprintf(t, "%.*s", sizeof(api->totbidcnt), api->totbidcnt);
	sprintf(t, "%*d", sizeof(ra002->b_tot_cnt), atoi(t));
	memcpy(ra002->b_tot_cnt, t, sizeof(ra002->b_tot_cnt));

	sprintf(t, "%*d", sizeof(ra002->exce_vol), (b_vol-s_vol));
	memcpy(ra002->exce_vol, t, sizeof(ra002->exce_vol));

	ra002->Enter[0] = '\r';
	ra002->Enter[1] = '\n';

	print_of_hoga(api, ra002);

	return string(m_buf_hoga);
}


void print_kf_sise(FC0_OutBlock* api, __MAX::TRA001* ra001)
{
	__common.debug_fmt(
	"[KF_SISE_API]"
	"(chetime:%.*s)"
	"(sign:%.*s)"
	"(change:%.*s)"
	"(drate:%.*s)"
	"(price:%.*s)"
	"(open:%.*s)"
	"(high:%.*s)"
	"(low:%.*s)"
	"(cgubun:%.*s)"
	"(cvolume:%.*s)"
	"(volume:%.*s)"
	"(value:%.*s)"
	"(mdvolume:%.*s)"
	"(mdchecnt:%.*s)"
	"(msvolume:%.*s)"
	"(mschecnt:%.*s)"
	"(cpower:%.*s)"
	"(offerho1:%.*s)"
	"(bidho1:%.*s)"
	"(openyak:%.*s)"
	"(k200jisu:%.*s)"
	"(theoryprice:%.*s)"
	"(kasis:%.*s)"
	"(sbasis:%.*s)"
	"(ibasis:%.*s)"
	"(openyakcha:%.*s)"
	"(jgubun:%.*s)"
	"(jnilvolume:%.*s)"
	"(futcode:%.*s)"
		
	,sizeof(api->chetime), api->chetime
	,sizeof(api->sign), api->sign
	,sizeof(api->change), api->change
	,sizeof(api->drate), api->drate
	,sizeof(api->price), api->price
	,sizeof(api->open), api->open
	,sizeof(api->high), api->high
	,sizeof(api->low), api->low
	,sizeof(api->cgubun), api->cgubun
	,sizeof(api->cvolume), api->cvolume
	,sizeof(api->volume), api->volume
	,sizeof(api->value), api->value
	,sizeof(api->mdvolume), api->mdvolume
	,sizeof(api->mdchecnt), api->mdchecnt
	,sizeof(api->msvolume), api->msvolume
	,sizeof(api->mschecnt), api->mschecnt
	,sizeof(api->cpower), api->cpower
	,sizeof(api->offerho1), api->offerho1
	,sizeof(api->bidho1), api->bidho1
	,sizeof(api->openyak), api->openyak
	,sizeof(api->k200jisu), api->k200jisu
	,sizeof(api->theoryprice), api->theoryprice
	,sizeof(api->kasis), api->kasis
	,sizeof(api->sbasis), api->sbasis
	,sizeof(api->ibasis), api->ibasis
	,sizeof(api->openyakcha), api->openyakcha
	,sizeof(api->jgubun), api->jgubun
	,sizeof(api->jnilvolume), api->jnilvolume
	,sizeof(api->futcode), api->futcode
	);


	__common.debug_fmt(
		"<KF_SISE_RA>"
		"(packet_cd:%.*s)"
		"(stk_cd:%.*s)"
		"(acnt_tp:%.*s)"
		"(tm:%.*s)"
		"(kp200:%.*s)"
		"(bsTp:%.*s)"
		"(now_prc:%.*s)"
		"(chg:%.*s)"
		"(acml_cntr_vol:%.*s)"
		"(acml_amt:%.*s)"
		"(open:%.*s)"
		"(high:%.*s)"
		"(low:%.*s)"
		"(cntr_vol:%.*s)"
		"(fluc_rt:%.*s)"
		"(mkt_basis:%.*s)"
		"(ncntr_fluc:%.*s)"
		"(ncntr_vol:%.*s)"

		,sizeof(ra001->header.packet_cd), ra001->header.packet_cd
		,sizeof(ra001->header.stk_cd), ra001->header.stk_cd
		,sizeof(ra001->header.acnt_tp), ra001->header.acnt_tp
		,sizeof(ra001->tm), ra001->tm
		,sizeof(ra001->kp200), ra001->kp200
		,sizeof(ra001->bsTp), ra001->bsTp
		,sizeof(ra001->now_prc), ra001->now_prc
		,sizeof(ra001->chg), ra001->chg
		,sizeof(ra001->acml_cntr_vol), ra001->acml_cntr_vol
		,sizeof(ra001->acml_amt), ra001->acml_amt
		,sizeof(ra001->open), ra001->open
		,sizeof(ra001->high), ra001->high
		,sizeof(ra001->low), ra001->low
		,sizeof(ra001->cntr_vol), ra001->cntr_vol
		,sizeof(ra001->fluc_rt), ra001->fluc_rt
		,sizeof(ra001->mkt_basis), ra001->mkt_basis
		,sizeof(ra001->ncntr_fluc), ra001->ncntr_fluc
		,sizeof(ra001->ncntr_vol), ra001->ncntr_vol
		);
}


void print_kf_hoga(FH0_OutBlock* api, __MAX::TRA002* ra002)
{
	__common.debug_fmt("[HOGA_API](hotime:%.*s)"
		"(offerho1:%.*s)(bidho1:%.*s)(offerrem1:%.*s)(bidrem1:%.*s)(offercnt1:%.*s)(bidcnt1:%.*s)"
		"(offerho2:%.*s)(bidho2:%.*s)(offerrem2:%.*s)(bidrem2:%.*s)(offercnt2:%.*s)(bidcnt2:%.*s)"
		"(offerho3:%.*s)(bidho3:%.*s)(offerrem3:%.*s)(bidrem3:%.*s)(offercnt3:%.*s)(bidcnt3:%.*s)"
		"(offerho4:%.*s)(bidho4:%.*s)(offerrem4:%.*s)(bidrem4:%.*s)(offercnt4:%.*s)(bidcnt4:%.*s)"
		"(offerho5:%.*s)(bidho5:%.*s)(offerrem5:%.*s)(bidrem5:%.*s)(offercnt5:%.*s)(bidcnt5:%.*s)"
		"(totofferrem:%.*s)(totbidrem:%.*s)(totoffercnt:%.*s)(totbidcnt:%.*s)"
		"(futcode:%.*s)(danhochk:%.*s)(alloc_gubun:%.*s)"

		,sizeof(api->hotime), api->hotime
		,sizeof(api->offerho1), api->offerho1
		,sizeof(api->bidho1), api->bidho1
		,sizeof(api->offerrem1), api->offerrem1
		,sizeof(api->bidrem1), api->bidrem1
		,sizeof(api->offercnt1), api->offercnt1
		,sizeof(api->bidcnt1), api->bidcnt1
		,sizeof(api->offerho2), api->offerho2
		,sizeof(api->bidho2), api->bidho2
		,sizeof(api->offerrem2), api->offerrem2
		,sizeof(api->bidrem2), api->bidrem2
		,sizeof(api->offercnt2), api->offercnt2
		,sizeof(api->bidcnt2), api->bidcnt2
		,sizeof(api->offerho3), api->offerho3
		,sizeof(api->bidho3), api->bidho3
		,sizeof(api->offerrem3), api->bidho3
		,sizeof(api->bidrem3), api->bidrem3
		,sizeof(api->offercnt3), api->offercnt3
		,sizeof(api->bidcnt3), api->bidcnt3
		,sizeof(api->offerho4), api->offerho4
		,sizeof(api->bidho4), api->bidho4
		,sizeof(api->offerrem4), api->offerrem4
		,sizeof(api->bidrem4), api->bidrem4
		,sizeof(api->offercnt4), api->offercnt4
		,sizeof(api->bidcnt4), api->bidcnt4
		,sizeof(api->offerho5), api->offerho5
		,sizeof(api->bidho5), api->bidho5
		,sizeof(api->offerrem5), api->offerrem5
		,sizeof(api->bidrem5), api->bidrem5
		,sizeof(api->offercnt5), api->offercnt5
		,sizeof(api->bidcnt5), api->bidcnt5
		,sizeof(api->totofferrem), api->totofferrem
		,sizeof(api->totbidrem), api->totbidrem
		,sizeof(api->totoffercnt), api->totoffercnt
		,sizeof(api->totbidcnt), api->totbidcnt
		,sizeof(api->futcode), api->futcode
		,sizeof(api->danhochk), api->danhochk
		,sizeof(api->alloc_gubun), api->alloc_gubun
		);

	__common.debug_fmt("<HOGA_RA>(packet_cd:%.*s)(stk_cd:%.*s)(acnt_tp:%.*s)(tm:%.*s)"
		"(s_vol_1:%.*s)(s_vol_2:%.*s)(s_vol_3:%.*s)(s_vol_4:%.*s)(s_vol_5:%.*s)"
		"(s_hoga_1:%.*s)(s_hoga_2:%.*s)(s_hoga_3:%.*s)(s_hoga_4:%.*s)(s_hoga_5:%.*s)"
		"(s_cnt_1:%.*s)(s_cnt_2:%.*s)(s_cnt_3:%.*s)(s_cnt_4:%.*s)(s_cnt_5:%.*s)"
		"(b_vol_1:%.*s)(b_vol_2:%.*s)(b_vol_3:%.*s)(b_vol_4:%.*s)(b_vol_5:%.*s)"
		"(b_hoga_1:%.*s)(b_hoga_2:%.*s)(b_hoga_3:%.*s)(b_hoga_4:%.*s)(b_hoga_5:%.*s)"
		"(b_cnt_1:%.*s)(b_cnt_2:%.*s)(b_cnt_3:%.*s)(b_cnt_4:%.*s)(b_cnt_5:%.*s)"		
		"(s_tot_vol:%.*s)(s_tot_cnt:%.*s)(b_tot_vol:%.*s)(b_tot_cnt:%.*s)(exce_vol:%.*s)"

		, sizeof(ra002->header.packet_cd), ra002->header.packet_cd
		, sizeof(ra002->header.stk_cd), ra002->header.stk_cd
		, sizeof(ra002->header.acnt_tp), ra002->header.acnt_tp
		, sizeof(ra002->tm), ra002->tm

		,sizeof(ra002->s_vol_1), ra002->s_vol_1
		,sizeof(ra002->s_vol_2), ra002->s_vol_2
		,sizeof(ra002->s_vol_3), ra002->s_vol_3
		,sizeof(ra002->s_vol_4), ra002->s_vol_4
		,sizeof(ra002->s_vol_5), ra002->s_vol_5
		,sizeof(ra002->s_hoga_1), ra002->s_hoga_1
		,sizeof(ra002->s_hoga_2), ra002->s_hoga_2
		,sizeof(ra002->s_hoga_3), ra002->s_hoga_3
		,sizeof(ra002->s_hoga_4), ra002->s_hoga_4
		,sizeof(ra002->s_hoga_5), ra002->s_hoga_5
		,sizeof(ra002->s_cnt_1), ra002->s_cnt_1
		,sizeof(ra002->s_cnt_2), ra002->s_cnt_2
		,sizeof(ra002->s_cnt_3), ra002->s_cnt_3
		,sizeof(ra002->s_cnt_4), ra002->s_cnt_4
		,sizeof(ra002->s_cnt_5), ra002->s_cnt_5
		
		, sizeof(ra002->b_vol_1), ra002->b_vol_1
		, sizeof(ra002->b_vol_2), ra002->b_vol_2
		, sizeof(ra002->b_vol_3), ra002->b_vol_3
		, sizeof(ra002->b_vol_4), ra002->b_vol_4
		, sizeof(ra002->b_vol_5), ra002->b_vol_5
		, sizeof(ra002->b_hoga_1), ra002->b_hoga_1
		, sizeof(ra002->b_hoga_2), ra002->b_hoga_2
		, sizeof(ra002->b_hoga_3), ra002->b_hoga_3
		, sizeof(ra002->b_hoga_4), ra002->b_hoga_4
		, sizeof(ra002->b_hoga_5), ra002->b_hoga_5
		, sizeof(ra002->b_cnt_1), ra002->b_cnt_1
		, sizeof(ra002->b_cnt_2), ra002->b_cnt_2
		, sizeof(ra002->b_cnt_3), ra002->b_cnt_3
		, sizeof(ra002->b_cnt_4), ra002->b_cnt_4
		, sizeof(ra002->b_cnt_5), ra002->b_cnt_5

		,sizeof(ra002->s_tot_vol), ra002->s_tot_vol
		,sizeof(ra002->s_tot_cnt), ra002->s_tot_cnt
		,sizeof(ra002->b_tot_vol), ra002->b_tot_vol
		,sizeof(ra002->b_tot_cnt), ra002->b_tot_cnt
		, sizeof(ra002->exce_vol), ra002->exce_vol
		);
}



void print_of_sise(OVC_OutBlock* api, __MAX::TRA001* ra001)
{ 
	__common.debug_fmt(
		"[OF_SISE_RA]"
		"(symbol:%.*s)"
		"(ovsdate:%.*s)"
		"(kordate:%.*s)"
		"(trdtm:%.*s)"
		"(kortm:%.*s)"
		"(curpr:%.*s)"
		"(ydiffpr:%.*s)"
		"(ydiffSign:%.*s)"
		"(open:%.*s)"
		"(high:%.*s)"
		"(low:%.*s)"
		"(chgrate:%.*s)"
		"(trdq:%.*s)"
		"(totq:%.*s)"
		"(cgubun:%.*s)"
		"(mdvolume:%.*s)"
		"(msvolume:%.*s)"
		"(ovsmkend:%.*s)"

		,sizeof(api->symbol,api->symbol) //8];    char    _symbol;    // ,api-> ) //string,    8] 종목코드                        StartPos 0, Length 8
		,sizeof(api->ovsdate,api->ovsdate) //8];    char    _ovsdate;    // ,api-> ) //string,    8] 체결일자(현지)                  StartPos 9, Length 8
		,sizeof(api->kordate,api->kordate) //8];    char    _kordate;    // ,api-> ) //string,    8] 체결일자(한국)                  StartPos 18, Length 8
		,sizeof(api->trdtm,api->trdtm) //6];    char    _trdtm;    // ,api-> ) //string,    6] 체결시간(현지)                  StartPos 27, Length 6
		,sizeof(api->kortm,api->kortm) //6];    char    _kortm;    // ,api-> ) //string,    6] 체결시간(한국)                  StartPos 34, Length 6
		,sizeof(api->curpr,api->curpr) //15];    char    _curpr;    // ,api-> ) //double, 15.9] 체결가격                        StartPos 41, Length 15
		,sizeof(api->ydiffpr,api->ydiffpr) //15];    char    _ydiffpr;    // ,api-> ) //double, 15.9] 전일대비                        StartPos 57, Length 15
		,sizeof(api->ydiffSign,api->ydiffSign) //1];    char    _ydiffSign;    // ,api-> ) //string,    1] 전일대비기호                    StartPos 73, Length 1
		,sizeof(api->open,api->open) //15];    char    _open;    // ,api-> ) //double, 15.9] 시가                            StartPos 75, Length 15
		,sizeof(api->high,api->high) //15];    char    _high;    // ,api-> ) //double, 15.9] 고가                            StartPos 91, Length 15
		,sizeof(api->low,api->low) //15];    char    _low;    // ,api-> ) //double, 15.9] 저가                            StartPos 107, Length 15
		,sizeof(api->chgrate,api->chgrate) //6];    char    _chgrate;    // ,api-> ) //float ,  6.2] 등락율                          StartPos 123, Length 6
		,sizeof(api->trdq,api->trdq) //10];    char    _trdq;    // ,api-> ) //long  ,   10] 건별체결수량                    StartPos 130, Length 10
		,sizeof(api->totq,api->totq) //15];    char    _totq;    // ,api-> ) //string,   15] 누적체결수량                    StartPos 141, Length 15
		,sizeof(api->cgubun,api->cgubun) //1];    char    _cgubun;    // ,api-> ) //string,    1] 체결구분                        StartPos 157, Length 1
		,sizeof(api->mdvolume,api->mdvolume) //15];    char    _mdvolume;    // ,api-> ) //string,   15] 매도누적체결수량                StartPos 159, Length 15
		,sizeof(api->msvolume,api->msvolume) //15];    char    _msvolume;    // ,api-> ) //string,   15] 매수누적체결수량                StartPos 175, Length 15
		,sizeof(api->ovsmkend,api->ovsmkend) //8];    char    _ovsmkend;

	);
	__common.debug_fmt(
		"<OF_SISE_RA>"
		"(packet_cd:%.*s)"
		"(stk_cd:%.*s)"
		"(acnt_tp:%.*s)"
		"(tm:%.*s)"
		"(kp200:%.*s)"
		"(bsTp:%.*s)"
		"(now_prc:%.*s)"
		"(chg:%.*s)"
		"(acml_cntr_vol:%.*s)"
		"(acml_amt:%.*s)"
		"(open:%.*s)"
		"(high:%.*s)"
		"(low:%.*s)"
		"(cntr_vol:%.*s)"
		"(fluc_rt:%.*s)"
		"(mkt_basis:%.*s)"
		"(ncntr_fluc:%.*s)"
		"(ncntr_vol:%.*s)"

		, sizeof(ra001->header.packet_cd), ra001->header.packet_cd
		, sizeof(ra001->header.stk_cd), ra001->header.stk_cd
		, sizeof(ra001->header.acnt_tp), ra001->header.acnt_tp
		, sizeof(ra001->tm), ra001->tm
		, sizeof(ra001->kp200), ra001->kp200
		, sizeof(ra001->bsTp), ra001->bsTp
		, sizeof(ra001->now_prc), ra001->now_prc
		, sizeof(ra001->chg), ra001->chg
		, sizeof(ra001->acml_cntr_vol), ra001->acml_cntr_vol
		, sizeof(ra001->acml_amt), ra001->acml_amt
		, sizeof(ra001->open), ra001->open
		, sizeof(ra001->high), ra001->high
		, sizeof(ra001->low), ra001->low
		, sizeof(ra001->cntr_vol), ra001->cntr_vol
		, sizeof(ra001->fluc_rt), ra001->fluc_rt
		, sizeof(ra001->mkt_basis), ra001->mkt_basis
		, sizeof(ra001->ncntr_fluc), ra001->ncntr_fluc
		, sizeof(ra001->ncntr_vol), ra001->ncntr_vol
	);
}
void print_of_hoga(OVH_OutBlock* api, __MAX::TRA002* ra002)
{
	__common.debug_fmt("[HOGA_API](hotime:%.*s)"
		"(offerho1:%.*s)(bidho1:%.*s)(offerrem1:%.*s)(bidrem1:%.*s)(offerno1:%.*s)(bidno1:%.*s)"
		"(offerho2:%.*s)(bidho2:%.*s)(offerrem2:%.*s)(bidrem2:%.*s)(offerno2:%.*s)(bidno2:%.*s)"
		"(offerho3:%.*s)(bidho3:%.*s)(offerrem3:%.*s)(bidrem3:%.*s)(offerno3:%.*s)(bidno3:%.*s)"
		"(offerho4:%.*s)(bidho4:%.*s)(offerrem4:%.*s)(bidrem4:%.*s)(offerno4:%.*s)(bidno4:%.*s)"
		"(offerho5:%.*s)(bidho5:%.*s)(offerrem5:%.*s)(bidrem5:%.*s)(offerno5:%.*s)(bidno5:%.*s)"
		"(totofferrem:%.*s)(totbidrem:%.*s)(totoffercnt:%.*s)(totbidcnt:%.*s)"		

		, sizeof(api->hotime), api->hotime
		, sizeof(api->offerho1), api->offerho1
		, sizeof(api->bidho1), api->bidho1
		, sizeof(api->offerrem1), api->offerrem1
		, sizeof(api->bidrem1), api->bidrem1
		, sizeof(api->offerno1), api->offerno1
		, sizeof(api->bidno1), api->bidno1
		, sizeof(api->offerho2), api->offerho2
		, sizeof(api->bidho2), api->bidho2
		, sizeof(api->offerrem2), api->offerrem2
		, sizeof(api->bidrem2), api->bidrem2
		, sizeof(api->offerno2), api->offerno2
		, sizeof(api->bidno2), api->bidno2
		, sizeof(api->offerho3), api->offerho3
		, sizeof(api->bidho3), api->bidho3
		, sizeof(api->offerrem3), api->bidho3
		, sizeof(api->bidrem3), api->bidrem3
		, sizeof(api->offerno3), api->offerno3
		, sizeof(api->bidno3), api->bidno3
		, sizeof(api->offerho4), api->offerho4
		, sizeof(api->bidho4), api->bidho4
		, sizeof(api->offerrem4), api->offerrem4
		, sizeof(api->bidrem4), api->bidrem4
		, sizeof(api->offerno4), api->offerno4
		, sizeof(api->bidno4), api->bidno4
		, sizeof(api->offerho5), api->offerho5
		, sizeof(api->bidho5), api->bidho5
		, sizeof(api->offerrem5), api->offerrem5
		, sizeof(api->bidrem5), api->bidrem5
		, sizeof(api->offerno5), api->offerno5
		, sizeof(api->bidno5), api->bidno5
		, sizeof(api->totofferrem), api->totofferrem
		, sizeof(api->totbidrem), api->totbidrem
		, sizeof(api->totoffercnt), api->totoffercnt
		, sizeof(api->totbidcnt), api->totbidcnt
	);

	__common.debug_fmt("<HOGA_RA>(packet_cd:%.*s)(stk_cd:%.*s)(acnt_tp:%.*s)(tm:%.*s)"
		"(s_vol_1:%.*s)(s_vol_2:%.*s)(s_vol_3:%.*s)(s_vol_4:%.*s)(s_vol_5:%.*s)"
		"(s_hoga_1:%.*s)(s_hoga_2:%.*s)(s_hoga_3:%.*s)(s_hoga_4:%.*s)(s_hoga_5:%.*s)"
		"(s_cnt_1:%.*s)(s_cnt_2:%.*s)(s_cnt_3:%.*s)(s_cnt_4:%.*s)(s_cnt_5:%.*s)"
		"(b_vol_1:%.*s)(b_vol_2:%.*s)(b_vol_3:%.*s)(b_vol_4:%.*s)(b_vol_5:%.*s)"
		"(b_hoga_1:%.*s)(b_hoga_2:%.*s)(b_hoga_3:%.*s)(b_hoga_4:%.*s)(b_hoga_5:%.*s)"
		"(b_cnt_1:%.*s)(b_cnt_2:%.*s)(b_cnt_3:%.*s)(b_cnt_4:%.*s)(b_cnt_5:%.*s)"
		"(s_tot_vol:%.*s)(s_tot_cnt:%.*s)(b_tot_vol:%.*s)(b_tot_cnt:%.*s)(exce_vol:%.*s)"

		, sizeof(ra002->header.packet_cd), ra002->header.packet_cd
		, sizeof(ra002->header.stk_cd), ra002->header.stk_cd
		, sizeof(ra002->header.acnt_tp), ra002->header.acnt_tp
		, sizeof(ra002->tm), ra002->tm

		, sizeof(ra002->s_vol_1), ra002->s_vol_1
		, sizeof(ra002->s_vol_2), ra002->s_vol_2
		, sizeof(ra002->s_vol_3), ra002->s_vol_3
		, sizeof(ra002->s_vol_4), ra002->s_vol_4
		, sizeof(ra002->s_vol_5), ra002->s_vol_5
		, sizeof(ra002->s_hoga_1), ra002->s_hoga_1
		, sizeof(ra002->s_hoga_2), ra002->s_hoga_2
		, sizeof(ra002->s_hoga_3), ra002->s_hoga_3
		, sizeof(ra002->s_hoga_4), ra002->s_hoga_4
		, sizeof(ra002->s_hoga_5), ra002->s_hoga_5
		, sizeof(ra002->s_cnt_1), ra002->s_cnt_1
		, sizeof(ra002->s_cnt_2), ra002->s_cnt_2
		, sizeof(ra002->s_cnt_3), ra002->s_cnt_3
		, sizeof(ra002->s_cnt_4), ra002->s_cnt_4
		, sizeof(ra002->s_cnt_5), ra002->s_cnt_5

		, sizeof(ra002->b_vol_1), ra002->b_vol_1
		, sizeof(ra002->b_vol_2), ra002->b_vol_2
		, sizeof(ra002->b_vol_3), ra002->b_vol_3
		, sizeof(ra002->b_vol_4), ra002->b_vol_4
		, sizeof(ra002->b_vol_5), ra002->b_vol_5
		, sizeof(ra002->b_hoga_1), ra002->b_hoga_1
		, sizeof(ra002->b_hoga_2), ra002->b_hoga_2
		, sizeof(ra002->b_hoga_3), ra002->b_hoga_3
		, sizeof(ra002->b_hoga_4), ra002->b_hoga_4
		, sizeof(ra002->b_hoga_5), ra002->b_hoga_5
		, sizeof(ra002->b_cnt_1), ra002->b_cnt_1
		, sizeof(ra002->b_cnt_2), ra002->b_cnt_2
		, sizeof(ra002->b_cnt_3), ra002->b_cnt_3
		, sizeof(ra002->b_cnt_4), ra002->b_cnt_4
		, sizeof(ra002->b_cnt_5), ra002->b_cnt_5

		, sizeof(ra002->s_tot_vol), ra002->s_tot_vol
		, sizeof(ra002->s_tot_cnt), ra002->s_tot_cnt
		, sizeof(ra002->b_tot_vol), ra002->b_tot_vol
		, sizeof(ra002->b_tot_cnt), ra002->b_tot_cnt
		, sizeof(ra002->exce_vol), ra002->exce_vol
	);
}
