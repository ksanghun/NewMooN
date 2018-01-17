// ZListCtrlLog.cpp : implementation file
//

#include "stdafx.h"
#include "ZListCtrlLog.h"
#include "resource.h"
#include "MooNView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CZListCtrlLog, CListCtrl)

CZListCtrlLog::CZListCtrlLog()
{
	m_nCulNum = m_nRecordNum = 0;
	m_SelectionFlag = FALSE;
	m_selItem = -1;
	m_strSearchId = L"";

	m_currDrawId = 0;
	m_colorid = 0;
}

CZListCtrlLog::~CZListCtrlLog()
{
}


BEGIN_MESSAGE_MAP(CZListCtrlLog, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CZListCtrlLog::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CZListCtrlLog::OnNMCustomdraw)
	ON_MESSAGE(MSG_POST_SUBCLASS_LISTVIEW, OnPostSubclassListview)

	ON_NOTIFY_REFLECT(NM_DBLCLK, &CZListCtrlLog::OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CZListCtrlLog::OnLvnItemchanged)

//	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);

	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CZListCtrlLog::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	DWORD dwStyles = GetExtendedStyle();
	dwStyles |= LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwStyles);

	PostMessage(MSG_POST_SUBCLASS_LISTVIEW, 0L, 0L);
}



LRESULT CZListCtrlLog::OnPostSubclassListview(WPARAM wParam, LPARAM lParam)
{
	m_ctlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
	m_ctlHeader.Invalidate(TRUE);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawHeaderCtrl

CZListCtrlLog::CCustomDrawHeaderCtrl::CCustomDrawHeaderCtrl()
{
	m_brBackground.CreateSolidBrush(RGB(200, 100, 50));
}

CZListCtrlLog::CCustomDrawHeaderCtrl::~CCustomDrawHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CZListCtrlLog::CCustomDrawHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CListCtrlColoredHeader::CCustomDrawHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, CZListCtrlLog::CCustomDrawHeaderCtrl::OnCustomdraw)
	ON_MESSAGE(WM_ERASEBKGND, CZListCtrlLog::CCustomDrawHeaderCtrl::OnEraseBkgndOverride)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawHeaderCtrl message handlers


// CZListCtrlLog message handlers
void CZListCtrlLog::InitListCtrl()
{
	m_Edit.Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL , CRect(0, 0, 800, 500), this, NULL);
	m_Edit.ShowWindow(SW_HIDE);	
}


void CZListCtrlLog::ResetListCtrl()
{
	DeleteAllItems();
	m_nRecordNum = 0;

}

void CZListCtrlLog::AddListToTraining()
{
	for (int row = 0; row < GetItemCount(); ++row)
	{
		int pageId = _ttoi(GetItemText(row, 13));
		int matchId = _ttoi(GetItemText(row, 14));
		CString strCode = GetItemText(row, 1);

		pView->AddListToTraining(pageId, matchId, strCode);
	}
}


void CZListCtrlLog::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//// TODO: Add your control notification handler code here
	//m_currDrawId = 0;
	//m_colorid = 0;

	//Invalidate();
	//HWND hWnd1 = GetSafeHwnd();
	//LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	//RECT rect;
	////get the row number
	//nItem = temp->iItem;
	////get the column number
	//nSubItem = temp->iSubItem;

	//// Update Camera Pos //
	//pView->SetPositionByList(GetItemText(nItem, 13), GetItemText(nItem, 14));

	//if (nSubItem != 1) {
	//	*pResult = 0;		
	//	UpdateCodeValue();
	//	m_Edit.ShowWindow(SW_HIDE);
	//	return;
	//}

	////Retrieve the text of the selected subItem from the list
	//CString str = GetItemText(nItem, nSubItem);

	//RECT rect1, rect2;
	//// this macro is used to retrieve the Rectanle of the selected SubItem
	//ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	////Get the Rectange of the listControl
	//::GetWindowRect(temp->hdr.hwndFrom, &rect1);
	////Get the Rectange of the Dialog
	//::GetWindowRect(m_hWnd, &rect2);

	//int x = rect1.left - rect2.left;
	//int y = rect1.top - rect2.top;

	//if (nItem != -1) {
	//	m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
	//	m_Edit.ShowWindow(SW_SHOW);
	//	m_Edit.SetFocus();
	//	::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
	//	m_Edit.SetWindowTextW(str);
	//	m_selItem = nItem;
	//	m_strSearchId = GetItemText(nItem, 3);
	//}

	//*pResult = 0;
}

