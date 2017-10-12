#include "stdafx.h"
#include "MNView.h"
#include "MNDataManager.h"
#include "MainFrm.h"
#include "resource.h"

enum TIMEREVNT { _RENDER = 100, _ADDIMG, _SEARCHIMG, _MOVECAMANI, _UPDATE_PAGE, _GEN_THUMBNAIL, _DO_SEARCH, _DO_EXTRACTION, _DO_OCR};


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
	m_LogFont.lfCharSet = DEFAULT_CHARSET;
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
	m_selParaId = -1;
	m_selOCRId = -1;

	m_bIsAllOCR = false;
	m_bIsShowParagraph = true;
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
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
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

	if ((m_selParaId >= 0)&&(m_pSelectPageForCNS)){
		m_pSelectPageForCNS->DrawSelectedParagraph(m_selParaId);
	}

	DrawOCRRes();

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

		if(m_bIsShowParagraph)
			vecImg[i]->DrawParagraph(m_selParaId);
	//	vecImg[i]->DrawOCRRes();
	}
	glPointSize(1);
}

void CMNView::DrawBGPage()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->DrawThumbNail(0.3f);
		vecImg[i]->DrawMatchItem();
		if (m_bIsShowParagraph)
			vecImg[i]->DrawParagraph(m_selParaId);
	//	vecImg[i]->DrawOCRRes();
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
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if (nIDEvent == _RENDER) {
		Render();
		// for debugging//

	}

	else if (nIDEvent == _GEN_THUMBNAIL) {		
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Image Loading.....%d"), int(complete * 100));
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

			str.Format(_T("Extraction.....done"));
			pM->AddOutputString(str, true);
			// Rotate Page //
			//wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
			//SINGLETON_DataMng::GetInstance()->ApplyDeskewPage();

		}
	}

	else if (nIDEvent == _DO_OCR) {
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Text Recognition.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		if (m_addImgCnt == m_loadedImgCnt) {
			m_addImgCnt = 0;
			KillTimer(_DO_OCR);

			str.Format(_T("Text Recognition.....done"));
			pM->AddOutputString(str, true);
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
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

	if (m_pSelectPageForCNS) {
		m_pSelectPageForCNS->SetSelection(false);
		m_pSelectPageForCNS = NULL;
		m_selParaId = -1;
		m_selOCRId = -1;
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
	//if (m_IsSearchMatchItems) {
		
	//}
	//else {
	DrawImageByOrderForPicking();
	if (m_bIsShowParagraph)
		DrawParagrphForPicking();
	DrawMatchItemForPicking();
	DrawOCRForPicking();
	
	//}
	//=============================================//

	hits = glRenderMode(GL_RENDER);
	if (hits > 0)
	{
		for (int i = 0; i < hits; i++) {
			int selid = selectBuff[i * 4 + 3];

			if (selid < _PICK_PARA) {		// Page selection
				m_pSelectPageForCNS = SINGLETON_DataMng::GetInstance()->GetPageByOrderID(selid);
				if (m_pSelectPageForCNS)
					m_pSelectPageForCNS->SetSelection(true);
			}
			else if ((selid >= _PICK_PARA) && (selid < _PICK_WORD)) {		// Text block selectoin
				m_selParaId = selid - _PICK_PARA;
				float fdeskew = m_pSelectPageForCNS->GetDeskewParam(m_selParaId);
				pM->SetParagraphInfo(fdeskew, m_pSelectPageForCNS->GetName());
			}
			else if ((selid >= _PICK_WORD) && (selid < _PICK_MATCH)){		// Word selection
				m_selOCRId = selid - _PICK_WORD;
				_stOCRResult ocrRes = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
				pM->SetOCRResInfo(ocrRes.strCode, ocrRes.fConfidence, ocrRes.type);
			}
			else if (selid >= _PICK_MATCH) {
				selid = selid - _PICK_MATCH;
			}
		}
	}

	//	if (m_IsSearchMatchItems) {
	//		m_selMatchItemId = selectBuff[3];
	//		m_pSelectPageForCNS->SetSelMatchItem(m_selMatchItemId);
	//	}
	//	else {
	//		m_pSelectPageForCNS = SINGLETON_DataMng::GetInstance()->GetPageByOrderID(selectBuff[3]);
	//		if (m_pSelectPageForCNS)
	//			m_pSelectPageForCNS->SetSelection(true);
	//	}
	//}
	//else {
	//	if (m_pSelectPageForCNS)
	//		m_pSelectPageForCNS->SetSelMatchItem(-1);
	//}
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

void CMNView::DrawParagrphForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {		
		vecImg[i]->DrawParagraphForPick();
	}
}


void CMNView::DrawMatchItemForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		vecImg[i]->DrawMatchItemForPick();
	}
}

