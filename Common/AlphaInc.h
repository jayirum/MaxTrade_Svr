#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4700)
#pragma warning(disable:4703)
#pragma warning(disable:4267)
#pragma warning(disable:4477)
#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 4996 )
#pragma warning(disable:26812)
#pragma warning(disable:6386)
#pragma warning(disable:6101)

#include <Windows.h>
#include <string>
#include <tchar.h>
using namespace std;

#ifndef _In_
	#define _In_
#endif
#ifndef _Out_
	#define _Outn_
#endif
#ifndef _InOut_
	#define _InOut_
#endif

//DEF-REGISTRY
#define	ALPHABLASH_ROOT			"Software\\ALPHABLASH"
#define	ALPHABLASH_DIRECTORY	"Software\\ALPHABLASH\\DIRECTORY"
#define	ALPHABLASH_ALERMSVR		"Software\\ALPHABLASH\\ALERMSVR"

namespace __ALPHA
{
#ifdef _UNICODE
	void ComposeEAConfigFileName(_In_ char* pzDir, _Out_ wchar_t* pwzFileName);
#endif

	enum EN_RET_VAL { RET_ERR=-1, RET_SKIP, RET_OK};

	enum CLIENT_TP	{ MASTR=0, COPIER};
	#define MC_TP_MASTER	'M'
	#define MC_TP_COPIER	'C'

	enum PUBSCOPE_TP {
		ALLCOPIERS_UNDER_ONEMASTER,	//	All slaves under one master id
		ONECOPIER_WITH_ID,			//	one Specific Slave
		ALLMASTERS,					//	All Masters
		ONEMASTER_WITH_ID,			//	one specific master
		ALL
	};


	//enum ORD_ACTION {
	//	ORD_ACTION_NONTRADE_SYMBOL=-1,
	//	ORD_ACTION_NONE = 0,
	//	ORD_ACTION_OPEN,			// Pending�ֹ� �Ǵ� ������ ����
	//	ORD_ACTION_CLOSE_FULL,		// ������ ��ü û��
	//	ORD_ACTION_CLOSE_PARTIAL,	// ������ �κ� û��
	//	ORD_ACTION_CHANGE,			// OpenPrice, SL, TP, Expiry
	//	ORD_ACTION_DELETED,			// pending ����
	//	ORD_ACTION_FAILED = 90
	//};

	enum EN_USER_ACTION { 
		USERACTION_ORD				//user action	Order
		,USERACTION_UPDATE			//user_action	1	user action	Update User Info
		,USERACTION_LOGOFF_FORCELY	//user_action	2	user action	Log Off forcely
	};
	enum EN_ORD_ACTION { ORDACTION_NONE, ORDACTION_NEW, ORDACTION_CLOSE, ORDACTION_CHANGE };
	enum EN_CLOSE_TP {
		CLOSETP_ALL,
		CLOSETP_SYMBOL,
		CLOSETP_MAGIC,
		CLOSETP_PROFIT,
		CLOSETP_LOSS,
		CLOSETP_TICKET,
		CLOSETP_BUY,
		CLOSETP_SELL
	};
	enum EN_ACTION_SCOPE { SCOPE_EVERY_BROKER, SCOPE_ONEBROKER_ALL, SCOPE_ONEBROKER_PART }; //#define FDN_ACTION_SCOPE	530

	//enum EN_DELORD_TP { DELORDTP_ALL, DELORDTP_SYMBOL, DELORDTP_MAGIC, DELORDTP_TICKET };
	enum EN_PRC_PT_TP	{ TP_PRICE, TP_POINT };	// #define FDN_PRC_PT_TP      528
	//enum IDX_CHG_ACTION {
	//	IDX_CHG_OPEN_PRC = 0,
	//	IDX_CHG_SL,
	//	IDX_CHG_TP,
	//	IDX_CHG_EXPIRY
	//};


#define MAX_TIMEFRAMES_CNT	9
	enum EN_TIMEFRAMES {
		IDX_1M = 0,
		IDX_5M,
		IDX_15M,
		IDX_30M,
		IDX_1H,
		IDX_4H,
		IDX_1D,
		IDX_1W,
		IDX_1MN
	};
#define TIMEFRAME_M1    "M1"
#define TIMEFRAME_M5    "M5"
#define TIMEFRAME_M15   "M15"
#define TIMEFRAME_M30   "M30"
#define TIMEFRAME_H1    "H1"
#define TIMEFRAME_H4    "H4"
#define TIMEFRAME_D1    "D1"
#define TIMEFRAME_W1    "W1"
#define TIMEFRAME_MN1   "MN1"

