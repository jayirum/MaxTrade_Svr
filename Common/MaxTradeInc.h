#pragma once


#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 4996 )
#pragma warning( disable : 26496)
#pragma warning( disable : 26495)


#include <WinSock2.h>
#include <map>
#include <string>
#include <vector>
#include <boost/pool/pool.hpp>

using namespace std;

namespace __MAX
{
	const int BUF_LEN = 512;
	const int BIGBUF_LEN = 1024;

	inline constexpr char ACNT_TP_K = '1';
	inline constexpr char ACNT_TP_FU = '2';
	inline constexpr char ACNT_TP_CC = '3';

	///////////////////////////////////////////////////////////////////////////////////////////////////
	inline constexpr char BUY_S		= 'B';
	inline constexpr char SELL_S	= 'S';
	inline const string ORDTP_MARKET = "01";
	inline const string ORDTP_LIMKIT = "02";
	inline const string PROCTP_NEW = "1";
	inline const string PROCTP_MODIFY = "2";
	inline const string PROCTP_CANCEL = "3";

	enum class BSTP_IDX{BUY=0, SELL};
	//inline const int BUY_IDX = 0;
	//inline const int SELL_IDX = 1;

	///////////////////////////////////////////////////////////////////////////////////////////////////


	const char ERR_SUCC[]			= "0000";
	const char ERR_WRONG_PACKET[] = "1001";
	const char ERR_WITH_MSG[]		= "9999";

	const char MSG_SUCC[] = "정상처리 되었습니다.";

	//------------------------------------------------------------------------------------//
	const int WM_ORDER_SEND		= WM_USER + 411;
	const int WM_RECEIVE_DATA	= WM_USER + 412;
	const int WM_LOGON			= WM_USER + 413;
;	const int WM_MARKET_DATA	= WM_USER + 414;
;	const int WM_POSITION_DATA	= WM_USER + 415;
	const int WM_ORDER_OPEN		= WM_USER + 416;
	const int WM_ORDER_CLOSE	= WM_USER + 417;
	const int WM_CONFIG_UPDATED	= WM_USER + 418;
	const int WM_LOGON_AUTH		= WM_USER + 419;
	const int WM_MSG			= WM_USER + 422;
	const int WM_COMMAND_CODE	= WM_USER + 423;
	const int WM_REQUEST_SENDDATA = WM_USER + 424;
	const int WM_JUST_RELAY		= WM_USER + 425;
	const int WM_BALANCE = WM_USER + 426;
	const int WM_JUST_BYPASS = WM_USER + 426;
	//------------------------------------------------------------------------------------//


	// packet code 관련 ////////////////////////////////////////////////////////////////////////////
	const int POS_START_PACKET_CD = 3;
	const int PACKET_CD_LEN = 5;

	const char CD_LOGON[] = "PC001";
	const char CD_REG_STK[] = "PC003";
	const char CD_SISE[] = "RA001";
	const char CD_HOGA[] = "RA002";
	const char CD_CNTR[] = "TA001";
	const char CD_NOTI_INOUT[] = "NM001";
	const char CD_NOTI_MKTCLOSE[] = "NL001";

	const char ORD_TP_MARKET[] = "01";
	const char ORD_TP_LIMIT[] = "02";
	const char ORD_TP_MODIFY[] = "03";
	const char ORD_TP_CANCEL[] = "04";
	const char ORD_TP_MIT[] = "05";
	const char ORD_TP_CNCL_SYMBOL[] = "07";
	const char ORD_TP_CNCL_ALL[] = "08";
	const char ORD_TP_CLR_SYMBOL[] = "09";
	const char ORD_TP_CLR_ALL[] = "10";

	const char ORD_PROC_NEW[] = "1";
	const char ORD_PROC_MODIFY[] = "2";
	const char ORD_PROC_CANCEL[] = "3";

	const char COND_TP_MIT[] = "01";

	const char PACKET_DELIMITER[] = "\n";
	const int DELIMITER_LEN = 1;
	const int LEN_DELIMITER = DELIMITER_LEN;
	const int LEN_STK_CD = 10;
	const int LEN_USERID = 20;
	const int LEN_ORD_NO = 20;
	const int LEN_PRC = 15;


	const char POLLING_DATA[] = "@POLLING";
	////////////////////////////////////////////////////////////////////////////// packet code 관련 

	struct THeader
	{
		char len[3];
		char packet_cd[5];
		char userid[LEN_USERID];
		char acnt_tp[1];	// 1:국내, 2:해외, 3:가상화폐
		char err_cd[4];		
		char tm[9];			// hhmmssmmm
	};

