// FromProperties.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "FormProperties.h"


// CFromProperties

IMPLEMENT_DYNCREATE(CFormProperties, CFormView)

CFormProperties::CFormProperties()
	: CFormView(IDD_FROMPROPERTIES)
	, m_fEditTh(75.0f)
{
}

CFormProperties::~CFormProperties()
{
}

void CFormProperties::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_fEditTh);
	DDV_MinMaxFloat(pDX, m_fEditTh, 0.0f, 100.0f);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderTh);
	DDX_Text(pDX, IDC_MFCCOLORBUTTON1, m_colorTh);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_ctrlColorBtn);
}

BEGIN_MESSAGE_MAP(CFormProperties, CFormView)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CFormProperties::OnNMReleasedcaptureSlider1)
	ON_EN_CHANGE(IDC_EDIT1, &CFormProperties::OnEnChangeEdit1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CFormProperties::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_GETCUSTOMSPLITRECT, IDC_MFCCOLORBUTTON1, &CFormProperties::OnNMGetCustomSplitRectMfccolorbutton1)
END_MESSAGE_MAP()


// CFromProperties diagnostics

#ifdef _DEBUG
void CFormProperties::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormProperties::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFromProperties message handlers


BOOL CFormProperties::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CFormProperties::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	m_ctrlColorBtn.SetColor(RGB(255, 0, 0));

	m_sliderTh.SetRange(0, 100, TRUE);
	m_sliderTh.SetTicFreq(100);
	m_sliderTh.SetPos(m_fEditTh);

}


void CFormProperties::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;	
}


void CFormProperties::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sliderTh.SetPos(m_fEditTh);
	UpdateData(FALSE);
}


void CFormProperties::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	UpdateData(TRUE);
	int pos = m_sliderTh.GetPos();

	if (pos != (int)m_fEditTh) {
		m_fEditTh = (float)pos;
		UpdateData(FALSE);
	}
}


void CFormProperties::OnNMGetCustomSplitRectMfccolorbutton1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
