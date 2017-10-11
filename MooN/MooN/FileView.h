
#pragma once

#include "ViewTree.h"
#include "DragDropTreeCtrl.h"
typedef	std::vector<CString> _strlist;

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// Construction
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:

	CDragDropTreeCtrl m_wndFileView;
	CImageList m_FileViewImages;

	HTREEITEM m_rootItem;
	unsigned short m_nWidth;

protected:
	

// Implementation
public:
	virtual ~CFileView();

	void FillFileView(CString strFolder);
	CDragDropTreeCtrl* GetTreeCtrl() { return &m_wndFileView; }
private:
	HTREEITEM ExtractFolder(CString strFolder, CString strName, _strlist& filelist, HTREEITEM& parentItem);
	bool IsSupportFormat(CString strPath);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};

