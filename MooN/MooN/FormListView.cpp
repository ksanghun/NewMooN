// FormListView.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "FormListView.h"
#include "MNDataManager.h"

// CFormListView

IMPLEMENT_DYNCREATE(CFormListView, CFormView)

CFormListView::CFormListView()
	: CFormView(IDD_FORMLISTVIEW)
//	, m_bIsAutoFill(TRUE)
{
	m_bIsCreated = false;
	m_cutMaxWidth = _NORMALIZE_SIZE_H;
}

CFormListView::~CFormListView()
{
	for (int i = 0; i < m_imgList.GetImageCount(); i++)
	{
		m_imgList.Remove(i);
	}
}

void CFormListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
//	DDX_Check(pDX, IDC_CHECK_AUTOFILL, m_bIsAutoFill);
}

BEGIN_MESSAGE_MAP(CFormListView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BN_ADD_TO_OCR, &CFormListView::OnBnClickedBnAddToOcr)
//	ON_BN_CLICKED(IDC_CHECK_AUTOFILL, &CFormListView::OnBnClickedCheckAutofill)
END_MESSAGE_MAP()


// CFormListView diagnostics

#ifdef _DEBUG
void CFormListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormListView message handlers


BOOL CFormListView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CFormListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(m_bIsCreated)
		m_ctrlList.MoveWindow(0, 40, cx, cy-40);
}

void CFormListView::ResizeListColSize(int _maxwidth)
{
	//m_ctrlList.SetColumnWidth(0, _maxwidth);
	m_cutMaxWidth = _maxwidth;
}

void CFormListView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	m_ctrlList.EnableScrollBarCtrl(SB_HORZ);
	m_ctrlList.ShowScrollBar(SB_HORZ);
	m_ctrlList.SetScrollRange(SB_HORZ, 0, 2000);
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	//CFont font;
	//font.CreatePointFont(14, L"Courier New");
	//m_ctrlList.SetFont(&font);

	m_ctrlList.InitListCtrl();
	m_ctrlList.AddUserColumn(L"Cut", _NORMALIZE_SIZE_W + 5);
	m_ctrlList.AddUserColumn(L"Code", 100);
	m_ctrlList.AddUserColumn(L"Accuracy", 50);	
	m_ctrlList.AddUserColumn(L"Search ID", 40);
	m_ctrlList.AddUserColumn(L"Cut ID", 100);
	m_ctrlList.AddUserColumn(L"Cut File ID", 100);
	m_ctrlList.AddUserColumn(L"Cut Pos.", 100);
	m_ctrlList.AddUserColumn(L"Match ID", 100);
	m_ctrlList.AddUserColumn(L"Match File ID", 100);
	m_ctrlList.AddUserColumn(L"Match Pos", 100);
	m_ctrlList.AddUserColumn(L"Threshold", 100);	
	m_ctrlList.AddUserColumn(L"Base64", 100);
	m_ctrlList.AddUserColumn(L"ID", 0);

	m_ctrlList.AddUserColumn(L"PID", 100);
	m_ctrlList.AddUserColumn(L"MID", 100);
	m_ctrlList.AddUserColumn(L"UUID", 100);

	UINT nFlags = ILC_MASK;
	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
	m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);
	m_ctrlList.SetImageList(&m_imgList, LVSIL_SMALL);

	m_bIsCreated = true;

	m_nRecordNum = 0;
	m_imgListId = 0;

	m_bIsAutoFill = TRUE;
//	m_ctrlList.SetAutoFillOption(m_bIsAutoFill);
}

void CFormListView::ResetLogList()
{
//	m_imgList.DeleteImageList();
	//int imglistcnt = m_imgList.GetImageCount();
	//for (int i = 0; i < imglistcnt; i++){
	//	m_imgList.Remove(i);
	//}

	while (m_imgList.GetImageCount()) m_imgList.Remove(0);


	//m_imgList.DeleteImageList();
	//UINT nFlags = ILC_MASK;
	//nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
	//m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);
	//m_ctrlList.SetImageList(&m_imgList, LVSIL_SMALL);

	m_ctrlList.ResetListCtrl();

	m_nRecordNum = 0;
	m_imgListId = 0;
}

