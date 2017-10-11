#pragma once

#include "FormProperties.h"
//#include "data_type.h"
// CDockProperties

class CDockProperties : public CDockablePane
{
	DECLARE_DYNAMIC(CDockProperties)

private:
	CFormProperties* m_pProperties;

public:
	CDockProperties();
	virtual ~CDockProperties();

	float GetThreshold();// { return m_pProperties->GetThreshold(); }
	COLORREF GetMatchingColor();// { return m_pProperties->GetMatchingColor(); }
	void SetParagraphInfo(float fskew, CString strName);
	void GetCurrSetting();

	void SetOCRResInfo(wchar_t* strCode, float fConfi, int lang);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


