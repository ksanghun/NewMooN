#pragma once
#include "ZEdit.h"

// CZListCtrlLog
#define MSG_POST_SUBCLASS_LISTVIEW (WM_APP+50)

class CZListCtrlLog : public CListCtrl
{
	DECLARE_DYNAMIC(CZListCtrlLog)

public:
	CZListCtrlLog();
	virtual ~CZListCtrlLog();

	void InitListCtrl();
	void AddUserColumn(CString strLable, unsigned short colWidth);
	void AddRecode();
	void ResetListCtrl();
	void AddListToTraining();
	void SetAutoFillOption(bool IsAutoFill) { m_bIsAutoFill = IsAutoFill; }

	void SelItemByLineTextBoxID(int _id);
	void DeleteItemByLineTextBoxID(int linetextid);


// ColumnHeader Class============================//
	class CCustomDrawHeaderCtrl : public CHeaderCtrl
	{
	public:
		CCustomDrawHeaderCtrl();
		virtual ~CCustomDrawHeaderCtrl();


	protected:
		CBrush m_brBackground;
		afx_msg LRESULT OnEraseBkgndOverride(WPARAM wParam, LPARAM lParam);
		afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	};
	CCustomDrawHeaderCtrl m_ctlHeader;
//===========================================//


	CRect	m_SelectionRect;
	BOOL	m_SelectionFlag;
	CPoint  m_Point;
	int		m_Rows;
	int     m_Columns;
	int		m_PrevItem;

	int		m_selItem;
	CString	m_strSearchId;

	bool	m_bIsAutoFill;
	bool	m_bMoveCamera;

	CFont m_Font;
protected:

	virtual void PreSubclassWindow();
	void UpdateCodeValue();
	

	CZEdit m_Edit;
	int nItem, nSubItem;
	int m_nCulNum, m_nRecordNum;

	int m_currDrawId;
	int m_colorid;

	afx_msg LRESULT OnPostSubclassListview(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);


	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);

};