void CZListCtrlLog::UpdateCodeValue()
{
	if (m_selItem >= 0) {
		Invalidate();
		CString str;
		m_Edit.GetWindowText(str);	

		// Update all rows===========================//
		if (m_bIsAutoFill) {
			for (int row = 0; row < GetItemCount(); ++row)
			{
				if (GetItemText(row, 3) == m_strSearchId) {
					SetItem(row, 1, LVIF_TEXT, str, 0, 0, 0, NULL);
				}
			}
		}
		else {
			SetItem(m_selItem, 1, LVIF_TEXT, str, 0, 0, 0, NULL);
		}
		//===========================================//

		//m_selItem = -1;
		//m_strSearchId = L"";
	}
}

BOOL CZListCtrlLog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_currDrawId = 0;
	m_colorid = 0;

	if (pMsg->message == WM_KEYDOWN)
	{
		

		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			UpdateCodeValue();

			SetItemState(m_selItem, 0, LVIS_FOCUSED | LVIS_SELECTED);



			// fine next selitem //
			int listcnt = GetItemCount();
			int sid = _wtoi(GetItemText(m_selItem, 3));
			while (m_selItem < (listcnt-1)) {
				m_selItem++;
				int nextId = _wtoi(GetItemText(m_selItem, 3));
				if (sid != nextId) {					
					break;
				}
			}

			int moveid = m_selItem + 5;
			if (moveid >= listcnt)
				moveid = listcnt - 1;

			EnsureVisible(moveid, TRUE);
			SetItemState(m_selItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			//	SetFocus();
			//EnsureVisible(m_selItem + 10, TRUE);




			//Retrieve the text of the selected subItem from the list
			//Invalidate();
			//HWND hWnd1 = GetSafeHwnd();
			//CString str = GetItemText(nItem, nSubItem);

			//RECT rect, rect1, rect2;
			//// this macro is used to retrieve the Rectanle of the selected SubItem
			//ListView_GetSubItemRect(hWnd1, m_selItem, 1, LVIR_BOUNDS, &rect);
			////Get the Rectange of the listControl
			//::GetWindowRect(temp->hdr.hwndFrom, &rect1);
			////Get the Rectange of the Dialog
			//::GetWindowRect(m_hWnd, &rect2);

			//int x = rect1.left - rect2.left;
			//int y = rect1.top - rect2.top;

			//if (nItem != -1) {
			//	m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
			//	m_Edit.ShowWindow(SW_SHOW);
			//	m_Edit.SetFocus();
			//	::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
			//	m_Edit.SetWindowTextW(str);
			//	m_selItem = nItem;
			//	m_strSearchId = GetItemText(nItem, 3);
			//}






		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CZListCtrlLog::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW  pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = CDRF_DODEFAULT;
	//obtain row and column of item

	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.
		// We'll cycle the colors through red, green, and light blue.

		int itemid = (int)(pNMCD->nmcd).dwItemSpec; //this is item id
		int sid = _wtoi(GetItemText(itemid, 3));
		int confi = _wtoi(GetItemText(itemid, 2));

		//if (confi > 75)
		//	pNMCD->clrText = RGB(0, 0, 255);

		//else if ((confi > 65) && (confi <= 75))
		//	pNMCD->clrText = RGB(0, 200, 0);

		//else if ((confi > 55) && (confi <= 65))
		//	pNMCD->clrText = RGB(255, 125, 0);

		//else 
		//	pNMCD->clrText = RGB(255, 0, 0);



		// change background //
		//if (sid % 2 == 0) {
		//	pNMCD->clrText = RGB(0, 0, 0);
		//	pNMCD->clrTextBk = RGB(210, 210, 250);
		//}
		//else {
		//	pNMCD->clrText = RGB(0, 0, 0);
		//	pNMCD->clrTextBk = RGB(210, 250, 210);
		//}

		pNMCD->clrText = RGB(0, 0, 0);
		if (sid != m_currDrawId) {
			// change color//
			m_currDrawId = sid;
			m_colorid++;
		}

		if (m_colorid % 2 == 0) {
			if (confi < 75) 		pNMCD->clrText = RGB(255, 0, 0);
			else					pNMCD->clrText = RGB(0, 0, 0);

			pNMCD->clrTextBk = RGB(200, 200, 200);
		}
		else {
			if (confi < 75) 		pNMCD->clrText = RGB(255, 0, 0);
			else					pNMCD->clrText = RGB(0, 0, 0);

			pNMCD->clrTextBk = RGB(250, 250, 250);
		}

		


		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
	}


	////Remove standard highlighting of selected (sub)item.
	//pNMCD->nmcd.uItemState = CDIS_DEFAULT;

	//switch (pNMCD->nmcd.dwDrawStage)
	//{
	//	case  CDDS_PREPAINT:  // First stage (for the whole control)
	//	{

	//		*pResult = CDRF_NOTIFYITEMDRAW;

	//	}
	//	break;

	//	case CDDS_ITEMPREPAINT:
	//	{

	//		*pResult = CDRF_NOTIFYSUBITEMDRAW;

	//	}
	//	break;

	//case  CDDS_ITEMPREPAINT | CDDS_SUBITEM: // Stage three 
	//{

	//	//if (sub)item is of interest, set custom text/background color
	//	int itemid = (int)(pNMCD->nmcd).dwItemSpec; //this is item id
	//	int sid = _wtoi(GetItemText(itemid, 2));
	//	int confi = _wtoi(GetItemText(itemid, 1));

	//	if (confi > 75) {
	//		pNMCD->clrText = RGB(0, 0, 255);
	//		//	pNMCD->clrTextBk = RGB(200, 255, 200);
	//	}
	//	else if ((confi > 65) && (confi <= 75))
	//	{
	//		pNMCD->clrText = RGB(0, 255, 0);
	//		//	pNMCD->clrTextBk = RGB(255, 255, 200);
	//	}

	//	else if ((confi > 55) && (confi <= 65))
	//	{
	//		pNMCD->clrText = RGB(255, 125, 0);
	//		//	pNMCD->clrTextBk = RGB(255, 200, 200);
	//	}

	//	else {
	//		pNMCD->clrText = RGB(255, 0, 0);
	//		//	pNMCD->clrTextBk = RGB(255, 150, 150);
	//	}

	//	//if (sid % 2 == 0){
	//	//	pNMCD->clrText = RGB(0, 0, 0);
	//	//	pNMCD->clrTextBk = RGB(210, 210, 210);
	//	//}
	//	//else{
	//	//	pNMCD->clrText = RGB(0, 0, 0);
	//	//	pNMCD->clrTextBk = RGB(250, 250, 250);
	//	//}
	//	*pResult = CDRF_NOTIFYPOSTPAINT;

	//}
	//break;


	//case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
	//{

	//}
	//break;

	//default:// it wasn't a notification that was interesting to us.
	//{
	//	*pResult = CDRF_DODEFAULT;
	//}
	//break;
	//}

}


