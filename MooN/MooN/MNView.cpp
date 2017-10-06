#include "stdafx.h"
#include "MNView.h"
#include "MNDataManager.h"
#include "MainFrm.h"

enum TIMEREVNT { _RENDER = 100, _ADDIMG, _SEARCHIMG, _MOVECAMANI, _UPDATE_PAGE, _GEN_THUMBNAIL, _DO_SEARCH, _DO_EXTRACTION};
#define DEFAULT_X_OFFSET -5800;
#define DEFAULT_Y_OFFSET 2500;

#define MAX_CAM_HIGHTLEVEL 5000
#define MIN_CAM_HIGHTLEVEL 2
#define MAX_CAM_FOV 45.0f

#define ANI_FRAME_CNT 10
#define THUMBNAIL_SIZE 64

CMNView::CMNView()
{
	memset(&m_LogFont, 0, sizeof(m_LogFont));
	m_LogFont.lfCharSet = ANSI_CHARSET;
	m_LogFont.lfHeight = -14;
	m_LogFont.lfWidth = 0;
	//	m_LogFont.lfWeight = FW_BOLD;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));


	m_addImgCnt = 0;
	m_isAnimation = true;
	m_mouseMode = 0;

	m_pSelectPageForCNS = NULL;
	m_bIsCutNSearchMode = false;
	m_stateKeyDown = false;

	mtSetPoint3D(&m_PO, 0.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_PN, 0.0f, 0.0f, 1.0f);

	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;

	//m_IsDBSearch = false;
	//m_selWordId = -1;

	m_IsSearchMatchItems  = false;
	m_selMatchItemId = -1;

	m_fThreshold = 0.75f;
	m_colorAccScale = 1.0f;

	m_resColor.r = 1.0f; m_resColor.g = 0.0f; m_resColor.b = 0.0f;  m_resColor.a = 1.0f;
	m_cnsSearchId = 1;
}


CMNView::~CMNView()
{
	delete m_pBmpInfo;
}

BEGIN_MESSAGE_MAP(CMNView, COGLWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEHOVER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CMNView::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL * 2);
	m_cameraPri.InitializeCamera(MAX_CAM_FOV, 0, 0, m_lookAt, _nWidth, _nHeight);
	SetTimer(_RENDER, 30, NULL);
	SetTimer(_UPDATE_PAGE, 500, NULL);

//	SINGLETON_DataMng::GetInstance()->Test();
	glInitNames();
	if (m_OCRMng.InitOCRMng() == false) {
		TRACE("OCR Error");
	}
//	m_OCRMng.TestFunc();
//	m_Extractor.TestFunc();
}

void CMNView::InitCamera(bool movexy)
{
	KillTimer(_MOVECAMANI);
	m_nAniCnt = 0;
	if (movexy) {
		m_AniMoveVec = m_PO - m_lookAt;
	}
	else {
		m_AniMoveVec = m_PO;
	}
	m_fAniMoveSca = m_cameraPri.GetLevelHeight() - MAX_CAM_HIGHTLEVEL * 2;

	SetTimer(_MOVECAMANI, 20, NULL);
}
void CMNView::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	float fLevelHeight = m_cameraPri.GetLevelHeight();
	float zoomValue = fLevelHeight*0.1f + MIN_CAM_HIGHTLEVEL;
	if (zDelta > 0) { zoomValue = -zoomValue; }
	fLevelHeight += zoomValue;

	if (fLevelHeight < MIN_CAM_HIGHTLEVEL) { fLevelHeight = MIN_CAM_HIGHTLEVEL; }

	m_cameraPri.SetInitLevelHeight(fLevelHeight);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
}


void CMNView::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPointSize(5);
	if (m_isAnimation) {
		DrawBGPageAni();
	}
	else {
		DrawBGPage();
	}
	glPointSize(1);
	//// Draw Word Boundary ==============================//
	//_vecPageObj::iterator iter = SINGLETON_TMat::GetInstance()->GetVecImageBegin();
	//for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++) {
	//	(*iter)->DrawWordBoundary(m_selWordId);
	//}
	
	DrawCNSRect(0.0f, 0.99f, 0.1f, 1.0f);

	Render2D();
	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CMNView::DrawCNSRect(float r, float g, float b, float a)
{
	glLineWidth(2);
	glDisable(GL_DEPTH_TEST);
	glColor4f(r, g, b, a);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glVertex3f(m_CNSRectStart.x, m_CNSRectEnd.y, m_CNSRectEnd.z);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectEnd.y, m_CNSRectEnd.z);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1);
}

