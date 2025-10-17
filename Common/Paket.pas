unit PacketStruct;

interface

type
	// Header Size(42)
	TTRHeader = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // ��Ŷ�ڵ�
    USER_ID       : array[0..19] of Char;     // �����ID
    ACNT_TP       : array[0..0]  of Char;     // 0:��ü, 1:�����Ļ�, 2:�ؿ��Ļ�, 3:��������
    ERR_CODE      : array[0..3]  of Char;     // ����:0000
    TM            : array[0..8]  of Char;     // Send Time HHNNSSZZZ
  end;

	// Real, LOG Packet TP(15)
	TThinHeader = Packed Record
    PACKET_CD     : array[0..4]  of Char;     // Real, Log Packet TP [R]
  	STK_CD        : array[0..9]  of Char;     // �����ڵ�
    ACNT_TP       : array[0..0]  of Char;     // 0:��ü, 1:�����Ļ�, 2:�ؿ��Ļ�, 3:��������
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
    DUP_YN        : array[0..0]  of Char;     // �ߺ��α��� Y: ���� / N: �Ұ�
    MENTOR_ID     : array[0..19] of Char;     // ����ID
  end;

  // �й輭��
	TPC002 = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // ��Ŷ�ڵ�
    USER_ID       : array[0..19] of Char;     // ȸ��ID
    USER_PWD      : array[0..14] of Char;     // ȸ��PassWord
    USER_MACADD   : array[0..19] of Char;     // �ƾ�巹��
    HTS_VER       : array[0..9]  of Char;     // HTS Version
  end;

  // Real Server ���񺯰�
	TPC003 = Packed Record
    LENGTH        : array[0..2]  of Char;     // Header + Data Length
    PACKET_CD     : array[0..4]  of Char;     // ��Ŷ�ڵ�
    USER_ID       : array[0..19] of Char;     // ȸ��ID
    STK_CD1       : array[0..9]  of Char;     // �����ڵ�1
    STK_CD2       : array[0..9]  of Char;     // �����ڵ�2
    STK_CD3       : array[0..9]  of Char;     // �����ڵ�3
  end;

  // �й輭�� ����
	TPS001 = Packed Record
    LENGTH        : array[0..2]   of Char;    // Header + Data Length
    PACKET_CD     : array[0..4]   of Char;    // ��Ŷ�ڵ�
    USER_ID       : array[0..19]  of Char;    // ȸ��ID
    SERVER_IP     : array[0..14]  of Char;    // ���Ӽ�IP
    USER_MSG      : array[0..49]  of Char;    // �޼���
  end;

  // Client LOG (Client -> G_F_LOG)
  TPC900 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
    ACNT_NO				: array[0..10]   of Char;	  // ���¹�ȣ
    LOG_CLT_DT		: array[0..7]    of Char;   // Ŭ���̾�Ʈ ���� YYYYMMDD
    LOG_CLT_TM		: array[0..11]   of Char;   // Ŭ���̾�Ʈ �ð� HH:NN:SS:MMM
    LOG_IP				: array[0..15]   of Char;   // Ŭ���̾�ƮIP
    USER_ID				: array[0..19]   of Char;   // ȸ��ID
    USER_NM				: array[0..19]   of Char;   // ȸ����
    LOG_TP				: array[0..2]    of Char;   // �α����� CODE_MST����
    BS_TP					: array[0..0]    of Char;   // �ŵ�/�ż����� S:�ŵ� B:�ż�
    NOW_PRC				: array[0..14]   of Char;   // ���簡
    ORD_PRC				: array[0..14]   of Char;   // �ֹ���
    ORD_QTY				: array[0..8]    of Char;   // �ֹ�����
    HOGA_NO				: array[0..1]    of Char;   // ȣ��
    LOG_MSG				: array[0..49]   of Char;   // �α׳���
  end;

  // ���������
	TPM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    MACHINE_TP    : array[0..0]  of Char;     // �ӽŸ� F: FRONT / B: BIZ
    SERVER_NM     : array[0..19] of Char;     // ������ G_F_REAL_F / G_F_REA_GF
  end;

  // �ֹ���û
	TTC001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    STK_CD        : array[0..9]  of Char;     // �����ڵ�
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
		ACNT_PWD      : array[0..3]  of Char;     // ���� ��й�ȣ
		ORD_NO        : array[0..8]  of Char;     // �ֹ���ȣ
		BS_TP         : array[0..0]  of Char;     // �ŵ��ż�����  S:�ŵ� B:�ż�
		ORD_TP        : array[0..1]  of Char;     // �ֹ����� CODE_MST����
		COND_TP       : array[0..0]  of Char;     // �����ֹ� 0:default, 1:MIT���� 2:MIT�ߵ� 3:SL�ߵ�
		ORD_PRC       : array[0..14] of Char;     // �ֹ�����
		ORD_QTY       : array[0..8]  of Char;     // �ֹ�����
		MIT_PRC       : array[0..14] of Char;     // MIT��������
		SL_PRC        : array[0..14] of Char;     // SL��������
		SL_TP         : array[0..0]  of Char;     // S:���� P:����
		ORG_ORD_NO    : array[0..8]  of Char;     // ���ֹ���ȣ
		ORG_ORD_PRC   : array[0..14] of Char;     // ���ֹ��ݾ�
		MNG_ID        : array[0..19] of Char;     // CLIENT/LOSSCUT/BATCH/MNG_ID(�������/û��)
		CLIENT_IP     : array[0..14] of Char;     // Client IP
    API_SEQ       : array[0..2]  of Char;     // 1~199
    API_ORG_NO    : array[0..19] of Char;     // API���ֹ���ȣ
    HOGA_NO       : array[0..1]  of Char;     // �ֹ���ȣ�� -1:�ݴ�1ȣ������ 0:1ȣ�����̹���, 1~5:ȣ�� 6:5ȣ���̻����
    SAME_HOGA_YN  : array[0..0]  of Char;     // ����ȣ���ֹ�����
		NOW_PRC       : array[0..14] of Char;     // �ֹ��� ���簡
		MIT_TICKS     : array[0..2]  of Char;     // MITƽ
  end;

  // �ֹ���û
	TTC002 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    STK_CD        : array[0..9]  of Char;     // �����ڵ�
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
		ACNT_PWD      : array[0..3]  of Char;     // ���� ��й�ȣ
		BS_TP         : array[0..0]  of Char;     // �ŵ��ż�����  S:�ŵ� B:�ż�
		ORD_TP        : array[0..1]  of Char;     // �ֹ����� CODE_MST����
		COND_TP       : array[0..0]  of Char;     // �����ֹ�
		MNG_ID        : array[0..19] of Char;     // CLIENT/LOSSCUT/BATCH/MNG_ID(�������/û��)
		CLIENT_IP     : array[0..14] of Char;     // Client IP
  end;

  // Leverage ���� ��û
	TTC003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
		LEVERAGE      : array[0..2] of Char;      // ������ ��������
  end;

  // �ֹ�ü��
  TTA001 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    TR_CD         : array[0..0]  of Char;     // 1:�ű��ֹ�, 2:����, 3:���, 4:ü��, 5:�ź�
    API_TP        : array[0..0]  of Char;     // R:��ü��, V:����ü��
    STK_CD        : array[0..9]  of Char;     // �����ڵ�
    API_ORD_NO    : array[0..19] of Char;     // API�ֹ���ȣ
    API_CNTR_NO   : array[0..19] of Char;     // APIü���ȣ
    ORD_NO        : array[0..8]  of Char;     // �ֹ���ȣ
    ORD_PRC       : array[0..14] of Char;     // �ֹ�����
    ORD_QTY       : array[0..8]  of Char;     // �ֹ�����
    CNTR_PRC      : array[0..14] of Char;     // ü�ᰡ��
    CNTR_QTY      : array[0..8]  of Char;     // ü�����
    REMN_QTY      : array[0..8]  of Char;     // �ֹ��ܷ�
    API_RSLT_CD   : array[0..3]  of Char;     // API ����ڵ�
    API_MSG       : array[0..79] of Char;     // API �޼���
    API_DT        : array[0..7]  of Char;     // API ����
    API_TM        : array[0..5]  of Char;     // API �ð�
    API_ORG_NO    : array[0..19] of Char;     // API ���ֹ���ȣ
    API_RJCT_CODE : array[0..3]  of Char;     // API �ź��ڵ�
  end;

  // API�ֹ���ȣ SEQ
  TTA002 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    API_SEQ       : array[0..2]  of Char;     // �ֹ���ȣ 1~199
    API_ORD_NO    : array[0..19]  of Char;    // API�ֹ���ȣ
    API_ORG_NO    : array[0..19]  of Char;    // API���ֹ���ȣ - ����, ��� ����
    API_MSG       : array[0..79] of Char;     // ��¸޼���
  End;

  // �ֹ�ü�� CLIENT�뺸
  TTB001 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
    STK_CD        : array[0..9 ] of Char;     // �����ڵ�
    TR_CD         : array[0..0]  of Char;     // 1:�ű��ֹ�, 2:����, 3:���, 4:ü��, 5:�ź�
    ORD_NO        : array[0..8]  of Char;     // �ֹ���ȣ
    ORG_ORD_NO    : array[0..8]  of Char;     // ���ֹ���ȣ
    CNTR_NO       : array[0..8]  of Char;     // ü���ȣ
    ORD_TP        : array[0..1]  of Char;     // �ֹ����� CODE_MST����
		COND_TP       : array[0..0]  of Char;     // �����ֹ� 0:default, 1:MIT���� 2:MIT�ߵ� 3:SL�ߵ�
    BS_TP         : array[0..0]  of Char;     // �ŵ��ż�����  S:�ŵ� B:�ż�
    ORD_QTY       : array[0..8]  of Char;     // �ֹ�����
    ORD_PRC       : array[0..14] of Char;     // �ֹ�����
    CNTR_QTY      : array[0..8]  of Char;     // ü�����
    CNTR_PRC      : array[0..14] of Char;     // ü�ᰡ��
    REMN_QTY      : array[0..8]  of Char;     // �ֹ��ܷ�
    CLR_PL        : array[0..14] of Char;     // ü���� ��� ü�����
    CMSN          : array[0..14] of Char;     // ü���� ��� ������
    AF_CLR_PL	    : array[0..14] of Char;  	  // �ŷ� �� ��������
    AF_CMSN  	    : array[0..14] of Char;	    // �ŷ� �� ������
    AF_NCLR_QTY	  : array[0..8]  of Char;     // �ŷ� �� ��û���ܰ���
    AF_AVG_PRC	  : array[0..14] of Char;     // �ŷ� �� ��մܰ�
    AF_BS_TP	    : array[0..0]  of Char;  	  // �ŷ� �� �ܰ� �ŵ��ż� ����
    AF_CAN_ORD_S  : array[0..8]  of Char;     // �ŷ� �� �ŵ����ɼ���
    AF_CAN_ORD_B  : array[0..8]  of Char;     // �ŷ� �� �ż����ɼ���
    ORD_TM        : array[0..7]  of Char;     // �ֹ�, ü��ð�
    API_TP        : array[0..0]  of Char;     // R:��ü��, V:����ü��
		MIT_PRC       : array[0..14] of Char;     // MIT��������
		MIT_TICKS     : array[0..2]  of Char;     // MITƽ
    SAME_HOGA_YN	: array[0..0]  of Char;			// ����ȣ������
    ORG_ORD_PRC   : array[0..14] of Char;     // ������ �ֹ�����
  end;

  // �ֹ�ü�� API_TP�뺸
  TTB002 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ORD_NO        : array[0..10]  of Char;     // �ֹ���ȣ
    API_TP        : array[0..0]  of Char;     // R:��ü��, V:����ü��
  End;

  // �������������û�� ���� ��� �뺸
  TTB003 = Packed Record
    GT_HEADER     : TTRHeader;                // Header
    ACNT_NO       : array[0..10]  of Char;    // ���¹�ȣ
    LEVERAGE     	: array[0..2]  of Char;     // ������ ���ο� ����. ���н� ��������
    MSG					  : ARRAY[0..70] OF CHAR;			// �޼��� (����/����)
  End;


  // NL ��Ŷ�� MNG_YN �⺻������ �߰��ؾ� ��.
	TNL_HEADER = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    MNG_YN        : array[0..0]  of Char;     // �������뺸���� Y:�� N:��
  end;

  // ���� �뺸
	TNR001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
  end;

  // BATCH �뺸
	TNL001 = Packed Record
  	GTN_HEADER    : TNL_HEADER;               // Header
    ARTC_CD       : array[0..5]  of Char;     // ǰ���ڵ�
    NOTI_MSG      : array[0..29] of Char;     // �޼��� ������
    REFRESH_YN    : array[0..0]  of Char;     // Ŭ���̾�Ʈ ���ſ���
  end;

  // LOSSCUST �뺸
	TNL002 = Packed Record
  	GTN_HEADER    : TNL_HEADER;               // Header
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
    ACNT_AMT      : array[0..14] of Char;     // �ν��ƴ�� �ܾ�
    CLR_PL        : array[0..14] of Char;     // û�����
    CMSN          : array[0..14] of Char;     // ������
    ESTM_PL       : array[0..14] of Char;     // �򰡼���
    ESTM_ACNT_AMT : array[0..14] of Char;     // ���ܾ�
    LOSSCUT_TM    : array[0..5]  of Char;     // �ν��ƽð�
  end;

  // ����� ��� �뺸
	TNM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_MSG      : array[0..49] of Char;     // ����ݸ޼��� (�����ڿ��� �޼��� ����)
    REFRESH_YN    : array[0..0]  of Char;     // Ŭ���̾�Ʈ ���ſ���
  end;

  // �������� ����ü:HEADER.USERID = '@ALLUSER'
	TNM002 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_TP       : array[0..1]   of Char;    // ACTION 00:�������� 01:�뺸
                                              //        98:�ߺ��α��� (G_F_LOGIN���� ��Ŷ����)
                                              //        99:��������
    NOTI_TITEL    : array[0..49]  of Char;    // TITLE
    NOTI_DATA     : array[0..999] of Char;    // �޼���
  end;

  // �������� ��� �뺸
	TNM003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_MSG      : array[0..49] of Char;     // �������ո޼��� (�����ڿ��� �޼��� ����)
    REFRESH_YN    : array[0..0]  of Char;     // Ŭ���̾�Ʈ ���ſ���
  end;

  // ����ݿ�û
	TNC001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    USER_ID       : array[0..19] of Char;     // ���� USERID
    IO_TP         : array[0..0]  of Char;     // ��û����
    REQ_AMT       : array[0..14] of Char;     // ��û�ݾ�
    ACNT_NO       : array[0..10] of Char;     // ���¹�ȣ
    NOTI_MSG      : array[0..99] of Char;     // ����ݸ޼��� (CLIENT���� �޼��� ����)
  end;

  // �������ս�û �뺸
  TNC002 = packed Record
    GT_HEADER     : TTRHeader;                // Header
    ARTC_CD       : array[0..5]  of Char;     // ǰ���ڵ�
    OVERNGT_YN    : array[0..0]  of Char;     // ��û����
    NOTI_MSG      : array[0..99] of Char;     // �뺸�޼��� (CLIENT���� �޼��� ����)
  End;

  // HTS -> ������ �뺸 (�ű԰��� �뺸)
	TNC003 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    USER_ID       : array[0..19] of Char;     // ���� USERID
    NOTI_TP       : array[0..1]  of Char;     // �뺸���� (01: �ű԰����뺸)
    NOTI_TITLE    : array[0..19] of Char;     // �뺸Ÿ��Ʋ (CLIENT���� �޼��� ����)
    NOTI_MSG      : array[0..99] of Char;     // �뺸�޼��� (CLIENT���� �޼��� ����)
  end;

  // ��Ŀ���� ������ �뺸
	TNB001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    NOTI_TP       : array[0..1]  of Char;     // �뺸����
    NOTI_MSG      : array[0..99] of Char;     // ��Ŀ���� �����뺸 (�����ڿ��� �޼��� ����)
    REFRESH_YN    : array[0..1]  of Char;     // Ŭ���̾�Ʈ ���ſ���
  end;

  // Real �ü�
	TRA001 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
  	TM            : array[0..5]  of Char;     // ü��ð�HHMMSS
    KP200         : array[0..9]  of Char;     // KP200
    BS_TP         : array[0..0]  of Char;     // ü�ᱸ��
  	NOW_PRC       : array[0..9]  of Char;     // ���簡
  	CHG           : array[0..9]  of Char;     // ���ϴ��
  	ACML_CNTR_QTY : array[0..8]  of Char;     // �����ŷ���(�������)
  	ACML_AMT      : array[0..14] of Char;     // �����ŷ����(�������)
  	OPEN_PRC      : array[0..9]  of Char;     // �ð�
  	HIGH_PRC      : array[0..9]  of Char;     // ��
  	LOW_PRC       : array[0..9]  of Char;     // ����
  	CNTR_QTY      : array[0..8]  of Char;     // ü�ᷮ(���)
  	FLUC_RT       : array[0..4]  of Char;     // �����
  	MKT_BASIS     : array[0..4]  of Char;     // ����BASIS
  	NCNTR_FLUC    : array[0..8]  of Char;     // �̰�ü��������
    NCNTR_CNT     : array[0..8]  of Char;     // �̰�����������(�������)
  end;

  // Real ȣ��
	TRA002 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
    TM            : array[0..5]  of Char;     // ���Žð�
    S_RMNQ_1      : array[0..5]  of Char;     // �ŵ�ȣ������1
    S_RMNQ_2      : array[0..5]  of Char;     // �ŵ�ȣ������2
    S_RMNQ_3      : array[0..5]  of Char;     // �ŵ�ȣ������3
    S_RMNQ_4      : array[0..5]  of Char;     // �ŵ�ȣ������4
    S_RMNQ_5      : array[0..5]  of Char;     // �ŵ�ȣ������5
    S_HOGA_1      : array[0..9]  of Char;     // �ŵ�ȣ��1
    S_HOGA_2      : array[0..9]  of Char;     // �ŵ�ȣ��2
    S_HOGA_3      : array[0..9]  of Char;     // �ŵ�ȣ��3
    S_HOGA_4      : array[0..9]  of Char;     // �ŵ�ȣ��4
    S_HOGA_5      : array[0..9]  of Char;     // �ŵ�ȣ��5
    S_CNT_1       : array[0..4]  of Char;     // �ŵ�ȣ���Ǽ�1
    S_CNT_2       : array[0..4]  of Char;     // �ŵ�ȣ���Ǽ�2
    S_CNT_3       : array[0..4]  of Char;     // �ŵ�ȣ���Ǽ�3
    S_CNT_4       : array[0..4]  of Char;     // �ŵ�ȣ���Ǽ�4
    S_CNT_5       : array[0..4]  of Char;     // �ŵ�ȣ���Ǽ�5
    B_RMNQ_1      : array[0..5]  of Char;     // �ż�ȣ������1
    B_RMNQ_2      : array[0..5]  of Char;     // �ż�ȣ������2
    B_RMNQ_3      : array[0..5]  of Char;     // �ż�ȣ������3
    B_RMNQ_4      : array[0..5]  of Char;     // �ż�ȣ������4
    B_RMNQ_5      : array[0..5]  of Char;     // �ż�ȣ������5
    B_HOGA_1      : array[0..9]  of Char;     // �ż�ȣ��1
    B_HOGA_2      : array[0..9]  of Char;     // �ż�ȣ��2
    B_HOGA_3      : array[0..9]  of Char;     // �ż�ȣ��3
    B_HOGA_4      : array[0..9]  of Char;     // �ż�ȣ��4
    B_HOGA_5      : array[0..9]  of Char;     // �ż�ȣ��5
    B_CNT_1       : array[0..4]  of Char;     // �ż�ȣ���Ǽ�1
    B_CNT_2       : array[0..4]  of Char;     // �ż�ȣ���Ǽ�2
    B_CNT_3       : array[0..4]  of Char;     // �ż�ȣ���Ǽ�3
    B_CNT_4       : array[0..4]  of Char;     // �ż�ȣ���Ǽ�4
    B_CNT_5       : array[0..4]  of Char;     // �ż�ȣ���Ǽ�5
    S_TOT_RMNQ    : array[0..8]  of Char;     // �ŵ�ȣ���Ѽ���
    S_TOT_CNT     : array[0..8]  of Char;     // �ŵ�ȣ���ѰǼ�
    B_TOT_RMNQ    : array[0..8]  of Char;     // �ż�ȣ���Ѽ���
    B_TOT_CNT     : array[0..8]  of Char;     // �ż�ȣ���ѰǼ�
    EXCE_RMNQ     : array[0..8]  of Char;     // �ʰ��ܷ�(�ż��ܷ�-�ŵ��ܷ�)
  end;

	TRA003 = Packed Record
  	GTR_HEADER    : TThinHeader;              // Real, Log Packet TP [R]
  	TM            : array[0..5]  of Char;     // ü��ð�HHMMSS
    BS_TP         : array[0..0]  of Char;     // ü�ᱸ��
  	NOW_PRC       : array[0..9]  of Char;     // ���簡
  	CHG           : array[0..9]  of Char;     // ���ϴ��
  	OPEN_PRC      : array[0..9]  of Char;     // �ð�
  	HIGH_PRC      : array[0..9]  of Char;     // ��
  	LOW_PRC       : array[0..9]  of Char;     // ����
  	CNTR_QTY      : array[0..8]  of Char;     // ü�ᷮ(���)
    S_HOGA_1      : array[0..9]  of Char;     // �ŵ�ȣ��1
    S_HOGA_2      : array[0..9]  of Char;     // �ŵ�ȣ��2
    B_HOGA_1      : array[0..9]  of Char;     // �ż�ȣ��1
    B_HOGA_2      : array[0..9]  of Char;     // �ż�ȣ��2
    S_TOT_RMNQ    : array[0..8]  of Char;     // �ŵ�ȣ���Ѽ���
    S_TOT_CNT     : array[0..8]  of Char;     // �ŵ�ȣ���ѰǼ�
    B_TOT_RMNQ    : array[0..8]  of Char;     // �ż�ȣ���Ѽ���
    B_TOT_CNT     : array[0..8]  of Char;     // �ż�ȣ���ѰǼ�
    EXCE_RMNQ     : array[0..8]  of Char;     // �ʰ��ܷ�(�ż��ܷ�-�ŵ��ܷ�)
  end;

  // API ��ȸ ��û
	TQL001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // ��ȸ�ڵ� 001:������ȸ 051:��û����ȸ
  end;

  // API ��ȸ ��û
	TQM001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // ��ȸ�ڵ� 101:��ü��ȸ(��Ź��,�ܰ�,��ü��)
  end;

  // API ���
	TQA001 = Packed Record
  	GT_HEADER     : TTRHeader;                // Header
    QUERY_CD      : array[0..2]  of Char;     // ��ȸ�ڵ� 001:������ȸ 051:��û����ȸ
                                              // 101:��ü��ȸ(��Ź��,�ܰ�,��ü��)-��������ȸ
    RESULT_YN     : array[0..0]  of Char;     // ó���������
  end;

implementation

end.