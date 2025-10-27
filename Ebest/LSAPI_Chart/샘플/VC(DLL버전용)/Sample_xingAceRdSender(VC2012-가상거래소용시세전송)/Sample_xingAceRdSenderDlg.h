
// Sample_xingAceRdSenderDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "IXingAceRdSender.h"

// CSample_xingAceRdSenderDlg ��ȭ ����
class CSample_xingAceRdSenderDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CSample_xingAceRdSenderDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SAMPLE_XINGACERDSENDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	//-----------------------------------------------------------------------
	// ����

	BOOL				m_bAttr;				// TR List ���� ������ TR�� Attribute�� ������ �ִ��� ����
	IXingAceRdSender	m_iSender;				// xingAceRdSender API ��ü
	int					m_nSendIndex;			// �������϶� ���� ������ ����Ʈ�� �ε���

	//-----------------------------------------------------------------------
	// �Լ�
	void		InitControl	();
	void		InitTrData	();

	void		ProcessTR	( int nSelect );

	void		SendRealData();


	// ������ �޽��� �� �Լ�
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
