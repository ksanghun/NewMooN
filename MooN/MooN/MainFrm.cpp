
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MooN.h"

#include "MainFrm.h"
#include "MNDataManager.h"

#include "DlgConfig.h"
#include "MoonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_VIEW_INITIALIZE, &CMainFrame::OnViewInitialize)
	ON_COMMAND(ID_VIEW_LOGVIEW, &CMainFrame::OnViewLogview)
	ON_COMMAND(ID_PROJECT_ADDIMAGE, &CMainFrame::OnProjectAddimage)
	ON_COMMAND(ID_PROJECT_REMOVEIMAGE, &CMainFrame::OnProjectRemoveimage)
	ON_COMMAND(ID_PROJECT_CONFIGURATION, &CMainFrame::OnProjectConfiguration)
	ON_COMMAND(ID_TOOLS_CUTAREA, &CMainFrame::OnToolsCutarea)
	ON_COMMAND(ID_TOOLS_SEARCH, &CMainFrame::OnToolsSearch)
	ON_COMMAND(ID_TOOLS_KEYWORDSEARCH, &CMainFrame::OnToolsKeywordsearch)
	ON_COMMAND(ID_TOOLS_CLEARRESULT, &CMainFrame::OnToolsClearresult)
	ON_COMMAND(ID_ANALYZE_EXTRACTTEXTBOUNDARY, &CMainFrame::OnAnalyzeExtracttextboundary)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_OCR_ENGLISHWORD, &CMainFrame::OnOcrEnglishword)
	ON_COMMAND(ID_OCR_CHINESECHARACTER, &CMainFrame::OnOcrChinesecharacter)
	ON_COMMAND(ID_OCR_CHINESEWORD, &CMainFrame::OnOcrChineseword)
	ON_COMMAND(ID_OCR_ENGLISHCHARACTER, &CMainFrame::OnOcrEnglishcharacter)
	ON_COMMAND(ID_OCR_KOREANWO, &CMainFrame::OnOcrKoreanwo)
	ON_COMMAND(ID_OCR_KOREANCHARACTER, &CMainFrame::OnOcrKoreancharacter)
	ON_COMMAND(ID_ANALYSIS_DATATRAINING, &CMainFrame::OnAnalysisDatatraining)
	ON_COMMAND(ID_OCR_REMOVENOISE, &CMainFrame::OnOcrRemovenoise)
	ON_COMMAND(ID_FILE_EXPORTDATABASE, &CMainFrame::OnFileExportdatabase)
	ON_COMMAND(ID_OCR_SEARCH_SELECTION, &CMainFrame::OnOcrSearchSelection)
	ON_COMMAND(ID_OCR_ADD_LINEBOX, &CMainFrame::OnOcrAddLinebox)
	ON_COMMAND(ID_OCR_DELETEL_INEBOX, &CMainFrame::OnOcrDeletelInebox)
	ON_COMMAND(ID_OCR_ROTATE_LINE, &CMainFrame::OnOcrRotateLine)
	ON_COMMAND(ID_OCR_UNDOROTAION_LINE, &CMainFrame::OnOcrUndorotaionLine)
	ON_COMMAND(ID_OCR_SPLITE_LINE, &CMainFrame::OnOcrSpliteLine)
	ON_COMMAND(ID_OCR_ADD_TEXTBOX, &CMainFrame::OnOcrAddTextbox)
	ON_COMMAND(ID_OCR_DELETETE_XTBOX32820, &CMainFrame::OnOcrDeleteteXtbox32820)
	ON_COMMAND(ID_OCR_TRAIN_TEXT, &CMainFrame::OnOcrTrainText)
	ON_COMMAND(ID_RECOGNIZETEXT_FROMUSERDB, &CMainFrame::OnRecognizetextFromuserdb)
	ON_WM_CLOSE()
	ON_WM_NCDESTROY()
	ON_COMMAND(ID_EXPLORER_EXPORTDATABASE, &CMainFrame::OnExplorerExportdatabase)
	ON_COMMAND(ID_FILE_SAVEALL, &CMainFrame::OnFileSaveall)
	ON_COMMAND(ID_OCR_CUTSEARCH, &CMainFrame::OnOcrCutsearch)
	ON_COMMAND(ID_EXPLORER_EXPORTDATABASE_HTML, &CMainFrame::OnExplorerExportdatabaseHtml)
	ON_COMMAND(ID_OCR_CUTANDSEARCHALL, &CMainFrame::OnOcrCutandsearchall)
	ON_COMMAND(ID_OPTIONS_AUTOFILLON, &CMainFrame::OnOptionsAutofillon)
	ON_COMMAND(ID_OPTIONS_AUTOFILLOFF, &CMainFrame::OnOptionsAutofilloff)
	ON_COMMAND(ID_OPTIONS_TRAININGALL, &CMainFrame::OnOptionsTrainingall)
	ON_COMMAND(ID_OPTIONS_TRAININGSELECTION, &CMainFrame::OnOptionsTrainingselection)
	ON_COMMAND(ID_OCR_MERGE_BOX, &CMainFrame::OnOcrMergeBox)
	ON_COMMAND(ID_SPLITETEXTBOX_VERTICALLY, &CMainFrame::OnSplitetextboxVertically)
	ON_COMMAND(ID_SPLITETEXTBOX_HORIZONTALL, &CMainFrame::OnSplitetextboxHorizontall)
	ON_COMMAND(ID_OCR_MERGELINEBOX, &CMainFrame::OnOcrMergelinebox)
	ON_COMMAND(ID_SPLITLINEBOX_VERTICALLY, &CMainFrame::OnSplitlineboxVertically)
	ON_COMMAND(ID_SPLITLINEBOX_HORIZONTALY, &CMainFrame::OnSplitlineboxHorizontaly)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
	SINGLETON_DataMng::Destory();
}