void CMNView::DrawOCRForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		vecImg[i]->DrawOCRResForPick();
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

	//	if ((selRect.width < 0) || (selRect.width>512) || (selRect.height < 0) || (selRect.height>512))
		if ((selRect.width < 0) || (selRect.height < 0))
			selRect = RECT2D();

		return selRect;
	}
	return RECT2D();
}

void CMNView::DoOCR()
{
	if (m_bIsAllOCR) {
		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		m_addImgCnt = 0;
		m_loadedImgCnt = imgVec.size();

		size_t i = 0;
		for (i = 0; i < imgVec.size(); i++) {

			if (imgVec[i]->GetVecOCRResult().size() > 0) {
				continue;
			}

			DoOCRForPage(imgVec[i]);

			m_addImgCnt++;
			imgVec[i]->SetIsSearched(true);
			if (i > 0) {
				imgVec[i - 1]->SetIsSearched(false);
			}
		}
		imgVec[i - 1]->SetIsSearched(false);
	}

	else {
		if (m_pSelectPageForCNS) {
			m_addImgCnt = 0;
			m_loadedImgCnt = 1;
			DoOCRForPage(m_pSelectPageForCNS);
			m_addImgCnt = 1;
		}
	}
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
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Image Loading.....start"));
	pM->AddOutputString(str, false);

	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadGenThumbnailImg, this);

	SetTimer(_GEN_THUMBNAIL, 100, NULL);

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
		if (vecImg[i]->GetVecParagraph().size()>0){
			continue;
		}

		CString strpath = vecImg[i]->GetPath();
		cv::Mat srcImg = vecImg[i]->GetSrcPageGrayImg().clone();
		if (srcImg.ptr()) {

		//	cv::GaussianBlur(srcImg, srcImg, cv::Size(7, 7), 0);
			cv::threshold(srcImg, srcImg, 200, 255, cv::THRESH_OTSU);
		//	cv::threshold(srcImg, b2, 0, 255, CV_THRESH_BINARY + cv::THRESH_OTSU);
			cv::bitwise_not(srcImg, srcImg);

			int fsize = (int)m_extractionSetting.engSize;

			// Detect text block //
			std::vector<_extractBox> vecBox;
			if (m_extractionSetting.isEng) {
				ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __ENG);		// Start with English //
			}
			else {
				if (m_extractionSetting.isChi) {
					ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __CHI);
				}
				else if (m_extractionSetting.isKor) {
					ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __KOR);
				}
			}
			//if (m_extractionSetting.IsHori) {
			//	m_Extractor.Extraction(srcImg, fsize*2, -1, vecBox);
			//}
			//else {
			//	m_Extractor.Extraction(srcImg, -1, fsize * 2, vecBox);
			//}

			for (size_t j = 0; j < vecBox.size(); j++) {
				// Calculate Deskew //
				//m_Extractor.verifyImgSize(vecBox[j].textbox, srcImg.cols, srcImg.rows);
				cv::Mat para = srcImg(vecBox[j].textbox);
				//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);
				
				float deskew = m_Extractor.DeSkewImg(para);
				TRACE(L"Deskew Angle: %3.2f\n", deskew);
				vecImg[i]->AddParagraph(vecBox[j].textbox, m_extractionSetting.IsVerti, deskew);
			

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

void CMNView::ProcOCR(bool IsAll)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Text Recognition.....start"));
	pM->AddOutputString(str, false);

	m_bIsAllOCR = IsAll;

	pM->GetCurrSetting();
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();


//	InitCamera(false);
	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadDoOCR, this);

	//DoExtractBoundary();
	//DoOCR();

	//std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	//for (size_t i = 0; i < imgVec.size(); i++) {
	//	imgVec[i]->SetIsSearched(false);
	//}

	SetTimer(_DO_OCR, 100, NULL);
}

