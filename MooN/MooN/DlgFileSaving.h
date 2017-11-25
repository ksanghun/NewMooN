#pragma once


// CDlgFileSaving dialog

class CDlgFileSaving : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileSaving)

public:
	CDlgFileSaving(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFileSaving();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIG_SAVEFILES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void SaveAllFiles();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
