
// MooNView.h : interface of the CMooNView class
//

#pragma once
#include "MNView.h"
#include "DragDropTreeCtrl.h"

//class CDragDropTreeCtrl;
class CMooNView : public CView
{
protected: // create from serialization only
	CMooNView();
	DECLARE_DYNCREATE(CMooNView)

// Attributes
public:
//	CMooNDoc* GetDocument() const;

// Operations
public:
//	void SetTreeDragItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl);
	void SetTreeDragItem(SEL_ITEM_LIST& itemList, CDragDropTreeCtrl* pCtrl);

	short ProcSetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl);
	short SetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl, float& offset);
	void EnableCutSearchMode();
	void DoCurNSearch();
	void DoExtractBoundary();
	void DoExtractBoundaryPage();
	void DoOCR();

	void AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt);
	void InitCamera(bool bInit);
	void RemoveImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl);

	void SetPositionByList(CString strPid, CString strMid, bool IsCameraMove);
	void AddListToTraining(int pageid, int matchid, CString strCode);
	CMNView* GetImageView() { return m_pViewImage; }

	//void SetThresholdValue(float _th);
	//void SetResultColor(int R, int G, int B);
	RECT2D GetSelectedAreaForCNS();
	void EncodeTextFromTreeCtrl(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, CFile& cfile);

	void SetTypeMode(bool IsEnable) { m_pViewImage->SetTypeMode(IsEnable); }

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMooNView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


private:
	CMFCTabCtrl m_ctrlTab;
	CMNView* m_pViewImage;
protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in MooNView.cpp
//inline CMooNDoc* CMooNView::GetDocument() const
//   { return reinterpret_cast<CMooNDoc*>(m_pDocument); }
#endif