void CMNView::Render2D()
{

}

void CMNView::DrawBGPageAni()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->DrawThumbNail(0.3f);
		vecImg[i]->RotatePos(1.0f);
		vecImg[i]->DrawMatchItem();
		vecImg[i]->DrawParagraph();
	}
	glPointSize(1);
}

void CMNView::DrawBGPage()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->DrawThumbNail(0.3f);
		vecImg[i]->DrawMatchItem();
		vecImg[i]->DrawParagraph();
	}
	glPointSize(1);
}


void CMNView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;
	//=======================//

	SelectObject3D(point.x, point.y, 2, 2, 0);
	m_stratPnt = point;

	if (m_bIsCutNSearchMode) {
		m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectStart);
		m_CNSRectEnd = m_CNSRectStart;
	}
	else {
		m_mouseMode = 2;
		IDragMap(point.x, point.y, 0);
	}

	SetCapture();
	COGLWnd::OnLButtonDown(nFlags, point);
}

void CMNView::IDragMap(int x, int y, short sFlag)
{
	POINT3D curPos, prePos, transPos, ptLookAt;
	BOOL res = FALSE;
	int dx = 0, dy = 0;


	switch (sFlag) {
	case 0:		// DOWN
		m_dragOper.init();
		m_dragOper.IsDrag = true;
		m_dragOper.px = x;
		m_dragOper.py = y;
		break;

	case 1:		// MOVE
		if (m_dragOper.IsDrag) {

			curPos = m_cameraPri.ScreenToWorld(x, y);
			prePos = m_cameraPri.ScreenToWorld(m_dragOper.px, m_dragOper.py);
			transPos = prePos - curPos;
			ptLookAt = m_cameraPri.GetLookAt();

			ptLookAt.x += transPos.x*m_moveVec.x;
			ptLookAt.y += transPos.y*m_moveVec.y;

			m_dragOper.px = x;
			m_dragOper.py = y;

			//	m_DemProj->ProjectPoint(&ptLookAt, &m_cameraPri.m_currentBlockid);
			m_cameraPri.SetModelViewMatrix(ptLookAt, 0, 0);
			m_lookAt = ptLookAt;
			//	UpdateCamera(ptLookAt, 0,0);
		}
		break;

	case 2:		// UP
		ReleaseCapture();
		m_dragOper.init();
		break;
	}
}


void CMNView::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_rectWidth = cx;
	m_rectHeight = cy;
	m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0.0f, 0.0f);
}


void CMNView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == _RENDER) {
		Render();
		// for debugging//

	}

	else if (nIDEvent == _GEN_THUMBNAIL) {
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Generating thumbnails.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		if (m_addImgCnt == m_loadedImgCnt) {
			m_addImgCnt = 0;
			KillTimer(_GEN_THUMBNAIL);
			UploadThumbnail();
		}
	}
	else if (nIDEvent == _DO_SEARCH) {
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Matching.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		if (m_addImgCnt == m_loadedImgCnt) {
			m_addImgCnt = 0;
			KillTimer(_DO_SEARCH);		

			pM->AddOutputString(L"Set Matching Results", false);
			SINGLETON_DataMng::GetInstance()->SetMatchingResults();
			pM->AddOutputString(L"Sorting Results", false);
			SINGLETON_DataMng::GetInstance()->SortMatchingResults();

			pM->AddOutputString(L"Start adding results into log list", false);
			pM->AddMatchResult();
			pM->AddOutputString(L"End adding results into log list", false);

			pM->ResizeListColSize(SINGLETON_DataMng::GetInstance()->GetMaxCutWidth());
		}
	}

	else if (nIDEvent == _DO_EXTRACTION) {
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Extraction.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		if (m_addImgCnt == m_loadedImgCnt) {
			m_addImgCnt = 0;
			KillTimer(_DO_EXTRACTION);

			// Rotate Page //
			//wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
			//SINGLETON_DataMng::GetInstance()->ApplyDeskewPage();

		}
	}

	else if (nIDEvent == _MOVECAMANI) {

		float fDelta = SINGLETON_DataMng::GetInstance()->GetAniAcceration(m_nAniCnt);

		m_lookAt.x = m_lookAt.x + m_AniMoveVec.x * fDelta;
		m_lookAt.y = m_lookAt.y + m_AniMoveVec.y * fDelta;
		m_lookAt.z = m_lookAt.z + m_AniMoveVec.z * fDelta;

		float levelheight = m_cameraPri.GetLevelHeight() - m_fAniMoveSca*fDelta; 
		m_cameraPri.SetInitLevelHeight(levelheight);
		m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);

		m_nAniCnt++;
		if (m_nAniCnt >= ANI_FRAME_CNT) {
			KillTimer(_MOVECAMANI);
			m_mouseMode = 0;
			IDragMap(m_stratPnt.x, m_stratPnt.y, 2);
		}
	}
	else if (nIDEvent == _UPDATE_PAGE) {
		SINGLETON_DataMng::GetInstance()->UpdatePageStatus(m_cameraPri.GetEye());
	}

	COGLWnd::OnTimer(nIDEvent);
}


