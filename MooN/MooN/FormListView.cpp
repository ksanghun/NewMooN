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
{
	m_bIsCreated = false;
	m_cutMaxWidth = _NORMALIZE_SIZE_H;
}

CFormListView::~CFormListView()
{
}

void CFormListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
}

BEGIN_MESSAGE_MAP(CFormListView, CFormView)
	ON_WM_SIZE()
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
		m_ctrlList.MoveWindow(0, 20, cx, cy-20);
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
	m_ctrlList.AddUserColumn(L"Base64", 1000);
	m_ctrlList.AddUserColumn(L"ID", 0);

	m_ctrlList.AddUserColumn(L"PID", 0);
	m_ctrlList.AddUserColumn(L"MID", 0);

	UINT nFlags = ILC_MASK;
	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
	m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);
	m_ctrlList.SetImageList(&m_imgList, LVSIL_SMALL);

	m_bIsCreated = true;

	m_nRecordNum = 0;
	m_imgListId = 0;
}

void CFormListView::ResetLogList()
{
	m_imgList.DeleteImageList();
	UINT nFlags = ILC_MASK;
	m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);
	m_ctrlList.ResetListCtrl();

	m_nRecordNum = 0;
	m_imgListId = 0;
}

void CFormListView::AddRecord()
{
//	ResetLogList();

	
	CString strItem;
	std::map<unsigned long, stVecMatchResult>& mapMatchRes = SINGLETON_DataMng::GetInstance()->GetMatchingResults();
	std::map<unsigned long, stVecMatchResult>::iterator iter = mapMatchRes.begin();
	for (; iter != mapMatchRes.end(); iter++) {
		for (int i = 0; i < iter->second.size(); i++) {

			if (iter->second[i].IsOnList == false) {

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
}