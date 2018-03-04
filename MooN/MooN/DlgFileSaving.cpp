// DlgFileSaving.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "DlgFileSaving.h"
#include "afxdialogex.h"
#include "MNDataManager.h"


// CDlgFileSaving dialog

IMPLEMENT_DYNAMIC(CDlgFileSaving, CDialog)

CDlgFileSaving::CDlgFileSaving(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIG_SAVEFILES, pParent)
{

}

CDlgFileSaving::~CDlgFileSaving()
{
}

void CDlgFileSaving::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFileSaving, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgFileSaving message handlers


BOOL CDlgFileSaving::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  Add extra initialization here
	SetTimer(100, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFileSaving::SaveAllFiles()
{
	


	
}

void CDlgFileSaving::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 100) {
		SINGLETON_DataMng::GetInstance()->UpdateAllImgVecData();
	//	SINGLETON_DataMng::GetInstance()->UpdateSDBFiles();
		SINGLETON_DataMng::GetInstance()->UpdateImgClassDB();
		CDialog::OnOK();
		KillTimer(100);
	}

	CDialog::OnTimer(nIDEvent);
}