void CMNView::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	COGLWnd::OnMouseHover(nFlags, point);
}


void CMNView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsCutNSearchMode) {
		if (m_stateKeyDown == false) {
			m_bIsCutNSearchMode = false;
			SendMessage(WM_SETCURSOR);
		}
	}
	else {
		m_mouseMode = 0;
		IDragMap(point.x, point.y, 2);
	}
	ReleaseCapture();
	COGLWnd::OnLButtonUp(nFlags, point);
}

void CMNView::MoveCameraPos(POINT3D target, int zoom)
{
	KillTimer(_MOVECAMANI);
	m_nAniCnt = 0;
	m_AniMoveVec = target - m_lookAt;
	m_fAniMoveSca = m_cameraPri.GetLevelHeight() - zoom;
	SetTimer(_MOVECAMANI, 20, NULL);
}

void CMNView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if (m_pSelectPageForCNS) {
		m_pSelectPageForCNS->SetCandidate(true);
		MoveCameraPos(m_pSelectPageForCNS->GetPos(), (DEFAULT_PAGE_SIZE + 200));
		//KillTimer(_MOVECAMANI);
		//m_nAniCnt = 0;
		//m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
		//m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;
		//SetTimer(_MOVECAMANI, 20, NULL);
	}

	COGLWnd::OnLButtonDblClk(nFlags, point);
}

void CMNView::UploadThumbnail()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->UploadThumbImage();
	}
}

void CMNView::GenerateThumbnail()
{
	m_addImgCnt = 0;	
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	m_loadedImgCnt = vecImg.size();

	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->LoadThumbImage(THUMBNAIL_SIZE);
		m_addImgCnt++;
	}
}

int CMNView::SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode)
{
	if (m_pSelectPageForCNS) {
		m_pSelectPageForCNS->SetSelection(false);
		m_pSelectPageForCNS = NULL;
	}

	if (m_IsSearchMatchItems)
		m_selMatchItemId = -1;


	GLuint selectBuff[1024];
	memset(&selectBuff, 0, sizeof(GLuint) * 1024);

	GLint hits, viewport[4];
	hits = 0;

	glSelectBuffer(1024, selectBuff);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glRenderMode(GL_SELECT);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, 2, 2, viewport);
	gluPerspective(m_cameraPri.m_Cntfovy, (float)viewport[2] / (float)viewport[3], m_cameraPri.GetNearPlane(), m_cameraPri.GetFarPlane());

	//m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	glMatrixMode(GL_MODELVIEW);

	// Draw for picking==============================//
	if (m_IsSearchMatchItems) {
		DrawMatchItemForPicking();
	}
	else {
		DrawImageByOrderForPicking();
	}
	//=============================================//

	hits = glRenderMode(GL_RENDER);
	if (hits > 0)
	{
		if (m_IsSearchMatchItems) {
			m_selMatchItemId = selectBuff[3];
			m_pSelectPageForCNS->SetSelMatchItem(m_selMatchItemId);
		}
		else {
			m_pSelectPageForCNS = SINGLETON_DataMng::GetInstance()->GetPageByOrderID(selectBuff[3]);
			if (m_pSelectPageForCNS)
				m_pSelectPageForCNS->SetSelection(true);
		}
	}
	else {
		if (m_pSelectPageForCNS)
			m_pSelectPageForCNS->SetSelMatchItem(-1);
	}
	//===============================================//

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return hits;
}