void CMNView::ProcExtractBoundary(_stExtractionSetting _info)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Extraction.....start"));
	pM->AddOutputString(str, false);


	m_extractionSetting = _info;
//	InitCamera(false);
	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadDoExtraction, this);


	//std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	//for (size_t i = 0; i < imgVec.size(); i++) {
	//	imgVec[i]->SetIsSearched(false);
	//}

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

void CMNView::DeleteSelParagraph()
{
	if((m_pSelectPageForCNS)){
		m_pSelectPageForCNS->DeleteSelPara(m_selParaId);
		m_selParaId = -1;
	}
}

bool CMNView::DeleteSelOCRRes()
{
	bool res = false;
	if ((m_pSelectPageForCNS)) {
		res = m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
		m_selOCRId = -1;
	}
	return res;
}

void CMNView::ConfirmOCRRes()
{
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->ConfirmOCRRes(m_selOCRId);
		m_selOCRId = -1;
	}
}

void CMNView::UpdateOCRCode(CString _strCode)
{
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->UpdateOCRCode(_strCode, m_selOCRId);
		m_selOCRId = -1;
	}
}

void CMNView::DeleteAllLines()
{
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->ClearParagraph();
		m_selParaId = -1;
	}
}

void CMNView::DeleteAllOCRRes()
{
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->ClearOCRResult();
		m_selOCRId = -1;
	}
}

void CMNView::AddOCRRes()
{
	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {

			_stOCRResult ocrres;
			ocrres.fConfidence = 0.0f;
			ocrres.type = 0;
			ocrres.rect.x = rect.x1;
			ocrres.rect.y = rect.y1;
			ocrres.rect.width = rect.width;
			ocrres.rect.height = rect.height;
			memset(&ocrres.strCode, 0x00, sizeof(ocrres.strCode));
			m_pSelectPageForCNS->AddOCRResult(ocrres);
		}
	}
}


void CMNView::AddParagraph()
{
	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {

			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg();
			if (srcImg.ptr()) {

				cv::Rect r;
				r.x = rect.x1;
				r.y = rect.y1;
				r.width = rect.width;
				r.height = rect.height;

				cv::Mat para = srcImg(r);
				float deskew = m_Extractor.DeSkewImg(para);
				m_pSelectPageForCNS->AddParagraph(r, _HORIZON_ALIGN, deskew);

			//	cv::Mat resizeImg;
			//	cv::resize(para, resizeImg, cv::Size(para.cols*2, para.rows*2));
			//	cv::imshow("AddPara1", resizeImg);
			////	cv::adaptiveThreshold(para, para, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 13, 1);
			////	cv::threshold(resizeImg, resizeImg, 150, 255, cv::THRESH_OTSU);
			//	//cv::bitwise_not(para, para);

			//	

			//	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			//		cv::Size(3,3),
			//		cv::Point(-1, -1));
			//	cv::erode(resizeImg, resizeImg, element);

			//	cv::resize(resizeImg, para, cv::Size(para.cols, para.rows));

			//	cv::imshow("AddPara2", resizeImg);
			//	cv::threshold(para, para, 200, 255, cv::THRESH_OTSU);
			//	cv::imshow("AddPara3", para);
				para.release();
			}
		}
	}
}

void CMNView::DeskewParagraph(float fAngle)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->DeSkewImg(m_selParaId, fAngle);
		m_pSelectPageForCNS->UpdateTexture();
	}
}

void CMNView::UndoDeskewParagraph()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->UnDoDeSkewImg(m_selParaId);
	}
}

