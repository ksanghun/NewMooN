// ZEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "ZEdit.h"


// CZEdit

IMPLEMENT_DYNAMIC(CZEdit, CEdit)

CZEdit::CZEdit()
{

}

CZEdit::~CZEdit()
{
}


BEGIN_MESSAGE_MAP(CZEdit, CEdit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CZEdit message handlers




void CZEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenuW(IDR_POPUP_LIST);
	CMenu* pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}