void CMNView::DrawImageByOrderForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		glPushName(i);
		vecImg[i]->DrawForPicking();
		glPopName();
	}
}

void CMNView::DrawMatchItemForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		glPushName(i);
		vecImg[i]->DrawMatchItemForPick();
		glPopName();
	}
}




void CMNView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture()) {
		if (m_bIsCutNSearchMode) {
			m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectEnd);
		}
		else {
			int xDelta = point.x - m_stratPnt.x;
			int yDelta = point.y - m_stratPnt.y;

			if (xDelta*xDelta > yDelta*yDelta) {
				m_moveVec.x = 1.0f;			m_moveVec.y = 0;
			}
			else {
				m_moveVec.y = 1.0f;			m_moveVec.x = 0;
			}
			if (m_mouseMode == 2) {		// MOVE
				if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)) {
					IDragMap(point.x, point.y, 1);
				}
			}
		}
	}
	COGLWnd::OnMouseMove(nFlags, point);
}

void CMNView::EnableCutSearchMode(bool IsEnable, bool bKey)
{
	m_bIsCutNSearchMode = IsEnable;
	m_stateKeyDown = bKey;
}

BOOL CMNView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsCutNSearchMode) {
		SetCursor(LoadCursor(NULL, IDC_CROSS));
		return TRUE;
	}
	else {
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}

	return COGLWnd::OnSetCursor(pWnd, nHitTest, message);
}

RECT2D CMNView::GetSelectedAreaForCNS()
{
	if (m_pSelectPageForCNS) {
		RECT2D selRect = m_pSelectPageForCNS->ConvertVec3DtoImgateCoord(m_CNSRectStart, m_CNSRectEnd);

		if ((selRect.width < 0) || (selRect.width>512) || (selRect.height < 0) || (selRect.height>512))
			selRect = RECT2D();

		return selRect;
	}
	return RECT2D();
}



bool CMNView::DoSearch()
{
	if (m_pCut.ptr() == NULL)
		return false;
	int search_size = (m_pCut.cols > m_pCut.rows) ? m_pCut.cols : m_pCut.rows;

	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();

	m_addImgCnt = 0;
	m_loadedImgCnt = imgVec.size();

	size_t i = 0;
	for (i = 0; i < imgVec.size(); i++) {

		cv::Mat grayImg = imgVec[i]->GetSrcPageGrayImg();
		//if (SINGLETON_DataMng::GetInstance()->LoadImageData(imgVec[i]->GetPath(), grayImg, true)) {
		if (grayImg.ptr()) {
			int result_cols = grayImg.cols - m_pCut.cols + 1;
			int result_rows = grayImg.rows - m_pCut.rows + 1;
			cv::Mat result(result_rows, result_cols, CV_32FC1);

			cv::matchTemplate(grayImg, m_pCut, result, CV_TM_CCOEFF_NORMED);
			//cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

			for (int y = 0; y < result.rows; y++) {
				for (int x = 0; x < result.cols; x++) {
					float fD = result.at<float>(y, x);
					if (fD > m_fThreshold) {
						stMatchInfo mInfo;
						mtSetPoint3D(&mInfo.pos, x + m_pCut.cols*0.5f, y + m_pCut.rows*0.5f, 0.0f);
						mInfo.accuracy = fD * 100;
						mInfo.strAccracy.Format(L"%d", (int)(fD));
						mInfo.rect = cv::Rect(x, y, m_pCut.cols, m_pCut.rows);
						mInfo.searchId = m_cnsSearchId;
						mInfo.cInfo = m_cutInfo;
						mInfo.strCode = "-";

						m_resColor.a = ((fD)*m_colorAccScale)*0.5f;
						mInfo.color = m_resColor;
						mInfo.IsAdded = false;
						imgVec[i]->AddMatchedPoint(std::move(mInfo), search_size);
					}
				}
			}
			//grayImg.release();
			result.release();

			imgVec[i]->SetIsSearched(true);
			if (i > 0) {
				imgVec[i-1]->SetIsSearched(false);
			}
		}		
		m_addImgCnt++;
	}

	imgVec[i - 1]->SetIsSearched(false);
	m_cnsSearchId++;
	return true;
}


