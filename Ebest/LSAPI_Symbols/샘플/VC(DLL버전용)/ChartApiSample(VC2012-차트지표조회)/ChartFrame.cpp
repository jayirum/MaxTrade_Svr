// ChartFrame.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "ChartFrame.h"
#include "IndexView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CChartFrame

IMPLEMENT_DYNCREATE(CChartFrame, CMDIChildWnd)

CChartFrame::CChartFrame()
{
    m_pwndView  = NULL  ;
}

CChartFrame::~CChartFrame()
{
}


BEGIN_MESSAGE_MAP(CChartFrame, CMDIChildWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CChartFrame 메시지 처리기입니다.

int CChartFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void CChartFrame::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWnd::OnSize(nType, cx, cy);

	// 프레임에 맞게 IndexView의 사이즈를 조절
    if (m_pwndView)
        m_pwndView->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );
}


//----------------------------------------------------------------------------------------------------
// ChartFrame에 IndexView를 표시합니다
//----------------------------------------------------------------------------------------------------
void CChartFrame::SetIndexName(int iType, LPCTSTR pszName)
{
	// IndexView를 생성합니다
    CView *pView = NULL;
    pView = (CView *)RUNTIME_CLASS(CIndexView)->CreateObject();
	ASSERT(pView);
    pView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect( 0, 0, 0, 0 ), this, 0, NULL);

	// 생성된 IndexView를 초기화합니다
    CIndexView * pIndexView = ( CIndexView* )pView;
    pIndexView->SetIndexName(iType, pszName );
    pIndexView->OnInitialUpdate();

    m_pwndView = pView;
}
