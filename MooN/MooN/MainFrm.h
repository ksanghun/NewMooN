
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "DockFormView.h"
#include "DockProperties.h"


#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

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
	void AddMatchResultCNS();
	void ResizeListColSize(int _maxwidth) { m_wndFormListView.ResizeListColSize(_maxwidth); }

	float GetThreshold() { return m_wndProperties.GetThreshold(); }
	COLORREF GetMatchingColor() { return m_wndProperties.GetMatchingColor(); }

	void SetParagraphInfo(float fdeskew, CString strName, bool IsVerti);
	void SelectListItemById(int line_text_id);
	void GetCurrSetting() { return m_wndProperties.GetCurrSetting(); }

	void SetOCRResInfo(wchar_t* strCode, float fConfi, int lang);
	void DeleteMatchList(int line_text_id);
	void ClearAllResults();
	void AddListToTraining(int& addNum, int& totalNum, bool& threadEnd);
private:
	void InitConfituration();
	void GetImgFilePath(CString strPath);

	bool Authorization();
	bool checkMacAddr();
	bool checkCurrTime();

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

	BOOL m_bFlag;  // for restarting
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
	afx_msg void OnAnalyzeExtracttextboundary(); // DO Cut&Search from segments!!!!
	afx_msg void OnFileOpen();
	virtual BOOL OnShowPanes(BOOL bShow);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOcrEnglishword();
	afx_msg void OnOcrChinesecharacter();
	afx_msg void OnOcrChineseword();
	afx_msg void OnOcrEnglishcharacter();
	afx_msg void OnOcrKoreanwo();
	afx_msg void OnOcrKoreancharacter();
	afx_msg void OnAnalysisDatatraining();
	afx_msg void OnOcrRemovenoise();
	afx_msg void OnFileExportdatabase();
	afx_msg void OnOcrSearchSelection();
	afx_msg void OnOcrAddLinebox();
	afx_msg void OnOcrDeletelInebox();
	afx_msg void OnOcrRotateLine();
	afx_msg void OnOcrUndorotaionLine();
	afx_msg void OnOcrSpliteLine();
	afx_msg void OnOcrAddTextbox();
	afx_msg void OnOcrDeleteteXtbox32820();
	afx_msg void OnOcrTrainText();
	afx_msg void OnRecognizetextFromuserdb();
	afx_msg void OnClose();
	afx_msg void OnNcDestroy();
	afx_msg void OnExplorerExportdatabase();
	afx_msg void OnFileSaveall();
	afx_msg void OnOcrCutsearch();
	afx_msg void OnExplorerExportdatabaseHtml();
	afx_msg void OnOcrCutandsearchall();
	afx_msg void OnOptionsAutofillon();
	afx_msg void OnOptionsAutofilloff();
	afx_msg void OnOptionsTrainingall();
	afx_msg void OnOptionsTrainingselection();
	afx_msg void OnOcrMergeBox();
	afx_msg void OnSplitetextboxVertically();
	afx_msg void OnSplitetextboxHorizontall();
	afx_msg void OnOcrMergelinebox();
	afx_msg void OnSplitlineboxVertically();
	afx_msg void OnSplitlineboxHorizontaly();
	afx_msg void OnExplorerEncodetext();
	afx_msg void OnOcrRemovepage();
};


