// FromProperties.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "FormProperties.h"
#include "MooNView.h"
#include "MNDataManager.h"


// CFromProperties

IMPLEMENT_DYNCREATE(CFormProperties, CFormView)

CFormProperties::CFormProperties()
	: CFormView(IDD_FROMPROPERTIES)
	, m_fEditTh(75.0f)
	, m_bEnglish(FALSE)
	, m_bChinese(TRUE)
	, m_bKorean(TRUE)
	//, m_nEngFontSize(32)
	//, m_nChiFontSize(32)
	//, m_nKorFontSize(32)
	, m_nAlign(0)
	, m_strPageName(_T(""))
	, m_fDeskew(0)
	, m_strCode(_T(""))
	, m_fConfidence(0)
	, m_bLineBox(TRUE)
	, m_editConfi(80)
	, m_editKeyword(_T(""))
	, m_editDefaultFontSize(32)
	, m_editDBth(90)
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
	//DDX_Text(pDX, IDC_EDIT_ENG_SIZE, m_nEngFontSize);
	//DDV_MinMaxUInt(pDX, m_nEngFontSize, 1, 1000);
	//DDX_Text(pDX, IDC_EDIT_CHI_SIZE, m_nChiFontSize);
	//DDV_MinMaxUInt(pDX, m_nChiFontSize, 1, 1000);
	//DDX_Text(pDX, IDC_EDIT_KOR_SIZE, m_nKorFontSize);
	//DDV_MinMaxUInt(pDX, m_nKorFontSize, 1, 1000);
	DDX_Radio(pDX, IDC_RADIO_HORIZON, m_nAlign);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strPageName);
	DDX_Text(pDX, IDC_EDIT_DESKEW, m_fDeskew);
	DDV_MinMaxFloat(pDX, m_fDeskew, -90, 90);
	DDX_Text(pDX, IDC_EDIT_ENCODE, m_strCode);
	DDX_Text(pDX, IDC_EDIT_CONFIDENCE, m_fConfidence);
	DDX_Control(pDX, IDC_COMBO_LANG, m_comboLanguage);
	DDX_Control(pDX, IDC_BN_RUNOCR, m_btnOCR);
	DDX_Check(pDX, IDC_CHECK_LINEBIOX, m_bLineBox);
	//DDX_Control(pDX, IDC_BN_ADD_PARA, m_btnLineAdd);
	//DDX_Control(pDX, IDC_BN_DEL_PARA, m_btnLineDel);
	//DDX_Control(pDX, IDC_BN_RE_EXTRACT, m_btnLineReExt);
	DDX_Control(pDX, IDC_SLIDER_CONFI, m_sliderConfi);
	DDX_Text(pDX, IDC_EDIT_CONFI, m_editConfi);
	DDV_MinMaxLong(pDX, m_editConfi, 0, 100);
	DDX_Text(pDX, IDC_EDIT_KEYWORD_SEARCH, m_editKeyword);
	DDX_Text(pDX, IDC_EDIT_DEFAULT_FONT_SIZE, m_editDefaultFontSize);
	DDV_MinMaxUInt(pDX, m_editDefaultFontSize, 1, 1000);
	DDX_Control(pDX, IDC_SLIDER_DBTH, m_sliderForDBTh);
	DDX_Text(pDX, IDC_EDIT_DBTH, m_editDBth);
	DDV_MinMaxUInt(pDX, m_editDBth, 0, 100);
}

BEGIN_MESSAGE_MAP(CFormProperties, CFormView)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CFormProperties::OnNMReleasedcaptureSlider1)
	ON_EN_CHANGE(IDC_EDIT1, &CFormProperties::OnEnChangeEdit1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CFormProperties::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_GETCUSTOMSPLITRECT, IDC_MFCCOLORBUTTON1, &CFormProperties::OnNMGetCustomSplitRectMfccolorbutton1)
	ON_BN_CLICKED(IDC_BN_EXTRACTLINE, &CFormProperties::OnBnClickedBnExtractline)
	//ON_BN_CLICKED(IDC_BN_ENG_FONT, &CFormProperties::OnBnClickedBnEngFont)
	//ON_BN_CLICKED(IDC_BN_CHI_FONT, &CFormProperties::OnBnClickedBnChiFont)
	//ON_BN_CLICKED(IDC_BN_KOR_FONT, &CFormProperties::OnBnClickedBnKorFont)
	//ON_BN_CLICKED(IDC_BN_APP_DESKEW, &CFormProperties::OnBnClickedBnAppDeskew)
	//ON_BN_CLICKED(IDC_BN_CANCEL, &CFormProperties::OnBnClickedBnCancel)
	//ON_BN_CLICKED(IDC_BN_DEL_PARA, &CFormProperties::OnBnClickedBnDelPara)
	//ON_BN_CLICKED(IDC_BN_ADD_PARA, &CFormProperties::OnBnClickedBnAddPara)
	//ON_BN_CLICKED(IDC_BN_RE_EXTRACT, &CFormProperties::OnBnClickedBnReExtract)
	ON_BN_CLICKED(IDC_BN_DEL_ALLLINBES, &CFormProperties::OnBnClickedBnDelAlllinbes)