	#define CHG_ACTION_SIZE	4

	#define URL_WEBSITE	"http://119.205.238.151/login"
	#define CONFIG_FILE	"AlphaBlash.ini"
	#define MSG_FILE_EN		"AlphaMsg_en.ini"
	#define MSG_FILE_KO		"AlphaMsg_ko.ini"
	#define MSG_FILE_JAP	"AlphaMsg_jap.ini"
	#define MSG_FILE_CHINA	"AlphaMsg_china.ini"

	const char MT4_LANG_KOR[] = "Korean";
	const char MT4_LANG_ENG[] = "English";
	const char MT4_LANG_JAP[] = "Japanese";
	const char MT4_LANG_CHINA[] = "Chinese";

	const char BUY_SIDE = 'B';
	const char SELL_SIDE = 'S';
	const char FLAT_SIDE = '0';

	#define QDATA_TP_CONFIG	1
	#define QDATA_TP_NOTI	2

	#define SYMBOL_CONFIG_CNT	20

	#define DEF_YES		true
	#define	DEF_NO		false

	#define DEF_DBEXEC_SP		'S'
	#define DEF_DBEXEC_QUERY	'Q'

	const int SIZE_SYMBOL	= 10;
	const int SIZE_USER_ID	= 20;
	const int SIZE_ACCNO	= 15;
	const int LEN_SYMBOL	= SIZE_SYMBOL;
	const int LEN_USERID	= SIZE_USER_ID;
	const int LEN_ACCNOT	= SIZE_ACCNO;
	const int LEN_PRC		= 10;
	const int LEN_BUF = 4096;
	const int LEN_BUF_2K = 2046;
	const int LEN_BUF_1K = 1024;
	const int BUF_LEN = LEN_BUF;
	const char DEF_BUY		= 'B';
	const char DEF_SELL		= 'S';
	const int FIELD_LEN = 32;
	const int LEN_FIELD = FIELD_LEN;
	const int DOT_CNT = 5;
	const int LEN_CODE_SIZE = 4;
	


	//#define WM_ORDER_SEND	WM_USER + 411
	//#define WM_RECEIVE_DATA	WM_USER + 412
	//#define WM_LOGON		WM_USER + 413
	//#define WM_MARKET_DATA	WM_USER + 414
	//#define WM_POSITION_DATA	WM_USER + 415
	//#define WM_ORDER_OPEN	WM_USER + 416
	//#define WM_ORDER_CLOSE	WM_USER + 417
	//#define WM_CONFIG_UPDATED	WM_USER + 418
	//#define WM_LOGON_AUTH		WM_USER + 419
	#define WM_LOGMSG_LOG		WM_USER + 420
	#define WM_LOGMSG_NOTI		WM_USER + 421
	//#define WM_MSG				WM_USER + 422
	//#define WM_COMMAND_CODE		WM_USER + 423
	//#define WM_REQUEST_SENDDATA WM_USER + 424
	//#define WM_JUST_RELAY		WM_USER + 425
	//#define WM_BALANCE			WM_USER + 426
	//#define WM_JUST_BYPASS		WM_USER + 426



	#define CMD_TERMINATE_EA			"1001"
	#define CMD_PAUSE_EA				"1002"
	#define CMD_RESUME_EA				"1002"
	#define CMD_MD_SUB					"2002"
	#define CMD_MD_UNSUB				"2003"

