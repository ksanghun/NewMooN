// DockFormView.cpp : implementation file
//

#include "stdafx.h"
#include "DockFormView.h"


// CDockFormView

IMPLEMENT_DYNAMIC(CDockFormView, CDockablePane)

CDockFormView::CDockFormView()
{
	m_pMyFormView = NULL;
}

CDockFormView::~CDockFormView()
{
	//if (m_pMyFormView)
	//	delete m_pMyFormView;
}


BEGIN_MESSAGE_MAP(CDockFormView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CDockFormView message handlers


int CDockFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pMyFormView = new CFormListView;
	if (!m_pMyFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 200, 200), this, 0, NULL))
	{
		AfxMessageBox(_T("Failed in creating CMyFormView"));
	}

	return 0;
}


void CDockFormView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(m_pMyFormView)
		m_pMyFormView->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}

void CDockFormView::AddMatchResult()
{
	if (m_pMyFormView)
		m_pMyFormView->AddRecord();

}

void CDockFormView::SetAutoFillOption(bool IsAuto)
{
	if (m_pMyFormView)
		m_pMyFormView->SetAutoFillOption(IsAuto);
}

void CDockFormView::AddMatchResultCNS()
{
	if (m_pMyFormView)
		m_pMyFormView->AddRecord_CNSAll();

}

void CDockFormView::AddListToTraining()
{
	if (m_pMyFormView)
		m_pMyFormView->AddListToTraining();

}

void CDockFormView::ResizeListColSize(int _maxwidth)
{
	if (m_pMyFormView)
		m_pMyFormView->ResizeListColSize(_maxwidth);
}

void CDockFormView::ResetLogList()
{
	if (m_pMyFormView) {
		m_pMyFormView->ResetLogList();
	}
}

void CDockFormView::SelItemByLineTextBoxID(int _id)
{
	if (m_pMyFormView) {
		m_pMyFormView->SelItemByLineTextBoxID(_id);
	}
}

void CDockFormView::DeleteItemByLineTextBoxID(int _id)
{
	if (m_pMyFormView) {
		m_pMyFormView->DeleteItemByLineTextBoxID(_id);
	}
}