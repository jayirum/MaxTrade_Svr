// IndexBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ChartAPISample.h"
#include "IndexBar.h"

#define IDC_TREE                30000


// CIndexBar

IMPLEMENT_DYNAMIC(CIndexBar, CDockablePane)

CIndexBar::CIndexBar()
{

}

CIndexBar::~CIndexBar()
{
}


BEGIN_MESSAGE_MAP(CIndexBar, CDockablePane)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblClkTree)
END_MESSAGE_MAP()


// CIndexBar �޽��� ó�����Դϴ�.

int CIndexBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

	// ��ǥ ǥ�ÿ� Ʈ�� ����
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | 
                    TVS_SHOWSELALWAYS | TVS_LINESATROOT;
    m_treIndex.Create( dwStyle, CRect( 0, 0, 0, 0 ), this, IDC_TREE );

	// ��ǥ ǥ�ÿ� Ʈ�� �ʱ�ȭ
    InitTreeCtrl();

    return 0;
}

void CIndexBar::OnDestroy()
{
    CDockablePane::OnDestroy();

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CIndexBar::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

	// ��ǥ ǥ�ÿ� Ʈ���� ����� ����
    m_treIndex.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}

void CIndexBar::OnDblClkTree( NMHDR * pNMHDR, LRESULT * pResult )
{
	// ��ǥ���� ����Ŭ�� ��, ��ǥ ǥ�� ȭ�� ������ �޽��� ����
    HTREEITEM hItem = m_treIndex.GetSelectedItem();
    if (hItem)
    {
        CString strParent, strText;
		HTREEITEM hItemParent = NULL;

		strText		= m_treIndex.GetItemText(hItem);
		hItemParent = m_treIndex.GetParentItem(hItem);

		if (hItemParent)
			strParent = m_treIndex.GetItemText(hItemParent);
		
		int iType=0;
		if (strParent == "��ǥ") 
		{
			iType = 0;
		} 
		else 
		{
			return;
		}

        AfxGetMainWnd()->SendMessage(WM_OPENSCREEN, iType, LPARAM(strText.GetBuffer()));
    }
}

//----------------------------------------------------------------------------------------------------
// ��ǥ ǥ�ÿ� Ʈ����Ʈ���� �ʱ�ȭ�մϴ�
//----------------------------------------------------------------------------------------------------
void CIndexBar::InitTreeCtrl()
{
    HTREEITEM hRoot = m_treIndex.InsertItem( "��ǥ", TVI_ROOT, TVI_LAST );

    m_treIndex.InsertItem( IDS_INDEX01, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX02, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX03, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX04, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX05, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX06, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX07, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX08, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX09, hRoot, TVI_LAST );
    m_treIndex.InsertItem( IDS_INDEX10, hRoot, TVI_LAST );

    m_treIndex.Expand( hRoot, TVE_EXPAND );
}
