// DockProperties.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "DockProperties.h"


// CDockProperties

IMPLEMENT_DYNAMIC(CDockProperties, CDockablePane)

CDockProperties::CDockProperties()
{
	m_pProperties = NULL;
}

CDockProperties::~CDockProperties()
{
}


BEGIN_MESSAGE_MAP(CDockProperties, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CDockProperties message handlers




void CDockProperties::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pProperties)
		m_pProperties->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}


int CDockProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pProperties = new CFormProperties;
	if (!m_pProperties->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 200, 200), this, 0, NULL))
	{
		AfxMessageBox(_T("Failed in creating CMyFormView"));
	}

	return 0;
}


float CDockProperties::GetThreshold() 
{ 
	if (m_pProperties) {
		return m_pProperties->GetThreshold();
	}
	else {
		return 0;
	}
}
COLORREF CDockProperties::GetMatchingColor() 
{ 
	if (m_pProperties) {
		return m_pProperties->GetMatchingColor();
	}
	else {
		return RGB(0, 0, 0);
	}
}

void CDockProperties::SetParagraphInfo(float fskew, CString strName)
{
	if (m_pProperties) {
		return m_pProperties->SetParagraphInfo(fskew, strName);
	}
}