bool CMainFrame::checkMacAddr()
{
	// Known MAC Address ====================//
	// CString authorized = L"00:0D:3A:A2:E7:C2";	// VM in Azure
	//========================================
	const int numMacAddr = 15;
	CString arrAutho[numMacAddr];
	arrAutho[0] = L"30:5A:3A:75:B6:45";		// FGS  01
	arrAutho[1] = L"00:1A:7D:DA:71:13";		// FGS  02
	arrAutho[2] = L"9C:5C:8E:BC:2D:A3";		// FGS  03
	arrAutho[3] = L"90:E6:BA:46:53:C6";		// FGS  04  
	arrAutho[4] = L"10:F0:05:A7:2F:FC";		// FGS	05
	arrAutho[5] = L"00:FF:19:CC:CE:96";		// Wayne's Destop
	arrAutho[6] = L"00-0D-3A-A0-1C-A5";		// Azura VM 01
	arrAutho[7] = L"00-0D-3A-A1-AA-7F";		// Azura VM 02
	arrAutho[8] = L"00:0D:3A:72:7B:89";
	arrAutho[9] = L"00:0D:3A:72:7B:89";
	arrAutho[10] = L"00:0D:3A:72:7B:89";
	arrAutho[11] = L"00:0D:3A:72:7B:89";
	arrAutho[12] = L"00:0D:3A:72:7B:89";
	arrAutho[13] = L"84:3A:4B:73:38:11";	// Wayne 
	arrAutho[14] = L"9E:B6:D0:63:F3:D7";	// My laptop 


	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	//	char *mac_addr = (char*)malloc(17);
	CString strMacAddr = L"";

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) {

		AfxMessageBox(L"couldn't read MAC Address");
		return false;
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) {
			return false;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		do {
			strMacAddr.Format(L"%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);


	for (int i = 0; i < numMacAddr; i++) {
		if (strMacAddr == arrAutho[i]) {
			return true;
		}
	}
	return false;
}

bool CMainFrame::checkCurrTime()
{
	WORD eYear = 2018;
	WORD eMonth = 12;
	WORD eDay = 30;

	SYSTEMTIME st;
	GetSystemTime(&st);


	if ((st.wYear < eYear)) {
		return true;
	}
	else if (st.wYear==eYear){
		if (st.wMonth < eMonth) {
			return true;
		}
		else if (st.wMonth == eMonth) {
			if ((st.wDay <= eDay)) {
				return true;
			}
		}
	}
	return false;
}

bool CMainFrame::Authorization()
{
	//if (checkMacAddr() == false){
	//	AfxMessageBox(L"Authorization failed");
	//	return false;
	//}

	if (checkCurrTime() == false) {
		AfxMessageBox(L"Authentication has expired");
		return false;
	}

	return true;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (Authorization() == false) {
	//	SendMessage(WM_CLOSE, 0, 0);
		return -1;
	}

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CBRS_SIZE_DYNAMIC;
	DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE;
	CMFCToolBarInfo tbi, tbiA;
	const CRect r1(1, 1, 1, 1);
	if (!m_wndToolBar.CreateEx(this, dwCtrlStyle, dwStyle, r1, IDR_MAINFRAME) ||
		!m_wndToolBar.LoadToolBarEx(IDR_MAINFRAME_256, tbi, TRUE))
	{
		return -1;      // fail to create
	}

	CSize szImage, szButton;
	szImage = m_wndToolBar.GetImageSize();
	szButton.cx = szImage.cx + 6; // button size must be at least image size + 6
	szButton.cy = szImage.cy + 6;
	m_wndToolBar.SetMenuSizes(szButton, szImage);


	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	DockPane(&m_wndProperties);
	CDockablePane* pTabbedBar = NULL;
	m_wndFileView.AttachToTabWnd(&m_wndProperties, DM_SHOW, TRUE, &pTabbedBar);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);


	m_wndFormListView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFormListView);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);


	dwStyle = m_wndFormListView.GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT);

	pTabbedBar->SetControlBarStyle(dwStyle);
	m_wndOutput.SetControlBarStyle(dwStyle);
	m_wndFormListView.SetControlBarStyle(dwStyle);

	// Init configuration //
	InitConfituration();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	cs.lpszName = L"Mo文oN";

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 300, 300), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	if (!m_wndOutput.Create(L"Output", this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM ))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	if (!m_wndProperties.Create(L"Properties", this, CRect(0, 0, 300, 300), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
		
	if (!m_wndFormListView.Create(L"Matching Results", this, CRect(0, 0, 400, 400), TRUE, ID_VIEW_LISTVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	

	

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


//For MooN=====================//

void CMainFrame::InitConfituration()
{
	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFle = path + "\\userdata\\conf.bin";


	// Folder Check================ //
	CString strfolder = path + "\\userdata";
	if (PathFileExists(strfolder) == 0) {
		CreateDirectory(strfolder, NULL);
	}
	
	


	// Load Config
	FILE* fp = 0;
	fopen_s(&fp, (CStringA)strFle, "rb");

	const int pathSize = 256;
	char srcPath[pathSize];
	//	memset(srcPath, 0x00, sizeof(srcPath));
	if (fp) {
		SYSTEMTIME st;
		fread(&st, sizeof(SYSTEMTIME), 1, fp);
		fread(srcPath, sizeof(srcPath), 1, fp);
		m_strSrcPath = (CStringA)srcPath;
		m_strLogPath = (CStringA)srcPath;
		fclose(fp);
	}
	else {
		CDlgConfig dlg;
		if (dlg.DoModal() == IDOK)
		{
			m_strSrcPath = dlg.GetSrcPath();
			fopen_s(&fp, (CStringA)strFle, "wb");
			SYSTEMTIME st;
			GetSystemTime(&st);
			if (fp) {
				sprintf_s(srcPath, sizeof(srcPath), (CStringA)m_strSrcPath, fp);
				fwrite(&st, sizeof(SYSTEMTIME), 1, fp);
				fwrite(srcPath, pathSize, 1, fp);
				fclose(fp);
			}
		}
	}



	//CString strLog = m_strLogPath + "\\logdata";
	//if (PathFileExists(strLog) == 0) {
	//	CreateDirectory(strLog, NULL);
	//}

//	SINGLETON_DataMng::GetInstance()->Test();
	SINGLETON_DataMng::GetInstance()->SetUserDBFolder(m_strLogPath);

	m_wndFileView.FillFileView(m_strSrcPath);
	GetImgFilePath(m_strSrcPath);

	SINGLETON_DataMng::GetInstance()->InitData();
}

void CMainFrame::GetImgFilePath(CString strPath)
{
	CString strSubPath;
	CString strFileName;
	CFileFind file_find;
	BOOL bWorking;
	bWorking = file_find.FindFile(strPath + ("\\*"));

	BOOL isSubDir = false;
	while (bWorking)
	{
		bWorking = file_find.FindNextFile();
		if (!file_find.IsDots()) {

			if (file_find.IsDirectory()) {//directory 				
				strSubPath = strPath + ("\\") + file_find.GetFileName();
				GetImgFilePath(strSubPath);
				//nCount++;
			}
		}
	}
}

void CMainFrame::AddOutputString(CString str, bool IsReplace)
{
	if (IsReplace == true) {
		m_wndOutput.ReplaceString(str);
	}
	else {
		m_wndOutput.AddString(str);
	}
}

void CMainFrame::AddMatchResult()
{
	m_wndFormListView.AddMatchResult();
}

void CMainFrame::AddMatchResultCNS()
{
	m_wndFormListView.AddMatchResultCNS();
}

void CMainFrame::OnViewInitialize()
{
	// TODO: Add your command handler code here
	pView->InitCamera(true);
}


void CMainFrame::OnViewLogview()
{
	// TODO: Add your command handler code here

}


void CMainFrame::OnProjectAddimage()
{
	// TODO: Add your command handler code here
	CDragDropTreeCtrl* pCtrl = m_wndFileView.GetTreeCtrl();
	//for (unsigned int i = 0; i < pCtrl->GetSelItemList()->size(); i++) {
	//	pView->SetTreeDragItem((*pCtrl->GetSelItemList())[i], pCtrl);
	//}
	pView->SetTreeDragItem((*pCtrl->GetSelItemList()), pCtrl);
}


void CMainFrame::OnProjectRemoveimage()
{
	// TODO: Add your command handler code here
	CDragDropTreeCtrl* pCtrl = m_wndFileView.GetTreeCtrl();
	for (unsigned int i = 0; i < pCtrl->GetSelItemList()->size(); i++) {
		HTREEITEM selItem = (*pCtrl->GetSelItemList())[i];
		pView->RemoveImageData(selItem, pCtrl);

		//SINGLETON_DataMng::GetInstance()->ResetResult();
		//ClearAllResults();
	}
}


void CMainFrame::OnProjectConfiguration()
{
	// TODO: Add your command handler code here
	CDlgConfig dlg(NULL, m_strSrcPath, m_strLogPath);


	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFle = path + "\\userdata\\conf.bin";

	bool isOK = false;
	if (dlg.DoModal() == IDOK)
	{
		m_strSrcPath = dlg.GetSrcPath();

		const int pathSize = 256;
		char srcPath[pathSize];
		// Save Config //
		// for MS
		FILE* fp = 0;
		fopen_s(&fp, (CStringA)strFle, "wb");
		SYSTEMTIME st;
		GetSystemTime(&st);
		if (fp) {
			sprintf_s(srcPath, sizeof(srcPath), (CStringA)m_strSrcPath, fp);
			fwrite(&st, sizeof(SYSTEMTIME), 1, fp);
			fwrite(srcPath, pathSize, 1, fp);
			fclose(fp);
		}

		//m_wndFileView.FillFileView(m_strSrcPath);
		//GetImgFilePath(m_strSrcPath);

		isOK = true;
	}


	//if (isOK) {
	//	m_bFlag = FALSE;
	////	PostQuitMessage(0);
	//	SendMessage(WM_CLOSE, 0, 0);
	//}
}


void CMainFrame::OnToolsCutarea()
{
	// TODO: Add your command handler code here
	pView->EnableCutSearchMode();
}


void CMainFrame::OnToolsSearch()
{
	// TODO: Add your command handler code here
	pView->DoCurNSearch();
}


void CMainFrame::OnToolsKeywordsearch()
{
	// TODO: Add your command handler code here
	m_wndProperties.DoKeywordSearch();
}


void CMainFrame::OnToolsClearresult()
{
	// TODO: Add your command handler code here
	ClearAllResults();
}

void CMainFrame::ClearAllResults()
{
	SINGLETON_DataMng::GetInstance()->ResetResult();
	m_wndFormListView.ResetLogList();
}


void CMainFrame::OnAnalyzeExtracttextboundary()  // DO Cut&Search from segments!!!!
{
	// TODO: Add your command handler code here
	m_wndProperties.GetCurrSetting();
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcCNSSegments();
	//pView->DoOCR();
}


void CMainFrame::OnFileOpen()
{
	// TODO: Add your command handler code here
}


BOOL CMainFrame::OnShowPanes(BOOL bShow)
{
	// TODO: Add your specialized code here and/or call the base class
	return CFrameWndEx::OnShowPanes(bShow);
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWndEx::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here	
	//m_wndFormListView.SetAutoHideMode(TRUE, CBRS_ALIGN_RIGHT);
	//m_wndFormListView.ShowPane(TRUE, TRUE, TRUE);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	CMNView* pViewImage = pView->GetImageView();

	if (pMsg->message == WM_KEYDOWN) {
		int nChar = (int)pMsg->wParam;
//		if (nChar == 90) {  // Ctrl key
		if (nChar == 17) {  // Ctrl key
			if (pViewImage) {
				pViewImage->EnableCutSearchMode(true, true);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}

		else if (nChar == 16) {		// Shift Key
			if (pViewImage) {
				pViewImage->EnableMultiSelectionhMode(true, true);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}

		else if (nChar == 39) {
			//if (pViewImage) {
			//	pViewImage->MoveNextPage();
			//}
		}
		else if (nChar == 37) {
			//if (pViewImage) {
			//	pViewImage->MovePrePage();
			//}
		}

		else if (nChar == 38) {
			//if (pViewImage) {
			//	pViewImage->MoveNextUp();
			//}
		}

		else if (nChar == 40) {
			//if (pViewImage) {
			//	pViewImage->MoveNextDown();
			//}
		}

		//else if (nChar == 88) {	// excute search
		//	pView->DoCurNSearch();
		//}

		else if (nChar == 46) {		// Delete Key
			if (pViewImage) {
				if (pViewImage->DeleteSelOCRRes() == false) {
					pViewImage->DeleteSelParagraph();
				}
				
			}
		}


		//else if (nChar == 17) {	// ctrl key
		//	if (pViewImage) {
		//		pViewImage->SetAnimation(false);
		//	}
		//}

		//else if (nChar == 67) {
		//	if (pViewImage) {
		//		pViewImage->ResetAllPages();
		//	}
		//}
		//else if (nChar == 68) {
		//	if (pViewImage) {
		//		pViewImage->SetDBSearch(true);
		//	}
		//}
	}


	if (pMsg->message == WM_KEYUP) {
		int nChar = (int)pMsg->wParam;
//		if (nChar == 90) {  // z key
		if (nChar == 17) {  // z key
			if (pViewImage) {
				pViewImage->EnableCutSearchMode(false, false);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}

		else if (nChar == 16) {		// Shift Key
			if (pViewImage) {
				pViewImage->EnableMultiSelectionhMode(false, false);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}
		//else if (nChar == 17) {	// ctrl key
		//	if (pViewImage) {
		//		pViewImage->SetAnimation(true);
		//	}
		//	float offset = 0.0f;
		//}
		//else if (nChar == 68) {	// "d"
		//	if (pViewImage) {
		//		pViewImage->SetDBSearch(false);
		//	}
		//}
	}

	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::SelectListItemById(int line_text_id)
{
//	m_wndProperties.SetParagraphInfo(fdeskew, strName, IsVerti);
	m_wndFormListView.SelItemByLineTextBoxID(line_text_id);
}

void CMainFrame::SetParagraphInfo(float fdeskew, CString strName, bool IsVerti)
{
	m_wndProperties.SetParagraphInfo(fdeskew, strName, IsVerti);
}

void CMainFrame::SetOCRResInfo(wchar_t* strCode, float fConfi, int lang)
{
	m_wndProperties.SetOCRResInfo(strCode, fConfi, lang);
}

void CMainFrame::DeleteMatchList(int line_text_id)
{
	m_wndFormListView.DeleteItemByLineTextBoxID(line_text_id);
}

void CMainFrame::OnOcrEnglishword()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrEnglishword();
	pViewImage->OcrFromTextBox(__ENG, 1);
}


void CMainFrame::OnOcrChinesecharacter()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrChiChar();
	pViewImage->OcrFromTextBox(__CHI, 0);
}


void CMainFrame::OnOcrChineseword()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrChiWord();
	pViewImage->OcrFromTextBox(__CHI, 1);
}


void CMainFrame::OnOcrEnglishcharacter()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrEnglishChar();
	pViewImage->OcrFromTextBox(__ENG, 0);
}


void CMainFrame::OnOcrKoreanwo()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrKorWord();
	pViewImage->OcrFromTextBox(__KOR, 1);
}


void CMainFrame::OnOcrKoreancharacter()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
//	pViewImage->OcrKorChar();
	pViewImage->OcrFromTextBox(__KOR, 0);
}


void CMainFrame::OnAnalysisDatatraining()
{
	// TODO: Add your command handler code here
	BeginWaitCursor();

//	SINGLETON_DataMng::GetInstance()->DBTraining();
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcTrainingOCRResbyConfidence(0.0f);
	EndWaitCursor();
}


void CMainFrame::OnOcrRemovenoise()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->RemoveNoise();
}


void CMainFrame::OnFileExportdatabase()
{
	// TODO: Add your command handler code here
	SINGLETON_DataMng::GetInstance()->ExportDatabase();
}


void CMainFrame::OnOcrSearchSelection()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcDoSearchSelection();
	
}


void CMainFrame::OnOcrAddLinebox()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	RECT2D rect = pImgView->GetSelectedAreaForCNS();
	cv::Rect r(rect.x1, rect.y1, rect.width, rect.height);
	pImgView->AddNewLineBox(r);
}


