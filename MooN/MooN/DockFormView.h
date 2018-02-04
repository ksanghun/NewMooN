#pragma once
#include "FormListView.h"

// CDockFormView

class CDockFormView : public CDockablePane
{
	DECLARE_DYNAMIC(CDockFormView)

private:
	CFormListView* m_pMyFormView;
public:
	CDockFormView();
	virtual ~CDockFormView();

	void AddMatchResult();
	void AddMatchResultCNS();
	void AddListToTraining();
	void ResizeListColSize(int _maxwidth);
	void ResetLogList();
	void SelItemByLineTextBoxID(int _id);
	void DeleteItemByLineTextBoxID(int _id);
	void SetAutoFillOption(bool IsAuto);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