	#define CMD_CLOSE_ALLPOS_DELETE_ALLORD	"3001"

	#define CMD_NOTI_LOGONOUT				"4001"


	//////////////////////////////////////////////////////////////////////////
	//
	// Must sync with BPProtocol.mqh
	//
	const char SYS_MT4	[] = "MT4";
	const char SYS_NINJA[] = "NINJA";
	const char SYS_ZORRO[] = "ZORRO";

	#define PACKET_CODE_SIZE 4
	#define PACKET_HEADER_SIZE	 10	// STX134=0074SOH

	const char CODE_REG_MASTER		[]	= "1001";
	const char CODE_REG_COPIER		[]	= "1002";
	const char CODE_MASTER_ORDER	[]	= "1003";
	const char CODE_PUBLISH_ORDER	[]	= "1004";
	const char CODE_PING			[]	= "1005";
	const char CODE_LOGOFF			[]	= "1006";
	const char CODE_LOGON			[]	= "1007";
	const char CODE_LOGON_MASTER	[]	= "1008";	// SLAVE ���� ����
	const char CODE_LOGOFF_MASTER	[]	= "1009";	// SLAVE ���� ����
	const char CODE_PING_LOGOFF		[]	= "1010";	// Log Off by ping condition
	const char CODE_COPIER_ORDER	[]  = "1011";
	const char CODE_USER_LOG		[]	= "1012";
	const char CODE_OPEN_ORDERS		[]	= "1013";
	const char CODE_ONLINE_COPIERS	[]	= "1014";
	const char CODE_CONFIG_SYMBOL	[]	= "1015";
	const char CODE_CONFIG_GENERAL	[]	= "1016";
	const char CODE_DUP_LOGON		[]	= "1017";
	const char CODE_OFFLINE_COPIERS	[]	= "1018";
	const char CODE_COPIER_UNSUBS	[]	= "1019";
	const char CODE_EA_MESSAGE		[]	= "1020";
	const char CODE_COPIER_LIST		[]	= "1021";
	const char CODE_RESET_MCTP		[]	= "1022";
	const char CODE_ORDER_OPEN		[]	= "1023";
	const char CODE_ORDER_CLOSE		[]	= "1024";
	const char CODE_HISTORY_CANDLES []  = "1025";
	const char CODE_LOGON_AUTH		[] = "1026";
	const char CODE_COMMAND_BY_CODE [] = "1027";
	const char CODE_ORDER_CHANGE	[] = "1028";
	const char CODE_MSG				[] = "1029";
	//const char CODE_POSORD_SNAPSHOT []	= "1029";
	


	const char CODE_MARKET_DATA		[] = "8001";
	const char CODE_POSITION		[] = "8002";
	const char CODE_SYMBOL_SPEC		[] = "8003";
	const char CODE_CANDLE_DATA		[] = "8004";
	const char CODE_POSORD			[] = "8005";
	const char CODE_BALANCE[] = "8006";

	const char CODE_PUBLISH_TEST	[]	= "9001";
	const char CODE_RETURN_ERROR	[]	= "9002";
	const char CODE_REG_ROUTER		[] = "9991";
	const char CODE_UNREG_ROUTER	[] = "9992";

	const char TP_COMMAND	= 'C';
	const char TP_ORDER		= 'O';
	const char TP_REG		= 'R';
	const char TP_UNREG		= 'U';
	const char TP_MASTER	= 'M';
	const char TP_COPIER	= 'C';

	#define DEF_REG		"R"
	#define DEF_UNREG	"U"

	#define DEF_POS	"P"
	#define DEF_ORD "O"

	#define DIRECTION_TO_EA		"TO_EA"
	#define DIRECTION_TO_MGR	"TO_MGR"
	#define DIRECTION_TO_BOTH	"TO_BOTH"

	
	//#define LEN_CHANNEL_NM	128
	//#define LOCAL_RELAY_CHANNEL	"BA_LOCALRELAY"