void CMNView::ExtractBox(cv::Mat& img, std::vector<_extractBox>& vecBox, bool IsVerti, _LANGUAGE_TYPE lang)
{
	if (img.ptr()) {
	//	cv::threshold(para, para, 125, 255, cv::THRESH_OTSU);
	//	cv::bitwise_not(para, para);
		int x_ext = 0, y_ext = 0;
		int fsize = 0;
		if (lang == __ENG) {
			fsize = (int)m_extractionSetting.engSize;
			x_ext = fsize * 2;
			y_ext = -1;
		}
		else if (lang == __CHI) {
			fsize = (int)m_extractionSetting.chiSize;
			x_ext = 0;
			y_ext = 1;
		}
		else if (lang == __KOR) {
			fsize = (int)m_extractionSetting.korSize;
			x_ext = 1;
			y_ext = 1;
		}

		m_Extractor.Extraction(img, x_ext, y_ext, vecBox);
	}
}



void CMNView::ReExtractParagraph(_LANGUAGE_TYPE lang, _ALIGHN_TYPE align)
{
	if ((m_pSelectPageForCNS)) {
		cv::Rect r = m_pSelectPageForCNS->GetSelParaRect(m_selParaId);
		if ((r.width > 0) && (r.height > 0)) {

			// Delete Previous Para rect //
			m_pSelectPageForCNS->DeleteSelPara(m_selParaId);

			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg();
			if (srcImg.ptr()) {
				cv::Mat para = srcImg(r);
				cv::threshold(para, para, 125, 255, cv::THRESH_OTSU);
				cv::bitwise_not(para, para);

				//int fsize = (int)m_extractionSetting.engSize;
				//// Detect text block //
				std::vector<_extractBox> vecLines;
				ExtractBox(para, vecLines, m_extractionSetting.IsVerti, lang);
				//_ALIGHN_TYPE align;
				//if (m_extractionSetting.nAlign == 0) {
				//	m_Extractor.Extraction(para, fsize * 2, -1, vecLines, _ALPHABETIC, _UNKNOWN_ALIGN);
				//	align = _HORIZON_ALIGN;
				//}
				//else {
				//	m_Extractor.Extraction(para, -1, fsize * 2, vecLines, _NONALPHABETIC, _UNKNOWN_ALIGN);
				//	align = _VERTICAL_ALIGN;
				//}

				for (size_t j = 0; j < vecLines.size(); j++) {
					// Calculate Deskew //
					cv::Mat imgLine = para(vecLines[j].textbox);
					float deskew = m_Extractor.DeSkewImg(imgLine);

					vecLines[j].textbox.x += r.x;
					vecLines[j].textbox.y += r.y;
					m_pSelectPageForCNS->AddParagraph(vecLines[j].textbox, m_extractionSetting.IsVerti, deskew);
				}
				para.release();
			}
		}
	}
}


