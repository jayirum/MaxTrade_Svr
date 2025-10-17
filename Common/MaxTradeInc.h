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

	const char MSG_SUCC[] = "����ó�� �Ǿ����ϴ�.";

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


	// packet code ���� ////////////////////////////////////////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////////////// packet code ���� 

	struct THeader
	{
		char len[3];
		char packet_cd[5];
		char userid[LEN_USERID];
		char acnt_tp[1];	// 1:����, 2:�ؿ�, 3:����ȭ��
		char err_cd[4];		
		char tm[9];			// hhmmssmmm
	};

	struct TNOTIHeader
	{
		THeader header;
		char mng_yn[1];	//�������뺸 ����
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
		char	dupYn[1];	//Y:�ߺ��α��� ���, 
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

	// [282TA001YUMJH               000001016336015VNQH25                                                  1139       20005.25        1              0        0        1    [���ǻ���ü�] ���簡/1ȣ�� ū�������� ü��ź�                                 20250310101633                        ]
	struct TTA001 // CNTR
	{
		THeader header;
		char tr_cd			[1];	//1:�ű��ֹ�, 2:���� 3:��� 4,ü�� 5:�ź�
		char api_tp			[1];	//R:��ü��, V : ����ü��
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

		char Enter[2];	// len �� ���� ����
	};

	struct TNM001 //����ݰ�� �뺸
	{
		THeader header;
		char msg[50];
		char refresh_yn[1];	// client refresh
	};


	struct TNL001 // �帶�� �뺸
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
		char ord_tp[2]; // 01:���尡, 02:������, 03:����, 04:���
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
	
	// ���簡
	struct TRA001
	{
		TRealHeader header;
		char tm			[6];	//hhmmss
		char kp200		[10];
		char bsTp		[1];	//ü�ᱸ��
		char now_prc	[10];
		char chg		[10]; //���ϴ��
		char acml_cntr_vol[9]; //�����ŷ���(�������)
		char acml_amt	[15];//�����ŷ����(�������)
		char open		[10];
		char high		[10];
		char low		[10];
		char cntr_vol	[9];
		char fluc_rt	[5]; //�����
		char mkt_basis	[5];	// ����BASIS
		char ncntr_fluc	[9];	// �̰�����������
		char ncntr_vol[	9];		// �̰�����������(�������)
		char Enter[2];
	};

	// ȣ��
	struct TRA002
	{
		TRealHeader header;
		
		char tm[6];
		char s_vol_1[6];	//�ŵ�ȣ������1
		char s_vol_2[6];	//�ŵ�ȣ������2
		char s_vol_3[6];	//�ŵ�ȣ������3
		char s_vol_4[6];	//�ŵ�ȣ������4
		char s_vol_5[6];	//�ŵ�ȣ������5
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
		char b_vol_1[6];	//�ŵ�ȣ������1
		char b_vol_2[6];	//�ŵ�ȣ������2
		char b_vol_3[6];	//�ŵ�ȣ������3
		char b_vol_4[6];	//�ŵ�ȣ������4
		char b_vol_5[6];	//�ŵ�ȣ������5
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
		
		char s_tot_vol[9];//	�ŵ�ȣ���Ѽ���
		char s_tot_cnt[9]; //	�ŵ�ȣ���ѰǼ�
		char b_tot_vol[9];//	�ż�ȣ���Ѽ���
		char b_tot_cnt[9]; //	�ż�ȣ���ѰǼ�
		char exce_vol[9]; //EXCE_RMNQ	9	�ʰ��ܷ�(�ż��ܷ�-�ŵ��ܷ�)

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
		//  �̸� ������ ������ ��ü�� �����Ͽ� Ǯ�� ����
		CMemoryPool() 
		{
			for (size_t i = 0; i < MEMPOOL_SIZE; ++i) {
				pool.push_back(new TData);
			}
		}

		//  ��ü ��û (Ǯ���� ��� ������ ��ü ����)
		TData* Alloc() 
		{
			std::lock_guard<std::mutex> lock(poolMutex);
			if (!pool.empty()) 
			{
				TData* obj = std::move(pool.back());  //  Ǯ���� �ϳ� ����
				pool.pop_back();
				//obj->init();
				return obj;
			}
			return new TData;  // Ǯ�� ������� ���� ����
		}

		// ��ü ��ȯ (Ǯ�� �ٽ� ����)
		void release(TData* obj) {
			std::lock_guard<std::mutex> lock(poolMutex);
			obj->init();
			pool.push_back(std::move(obj));  // ��ȯ�� ��ü�� �ٽ� Ǯ�� ����
		}

		// ���� ��� ������ ��ü ���� ��ȯ
		size_t availableCount() {
			std::lock_guard<std::mutex> lock(poolMutex);
			return pool.size();
		}

	private:
		std::vector<TData*> pool;  //  �޸� Ǯ
		std::mutex poolMutex;  //  ��Ƽ������ ��ȣ
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