	//+--------------------------------------------------------------------
	//+	Field
	//+--------------------------------------------------------------------

#define DEF_DELI			0x01
#define DEF_STX				0x02
#define DEF_ETX				0x03
#define DEF_DELI_COLUMN		0x05
#define DEF_DELI_RECORD	    0x06
//#define DEF_DELI_ARRAY      0x06
#define DEF_ENTER			0x0A
#define DEF_PACKETLEN_SIZE	4

//sprintf(temp, "%c%d=%0*d%c", DEF_STX, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, len, DEF_DELI);
// STX+134=0195+0x01
#define DEF_HEADER_SIZE 10	// STX, 134=0000, DELI

#define FDS_CODE				101
#define FDS_COMMAND_CODE		102	// CommandCodes.h
#define FDS_SYS					103 //  MT4, 
#define FDS_TM_HEADER			104 //  yyyymmdd_hhmmssmmm
#define FDS_SUCC_YN				105
#define FDS_MSG 				106

#define FDS_SYMBOL				107
#define FDS_NOTI_MSG			108 
#define FDS_MASTERCOPIER_TP		109	// M:MASTER, C:COPIER
#define FDS_OPEN_TM				110
#define FDS_CLOSE_TM			111
#define FDS_COMMENTS			112
#define FDS_LIVEDEMO			113
#define FDS_USERID_MINE			114
#define FDS_USERID_MASTER		115
#define FDS_LOGONOFF_MSG		116
#define FDS_MASTER_LOGON_YN		117
#define FDS_USER_LOG			118
#define FDS_ACCNO_MY			119
#define FDS_ACCNO_MINE			FDS_ACCNO_MY
#define FDS_ACCNO_MASTER		120
#define FDS_BROKER				121
#define FDS_USER_NICK_NM		122
#define FDS_USER_PASSWORD		123
#define FDS_RELAY_IP			124
#define FDS_RELAY_PORT			125
#define FDS_CONFIG_DATA			126
#define FDS_EXPIRY				127
//#define FDS_ARRAY_TICKET		128
//#define FDS_MT4_TICKET			129
#define FDS_LAST_ACTION_MT4_TM	130
#define FDS_OPEN_GMT			131
#define FDS_LAST_ACTION_GMT		132
//#define FDS_MASTER_TICKET       133
#define FDS_PACK_LEN		    134
#define FDS_ARRAY_SYMBOL		135
#define FDS_COPY_TP			    136
#define FDS_COPY_OPEN_YN	    137
#define FDS_COPY_ENTRY_YN       FDS_COPY_OPEN_YN
#define FDS_COPY_CLOSE_YN	    138
#define FDS_COPY_SL_YN			139
#define FDS_COPY_TP_YN			140
#define FDS_COPY_PENDING_YN		141
#define FDS_ORD_BUY_YN			142
#define FDS_ORD_BUY_LIMIT_YN	143
#define FDS_ORD_BUY_STOP_YN		144
#define FDS_ORD_SELL_YN			145
#define FDS_ORD_SELL_LIMIT_YN	146
#define FDS_ORD_SELL_STOP_YN	147
#define FDS_VOL_EQTYRATIO_MULTIPLE_YN	148
#define FDS_ORD_ACTION_CHG      149
#define FDS_MAXLOT_ONEORDER_YN	150
#define FDS_MAXLOT_TOTORDER_YN	151
#define FDS_MAX_SLPG_YN			152
#define FDS_MARGINLVL_LIMIT_YN	153
#define FDS_TIMEFILTER_YN		154
#define FDS_MARKETORD_YN		155
#define FDS_LIMITORD_YN			156
#define FDS_STOPORD_YN			157
#define FDS_CLOSE_GMT			158
#define FDS_CLIENT_IP			159
#define FDS_USERID_COPIER		160
#define FDS_ACCNO_COPIER		161
#define FDS_SITEAUTOLOGON_KEY	162
#define FDS_WEBSITE_URL			163
#define FDS_TR_PORT				164
#define FDS_ORD_POS_TP			165	//O:ORDER, P:POSITION
#define FDS_ORDER_GROUPKEY   166
//#define FDS_KEEP_ORGTICKET_YN    167
//#define FDS_MT4_TICKET_CLOSING   168
#define FDS_ORD_ACTION_SUB_PARTIAL_YN   169
#define FDS_ORD_SIDE	170
#define FDS_KEY			171
#define FDS_REGUNREG	172   //  R, U
#define FDS_DATA				173
#define FDS_TERMINAL_NAME		174
#define FDS_ARRAY_DATA			175
#define FDS_CLIENT_SOCKET_TP	176
#define FDS_MARKETDATA_TIME     177		//yyyy.mm.dd hh:mm:ss
#define FDS_TIMEFRAME			178		//M1/M5/M15/M30/H1/H4/D1/W1/MN1
#define FDS_CANDLE_TIME			179
//#define FDS_MT4_TICKET_ORG		180
#define FDS_CLR_TP				181
#define FDS_CLIENT_TP			182
#define FDS_TERMINAL_PATH		183
#define FDS_MAC_ADDR			184
#define FDS_USER_ID				185
#define FDS_COMPUTER_NM			186
#define FDS_DATASVR_IP			187
#define FDS_DATASVR_PORT		188
#define FDS_TIME				189
#define FDS_FLOW_DIRECTION		190

#define FDS_ROUTE_YN			191
#define FDS_BIZ_CODE			192
#define FDS_ARRAY_START			193
#define FDS_ARRAY_END			194
#define FDS_ORD_TYPE			195      // BUY, SELL BUYLIMIT.....
#define FDS_JUSTRELAY_YN		196
#define FDS_VERSION				197

#define FDN_ORD_TYPE		500 // OP_BUY, OP_SELL, OP_BUYLIMIT, OP_SELLLIMIT, OP_BUYSTOP, OP_SELLSTOP
#define FDN_PUBSCOPE_TP		501	
#define FDN_OPEN_TM			502
#define FDN_CLOS_TM			503	
#define FDN_ERR_CODE		504
#define FDN_RSLT_CODE		FDN_ERR_CODE
#define FDN_ORD_ACTION		505
#define FDN_ARRAY_SIZE		506	// ���ο��� ARRAY �� �����͸� ������ ��
#define FDN_SLTP_TP				507
#define FDN_COPY_VOL_TP			508
#define FDN_MARGINLVL_LIMIT_ACTION	509
#define FDN_TIMEFILTER_H	510
#define FDN_TIMEFILTER_M	511
#define FDN_BROKER_IDX		512
#define FDN_DECIMAL			513
#define FDN_SYMBOL_IDX		514
#define FDN_SIDE_IDX		515
#define FDN_RECORD_CNT		516
#define FDN_SUBS_STATUS		517
#define FDN_TERMINAL_IDX	518
#define FDN_ORDER_CMD	    519   // MT4 CMD - OP_BUY, OP_SELL, OP_BUYLIMIT...
#define FDN_MAGIC_NO		520
#define FDN_DATA_CNT        521
#define FDN_TICKET			522
#define FDN_APP_TP			523
#define FDN_CMD_CODE		524
#define FDN_COUNT			525
#define FDN_CLOSE_TP		526
#define FDN_DELORD_TP		527
//#define FDN_PRC_PT_TP_1    	528
//#define FDN_PRC_PT_TP_2    	529
#define FDN_ACTION_SCOPE	530
#define FDN_PACKET_SEQ		599

#define FDD_OPEN_PRC		700
#define FDD_CLOSE_PRC		701  
#define FDD_LOTS			702
#define FDD_VOL         702
#define FDD_SLPRC			703
#define FDD_TPPRC			704
#define FDD_PROFIT			705
#define FDD_SWAP			706
#define FDD_CMSN			707
#define FDD_VOL_MULTIPLIER_VAL	708
#define FDD_VOL_FIXED_VAL		709
#define FDD_VOL_EQTYRATIO_MULTIPLE_VAL	710
#define FDD_MAXLOT_ONEORDER_VAL	711
#define FDD_MAXLOT_TOTORDER_VAL	712
#define FDD_MAX_SLPG_VAL		713
#define FDD_MARGINLVL_LIMIT_VAL	714
#define FDD_CLOSE_LOTS          715
#define FDD_EQUITY				716
#define FDD_PIP_SIZE			717
#define FDD_BID					718
#define FDD_ASK					719
#define FDD_SPREAD				720
#define FDD_HIGH_PRC			721
#define FDD_LOW_PRC				722
#define FDD_BALANCE				723
#define FDD_FREE_MGN			724
#define FDD_SL_PT				725
#define FDD_TP_PT				726