void CZListCtrlLog::CCustomDrawHeaderCtrl::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pCDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	// Take the default processing unless we set this to something else below

	*pResult = CDRF_DODEFAULT;

	switch (pCDraw->dwDrawStage)
	{
	case CDDS_PREPAINT:
	{
		// the item's prepaint stage: return CDRF_NOTIFYITEMDRAW so that we 
		// get future WM_CUSTOMDRAW messages
		*pResult = CDRF_NOTIFYITEMDRAW;

		break;
	}

	case CDDS_ITEMPREPAINT:
	{
		// the item's pre-paint stage: change the DC settings to get needed text colors
		CDC dc;
		dc.Attach(pCDraw->hdc);		

		dc.SetTextColor(RGB(0, 0, 255));
	
		dc.Detach();

		*pResult = CDRF_DODEFAULT;

		break;
	}

	case CDDS_ITEMPOSTPAINT:
	{
		// the item's postpaint stage: draw on top of whatever is there

		*pResult = CDRF_DODEFAULT;

		break;
	}
	}

}

LRESULT CZListCtrlLog::CCustomDrawHeaderCtrl::OnEraseBkgndOverride(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void CZListCtrlLog::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item

	TCHAR  lpBuffer[256];

	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = lpDrawItemStruct->itemID;
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;


	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nCol;
	if (!m_SelectionFlag)
	{
		for (nCol = 0; GetColumn(nCol, &lvc); nCol++)
		{
			if (nCol>0)
			{
				GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_BOUNDS, m_SelectionRect);
			}
			else
			{
				GetItemRect(lpDrawItemStruct->itemID, m_SelectionRect, LVIR_BOUNDS);
				m_SelectionRect.right = GetColumnWidth(0);
				m_SelectionRect.left = 0;
			}
			if (m_SelectionRect.PtInRect(m_Point))
			{
				m_SelectionFlag = TRUE;
				//				TRACE("\n***%d,%d",lpDrawItemStruct->itemID,nCol);
				break;
			}
			else
				m_SelectionFlag = FALSE;

		}



		if ((lpDrawItemStruct->itemState & ODS_SELECTED) && m_SelectionFlag)
		{
			CRect rc = lpDrawItemStruct->rcItem;

			rc.left = m_SelectionRect.left;
			rc.right = m_SelectionRect.right;

			pDC->FillSolidRect(&rc, RGB(255, 0, 0));//GetSysColor(COLOR_HIGHLIGHT)) ; 
			//pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
		}
		else
		{
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_WINDOW));
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}

	}

	for (nCol = 0; GetColumn(nCol, &lvc); nCol++)
	{
		if (nCol > 0)
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol - 1, &lvcprev);
			lpDrawItemStruct->rcItem.left += lvcprev.cx;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left;
		}

		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT		uFormat = DT_LEFT;

		::DrawText(lpDrawItemStruct->hDC, lpBuffer, (int)wcslen(lpBuffer), &lpDrawItemStruct->rcItem, uFormat);

		pDC->SelectStockObject(SYSTEM_FONT);
	}
}