	struct TNOTIHeader
	{
		THeader header;
		char mng_yn[1];	//관리자통보 여부
	};

	struct TRealHeader	// RA001, RA002
	{
		char packet_cd[5];
		char stk_cd[LEN_STK_CD];
		char acnt_tp[1];
	};

	struct TPC001
	{
		THeader	header;
		char	userTp[1];	//C:Client, M:Manager, S:System, O:G_B_OMS
		char	dupYn[1];	//Y:중복로그인 허용, 
		char	mentorId[20];
	};
	struct TPC003
	{
		char len[3];
		char packet_cd[5];
		char userid[LEN_USERID];
		char stkcd1[LEN_STK_CD];
		char stkcd2[LEN_STK_CD];
		char stkcd3[LEN_STK_CD];
	};

	// [282TA001YUMJH               000001016336015VNQH25                                                  1139       20005.25        1              0        0        1    [증권사오시세] 현재가/1호가 큰가격차이 체결거부                                 20250310101633                        ]
	struct TTA001 // CNTR
	{
		THeader header;
		char tr_cd			[1];	//1:신규주문, 2:정정 3:취소 4,체결 5:거부
		char api_tp			[1];	//R:실체결, V : 가상체결
		char stk_cd			[10];
		char api_ord_no		[20];
		char api_cntr_no	[20];
		char ord_no			[9];
		char ord_prc		[15];
		char ord_qty		[9];
		char cntr_prc		[15];
		char cntr_qty		[9];
		char remn_qty		[9];
		char api_rslt_cd	[4];
		char api_msg		[80];
		char api_dt			[8];//YYYYMMDD
		char api_tm			[6]; //HHNNSS
		char api_org_no		[20];
		char api_rjct_cd	[4];

		char Enter[2];	// len 에 들어가지 않음
	};

	struct TNM001 //입출금결과 통보
	{
		THeader header;
		char msg[50];
		char refresh_yn[1];	// client refresh
	};


	struct TNL001 // 장마감 통보
	{
		TNOTIHeader notiHeader;
		char artc_cd[6];
		char msg[30];
		char refresh_yn[1];
	};
	
	struct TTC001
	{
		THeader header;
		char symbol[10];
		char acnt_no[11];
		char acnt_pwd[4];
		char ord_no[9];
		char bs_tp[1];
		char ord_tp[2]; // 01:시장가, 02:지정가, 03:정정, 04:취소
		char cond_tp[1];
		char ord_prc[15];
		char ord_vol[9];
		char mit_prc[15];
		char sl_prc[15];
		char sl_tp[1];
		char org_ord_no[9];
		char org_ord_prc[15];
		char mng_id[20];
		char client_ip[15];
		char api_seq[3];
		char api_org_no[20];
		char hoga_no[2];
		char same_hoga_yn[1];
		char now_prc[15];
		char mit_ticks[3];
		char enter[2];
	};
	
	// 현재가
	struct TRA001
	{
		TRealHeader header;
		char tm			[6];	//hhmmss
		char kp200		[10];
		char bsTp		[1];	//체결구분
		char now_prc	[10];
		char chg		[10]; //전일대비
		char acml_cntr_vol[9]; //누적거래량(단위계약)
		char acml_amt	[15];//누적거래대금(단위억원)
		char open		[10];
		char high		[10];
		char low		[10];
		char cntr_vol	[9];
		char fluc_rt	[5]; //등락율
		char mkt_basis	[5];	// 시장BASIS
		char ncntr_fluc	[9];	// 미결제약정증감
		char ncntr_vol[	9];		// 미결제약정수량(단위계약)
		char Enter[2];
	};

	// 호가
	struct TRA002
	{
		TRealHeader header;
		
		char tm[6];
		char s_vol_1[6];	//매도호가수량1
		char s_vol_2[6];	//매도호가수량2
		char s_vol_3[6];	//매도호가수량3
		char s_vol_4[6];	//매도호가수량4
		char s_vol_5[6];	//매도호가수량5
		char s_hoga_1[10];
		char s_hoga_2[10];
		char s_hoga_3[10];
		char s_hoga_4[10];
		char s_hoga_5[10];
		char s_cnt_1[5];
		char s_cnt_2[5];
		char s_cnt_3[5];
		char s_cnt_4[5];
		char s_cnt_5[5];
		char b_vol_1[6];	//매도호가수량1
		char b_vol_2[6];	//매도호가수량2
		char b_vol_3[6];	//매도호가수량3
		char b_vol_4[6];	//매도호가수량4
		char b_vol_5[6];	//매도호가수량5
		char b_hoga_1[10];
		char b_hoga_2[10];
		char b_hoga_3[10];
		char b_hoga_4[10];
		char b_hoga_5[10];
		char b_cnt_1[5];
		char b_cnt_2[5];
		char b_cnt_3[5];
		char b_cnt_4[5];
		char b_cnt_5[5];
		