void CMNView::DrawOCRRes()
{
//	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	glLineWidth(2);
//	for (size_t i = 0; i < imgVec.size(); i++) {
	if (m_pSelectPageForCNS) {
		if (m_pSelectPageForCNS->IsNear()) {

			glPushMatrix();
			glTranslatef(m_pSelectPageForCNS->GetPos().x, m_pSelectPageForCNS->GetPos().y, m_pSelectPageForCNS->GetPos().z);

			std::vector<_stOCRResult> ocrRes = m_pSelectPageForCNS->GetVecOCRResult();
			if (ocrRes.size() > 0) {
				// Draw detected position //
				glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
				glPushMatrix();
				glScalef(m_pSelectPageForCNS->GetfXScale(), m_pSelectPageForCNS->GetfYScale(), 1.0f);
				glTranslatef(-m_pSelectPageForCNS->GetImgWidth()*0.5f, -m_pSelectPageForCNS->GetImgHeight()*0.5f, 0.0f);

				//if (m_bIsNear){		
				glLineWidth(1);
				POINT3D tPos, tColor;
				RECT2D rect;
				for (int j = 0; j < ocrRes.size(); j++) {

					rect.set(ocrRes[j].rect.x, ocrRes[j].rect.x + ocrRes[j].rect.width, ocrRes[j].rect.y, ocrRes[j].rect.y + ocrRes[j].rect.height);

					// Draw Text //
					glColor4f(0.0f, 0.0f, 1.0f, 0.99f);
					mtSetPoint3D(&tPos, (rect.x1 + rect.x2)*0.5f, m_pSelectPageForCNS->GetImgHeight() - rect.y1 + 5, 1.0f);
					gl_DrawText(tPos, ocrRes[j].strCode, m_LogFont, 1, m_pBmpInfo, m_CDCPtr);

					
					mtSetPoint3D(&tColor, 0.0f, 1.0f, 0.0f);
					if (ocrRes[j].fConfidence < 0.80f)
						mtSetPoint3D(&tColor, 1.0f, 0.0f, 0.0f);

					glColor4f(tColor.x, tColor.y, tColor.z, 0.2f);
					if (j == m_selOCRId) {
						glColor4f(tColor.x, tColor.y, tColor.z, 0.5f);
						//glBegin(GL_QUADS);
						//glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
						//glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y2, 0.0f);
						//glVertex3f(rect.x2, m_pSelectPageForCNS->GetImgHeight() - rect.y2, 0.0f);
						//glVertex3f(rect.x2, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
						//	glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
						//glEnd();
					}
					
					glBegin(GL_QUADS);
					glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
					glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y2, 0.0f);
					glVertex3f(rect.x2, m_pSelectPageForCNS->GetImgHeight() - rect.y2, 0.0f);
					glVertex3f(rect.x2, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
					//glVertex3f(rect.x1, m_pSelectPageForCNS->GetImgHeight() - rect.y1, 0.0f);
					glEnd();
				}

				glPopMatrix();
			}
			glPopMatrix();
		}
	}
	glLineWidth(1);
}

void CMNView::DoOCRForCutImg(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage)
{
	std::vector<_stOCRResult> ocrRes;
	if (m_extractionSetting.isEng) {
		m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetEngTess(), tesseract::RIL_WORD);
	}

	std::vector<_stOCRResult> ocrResChi;
	if (m_extractionSetting.isChi) {
		if (ocrRes.size() > 0) {
			for (int k = 0; k < ocrRes.size(); k++) {

				if (ocrRes[k].fConfidence < 0.70f) {
					cv::Mat imgword = img(ocrRes[k].rect);

					std::vector<_stOCRResult> ocrTmp;
					m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);


					// Select Result between chi and eng //
					float averConf = 0;
					for (int m = 0; m < ocrTmp.size(); m++) {
						averConf += ocrTmp[m].fConfidence;
					}
					averConf /= (float)ocrTmp.size();

					if (ocrRes[k].fConfidence < averConf*0.9f) {
						ocrRes[k].type = 100;
					}

					//===========================//
					if (ocrRes[k].type == 100) {		// Substitute chi result to eng result //
						for (int m = 0; m < ocrTmp.size(); m++) {
							ocrTmp[m].rect.x += ocrRes[k].rect.x;
							ocrTmp[m].rect.y += ocrRes[k].rect.y;
							ocrResChi.push_back(ocrTmp[m]);
						}
					}
					//================//
				}
			}
		}
		else {
			m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);
		}
	}


	for (int k = 0; k < ocrRes.size(); k++) {
		if (ocrRes[k].type < 100) {
			ocrRes[k].rect.x += rect.x;
			ocrRes[k].rect.y += rect.y;
			ocrRes[k].type = 0;
			pPage->AddOCRResult(ocrRes[k]);
		}
	}

	for (int k = 0; k < ocrResChi.size(); k++) {
		ocrResChi[k].rect.x += rect.x;
		ocrResChi[k].rect.y += rect.y;
		ocrResChi[k].type = 1;
		pPage->AddOCRResult(ocrResChi[k]);
	}
}

void CMNView::OcrEnglishword()
{
	if (m_pSelectPageForCNS) {
		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);

		cv::Rect r = ocrres.rect;
		r.x -= 1;
		r.y -= 1;
		r.width += 2;
		r.height += 2;

		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);

		m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
		std::vector<_stOCRResult> ocrTmp;
		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_WORD);

		// Update OCR Res //
		if (ocrTmp.size() >= 0) {
			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
			for (int i = 0; i < ocrTmp.size(); i++) {
				ocrTmp[i].rect.x += r.x;
				ocrTmp[i].rect.y += r.y;
				ocrTmp[i].type = 0;
				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
			}
			m_selOCRId = -1;
		}
	}
}