void CFormListView::SelItemByLineTextBoxID(int _id)
{
	m_ctrlList.SelItemByLineTextBoxID(_id);
}

void CFormListView::DeleteItemByLineTextBoxID(int _id)
{
	m_ctrlList.DeleteItemByLineTextBoxID(_id);
}

void CFormListView::AddRecord()
{
//	ResetLogList();	
	CString strItem;
	std::map<unsigned long, stVecMatchResult>& mapMatchRes = SINGLETON_DataMng::GetInstance()->GetMatchingResults();
	//std::map<unsigned long, stVecMatchResult>::iterator iter = mapMatchRes.begin();
	//for (; iter != mapMatchRes.end(); iter++) {
	std::map<unsigned long, stVecMatchResult>::iterator iter = mapMatchRes.end();
	iter--;
	for (; iter != mapMatchRes.end(); iter--) {
	//	for (int i = 0; i < iter->second.size(); i++) {
		for (int i = iter->second.size()-1; i >=0; i--) {

			if (iter->second[i].IsOnList == false) {

				//cv::imshow("cutimg", iter->second[i].cutImg);
				CBitmap* pbmp = SINGLETON_DataMng::GetInstance()->GetLogCBitmap(iter->second[i].cutImg);

				if (pbmp != NULL) {
					BITMAP bmpObj;
					pbmp->GetBitmap(&bmpObj);
					m_imgList.Add(pbmp, RGB(255, 0, 0));
					delete pbmp;

										// Add resutl information //
					m_ctrlList.InsertItem(m_nRecordNum, L"", m_imgListId);

					m_ctrlList.SetItem(m_nRecordNum, 1, LVIF_TEXT, iter->second[i].strCode, m_imgListId, 0, 0, NULL);	// CODE //

					strItem.Format(L"%3.2f", iter->second[i].accuracy);
					m_ctrlList.SetItem(m_nRecordNum, 2, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].searchId);
					m_ctrlList.SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].cutId);
					m_ctrlList.SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].fileId);
					m_ctrlList.SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].posId);
					m_ctrlList.SetItem(m_nRecordNum, 6, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%d%u", (int)(iter->second[i].accuracy), iter->second[i].matchId);
					m_ctrlList.SetItem(m_nRecordNum, 7, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].matchFile);
					m_ctrlList.SetItem(m_nRecordNum, 8, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%u", iter->second[i].matchPos);
					m_ctrlList.SetItem(m_nRecordNum, 9, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%3.2f", iter->second[i].fTh);
					m_ctrlList.SetItem(m_nRecordNum, 10, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					m_ctrlList.SetItem(m_nRecordNum, 11, LVIF_TEXT, iter->second[i].strBase64, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%d", i);
					m_ctrlList.SetItem(m_nRecordNum, 12, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					//=====For Interaction=====================================================//
					strItem.Format(L"%d", iter->second[i].id_page);
					m_ctrlList.SetItem(m_nRecordNum, 13, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%d", iter->second[i].id_match);
					m_ctrlList.SetItem(m_nRecordNum, 14, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

					strItem.Format(L"%d", iter->second[i].uuid);
					m_ctrlList.SetItem(m_nRecordNum, 15, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);
					//=========================================================================//

					iter->second[i].IsOnList = true;

					m_nRecordNum++;
					m_imgListId++;
				}
			}

			else {
				int a = 0;
			}
		}
	}



	//m_ctrlList.SetItemState(100, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	//m_ctrlList.SetFocus();
	//m_ctrlList.EnsureVisible(100, TRUE);
	
}

void CFormListView::OnBnClickedBnAddToOcr()
{
	// TODO: Add your control notification handler code here

	m_ctrlList.AddListToTraining();
}


//void CFormListView::OnBnClickedCheckAutofill()
//{
//	// TODO: Add your control notification handler code here
//	if (m_bIsAutoFill == TRUE) {
//		m_bIsAutoFill = FALSE;
//	}
//	else {
//		m_bIsAutoFill = TRUE;
//	}
//	m_ctrlList.SetAutoFillOption(m_bIsAutoFill);
//}
void CFormListView::SetAutoFillOption(bool IsAuto)
{
	m_ctrlList.SetAutoFillOption(IsAuto);
}

void CFormListView::AddRecord_CNSAll()
{
	//ResetLogList();
	//CString strItem;
	//std::map<unsigned int, std::vector<_stCNSResult>> mapCns = SINGLETON_DataMng::GetInstance()->GetCNSMatchingResults();

	//std::map<unsigned int, std::vector<_stCNSResult>>::iterator iter = mapCns.begin();
	//for (; iter != mapCns.end(); iter++) {
	//	unsigned int uuid = iter->first;
	//	for (int i = 0; i < iter->second.size(); i++) {

	//		if (iter->second[i].cutimg.ptr() == nullptr)
	//			continue;

	//		cv::Rect nRect(0, 0, iter->second[i].cutimg.cols, iter->second[i].cutimg.rows);

	//		CBitmap* pbmp = 0;
	//		if (nRect.width > _NORMALIZE_SIZE_W) {
	//			nRect.width = _NORMALIZE_SIZE_W;
	//			cv::Mat tmpimg = iter->second[i].cutimg.clone();
	//			cv::resize(tmpimg, tmpimg, cv::Size(nRect.width, nRect.height));

	//			cv::Mat cutImg = cv::Mat(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), iter->second[i].cutimg.type());
	//			cutImg.setTo(255);
	//			tmpimg.copyTo(cutImg(nRect));

	//			pbmp = SINGLETON_DataMng::GetInstance()->GetLogCBitmap(cutImg);
	//			cutImg.release();
	//			tmpimg.release();
	//		}
	//		else {
	//			cv::Mat cutImg = cv::Mat(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), iter->second[i].cutimg.type());
	//			cutImg.setTo(255);
	//			iter->second[i].cutimg.copyTo(cutImg(nRect));

	//			pbmp = SINGLETON_DataMng::GetInstance()->GetLogCBitmap(cutImg);
	//			cutImg.release();
	//		}


	//		if (pbmp != NULL) {
	//			BITMAP bmpObj;
	//			pbmp->GetBitmap(&bmpObj);
	//			m_imgList.Add(pbmp, RGB(255, 0, 0));
	//			delete pbmp;

	//			// Add resutl information //
	//			m_ctrlList.InsertItem(m_nRecordNum, L"", m_imgListId);

	//			m_ctrlList.SetItem(m_nRecordNum, 1, LVIF_TEXT, L"", m_imgListId, 0, 0, NULL);	// CODE //

	//			strItem.Format(L"%3.2f", iter->second[i].fConfi);
	//			m_ctrlList.SetItem(m_nRecordNum, 2, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", uuid);
	//			m_ctrlList.SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", 0);
	//			m_ctrlList.SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", 0);
	//			m_ctrlList.SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", 0);
	//			m_ctrlList.SetItem(m_nRecordNum, 6, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%d%u", (int)(iter->second[i].fConfi), iter->second[i].searchid);
	//			m_ctrlList.SetItem(m_nRecordNum, 7, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", 0);
	//			m_ctrlList.SetItem(m_nRecordNum, 8, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%u", 0);
	//			m_ctrlList.SetItem(m_nRecordNum, 9, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%3.2f", 0.75f);
	//			m_ctrlList.SetItem(m_nRecordNum, 10, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			m_ctrlList.SetItem(m_nRecordNum, 11, LVIF_TEXT, L"", m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%d", i);
	//			m_ctrlList.SetItem(m_nRecordNum, 12, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			//=====For Interaction=====================================================//
	//			strItem.Format(L"%d", iter->second[i].pageid);
	//			m_ctrlList.SetItem(m_nRecordNum, 13, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

	//			strItem.Format(L"%d", iter->second[i].objid);
	//			m_ctrlList.SetItem(m_nRecordNum, 14, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);
	//			//=========================================================================//

	//			m_nRecordNum++;
	//			m_imgListId++;
	//		}
	//	}

	//}
}