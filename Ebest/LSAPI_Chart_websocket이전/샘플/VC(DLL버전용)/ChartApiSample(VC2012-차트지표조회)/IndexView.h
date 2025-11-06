#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"


// CIndexView 폼 뷰입니다.

class CIndexView : public CFormView
{
	DECLARE_DYNCREATE(CIndexView)

public:
    void			SetIndexName(int iInexType,  LPCTSTR pszName);

private:
	enum {CHART_INDEX_ };

	//------------------------------------------------------------
	// 지표명
    CString			m_strIndexName;

	//------------------------------------------------------------
	// 표시하는 헤더 컬럼 수
    int				m_nColCount;

	//------------------------------------------------------------
	// 엑셀에 저장한 시고저종 데이터를 이용해 지표데이터를 가공할지 여부
	// false : ChartIndex TR 조회
	// true  : 엑셀데이터를 이용해 지표데이터 조회
	BOOL			m_bExcelData;			

	//------------------------------------------------------------
	// 조회(또는 실시간) 수신 데이터의 마지막 표시 위치
	// 신규로 표시할지, 기존의 위치에 업데이트 할지를 판단할 수 있다
	int				m_nCurrentPos;

	//------------------------------------------------------------
	// 기타
	CStringArray	m_arrTRCode;	// TR코드 리스트
	CStringArray	m_arrIndexID;	// 지표ID 리스트

private:
	void			InitIndexList();										// 지표표시 리스트 초기화
	BOOL			ReceiveChartIndex(LPRECV_PACKET pPKData);				// 조회 결과 표시
	void			InsertColumn(int nColCnt, ChartIndexOutBlock1 *pBlock);	// 지표 표시 리스트에 유효한 컬럼을 표시
    void			InsertData(int i, ChartIndexOutBlock1 *pBlock);			// 지표 표시 리스트에 데이터를 표시

protected:
	CIndexView();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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
