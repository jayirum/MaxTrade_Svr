unit PacketStruct;

interface

type
	// Header Size(42)
	TTRHeader = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // 패킷코드
    USER_ID       : array[0..19] of Char;     // 사용자ID
    ACNT_TP       : array[0..0]  of Char;     // 0:전체, 1:국내파생, 2:해외파생, 3:국내현물
    ERR_CODE      : array[0..3]  of Char;     // 성공:0000
    TM            : array[0..8]  of Char;     // Send Time HHNNSSZZZ
  end;

	// Real, LOG Packet TP(15)
	TThinHeader = Packed Record
    PACKET_CD     : array[0..4]  of Char;     // Real, Log Packet TP [R]
  	STK_CD        : array[0..9]  of Char;     // 종목코드
    ACNT_TP       : array[0..0]  of Char;     // 0:전체, 1:국내파생, 2:해외파생, 3:국내현물
  end;

  // POLLING
	TPC000 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    CLIENT_IP     : array[0..14]  of Char;    // Client IP
  end;

  // LOGIN
	TPC001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    USER_TP       : array[0..0]  of Char;     // C: Client / M: Manager / S: System
    DUP_YN        : array[0..0]  of Char;     // 중복로그인 Y: 가능 / N: 불가
    MENTOR_ID     : array[0..19] of Char;     // 멘토ID
  end;

  // 분배서버
	TPC002 = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // 패킷코드
    USER_ID       : array[0..19] of Char;     // 회원ID
    USER_PWD      : array[0..14] of Char;     // 회원PassWord
    USER_MACADD   : array[0..19] of Char;     // 맥어드레스
    HTS_VER       : array[0..9]  of Char;     // HTS Version
  end;

  // Real Server 종목변경
	TPC003 = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // 패킷코드
    USER_ID       : array[0..19] of Char;     // 회원ID
    STK_CD1       : array[0..9]  of Char;     // 종목코드1
    STK_CD2       : array[0..9]  of Char;     // 종목코드2
    STK_CD3       : array[0..9]  of Char;     // 종목코드3
  end;

  // 분배서버 리턴
	TPS001 = Packed Record
    LENGTH        : array[0..2]   of Char;    // Header + Data Length
    PACKET_CD     : array[0..4]   of Char;    // 패킷코드
    USER_ID       : array[0..19]  of Char;    // 회원ID
    SERVER_IP     : array[0..14]  of Char;    // 접속서IP
    USER_MSG      : array[0..49]  of Char;    // 메세지
  end;

  // Client LOG (Client -> G_F_LOG)
  TPC900 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
    ACNT_NO				: array[0..10]   of Char;	  // 계좌번호
    LOG_CLT_DT		: array[0..7]    of Char;   // 클라이언트 일자 YYYYMMDD
    LOG_CLT_TM		: array[0..11]   of Char;   // 클라이언트 시간 HH:NN:SS:MMM
    LOG_IP				: array[0..15]   of Char;   // 클라이언트IP
    USER_ID				: array[0..19]   of Char;   // 회원ID
    USER_NM				: array[0..19]   of Char;   // 회원명
    LOG_TP				: array[0..2]    of Char;   // 로그유형 CODE_MST참조
    BS_TP					: array[0..0]    of Char;   // 매도/매수구분 S:매도 B:매수
    NOW_PRC				: array[0..14]   of Char;   // 현재가
    ORD_PRC				: array[0..14]   of Char;   // 주문가
    ORD_QTY				: array[0..8]    of Char;   // 주문수량
    HOGA_NO				: array[0..1]    of Char;   // 호가
    LOG_MSG				: array[0..49]   of Char;   // 로그내용
  end;

  // 서버재실행
	TPM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    MACHINE_TP    : array[0..0]  of Char;     // 머신명 F: FRONT / B: BIZ
    SERVER_NM     : array[0..19] of Char;     // 서버명 G_F_REAL_F / G_F_REA_GF
  end;

  // 주문요청
	TTC001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    STK_CD        : array[0..9]  of Char;     // 종목코드
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
		ACNT_PWD      : array[0..3]  of Char;     // 계좌 비밀번호
		ORD_NO        : array[0..8]  of Char;     // 주문번호
		BS_TP         : array[0..0]  of Char;     // 매도매수구분  S:매도 B:매수
		ORD_TP        : array[0..1]  of Char;     // 주문유형 CODE_MST참조
		COND_TP       : array[0..0]  of Char;     // 조건주문 0:default, 1:MIT설정 2:MIT발동 3:SL발동
		ORD_PRC       : array[0..14] of Char;     // 주문가격
		ORD_QTY       : array[0..8]  of Char;     // 주문수량
		MIT_PRC       : array[0..14] of Char;     // MIT설정가격
		SL_PRC        : array[0..14] of Char;     // SL설정가격
		SL_TP         : array[0..0]  of Char;     // S:손절 P:익절
		ORG_ORD_NO    : array[0..8]  of Char;     // 원주문번호
		ORG_ORD_PRC   : array[0..14] of Char;     // 원주문금액
		MNG_ID        : array[0..19] of Char;     // CLIENT/LOSSCUT/BATCH/MNG_ID(강제취소/청산)
		CLIENT_IP     : array[0..14] of Char;     // Client IP
    API_SEQ       : array[0..2]  of Char;     // 1~199
    API_ORG_NO    : array[0..19] of Char;     // API원주문번호
    HOGA_NO       : array[0..1]  of Char;     // 주문시호가 -1:반대1호가범위 0:1호가사이범위, 1~5:호가 6:5호가이상범위
    SAME_HOGA_YN  : array[0..0]  of Char;     // 동시호가주문여부
		NOW_PRC       : array[0..14] of Char;     // 주문시 현재가
		MIT_TICKS     : array[0..2]  of Char;     // MIT틱
  end;

  // 주문요청
	TTC002 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    STK_CD        : array[0..9]  of Char;     // 종목코드
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
		ACNT_PWD      : array[0..3]  of Char;     // 계좌 비밀번호
		BS_TP         : array[0..0]  of Char;     // 매도매수구분  S:매도 B:매수
		ORD_TP        : array[0..1]  of Char;     // 주문유형 CODE_MST참조
		COND_TP       : array[0..0]  of Char;     // 조건주문
		MNG_ID        : array[0..19] of Char;     // CLIENT/LOSSCUT/BATCH/MNG_ID(강제취소/청산)
		CLIENT_IP     : array[0..14] of Char;     // Client IP
  end;

  // Leverage 변경 요청
	TTC003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
		LEVERAGE      : array[0..2] of Char;      // 변경할 레버리지
  end;

  // 주문체결
  TTA001 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    TR_CD         : array[0..0]  of Char;     // 1:신규주문, 2:정정, 3:취소, 4:체결, 5:거부
    API_TP        : array[0..0]  of Char;     // R:실체결, V:가상체결
    STK_CD        : array[0..9]  of Char;     // 종목코드
    API_ORD_NO    : array[0..19] of Char;     // API주문번호
    API_CNTR_NO   : array[0..19] of Char;     // API체결번호
    ORD_NO        : array[0..8]  of Char;     // 주문번호
    ORD_PRC       : array[0..14] of Char;     // 주문가격
    ORD_QTY       : array[0..8]  of Char;     // 주문수량
    CNTR_PRC      : array[0..14] of Char;     // 체결가격
    CNTR_QTY      : array[0..8]  of Char;     // 체결수량
    REMN_QTY      : array[0..8]  of Char;     // 주문잔량
    API_RSLT_CD   : array[0..3]  of Char;     // API 결과코드
    API_MSG       : array[0..79] of Char;     // API 메세지
    API_DT        : array[0..7]  of Char;     // API 일자
    API_TM        : array[0..5]  of Char;     // API 시각
    API_ORG_NO    : array[0..19] of Char;     // API 원주문번호
    API_RJCT_CODE : array[0..3]  of Char;     // API 거부코드
  end;

  // API주문번호 SEQ
  TTA002 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    API_SEQ       : array[0..2]  of Char;     // 주문번호 1~199
    API_ORD_NO    : array[0..19]  of Char;    // API주문번호
    API_ORG_NO    : array[0..19]  of Char;    // API원주문번호 - 정정, 취소 때만
    API_MSG       : array[0..79] of Char;     // 출력메세지
  End;

  // 주문체결 CLIENT통보
  TTB001 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
    STK_CD        : array[0..9 ] of Char;     // 종목코드
    TR_CD         : array[0..0]  of Char;     // 1:신규주문, 2:정정, 3:취소, 4:체결, 5:거부
    ORD_NO        : array[0..8]  of Char;     // 주문번호
    ORG_ORD_NO    : array[0..8]  of Char;     // 원주문번호
    CNTR_NO       : array[0..8]  of Char;     // 체결번호
    ORD_TP        : array[0..1]  of Char;     // 주문유형 CODE_MST참조
		COND_TP       : array[0..0]  of Char;     // 조건주문 0:default, 1:MIT설정 2:MIT발동 3:SL발동
    BS_TP         : array[0..0]  of Char;     // 매도매수구분  S:매도 B:매수
    ORD_QTY       : array[0..8]  of Char;     // 주문수량
    ORD_PRC       : array[0..14] of Char;     // 주문가격
    CNTR_QTY      : array[0..8]  of Char;     // 체결수량
    CNTR_PRC      : array[0..14] of Char;     // 체결가격
    REMN_QTY      : array[0..8]  of Char;     // 주문잔량
    CLR_PL        : array[0..14] of Char;     // 체결인 경우 체결손익
    CMSN          : array[0..14] of Char;     // 체결인 경우 수수료
    AF_CLR_PL	    : array[0..14] of Char;  	  // 거래 후 실현손익
    AF_CMSN  	    : array[0..14] of Char;	    // 거래 후 수수료
    AF_NCLR_QTY	  : array[0..8]  of Char;     // 거래 후 미청산잔고합
    AF_AVG_PRC	  : array[0..14] of Char;     // 거래 후 평균단가
    AF_BS_TP	    : array[0..0]  of Char;  	  // 거래 후 잔고 매도매수 구분
    AF_CAN_ORD_S  : array[0..8]  of Char;     // 거래 후 매도가능수량
    AF_CAN_ORD_B  : array[0..8]  of Char;     // 거래 후 매수가능수량
    ORD_TM        : array[0..7]  of Char;     // 주문, 체결시간
    API_TP        : array[0..0]  of Char;     // R:실체결, V:가상체결
		MIT_PRC       : array[0..14] of Char;     // MIT설정가격
		MIT_TICKS     : array[0..2]  of Char;     // MIT틱
    SAME_HOGA_YN	: array[0..0]  of Char;			// 동시호가여부
    ORG_ORD_PRC   : array[0..14] of Char;     // 정전전 주문가격
  end;

  // 주문체결 API_TP통보
  TTB002 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ORD_NO        : array[0..10]  of Char;     // 주문번호
    API_TP        : array[0..0]  of Char;     // R:실체결, V:가상체결
  End;

  // 레버리지변경요청에 대한 결과 통보
  TTB003 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10]  of Char;    // 계좌번호
    LEVERAGE     	: array[0..2]  of Char;     // 성공시 새로운 비율. 실패시 이전비율
    MSG					  : ARRAY[0..70] OF CHAR;			// 메세지 (성공/실패)
  End;


  // NL 패킷은 MNG_YN 기본적으로 추가해야 함.
	TNL_HEADER = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    MNG_YN        : array[0..0]  of Char;     // 관리자통보여부 Y:유 N:무
  end;

  // 갱신 통보
	TNR001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
  end;

  // BATCH 통보
	TNL001 = Packed Record
  	GTN_HEADER    : TNL_HEADER;               // Header
    ARTC_CD       : array[0..5]  of Char;     // 품목코드
    NOTI_MSG      : array[0..29] of Char;     // 메세지 내려감
    REFRESH_YN    : array[0..0]  of Char;     // 클라이언트 갱신여부
  end;

  // LOSSCUST 통보
	TNL002 = Packed Record
  	GTN_HEADER    : TNL_HEADER;               // Header
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
    ACNT_AMT      : array[0..14] of Char;     // 로스컷당시 잔액
    CLR_PL        : array[0..14] of Char;     // 청산손익
    CMSN          : array[0..14] of Char;     // 수수료
    ESTM_PL       : array[0..14] of Char;     // 평가손익
    ESTM_ACNT_AMT : array[0..14] of Char;     // 평가잔액
    LOSSCUT_TM    : array[0..5]  of Char;     // 로스컷시간
  end;

  // 입출금 결과 통보
	TNM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_MSG      : array[0..49] of Char;     // 입출금메세지 (관리자에서 메세지 생성)
    REFRESH_YN    : array[0..0]  of Char;     // 클라이언트 갱신여부
  end;

  // 공지사항 ※전체:HEADER.USERID = '@ALLUSER'
	TNM002 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_TP       : array[0..1]   of Char;    // ACTION 00:공지사항 01:통보
                                              //        98:중복로그인 (G_F_LOGIN에서 패킷생성)
                                              //        99:강제종료
    NOTI_TITEL    : array[0..49]  of Char;    // TITLE
    NOTI_DATA     : array[0..999] of Char;    // 메세지
  end;

  // 오버나잇 결과 통보
	TNM003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_MSG      : array[0..49] of Char;     // 오버나잇메세지 (관리자에서 메세지 생성)
    REFRESH_YN    : array[0..0]  of Char;     // 클라이언트 갱신여부
  end;

  // 입출금요청
	TNC001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    USER_ID       : array[0..19] of Char;     // 보낸 USERID
    IO_TP         : array[0..0]  of Char;     // 요청구분
    REQ_AMT       : array[0..14] of Char;     // 요청금액
    ACNT_NO       : array[0..10] of Char;     // 계좌번호
    NOTI_MSG      : array[0..99] of Char;     // 입출금메세지 (CLIENT에서 메세지 생성)
  end;

  // 오버나잇신청 통보
  TNC002 = packed Record
    GT_HEADER     : TTRHeader;                // Header
    ARTC_CD       : array[0..5]  of Char;     // 품목코드
    OVERNGT_YN    : array[0..0]  of Char;     // 요청구분
    NOTI_MSG      : array[0..99] of Char;     // 통보메세지 (CLIENT에서 메세지 생성)
  End;

  // HTS -> 관리자 통보 (신규가입 통보)
	TNC003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    USER_ID       : array[0..19] of Char;     // 보낸 USERID
    NOTI_TP       : array[0..1]  of Char;     // 통보구분 (01: 신규가입통보)
    NOTI_TITLE    : array[0..19] of Char;     // 통보타이틀 (CLIENT에서 메세지 생성)
    NOTI_MSG      : array[0..99] of Char;     // 통보메세지 (CLIENT에서 메세지 생성)
  end;

  // 리커버리 관리자 통보
	TNB001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_TP       : array[0..1]  of Char;     // 통보구분
    NOTI_MSG      : array[0..99] of Char;     // 리커버리 에러통보 (관리자에서 메세지 생성)
    REFRESH_YN    : array[0..1]  of Char;     // 클라이언트 갱신여부
  end;

  // Real 시세
	TRA001 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
  	TM            : array[0..5]  of Char;     // 체결시간HHMMSS
    KP200         : array[0..9]  of Char;     // KP200
    BS_TP         : array[0..0]  of Char;     // 체결구분
  	NOW_PRC       : array[0..9]  of Char;     // 현재가
  	CHG           : array[0..9]  of Char;     // 전일대비
  	ACML_CNTR_QTY : array[0..8]  of Char;     // 누적거래량(단위계약)
  	ACML_AMT      : array[0..14] of Char;     // 누적거래대금(단위억원)
  	OPEN_PRC      : array[0..9]  of Char;     // 시가
  	HIGH_PRC      : array[0..9]  of Char;     // 고가
  	LOW_PRC       : array[0..9]  of Char;     // 저가
  	CNTR_QTY      : array[0..8]  of Char;     // 체결량(계약)
  	FLUC_RT       : array[0..4]  of Char;     // 등락율
  	MKT_BASIS     : array[0..4]  of Char;     // 시장BASIS
  	NCNTR_FLUC    : array[0..8]  of Char;     // 미결체약정증감
    NCNTR_CNT     : array[0..8]  of Char;     // 미결제약정수량(단위계약)
  end;

  // Real 호가
	TRA002 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
    TM            : array[0..5]  of Char;     // 수신시간
    S_RMNQ_1      : array[0..5]  of Char;     // 매도호가수량1
    S_RMNQ_2      : array[0..5]  of Char;     // 매도호가수량2
    S_RMNQ_3      : array[0..5]  of Char;     // 매도호가수량3
    S_RMNQ_4      : array[0..5]  of Char;     // 매도호가수량4
    S_RMNQ_5      : array[0..5]  of Char;     // 매도호가수량5
    S_HOGA_1      : array[0..9]  of Char;     // 매도호가1
    S_HOGA_2      : array[0..9]  of Char;     // 매도호가2
    S_HOGA_3      : array[0..9]  of Char;     // 매도호가3
    S_HOGA_4      : array[0..9]  of Char;     // 매도호가4
    S_HOGA_5      : array[0..9]  of Char;     // 매도호가5
    S_CNT_1       : array[0..4]  of Char;     // 매도호가건수1
    S_CNT_2       : array[0..4]  of Char;     // 매도호가건수2
    S_CNT_3       : array[0..4]  of Char;     // 매도호가건수3
    S_CNT_4       : array[0..4]  of Char;     // 매도호가건수4
    S_CNT_5       : array[0..4]  of Char;     // 매도호가건수5
    B_RMNQ_1      : array[0..5]  of Char;     // 매수호가수량1
    B_RMNQ_2      : array[0..5]  of Char;     // 매수호가수량2
    B_RMNQ_3      : array[0..5]  of Char;     // 매수호가수량3
    B_RMNQ_4      : array[0..5]  of Char;     // 매수호가수량4
    B_RMNQ_5      : array[0..5]  of Char;     // 매수호가수량5
    B_HOGA_1      : array[0..9]  of Char;     // 매수호가1
    B_HOGA_2      : array[0..9]  of Char;     // 매수호가2
    B_HOGA_3      : array[0..9]  of Char;     // 매수호가3
    B_HOGA_4      : array[0..9]  of Char;     // 매수호가4
    B_HOGA_5      : array[0..9]  of Char;     // 매수호가5
    B_CNT_1       : array[0..4]  of Char;     // 매수호가건수1
    B_CNT_2       : array[0..4]  of Char;     // 매수호가건수2
    B_CNT_3       : array[0..4]  of Char;     // 매수호가건수3
    B_CNT_4       : array[0..4]  of Char;     // 매수호가건수4
    B_CNT_5       : array[0..4]  of Char;     // 매수호가건수5
    S_TOT_RMNQ    : array[0..8]  of Char;     // 매도호가총수량
    S_TOT_CNT     : array[0..8]  of Char;     // 매도호가총건수
    B_TOT_RMNQ    : array[0..8]  of Char;     // 매수호가총수량
    B_TOT_CNT     : array[0..8]  of Char;     // 매수호가총건수
    EXCE_RMNQ     : array[0..8]  of Char;     // 초과잔량(매수잔량-매도잔량)
  end;

	TRA003 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
  	TM            : array[0..5]  of Char;     // 체결시간HHMMSS
    BS_TP         : array[0..0]  of Char;     // 체결구분
  	NOW_PRC       : array[0..9]  of Char;     // 현재가
  	CHG           : array[0..9]  of Char;     // 전일대비
  	OPEN_PRC      : array[0..9]  of Char;     // 시가
  	HIGH_PRC      : array[0..9]  of Char;     // 고가
  	LOW_PRC       : array[0..9]  of Char;     // 저가
  	CNTR_QTY      : array[0..8]  of Char;     // 체결량(계약)
    S_HOGA_1      : array[0..9]  of Char;     // 매도호가1
    S_HOGA_2      : array[0..9]  of Char;     // 매도호가2
    B_HOGA_1      : array[0..9]  of Char;     // 매수호가1
    B_HOGA_2      : array[0..9]  of Char;     // 매수호가2
    S_TOT_RMNQ    : array[0..8]  of Char;     // 매도호가총수량
    S_TOT_CNT     : array[0..8]  of Char;     // 매도호가총건수
    B_TOT_RMNQ    : array[0..8]  of Char;     // 매수호가총수량
    B_TOT_CNT     : array[0..8]  of Char;     // 매수호가총건수
    EXCE_RMNQ     : array[0..8]  of Char;     // 초과잔량(매수잔량-매도잔량)
  end;

  // API 조회 요청
	TQL001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // 조회코드 001:종목조회 051:미청산조회
  end;

  // API 조회 요청
	TQM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // 조회코드 101:전체조회(예탁금,잔고,미체결)
  end;

  // API 결과
	TQA001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // 조회코드 001:종목조회 051:미청산조회
                                              // 101:전체조회(예탁금,잔고,미체결)-관리자조회
    RESULT_YN     : array[0..0]  of Char;     // 처리결과여부
  end;

implementation

end.