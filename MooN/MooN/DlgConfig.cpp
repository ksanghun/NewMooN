// DlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "DlgConfig.h"
#include "afxdialogex.h"


// CDlgConfig dialog

IMPLEMENT_DYNAMIC(CDlgConfig, CDialog)

CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/, CString strSrc, CString strLog)
	: CDialog(CDlgConfig::IDD, pParent)
	, strSrcPath(_T(""))
	, strLogPath(_T(""))
{
	strSrcPath = strSrc;
	strLogPath = strLog;
}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SRCPATH, strSrcPath);
	DDX_Text(pDX, IDC_EDIT_LOGPATH, strLogPath);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CDlgConfig::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDOK, &CDlgConfig::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConfig message handlers


void CDlgConfig::OnBnClickedButtonBrowser()
{
	// TODO: Add your control notification handler code here
	ITEMIDLIST *pidlBrowse;
	CString strPath;

	BROWSEINFO BrInfo;

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset(&BrInfo, 0, sizeof(BrInfo));
	BrInfo.pszDisplayName = (LPWSTR)(LPCTSTR)strPath;
	BrInfo.lpszTitle = _T("Select Source Location");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그 띄우기
	pidlBrowse = SHBrowseForFolder(&BrInfo);

	if (pidlBrowse != NULL)
	{
		BOOL bSuccess = ::SHGetPathFromIDList(pidlBrowse, (LPWSTR)(LPCTSTR)strPath);
		if (!bSuccess)
		{
			MessageBox(_T("Wrong folder name."), _T(""), MB_OKCANCEL | MB_ICONASTERISK);
			return;
		}
	}

	strSrcPath = strPath;
	strLogPath = strPath;
	UpdateData(FALSE);
}


void CDlgConfig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (strSrcPath != L"") {
		CDialog::OnOK();
	}
	else {
		AfxMessageBox(_T("Select Source File Folder"));
	}
	CDialog::OnOK();
}
