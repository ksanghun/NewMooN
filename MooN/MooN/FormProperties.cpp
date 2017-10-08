// FromProperties.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "FormProperties.h"
#include "MooNView.h"


// CFromProperties

IMPLEMENT_DYNCREATE(CFormProperties, CFormView)

CFormProperties::CFormProperties()
	: CFormView(IDD_FROMPROPERTIES)
	, m_fEditTh(75.0f)
	, m_bEnglish(TRUE)
	, m_bChinese(FALSE)
	, m_bKorean(FALSE)
	, m_nEngFontSize(1)
	, m_nChiFontSize(1)
	, m_nKorFontSize(1)
	, m_nAlign(0)
	, m_strPageName(_T(""))
	, m_fDeskew(0)
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
	DDX_Check(pDX, IDC_CHECK_ENG, m_bEnglish);
	DDX_Check(pDX, IDC_CHECK_CHI, m_bChinese);
	DDX_Check(pDX, IDC_CHECK_KOR, m_bKorean);
	DDX_Text(pDX, IDC_EDIT_ENG_SIZE, m_nEngFontSize);
	DDV_MinMaxUInt(pDX, m_nEngFontSize, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_CHI_SIZE, m_nChiFontSize);
	DDV_MinMaxUInt(pDX, m_nChiFontSize, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_KOR_SIZE, m_nKorFontSize);
	DDV_MinMaxUInt(pDX, m_nKorFontSize, 1, 1000);
	DDX_Radio(pDX, IDC_RADIO_HORIZON, m_nAlign);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strPageName);
	DDX_Text(pDX, IDC_EDIT_DESKEW, m_fDeskew);
	DDV_MinMaxFloat(pDX, m_fDeskew, -90, 90);
}

BEGIN_MESSAGE_MAP(CFormProperties, CFormView)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CFormProperties::OnNMReleasedcaptureSlider1)
	ON_EN_CHANGE(IDC_EDIT1, &CFormProperties::OnEnChangeEdit1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CFormProperties::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_GETCUSTOMSPLITRECT, IDC_MFCCOLORBUTTON1, &CFormProperties::OnNMGetCustomSplitRectMfccolorbutton1)
	ON_BN_CLICKED(IDC_BN_EXTRACTLINE, &CFormProperties::OnBnClickedBnExtractline)
	ON_BN_CLICKED(IDC_BN_ENG_FONT, &CFormProperties::OnBnClickedBnEngFont)
	ON_BN_CLICKED(IDC_BN_CHI_FONT, &CFormProperties::OnBnClickedBnChiFont)
	ON_BN_CLICKED(IDC_BN_KOR_FONT, &CFormProperties::OnBnClickedBnKorFont)
	ON_BN_CLICKED(IDC_BN_APP_DESKEW, &CFormProperties::OnBnClickedBnAppDeskew)
	ON_BN_CLICKED(IDC_BN_CANCEL, &CFormProperties::OnBnClickedBnCancel)
	ON_BN_CLICKED(IDC_BN_DEL_PARA, &CFormProperties::OnBnClickedBnDelPara)
	ON_BN_CLICKED(IDC_BN_ADD_PARA, &CFormProperties::OnBnClickedBnAddPara)
	ON_BN_CLICKED(IDC_BN_RE_EXTRACT, &CFormProperties::OnBnClickedBnReExtract)
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


void CFormProperties::OnBnClickedBnExtractline()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	_stExtractionSetting extractonInfo;
	extractonInfo.init();

	extractonInfo.nAlign = m_nAlign;
	extractonInfo.chiSize = m_nChiFontSize;
	extractonInfo.engSize = m_nEngFontSize;
	extractonInfo.korSize = m_nEngFontSize;
	extractonInfo.isChi = m_bChinese;
	extractonInfo.isEng = m_bEnglish;
	extractonInfo.isKor = m_bKorean;


	pView->DoExtractBoundary(extractonInfo);

}


void CFormProperties::OnBnClickedBnEngFont()
{
	// TODO: Add your control notification handler code here
	RECT2D rect = pView->GetSelectedAreaForCNS();
	m_nEngFontSize = rect.width > rect.height ? rect.width : rect.height;
	UpdateData(FALSE);
}


void CFormProperties::OnBnClickedBnChiFont()
{
	// TODO: Add your control notification handler code here
	RECT2D rect = pView->GetSelectedAreaForCNS();
	m_nChiFontSize = rect.width > rect.height ? rect.width : rect.height;
	UpdateData(FALSE);
}


void CFormProperties::OnBnClickedBnKorFont()
{
	// TODO: Add your control notification handler code here
	RECT2D rect = pView->GetSelectedAreaForCNS();
	m_nKorFontSize = rect.width > rect.height ? rect.width : rect.height;
	UpdateData(FALSE);
}

void CFormProperties::SetParagraphInfo(float fskew, CString strName)
{
	m_strPageName = strName;
	m_fDeskew = fskew;
	UpdateData(FALSE);
}

void CFormProperties::OnBnClickedBnAppDeskew()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	
	UpdateData(TRUE);
	pImgView->DeskewParagraph(m_fDeskew);
}


void CFormProperties::OnBnClickedBnCancel()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->UndoDeskewParagraph();
}


void CFormProperties::OnBnClickedBnDelPara()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteSelParagraph();
}


void CFormProperties::OnBnClickedBnAddPara()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->AddParagraph();
}


void CFormProperties::OnBnClickedBnReExtract()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->ReExtractParagraph();
}
