
// MooNView.cpp : implementation of the CMooNView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MooN.h"
#endif

#include "MooNDoc.h"
#include "MooNView.h"

#include "DragDropTreeCtrl.h"
#include "MainFrm.h"
#include "MNDataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMooNView

IMPLEMENT_DYNCREATE(CMooNView, CView)

BEGIN_MESSAGE_MAP(CMooNView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMooNView construction/destruction
CMooNView* pView = NULL;;
CMooNView::CMooNView()
{
	// TODO: add construction code here
	pView = this;
	m_pViewImage = NULL;

}

CMooNView::~CMooNView()
{
	if (m_pViewImage) {
		delete m_pViewImage;
	}
}

BOOL CMooNView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMooNView drawing

void CMooNView::OnDraw(CDC* /*pDC*/)
{
}

void CMooNView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMooNView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
//#endif
}


// CMooNView diagnostics

#ifdef _DEBUG
void CMooNView::AssertValid() const
{
	CView::AssertValid();
}

void CMooNView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


// CMooNView message handlers


void CMooNView::SetTreeDragItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	int addcnt = 0;
	AddImageData(hItem, pCtrl, addcnt);


	CString str;
	str.Format(L"%d image(s) added", addcnt);
	pM->AddOutputString(str, false);

	if (m_pViewImage) {
		m_pViewImage->ProcGenerateThumbnail();
	}
}


void CMooNView::AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt)
{
	USES_CONVERSION;	char* sz = 0;

	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem == NULL) {  // No Child!! File
		
		HTREEITEM pItem = pCtrl->GetParentItem(_item);

		pCtrl->SetItemImage(_item, 4, 4);
		strName = pCtrl->GetItemText(_item);
		strPName = pCtrl->GetItemFullPath(pItem);
		strPath = pCtrl->GetItemFullPath(_item);

		sz = T2A(strPName);
		pCode = getHashCode(sz);
		sz = T2A(strPath);
		cCode = getHashCode(sz);

		// Add Image Data //
		
		SINGLETON_DataMng::GetInstance()->PushImageDataSet(strPath, strPName, strName, cCode, pCode);		
		cnt++;

	}
	else {		// Has Child : Folder //
		strPName = pCtrl->GetItemFullPath(_item);
		sz = T2A(strPName);
		pCode = getHashCode(sz);
		// Change Item Status //
		pCtrl->SetItemImage(_item, 1, 1);
		while (hChildItem) {
			HTREEITEM cItem = pCtrl->GetChildItem(hChildItem);


			if (cItem == NULL) { // File //

				pCtrl->SetItemImage(hChildItem, 4, 4);
				
				strName = pCtrl->GetItemText(hChildItem);
				strPath = pCtrl->GetItemFullPath(hChildItem);
				//==================================//
				char* sz = T2A(strPath);
				cCode = getHashCode(sz);
				// Add Image Data //
				SINGLETON_DataMng::GetInstance()->PushImageDataSet(strPath, strPName, strName, cCode, pCode);
				cnt++;
			}

			else {
				AddImageData(hChildItem, pCtrl, cnt);
			}
			hChildItem = pCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

short CMooNView::ProcSetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
{
	if (m_pViewImage) {
		m_pViewImage->InitCamera();
	}
	float offset = 0.0f;
	return SetSelectedItem(hItem, pCtrl, offset);
}

short CMooNView::SetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl, float& offset)
{
	short res = false;
	USES_CONVERSION;	char* sz = 0;
	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(hItem);

	if (hChildItem != NULL) {  // Has Child!! Folder  --" change image for cut&search mode
		strName = pCtrl->GetItemText(hItem);
		strPath = pCtrl->GetItemFullPath(hItem);

		sz = T2A(strPath);
		cCode = getHashCode(sz);
		// Set Page Selected //
		res = SINGLETON_DataMng::GetInstance()->SelectPages(cCode);
	}

	else {		// File - Select a image //
		strPath = pCtrl->GetItemFullPath(hItem);
		sz = T2A(strPath);
		cCode = getHashCode(sz);
		//		SINGLETON_TMat::GetInstance()->SelectPages(cCode);
		//while (hChildItem){
		//	HTREEITEM cItem = pCtrl->GetChildItem(hChildItem);

		//	if (cItem == NULL){ // File //
		//		strName = pCtrl->GetItemText(hChildItem);
		//		strPath = pCtrl->GetItemFullPath(hChildItem);
		//		//==================================//
		//		char* sz = T2A(strPath);			
		//		cCode = getHashCode(sz);
		//		// Set Page Selected //
		//		SINGLETON_TMat::GetInstance()->SelectPages(cCode, pCode);
		//	}
		//	else{
		//		SetSelectedItem(hChildItem, pCtrl, offset);
		//	}
		//	hChildItem = pCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		//}
	}
	return res;
}

int CMooNView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_ctrlTab.Create(CMFCTabCtrl::STYLE_3D, rectDummy, this, 0x00))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	CRect cRect;
	GetWindowRect(&cRect);
	if (m_pViewImage == NULL) {
		m_pViewImage = new CMNView;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewImage->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, &m_ctrlTab, 0x01);
		m_pViewImage->InitGLview(0, 0);
	}

	m_ctrlTab.AddTab(m_pViewImage, L"Image View", (UINT)0);
	return 0;
}


void CMooNView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_ctrlTab.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlTab.SetLocation(CMFCTabCtrl::LOCATION_TOP);
}




BOOL CMooNView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pViewImage) {
		m_pViewImage->MouseWheel(zDelta);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMooNView::InitCamera(bool bInit)
{
	if (m_pViewImage) {
		m_pViewImage->InitCamera(bInit);
	}
}

void CMooNView::RemoveImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl)
{
	USES_CONVERSION;	char* sz = 0;
	CString strPath, strPName, strName;
	unsigned long pCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem) {
		pCtrl->SetItemImage(_item, 0, 0);

		strPName = pCtrl->GetItemFullPath(_item);
		sz = T2A(strPName);
		pCode = getHashCode(sz);

		SINGLETON_DataMng::GetInstance()->PopImageDataSet(pCode);
	}
}

void CMooNView::EnableCutSearchMode()
{
	if (m_pViewImage) {
		m_pViewImage->EnableCutSearchMode(true, false);
		m_pViewImage->SendMessage(WM_SETCURSOR);
	}
}

void CMooNView::DoCurNSearch()
{
	if ((m_pViewImage)) {
		m_pViewImage->ProcDoSearch();
	}
}

void CMooNView::SetPositionByList(CString strPid, CString strMid)
{
	CMNPageObject* pPage = SINGLETON_DataMng::GetInstance()->GetPageByID(_ttoi(strPid));
	if (pPage) {
		POINT3D tPos;
		int mid = _ttoi(strMid);
		if(pPage->GetPosByMatchID(mid, tPos)){
			pPage->SetSelMatchItem(mid);
			m_pViewImage->MoveCameraPos(tPos, 200);
		}
	}
}

//void CMooNView::SetThresholdValue(float _th)
//{
//	if(m_pViewImage)
//		m_pViewImage->SetThreshold(_th);
//
//}
//void CMooNView::SetResultColor(int R, int G, int B)
//{
//	if (m_pViewImage) {
//		POINT3D color;
//		mtSetPoint3D(&color, (float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f);
//		m_pViewImage->SetResColor(color);
//	}
//}