void CMainFrame::OnOcrDeletelInebox()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteSelParagraph();

	
}


void CMainFrame::OnOcrRotateLine()
{
	// TODO: Add your command handler code here
	m_wndProperties.RotateSelLinebox();
}


void CMainFrame::OnOcrUndorotaionLine()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->UndoDeskewParagraph();
}


void CMainFrame::OnOcrSpliteLine()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	GetCurrSetting();
	pImgView->ReExtractParagraph();
}


void CMainFrame::OnOcrAddTextbox()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();

	RECT2D rect = pImgView->GetSelectedAreaForCNS();
	cv::Rect r(rect.x1, rect.y1, rect.width, rect.height);
	pImgView->AddNewTextBox(r);
}


void CMainFrame::OnOcrDeleteteXtbox32820()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->DeleteSelOCRRes();
}


void CMainFrame::OnOcrTrainText()
{
	// TODO: Add your command handler code here
	CMNView* pImgView = pView->GetImageView();
	pImgView->ConfirmOCRRes();
}


void CMainFrame::OnRecognizetextFromuserdb()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->OcrFromTextBox(__LANG_NONE, 0);
}


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
//	m_bFlag = TRUE;
	CFrameWndEx::OnClose();
}


void CMainFrame::OnNcDestroy()
{
	CFrameWndEx::OnNcDestroy();

	// TODO: Add your message handler code here
	//USES_CONVERSION;
	//if (m_bFlag != FALSE) return;
	//Sleep(300);                                                         // To Delay...
	//WinExec((LPCSTR)AfxGetApp()->m_pszExeName, SW_SHOW);
}