void CMNView::OcrEnglishChar()
{
	if (m_pSelectPageForCNS) {
		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);

		cv::Rect r = ocrres.rect;
		r.x -= 1;
		r.y -= 1;
		r.width += 2;
		r.height += 2;

		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);

		m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
		std::vector<_stOCRResult> ocrTmp;
		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL);

		// Update OCR Res //
		if (ocrTmp.size() >= 0) {
			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
			for (int i = 0; i < ocrTmp.size(); i++) {
				ocrTmp[i].rect.x += r.x;
				ocrTmp[i].rect.y += r.y;
				ocrTmp[i].type = 0;
				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
			}
			m_selOCRId = -1;
		}
	}
}

void CMNView::OcrChiChar()
{
	if (m_pSelectPageForCNS) {
		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);

		cv::Rect r = ocrres.rect;
		r.x -= 1;
		r.y -= 1;
		r.width += 2;
		r.height += 2;

		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);

		m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_CHAR);
		std::vector<_stOCRResult> ocrTmp;
		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);

		// Update OCR Res //
		if (ocrTmp.size() >= 0) {
			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
			for (int i = 0; i < ocrTmp.size(); i++) {
				ocrTmp[i].rect.x += r.x;
				ocrTmp[i].rect.y += r.y;
				ocrTmp[i].type = 1;
				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
			}
			m_selOCRId = -1;
		}
	}
}

void CMNView::OcrChiWord()
{
	if (m_pSelectPageForCNS) {
		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);

		cv::Rect r = ocrres.rect;
		r.x -= 1;
		r.y -= 1;
		r.width += 2;
		r.height += 2;

		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);

		m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_WORD);
		std::vector<_stOCRResult> ocrTmp;
		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);

		// Update OCR Res //
		if (ocrTmp.size() >= 0) {
			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
			for (int i = 0; i < ocrTmp.size(); i++) {
				ocrTmp[i].rect.x += r.x;
				ocrTmp[i].rect.y += r.y;
				ocrTmp[i].type = 1;
				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
			}
			m_selOCRId = -1;
		}
	}
}

void CMNView::DoOCRForPage(CMNPageObject* pPage)
{
	m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_BLOCK);
	m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_LINE);

	if (pPage) {
		if (pPage->GetVecOCRResult().size() == 0) {
			cv::Mat gray = pPage->GetSrcPageGrayImg();
			std::vector<stParapgraphInfo> para = pPage->GetVecParagraph();


			for (int j = 0; j < para.size(); j++) {
				cv::Mat imgLine = gray(para[j].rect);

				DoOCRForCutImg(imgLine, para[j].rect, pPage);
				//	cv::threshold(imgLine, imgLine, 150, 255, cv::THRESH_BINARY);
				//std::vector<_stOCRResult> ocrRes;
				//if (m_extractionSetting.isEng) {
				//	m_OCRMng.extractWithOCR(imgLine, ocrRes, m_OCRMng.GetEngTess(), tesseract::RIL_WORD);
				//}

				//std::vector<_stOCRResult> ocrResChi;
				//if (m_extractionSetting.isChi) {
				//	if (ocrRes.size() > 0) {
				//		for (int k = 0; k < ocrRes.size(); k++) {

				//			if (ocrRes[k].fConfidence < 0.70f) {
				//				cv::Mat imgword = imgLine(ocrRes[k].rect);

				//				std::vector<_stOCRResult> ocrTmp;
				//				m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);


				//				// Select Result between chi and eng //
				//				float averConf = 0;
				//				for (int m = 0; m < ocrTmp.size(); m++) {
				//					averConf += ocrTmp[m].fConfidence;
				//				}
				//				averConf /= (float)ocrTmp.size();

				//				if (ocrRes[k].fConfidence < averConf*0.9f) {
				//					ocrRes[k].type = 100;
				//				}

				//				//===========================//
				//				if (ocrRes[k].type == 100) {		// Substitute chi result to eng result //
				//					for (int m = 0; m < ocrTmp.size(); m++) {
				//						ocrTmp[m].rect.x += ocrRes[k].rect.x;
				//						ocrTmp[m].rect.y += ocrRes[k].rect.y;
				//						ocrResChi.push_back(ocrTmp[m]);
				//					}
				//				}
				//				//================//
				//			}
				//		}
				//	}
				//	else {
				//		m_OCRMng.extractWithOCR(imgLine, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);
				//	}
				//}


				//for (int k = 0; k < ocrRes.size(); k++) {
				//	if (ocrRes[k].type < 100) {
				//		ocrRes[k].rect.x += para[j].rect.x;
				//		ocrRes[k].rect.y += para[j].rect.y;
				//		ocrRes[k].type = 0;
				//		pPage->AddOCRResult(ocrRes[k]);
				//	}
				//}

				//for (int k = 0; k < ocrResChi.size(); k++) {
				//	ocrResChi[k].rect.x += para[j].rect.x;
				//	ocrResChi[k].rect.y += para[j].rect.y;
				//	ocrResChi[k].type = 1;
				//	pPage->AddOCRResult(ocrResChi[k]);
				//}
			}
		}
	}
}