BOOL CZListCtrlLog::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	//cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL | WS_BORDER;

	return CListCtrl::PreCreateWindow(cs);
}




void CZListCtrlLog::AddUserColumn(CString strLable, unsigned short colWidth)
{
	InsertColumn(m_nCulNum, strLable, LVCFMT_LEFT, colWidth, -1);
	m_nCulNum++;
}
void CZListCtrlLog::AddRecode()
{
	m_currDrawId = 0;
	m_colorid = 0;
	//CString strItem;
	//std::map<unsigned long, MATCHGROUP>& matches = SINGLETON_TMat::GetInstance()->GetMatchResults();
	//std::map<unsigned long, MATCHGROUP>::iterator iter_gr = matches.begin();

	//for (; iter_gr != matches.end(); iter_gr++){

	//	for (int i = 0; i < iter_gr->second.matche.size(); i++){

	//	//	SetItem(m_nRecordNum, , LVIF_TEXT, L"-", 0, 0, 0, NULL);

	//	//	InsertItem(m_nRecordNum, L"");
	//		SetItem(m_nRecordNum, 0, LVIF_TEXT, L"-", 0, 0, 0, NULL);


	//		strItem.Format(L"%d", iter_gr->second.searchId);
	//		SetItem(m_nRecordNum, 1, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%u", iter_gr->second.matche[i].cutId);
	//		SetItem(m_nRecordNum, 2, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%u", iter_gr->second.matche[i].fileId);
	//		SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%u", iter_gr->second.matche[i].posId);
	//		SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%d%u", (int)(iter_gr->second.matche[i].accuracy * 100),iter_gr->second.matche[i].matchId);
	//		SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%u", iter_gr->second.matche[i].matchFile);
	//		SetItem(m_nRecordNum, 6, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%u", iter_gr->second.matche[i].matchPos);
	//		SetItem(m_nRecordNum, 7, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%3.2f", iter_gr->second.matche[i].fTh);
	//		SetItem(m_nRecordNum, 8, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		strItem.Format(L"%3.2f", iter_gr->second.matche[i].accuracy);
	//		SetItem(m_nRecordNum, 9, LVIF_TEXT, strItem, 0, 0, 0, NULL);

	//		

	//		SetItem(m_nRecordNum, 10, LVIF_TEXT, iter_gr->second.matche[i].strBase64, 0, 0, 0, NULL);

	//		m_nRecordNum++;
	//	}
	//	
	//}

}