// Thread Functions ====================================//
void CMNView::ProcGenerateThumbnail()
{
	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadGenThumbnailImg, this);

	SetTimer(_GEN_THUMBNAIL, 100, NULL);
	//GenerateThumbnail();

}


void CMNView::DoExtractBoundary()
{
	//cv::Mat hHisto;
	//cv::Mat vHisto;

	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	m_addImgCnt = 0;
	m_loadedImgCnt = vecImg.size();
	int i;
	for (i = 0; i < (int)vecImg.size(); i++) {
		CString strpath = vecImg[i]->GetPath();
		cv::Mat srcImg = vecImg[i]->GetSrcPageGrayImg();
		if (srcImg.ptr()) {

		//	cv::GaussianBlur(srcImg, srcImg, cv::Size(7, 7), 0);
			cv::threshold(srcImg, srcImg, 125, 255, cv::THRESH_OTSU);
		//	cv::threshold(srcImg, b2, 0, 255, CV_THRESH_BINARY + cv::THRESH_OTSU);
			cv::bitwise_not(srcImg, srcImg);

			int fsize = (int)m_extractionSetting.engSize;

			// Detect text block //
			_ALIGHN_TYPE align; 
			std::vector<_extractBox> vecBox;
			if (m_extractionSetting.nAlign == 0) {
				m_Extractor.Extraction(srcImg, fsize*2, -1, vecBox, _ALPHABETIC, _UNKNOWN_ALIGN);
				align = _HORIZON_ALIGN;
			}
			else {
				m_Extractor.Extraction(srcImg, -1, fsize * 2, vecBox, _NONALPHABETIC, _UNKNOWN_ALIGN);
				align = _VERTICAL_ALIGN;
			}

			for (size_t j = 0; j < vecBox.size(); j++) {
				// Calculate Deskew //
				//m_Extractor.verifyImgSize(vecBox[j].textbox, srcImg.cols, srcImg.rows);
				cv::Mat para = srcImg(vecBox[j].textbox);
				//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);
				
				float deskew = m_Extractor.DeSkewImg(para);
				TRACE(L"Deskew Angle: %3.2f\n", deskew);
				vecImg[i]->AddParagraph(vecBox[j].textbox, align, deskew);
			

				// Rotate Image //

			//	if (deskew < -3.0f) deskew = -3.0f;
			//	if (deskew > 3.0f) deskew = 3.0f;
			//	cv::Mat rotMat, rotatedFrame, invRot;
			//	rotMat = getRotationMatrix2D(cv::Point2f(0, 0), deskew, 1);
			////	invRot = getRotationMatrix2D(cv::Point2f(0, 0), -deskew, 1);
			//	cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC);

				//CString strTitle;
				//strTitle.Format(L"desket %d", j);
				//USES_CONVERSION;
				//char* sz = T2A(strTitle);
				//cv::imshow(sz, rotatedFrame);

				// Line Detection //
				//std::vector<_extractBox> vecLines;

				//if (align == _HORIZON_ALIGN) {
				//	m_Extractor.Extraction(rotatedFrame, 16, -1, vecLines, _ALPHABETIC, align);
				//}
				//else {
				//	m_Extractor.Extraction(rotatedFrame, 0, 16, vecLines, _NONALPHABETIC, align);
				//}

				//
				//for (size_t k = 0; k < vecLines.size(); k++) {
				//	cv::Rect r = vecLines[k].textbox;
				//	cv::RotatedRect rRect = cv::RotatedRect(cv::Point2f(r.x, r.y), cv::Size2f(r.width, r.height), deskew);
				//	cv::Point2f vertices[4];
				//	rRect.points(vertices);

				//	r.x += vecBox[j].textbox.x;
				//	r.y += vecBox[j].textbox.y;

				//	vecImg[i]->AddParagraph(r, align, deskew);
				//}


				//cv::Mat hHisto = m_Extractor.GetLinesbyHistogram(rotatedFrame, vecLines,0);
				//cv::Mat vHisto = m_Extractor.GetLinesbyHistogram(rotatedFrame, vecLines, 1);
				//cv::imshow("origin", rotatedFrame);
				//cv::imshow("hh", hHisto);
				//cv::imshow("vv", vHisto);


			}
			vecBox.clear();
			srcImg.release();
		}

		vecImg[i]->SetIsSearched(true);
		if (i > 0) {
			vecImg[i - 1]->SetIsSearched(false);
		}

		m_addImgCnt++;
	}
	
	vecImg[i - 1]->SetIsSearched(false);
}