//	ON_BN_CLICKED(IDC_BN_DEL_ALLOCR, &CFormProperties::OnBnClickedBnDelAllocr)
	ON_BN_CLICKED(IDC_BN_RUNOCR, &CFormProperties::OnBnClickedBnRunocr)
	//ON_BN_CLICKED(IDC_BN_DEL_OCRRES, &CFormProperties::OnBnClickedBnDelOcrres)
	ON_BN_CLICKED(IDC_BN_ADD_MODIFYOCRRES, &CFormProperties::OnBnClickedBnAddModifyocrres)
	//ON_BN_CLICKED(IDC_BN_WORD_CONFIRM, &CFormProperties::OnBnClickedBnWordConfirm)
	//ON_BN_CLICKED(IDC_BN_ADD_OCRRES, &CFormProperties::OnBnClickedBnAddOcrres)
	ON_EN_CHANGE(IDC_EDIT_FILENAME, &CFormProperties::OnEnChangeEditFilename)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_CHECK_LINEBIOX, &CFormProperties::OnBnClickedCheckLinebiox)
	ON_EN_CHANGE(IDC_EDIT_CONFI, &CFormProperties::OnEnChangeEditConfi)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONFI, &CFormProperties::OnNMCustomdrawSliderConfi)
	//ON_EN_CHANGE(IDC_EDIT_KOR_SIZE, &CFormProperties::OnEnChangeEditKorSize)
	//ON_EN_CHANGE(IDC_EDIT_CHI_SIZE, &CFormProperties::OnEnChangeEditChiSize)
	ON_BN_CLICKED(IDC_BN_ALL_DEL_OCRRES, &CFormProperties::OnBnClickedBnAllDelOcrres)
	ON_BN_CLICKED(IDC_BN_ENCODE, &CFormProperties::OnBnClickedBnEncode)
//	ON_BN_CLICKED(IDC_BN_KEYWORD_SEARCH, &CFormProperties::OnBnClickedBnKeywordSearch)
ON_BN_CLICKED(IDC_BN_DESKEW_ALL, &CFormProperties::OnBnClickedBnDeskewAll)
ON_BN_CLICKED(IDC_BN_SETFONTSIZE, &CFormProperties::OnBnClickedBnSetfontsize)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DBTH, &CFormProperties::OnNMCustomdrawSliderDbth)
ON_BN_CLICKED(IDC_BN_MATCH_FROM_DB, &CFormProperties::OnBnClickedBnMatchFromDb)
ON_EN_CHANGE(IDC_EDIT_KEYWORD_SEARCH, &CFormProperties::OnEnChangeEditKeywordSearch)
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

	m_comboLanguage.AddString(L"English");
	m_comboLanguage.AddString(L"Chinese");
	m_comboLanguage.AddString(L"Korean");
	m_comboLanguage.AddString(L"DB");
	m_comboLanguage.AddString(L"Cut&Search");
	m_comboLanguage.SetCurSel(0);


	m_sliderConfi.SetRange(1, 100, TRUE);
	m_sliderConfi.SetTicFreq(100);
	m_sliderConfi.SetPos(m_editConfi);

	m_sliderForDBTh.SetRange(1, 100, TRUE);
	m_sliderForDBTh.SetTicFreq(100);
	m_sliderForDBTh.SetPos(m_editConfi);


	CMNView* pImgView = pView->GetImageView();
	pImgView->SetDispConfidence(m_editConfi);
	pImgView->SetDBTreshold(m_editDBth);

	pImgView->EnableShowLine(true);
	UpdateData(FALSE);

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
//
void CFormProperties::GetCurrSetting()
{
	UpdateData(TRUE);
	_stExtractionSetting extractonInfo;
	extractonInfo.init();

	extractonInfo.IsVerti = static_cast<bool>(m_nAlign);
	//extractonInfo.chiSize = m_nChiFontSize;
	//extractonInfo.engSize = m_nEngFontSize;
	//extractonInfo.korSize = m_nEngFontSize;
	extractonInfo.chiSize = 32;
	extractonInfo.engSize = 32;
	extractonInfo.korSize = 32;

	extractonInfo.isChi = m_bChinese;
	extractonInfo.isEng = m_bEnglish;
	extractonInfo.isKor = m_bKorean;

	SINGLETON_DataMng::GetInstance()->SetExtractionSetting(extractonInfo);

}
void CFormProperties::OnBnClickedBnExtractline()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//_stExtractionSetting extractonInfo;
	//extractonInfo.init();

	//extractonInfo.IsVerti = (bool)m_nAlign;
	//extractonInfo.chiSize = m_nChiFontSize;
	//extractonInfo.engSize = m_nEngFontSize;
	//extractonInfo.korSize = m_nEngFontSize;
	//extractonInfo.isChi = m_bChinese;
	//extractonInfo.isEng = m_bEnglish;
	//extractonInfo.isKor = m_bKorean;

	GetCurrSetting();
