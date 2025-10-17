
// Sample_xingAceRdSenderDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "IXingAceRdSender.h"

// CSample_xingAceRdSenderDlg 대화 상자
class CSample_xingAceRdSenderDlg : public CDialogEx
{
// 생성입니다.
public:
	CSample_xingAceRdSenderDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SAMPLE_XINGACERDSENDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	//-----------------------------------------------------------------------
	// 변수

	BOOL				m_bAttr;				// TR List 에서 선택한 TR이 Attribute를 가지고 있는지 여부
	IXingAceRdSender	m_iSender;				// xingAceRdSender API 객체
	int					m_nSendIndex;			// 전송중일때 다음 전송할 리스트의 인덱스

	//-----------------------------------------------------------------------
	// 함수
	void		InitControl	();
	void		InitTrData	();

	void		ProcessTR	( int nSelect );

	void		SendRealData();


	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlTrList;
	CListCtrl m_ctrlSendList;
	CListCtrl m_ctrlOutBlockList;
	CListCtrl m_ctrlInBlockList;
	afx_msg void OnItemchangedListTr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedButtonUpdateInblock();
	afx_msg void OnClickedButtonUpdateOutblock();
	virtual void OnOK();
	afx_msg void OnClickedButtonInsertSendlist();
	afx_msg void OnClickedButtonSend();
	afx_msg void OnClose();
	afx_msg void OnClickedButtonSendListUp();
	afx_msg void OnClickedButtonSendListDown();
	afx_msg void OnClickedButtonSendListClear();
	afx_msg void OnClickedButtonSendListDel();
	afx_msg void OnClickedButtonSendListAllDel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedButtonSendListLoad();
	afx_msg void OnClickedButtonSendListSave();
};