void CMNView::ProcExtractBoundary(_stExtractionSetting _info)
{
	m_extractionSetting = _info;
//	InitCamera(false);
	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadDoExtraction, this);

	//DoExtractBoundary();


	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < imgVec.size(); i++) {
		imgVec[i]->SetIsSearched(false);
	}

	SetTimer(_DO_EXTRACTION, 100, NULL);
}

void CMNView::ProcDoSearch()
{
	InitCamera(false);	
	if (m_pSelectPageForCNS) {

		// Prepare Cut&Search ==========================================//
		RECT2D rect = GetSelectedAreaForCNS();

		// Update Font Size ==//
		m_Extractor.SetFontSize(rect.width, rect.height);

		if (m_pCut.ptr() != NULL) {
			m_pCut.release();
		}
		CString strpath = m_pSelectPageForCNS->GetPath();
		cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg();;
		if (srcImg.ptr()) {
			m_pCut = srcImg(cv::Rect(rect.x1, rect.y1, rect.width, rect.height));
		}

		
		m_cutInfo.fileid = getHashCode((CStringA)m_pSelectPageForCNS->GetPath());
		m_cutInfo.posid = (int)rect.x1 * 10000 + (int)rect.y1;
		CString strId;
		strId.Format(L"%u%u", m_cutInfo.fileid, m_cutInfo.posid);
		m_cutInfo.id = getHashCode((CStringA)strId);
		m_cutInfo.th = m_fThreshold;

		srcImg.release();
		//==============================================================//
		CWinThread* pl;
		m_bIsThreadEnd = false;
		pl = AfxBeginThread(ThreadDoSearch, this);

		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		for (size_t i = 0; i < imgVec.size(); i++) {
			imgVec[i]->SetIsSearched(false);
		}

		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		m_fThreshold = pM->GetThreshold()*0.01f;
		COLORREF resColor = pM->GetMatchingColor();
		m_resColor.r = (float)GetRValue(resColor)*0.00392f;
		m_resColor.g = (float)GetGValue(resColor)*0.00392f;
		m_resColor.b = (float)GetBValue(resColor)*0.00392f;

		SetTimer(_DO_SEARCH, 100, NULL);
	}
	
	


	//RECT2D rect = GetSelectedAreaForCNS();
	//if (rect.width > 0) {
	//	m_pMatchingProcessor.PrepareCutNSearch(m_pViewImage->GetSelectedPageForCNS(), m_pViewImage->GetSelectedAreaForCNS());

	//	m_searchCnt = 0;
	//	SetTimer(_TIMER_SEARCH_PAGE, 10, NULL);
	//	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	//	pM->AddOutputString(_T("Start Search Process...."), true);
	//}
	//else {
	//	AfxMessageBox(L"Select area for Searching");
	//}

}



void CMNView::MovePrePage()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250) {
		SelectObject3D(10, m_rectHeight*0.5f, 2, 2, 0);
		if (m_pSelectPageForCNS) {

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}
void CMNView::MoveNextPage()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250) {
		SelectObject3D(m_rectWidth - 10, m_rectHeight*0.5f, 2, 2, 0);
		if (m_pSelectPageForCNS) {

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}

void CMNView::MoveNextUp()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250) {
		SelectObject3D(m_rectWidth*0.5f, 10, 2, 2, 0);
		if (m_pSelectPageForCNS) {

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}

void CMNView::MoveNextDown()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250) {
		SelectObject3D(m_rectWidth*0.5f, m_rectHeight - 10, 2, 2, 0);
		if (m_pSelectPageForCNS) {

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}






// Thread============//
UINT ThreadGenThumbnailImg(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->GenerateThumbnail();
	return 0L;
}

static UINT ThreadDoSearch(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoSearch();
	return 0L;
}

static UINT ThreadDoExtraction(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoExtractBoundary();
	return 0L;
}