//	pView->DoExtractBoundary();
	CMNView* pImgView = pView->GetImageView();
	pImgView->DoExtractBoundaryForSelected();

}


//void CFormProperties::OnBnClickedBnEngFont()
//{
//	// TODO: Add your control notification handler code here
//	RECT2D rect = pView->GetSelectedAreaForCNS();
////	m_nEngFontSize = rect.width > rect.height ? rect.width : rect.height;
//	UpdateData(FALSE);
//}
//
//
//void CFormProperties::OnBnClickedBnChiFont()
//{
//	// TODO: Add your control notification handler code here
//	RECT2D rect = pView->GetSelectedAreaForCNS();
////	m_nChiFontSize = rect.width > rect.height ? rect.width : rect.height;
//	UpdateData(FALSE);
//}
//
//
//void CFormProperties::OnBnClickedBnKorFont()
//{
//	// TODO: Add your control notification handler code here
//	RECT2D rect = pView->GetSelectedAreaForCNS();
////	m_nKorFontSize = rect.width > rect.height ? rect.width : rect.height;
//	UpdateData(FALSE);
//}

void CFormProperties::SetParagraphInfo(float fskew, CString strName, bool IsVerti)
{
	UpdateData(TRUE);
	m_strPageName = strName;
	m_fDeskew = fskew;
	
	if (IsVerti) {
		m_nAlign = 1;
	}
	else {
		m_nAlign = 0;
	}

	UpdateData(FALSE);
}

void CFormProperties::SetOCRInfo(wchar_t* strCode, float fConfi, int lang)
{
	UpdateData(TRUE);
	m_strCode = CString(strCode);
	m_fConfidence = fConfi;
	m_comboLanguage.SetCurSel(lang);
	UpdateData(FALSE);
}

//void CFormProperties::OnBnClickedBnAppDeskew()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();	
//	UpdateData(TRUE);
//	pImgView->DeskewParagraph(m_fDeskew);
//}

void CFormProperties::RotateSelLinebox()
{
	CMNView* pImgView = pView->GetImageView();
	UpdateData(TRUE);
	pImgView->DeskewParagraph(m_fDeskew);
}


//void CFormProperties::OnBnClickedBnCancel()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->UndoDeskewParagraph();
//}


//void CFormProperties::OnBnClickedBnDelPara()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->DeleteSelParagraph();
//}


//void CFormProperties::OnBnClickedBnAddPara()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->AddParagraph();
//}


//void CFormProperties::OnBnClickedBnReExtract()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	GetCurrSetting();
//	pImgView->ReExtractParagraph();
//}


void CFormProperties::OnBnClickedBnDelAlllinbes()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteAllLines();
//	pImgView->DeleteAllOCRRes();
}


void CFormProperties::OnBnClickedBnDelAllocr()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteAllOCRRes();
}


void CFormProperties::OnBnClickedBnRunocr()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->ProcOCR(false);
	
}


//void CFormProperties::OnBnClickedBnDelOcrres()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->DeleteSelOCRRes();
//}


void CFormProperties::OnBnClickedBnAddModifyocrres()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CMNView* pImgView = pView->GetImageView();
	pImgView->UpdateOCRCode(m_strCode);
}


//void CFormProperties::OnBnClickedBnWordConfirm()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->ConfirmOCRRes();
//}


//void CFormProperties::OnBnClickedBnAddOcrres()
//{
//	// TODO: Add your control notification handler code here
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->AddOCRRes();
//}


