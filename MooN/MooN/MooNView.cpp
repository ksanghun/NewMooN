
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


void CMooNView::SetTreeDragItem(SEL_ITEM_LIST& itemList, CDragDropTreeCtrl* pCtrl)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	int addcnt = 0;

	for (auto i = 0; i < itemList.size(); i++) {
		AddImageData(itemList[i], pCtrl, addcnt);
	}
	


	CString str;
	str.Format(L"%d image(s) added", addcnt);
	pM->AddOutputString(str, false);

	if (m_pViewImage) {
		m_pViewImage->ProcGenerateThumbnail();
	}

	if (itemList.size() > 0) {
		HTREEITEM hChildItem = pCtrl->GetChildItem(itemList[0]);
		if (hChildItem == NULL) {  // No Child!! File
			HTREEITEM hPItem = pCtrl->GetParentItem(itemList[0]);
			ProcSetSelectedItem(hPItem, pCtrl);
		}
		else {
			ProcSetSelectedItem(itemList[0], pCtrl);
		}
	}
}


void CMooNView::AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt)
{
	USES_CONVERSION;	char* sz = 0;

	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0, nameCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem == NULL) {  // No Child!! File
		
		HTREEITEM pItem = pCtrl->GetParentItem(_item);

		pCtrl->SetItemImage(_item, 4, 4);
		strName = pCtrl->GetItemText(_item);
		if (SINGLETON_DataMng::GetInstance()->IsSupportFormat(strName)) {
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
				if (SINGLETON_DataMng::GetInstance()->IsSupportFormat(strName)) {
					char* sz = T2A(strPath);
					cCode = getHashCode(sz);
					sz = T2A(strName);
					nameCode = getHashCode(sz);
					// Add Image Data //
					SINGLETON_DataMng::GetInstance()->PushImageDataSet(strPath, strPName, strName, cCode, pCode);
					cnt++;
				}
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

void CMooNView::DoExtractBoundary()
{
	if ((m_pViewImage)) {
		m_pViewImage->ProcExtractBoundary();
	}
}

void CMooNView::DoExtractBoundaryPage()
{
	if ((m_pViewImage)) {
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		CString str;
		str.Format(_T("Extraction.....start"));
		pM->AddOutputString(str, false);
		m_pViewImage->DoExtractBoundaryForSelected();
		str.Format(_T("Extraction.....done"));
		pM->AddOutputString(str, true);
	}
}

void CMooNView::DoOCR()
{
	if ((m_pViewImage)) {
		m_pViewImage->ProcOCR(true);
	}
}


void CMooNView::AddListToTraining(int pageid, int matchid, CString strCode)
{
	//SINGLETON_DataMng::GetInstance()->ProcDBTrainingFromCutSearch();
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->AddOutputString(L"Training Cut & Search Results...", false);

	CMNPageObject* pPage = SINGLETON_DataMng::GetInstance()->GetPageByID(pageid);
	if (pPage) {
		_stOCRResult ocrRes;
		ocrRes.type = _CNS;  

		if (pPage->GetRectByMatchID(matchid, ocrRes.rect, ocrRes.fConfidence)) {

			// Need to add cut&Search result to OCR results !!!! //
			memset(&ocrRes.strCode, 0x00, sizeof(wchar_t)*_MAX_WORD_SIZE);
			wsprintf(ocrRes.strCode, strCode.GetBuffer());
						
			//char char_str[_MAX_WORD_SIZE * 2];
			//memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
			//int char_str_len = WideCharToMultiByte(CP_ACP, 0, ocrRes.strCode, -1, NULL, 0, NULL, NULL);
			//WideCharToMultiByte(CP_ACP, 0, ocrRes.strCode, -1, char_str, char_str_len, 0, 0);
//			ocrRes.hcode = getHashCode(char_str);
			ocrRes.bNeedToDB = true;	

			pPage->AddOCRResult(-1, ocrRes);  // hcode here!!

			cv::Mat cutImg = pPage->GetSrcPageGrayImg()(ocrRes.rect).clone();
//			SINGLETON_DataMng::GetInstance()->DBTrainingFromCutSearch(cutImg, ocrRes.strCode, ocrRes.hcode);
			SINGLETON_DataMng::GetInstance()->DBTrainingFromCutSearch(cutImg, ocrRes.strCode, 0);
			cutImg.release();		
		}
	}

	pM->AddOutputString(L"Training Cut & Search Results...complete", false);
}


void CMooNView::SetPositionByList(CString strPid, CString strMid, bool IsCameraMove)
{
	CMNPageObject* pPage = SINGLETON_DataMng::GetInstance()->GetPageByID(_ttoi(strPid));
	if (pPage) {
		POINT3D tPos;
		int mid = _ttoi(strMid);
		if(pPage->GetPosByMatchID(mid, tPos)){
			pPage->SetSelMatchItem(mid);
			if(IsCameraMove)
				m_pViewImage->MoveCameraPos(tPos, 200);
		}
	}
}

RECT2D CMooNView::GetSelectedAreaForCNS()
{
	return m_pViewImage->GetSelectedAreaForCNS();
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