void CMNView::DoExtractBoundaryForSelected(_stExtractionSetting _info)
{
	m_addImgCnt = 0;
	m_loadedImgCnt = 1;
	m_extractionSetting = _info;
	if (m_pSelectPageForCNS) {
		if (m_pSelectPageForCNS->GetVecParagraph().size() == 0) {

			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
			if (srcImg.ptr()) {

				//	cv::GaussianBlur(srcImg, srcImg, cv::Size(7, 7), 0);
				cv::threshold(srcImg, srcImg, 200, 255, cv::THRESH_OTSU);
				//	cv::threshold(srcImg, b2, 0, 255, CV_THRESH_BINARY + cv::THRESH_OTSU);
				cv::bitwise_not(srcImg, srcImg);
				int fsize = (int)m_extractionSetting.engSize;

				// Detect text block //
				//_ALIGHN_TYPE align;
				std::vector<_extractBox> vecBox;
				//if (m_extractionSetting.nAlign == 0) {
				//	m_Extractor.Extraction(srcImg, fsize * 2, -1, vecBox);
				//	align = _HORIZON_ALIGN;
				//}
				//else {
				//	m_Extractor.Extraction(srcImg, -1, fsize * 2, vecBox);
				//	align = _VERTICAL_ALIGN;
				//}
				if (m_extractionSetting.isEng) {
					ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __ENG);		// Start with English //
				}
				else {
					if (m_extractionSetting.isChi) {
						ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __CHI);
					}
					else if (m_extractionSetting.isKor) {
						ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __KOR);
					}
				}

				for (size_t j = 0; j < vecBox.size(); j++) {
					// Calculate Deskew //
					//m_Extractor.verifyImgSize(vecBox[j].textbox, srcImg.cols, srcImg.rows);
					cv::Mat para = srcImg(vecBox[j].textbox);
					//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);

					float deskew = m_Extractor.DeSkewImg(para);
					m_pSelectPageForCNS->AddParagraph(vecBox[j].textbox, m_extractionSetting.IsVerti, deskew);
				}
				vecBox.clear();
				srcImg.release();
			}
		}
	}

	m_addImgCnt = 1;
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
//	pClass->DoExtractBoundaryForSelected();
	pClass->DoExtractBoundary();
	return 0L;
}

static UINT ThreadDoOCR(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoOCR();
	return 0L;
}


void CMNView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	if (m_selOCRId >= 0) {
		CMenu menu;
		menu.LoadMenuW(IDR_MENU_OCR);
		CMenu* pMenu = menu.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	
}


void CMNView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	SelectObject3D(point.x, point.y, 2, 2, 0);	

	COGLWnd::OnRButtonDown(nFlags, point);
}