void CFormProperties::OnEnChangeEditFilename()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CFormProperties::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	//if ((nIDCtl == IDC_BN_RUNOCR))         //checking for the button 
	//{
	//	CDC dc;
	//	RECT rect;
	//	dc.Attach(lpDrawItemStruct->hDC);   // Get the Button DC to CDC
	//	rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.
	//	dc.Draw3dRect(&rect, RGB(0, 0, 0), RGB(0, 0, 0));
	//	dc.FillSolidRect(&rect, RGB(100, 250, 150));//Here you can define the required color to appear on the Button.
	//	UINT state = lpDrawItemStruct->itemState;  //This defines the state of the Push button either pressed or not. 
	//	if ((state & ODS_SELECTED))		dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
	//	else			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
	//	dc.SetBkColor(RGB(100, 250, 150));   //Setting the Text Background color
	//	dc.SetTextColor(RGB(0, 0, 0));     //Setting the Text Color
	//	TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
	//	ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
	//	::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 
	//	dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER);//Redraw the  Caption of Button Window 
	//	dc.Detach();  // Detach the Button DC
	//}

	//if (nIDCtl == IDC_BN_ALL_DEL_OCRRES)         //checking for the button 
	//{
	//	CDC dc;
	//	RECT rect;
	//	dc.Attach(lpDrawItemStruct->hDC);   // Get the Button DC to CDC
	//	rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.
	//	dc.Draw3dRect(&rect, RGB(0, 0, 0), RGB(0, 0, 0));
	//	dc.FillSolidRect(&rect, RGB(250, 250, 0));//Here you can define the required color to appear on the Button.
	//	UINT state = lpDrawItemStruct->itemState;  //This defines the state of the Push button either pressed or not. 
	//	if ((state & ODS_SELECTED))		dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
	//	else			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
	//	dc.SetBkColor(RGB(250, 250, 0));   //Setting the Text Background color
	//	dc.SetTextColor(RGB(0, 0, 0));     //Setting the Text Color
	//	TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
	//	ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
	//	::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 
	//	dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER);//Redraw the  Caption of Button Window 
	//	dc.Detach();  // Detach the Button DC
	//}
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CFormProperties::OnBnClickedCheckLinebiox()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CMNView* pImgView = pView->GetImageView();
	if (m_bLineBox == FALSE) {
		//m_btnLineAdd.EnableWindow(FALSE);
		//m_btnLineDel.EnableWindow(FALSE);
		//m_btnLineReExt.EnableWindow(FALSE);
		


		pImgView->EnableShowLine(false);
	}
	else {
		//m_btnLineReExt.EnableWindow(TRUE);
		//m_btnLineAdd.EnableWindow(TRUE);
		//m_btnLineDel.EnableWindow(TRUE);

		pImgView->EnableShowLine(true);
	}

}


void CFormProperties::OnEnChangeEditConfi()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_sliderConfi.SetPos(m_editConfi);
	UpdateData(FALSE);
}


void CFormProperties::OnNMCustomdrawSliderConfi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateData(TRUE);
	int pos = m_sliderConfi.GetPos();

	if (pos != (int)m_fEditTh) {
		m_editConfi = pos;
		UpdateData(FALSE);

		CMNView* pImgView = pView->GetImageView();
		pImgView->SetDispConfidence(m_editConfi);
	}
}


//void CFormProperties::OnEnChangeEditKorSize()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CFormView::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}


//void CFormProperties::OnEnChangeEditChiSize()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CFormView::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}


void CFormProperties::OnBnClickedBnAllDelOcrres()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteAllOCRRes();
}


void CFormProperties::OnBnClickedBnEncode()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->EncodePage();
}


//void CFormProperties::OnBnClickedBnKeywordSearch()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//	SINGLETON_DataMng::GetInstance()->DoKeywordSearch(m_editKeyword);
//	CMNView* pImgView = pView->GetImageView();
//	pImgView->InitCamera();
//}

void CFormProperties::DoKeywordSearch()
{
	UpdateData(TRUE);
	SINGLETON_DataMng::GetInstance()->DoKeywordSearch(m_editKeyword);
	CMNView* pImgView = pView->GetImageView();
	pImgView->InitCamera();
}


void CFormProperties::OnBnClickedBnDeskewAll()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeskewParagraph(m_fDeskew);
}


void CFormProperties::OnBnClickedBnSetfontsize()
{
	// TODO: Add your control notification handler code here
}


void CFormProperties::OnNMCustomdrawSliderDbth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateData(TRUE);
	int pos = m_sliderForDBTh.GetPos();

	if (pos != (int)m_fEditTh) {
		m_editDBth = pos;
		UpdateData(FALSE);

		CMNView* pImgView = pView->GetImageView();
		pImgView->SetDBTreshold(m_editDBth);
	}
}


void CFormProperties::OnBnClickedBnMatchFromDb()
{
	// TODO: Add your control notification handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DoOCRFromMooN();
}


void CFormProperties::OnEnChangeEditKeywordSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
