
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "DockFormView.h"
#include "DockProperties.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void AddOutputString(CString str, bool IsReplace);
	void AddMatchResult();
	void ResizeListColSize(int _maxwidth) { m_wndFormListView.ResizeListColSize(_maxwidth); }

	float GetThreshold() { return m_wndProperties.GetThreshold(); }
	COLORREF GetMatchingColor() { return m_wndProperties.GetMatchingColor(); }

	void SetParagraphInfo(float fdeskew, CString strName);
	void GetCurrSetting() { return m_wndProperties.GetCurrSetting(); }

	void SetOCRResInfo(wchar_t* strCode, float fConfi, int lang);

private:
	void InitConfituration();
	void GetImgFilePath(CString strPath);
	

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

	CMFCToolBar       m_wndToolBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	COutputWnd        m_wndOutput;
	CDockFormView	  m_wndFormListView;
	CDockProperties	  m_wndProperties;

	CString			m_strSrcPath;
	CString			m_strLogPath;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:

	afx_msg void OnViewInitialize();
	afx_msg void OnViewLogview();
	afx_msg void OnProjectAddimage();
	afx_msg void OnProjectRemoveimage();
	afx_msg void OnProjectConfiguration();
	afx_msg void OnToolsCutarea();
	afx_msg void OnToolsSearch();
	afx_msg void OnToolsKeywordsearch();
	afx_msg void OnToolsClearresult();
	afx_msg void OnAnalyzeExtracttextboundary();
	afx_msg void OnFileOpen();
	virtual BOOL OnShowPanes(BOOL bShow);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOcrEnglishword();
	afx_msg void OnOcrChinesecharacter();
	afx_msg void OnOcrChineseword();
};


