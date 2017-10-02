#pragma once
#include "resource.h"

// CDlgConfig dialog

class CDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL, CString strSrc = _T(""), CString strLog = _T(""));   // standard constructor
	virtual ~CDlgConfig();

	CString GetSrcPath() { return strSrcPath; }
	CString GetLogPath() { return strLogPath; }
	void SetSrcPath(CString str) { strSrcPath = str; }
	void SetLogPath(CString str) { strLogPath = str; }
	void UpdateStrData() { UpdateData(FALSE); }

// Dialog Data
	enum { IDD = IDD_DLG_CINFIG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString strSrcPath;
	CString strLogPath;
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedOk();
};