		char s_tot_vol[9];//	매도호가총수량
		char s_tot_cnt[9]; //	매도호가총건수
		char b_tot_vol[9];//	매수호가총수량
		char b_tot_cnt[9]; //	매수호가총건수
		char exce_vol[9]; //EXCE_RMNQ	9	초과잔량(매수잔량-매도잔량)

		char Enter[2];
	};

	bool Is_SisePacket(const char* pData);
	bool Is_HogaPacket(const char* pData);
	bool Is_RegStkPacket(const char* pData);
	char* FormatPrc(const double dPrc, _Out_ char* pzPrc);
	void compose_ra001_from_binance(std::string& jsonData, std::map<std::string, long>&	mapSymbol, char* out);
	void compose_ra002_from_binance(std::string& jsonData, char* out);

	class CPacketBuffer
	{
	public:
		CPacketBuffer();
		~CPacketBuffer();

		bool	Add(_In_ char* pRecvBuf, int nSize);
		bool	GetOnePacketLock(_Out_ int* pnLen, _Out_ char* pOutBuf);
		bool 	GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf);

		string	GetMsg() { return m_sMsg; }
		int		GetBuffLen() { return (int)m_sBuffer.size(); }
	private:
		void	Erase(int nStartPos, int nLen);
		void	RemoveAll(bool bLock);

		void	Lock() { EnterCriticalSection(&m_cs); }
		void	Unlock() { LeaveCriticalSection(&m_cs); }
	private:
		CRITICAL_SECTION	m_cs;
		string				m_sBuffer;
		string 				m_sMsg;
	};

	class CPacketBufferIocp
	{
	public:
		CPacketBufferIocp();
		~CPacketBufferIocp();

		void	AddSocket(SOCKET sock);
		int		AddPacket(SOCKET sock, char* pBuf, int nSize);

		BOOL	GetOnePacket(SOCKET sock, _Out_ int* pnLen, _Out_ char* pOutBuf);

	private:
		void	Lock() { EnterCriticalSection(&m_cs); }
		void	UnLock() { LeaveCriticalSection(&m_cs); }

	private:

		map<SOCKET, CPacketBuffer*>		m_mapBuffer;
		CRITICAL_SECTION		m_cs;
	};


	struct TData
	{
		char d[BIGBUF_LEN];
		void init() { memset(d, 0x00, sizeof(d)); }
		TData() { init(); }
	};

#define MEMPOOL_SIZE	1000
	class CMemoryPool 
	{

	public:
		//  미리 정해진 개수의 객체를 생성하여 풀에 저장
		CMemoryPool() 
		{
			for (size_t i = 0; i < MEMPOOL_SIZE; ++i) {
				pool.push_back(new TData);
			}
		}

		//  객체 요청 (풀에서 사용 가능한 객체 제공)
		TData* Alloc() 
		{
			std::lock_guard<std::mutex> lock(poolMutex);
			if (!pool.empty()) 
			{
				TData* obj = std::move(pool.back());  //  풀에서 하나 꺼냄
				pool.pop_back();
				//obj->init();
				return obj;
			}
			return new TData;  // 풀이 비었으면 새로 생성
		}

		// 객체 반환 (풀에 다시 저장)
		void release(TData* obj) {
			std::lock_guard<std::mutex> lock(poolMutex);
			obj->init();
			pool.push_back(std::move(obj));  // 반환된 객체를 다시 풀에 저장
		}

		// 현재 사용 가능한 객체 개수 반환
		size_t availableCount() {
			std::lock_guard<std::mutex> lock(poolMutex);
			return pool.size();
		}

	private:
		std::vector<TData*> pool;  //  메모리 풀
		std::mutex poolMutex;  //  멀티스레드 보호
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class THREAD_FLAG { IDLE, RUN, STOP };
	class CThreadFlag
	{
	public:
		void	set_idle() { m_flag = THREAD_FLAG::IDLE; }
		void	set_stop() { m_flag = THREAD_FLAG::STOP; }
		void	set_run() { m_flag = THREAD_FLAG::RUN; }

		bool	is_stopped() { return (m_flag == THREAD_FLAG::STOP); }
		bool	is_idle() { return (m_flag == THREAD_FLAG::IDLE); }
		bool	is_running() { return (m_flag == THREAD_FLAG::RUN); }
	private:
		THREAD_FLAG	m_flag;
	};
}