void CZListCtrlLog::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_currDrawId = 0;
	m_colorid = 0;

	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//// TODO: Add your control notification handler code here
	//Invalidate();
	//HWND hWnd1 = GetSafeHwnd();
	//LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	//RECT rect;
	////get the row number
	//nItem = temp->iItem;
	////get the column number
	//nSubItem = temp->iSubItem;
	////if (nSubItem == 0 || nSubItem == -1 || nItem == -1){
	////	*pResult = 0;
	////	m_Edit.ShowWindow(SW_HIDE);
	////	return;
	////}

	//if (nSubItem !=1){
	//	*pResult = 0;
	//	m_Edit.ShowWindow(SW_HIDE);
	//	return;
	//}

	////Retrieve the text of the selected subItem from the list
	//CString str = GetItemText(nItem, nSubItem);

	//RECT rect1, rect2;
	//// this macro is used to retrieve the Rectanle of the selected SubItem
	//ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	////Get the Rectange of the listControl
	//::GetWindowRect(temp->hdr.hwndFrom, &rect1);
	////Get the Rectange of the Dialog
	//::GetWindowRect(m_hWnd, &rect2);

	//int x = rect1.left - rect2.left;
	//int y = rect1.top - rect2.top;	



	//if (nItem != -1)
	//	m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
	//m_Edit.ShowWindow(SW_SHOW);
	//m_Edit.SetFocus();
	//::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
	//m_Edit.SetWindowTextW(str);


	*pResult = 0;
}


void CZListCtrlLog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//m_Point = point;
	//m_SelectionFlag = FALSE;
	//Invalidate();

	//CString str;
	//m_Edit.GetWindowText(str);
	//SetItem(nItem, nSubItem, LVIF_TEXT, str, 0, 0, 0, NULL);
//	m_Edit.ShowWindow(SW_HIDE);

	CListCtrl::OnLButtonDown(nFlags, point);
}


void CZListCtrlLog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	//CMenu menu;
	//menu.LoadMenuW(IDR_POPUP_MENU);
	//CMenu* pMenu = menu.GetSubMenu(0);
	//pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}


void CZListCtrlLog::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_currDrawId = 0;
	m_colorid = 0;

	Invalidate();
	HWND hWnd1 = GetSafeHwnd();
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	RECT rect;
	//get the row number
	nItem = temp->iItem;
	//get the column number
	//nSubItem = temp->iSubItem;
	nSubItem = 1;

	// Update Camera Pos //
	pView->SetPositionByList(GetItemText(nItem, 13), GetItemText(nItem, 14));

	//if (nSubItem != 1) {
	//	*pResult = 0;
	//	UpdateCodeValue();
	//	m_Edit.ShowWindow(SW_HIDE);
	//	return;
	//}
	
	//EnsureVisible(nItem, TRUE);
	//Invalidate();

	//Retrieve the text of the selected subItem from the list
	CString str = GetItemText(nItem, nSubItem);

	RECT rect1, rect2;
	// this macro is used to retrieve the Rectanle of the selected SubItem
//	ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	ListView_GetSubItemRect(hWnd1, temp->iItem, nSubItem, LVIR_BOUNDS, &rect);
	//Get the Rectange of the listControl
	::GetWindowRect(temp->hdr.hwndFrom, &rect1);
	//Get the Rectange of the Dialog
	::GetWindowRect(m_hWnd, &rect2);

	int x = rect1.left - rect2.left;
	int y = rect1.top - rect2.top;

	if (nItem != -1) {
		m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
		m_Edit.ShowWindow(SW_SHOW);
		m_Edit.SetFocus();
		::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
		m_Edit.SetWindowTextW(str);
		
		DWORD dwSel = m_Edit.GetSel();
		m_Edit.SetSel(HIWORD(dwSel), -1);

		m_selItem = nItem;
		m_strSearchId = GetItemText(nItem, 3);
	}
	else {
		*pResult = 0;
		UpdateCodeValue();
		m_Edit.ShowWindow(SW_HIDE);
		return;
	}



	


	*pResult = 0;
}


void CZListCtrlLog::SelItemByLineTextBoxID(int _id)
{
	m_currDrawId = 0;
	m_colorid = 0;


	int cnt = GetItemCount();
	for (int i = 0; i < cnt; i++) {
		CString strId = GetItemText(i, 15);
		int id = _ttoi(strId);
		if (id == _id) {
			SetItemState(m_selItem, 0, LVIS_FOCUSED | LVIS_SELECTED);
			m_selItem = i;
			break;
		}
	}

	
	EnsureVisible(m_selItem, TRUE);
	SetItemState(m_selItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	
}