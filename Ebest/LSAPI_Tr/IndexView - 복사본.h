#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"


// CIndexView �� ���Դϴ�.

class CIndexView : public CFormView
{
	DECLARE_DYNCREATE(CIndexView)

public:
    void			SetIndexName(int iInexType,  LPCTSTR pszName);

private:
	enum {CHART_INDEX_ };

	//------------------------------------------------------------
	// ��ǥ��
    CString			m_strIndexName;

	//------------------------------------------------------------
	// ǥ���ϴ� ��� �÷� ��
    int				m_nColCount;

	//------------------------------------------------------------
	// ������ ������ �ð����� �����͸� �̿��� ��ǥ�����͸� �������� ����
	// false : ChartIndex TR ��ȸ
	// true  : ���������͸� �̿��� ��ǥ������ ��ȸ
	BOOL			m_bExcelData;			

	//------------------------------------------------------------
	// ��ȸ(�Ǵ� �ǽð�) ���� �������� ������ ǥ�� ��ġ
	// �űԷ� ǥ������, ������ ��ġ�� ������Ʈ ������ �Ǵ��� �� �ִ�
	int				m_nCurrentPos;

	//------------------------------------------------------------
	// ��Ÿ
	CStringArray	m_arrTRCode;	// TR�ڵ� ����Ʈ
	CStringArray	m_arrIndexID;	// ��ǥID ����Ʈ

private:
	void			InitIndexList();										// ��ǥǥ�� ����Ʈ �ʱ�ȭ
	BOOL			ReceiveChartIndex(LPRECV_PACKET pPKData);				// ��ȸ ��� ǥ��
	void			InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock);	// ��ǥ ǥ�� ����Ʈ�� ��ȿ�� �÷��� ǥ��
    void			InsertData(int i, ChartIndexOutBlock1 *pBlock);			// ��ǥ ǥ�� ����Ʈ�� �����͸� ǥ��

protected:
	CIndexView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CIndexView();

public:
	enum { IDD = IDD_INDEX11 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()
public:
    virtual void	OnInitialUpdate();
    afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void	OnDestroy();
    afx_msg void	OnSize(UINT nType, int cx, int cy);
    afx_msg void	OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnBnClickedCheckExcel();
	afx_msg void	OnBtnQuery();
    afx_msg	LRESULT	OnXMReceiveData(WPARAM wParam, LPARAM lParam);
    afx_msg	LRESULT	OnXMTimeoutData(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnXMReceiveRealData(WPARAM wParam, LPARAM lParam);

	CComboBox		m_cmbMarket;
    CEdit			m_edtCode;
    CComboBox		m_cmbPeriod;
    CEdit			m_edtCount;
    CDateTimeCtrl	m_datEnd;
	CEdit			m_edtXlsFile;
    CListCtrl		m_ctlList;
	CStatic			m_stMsg;
};
