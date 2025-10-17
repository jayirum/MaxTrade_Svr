#pragma once

enum  TCMDType { cmtIOCPEvent, cmtAccept, cmtDisconnect, cmtTR_CODE };
enum  TR_CODE { ctDestAdd, ctDestDel, ctDestPushSend, ctDestRRSend, ctDestRRSendResponse, ctUserID, ctDisconnect, ctUserConnectOk, ctUsersAlready, ctClientUniqKey };
enum  TDeliveryMode { Delivery_RR, Delivery_RR_Response, Delivery_Push };

const int MESSAGE_TO_SEND = 0;
const int RECEIVED_MESSAGE = 1;
const int RESPOND_MESSAGE = 2;


typedef int		(__stdcall *TSMInitialize)					();
typedef int		(__stdcall *TSMDeInitialize)					();
typedef int		(__stdcall *TSMClientConnect)				(int index, char* host, long int port);
typedef int		(__stdcall *TSMClientDisconnect)			(int index);
typedef bool	(__stdcall *TSMClientIsConnected)			(int index);
typedef int		(__stdcall *TSMSetMessageParameters)		(int index, int messtype, TDeliveryMode DeliveryMode, char* Destination, char* Msg);
typedef int		(__stdcall *TSMSetMessageBinaryField)		(int index, int messtype, char* FieldName, char* Data, int DataSize);
typedef int		(__stdcall *TSMSetMessageStringField)		(int index, int messtype, char* FieldName, char* Str);
typedef int		(__stdcall *TSMSetMessageIntegerField)		(int index, int messtype, char* FieldName, int Val);
typedef int		(__stdcall *TSMSendMessage)					(int index, int messtype);
typedef char*	(__stdcall *TSMGetClientUniqKey)			(int index);
typedef char*	(__stdcall *TSMGetClientIP)					(int index);
typedef int		(__stdcall *TSMEventAddDestination)			(int index, char* Dest, char* Msg);
typedef int		(__stdcall *TSMEventRemoveDestination)		(int index, char* Dest, char* Msg);
typedef int		(__stdcall *TSMEventAllRemoveDestination)	(int index);
typedef int		(__stdcall *TSMGetReceivedCnt)				(int index);
typedef int		(__stdcall *TSMSetWorkEventCallBack)		(int index, char* CallBackProc, char* pCustomPtr);
typedef char*	(__stdcall *TSMMessageGetBinaryFieldValue)	(int index, int messtype, char* FieldName);
typedef char*	(__stdcall *TSMMessageGetStringFieldValue)	(int index, int messtype, char* FieldName);
typedef int		(__stdcall *TSMMessageGetIntegerFieldValue)	(int index, int messtype, char* FieldName);
typedef double	(__stdcall *TSMMessageGetDoubleFieldValue)	(int index, int messtype, char* FieldName);
typedef int		(__stdcall *TSMGetObjectsNumber)			();
typedef int		(__stdcall *TSMGetMaximumObjectsNumber)		();
typedef int		(__stdcall *TSMCreateInstance)				();
typedef int		(__stdcall *TSMMessageGetDeliveryMode)		(int index, int messtype);
typedef bool	(__stdcall *TSMSendResponse)				(int index, int messtype);
typedef char*	(__stdcall *TSMMessageGetDestination)		(int index, int messtype);
typedef char*	(__stdcall *TSMMessageGetMsg)				(int index, int messtype);
typedef int		(__stdcall *TSMMessageGetClientSession)		(int index, int messtype);
typedef int		(__stdcall *TSMSMessageToSMessageEx)		(int index);
typedef char*	(__stdcall *TSMGetMsgOfRecvMsg)				(int index);
typedef int		(__stdcall *TSetSMMessageHeader)			(int index, int DeliveryMode, char* Destination, char* Msg, int ClientSession);
typedef int		(__stdcall *TSMMessageGetBinaryFieldValueEx)(int index, char* FieldName, void * pResult);


	// CLENT  [] => SERVER ( Request & Reply )
	const char   SISE_PROC		[] = "\\SISE\\SVR\\";
	const char   ORDER_PROC		[] = "\\ORDER\\SVR\\";
	const char   SEARCH_PROC	[] = "\\SEARCH\\SVR\\";
	const char   BANKING_PROC	[] = "\\BANKING\\SVR\\";
	const char   ACCOUNT_PROC	[] = "\\ACCOUNT\\SVR\\";
	const char   MANAGER_PROC	[] = "\\MANAGER\\SVR\\";
	const char   LOGIN_PROC		[] = "\\LOGIN\\SVR\\";
	const char   BATCH_PROC		[] = "\\BATCH\\SVR\\";
	const char   SYS_ORD_PROC	[] = "\\SYS\\ORD\\SVR\\";

	// GW
	const char   ORDER_GW[] = "\\ORDER\\GW\\";
	const char   SISE_GW[] = "\\SISE\\GW\\";
	const char   EXEC_GW[] = "\\EXEC\\GW\\";

	// REAL GW
	const char   ORD_REAL_GW[] = "\\ORD\\REAL\\GW\\";
	const char   EXEC_REAL_GW[] = "\\EXEC\\REAL\\GW\\";

	// RCV
	const char   ORDER_RCV[] = "\\ORDER\\RESPONSE\\";
	const char   ORD_REAL_RCV[] = "\\ORD\\REAL\\RESPONSE\\";

	// EXCHANGE
	const char   EXCH_PROC[] = "\\EXCHANGE\\SVR\\";

	// EXEC
	const char   EXEC_PROC[] = "\\EXEC\\SVR\\";

	// SERVER [] => CLIENT ( Push )
	const char   SISE_REQ[] = "\\SISE\\CLI\\";
	const char   ORDER_SEND[] = "\\ORDER\\CLI\\";
	const char   MANAGER_SEND[] = "\\MANAGER\\CLI\\";
	const char   ALL_CLIENT[] = "\\ALL\\CLIENT\\";

	// Server [] => Server ( Push )
	const char   LOSSCUT_PROC[] = "\\LOSSCUT\\SVR\\";
	const char   HEDGE_PROC[] = "\\HEDGE\\SVR\\";

	// ALL SERVER
	const char   ALL_SERVER[] = "\\ALL\\SERVER\\";

	// SIGNAL & STRAT
	const char   SIG_REALY[] = "\\SIG\\RELAY\\";



	const char fldHDUserID[] = "H001";
	const char fldHDWndHandle[] = "H002";
	const char fldHDIPAddr[] = "H003";
	const char fldHDErrFlag[] = "H004";
	const char fldHDMsgCode[] = "H005";
	const char fldHDMsg[] = "H006";
	const char fldHDMacAddr[] = "H007";


	const char fldFutMast[] = "S001";
	const char fldFutHoga[] = "S002";
	const char fldFutExec[] = "S003";
	const char fldOptMast[] = "S004";
	const char fldOptHoga[] = "S005";
	const char fldOptExec[] = "S006";
	const char fldFXMast[] = "S007";
	const char fldFXHoga[] = "S008";
	const char fldFXExec[] = "S009";


	const char fldSPOTMast[] = "S010";
	const char fldSPOTHoga[] = "S011";
	const char fldSPOTExec[] = "S012";


	const char fldEurexMast[] = "S013";
	const char fldEurexHoga[] = "S014";
	const char fldEurexExec[] = "S015";

	// Binary Field
	const char fldBinOrder[] = "B001";  // TTradeOrder
	const char fldBinOrigOrder[] = "B002";  // TTradeOrder
	const char fldBinOpenInterest[] = "B003";  // TTradeOpenInterest
	const char fldBinBalance[] = "B004";  // TTradeBalance

	const char fldBinTotOrder[] = "B005";  // TTradeTotOrder
	const char fldBinTotOI[] = "B006";  // TTradeTotOpenInterest
	const char fldBinTotBalance[] = "B007";  // TTradeTotBalance
	const char fldBinTotSysOrder[] = "B008";  // TTradeTotOrder
	const char fldBinTotSysOI[] = "B009";  // TTradeTotOpenInterest
	const char fldBinUser[] = "B010";  // TTradeUser

									   // Field
	const char fldSymbol[] = "U001";
	const char fldAccountNo[] = "U002";
	const char fldOrderQty[] = "U003";
	const char fldPrice[] = "U004";
	const char fldSide[] = "U005";
	const char fldOrdType[] = "U006";
	const char fldHostOrdNo[] = "U007";
	const char fldOrigHostOrdNo[] = "U008";
	const char fldExecQty[] = "U009";
	const char fldExecPrice[] = "U010";
	const char fldOrderID[] = "U011";
	const char fldOrigOrderID[] = "U012";
	const char fldSpeedOrdFlag[] = "U013";
	const char fldPendingStatus[] = "U014";
	const char fldSystemOrdFlag[] = "U015";
	const char fldBankingType[] = "U016";
	const char fldComments[] = "U017";
	const char fldDepositReqMoney[] = "U018";
	const char fldDepositMoney[] = "U019";
	const char fldWithDrawReqMoney[] = "U020";
	const char fldWithDrawMoney[] = "U021";
	const char fldTransferFee[] = "U022";
	const char fldBankName[] = "U023";
	const char fldBankAccountNo[] = "U024";
	const char fldBankAccountHolder[] = "U025";
	const char fldBankingProcFlag[] = "U026";
	const char fldDepositHolder[] = "U027";
	const char fldReqSeq[] = "U028";
	const char fldReqType[] = "U029";
	const char fldUserStatus[] = "U030";
	const char fldLicenseType[] = "U031";
	const char fldLoanCount[] = "U032";
	const char fldReqProcFlag[] = "U033";
	const char fldUserID[] = "U034";
	const char fldUserName[] = "U035";
	const char fldPassword[] = "U036";
	const char fldEMail[] = "U037";
	const char fldPhoneNo[] = "U038";
	const char fldMobilePhoneNo[] = "U039";
	const char fldUserAddress[] = "U040";
	const char fldFutCommissionRate[] = "U041";
	const char fldOptCommissionRate[] = "U042";
	const char fldLicenseFee[] = "U043";
	const char fldLicenseImposeType[] = "U044";
	const char fldRoleID[] = "U045";
	const char fldLicenseMoney[] = "U046";
	const char fldUseDays[] = "U047";
	const char fldTotRec[] = "U048";
	const char fldTotRec2[] = "U049";
	const char fldNoticeSeq[] = "U050";
	const char fldNoticeSubject[] = "U051";
	const char fldNoticeText[] = "U052";
	const char fldTradeDate[] = "U053";
	const char fldRecStatus[] = "U054";
	const char fldMoney[] = "U055";
	const char fldCmplSeq[] = "U056";
	const char fldCmplType[] = "U057";
	const char fldBankingSeq[] = "U058";
	const char fldUpdateTime[] = "U059";
	const char fldTotRec3[] = "U060";
	const char fldTotRec4[] = "U061";
	const char fldExpireFlag[] = "U062";
	const char fldExecMsgType[] = "U063";
	const char fldBalanceMoney[] = "U064";
	const char fldClearCondition[] = "U065";
	const char fldTradeLimitFlag[] = "U066";
	const char fldEvalMoney[] = "U067";
	const char fldLossCutFlag[] = "U068";
	const char fldHostExecID[] = "U069";
	const char fldExecutionID[] = "U070";
	const char fldNewUserFlag[] = "U071";

	const char fldHedgeAccountNo[] = "U072";
	const char fldFutFlag[] = "U073";
	const char fldOptFlag[] = "U074";
	const char fldHedgeAccountFlag[] = "U076";
	const char fldNewOrdType[] = "U077";
	const char fldNewOrdTick[] = "U078";
	const char fldClearOrdType[] = "U079";
	const char fldClearOrdTick[] = "U080";
	const char fldErrorMsg[] = "U081";
	const char fldLeverageType[] = "U082";
	const char fldSMSRcvFlag[] = "U083";
	const char fldEMailRcvFlag[] = "U084";
	const char fldServerIP[] = "U085";
	const char fldServerPort[] = "U086";
	const char fldMarketServerIP[] = "U087";
	const char fldMarketServerPort[] = "U088";
	const char fldDBServerIP[] = "U089";
	const char fldDbServerPort[] = "U090";
	const char fldMileage[] = "U091";
	const char fldSwitchMileage[] = "U092";
	const char fldFxBaseCommission[] = "U093";

	const char fldFutOpenTime[] = "U094";
	const char fldFutCloseTime[] = "U095";
	const char fldFutExpOpenTime[] = "U096";
	const char fldFutExpCloseTime[] = "U097";
	const char fldFutOrdCloseFlag[] = "U098";
	const char fldFutConfigMsg[] = "U099";
	const char fldFxOpenTime[] = "U100";
	const char fldFxCloseTime[] = "U101";
	const char fldFxExpOpenTime[] = "U102";
	const char fldFxExpCloseTime[] = "U103";
	const char fldFxOrdCloseFlag[] = "U104";
	const char fldFxConfigMsg[] = "U105";
	const char fldRunBackupOrder[] = "U106";
	const char fldRunUploadSecurity[] = "U107";
	const char fldRunEventDeposit[] = "U108";
	const char fldBeableQty[] = "U109";
	const char fldBeableOptQty[] = "U301";
	const char fldBeableFutQty[] = "U302";
	const char fldBeableFxQty[] = "U303";
	const char fldProduct[] = "U110";
	const char fldAuthorityKey[] = "U111";
	const char fldForciblyOrdFlag[] = "U112";
	const char fldOpenPriceFlag[] = "U113";
	const char fldFXCommissionType[] = "U114";
	const char fldAlreadyLoginFlag[] = "U115";
	const char fldUserIdentity[] = "U116";
	const char fldUSDExRate[] = "U117";
	const char fldPopupFlag[] = "U118";
	const char fldCmeCommissionRate[] = "U119";
	const char fldOrderPath[] = "U120";
	const char fldCustNo[] = "U121";
	const char fldMaxBeableQty[] = "U122";
	const char fldTradeProduct[] = "U123";
	const char fldOrdCondType[] = "U124";
	const char fldSecurityKey[] = "U125";
	const char fldOrigMsgType[] = "U126";
	const char fldRealHostOrdNo[] = "U127";
	const char fldFutGrade[] = "U128";
	const char fldFXGrade[] = "U129";
	const char fldFutUseFlag[] = "U130";
	const char fldOptUseFlag[] = "U131";
	const char fldFXUseFlag[] = "U132";
	const char fldHighPrice[] = "U133";
	const char fldLowPrice[] = "U134";
	const char fldLossCutMoney[] = "U135";
	const char fldGoodsType[] = "U136";
	const char fldCloseOrdFlag[] = "U137";
	const char fldRealOrdFlag[] = "U138";
	const char fldAutoCxlFlag[] = "U139";
	const char fldOrdHandleType[] = "U140";
	const char fldFXVIPOrdFlag[] = "U141";
	const char fldOverNightOrdFlag[] = "U142";
	const char fldOverNightAccountNo[] = "U143";
	const char fldRecommenderName[] = "U144";
	const char fldCMEOpenTime[] = "U145";
	const char fldCMECloseTime[] = "U146";
	const char fldCMEOrdCloseFlag[] = "U147";
	const char fldSTOPSetType[] = "U148";
	const char fldSTOPBasePrice[] = "U149";
	const char fldSTOPProfitUseFlag[] = "U150";
	const char fldSTOPProfitTick[] = "U151";
	const char fldSTOPLossUseFlag[] = "U152";
	const char fldSTOPLossTick[] = "U153";
	const char fldMITPlcType[] = "U154";
	const char fldMITPrice[] = "U155";
	const char fldMITOrigPrice[] = "U156";
	const char fldStrikePrice[] = "U157";
	const char fldMktSvrType[] = "U158";
	const char fldFXOptGrade[] = "U159";

	const char fldEurexOpenTime[] = "U160";
	const char fldEurexCloseTime[] = "U161";
	const char fldEurexExpOpenTime[] = "U162";
	const char fldEurexExpCloseTime[] = "U163";
	const char fldEurexOrdCloseFlag[] = "U164";
	const char fldEurexConfigMsg[] = "U165";

	const char fldHKDExRate[] = "U166";
	const char fvBreakTimeFlag[] = "U167";



	// HEADER PART
	const char fldHStratCode[] = "S001";
	const char fldHStratUserID[] = "S002";
	const char fldHStratCompID[] = "S003";
	const char fldHStratTp[] = "S004";
	const char fldHStratSymbol[] = "S005";
	const char fldHStratGroupKey[] = "S006";
	const char fldHStratApiYN[] = "S007";
	const char fldHStratTM[] = "S008";
	const char fldHStratReserve[] = "S009";

	const char fldStratID[] = "S010";
	const char fldStratChartTp[] = "S011";
	const char fldStratChartNm[] = "S012";
	const char fldStratPrc[] = "S013";
	const char fldStratChartGb[] = "S014";
	const char fldStratArrowType[] = "S015";

	const char fldStratMatchedChart[] = "S016";
	const char fldStratClrYN[] = "S017";
	const char fldStratCrossTp[] = "S018";
	const char fldStratClrAssistPrc[] = "S019";
	const char fldStratNowPrc[] = "S020";

	const char fldStratBreakTp[] = "S021";
	const char fldStratMAPrice[] = "S022";

	const char fldStratSide[] = "S023";
	const char fldStratOrdTp[] = "S024";
	const char fldStratProcTp[] = "S025";
	const char fldStratOrdPrc[] = "S026";
	const char fldStratOrdQty[] = "S027";
	const char fldStratCnclAllYN[] = "S028";
	const char fldStratOrigNo[] = "S029";
	const char fldStratOrigPrc[] = "S030";
	const char fldStratSLTick[] = "S031";
	const char fldStratPTTick[] = "S032";


	const char  MSG_ORD_FUT_NEW[] = "\\FUT\\NEW\\";
	const char  MSG_ORD_FUT_REP[] = "\\FUT\\REP\\";
	const char  MSG_ORD_FUT_CXL[] = "\\FUT\\CXL\\";
	const char  MSG_ORD_OPT_NEW[] = "\\OPT\\NEW\\";
	const char  MSG_ORD_OPT_REP[] = "\\OPT\\REP\\";
	const char  MSG_ORD_OPT_CXL[] = "\\OPT\\CXL\\";
	const char  MSG_ORD_FX_NEW[] = "\\FX\\NEW\\";
	const char  MSG_ORD_FX_REP[] = "\\FX\\REP\\";
	const char  MSG_ORD_FX_CXL[] = "\\FX\\CXL\\";
	const char  MSG_ORD_CME_NEW[] = "\\CME\\NEW\\";
	const char  MSG_ORD_CME_REP[] = "\\CME\\REP\\";
	const char  MSG_ORD_CME_CXL[] = "\\CME\\CXL\\";


	const char  MSG_ORD_All_CLEAR[] = "\\ORD\\ALL\\CLEAR\\";
	const char  MSG_ORD_ALL_CXL[] = "\\ORD\\ALL\\CXL\\";
	const char  MSG_ORD_ALL_REP[] = "\\ORD\\ALL\\REP\\";
	const char  MSG_ORD_HEDGE_ALL_CLEAR[] = "\\ORD\\HEDGE_ALL\\CLEAR\\";


	const char  MSG_ORD_SYMBOL_CLEAR[] = "\\ORD\\SYMBOL\\CLEAR\\";
	const char  MSG_ORD_SYMBOL_CXL[] = "\\ORD\\SYMBOL\\CXL\\";



	const char  MSG_ORD_HSI_BREAK[] = "\\ORD\\HSI\\BREAK\\";


	const char  MSG_ORD_SYS_CXL[] = "\\ORD\\SYS\\CXL\\";
	const char  MSG_ORD_SYS_CLEAR[] = "\\ORD\\SYS\\CLEAR\\";
	const char  MSG_ORD_SYS_CLOSE[] = "\\ORD\\SYS\\CLOSE\\";
	const char  MSG_ORD_SYS_HTS[] = "\\ORD\\SYS\\HTS\\";
	const char  MSG_ORD_SYS_CME_CLOSE[] = "\\ORD\\SYS\\CME\\CLOSE\\";
	const char  MSG_ORD_SYS_EXPIRE[] = "\\ORD\\SYS\\EXPIRE\\";


	const char  MSG_ORD_LOSSCUT[] = "\\ORD\\SYS\\LOSSCUT\\";
	const char  MSG_MGR_LOSSCUT[] = "\\ORD\\MGR\\LOSSCUT\\";

	const char  MSG_RCV_LOSSCUT[] = "\\RCV\\SYS\\LOSSCUT\\";

	const char  MSGP_ORD_NEW[] = "\\SEND\\ORD\\NEW\\";
	const char  MSGP_ORD_REP[] = "\\SEND\\ORD\\REP\\";
	const char  MSGP_ORD_CXL[] = "\\SEND\\ORD\\CXL\\";

	const char  MSGR_GW_EXCH[] = "\\R\\GW\\EXCH\\";

	const char  MSG_EXCH_RCV[] = "\\EXCH\\RCV\\";

	const char  MSG_EXEC[] = "\\EXEC\\";
	const char  MSG_EXCH_REP[] = "\\EXCH\\REP\\";
	const char  MSG_EXCH_CXL[] = "\\EXCH\\CXL\\";
	const char  MSG_EXCH_REJ[] = "\\EXCH\\REJ\\";

	const char  MSGR_REQ_DEPOSIT[] = "\\RCV\\REQ\\DEPOSIT\\";
	const char  MSGR_REQ_WITHDRAW[] = "\\RCV\\REQ\\WITHDRAW\\";
	const char  MSGR_CHG_ACCSTATUS[] = "\\RCV\\CHG\\ACCSTATUS\\";
	const char  MSGR_CHG_LICENSE[] = "\\RCV\\CHG\\LICENSE\\";
	const char  MSGR_CHG_LOAN_CNT[] = "\\RCV\\CHG\\LOANCNT\\";
	const char  MSGR_REQ_BEABLE_QTY[] = "\\RCV\\REQ\\BEABLE_QTY\\";
	const char  MSGR_REQ_OVERNIGHT[] = "\\RCV\\REQ\\OVERNIGHT\\";
	const char  MSGR_REQ_ONCLOSE[] = "\\RCV\\REQ\\ONCLOSE\\";
	const char  MSGR_REQ_CMEUSE[] = "\\RCV\\REQ\\CMEUSE\\";
	const char  MSGR_REQ_EXPIRECALC[] = "\\RCV\\REQ\\EXPIRECALC\\";
	const char  MSGR_SET_ORDCOND[] = "\\RCV\\SET\\ORDCOND\\";
	const char  MSGR_RESET_ORDCOND[] = "\\RCV\\RESET\\ORDCOND\\";
	const char  MSGP_CHG_ORDCOND[] = "\\SEND\\CHG\\ORDCOND\\";

	const char  MSGP_REQ_DEPOSIT[] = "\\SEND\\REQ\\DEPOSIT\\";
	const char  MSGP_REQ_WITHDRAW[] = "\\SEND\\REQ\\WITHDRAW\\";
	const char  MSGP_REQ_OVERNIGHT[] = "\\SEND\\REQ\\OVERNIGHT\\";
	const char  MSGP_CHG_ACCSTATUS[] = "\\SEND\\CHG\\ACCSTATUS\\";
	const char  MSGP_CHG_LICENSE[] = "\\SEND\\CHG\\LICENSE\\";
	const char  MSGP_CHG_LOAN_CNT[] = "\\SEND\\CHG\\LOANCNT\\";
	const char  MSGR_CHG_LEVERAGE[] = "\\SEND\\CHG\\LEVERAGE\\";
	const char  MSGR_CHG_USERINFO[] = "\\SEND\\CHG\\USERINFO\\";
	const char  MSGR_SWITCH_MILEAGE[] = "\\SEND\\SWITCH\\MILEAGE\\";
	const char  MSGP_REQ_BEABLE_QTY[] = "\\SEND\\REQ\\BEABLE_QTY\\";

	const char  MSGR_DEPOSIT[] = "\\MGR\\DEPOSIT\\";
	const char  MSGR_WITHDRAW[] = "\\MGR\\WITHDRAW\\";
	const char  MSGR_EVENT_DEPOSIT[] = "\\MGR\\EVENT\\DEPOSIT\\";
	const char  MSGR_MILEAGE_DEPOSIT[] = "\\MGR\\MILEAGE\\DEPOSIT\\";
	const char  MSGR_MILEAGE_WITHDRAW[] = "\\MGR\\MILEAGE\\WITHDRAW\\";

	const char  MSGR_LICENSE_WITHDRAW[] = "\\MGR\\LICENSE\\WITHDRAW\\";
	const char  MSGR_LOAN_DEPOSIT[] = "\\MGR\\LOAN\\DEPOSIT\\";
	const char  MSGR_LOAN_WITHDRAW[] = "\\MGR\\LOAN\\WITHDRAW\\";
	const char  MSGR_LICENSE_IMPOSE[] = "\\MGR\\LICENSE\\IMPOSE\\";
	const char  MSGR_ACCSTATUS[] = "\\MGR\\ACCSTATUS\\";
	const char  MSGR_INCREASE_DAYS[] = "\\MGR\\INCREASE\\DAYS\\";
	const char  MSGR_LICENSE_CHARGE[] = "\\MGR\\LICENSE\\CHARGE\\";
	const char  MSGR_BEABLE_QTY[] = "\\MGR\\BEABLE_QTY\\";


	const char  MSGP_LOSSCUT_MONEY[] = "\\SEND\\LOSSCUT\\MONEY\\";
	const char  MSGP_LOSSCUT_SET_MONEY[] = "\\SEND\\LOSSCUT\\SET_MONEY\\";
	const char  MSGP_LOSSCUT_RELOAD[] = "\\SEND\\LOSSCUT\\RELOAD\\";
	const char  MSGP_BEABLE_QTY[] = "\\SEND\\BEABLE_QTY\\";

	const char  MSGR_WORK_COMPLETE[] = "\\MGR\\WORK\\COMPLETE\\";    // Manager Complete

	const char  MSGR_USER_BANK_ACCOUNT[] = "\\REG\\USER_BANK_ACCCOUNT\\";
	const char  MSGR_BANK_ACCOUNT[] = "\\REG\\BANK_ACCCOUNT\\";

	const char  MSGR_TOT_BALANCE[] = "\\SCH\\TOT\\BALANCE\\";

	const char  MSGP_TOT_BALANCE[] = "\\TOT\\BALANCE\\";
	const char  MSGP_TOT_OPENINT[] = "\\TOT\\OPENINT\\";
	const char  MSGP_TOT_ORDER[] = "\\TOT\\ORDER\\";

	const char  MSGR_USER_REG[] = "\\MGR\\REG\\USER\\";
	const char  MSGR_USER_DEL[] = "\\MGR\\DEL\\USER\\";
	const char  MSGR_USER_RESET[] = "\\MGR\\RESET\\USER\\";
	const char  MSGR_NOTICE_REG[] = "\\MGR\\REG\\NOTICE\\";
	const char  MSGR_NOTICE_CHG[] = "\\MGR\\CHG\\NOTICE\\";
	const char  MSGR_NOTICE_DEL[] = "\\MGR\\DEL\\NOTICE\\";

	const char  MSGP_USER_REG[] = "\\SEND\\REG\\USER\\";
	const char  MSGP_USER_DEL[] = "\\SEND\\DEL\\USER\\";

	const char  MSGR_CONFIRM_MEMBER[] = "\\MGR\\CONFIRM\\MEMBER\\";
	const char  MSGR_REJECT_MEMBER[] = "\\MGR\\REJECT\\MEMBER\\";

	const char  MSGR_CHG_FUT_CONFIG[] = "\\MGR\\CHG\\FUT\\CONFIG\\";
	const char  MSGR_CHG_EUREX_CONFIG[] = "\\MGR\\CHG\\EUREX\\CONFIG\\";
	const char  MSGR_CHG_FX_CONFIG[] = "\\MGR\\CHG\\FX\\CONFIG\\";
	const char  MSGR_CHG_CME_CONFIG[] = "\\MGR\\CHG\\CME\\CONFIG\\";
	const char  MSGR_CLOSE_FUT[] = "\\MGR\\CLOSE\\FUT\\";
	const char  MSGR_CLOSE_FX[] = "\\MGR\\CLOSE\\FX\\";
	const char  MSGR_CLOSE_CME[] = "\\MGR\\CLOSE\\CME\\";

	const char  MSGR_ORDER_DEL[] = "\\MGR\\DEL\\ORDER\\";
	const char  MSGP_ORDER_DEL[] = "\\SEND\\DEL\\ORDER\\";
	const char  MSGR_MANUAL_EXEC[] = "\\MGR\\MANUAL\\EXEC\\";
	const char  MSGP_MANUAL_EXEC[] = "\\SEND\\MANUAL\\EXEC\\";

	const char  MSGP_NOTICE[] = "\\SEND\\NOTICE\\";
	const char  MSGP_WARNING[] = "\\SEND\\WARNING\\";
	const char  MSGP_MESSAGE[] = "\\SEND\\MESSAGE\\";

	const char  MSGP_CHG_MONEY[] = "\\SEND\\CHG\\MONEY\\";

	// SERVER RESTART
	const char  MSGP_RESTART[] = "\\SERVER\\RESTART\\"; // SERVER RESTART

														// Hedge
	const char  MSGR_HEDGE_USER_REG[] = "\\MGR\\REG\\HEDGE_USER\\";
	const char  MSGR_SET_HEDGE[] = "\\MGR\\SET\\HEDGE\\";
	const char  MSGP_SET_HEDGE[] = "\\SEND\\SET\\HEDGE\\";

	// MARKET PRICE
	const char  MSG_SISE_FUT_MAST[] = "\\FUT\\MAST\\";
	const char  MSG_SISE_FUT_HOGA[] = "\\FUT\\HOGA\\";
	const char  MSG_SISE_FUT_EXEC[] = "\\FUT\\EXEC\\";
	const char  MSG_SISE_OPT_MAST[] = "\\OPT\\MAST\\";
	const char  MSG_SISE_OPT_HOGA[] = "\\OPT\\HOGA\\";
	const char  MSG_SISE_OPT_EXEC[] = "\\OPT\\EXEC\\";
	const char  MSG_SISE_FX_MAST[] = "\\FX\\MAST\\";
	const char  MSG_SISE_FX_HOGA[] = "\\FX\\HOGA\\";
	const char  MSG_SISE_FX_EXEC[] = "\\FX\\EXEC\\";
	const char  MSG_SISE_CME_MAST[] = "\\CME\\MAST\\";
	const char  MSG_SISE_CME_HOGA[] = "\\CME\\HOGA\\";
	const char  MSG_SISE_CME_EXEC[] = "\\CME\\EXEC\\";

	const char  MSG_SISE_SPOT_MAST[] = "\\SPOT\\MAST\\";
	const char  MSG_SISE_SPOT_HOGA[] = "\\SPOT\\HOGA\\";
	const char  MSG_SISE_SPOT_EXEC[] = "\\SPOT\\EXEC\\";

	const char  MSG_SISE_EUREX_MAST[] = "\\EUREX\\MAST\\";
	const char  MSG_SISE_EUREX_HOGA[] = "\\EUREX\\HOGA\\";
	const char  MSG_SISE_EUREX_EXEC[] = "\\EUREX\\EXEC\\";

	// MARKET PRICE
	const char  MSG_MKT_KOSPI200[] = "\\MKT\\KOSPI200\\EXEC\\";
	const char  MSG_MKT_FUT_HOGA[] = "\\MKT\\FUT\\HOGA\\";
	const char  MSG_MKT_FUT_EXEC[] = "\\MKT\\FUT\\EXEC\\";
	const char  MSG_MKT_OPT_HOGA[] = "\\MKT\\OPT\\HOGA\\";
	const char  MSG_MKT_OPT_EXEC[] = "\\MKT\\OPT\\EXEC\\";
	const char  MSG_MKT_FX_HOGA[] = "\\MKT\\FX\\HOGA\\";
	const char  MSG_MKT_FX_EXEC[] = "\\MKT\\FX\\EXEC\\";
	const char  MSG_MKT_CME_HOGA[] = "\\MKT\\CME\\HOGA\\";
	const char  MSG_MKT_CME_EXEC[] = "\\MKT\\CME\\EXEC\\";
	const char  MSG_MKT_CME_MAST[] = "\\MKT\\CME\\MAST\\";

	const char  MSG_MKT_SPOT_HOGA[] = "\\MKT\\SPOT\\HOGA\\";
	const char  MSG_MKT_SPOT_EXEC[] = "\\MKT\\SPOT\\EXEC\\";

	const char  MSG_MKT_EUREX_HOGA[] = "\\MKT\\EUREX\\HOGA\\";
	const char  MSG_MKT_EUREX_EXEC[] = "\\MKT\\EUREX\\EXEC\\";
	const char  MSG_MKT_EUREX_MAST[] = "\\MKT\\EUREX\\MAST\\";

	// Order GW

	const char  MSGR_GW_ORD_NEW[] = "\\R\\GW\\ORD\\NEW\\";
	const char  MSGR_GW_ORD_REP[] = "\\R\\GW\\ORD\\REP\\";
	const char  MSGR_GW_ORD_CXL[] = "\\R\\GW\\ORD\\CXL\\";
	
	// FBI
	const char  MSGR_DEAL_STATUS[]		= "\\R\\RCV\\DEAL_STATUS\\" ;
	const char  MSGR_DEAL_STATUS_TENOP[]= "\\R\\RCV\\DEAL_STATUS_TENOP\\" ;
	const char  MSGR_DEAL_STATUS_SL[]	= "\\R\\RCV\\DEAL_STATUS_SL\\" ;
	const char	MSG_SYS_BATCH[] = "\\SYS\\BATCH\\";

	/*
	// Order REAL GW

	MSGR_GW_ORD_NEW    [] = "\\R\\REAL\\GW\\FUT\\NEW\\";
	MSGR_GW_ORD_REP    [] = "\\R\\REAL\\GW\\FUT\\REP\\";
	MSGR_GW_ORD_CXL    [] = "\\R\\REAL\\GW\\FUT\\CXL\\";
	*/

	// Order Rcv
	const char  MSGR_RCV_ORD_NEW[] = "\\R\\RCV\\ORD\\NEW\\";
	const char  MSGR_RCV_ORD_REP[] = "\\R\\RCV\\ORD\\REP\\";
	const char  MSGR_RCV_ORD_CXL[] = "\\R\\RCV\\ORD\\CXL\\";
	const char  MSGR_RCV_ORD_REJ[] = "\\R\\RCV\\ORD\\REJ\\";

	//MSGR_LOGIN
	const char  MSGR_LOGIN[] = "\\R\\RCV\\LOGIN\\";
	const char  MSGR_LOGOUT[] = "\\R\\RCV\\LOGOUT\\";
	const char  MSGP_LOGOUT[] = "\\P\\SEND\\LOGOUT\\";
	const char  MSGR_KILL_USER[] = "\\R\\KILL\\USER\\";
	const char  MSGP_KILL_USER[] = "\\P\\KILL\\USER\\";

	//MSGR_RUN_DAILY
	const char  MSGR_RUN_DAILY[] = "\\R\\RCV\\RUN\\DAILY\\";
	//MSGR_RUN_SECURITY
	const char  MSGR_RUN_SECURITY[] = "\\R\\RCV\\RUN\\SECURITY\\";

	//Change Order Close Time
	const char  MSGP_SYS_ORD_TIME[] = "\\P\\SYS\\ORD\\TIME\\";


	const char  MSGR_SET_BEABLE_QTY[] = "\\R\\RCV\\SET\\BEABLE_QTY\\";


	const char  MSGR_SET_USD_EX_RATE[] = "\\R\\RCV\\SET\\USD_EX_RATE\\";
	const char  MSGP_SET_USD_EX_RATE[] = "\\P\\SEND\\USD_EX_RATE\\";


	const char  MSGR_SET_USER_GRADE[] = "\\R\\RCV\\SET\\USER_GRADE\\";
	const char  MSGP_SET_USER_GRADE[] = "\\P\\SEND\\SET\\USER_GRADE\\";


	const char  MSGR_SET_ENVIRONMENT[] = "\\R\\RCV\\SET\\USER_ENV\\";


	const char  MSGR_SET_FX_VIP_ORD[] = "\\R\\RCV\\SET\\FX_VIP_ORD\\";


	const char  MSGP_RELOAD_OI[] = "\\P\\SEND\\RELOAD\\OI\\";


	const char  MSGR_STOP_ORDER[] = "\\R\\RCV\\STOP\\ORDER\\";

	const char  MSGR_MIT_ORDER[] = "\\R\\RCV\\MIT\\ORDER\\";

	const char  MSGR_MANAGER_ORD_REJ[] = "\\R\\RCV\\MANAGER\\ORD\\REJ\\";

	const char  MSGR_OPT_EXPIRE[] = "\\R\\RCV\\OPT\\EXPIRE\\";

	const char  MSGR_EUREX_EXPIRE[] = "\\R\\RCV\\EUREX\\EXPIRE\\";

	const char  MSGR_SCH_MKTSERVER[] = "\\R\\RCV\\SCH\\MKTSERVER\\";
	const char  MSGR_CHG_MKTSERVER[] = "\\R\\RCV\\CHG\\MKTSERVER\\";

	const char  KR_REQ_5611[] = "\\KRSEND\\REQ5611\\";

	const char  fvSide_SELL[] = "1";
	const char  fvSide_BUY[] = "2";


	const int fvProduct_KOSPI200 = 0;   //  KOSPI200
	const int fvProduct_FUT = 1;   //  ����
	const int fvProduct_CALL = 2;   //  �ݿɼ�
	const int fvProduct_PUT = 3;   //  ǲ�ɼ�
	const int fvProduct_SPREAD = 4;   //  ��������
	const int fvProduct_OPT = 5;   //  �ɼ� ( ���� : ���� Product Value�� ������ ����. �ɼ� ��ü ��ȸ�ø� ���)
	const int fvProduct_EUROFX = 7;   //  EURO FX
	const int fvProduct_AUSTFX = 72;  //  AUSTRAINAN DOLLAR
	const int fvProduct_POUDFX = 73;  //  BRITISH POUND
	const int fvProduct_YENFX = 74;  //  JapanYen
	const int fvProduct_FRACFX = 75;  //  SWITSS FRANC
	const int fvProduct_CANADAFX = 76;  //  Canada dollar
	const int fvProduct_CME = 8;   //  CME

	const int fvProduct_SPOT = 9;  // Spot
	const int fvProduct_SPOTGOLD = 91; // Spot_Gold
	const int fvProduct_SPOTOIL = 92; // Spot_OIL
	const int fvProduct_SPOTSILVER = 93; // Spot_SILVER
	const int fvProduct_SPOTCOPPER = 94; // Spot_COPPER
	const int fvProduct_SPOTSP500 = 95;  // Spot_SP500
	const int fvProduct_SGX_FTSE = 96;  // SGX FTSE China A50
	const int fvProduct_HSI = 97;   // �׼�
	const int fvProduct_YM = 90;   // �ٿ�
	const int fvProduct_NASDAQ = 98;   // ������
	const int fvProduct_NIKKEI = 100;  // ������
	const int fvProduct_GAS = 101;  // õ������
	const int fvProduct_FDAX = 99;  // �ڽ�
	const int fvProduct_BIT = 111; // ��Ʈ����

	const int fvProduct_EUREX_CALL = 51;   //  �ݿɼ�
	const int fvProduct_EUREX_PUT = 52;   //  ǲ�ɼ�
	const int fvProduct_EUREX_OPT = 55;   //  �߰��ɼǿ�  ( ���� : ���� Product Value�� ������ ����. �ɼ� ��ü ��ȸ�ø� ���)



	const int fvGoodsType_LOCAL = 1;    // ��������
	const int fvGoodsType_CME = 2;    // �߰�����
	const int fvGoodsType_GLOBAL = 3;    // �ؿܼ���

	const int fvSymbolType_FUT = '101'; //FUT
	const int fvSymbolType_CALL = '201'; //CALL
	const int fvSymbolType_PUT = '301'; //PUT
	const int fvSymbolType_FX = '6E';  //GF

	const char fvATM_Name[] = "ATM";
	const char fvITM_Name[] = "ITM";
	const char fvOTM_Name[] = "OTM";

	const char fvATM = '1';
	const char fvITM = '2';
	const char fvOTM = '3';

	//const char fvSide_SELL = '1';    // �ŵ�
	//const char fvSide_BUY = '2';    // �ż�
	const char fvSideName_SELL[] = "�ŵ�";  // �ŵ�
	const char fvSideName_BUY[] = "�ż�";  // �ż�

	const char fvOrdType_LIMIT = '1';  // ������
	const char fvOrdType_MARKET = '2';  // ���尡
	const char fvOrdType_CONDLIMIT = '3';  // ���Ǻ�������
	const char fvOrdType_BESTLIMIT = '4';  // ������������

	const char fvFXOrdType_LIMIT = '2';  // ������
	const char fvFXOrdType_MARKET = '1';  // ���尡

	const char fvErrFlag_YES = 'Y';   // ERROR
	const char fvErrFlag_NO = 'N';   // ����

	//int fvDibStatus_GOOD = 0;    // ����
	//int fvDibStatus_ERROR = -1;   // ����

	const char fvOrdStatus_NOMAL = '0';   // good
	const char fvOrdStatus_PARTIALFILL = '1';   // partially filler
	const char fvOrdStatus_FILL = '2';   // fullly filled
	const char fvOrdStatus_CANCELED = '4';   // canceled
	const char fvOrdStatus_REJECT = '8';   // reject

	const char fvPendingStatus_NOMAL = '0';   // good
	const char fvPendingStatus_REPLACE = '1';   // Pending Replace
	const char fvPendingStatus_CANCEL = '2';   // Pending Cancel

	const char fvPlcType_NEW = '0';   // �ű�
	const char fvPlcType_REP = '1';   // ����
	const char fvPlcType_CXL = '2';   // ���

	const char fvSchOrdStatus_All = '0';   // All
	const char fvSchOrdStatus_Live = '1';   // Live
	const char fvSchOrdStatus_Fill = '2';   // Fill

	const char fvUserRole_ADMIN[] = "9999";   // ������
	const char fvUserRole_SUPERADMIN[] = "9100";   // �ְ������
	const char fvUserRele_TRADER[] = "1000";   // �Ϲݻ����
	const char fvUserRele_PARTNER[] = "2000";   // ��Ʈ��
	const char fvUserRele_PARTNER2[] = "2100";   // ��Ʈ��2
	const char fvUserRele_TESTER[] = "9000";   // �׽���

	const char fvUserStatus_NORMAL = '1';   // ����
	const char fvUserStatus_STOP = '2';   // ���� (�޽�)
	const char fvUserStatus_DEL = '3';   // ���� (����)

	const char fvSchUserStatus_ALL = '0';   // ��ü
	const char fvSchUserStatus_NORMAL = '1';   // ����
	const char fvSchUserStatus_STOP = '2';   // ���� (�޽�)
	const char fvSchUserStatus_DEL = '3';   // ���� (����)
	const char fvSchUserStatus_USEALL = '5';   // ���� + ����
	const char fvSchUserStatus_LIVE = '6';   // �ŸŰ��� �ִ»���ڸ�

	const char fvLicenseType_DAY = '1';   // ��
	const char fvLicenseType_WEEK = '2';   // ��
	const char fvLicenseType_2WEEK = '3';   // 2��
	const char fvLicenseType_MONTH = '4';   // ��

	const char fvSpeedOrdFlag_YES = 'Y';   // SPEED ORDER
	const char fvSpeedOrdFlag_NO = 'N';   // GENERAL ORDER

	const char fvSystemOrdFlag_YES = 'Y';   // System Order
	const char fvSystemOrdFlag_NO = 'N';   // User Order

	const char fvClearCondition_ALL = 'Y';   // ��ü
	const char fvClearCondition_NOTALL = 'N';   // 500,000 ����

	const char fvBankingType_ALL = '0';   // ��ü
	const char fvBankingType_DEPOSIT = '1';   // �Ա�
	const char fvBankingType_WITHDRAW = '2';   // ���
	const char fvBankingType_MILEAGE = '3';   // ���ϸ����Ա�

	const char fvBankingProcFlag_REQ = '0';   // ��û��
	const char fvBankingProcFlag_CMPL = '1';   // ó���Ϸ�
	const char fvBankingProcFlag_REJ = '2';   // �ź�

	const char fvReqProcFlag_REQ = '0';   // ��û��
	const char fvReqProcFlag_CMPL = '1';   // ó���Ϸ�
	const char fvReqProcFlag_REJ = '2';   // �ź�

	const char fvReqType_OVERNIGHT = '9';   // OverNight
	const char fvReqType_ACCSTATUS = '1';   // ���»��º���
	const char fvReqType_LICENSETYPE = '2';   // �̿��Ÿ�Ժ���
	const char fvReqType_LOANCOUNT = '3';   // ���¼�����

	const char fvCmplType_USER_BANKING = '1';   // �Ա�/���
	const char fvCmplType_USER_REQ = '2';   // ���»���/�̿��Ÿ��/���¼�/OverNight

	const char fvCmplDetailType_DEPOSIT = '1'; // �Ա�
	const char fvCmplDetailType_WITHDRAW = '2'; // ���
	const char fvCmplDetailType_ACCSTATUS = '3';
	const char fvCmplDetailType_LICENSETYPE = '4';
	const char fvCmplDetailType_LOANCOUNT = '5';
	const char fvCmplDetailType_OVERNIGHT = '6';

	const char fvMgrBankingType_DEPOSIT = '1'; // ���ű��Ա�
	const char fvMgrBankingType_WITHDRAW = '2'; // ���ű����
	const char fvMgrBankingType_LICENSEWITHDRAW = '3'; // �������
	const char fvMgrBankingType_LOANDEPOSIT = '4'; // �뿩���Ա�
	const char fvMgrBankingType_LOANWITHDRAW = '5'; // �뿩�����
	const char fvMgrBankingType_EVENTDEPOSIT = '6'; // �̺�Ʈ�Ա�
	const char fvMgrBankingType_MILEAGEDEPOSIT = '7'; // ���ϸ����Ա�
	const char fvMgrBankingType_MILEAGEWITHDRAW = '8'; // ���ϸ����Ա�

	const char fvUserManageType_REG = '1';   // ���
	const char fvUserManageType_DEL = '2';   // ����
	const char fvUserManageType_RESET = '3';   // �ʱ�ȭ

	const char fvNoticeManageType_REG = '1';   // ���
	const char fvNoticeManageType_CHG = '2';   // ����
	const char fvNoticeManageType_DEL = '3';   // ����
	
	const char fvRecStatus_INSERT = '1';
	const char fvRecStatus_UPDATE = '2';
	const char fvRecStatus_DELETE = '3';
	
	const char fvSearchTerm_DAY = '1'; // day
	const char fvSearchTerm_WEEK = '2'; // Week
	const char fvSearchTerm_MONTH = '3'; // Moneh
	
	const char fvExecMsgType_RECEIVE = '1'; // ����
	const char fvExecMsgType_REPLACED = '2'; // ����Ȯ��
	const char fvExecMsgType_CANCELED = '3'; // ���Ȯ��
	const char fvExecMsgType_EXECUTION = '4'; // ü��
	const char fvExecMsgType_REJECT = '5'; // �ź�

	const char fvExecMsgName_RECEIVE []=  "����";     // ����
	const char fvExecMsgName_REPLACED[] = "����Ȯ��"; // ����Ȯ��
	const char fvExecMsgName_CANCELED[] = "���Ȯ��"; // ���Ȯ��
	const char fvExecMsgName_EXECUTION[] = "ü��";     // ü��
	const char fvExecMsgName_REJECT[] = "�ź�";     // �ź�

	const char fvExpireFlag_YES = 'Y'; // Expire
	const char fvExpireFlag_NO = 'N'; //

	const char fvTradeLimitFlag_YES = 'Y'; // �ֹ�����
	const char fvTradeLimitFlag_NO = 'N'; // ���Ѿ���

	const char fvLossCutFlag_YES = 'Y'; // LossCut Order
	const char fvLossCutFlag_NO = 'N'; //

	const char fvForciblyOrdFlag_YES = 'Y';
	const char fvForciblyOrdFlag_NO = 'N';

	const int fvBaseEvalMoney = 500000;

	const char fvSymbol_KOSPI200[] = "00800";
	const char fvUpCode_KOSPI200[] = "101";

	const char fvREAL_SERVER = '0';
	const char fvTEST_SERVER = '1';

	const int fvLeverageValue_X1 = 1;
	const int fvLeverageValue_X2 = 2;
	const int fvLeverageValue_X5 = 5;
	const int fvLeverageValue_X10 = 10;
	const int fvLeverageValue_X20 = 20;
	const int fvLeverageValue_X50 = 50;

	const char fvLeverageType_X1 = '0'; // 1
	const char fvLeverageType_X2 = '1'; // 2
	const char fvLeverageType_X5 = '2'; // 5
	const char fvLeverageType_X10 = '3'; // 10
	const char fvLeverageType_X20 = '4'; // 20
	const char fvLeverageType_X50 = '5'; // 50

	const char fvOrderPath_NORMAL = '0'; // REAL
	const char fvOrderPath_VIP = '1'; // VIP

	const char fvTradeProduct_FUT = '1'; //����     100,000
	const char fvTradeProduct_OPT = '2'; //�ɼ�     200,000
	const char fvTradeProduct_FUTOPT = '3'; //�����ɼ� 300,000
	const char fvTradeProduct_FXOPT = '4'; //FX�ɼ�   400,000

	const char fvOrdCondType_OVERNIGHT = '1'; //��������
	const char fvOrdCondType_ONCLOSE = '2'; //���ø���
	const char fvOrdCondType_CMEUSE = '3'; //CME ���
	const char fvOrdCondType_EXPIRECALC = '4'; //��������

	const char fvFXCommType_UNLOCK = '1'; // ����
	const char fvFXCommType_LOCK = '2'; // ����

	const char fvUserGrade_WAIT = '0'; // ���
	const char fvUserGrade_COMPLETE = '9'; // �Ϸ�

	const char fvCloseOrdFlag_YES = 'Y'; // �����ֹ�
	const char fvCloseOrdFlag_NO = 'N'; // �������ֹ�

	const char fvRealOrdFlag_YES = 'Y';  // Real
	const char fvRealOrdFlag_NO = 'N';  // Test

	const char fvOrdHandleType_NORMAL = '0';
	const char fvOrdHandleType_SPEED = '1';
	const char fvOrdHandleType_STOPLOSS = '2';
	const char fvOrdHandleType_MIT = '3';
	const char fvOrdHandleType_LOSSCUT = '4';
	const char fvOrdHandleType_MANAGER = '5';

	// STOP / MIT LOG
	const char fvSTOPSetType_SET = '1';
	const char fvSTOPSetType_RESET = '2';
	const char fvFlag_YES = 'Y';
	const char fvFlag_NO = 'N';
	const char fvMITPlcType_NEW = '0';   // �ű�
	const char fvMITPlcType_REP = '1';   // ����
	const char fvMITPlcType_CXL = '2';   // ���

	// Market Server ����
	const char fvMktSvr_MAIN = 'M';  // Main
	const char fvMktSvr_BACK = 'B';  // Back

	const char fvBreakTimeFlag_YES = 'Y';         //hangseng
	const char fvBreakTimeFlag_NO = 'N';         //hangseng

	