void CMainFrame::OnExplorerExportdatabase()
{
	// TODO: Add your command handler code here
//	SINGLETON_DataMng::GetInstance()->ExportDatabase(m_wndFileView.GetExtractDBFolder());
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcExportDB(m_wndFileView.GetExtractDBFolder());

}


void CMainFrame::OnFileSaveall()
{
	// TODO: Add your command handler code here
	SINGLETON_DataMng::GetInstance()->Save();
}


void CMainFrame::OnOcrCutsearch()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcDoSearchBySelection();
}


void CMainFrame::OnExplorerExportdatabaseHtml()
{
	// TODO: Add your command handler code here
//	SINGLETON_DataMng::GetInstance()->ExportDatabaseToHtml(m_wndFileView.GetExtractDBFolder());
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcExportDB(m_wndFileView.GetExtractDBFolder(), true);
}


void CMainFrame::OnOcrCutandsearchall()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->ProcDoSearchBySelectionAll();
}


void CMainFrame::OnOptionsAutofillon()
{
	// TODO: Add your command handler code here
	m_wndFormListView.SetAutoFillOption(true);
}


void CMainFrame::OnOptionsAutofilloff()
{
	// TODO: Add your command handler code here
	m_wndFormListView.SetAutoFillOption(false);
}


void CMainFrame::OnOptionsTrainingall()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnOptionsTrainingselection()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnOcrMergeBox()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->MergeSelectedTextBox();
}


void CMainFrame::OnSplitetextboxVertically()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->SetSplitBoxMode(_VERTI_DIR, _SPLIT_TEXT);
}


void CMainFrame::OnSplitetextboxHorizontall()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->SetSplitBoxMode(_HORI_DIR, _SPLIT_TEXT);
}


void CMainFrame::OnOcrMergelinebox()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->MergeSelectedLineBox();
}


void CMainFrame::OnSplitlineboxVertically()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->SetSplitBoxMode(_VERTI_DIR, _SPLIT_LINE);
}


void CMainFrame::OnSplitlineboxHorizontaly()
{
	// TODO: Add your command handler code here
	CMNView* pViewImage = pView->GetImageView();
	pViewImage->SetSplitBoxMode(_HORI_DIR, _SPLIT_LINE);
}

