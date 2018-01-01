#pragma once
#include "afxcmn.h"
#include "ZListCtrlLog.h"



// CFormListView form view

class CFormListView : public CFormView
{
	DECLARE_DYNCREATE(CFormListView)

protected:
public:

	CFormListView();           // protected constructor used by dynamic creation
	virtual ~CFormListView();

	void ResizeListColSize(int _maxwidth);
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMLISTVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


private:
	CZListCtrlLog m_ctrlList;
	CImageList m_imgList;
	bool m_bIsCreated;
	int m_nRecordNum;
	int m_imgListId;
	int m_cutMaxWidth;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	void ResetLogList();
	void AddRecord();
	void AddRecord_CNSAll();

	afx_msg void OnBnClickedBnAddToOcr();
	afx_msg void OnBnClickedCheckAutofill();
	BOOL m_bIsAutoFill;
};