	//
	// Must sync with BPProtocol.mqh
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	// Error Code - Must save on MsgCode.ini 
	//

#define ERR_OK						0
#define E_OK						0
#define ERR_UNDEFINED			9999		// Undefined Error Code
#define E_UNDEFINED				9999		// Undefined Error Code


// business
#define E_MASTER_NOT_CONN		2001	//	�����Ͱ� �¶����� �ƴմϴ�.
#define E_NO_EMAIL				2002	//	�̸����� �����ϴ�.
#define E_NO_MASTER_COPIER_LINK	2003	//	������-ī�Ǿ� ���谡 �����ϴ�..
#define E_NOEXIST_USERID		2004	//	USERID �� �����ϴ�.
#define E_NO_MT4ACC				2005	//	MT4 Account �� �����ϴ�.
#define E_USERID_DUP			2006	//	USERID �� �̹� �����մϴ�.
#define	E_CANNOTFIND_ORD		2007	//	�ش� �ֹ��� �����ϴ�.
#define	E_NOT_OUR_ORDTYPE		2008	//	�ֹ�Ÿ�� ���� �Դϴ�.
#define	E_WRONG_PASSWORD		2009	//	��й�ȣ ���� �Դϴ�.
#define	E_NON_ACCEPTABLE_STATUS	2010	//	�α��� �Ұ� �����Դϴ�.
#define	E_NON_ACLLOCATED_SERVER	2011	//	�ش� �����ʹ� ������ �������� �ʾҽ��ϴ�.
#define	E_NON_LOGON				2012	//	�¶��� ���°� �ƴմϴ�.
#define	E_TERMINATE				2013	//	����Ǿ����ϴ�.
#define	E_TIMEOUT				2014	//	Ÿ�Ӿƿ� �Դϴ�.
#define E_UNKNOWN_ERR_CODE		2015	//	���ǵ��� ���� �����ڵ� �Դϴ�.
#define E_NO_SYMBOLS_CONFIG		2016	//	�������� ���� �ϼž� �մϴ�.
#define E_MT4ACC_TAKEN			2017	//	�� MT4 Account �� �̹� ����� �Դϴ�.

// MT4
#define	E_PLACEORDER_MT4		3001	//	MT4 �ֹ� �� ������ �߻��߽��ϴ�.
#define	E_GET_MT4INFO			3002	//	MT4 ��ȸ �� ������ �߻��߽��ϴ�.
#define	E_CNFG_ORDERLOTS		3003	//	���� - �ֹ����� ���� �Դϴ�.
#define	E_CNFG_SYMBOL			3004	//	���� - Symbol ���� �Դϴ�.
#define	E_CNFG_MAXSLIPPAGE		3005	//	���� - �������� ���� �Դϴ�.
#define	E_CNFG_ORDPRICE			3006	//	���� - �ֹ����� ���� �Դϴ�.
#define	E_NOT_READY_TRADE		3007	//	�ֹ� ���� �غ� �ȵƽ��ϴ�.
#define	E_CANNOT_CONN_SVR		3008	//	Alpha ������ ���� ���� �Դϴ�.
#define	E_LOAD_EXTERN_LIB		3009	//	���̺귯�� �ε� ���� �Դϴ�.
#define	E_PUBLISH_OPENORD		3010	//	�����ֹ� Publishing �� �����߽��ϴ�.
#define	E_PUBLISH_CLOSEORD		3011	//	û���ֹ� Publishing �� �����߽��ϴ�.
#define	E_FUNCTION_NOT_PREPARED	3012	//	����� �������� �ʾҽ��ϴ�.
#define	E_ORDERSELECT			3013	//	MT4 �ֹ� ��ȸ �� ���� �Դϴ�.
#define	E_OUTOFINDEX			3014	//	MT4 ���� INDEX �ʰ� ���� �Դϴ�.
#define	E_LOGOUT_FROM_SVR		3015	//	������ ���ؼ� �α׾ƿ� �Ǿ����ϴ�.
#define	E_DISCONN_FROM_SVR		3016	//	�������� ������ �����߽��ϴ�.
#define  E_WRONG_SYMBOL          3017	//	�߸��� �ɺ��Դϴ�.
#define  E_WRONG_TICKET          3018	//	�߸��� TICKET NO. �Դϴ�.
#define  E_WRONG_TICK_INFO	      3019	// �߸��� TICK ���� �Դϴ�.

// socket error
#define E_WSA_STARTUP	1001
#define E_CREATE_SOCK	1002
#define E_CONNECT		1003
#define E_NON_CONNECT	1004
#define E_SEND			1005
#define E_RECV			1006
#define E_MAX_SOCK_ERR	1007
#define E_INVALID_SOCK	1008

// packet error
#define E_NOCODE				1101	//	��Ŷ�� �ڵ尡 �����ϴ�.
#define E_NO_CODE				E_NOCODE
#define E_INVALIDE_MASTERCOPIER	1102	//	��Ŷ�� ������/ī�Ǿ� ������ �߸��ƽ��ϴ�.
#define E_NO_USERID				1103	//	��Ŷ�� USERID �� �����ϴ�.
#define E_INVALID_CODE			1104	//	��Ŷ �ڵ尡 ��ȿ���� �ʽ��ϴ�.
#define E_NO_MASTER_ID			1105	//	��Ŷ�� ������ID �� �����ϴ�.
#define E_NO_ACNTNO				1106	//	��Ŷ�� MT4 ���¹�ȣ�� �����ϴ�.
#define E_NO_MASTER_ACCNO		1107	//	��Ŷ�� �������� MT4 ���¹�ȣ�� �����ϴ�.
#define E_NO_FIELD				1108	//	��Ŷ�� �����Ͱ� �����Ǿ����ϴ�.
#define E_NO_NICK_NAME			1109	//	��Ŷ�� NICK NAME �� �����ϴ�.


// system & db
#define E_SYS_DB_EXCEPTION		1201	//�����ͺ��̽� ó�� �� ���ܰ� �߻��߽��ϴ�.
#define E_READ_CONFIG			1202	//�������� ��ȸ ���� �Դϴ�.
#define E_MSG_FILE				1203	//�޼��� ���� ��ȸ ���� �Դϴ�.
#define E_DB_NOT_OPENED			1204	//DB is not opened yet.	


/*
*	MT4 CMD 
*/
enum {
	ORDER_TYPE_BUY = 0
	, ORDER_TYPE_SELL
	, ORDER_TYPE_BUY_LIMIT
	, ORDER_TYPE_SELL_LIMIT
	, ORDER_TYPE_BUY_STOP
	, ORDER_TYPE_SELL_STOP
	, ORDER_TYPE_BUY_STOP_LIMIT
	, ORDER_TYPE_SELL_STOP_LIMIT
};
//int getMT4Cmd_MarketBuy();
//int getMT4Cmd_MarketSell();
//char* getMT4CmdDesc(int nCmd, char* pOut);
//bool IsBuyOrder(int nCmd);
//char* getBuySellString(int nCmd, char* pOut);
//
//char* enMasterKey(const char* id, const char* accno, char* out);
//void deMasterKey(const char* key, /*out*/ char* id, /*out*/char* accno);
//char* Now(char* pBuf);
//BOOL IsMaster(string sTp);
//int TimeFrameIdx(const char* pzTimeFrame);


}