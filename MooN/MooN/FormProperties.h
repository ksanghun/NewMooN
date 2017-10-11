#pragma once
#include "afxcmn.h"
#include "afxcolorbutton.h"
#include "afxwin.h"


// CFromProperties form view

class CFormProperties : public CFormView
{
	DECLARE_DYNCREATE(CFormProperties)

protected:

public:
	CFormProperties();           // protected constructor used by dynamic creation
	virtual ~CFormProperties();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FROMPROPERTIES };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	float m_fEditTh;
	CSliderCtrl m_sliderTh;
	COLORREF m_colorTh;
	virtual void OnInitialUpdate();


	float GetThreshold() { return m_fEditTh; }
	COLORREF GetMatchingColor() { return m_ctrlColorBtn.GetColor(); }
	void SetParagraphInfo(float fskew, CString strName);
	void GetCurrSetting();
	void SetOCRInfo(wchar_t* strCode, float fConfi, int lang);

	CMFCColorButton m_ctrlColorBtn;
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMGetCustomSplitRectMfccolorbutton1(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL m_bEnglish;
	BOOL m_bChinese;
	BOOL m_bKorean;
	UINT m_nEngFontSize;
	UINT m_nChiFontSize;
	UINT m_nKorFontSize;
	BOOL m_nAlign;
	afx_msg void OnBnClickedBnExtractline();
	afx_msg void OnBnClickedBnEngFont();
	afx_msg void OnBnClickedBnChiFont();
	afx_msg void OnBnClickedBnKorFont();
	CString m_strPageName;
	float m_fDeskew;
	afx_msg void OnBnClickedBnAppDeskew();
	afx_msg void OnBnClickedBnCancel();
	afx_msg void OnBnClickedBnDelPara();
	afx_msg void OnBnClickedBnAddPara();
	afx_msg void OnBnClickedBnReExtract();
	afx_msg void OnBnClickedBnDelAlllinbes();
	afx_msg void OnBnClickedBnDelAllocr();
	afx_msg void OnBnClickedBnRunocr();
	CString m_strCode;
	float m_fConfidence;
	CComboBox m_comboLanguage;
	afx_msg void OnBnClickedBnDelOcrres();
	afx_msg void OnBnClickedBnAddModifyocrres();
	afx_msg void OnBnClickedBnWordConfirm();
	afx_msg void OnBnClickedBnAddOcrres();
};


