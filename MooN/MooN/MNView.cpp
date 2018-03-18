#include "stdafx.h"
#include "MNView.h"
#include "MNDataManager.h"
#include "MainFrm.h"
#include "resource.h"
#include "MooN.h"

enum TIMEREVNT { _RENDER = 100, _ADDIMG, _SEARCHIMG, _MOVECAMANI, _UPDATE_PAGE, _GEN_THUMBNAIL, _DO_SEARCH, _DO_EXTRACTION, _DO_OCR, _DO_OCR_BY_MOON, _DO_EXPORT_DB, _DO_CNS_SEGMENTS, _DO_ADDLIST_TO_DB};


#define DEFAULT_X_OFFSET -5800;
#define DEFAULT_Y_OFFSET 2500;

#define MAX_CAM_HIGHTLEVEL 5000
#define MIN_CAM_HIGHTLEVEL 2
#define MAX_CAM_FOV 20.0f

#define ANI_FRAME_CNT 10


CMNView::CMNView()
{
	memset(&m_LogFont, 0, sizeof(m_LogFont));
	m_LogFont.lfCharSet = CHINESEBIG5_CHARSET;
	m_LogFont.lfHeight = -20;
	m_LogFont.lfWidth = 0;
//	m_LogFont.lfWeight = FW_BOLD;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));


	m_addImgCnt = 0;
	m_isAnimation = true;
	m_mouseMode = 0;

	m_pSelectPageForCNS = NULL;
	m_bIsCutNSearchMode = false;
	m_bIsMultiSelectionhMode = false;
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
	//m_selParaId = -1;
	//m_selOCRId = -1;
	
	m_selOCRIdforMouseHover = - 1;
	m_selParaIdforMouseHover = -1;

	m_bIsAllOCR = false;
	m_bIsShowParagraph = true;

	m_spliteDirection = _NONE_DIR;
	m_spliteType = _SPLIT_TEXT;
	mtSetPoint3D(&m_vSplitPos, 0.0f, 0.0f, 0.0f);

	m_selImgId = -1;
	m_IsTypeMode = false;
	m_bIsAutoSegmentMode = false;
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

	m_glListIdForDrawOCRRes = glGenLists(1);



	// !! image to unique code value TEST ========================================//
	//wchar_t newword = (unsigned short)(std::stoi("0100010101000101", nullptr, 2));
	//CString str(newword);
	//int a = 0;

	//============================================================================//

	//CString str(L"㿈(HHĈȈЈࠈဈ");
	//unsigned int h = getHashCode((CStringA)str);

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
	POINT3D rectColor;
	mtSetPoint3D(&rectColor, 0.0f, 0.99f, 0.1f);
	if (m_bIsMultiSelectionhMode) {
		mtSetPoint3D(&rectColor, 0.99f, 0.5f, 0.1f);
	}
	


	if (m_bIsShowParagraph) {
		//std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		//if ((iter != m_mapSelectionInfo.end()) && (m_pSelectPageForCNS)) {
		//	m_pSelectPageForCNS->DrawSelectedParagraph(iter->second.lineid);
		//}
		if (m_pSelectPageForCNS) {
			std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
			for (int i = 0; i < (int)vecImg.size(); i++) {
				if (vecImg[i]->IsSelected()) {
					vecImg[i]->DrawSelectedParagraph(0);
					DrawOCRRes(vecImg[i]);
				}
			}
			//m_pSelectPageForCNS->DrawSelectedParagraph(0);
			//DrawOCRRes();

			glCallList(m_glListIdForDrawOCRRes);			
			DrawSplitLine(1.0f, 0.0f, 0.0f, 1.0f);
		}
	//	DrawCNSRect(rectColor.x, rectColor.y, rectColor.z, 1.0f);
	//	glCallList(m_glListIdForDrawOCRRes);
	}

	DrawCNSRect(rectColor.x, rectColor.y, rectColor.z, 1.0f);
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

void CMNView::DrawSplitLine(float r, float g, float b, float a)
{
	float fLength = 99999.0f;
	if (m_spliteDirection == _HORI_DIR) {		
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(2, 0xAAAA);

		glLineWidth(2);
		glDisable(GL_DEPTH_TEST);
		glColor4f(r, g, b, a);
		glBegin(GL_LINES);
		glVertex3f(-fLength, m_vSplitPos.y, m_vSplitPos.z);
		glVertex3f(fLength, m_vSplitPos.y, m_vSplitPos.z);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glLineWidth(1);

		glDisable(GL_LINE_STIPPLE);
	}
	else if (m_spliteDirection == _VERTI_DIR) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(2, 0xAAAA);

		glLineWidth(2);
		glDisable(GL_DEPTH_TEST);
		glColor4f(r, g, b, a);
		glBegin(GL_LINES);
		glVertex3f(m_vSplitPos.x,-fLength, m_vSplitPos.z);
		glVertex3f(m_vSplitPos.x, fLength, m_vSplitPos.z);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glLineWidth(1);

		glDisable(GL_LINE_STIPPLE);
	}
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
		

		if (m_bIsShowParagraph) {
//			vecImg[i]->DrawParagraph(m_selParaId);
			std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
			//if((iter != m_mapSelectionInfo.end())/* && (iter->second.vecTextId.size() >0)*/)
			//if (i == m_selImgId) {
				for (; iter != m_mapSelectionInfo.end(); iter++) {
					vecImg[i]->DrawParagraph(iter->second.lineid, static_cast<bool>(m_spliteType));
				}
			//}

		}
		else {
			vecImg[i]->DrawSDBItem();
		}
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
		if (m_bIsShowParagraph){
//			vecImg[i]->DrawParagraph(m_selParaId);

			std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
			if (iter != m_mapSelectionInfo.end())
				vecImg[i]->DrawParagraph(iter->second.lineid, static_cast<bool>(m_spliteType));
		}
		else {
			vecImg[i]->DrawSDBItem();
		}

	//	vecImg[i]->DrawOCRRes();
		vecImg[i]->DrawSDBItem();
	}
	glPointSize(1);
}


void CMNView::DoSplitTextBox()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if (m_pSelectPageForCNS) {

		if ((m_mapSelectionInfo.size() == 1) && (m_mapSelectionInfo.begin()->second.vecTextId.size() == 1)) {

			if ((m_selParaId >= 0) && (m_selOCRId >= 0)) {
				_stOCRResult res = m_pSelectPageForCNS->GetOCRResult(m_selParaId, m_selOCRId);

				// Convert Coordinate and find vertical positon for spliting //
				POINT3D tmpPos = m_vSplitPos;
				tmpPos.x += 8;
				tmpPos.y -= 8;
				RECT2D selRect = m_pSelectPageForCNS->ConvertVec3DtoImgateCoord(m_vSplitPos, tmpPos);

				cv::Rect r1 = res.rect, r2 = res.rect;
				if (m_spliteDirection == _VERTI_DIR) {					
					int diff = selRect.x1 - res.rect.x;
					if ((diff > 0) && (diff < res.rect.width)) {
						r1.width = diff;
						r2.x += diff;
						r2.width = res.rect.width - diff;

						m_pSelectPageForCNS->DeleteSelOCRRes(m_selParaId, m_selOCRId);
						AddNewTextBox(r1);
						AddNewTextBox(r2);
						pM->AddOutputString(L"A selected text box was divided by two.", true);

					}
				}
				else if (m_spliteDirection == _HORI_DIR) {
					int diff = selRect.y1 - res.rect.y;
					if ((diff > 0) && (diff < res.rect.height)) {
						r1.height = diff;

						r2.y += diff;
						r2.height = res.rect.height - diff;

						m_pSelectPageForCNS->DeleteSelOCRRes(m_selParaId, m_selOCRId);
						AddNewTextBox(r1);
						AddNewTextBox(r2);
						pM->AddOutputString(L"A selected text box was divided by two.", true);
					}
				}

				m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
				m_pSelectPageForCNS->SetSelMatchItem(-1);
				m_selOCRIdforMouseHover = -1;
				m_selParaIdforMouseHover = -1;
				m_selParaId = -1;
				m_selOCRId = -1;
			}
		}
		else {
			pM->AddOutputString(L"Can't splite multiple selections. Select a single box.", true);
		}
	}
}
void CMNView::DoSplitLineBox()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if (m_pSelectPageForCNS) {

		if ((m_mapSelectionInfo.size() == 1)) {

			if ((m_selParaId >= 0)) {
				cv::Rect rect  = m_pSelectPageForCNS->GetSelParaRect(m_selParaId);
				// Convert Coordinate and find vertical positon for spliting //
				POINT3D tmpPos = m_vSplitPos;
				tmpPos.x += 8;
				tmpPos.y -= 8;
				RECT2D selRect = m_pSelectPageForCNS->ConvertVec3DtoImgateCoord(m_vSplitPos, tmpPos);

				cv::Rect r1 = rect, r2 = rect;
				if (m_spliteDirection == _VERTI_DIR) {
					int diff = selRect.x1 - rect.x;
					if ((diff > 0) && (diff < rect.width)) {
						r1.width = diff;
						r2.x += diff;
						r2.width = rect.width - diff;

						m_pSelectPageForCNS->DeleteSelPara(m_selParaId);
						AddNewLineBox(r1);
						AddNewLineBox(r2);

						pM->AddOutputString(L"A selected line box was divided by two.", true);

					}
				}
				else if (m_spliteDirection == _HORI_DIR) {
					int diff = selRect.y1 - rect.y;
					if ((diff > 0) && (diff < rect.height)) {
						r1.height = diff;

						r2.y += diff;
						r2.height = rect.height - diff;

						m_pSelectPageForCNS->DeleteSelPara(m_selParaId);
						AddNewLineBox(r1);
						AddNewLineBox(r2);
						pM->AddOutputString(L"A selected line box was divided by two.", true);
					}
				}

				m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
				m_pSelectPageForCNS->SetSelMatchItem(-1);
				m_selOCRIdforMouseHover = -1;
				m_selParaIdforMouseHover = -1;
				m_selParaId = -1;
				m_selOCRId = -1;
			}
		}
		else {
			pM->AddOutputString(L"Can't splite multiple selections. Select a single box.", true);
		}
	}
}

void CMNView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	SetTypeMode(false);

	if (m_spliteDirection > _NONE_DIR) {
		// Do Split ==========================//
		if (m_spliteType == _SPLIT_TEXT) {
			DoSplitTextBox();
		}
		else if (m_spliteType == _SPLIT_LINE) {
			DoSplitLineBox();
		}
		//=====================================//
		m_spliteDirection = _NONE_DIR;
		m_spliteType = _SPLIT_TEXT;
	}

	else {
		mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
		m_CNSRectStart = m_CNSRectEnd;
		//=======================//

		if (!m_bIsMultiSelectionhMode) {
			SelectObject3D(point.x, point.y, 2, 2, 0);
		}
		m_stratPnt = point;

		if ((m_bIsCutNSearchMode) || (m_bIsMultiSelectionhMode)) {
			m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectStart);
			m_CNSRectEnd = m_CNSRectStart;
		}
		else {
			m_mouseMode = 2;
			IDragMap(point.x, point.y, 0);
		}
		SetCapture();
	}
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


void CMNView::SetThreadEnd(bool IsEnd) 
{
	m_bIsThreadEnd = IsEnd; 
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

			//m_pSelectPageForCNS = SINGLETON_DataMng::GetInstance()->GetPageByID(0);
			//if(m_pSelectPageForCNS)
			//	MoveCameraPos(m_pSelectPageForCNS->GetPos(), (DEFAULT_PAGE_SIZE + 200));
			InitCamera();

			// new thread for extract boundary //

			if (m_bIsAutoSegmentMode) {
				ProcExtractBoundary();
			}
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
	//	if (m_addImgCnt == m_loadedImgCnt) {
		if(m_bIsThreadEnd){
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
		//if (m_addImgCnt == m_loadedImgCnt) {
		if (m_bIsThreadEnd) {
			m_addImgCnt = 0;
			KillTimer(_DO_OCR);

			str.Format(_T("Text Recognition.....done"));
			pM->AddOutputString(str, true);
		}
	//	DrawOCRRes();
		MakeList_DrawOCRResText();
	}

	else if (nIDEvent == _DO_OCR_BY_MOON) {
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Text Recognition(MooN).....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		//if (m_addImgCnt == m_loadedImgCnt) {
		if (m_bIsThreadEnd) {
			m_addImgCnt = 0;
			KillTimer(_DO_OCR_BY_MOON);

			str.Format(_T("Text Recognition(MooN).....done"));
			pM->AddOutputString(str, true);
		}
		//	DrawOCRRes();
		MakeList_DrawOCRResText();
	}

	else if (nIDEvent == _DO_EXPORT_DB) {
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Exporting Database.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		//if (m_addImgCnt == m_loadedImgCnt) {
		if (m_bIsThreadEnd) {
			m_addImgCnt = 0;
			KillTimer(_DO_EXPORT_DB);

			str.Format(_T("Exporting Database.....done"));
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

	else if (nIDEvent == _DO_CNS_SEGMENTS) {
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Cut & Search All.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");

		pM->AddOutputString(str, true);

		// End of thread //
		//if (m_addImgCnt == m_loadedImgCnt) {
		if (m_bIsThreadEnd) {
			m_addImgCnt = 0;
			KillTimer(_DO_CNS_SEGMENTS);

			str.Format(_T("Cut & Search All.....done"));
			pM->AddOutputString(str, true);			

		//	pM->AddMatchResultCNS();
			SINGLETON_DataMng::GetInstance()->SetMatchingResults();
			pM->AddOutputString(SINGLETON_DataMng::GetInstance()->GetCNSResultInfo(), false);
			pM->AddMatchResult();
			MakeList_DrawOCRResText();
		}
	}

	else if (nIDEvent == _DO_ADDLIST_TO_DB) {
		float complete = (float)m_addImgCnt / (float)m_loadedImgCnt;
		CString str;
		str.Format(_T("Training Cut&Search.....%d"), int(complete * 100));
		str += _T("%");
		str += _T(" completed.");
		pM->AddOutputString(str, true);
		if (m_bIsThreadEnd) {
			m_addImgCnt = 0;
			KillTimer(_DO_ADDLIST_TO_DB);

			str.Format(_T("Training Cut&Search.....done"));
			pM->AddOutputString(str, true);
			pM->ClearAllResults();
			MakeList_DrawOCRResText();
		}
	}
	//else if (nIDEvent == _UPDATE_PAGE) {
	//	SINGLETON_DataMng::GetInstance()->UpdatePageStatus(m_cameraPri.GetEye());
	//}

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
	if ((m_bIsCutNSearchMode)) {
		if (m_stateKeyDown == false) {
			m_bIsCutNSearchMode = false;
			m_bIsMultiSelectionhMode = false;
			SendMessage(WM_SETCURSOR);
		}
	}

	else if (m_bIsMultiSelectionhMode) {

		// Call Selection Function //
		CPoint cPos;
		cPos.x = (m_stratPnt.x + point.x) / 2;
		cPos.y = (m_stratPnt.y + point.y) / 2;
		int width = m_stratPnt.x - point.x;
		int height = m_stratPnt.y - point.y;
		if (width < 0) width *= -1;
		if (height < 0) height *= -1;
		if (width < 1) width = 1;
		if (height < 1) height = 1;
		
		SelectObject3D(cPos.x, cPos.y, width, height, 0);

		mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
		m_CNSRectStart = m_CNSRectEnd;
		if (m_stateKeyDown == false) {
			m_bIsCutNSearchMode = false;
			m_bIsMultiSelectionhMode = false;
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

		if (m_cameraPri.GetLevelHeight() > DEFAULT_PAGE_SIZE + 200) {
			MoveCameraPos(m_pSelectPageForCNS->GetPos(), (DEFAULT_PAGE_SIZE + 200));
		}
		//KillTimer(_MOVECAMANI);
		//m_nAniCnt = 0;
		//m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
		//m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;
		//SetTimer(_MOVECAMANI, 20, NULL);
	}
	m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());

	for (auto i = 0; i < m_vecSelPage.size(); i++) {
		SINGLETON_DataMng::GetInstance()->GetPageByOrderID(m_vecSelPage[i])->SetSelection(false);
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

bool CMNView::IsThreadEnd()
{ 
	return m_bIsThreadEnd; 
}

void CMNView::GenerateThumbnail()
{
	m_addImgCnt = 0;	
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	m_loadedImgCnt = static_cast<int>(vecImg.size());

	for (size_t i = 0; i < vecImg.size(); i++) {
		vecImg[i]->LoadThumbImage(THUMBNAIL_SIZE);
		m_addImgCnt++;
	}

	m_bIsThreadEnd = true;
}

int CMNView::SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

	SINGLETON_DataMng::GetInstance()->UpdatePageStatus(m_cameraPri.GetEye());

	if (m_pSelectPageForCNS) {
	//	m_pSelectPageForCNS->SetSelection(false);
		m_pSelectPageForCNS = NULL;
		m_selParaId = -1;
		m_selOCRId = -1;	
		m_selImgId = -1;
		//std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		//for (; iter != m_mapSelectionInfo.end(); iter++) {
		//	iter->second.vecTextId.swap(std::vector<int>());
		//}		
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
	gluPickMatrix(x, viewport[3] - y, rect_width, rect_height, viewport);
	gluPerspective(m_cameraPri.m_Cntfovy, (float)viewport[2] / (float)viewport[3], m_cameraPri.GetNearPlane(), m_cameraPri.GetFarPlane());

	//m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	glMatrixMode(GL_MODELVIEW);

	// Draw for picking==============================//
	//if (m_IsSearchMatchItems) {
		
	//}
	//else {
	DrawImageByOrderForPicking();
	if (m_bIsShowParagraph) {
		DrawParagrphForPicking();
		DrawOCRForPicking();
	}
//	DrawMatchItemForPicking();
	
	
	//}
	//=============================================//
	hits = glRenderMode(GL_RENDER);
	int lineid = -1, textid = -1;
	int uuid = -1;


//	if ((!m_bIsMultiSelectionhMode) && (hits>1)) {
	if ((!m_bIsMultiSelectionhMode)) {
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());

		for (auto i = 0; i < m_vecSelPage.size(); i++) {
		//	SINGLETON_DataMng::GetInstance()->GetPageByOrderID(m_vecSelPage[i])->SetSelection(false);
			if (SINGLETON_DataMng::GetInstance()->GetPageByOrderID(m_vecSelPage[i])) {
				SINGLETON_DataMng::GetInstance()->GetPageByOrderID(m_vecSelPage[i])->SetSelection(false);
			}
		}
		m_vecSelPage.swap(std::vector<int>());
	}


	if (hits > 0){
		for (int i = 0; i < hits; i++) {
			int selid = selectBuff[i * 4 + 3];			

			if (selid < _PICK_PARA) {		// Page selection
				m_pSelectPageForCNS = SINGLETON_DataMng::GetInstance()->GetPageByOrderID(selid);
				m_vecSelPage.push_back(selid);
				MakeList_DrawOCRResText();
				if (m_pSelectPageForCNS) {

					m_selImgId = selid;
					m_pSelectPageForCNS->SetSelection(true);
				//	m_pSelectPageForCNS->SetIsNear(true);
				//	DrawOCRRes();					
					stParapgraphInfo lineInfo = m_pSelectPageForCNS->GetLineBoxInfo(0);
					if (lineInfo.rect.width > 0) {
						pM->SetParagraphInfo(lineInfo.deSkewAngle, L"", lineInfo.IsVerti);
					}
					m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
				}
			}
			else if ((selid >= _PICK_PARA) && (selid < _PICK_WORD)) {		// Text block selectoin
				m_selParaId = selid - _PICK_PARA;
				lineid = selid - _PICK_PARA;
				if (m_pSelectPageForCNS) {
					stParapgraphInfo lineInfo = m_pSelectPageForCNS->GetLineBoxInfo(lineid);
					if (lineInfo.rect.width > 0) {
						pM->SetParagraphInfo(lineInfo.deSkewAngle, m_pSelectPageForCNS->GetName(), lineInfo.IsVerti);
					}
				}

		//		if (m_bIsMultiSelectionhMode) {
					if (m_mapSelectionInfo.find(lineid) == m_mapSelectionInfo.end()) {
						_stLineTextSelectionInfo selinfo;
						selinfo.lineid = lineid;
						m_mapSelectionInfo[lineid] = selinfo;
					}
		//		}

			}
			else if ((selid >= _PICK_WORD) && (selid < _PICK_MATCH)){		// Word selection
				uuid = static_cast<unsigned int>(selid - _PICK_WORD);

				//lineid = selid / 10000;
				//textid = selid % 10000;
			//	selid = selid - _PICK_WORD;
			//	m_selOCRId = textid;
				if (m_pSelectPageForCNS) {

					m_pSelectPageForCNS->GetIDbyUUID(uuid, lineid, textid);
					m_selParaId = lineid;
					m_selOCRId = textid;

				//	_stOCRResult ocrRes = m_pSelectPageForCNS->GetOCRResult(m_selParaId, m_selOCRId);
					_stOCRResult ocrRes = m_pSelectPageForCNS->GetOCRResult(lineid, textid);
					pM->SetOCRResInfo(ocrRes.strCode, ocrRes.fConfidence, ocrRes.type);

						if (m_mapSelectionInfo.find(lineid) == m_mapSelectionInfo.end()) {
							_stLineTextSelectionInfo selinfo;
							selinfo.lineid = lineid;
							selinfo.vecTextId.push_back(textid);
							m_mapSelectionInfo[lineid] = selinfo;
						}
						else {
							m_mapSelectionInfo[lineid].vecTextId.push_back(textid);
						}
				}
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


	// Select Item on the list view //
	std::map<int, _stLineTextSelectionInfo>::iterator iter =  m_mapSelectionInfo.begin();
//	if ((m_selParaId >= 0) && (m_selOCRId >= 0)) {
	if(uuid>0){
//		int id = m_selParaId * 10000 + m_selOCRId;
		pM->SelectListItemById(uuid);
	}
	//if(iter != m_mapSelectionInfo.end()){
	//	if (iter->second.vecTextId.size() > 0) {
	//		int id = m_selParaId * 10000 + m_selOCRId;
	//	//	int id = iter->second.lineid * 10000 + iter->second.vecTextId[0];
	//		pM->SelectListItemById(id);
	//	}
	//}
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
		if (vecImg[i]->IsNear()) {
			vecImg[i]->DrawParagraphForPick();
		}
	}
}


void CMNView::DrawMatchItemForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		if (vecImg[i]->IsNear()) {
			vecImg[i]->DrawMatchItemForPick();
		}
	}
}

void CMNView::DrawOCRForPicking()
{
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (int i = 0; i < (int)vecImg.size(); i++) {
		if (vecImg[i]->IsNear()) {
			vecImg[i]->DrawOCRResForPick();
		}
	}
}



void CMNView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if (GetCapture()) {
		if ((m_bIsCutNSearchMode)|| (m_bIsMultiSelectionhMode)){
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
	else {
		SelectObject3DForMouseOver(point.x, point.y,2,2, 0);
		m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_vSplitPos);
		
	}
	COGLWnd::OnMouseMove(nFlags, point);
}

void CMNView::EnableCutSearchMode(bool IsEnable, bool bKey)
{
	m_bIsCutNSearchMode = IsEnable;
	m_stateKeyDown = bKey;
}

void CMNView::EnableMultiSelectionhMode(bool IsEnable, bool bKey)
{
	m_bIsMultiSelectionhMode = IsEnable;
	m_stateKeyDown = bKey;
}

BOOL CMNView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if ((m_bIsCutNSearchMode) || (m_bIsMultiSelectionhMode)){
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
	if (m_extractionSetting.IsVerti) {
		m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_BLOCK);
		m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_AUTO_OSD);
		m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_AUTO_OSD);
		m_OCRMng.SetOCRDetectMode(__JAP, tesseract::PSM_AUTO_OSD);
	}
	else {
		m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_BLOCK);
		m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_BLOCK);
		m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_BLOCK);
		m_OCRMng.SetOCRDetectMode(__JAP, tesseract::PSM_SINGLE_BLOCK);
	}

	if (m_bIsAllOCR) {
		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		m_addImgCnt = 0;
		m_loadedImgCnt = static_cast<int>(imgVec.size());

		size_t i = 0;
		for (i = 0; i < imgVec.size(); i++) {

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

	m_bIsThreadEnd = true;
}





bool CMNView::DoSearch()
{
	if (m_pCut.ptr() == NULL)
		return false;
	int search_size = (m_pCut.cols > m_pCut.rows) ? m_pCut.cols : m_pCut.rows;

	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();

	m_addImgCnt = 0;
	m_loadedImgCnt = static_cast<int>(imgVec.size());

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
						//mInfo.lineid = -1;
						//mInfo.objid = -1;
						mInfo.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();


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
				imgVec[i - 1]->SetIsSearched(false);
			}
		}
		m_addImgCnt++;
		if (m_bIsAutoFitMode) {
			imgVec[i]->SetFitCurArea();
		}
	}


	//for (i = 0; i < imgVec.size(); i++) {
	//	imgVec[i]->SetIsSearched(false);
	//}
	imgVec[i - 1]->SetIsSearched(false);

	m_cnsSearchId++;

	m_bIsThreadEnd = true;
	return true;
}


bool CMNView::DoSearchSegment()
{
	std::vector<stParapgraphInfo> vecline = m_pSelectPageForCNSAll->GetVecParagraph();
	std::vector<_stOCRResult> ocrRes;
	for (auto i = 0; i < vecline.size(); i++) {
		for (auto j = 0; j < vecline[i].vecTextBox.size(); j++) {
			ocrRes.push_back(std::move(vecline[i].vecTextBox[j]));
		}
	}
	
	m_addImgCnt = 0;
	m_loadedImgCnt = static_cast<int>(ocrRes.size());

	for (auto j = 0; j < ocrRes.size(); j++) {
		_stOCRResult ocrres = ocrRes[j];
		// Prepare Cut&Search ==========================================//
		RECT2D rect;
		rect.set(ocrres.rect.x, ocrres.rect.x + ocrres.rect.width, ocrres.rect.y, ocrres.rect.y + ocrres.rect.height);

		if ((rect.width > 16) && (rect.height > 16)) {

			// Update Font Size ==//
			m_Extractor.SetFontSize(rect.width, rect.height);

			if (m_pCut.ptr() != NULL) {
				m_pCut.release();
			}
			CString strpath = m_pSelectPageForCNSAll->GetPath();
			cv::Mat srcImg = m_pSelectPageForCNSAll->GetSrcPageGrayImg();
			if (srcImg.ptr()) {
				m_pCut = srcImg(cv::Rect(rect.x1, rect.y1, rect.width, rect.height));
			}


			m_cutInfo.fileid = getHashCode((CStringA)m_pSelectPageForCNSAll->GetPath());
			m_cutInfo.posid = (int)rect.x1 * 10000 + (int)rect.y1;
			CString strId;
			strId.Format(L"%u%u", m_cutInfo.fileid, m_cutInfo.posid);
			m_cutInfo.id = getHashCode((CStringA)strId);
			m_cutInfo.th = m_fThreshold;

			//==============================================================//


			if (m_pCut.ptr() == NULL)
				return false;
			int search_size = (m_pCut.cols > m_pCut.rows) ? m_pCut.cols : m_pCut.rows;
			std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();

			auto i = 0;
			for (i = 0; i < imgVec.size(); i++) {
				imgVec[i]->SetIsSearched(false);
			}

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
						imgVec[i - 1]->SetIsSearched(false);
					}
				}

				imgVec[i]->SetFitCurArea();
			}

			imgVec[i - 1]->SetIsSearched(false);
			m_cnsSearchId++;
		}
		m_addImgCnt++;
	}
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

void CMNView::ExtractBoundaryForPage(CMNPageObject* pPage, bool IsAuto)
{
	// exclude margin
	int nMargin = 4;
	cv::Rect mRect(nMargin, nMargin, pPage->GetSrcPageGrayImg().cols - nMargin * 2, pPage->GetSrcPageGrayImg().rows - nMargin * 2);

	//CString strpath = vecImg[i]->GetPath();
	cv::Mat srcImg = pPage->GetSrcPageGrayImg()(mRect).clone();
	if (srcImg.ptr()) {
		cv::bitwise_not(srcImg, srcImg);

		bool IsVerti = false;
		std::vector<_extractBox> vecBox;
		if (IsAuto) {
			_ALIGHN_TYPE align = m_Extractor.CheckHoughLines(srcImg);
			// Detect text block //			
			if (align == _VERTICAL_ALIGN) {
				IsVerti = true;
			}
		}
		else {
			IsVerti = m_extractionSetting.IsVerti;
		}

		ExtractLineBox(srcImg, vecBox, IsVerti, __ENG);

		for (size_t j = 0; j < vecBox.size(); j++) {
			vecBox[j].textbox.x += 4;
			vecBox[j].textbox.y += 4;
		}

		srcImg.release();
		cv::Mat srcImgforLine = pPage->GetSrcPageGrayImg().clone();
		cv::bitwise_not(srcImgforLine, srcImgforLine);
		for (size_t j = 0; j < vecBox.size(); j++) {
//		if (vecBox[j].textbox.area() > 25) {  // remove noise
			
			cv::Mat para = srcImgforLine(vecBox[j].textbox);
			//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);

			float deskew = m_Extractor.DeSkewImg(para);
			TRACE(L"Deskew Angle: %3.2f\n", deskew);

			bool IsAlphabetic = false;
			if (m_extractionSetting.isEng)	IsAlphabetic = true;
			pPage->AddParagraph(m_Extractor, para, vecBox[j].textbox, IsVerti, deskew, IsAlphabetic);
			para.release();
			
		}
		vecBox.swap(std::vector<_extractBox>());
		srcImgforLine.release();
	}

}

void CMNView::DoExtractBoundaryAuto()
{
	//cv::Mat hHisto;
	//cv::Mat vHisto;
	std::vector<CMNPageObject*> vecImg = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	m_addImgCnt = 0;
	m_loadedImgCnt = static_cast<int>(vecImg.size());
	int i;


	for (i = 0; i < (int)vecImg.size(); i++) {

		if (vecImg[i]->GetVecParagraph().size()>0){			
			vecImg[i]->SetIsSearched(true);
			if (i > 0) {
				vecImg[i - 1]->SetIsSearched(false);
			}
			m_addImgCnt++;
			continue;
		}

		ExtractBoundaryForPage(vecImg[i], true);

/*
		// exclude margin
		int nMargin = 4;
		cv::Rect mRect(nMargin, nMargin, vecImg[i]->GetSrcPageGrayImg().cols - nMargin*2, vecImg[i]->GetSrcPageGrayImg().rows - nMargin*2);

		//CString strpath = vecImg[i]->GetPath();
		cv::Mat srcImg = vecImg[i]->GetSrcPageGrayImg()(mRect).clone();
		if (srcImg.ptr()) {
			// Clear previous results (line, text segmentations)
			//vecImg[i]->ClearParagraph();		
			

		//	cv::GaussianBlur(srcImg, srcImg, cv::Size(7, 7), 0);
		//	cv::threshold(srcImg, srcImg, 128, 255, cv::THRESH_OTSU);
		////	cv::threshold(srcImg, b2, 0, 255, CV_THRESH_BINARY + cv::THRESH_OTSU);
			cv::bitwise_not(srcImg, srcImg);			
			_ALIGHN_TYPE align = m_Extractor.CheckHoughLines(srcImg);

		//	int fsize = (int)m_extractionSetting.engSize;

			// Detect text block //
			std::vector<_extractBox> vecBox;
			bool IsVerti = false;
			if (align == _VERTICAL_ALIGN) {
				IsVerti = true;
			}

			ExtractLineBox(srcImg, vecBox, IsVerti, __ENG);

			for (size_t j = 0; j < vecBox.size(); j++) {
				vecBox[j].textbox.x += 4;
				vecBox[j].textbox.y += 4;
			}

			srcImg.release();
			//if (IsVerti) {
			//	m_Extractor.Extraction(srcImg, -1, fsize * 2, vecBox);
			//}
			//else {
			//	m_Extractor.Extraction(srcImg, fsize * 2, -1, vecBox);
			//}

			//if (m_extractionSetting.isEng) {
			//	ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __ENG);		// Start with English //
			//}
			//else {
			//	if (m_extractionSetting.isChi) {
			//		ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __CHI);
			//	}
			//	else if (m_extractionSetting.isKor) {
			//		ExtractBox(srcImg, vecBox, m_extractionSetting.IsVerti, __KOR);
			//	}
			//}
			//if (m_extractionSetting.IsHori) {
			//	m_Extractor.Extraction(srcImg, fsize*2, -1, vecBox);
			//}
			//else {
			//	m_Extractor.Extraction(srcImg, -1, fsize * 2, vecBox);
			//}

			cv::Mat srcImgforLine = vecImg[i]->GetSrcPageGrayImg().clone();
			cv::bitwise_not(srcImgforLine, srcImgforLine);
			for (size_t j = 0; j < vecBox.size(); j++) {
				// Calculate Deskew //
				//m_Extractor.verifyImgSize(vecBox[j].textbox, srcImg.cols, srcImg.rows);

				if (vecBox[j].textbox.area() > 25) {  // remove noise

					cv::Mat para = srcImgforLine(vecBox[j].textbox);
					//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);

					float deskew = m_Extractor.DeSkewImg(para);
					TRACE(L"Deskew Angle: %3.2f\n", deskew);

					bool IsAlphabetic = false;
					if (m_extractionSetting.isEng)	IsAlphabetic = true;
					vecImg[i]->AddParagraph(m_Extractor, para, vecBox[j].textbox, IsVerti, deskew, IsAlphabetic);
					para.release();
				}
			}
			vecBox.swap(std::vector<_extractBox>());
			srcImgforLine.release();
		}

*/

		vecImg[i]->SetIsSearched(true);
		if (i > 0) {
			vecImg[i - 1]->SetIsSearched(false);
		}
		m_addImgCnt++;
	}
	
	vecImg[i - 1]->SetIsSearched(false);
	m_bIsThreadEnd = true;
}

void CMNView::ProcTrainingOCRResbyConfidence(float fConfi)
{
	if (m_pSelectPageForCNS) {
		std::vector<stParapgraphInfo> vecline = m_pSelectPageForCNS->GetVecParagraph();
		//std::vector<_stOCRResult> ocrRes;
		//for (auto i = 0; i < vecline.size(); i++) {
		//	for (auto j = 0; j < vecline[i].vecTextBox.size(); j++) {
		//		ocrRes.push_back(std::move(vecline[i].vecTextBox[j]));
		//	}
		//}
		for (auto i = 0; i < vecline.size(); i++) {
			for (auto j = 0; j < vecline[i].vecTextBox.size(); j++) {
						//mtSetPoint3D(&tColor, 0.0f, 1.0f, 0.0f);
				if (vecline[i].vecTextBox[j].fConfidence > m_dispConfi) {
					//	ocrRes[j].bNeedToDB = true;
					m_pSelectPageForCNS->UpdateOCRResStatus(i,j, true, vecline[i].vecTextBox[j].type);
				}				
			}
		}

		SINGLETON_DataMng::GetInstance()->DBTrainingForPage(m_pSelectPageForCNS);
	}
}

void CMNView::ProcOCRFromMooN()
{
//	DoOCRFromMooN();
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Text Recognition(MooN).....start"));
	pM->AddOutputString(str, false);

	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();

	//	InitCamera(false);
	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadDoOCRByMooN, this);

	//DoExtractBoundary();
	//DoOCR();

	//std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	//for (size_t i = 0; i < imgVec.size(); i++) {
	//	imgVec[i]->SetIsSearched(false);
	//}

	SetTimer(_DO_OCR_BY_MOON, 100, NULL);
}

void CMNView::ProcOCR(bool IsAll)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Text Recognition.....start"));
	pM->AddOutputString(str, false);

	m_bIsAllOCR = IsAll;
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

void CMNView::ProcAddListToTraining()
{
	CWinThread* pl;
	pl = AfxBeginThread(ThreadListToTraining, this);

	m_bIsThreadEnd = false;
	SetTimer(_DO_ADDLIST_TO_DB, 100, NULL);
}

void CMNView::ProcExtractBoundary()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Extraction.....start"));
	pM->AddOutputString(str, false);


	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	for (size_t i = 0; i < imgVec.size(); i++) {
		imgVec[i]->SetIsSearched(false);
	}

	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();

	CWinThread* pl;
	m_bIsThreadEnd = false;
	pl = AfxBeginThread(ThreadDoExtraction, this);
//	DoExtractBoundary();

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
		if (rect.width == 0) return;

		// Update Font Size ==//
		m_Extractor.SetFontSize(rect.width, rect.height);

		if (m_pCut.ptr() != NULL) {
			m_pCut.release();
		}
		CString strpath = m_pSelectPageForCNS->GetPath();
		cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg();;
		if (srcImg.ptr()) {
			cv::Rect r = cv::Rect(rect.x1, rect.y1, (rect.x2 - rect.x1), (rect.y2 - rect.y1));
			if (m_bIsAutoFitMode) {
				SINGLETON_DataMng::GetInstance()->FitCutImageRect(srcImg, r);
			}
			m_pCut = srcImg(r).clone();
		}
		
		m_cutInfo.fileid = getHashCode((CStringA)m_pSelectPageForCNS->GetPath());
		m_cutInfo.posid = (int)rect.x1 * 10000 + (int)rect.y1;
		CString strId;
		strId.Format(L"%u%u", m_cutInfo.fileid, m_cutInfo.posid);
		m_cutInfo.id = getHashCode((CStringA)strId);
		m_cutInfo.th = m_fThreshold;

		//srcImg.release();
		//==============================================================//
		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		// Load Full image //
		for (size_t i = 0; i < imgVec.size(); i++) {
		//	imgVec[i]->LoadFullImage();
			imgVec[i]->SetIsSearched(false);
		}

		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		m_fThreshold = pM->GetThreshold()*0.01f;
		COLORREF resColor = pM->GetMatchingColor();
		m_resColor.r = (float)GetRValue(resColor)*0.00392f;
		m_resColor.g = (float)GetGValue(resColor)*0.00392f;
		m_resColor.b = (float)GetBValue(resColor)*0.00392f;

		CWinThread* pl;
		m_bIsThreadEnd = false;
		pl = AfxBeginThread(ThreadDoSearch, this);
		SetTimer(_DO_SEARCH, 100, NULL);
	}
}

void CMNView::MovePrePage()
{
	if (m_IsTypeMode)	return;

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
	if (m_IsTypeMode)	return;

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
	if (m_IsTypeMode)	return;

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
	if (m_IsTypeMode)	return;

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
//	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if((m_pSelectPageForCNS)){
		std::map<int, _stLineTextSelectionInfo>::iterator iter =  m_mapSelectionInfo.begin();

		std::vector<int> vecSort;
		for (; iter != m_mapSelectionInfo.end(); iter++) {
			vecSort.push_back(iter->second.lineid);
		//	m_pSelectPageForCNS->DeleteSelPara(iter->second.lineid);
			iter->second.vecTextId.swap(std::vector<int>());
		}

		// sort selected id //
		for (auto i = 0; i < vecSort.size() - 1; i++) {
			for (auto j = i + 1; j < vecSort.size(); j++) {
				if (vecSort[i] < vecSort[j]) {
					int tmp = vecSort[i];
					vecSort[i] = vecSort[j];
					vecSort[j] = tmp;
				}
			}
		}

		for (auto i = 0; i < vecSort.size(); i++) {
			m_pSelectPageForCNS->DeleteSelPara(vecSort[i]);
		}


		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		m_selOCRIdforMouseHover = -1;
		m_selParaIdforMouseHover = -1;
	}
}

bool CMNView::DeleteSelOCRRes()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	bool res = false;
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		for (; iter != m_mapSelectionInfo.end(); iter++) {

			// Sort select ID decending //
			std::vector<int> vecSort = iter->second.vecTextId;
			for (auto i = 0; i < vecSort.size()-1; i++) {	
				for (auto j = i+1; j < vecSort.size(); j++) {
					if (vecSort[i] < vecSort[j]) {
						int tmp = vecSort[i];
						vecSort[i] = vecSort[j];
						vecSort[j] = tmp;
					}
				}
			}
			//====================//

			for (auto i = 0; i < vecSort.size(); i++) {
				unsigned int uuid = m_pSelectPageForCNS->DeleteSelOCRRes(iter->second.lineid, vecSort[i]);
				// Update list control //
				//int id = iter->second.lineid * 10000 + iter->second.vecTextId[i];
				pM->DeleteMatchList(uuid);
			}

			iter->second.vecTextId.swap(std::vector<int>());
		}
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		m_pSelectPageForCNS->SetSelMatchItem(-1);
		m_selOCRIdforMouseHover = -1;
		m_selParaIdforMouseHover = -1;
	//	m_selOCRId = -1;
	}

	return res;
}

void CMNView::ConfirmOCRRes()
{
	if ((m_pSelectPageForCNS)) {  // Last selected one !!
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		if ((m_selParaId >= 0) && (m_selOCRId >= 0)) {
			m_pSelectPageForCNS->ConfirmOCRRes(m_selParaId, m_selOCRId);

			SINGLETON_DataMng::GetInstance()->DBTrainingStrCode(m_pSelectPageForCNS, m_selParaId, m_selOCRId);
			MakeList_DrawOCRResText();
		}
		//for (; iter != m_mapSelectionInfo.end(); iter++) {
		//	for (auto i = 0; i < iter->second.vecTextId.size(); i++) {
		//	//	m_pSelectPageForCNS->ConfirmOCRRes(m_selParaId, m_selOCRId);
		//		m_pSelectPageForCNS->ConfirmOCRRes(iter->second.lineid, iter->second.vecTextId[0]);
		//	}
		////	m_selOCRId = -1;
		//	iter->second.vecTextId.swap(std::vector<int>());
		//}
		//m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		//SINGLETON_DataMng::GetInstance()->DBTrainingForPage(m_pSelectPageForCNS);
	}
}

void CMNView::UpdateOCRCode(CString _strCode, bool IsDBUpdate)
{
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		//for (; iter != m_mapSelectionInfo.end(); iter++) {
		//	for (auto i = 0; i < iter->second.vecTextId.size(); i++) {
		//		m_pSelectPageForCNS->UpdateOCRCode(_strCode, 1.0f, iter->second.lineid, iter->second.vecTextId[i]);
		//	}
		//	iter->second.vecTextId.swap(std::vector<int>());
		//}
		//m_selOCRId = -1;
		
		if (m_pSelectPageForCNS->UpdateOCRCode(_strCode, 1.0f, m_selParaId, m_selOCRId)) {
			//		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
			if (IsDBUpdate) {
				SINGLETON_DataMng::GetInstance()->DBUpdateStrCode(m_pSelectPageForCNS, m_selParaId, m_selOCRId);
			}
			else {
				SINGLETON_DataMng::GetInstance()->DBTrainingStrCode(m_pSelectPageForCNS, m_selParaId, m_selOCRId);
			}
			MakeList_DrawOCRResText();
		}
	}
}

void CMNView::DeleteAllLines()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->ClearParagraph();
//		m_selParaId = -1;
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		for (; iter != m_mapSelectionInfo.end(); iter++) {
			iter->second.vecTextId.swap(std::vector<int>()); 
		}
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());

		SINGLETON_DataMng::GetInstance()->ResetResult();
		pM->ClearAllResults();

	}
}

void CMNView::DeleteAllOCRRes()
{
	if ((m_pSelectPageForCNS)) {
		m_pSelectPageForCNS->ClearOCRResult();
	//	m_selOCRId = -1;
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		for (; iter != m_mapSelectionInfo.end(); iter++) {
			iter->second.vecTextId.swap(std::vector<int>());
		}
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		m_selOCRIdforMouseHover = -1;
		m_selParaIdforMouseHover = -1;

		MakeList_DrawOCRResText();
	}
}

void CMNView::AddNewTextBox(cv::Rect rect)
{
	USES_CONVERSION;
//	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {

			_stOCRResult ocrres;
			ocrres.fConfidence = 0.0f;
			ocrres.type = 0;
			//ocrres.rect.x = rect.x1;
			//ocrres.rect.y = rect.y1;
			ocrres.rect = rect;
			//ocrres.rect.width = rect.width;
			//ocrres.rect.height = rect.height;
			ocrres.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
			memset(&ocrres.strCode, 0x00, sizeof(wchar_t)*_MAX_WORD_SIZE);
			wsprintf(ocrres.strCode, L"");


			// Get para id //

			m_pSelectPageForCNS->AddOCRResult(-1, ocrres);
		}
	}
}


void CMNView::AddEditedLineBox(cv::Rect rect)
{
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
	//	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {
			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
			if (srcImg.ptr()) {

				cv::Mat para = srcImg(rect);
				cv::bitwise_not(para, para);
				std::vector<_extractBox> vecLines;
				_extractBox addlinebox;
				addlinebox.init();
				addlinebox.textbox = rect;
				vecLines.push_back(addlinebox);
					// Calculate Deskew //
		//		cv::Mat imgLine = para(vecLines[0].textbox);
				float deskew = m_Extractor.DeSkewImg(para);

				//vecLines[0].textbox.x += rect.x;
				//vecLines[0].textbox.y += rect.y;

				bool IsAlphabetic = false;
				if (m_extractionSetting.isEng)	IsAlphabetic = true;
				m_pSelectPageForCNS->AddParagraph(m_Extractor, para, vecLines[0].textbox, m_extractionSetting.IsVerti, deskew, IsAlphabetic);

				para.release();
			}
		}
	}
}

void CMNView::AddNewLineBox(cv::Rect rect)
{
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {

			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
			if (srcImg.ptr()) {
				
				cv::Mat para = srcImg(rect);
				cv::bitwise_not(para, para);
				std::vector<_extractBox> vecLines;
				ExtractLineBox(para, vecLines, m_extractionSetting.IsVerti, __ENG);

				for (auto j = 0; j < vecLines.size(); j++) {
					// Calculate Deskew //
					cv::Mat imgLine = para(vecLines[j].textbox);
					float deskew = m_Extractor.DeSkewImg(imgLine);

					vecLines[j].textbox.x += rect.x;
					vecLines[j].textbox.y += rect.y;

					bool IsAlphabetic = false;
					if (m_extractionSetting.isEng)	IsAlphabetic = true;
					m_pSelectPageForCNS->AddParagraph(m_Extractor, imgLine, vecLines[j].textbox, m_extractionSetting.IsVerti, deskew, IsAlphabetic);
				}			
				para.release();
			}
		}
	}
}

void CMNView::AddLineBox(cv::Rect rect)
{
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
	//	RECT2D rect = GetSelectedAreaForCNS();
	if ((m_pSelectPageForCNS)) {
		if ((rect.width > 0) && (rect.height > 0)) {

			cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
			if (srcImg.ptr()) {
				cv::bitwise_not(srcImg, srcImg);
				//cv::Rect r;
				//r.x = rect.x1;
				//r.y = rect.y1;
				//r.width = rect.width;
				//r.height = rect.height;

				cv::Mat para = srcImg(rect);
				float deskew = m_Extractor.DeSkewImg(para);

				bool IsAlphabetic = false;
				if (m_extractionSetting.isEng)	IsAlphabetic = true;
				m_pSelectPageForCNS->AddParagraph(m_Extractor, para, rect, m_extractionSetting.IsVerti, deskew, IsAlphabetic);

				para.release();
			}
		//	srcImg.release();
		}
	}
}

void CMNView::EncodePage()
{
	if ((m_pSelectPageForCNS)) {
		CString strPath = m_pSelectPageForCNS->GetPInfoPath(L".txt");
		CFile cfile;
		if (!cfile.Open(strPath, CFile::modeWrite | CFile::modeCreate))
		{
			return;
		}

		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
		if (m_extractionSetting.IsVerti) {
			m_pSelectPageForCNS->EncodeTexBoxVerti(cfile);
		}
		else {
			m_pSelectPageForCNS->EncodeTexBoxHori(cfile);
		}

		cfile.Close();
		::ShellExecute(NULL, L"open", L"notepad", strPath, NULL, SW_SHOW);
	}
}

void CMNView::DeskewParagraph(float fAngle)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		if (iter != m_mapSelectionInfo.end()) {
			m_pSelectPageForCNS->DeSkewImg(iter->second.lineid, fAngle);
		}
	//	m_pSelectPageForCNS->DeSkewImg(m_selParaId, fAngle);
	//	m_pSelectPageForCNS->UpdateTexture();
	}
}

void CMNView::UndoDeskewParagraph()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		if (iter != m_mapSelectionInfo.end()) {
			m_pSelectPageForCNS->UnDoDeSkewImg(iter->second.lineid);
		}
		//m_pSelectPageForCNS->UnDoDeSkewImg(m_selParaId);
	}
}

void CMNView::ExtractLineBox(cv::Mat& img, std::vector<_extractBox>& vecBox, bool IsVerti, _LANGUAGE_TYPE lang)
{
	if (img.ptr()) {
	//	cv::threshold(para, para, 125, 255, cv::THRESH_OTSU);
	//	cv::bitwise_not(para, para);
		int x_ext = 0, y_ext = 0;
		int fsize = 0;
		switch (lang) {
		case __ENG:
			fsize = (int)m_extractionSetting.engSize;
			break;
		case __CHI:
			fsize = (int)m_extractionSetting.chiSize;
			break;
		case __KOR:
			fsize = (int)m_extractionSetting.korSize;
			break;
		}

		if (IsVerti) {
		//	m_Extractor.Extraction(img, -2, fsize * 2, vecBox);
			m_Extractor.Extraction(img, -2, img.cols*5, vecBox);
			for (auto i = 0; i < vecBox.size(); i++) {
				if (vecBox[i].textbox.width > 4) {
					vecBox[i].textbox.x += 2;
					vecBox[i].textbox.width -= 4;
				}
			}
		}
		else {
		//	m_Extractor.Extraction(img, fsize * 2, -1, vecBox);
			m_Extractor.Extraction(img, img.rows*5, -1, vecBox);
		}
		
	}
}



void CMNView::ReExtractParagraph()
{
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
	if ((m_pSelectPageForCNS)) {

		int selline = 0;
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		if (iter != m_mapSelectionInfo.end()) {
			selline = iter->second.lineid;
		}
		else {
			return;
		}
		// Reset Selections //
		for (; iter != m_mapSelectionInfo.end(); iter++) {
			iter->second.vecTextId.swap(std::vector<int>());
		}
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		//=====================//




		//	cv::Rect r = m_pSelectPageForCNS->GetSelParaRect(m_selParaId);
			cv::Rect r = m_pSelectPageForCNS->GetSelParaRect(selline);
			if ((r.width > 0) && (r.height > 0)) {

				// Delete Previous Para rect //
				m_pSelectPageForCNS->DeleteSelPara(selline);
	//			m_selParaId = -1;

				cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg();
				if (srcImg.ptr()) {
					cv::Mat para = srcImg(r).clone();
					cv::threshold(para, para, 128, 255, cv::THRESH_OTSU);
					cv::bitwise_not(para, para);
					//	cv::imshow("Re-ext", para);
						//int fsize = (int)m_extractionSetting.engSize;
						//// Detect text block //
					std::vector<_extractBox> vecLines;
					ExtractLineBox(para, vecLines, m_extractionSetting.IsVerti, __ENG);
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

						bool IsAlphabetic = false;
						if (m_extractionSetting.isEng)	IsAlphabetic = true;
						m_pSelectPageForCNS->AddParagraph(m_Extractor, para, vecLines[j].textbox, m_extractionSetting.IsVerti, deskew, IsAlphabetic);
					}
					para.release();
				}
			}
		}
	
}

void CMNView::RemoveSelectedPage()
{
	if (m_pSelectPageForCNS) {
		RemoveImageData(m_pSelectPageForCNS->GetPCode(), m_pSelectPageForCNS->GetCode());
		m_pSelectPageForCNS = NULL;
	}
	MakeList_DrawOCRResText();
}

void CMNView::RemoveImageGroup(unsigned int pCode)
{
	m_pSelectPageForCNS = NULL;
	SINGLETON_DataMng::GetInstance()->PopImageDataGroup(pCode);
	MakeList_DrawOCRResText();
}
void CMNView::RemoveImageData(unsigned int pCode, unsigned int code)
{
//	m_pSelectPageForCNS = NULL;
	SINGLETON_DataMng::GetInstance()->PopImageData(pCode, code);
	MakeList_DrawOCRResText();	
}


void CMNView::MakeList_DrawOCRResText()
{
	glNewList(m_glListIdForDrawOCRRes, GL_COMPILE);
	if (m_pSelectPageForCNS) {
		if (m_pSelectPageForCNS->IsNear()) {

			glPushMatrix();
			glTranslatef(m_pSelectPageForCNS->GetPos().x, m_pSelectPageForCNS->GetPos().y, m_pSelectPageForCNS->GetPos().z);

			glColor4f(0.0f, 0.99f, 0.1f, 0.9f);
			glPushMatrix();
			glScalef(m_pSelectPageForCNS->GetfXScale(), m_pSelectPageForCNS->GetfYScale(), 1.0f);
			glTranslatef(-m_pSelectPageForCNS->GetImgWidth()*0.5f, -m_pSelectPageForCNS->GetImgHeight()*0.5f, 0.0f);

			POINT3D tPos;// , tColor;
			RECT2D rect;
			//std::vector<_stOCRResult> ocrRes = m_pSelectPageForCNS->GetVecOCRResult();
			std::vector<stParapgraphInfo> vecline = m_pSelectPageForCNS->GetVecParagraph();

			for (auto i = 0; i < vecline.size(); i++) {
				for (int j = 0; j < vecline[i].vecTextBox.size(); j++) {
					rect.set(vecline[i].vecTextBox[j].rect.x, vecline[i].vecTextBox[j].rect.x + vecline[i].vecTextBox[j].rect.width,
						vecline[i].vecTextBox[j].rect.y, vecline[i].vecTextBox[j].rect.y + vecline[i].vecTextBox[j].rect.height);
					// Draw Text //
					glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
					if (vecline[i].vecTextBox[j].fConfidence < m_dispConfi) {
						glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
					}
					if (m_extractionSetting.IsVerti) {
						mtSetPoint3D(&tPos, (rect.x2 + 10), m_pSelectPageForCNS->GetImgHeight() - (rect.y1 + rect.y2)*0.5f, 1.0f);
					}
					else {
						mtSetPoint3D(&tPos, (rect.x1 + rect.x2)*0.5f, m_pSelectPageForCNS->GetImgHeight() - rect.y1 + 5, 1.0f);
					}
					gl_DrawText(tPos, vecline[i].vecTextBox[j].strCode, m_LogFont, 1, m_pBmpInfo, m_CDCPtr);
				}
			}
			glPopMatrix();
			glPopMatrix();
		}
	}
	glEndList();
}


void CMNView::DrawOCRRes(CMNPageObject* pPage)
{
//	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
//	glLineWidth(2);
//	for (size_t i = 0; i < imgVec.size(); i++) {
//	glNewList(m_glListIdForDrawOCRRes, GL_COMPILE);

	//if (m_bIsThreadEnd == false)
	//	return;


	if (pPage) {
	//	if (pPage->IsNear()) {

			glPushMatrix();
			glTranslatef(pPage->GetPos().x, pPage->GetPos().y, pPage->GetPos().z);

			glColor4f(0.0f, 0.99f, 0.1f, 0.9f);
			glPushMatrix();
			glScalef(pPage->GetfXScale(), pPage->GetfYScale(), 1.0f);
			glTranslatef(-pPage->GetImgWidth()*0.5f, -pPage->GetImgHeight()*0.5f, 0.0f);

			POINT3D tPos, tColor;
			RECT2D rect;
			std::vector<stParapgraphInfo> vecline = pPage->GetVecParagraph();

			for (auto i = 0; i < vecline.size(); i++) {
				for (int j = 0; j < vecline[i].vecTextBox.size(); j++) {

					rect.set(vecline[i].vecTextBox[j].rect.x, vecline[i].vecTextBox[j].rect.x + vecline[i].vecTextBox[j].rect.width, 
						vecline[i].vecTextBox[j].rect.y, vecline[i].vecTextBox[j].rect.y + vecline[i].vecTextBox[j].rect.height);

					glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
					glBegin(GL_LINE_STRIP);
					glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
					glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y2, 0.0f);
					glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y2, 0.0f);
					glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y1, 0.0f);
					glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
					glEnd();

					if (vecline[i].vecTextBox[j].fConfidence > m_dispConfi) {

						if ((vecline[i].vecTextBox[j].type < 3)) {
							tColor = SINGLETON_DataMng::GetInstance()->GetColor(vecline[i].vecTextBox[j].fConfidence);
							glColor4f(tColor.x, tColor.y, tColor.z, 0.3f);
							glBegin(GL_QUADS);
							glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
							glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y2, 0.0f);
							glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y2, 0.0f);
							glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y1, 0.0f);
							//glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
							glEnd();
						}
						else {
							glColor4f(0.0f, 0.3f, 1.0f, 0.3f);
							glBegin(GL_QUADS);
							glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
							glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y2, 0.0f);
							glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y2, 0.0f);
							glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y1, 0.0f);
							glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
							glEnd();
						}

					}

					glLineWidth(4);
					if ((j == m_selOCRIdforMouseHover)&&(i== m_selParaIdforMouseHover)) {

						glColor4f(0.0f, 0.0f, 1.0f, 0.99f);
						if (m_extractionSetting.IsVerti) {
							mtSetPoint3D(&tPos, (rect.x2 + 10), pPage->GetImgHeight() - (rect.y1 + rect.y2)*0.5f, 1.0f);
						}
						else {
							mtSetPoint3D(&tPos, (rect.x1 + rect.x2)*0.5f, pPage->GetImgHeight() - rect.y1 + 5, 1.0f);
						}
						gl_DrawText(tPos, vecline[i].vecTextBox[j].strCode, m_LogFont, 1, m_pBmpInfo, m_CDCPtr);

						glColor4f(0.2f, 0.3f, 0.99f, 0.5f);
						glBegin(GL_LINE_STRIP);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y2, 0.0f);
						glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y2, 0.0f);
						glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y1, 0.0f);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
						glEnd();
					}

				//	if ((j == m_selOCRId) && (i == m_selParaId)) {
					if (IsTextBoxSelected(i, j)){
						glColor4f(0.0f, 1.0f, 0.0f, 0.9f);
						if((m_spliteDirection != _NONE_DIR) && (m_spliteType == _SPLIT_TEXT))
							glColor4f(1.0f, 0.0f, 0.0f, 0.9f);
						glBegin(GL_LINE_STRIP);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y2, 0.0f);
						glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y2, 0.0f);
						glVertex3f(rect.x2, pPage->GetImgHeight() - rect.y1, 0.0f);
						glVertex3f(rect.x1, pPage->GetImgHeight() - rect.y1, 0.0f);
						glEnd();
					}
					glLineWidth(1);
				}
			}
			glPopMatrix();
			glPopMatrix();
	//	}
	}
//	glLineWidth(1);
//	glEndList();
}

bool CMNView::IsTextBoxSelected(int lineid, int selid) 
{
	std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
	for (; iter != m_mapSelectionInfo.end(); iter++) {
		for (auto i = 0; i < iter->second.vecTextId.size(); i++) {
			if ((lineid == iter->second.lineid) && (selid == iter->second.vecTextId[i])) {
				return true;
			}
		}
	}
	return false;
}


void CMNView::DoOCinResults(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage, std::vector<_stOCRResult>& ocrRes, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale, int langType)
{
	std::vector<_stOCRResult> ocrAdd;
	for (int k = 0; k < ocrRes.size(); k++) {
		if ((ocrRes[k].fConfidence < 0.70f) && (ocrRes[k].type < 100)) {
			cv::Mat imgword = img(ocrRes[k].rect).clone();
			std::vector<_stOCRResult> ocrTmp;
			//float averConf = m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, langType);
			float averConf = m_OCRMng.extractWithOCR(imgword, ocrTmp, tess, level, fScale, langType);
		
			if ((averConf > ocrRes[k].fConfidence*1.2f) && (averConf > 0.6f)) {
		//	if ((averConf > ocrRes[k].fConfidence*1.2f)){// && (averConf > 0.5f)) {
				ocrRes[k].type = 100; 
				for (int m = 0; m < ocrTmp.size(); m++) {
					ocrTmp[m].rect.x += ocrRes[k].rect.x;
					ocrTmp[m].rect.y += ocrRes[k].rect.y;
					ocrAdd.push_back(ocrTmp[m]);
				}
				ocrTmp.swap(std::vector<_stOCRResult>());
			}
			imgword.release();
		}
	}

	for (int i = 0; i < ocrAdd.size(); i++) {
		ocrRes.push_back(ocrAdd[i]);
	}
	ocrAdd.swap(std::vector<_stOCRResult>());
}


bool CMNView::MeargingtTextBox(std::vector<_stOCRResult>& vecBox, int& depth)
{
	std::vector<_stOCRResult> tmp = vecBox;
//	vecBox = std::vector<_stOCRResult>();
	vecBox.swap(std::vector<_stOCRResult>());

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	for (int i = 0; i < tmp.size(); i++) {
		if ((tmp[i].type==-1) || (tmp[i].type==100)) continue;		// Merged box

		for (int j = i + 1; j < tmp.size(); j++) {
			cv::Rect andRect_overlap = (tmp[i].rect & tmp[j].rect);

			float fOverlap_i =  (float)andRect_overlap.area() / (float)tmp[i].rect.area();
			float fOverlap_j =  (float)andRect_overlap.area() / (float)tmp[j].rect.area();
			cv::Rect mergeBox = (tmp[i].rect | tmp[j].rect);
			if (fOverlap_i > 0.1f) {		// Skip i !				
				tmp[j].rect = mergeBox;
				tmp[j].fConfidence = 0.1f;
				tmp[i].type = -1;
				IsMerged = true;
			}else if(fOverlap_j > 0.1f){
				tmp[i].rect = mergeBox;
				tmp[i].fConfidence = 0.1f;
				tmp[j].type = -1;
				IsMerged = true;
			}
		}
	}

	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i].type != -1) {
			vecBox.push_back(tmp[i]);
		}
	}
	tmp.swap(std::vector<_stOCRResult>());

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtTextBox(vecBox, depth);
	}
	return true;
}

bool CMNView::IsSymbol(wchar_t ch) {
	return ((ch == '.') || (ch == ',') || (ch == ';') || (ch == ':') ||
			(ch == '(') || (ch == ')') || (ch == '[') || (ch == ']') ||
			(ch == '{') || (ch == '}') || (ch == '!') || (ch == '?'));
}


void CMNView::TrimTextBox(std::vector<_stOCRResult>& ocrRes, cv::Rect _rect)
{
	if (m_pSelectPageForCNS) {
		std::vector<_stOCRResult> tmp;// = ocrRes;

		int nWidth = 0, nHeight = 0;
		bool IsMerged = false;
		_extractBox resBox;

		int averHeight = 0;
		int addCnt = 0;

		for (int k = 0; k < ocrRes.size(); k++) {
			if ((ocrRes[k].type == -1) || (ocrRes[k].type == 100)) continue;		// Merged box

			int clen = static_cast<int>(wcslen(ocrRes[k].strCode)) - 1;
			if (clen > 0) {
				if (IsSymbol(ocrRes[k].strCode[0]) || IsSymbol(ocrRes[k].strCode[clen])) {
					cv::Rect r = ocrRes[k].rect;
					r.x += _rect.x;
					r.y += _rect.y;
					cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
			//		cv::imshow("test", imgword);

			//		m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
					m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_WORD);
					std::vector<_stOCRResult> ocrTmp;
			//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL, 1.0f, __ENG);
					m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(__ENG), tesseract::RIL_SYMBOL, 1.0f, __ENG);

					// Update OCR Res //
					if (ocrTmp.size() >= 0) {
						//	cv::Rect mergeBox(0, 0, 0, 0);
						//ocrRes[i].rect = cv::Rect(0, 0, 0, 0);
						//ocrRes[i].type = 100;
						memset(ocrRes[k].strCode, 0x00, sizeof(wchar_t)*(_MAX_WORD_SIZE));
						int cid = 0;
						int minx = 9999, maxx = 0, miny = 9999, maxy = 0;
						for (int j = 0; j < ocrTmp.size(); j++) {
							ocrTmp[j].rect.x += ocrRes[k].rect.x;
							ocrTmp[j].rect.y += ocrRes[k].rect.y;
							if (IsSymbol(ocrTmp[j].strCode[0])) {
								tmp.push_back(ocrTmp[j]);		// Save Symbol
							}
							else {
								if (minx > ocrTmp[j].rect.x)	minx = ocrTmp[j].rect.x;
								if (miny > ocrTmp[j].rect.y)	miny = ocrTmp[j].rect.y;

								if (maxx < (ocrTmp[j].rect.x+ ocrTmp[j].rect.width))	maxx = ocrTmp[j].rect.x + ocrTmp[j].rect.width;
								if (maxy < (ocrTmp[j].rect.y + ocrTmp[j].rect.height))	maxy = ocrTmp[j].rect.y + ocrTmp[j].rect.height;

								ocrRes[k].strCode[cid] = ocrTmp[j].strCode[0];
								cid++;

								averHeight += ocrTmp[j].rect.height;
								addCnt++;
							}
						}
						// Update texbox rect size;
						if (minx < 9999) {
							ocrRes[k].rect = cv::Rect(minx, miny, (maxx - minx), (maxy - miny));
						}
					}
				}
			}
		}

		if (addCnt > 0) {
			averHeight /= addCnt;
		}

		for (int i = 0; i < tmp.size(); i++) {
			if ((tmp[i].strCode[0] == '.') || (tmp[i].strCode[0] == ',')) {
				if (averHeight > 0) {
					tmp[i].rect.y -= (averHeight - tmp[i].rect.height);
					tmp[i].rect.height = averHeight;					
				}
			}

			if (tmp[i].rect.y < 0) tmp[i].rect.y = 0;
			if (tmp[i].rect.x < 0) tmp[i].rect.x = 0;
			ocrRes.push_back(tmp[i]);
		}
		tmp.swap(std::vector<_stOCRResult>());
	}

}


void CMNView::DoOCCorrection(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage, std::vector<_stOCRResult>& ocrRes, int& addCnt)
{
	m_dbTreshold = 0.9f;
	TRACE(L"OCR correctrion\n");
	// check duplication of ocr res rect //
	// Flush OCR Results // !!!!
	// 1. Merge text box if they are duplicated //
	int depth = 0;
	MeargingtTextBox(ocrRes, depth);
	TrimTextBox(ocrRes, rect);

	std::vector<_stOCRResult> ocrAdd;
	cv::Mat imgword;
	for (int k = 0; k < ocrRes.size(); k++) {
		if ((ocrRes[k].fConfidence < m_dbTreshold) && (ocrRes[k].type < 100)) {
		//	cv::Mat imgword = img(ocrRes[k].rect).clone();	
			imgword = img(ocrRes[k].rect);
			_stOCRResult dbRes = ocrRes[k];
			dbRes.fConfidence = 0.0f;
			SINGLETON_DataMng::GetInstance()->MatchingFromDB(imgword, dbRes);
			
			if (((dbRes.fConfidence+0.1) > ocrRes[k].fConfidence) && (dbRes.fConfidence > 0.6f)){
				ocrRes[k].type = 100;
				dbRes.type = __CNS;
				ocrAdd.push_back(dbRes);
			}
		//	imgword.release();
		}
		addCnt++;
	}

	for (int i = 0; i < ocrAdd.size(); i++) {
		ocrRes.push_back(ocrAdd[i]);
	}
	ocrAdd.swap(std::vector<_stOCRResult>());
}



void CMNView::DoOCRFromMooN()
{
	if (m_pSelectPageForCNS) {
	//	std::vector<_stOCRResult> ocrRes = m_pSelectPageForCNS->GetVecOCRResult();
		std::vector<stParapgraphInfo> vecline = m_pSelectPageForCNS->GetVecParagraph();

		m_addImgCnt = 0;
		m_loadedImgCnt = 0;
		for (auto i = 0; i < vecline.size(); i++) {
			m_loadedImgCnt += static_cast<int>(vecline[i].vecTextBox.size());
		}


		cv::Mat gray = m_pSelectPageForCNS->GetSrcPageGrayImg();
		cv::Rect rect = cv::Rect(0, 0, gray.cols, gray.rows);
		for (auto i = 0; i < vecline.size(); i++) {
			//std::vector<_stOCRResult> ocrRes;
			//for(auto j=0; j<vecline[i].vecTextBox.size(); j++){
			//	ocrRes.push_back(std::move(vecline[i].vecTextBox[j]));
			//}
			DoOCCorrection(gray, rect, m_pSelectPageForCNS, vecline[i].vecTextBox, m_addImgCnt);
			m_pSelectPageForCNS->DeleteAllOcrResInLine(i);

			for (int k = 0; k < vecline[i].vecTextBox.size(); k++) {
				if (vecline[i].vecTextBox[k].type < 100) {
					m_pSelectPageForCNS->AddOCRResult(i, vecline[i].vecTextBox[k]);
				}
			}
		}


		//cv::Mat gray = m_pSelectPageForCNS->GetSrcPageGrayImg();
		//cv::Rect rect = cv::Rect(0, 0, gray.cols, gray.rows);
		//DoOCCorrection(gray, rect, m_pSelectPageForCNS, ocrRes);

		//m_pSelectPageForCNS->DeleteAllOcrRes();

		//for (int k = 0; k < ocrRes.size(); k++) {
		//	if (ocrRes[k].type < 100) {
		//		m_pSelectPageForCNS->AddOCRResult(ocrRes[k]);
		//	}
		//}
	}

	m_bIsThreadEnd = true;
}

void CMNView::DoOCRForCutImg(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage, int lineId)
{
	std::vector<stParapgraphInfo>vecline = pPage->GetVecParagraph();
//	for (auto i = 0; i < vecline.size(); i++) {
		std::vector<_stOCRResult> ocrRes;// = pPage->GetVecOCRResult();
		for (auto j = 0; j < vecline[lineId].vecTextBox.size(); j++) {
			ocrRes.push_back(vecline[lineId].vecTextBox[j]);
		}

		float fScale = 1.0f;
		cv::Rect r = rect;
		r.x -= 2;
		r.y -= 2;
		r.width += 4;
		r.height += 4;

		if (r.x < 0) r.x = 0;
		if (r.y < 0) r.y = 0;
		if (r.width > img.cols) r.width = img.cols;
		if (r.height > img.rows) r.height = img.rows;

		_LANGUAGE_TYPE* pOrder = SINGLETON_DataMng::GetInstance()->GetOCROrder();
		for (int i = 0; i < _NUM_LANGUAGE_TYPE; i++) {
			if (pOrder[i] != __LANG_NONE) {

				if (ocrRes.size() > 0) {		// First OCR //
					m_OCRMng.SetOCRDetectMode(pOrder[i], tesseract::PSM_SINGLE_WORD);
					if (pOrder[i] == __ENG) {		// Alphabetic //
						DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetTess(pOrder[i]), tesseract::RIL_WORD, fScale, pOrder[i]);
					}
					else {
						DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetTess(pOrder[i]), tesseract::RIL_SYMBOL, fScale, pOrder[i]);
					}					
				}
				else {
					if (m_extractionSetting.IsVerti) {
						m_OCRMng.SetOCRDetectMode(pOrder[i], tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
					}
					else {
						m_OCRMng.SetOCRDetectMode(pOrder[i], tesseract::PSM_SINGLE_BLOCK);
					}

					if (pOrder[i] == __ENG) {		// Alphabetic //
						m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetTess(pOrder[i]), tesseract::RIL_WORD, fScale, pOrder[i]);
					}
					else {
						m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetTess(pOrder[i]), tesseract::RIL_SYMBOL, fScale, pOrder[i]);
					}					
				}
			}
		}

/**
		//================Start With English =============================//
		if (m_extractionSetting.isEng) {
			fScale = 32.0f / (float)m_extractionSetting.engSize;
			if (ocrRes.size() > 0) {
			//	m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
				m_OCRMng.SetOCRDetectMode(__ENG,tesseract::PSM_SINGLE_WORD);
			//	DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_WORD, fScale, __ENG);
				DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetTess(__CHI), tesseract::RIL_WORD, fScale, __ENG);
			}
			else {
			//	m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_BLOCK);
				m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_BLOCK);
			//	m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetEngTess(), tesseract::RIL_WORD, fScale, __ENG);				
				m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetTess(__ENG), tesseract::RIL_WORD, fScale, __ENG);
			}
		}

		//	DoOCCorrection(img, rect, pPage, ocrRes);
			//===========Korean================================//
		if (m_extractionSetting.isKor) {
			fScale = 32.0f / (float)m_extractionSetting.korSize;
			if (ocrRes.size() > 0) {
			//	m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_WORD);
				m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_WORD);
			//	DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetKorTess(), tesseract::RIL_SYMBOL, fScale, __KOR);
				DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetTess(__KOR), tesseract::RIL_SYMBOL, fScale, __KOR);
			}
			else {
				if (m_extractionSetting.IsVerti) {
				//	m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
					m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
				}
				else {
				//	m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_BLOCK);
					m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_BLOCK);
				}
				//m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetKorTess(), tesseract::RIL_SYMBOL, fScale, __KOR);
				m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetTess(__KOR), tesseract::RIL_SYMBOL, fScale, __KOR);
			}
		}

		//================= Chinise =============================//
		if (m_extractionSetting.isChi) {
			fScale = 32.0f / (float)m_extractionSetting.chiSize;
			if (ocrRes.size() > 0) {
			//	m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_WORD);
				m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_WORD);
			//	DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
				DoOCinResults(img, rect, pPage, ocrRes, m_OCRMng.GetTess(__CHI), tesseract::RIL_SYMBOL, fScale, __CHI);
			}
			else {
				if (m_extractionSetting.IsVerti) {
				//	m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
					m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
				}
				else {
				//	m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_BLOCK);
					m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_BLOCK);
				}
				//m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
				m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetTess(__CHI), tesseract::RIL_SYMBOL, fScale, __CHI);
			}
		}

		// COR Correctness //
		DoOCCorrection(img, rect, pPage, ocrRes);
*/
	// COR Correctness //
		int cnt = 0;
		DoOCCorrection(img, rect, pPage, ocrRes, cnt);
		// Add Results //
		for (int k = 0; k < ocrRes.size(); k++) {
			if (ocrRes[k].type < 100) {
				ocrRes[k].rect.x += rect.x;
				ocrRes[k].rect.y += rect.y;
				//if (ocrRes[k].type == 0) {
				//	// check ",.;:'"(){}[]?! in prefix and surpix, then trim them //
				//}
				pPage->AddOCRResult(lineId, ocrRes[k]);
			}
		}
//	}

//	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//	std::vector<_stOCRResult> ocrRes;
	//float fScale = 1.0f;
	//if (m_extractionSetting.isEng) {
	//	fScale = 32.0f / (float)m_extractionSetting.engSize;
	//	m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetEngTess(), tesseract::RIL_WORD, fScale);
	//}
	//int resNum = ocrRes.size();

	//std::vector<_stOCRResult> ocrResChi;
	//if (m_extractionSetting.isChi) {
	//	fScale = 32.0f / (float)m_extractionSetting.chiSize;		
	//	if (resNum > 0) {
	//		for (int k = 0; k < resNum; k++) {
	//			ocrRes[k].type = __ENG;
	//			if (ocrRes[k].fConfidence < 0.70f) {
	//				cv::Mat imgword = img(ocrRes[k].rect).clone();

	//				std::vector<_stOCRResult> ocrTmp;
	//				float averConf = m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale);
	//			//	if (ocrRes[k].fConfidence < averConf) {
	//				if ( averConf > 70.0f) {
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
	//		float fScale = 32.0f / (float)m_extractionSetting.chiSize;
	//		m_OCRMng.extractWithOCR(img, ocrRes, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale);
	//		for (int k = 0; k < ocrRes.size(); k++) {
	//			ocrRes[k].type = __CHI;
	//		}
	//	}
	//}


	//for (int k = 0; k < ocrRes.size(); k++) {
	//	if (ocrRes[k].type < 100) {
	//		ocrRes[k].rect.x += rect.x;
	//		ocrRes[k].rect.y += rect.y;
	//		ocrRes[k].type = 0;
	//		pPage->AddOCRResult(ocrRes[k]);
	//	}
	//}

	//for (int k = 0; k < ocrResChi.size(); k++) {
	//	ocrResChi[k].rect.x += rect.x;
	//	ocrResChi[k].rect.y += rect.y;
	//	ocrResChi[k].type = 1;
	//	pPage->AddOCRResult(ocrResChi[k]);
	//}

	//ocrRes.clear();
	//ocrResChi.clear();
}

//void CMNView::OcrEnglishword()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		if ((ocrres.rect.width > 0) && (ocrres.rect.height>0)) {
//
//			cv::Rect r = ocrres.rect;
//			r.x -= 1;
//			r.y -= 1;
//			r.width += 2;
//			r.height += 2;
//
//			cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);
//
//			m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
//			std::vector<_stOCRResult> ocrTmp;
//			float fScale = (float)m_extractionSetting.engSize / 32.0f;
//			m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_WORD, fScale, __ENG);
//
//			// Update OCR Res //
//			if (ocrTmp.size() >= 0) {
//				m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//				for (int i = 0; i < ocrTmp.size(); i++) {
//					ocrTmp[i].rect.x += r.x;
//					ocrTmp[i].rect.y += r.y;
//					ocrTmp[i].type = 0;
//					m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//				}
//				m_selOCRId = -1;
//			}
//		}
//	}
//}
//
//void CMNView::OcrEnglishChar()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		cv::Rect r = ocrres.rect;
//		r.x -= 1;
//		r.y -= 1;
//		r.width += 2;
//		r.height += 2;
//
//		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r);
//
//		m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
//		std::vector<_stOCRResult> ocrTmp;
//
//		float fScale = (float)m_extractionSetting.engSize / 32.0f;
//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL, fScale, __ENG);
//
//		// Update OCR Res //
//		if (ocrTmp.size() >= 0) {
//			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//			for (int i = 0; i < ocrTmp.size(); i++) {
//				ocrTmp[i].rect.x += r.x;
//				ocrTmp[i].rect.y += r.y;
//				ocrTmp[i].type = 0;
//				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//			}
//			m_selOCRId = -1;
//		}
//	}
//}
//
//void CMNView::OcrChiChar()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		cv::Rect r = ocrres.rect;
//		r.x -= 1;
//		r.y -= 1;
//		r.width += 2;
//		r.height += 2;
//
//		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
////		cv::imshow("cword", imgword);
//
//		m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_CHAR);
//		std::vector<_stOCRResult> ocrTmp;
//		float fScale = (float)m_extractionSetting.chiSize / 32.0f;
//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
//
//		// Update OCR Res //
//		if (ocrTmp.size() >= 0) {
//			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//			for (int i = 0; i < ocrTmp.size(); i++) {
//				ocrTmp[i].rect.x += r.x;
//				ocrTmp[i].rect.y += r.y;
//				ocrTmp[i].type = 1;
//				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//			}
//			m_selOCRId = -1;
//		}
//	}
//}
//
//void CMNView::OcrChiWord()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		cv::Rect r = ocrres.rect;
//		r.x -= 1;
//		r.y -= 1;
//		r.width += 2;
//		r.height += 2;
//
//		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
//
//		if (m_extractionSetting.IsVerti) {
//			m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
//		}
//		else {
//			m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_WORD);
//		}
//		std::vector<_stOCRResult> ocrTmp;
//		float fScale = 32.0f / (float)m_extractionSetting.chiSize;
//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
//
//		// Update OCR Res //
//		if (ocrTmp.size() >= 0) {
//			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//			for (int i = 0; i < ocrTmp.size(); i++) {
//				ocrTmp[i].rect.x += r.x;
//				ocrTmp[i].rect.y += r.y;
//				ocrTmp[i].type = 1;
//				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//			}
//			m_selOCRId = -1;
//		}
//	}
//}

void CMNView::RemoveNoise()
{
	if (m_pSelectPageForCNS) {
		RECT2D rect = GetSelectedAreaForCNS();
		cv::Rect r(rect.x1, rect.y1, rect.width, rect.height);
		m_pSelectPageForCNS->RemoveNoise(r);
	}
}
//void CMNView::OcrKorChar()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		cv::Rect r = ocrres.rect;
//		r.x -= 1;
//		r.y -= 1;
//		r.width += 2;
//		r.height += 2;
//
//		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
//		//		cv::imshow("cword", imgword);
//
//		m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_CHAR);
//		std::vector<_stOCRResult> ocrTmp;
//		float fScale = (float)m_extractionSetting.chiSize / 32.0f;
//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetKorTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
//
//		// Update OCR Res //
//		if (ocrTmp.size() >= 0) {
//			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//			for (int i = 0; i < ocrTmp.size(); i++) {
//				ocrTmp[i].rect.x += r.x;
//				ocrTmp[i].rect.y += r.y;
//				ocrTmp[i].type = 1;
//				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//			}
//			m_selOCRId = -1;
//		}
//	}
//}
//
//void CMNView::OcrKorWord()
//{
//	if (m_pSelectPageForCNS) {
//		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
//		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(m_selOCRId);
//
//		cv::Rect r = ocrres.rect;
//		r.x -= 1;
//		r.y -= 1;
//		r.width += 2;
//		r.height += 2;
//
//		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
//
//		if (m_extractionSetting.IsVerti) {
//			m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
//		}
//		else {
//			m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_WORD);
//		}
//		std::vector<_stOCRResult> ocrTmp;
//		float fScale = 32.0f / (float)m_extractionSetting.chiSize;
//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetKorTess(), tesseract::RIL_SYMBOL, fScale, __CHI);
//
//		// Update OCR Res //
//		if (ocrTmp.size() >= 0) {
//			m_pSelectPageForCNS->DeleteSelOCRRes(m_selOCRId);
//			for (int i = 0; i < ocrTmp.size(); i++) {
//				ocrTmp[i].rect.x += r.x;
//				ocrTmp[i].rect.y += r.y;
//				ocrTmp[i].type = 1;
//				m_pSelectPageForCNS->AddOCRResult(ocrTmp[i]);
//			}
//			m_selOCRId = -1;
//		}
//	}
//}


bool CMNView::MeargingtLineBox(std::vector<stParapgraphInfo>& vecBox, int& depth)
{
	std::vector<stParapgraphInfo> tmp = vecBox;
//	vecBox = std::vector<stParapgraphInfo>();
	vecBox.swap(std::vector<stParapgraphInfo>());

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	for (int i = 0; i < tmp.size(); i++) {

		for (int j = i + 1; j < tmp.size(); j++) {
			cv::Rect andRect_overlap = (tmp[i].rect & tmp[j].rect);

			float fOverlap_i = (float)andRect_overlap.area() / (float)tmp[i].rect.area();
			float fOverlap_j = (float)andRect_overlap.area() / (float)tmp[j].rect.area();
			cv::Rect mergeBox = (tmp[i].rect | tmp[j].rect);
			//if (fOverlap_i > 0.1f) {		// Skip i !				
			//	tmp[j].rect = mergeBox;
			//	tmp[j].fConfidence = 0.1f;
			//	tmp[i].type = -1;
			//	IsMerged = true;
			//}
			//else if (fOverlap_j > 0.1f) {
			//	tmp[i].rect = mergeBox;
			//	tmp[i].fConfidence = 0.1f;
			//	tmp[j].type = -1;
			//	IsMerged = true;
			//}
		}
	}

	for (int i = 0; i < tmp.size(); i++) {
		//if (tmp[i].type != -1) {
		//	vecBox.push_back(tmp[i]);
		//}
	}
	tmp.swap(std::vector<stParapgraphInfo>());

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtLineBox(vecBox, depth);
	}
	return true;
}



void CMNView::DoOCRForPage(CMNPageObject* pPage)
{
	if (pPage) {
	//	cv::Mat gray = pPage->GetSrcPageGrayImg().clone();
		std::vector<stParapgraphInfo> vecline = pPage->GetVecParagraph();

		if (vecline.size() > 0) {		// segmented !!
			for (auto i = 0; i < vecline.size(); i++) {
				for (auto j = 0; j < vecline[i].vecTextBox.size(); j++) {
					cv::Rect r = vecline[i].vecTextBox[j].rect;
					cv::Mat imgword = pPage->GetSrcPageGrayImg()(r);
				
					_stOCRResult ocrres = GetCORResult(imgword, false);
					//
					//vecline[i].vecTextBox[j].fConfidence = ocrres.fConfidence;
					//memcpy(vecline[i].vecTextBox[j].strCode, ocrres.strCode, sizeof(ocrres.strCode));
					pPage->UpdateOCRCode(ocrres.strCode, ocrres.fConfidence, i, j);

					imgword.release();
				}
			}
		}


		//else {		// without segmentation //

		//	for (auto i = 0; i < vecline.size(); i++) {
		//		for (auto j = 0; j < vecline[i].vecTextBox.size(); j++) {
		//			// recognized by rect --> dont chanage rect size here!!! //
		//			if (vecline[i].vecTextBox[j].fConfidence > 0.9f) {
		//				gray(vecline[i].vecTextBox[j].rect).setTo(cv::Scalar(255));
		//			}
		//			else {
		//				pPage->UpdateOCRResStatus(i, j, false, 100);
		//			}
		//		}
		//	}
		//	pPage->CleanUpOCRres();

		//	std::vector<stParapgraphInfo> para = pPage->GetVecParagraph();
		//	if (para.size() == 0)
		//		return;


		//	m_addImgCnt = 0;
		//	m_loadedImgCnt = static_cast<int>(para.size());
		//	int x1 = 9999, y1 = 9999, x2 = 0, y2 = 0;
		//	for (int i = 0; i < para.size(); i++) {

		//		//if (m_extractionSetting.IsVerti == false) {
		//		cv::Mat pimg = gray(para[i].rect).clone();
		//		DoOCRForCutImg(pimg, para[i].rect, pPage, i);
		//		pimg.release();
		//		m_addImgCnt++;
		//	}
		//}
	}
		
}

void CMNView::DoExtractBoundaryForSelected()
{
	if (m_pSelectPageForCNS) {
		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
		ExtractBoundaryForPage(m_pSelectPageForCNS, false);
	}
	//m_addImgCnt = 0;
	//m_loadedImgCnt = 1;
	//m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
	//if (m_pSelectPageForCNS) {
	//	int nMargin = 4;
	//	cv::Rect mRect(nMargin, nMargin, m_pSelectPageForCNS->GetSrcPageGrayImg().cols - nMargin * 2, m_pSelectPageForCNS->GetSrcPageGrayImg().rows - nMargin * 2);
	//	//CString strpath = vecImg[i]->GetPath();
	//	cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg()(mRect).clone();
	//	std::vector<stParapgraphInfo> lines = m_pSelectPageForCNS->GetVecParagraph();
	//	for (auto i = 0; i < lines.size(); i++) {
	//		cv::Rect r = lines[i].rect;			
	//		r.x = (r.x - 4) < 0 ? 0 : (r.x - 4);
	//		r.y = (r.y - 4) < 0 ? 0 : (r.y - 4);
	//		srcImg(r).setTo(cv::Scalar(255));
	//	}
	//	//if (m_pSelectPageForCNS->GetVecParagraph().size() > 0) {
	//	//	// Fill previous segmented area with white color //
	//	//}
	//	
	//	//cv::Mat srcImg = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
	//	if (srcImg.ptr()) {

	//		//	cv::GaussianBlur(srcImg, srcImg, cv::Size(7, 7), 0);
	//		//cv::threshold(srcImg, srcImg, 128, 255, cv::THRESH_OTSU);
	//		////	cv::threshold(srcImg, b2, 0, 255, CV_THRESH_BINARY + cv::THRESH_OTSU);
	//		cv::bitwise_not(srcImg, srcImg);
	//		int fsize = (int)m_extractionSetting.engSize;

	//		// Detect text block //
	//		//_ALIGHN_TYPE align;
	//		std::vector<_extractBox> vecBox;
	//		ExtractLineBox(srcImg, vecBox, m_extractionSetting.IsVerti, __ENG);

	//		for (size_t j = 0; j < vecBox.size(); j++) {
	//			vecBox[j].textbox.x += 4;
	//			vecBox[j].textbox.y += 4;
	//		}
	//		srcImg.release();


	//		cv::Mat srcImgforLine = m_pSelectPageForCNS->GetSrcPageGrayImg().clone();
	//		cv::bitwise_not(srcImgforLine, srcImgforLine);
	//		for (size_t j = 0; j < vecBox.size(); j++) {

	//			if ((vecBox[j].textbox.width < 32) || (vecBox[j].textbox.height < 32))
	//				continue;

	//			// Calculate Deskew //
	//			//m_Extractor.verifyImgSize(vecBox[j].textbox, srcImg.cols, srcImg.rows);
	//			cv::Mat para = srcImgforLine(vecBox[j].textbox);
	//			//_ALIGHN_TYPE align = m_Extractor.AllHoriVertLines(para);

	//			float deskew = m_Extractor.DeSkewImg(para);

	//			bool IsAlphabetic = false;
	//			if (m_extractionSetting.isEng)	IsAlphabetic = true;
	//			m_pSelectPageForCNS->AddParagraph(m_Extractor, para, vecBox[j].textbox, m_extractionSetting.IsVerti, deskew, IsAlphabetic);
	//		}
	//		vecBox.swap(std::vector<_extractBox>());
	//		srcImgforLine.release();
	//	}
	//}
	//
	//m_addImgCnt = 1;
}



void CMNView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
//	if (m_selOCRId >= 0) {

	if (m_spliteDirection == _NONE_DIR) {
		CMenu menu;
		menu.LoadMenuW(IDR_MENU_OCR);
		CMenu* pMenu = menu.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	else {
		m_spliteDirection = _NONE_DIR;
		m_spliteType = _SPLIT_TEXT;
	}


//		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_OCR, point.x, point.y, this, TRUE);
//	}	
}

void CMNView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
		
	//if (m_spliteDirection >= 0) {
	//	m_spliteDirection = -1;
	//}

//	m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
//	SelectObject3D(point.x, point.y, 2, 2, 0);	
	COGLWnd::OnRButtonDown(nFlags, point);
}

void CMNView::ProcDoSearchSelection()
{
	std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
	if (iter != m_mapSelectionInfo.end() && (iter->second.vecTextId.size() > 0)) {
		_stOCRResult ocrRes = m_pSelectPageForCNS->GetOCRResult(iter->second.lineid, iter->second.vecTextId[0]);
		SINGLETON_DataMng::GetInstance()->DoKeywordSearch(ocrRes.strCode);
		InitCamera();
	}

	//_stOCRResult ocrRes = m_pSelectPageForCNS->GetOCRResult(m_selParaId, m_selOCRId);
	//SINGLETON_DataMng::GetInstance()->DoKeywordSearch(ocrRes.strCode);
	//InitCamera();
}

int CMNView::SelectObject3DForMouseOver(int x, int y, int rect_width, int rect_height, int selmode)
{
	if (m_bIsThreadEnd == false) return 0;


	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

	if (m_pSelectPageForCNS) {
		m_selOCRIdforMouseHover = -1;
		m_selParaIdforMouseHover = -1;
	}

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
	DrawOCRForPicking();

	//}
	//=============================================//
	hits = glRenderMode(GL_RENDER);
	if (hits > 0)
	{
		unsigned int uuid = 0;
		for (int i = 0; i < hits; i++) {
			int selid = selectBuff[i * 4 + 3]; 
			if (m_pSelectPageForCNS) {
				if ((selid >= _PICK_WORD) && (selid < _PICK_MATCH)) {		// Word selection
					uuid = selid - _PICK_WORD;
					m_pSelectPageForCNS->GetIDbyUUID(uuid, m_selParaIdforMouseHover, m_selOCRIdforMouseHover);
				}
			}
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return hits;
}


void CMNView::OcrFromTextBox(_LANGUAGE_TYPE langType, int searchType)
{
	if (m_pSelectPageForCNS) {
		int selline = -1, seltext = -1;
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		if (iter != m_mapSelectionInfo.end() && (iter->second.vecTextId.size() > 0)) {
			selline = iter->second.lineid;
			seltext = iter->second.vecTextId[0];
		}
		else {
			return;
		}

		m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();
		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(selline, seltext);

		cv::Rect r = ocrres.rect;
		//r.x -= 1;
		//r.y -= 1;
		//r.width += 2;
		//r.height += 2;

		cv::Mat imgword = m_pSelectPageForCNS->GetSrcPageGrayImg()(r).clone();
		//cv::imshow("cword", imgword);

		std::vector<_stOCRResult> ocrTmp;
		float fScale = 1.0f;

		if (langType != __LANG_NONE) {
			if (searchType == 0) {
				if (langType == __ENG) {
					m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_WORD);
				}
				else {
					m_OCRMng.SetOCRDetectMode(langType, tesseract::PSM_SINGLE_CHAR);
				}
			}
			else {
				if (m_extractionSetting.IsVerti) {
					m_OCRMng.SetOCRDetectMode(langType, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
				}
				else {
					m_OCRMng.SetOCRDetectMode(langType, tesseract::PSM_SINGLE_BLOCK);
				}
			}
			//fScale = (float)m_extractionSetting.chiSize / 32.0f;
			if (langType == __ENG) {
				m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(langType), tesseract::RIL_WORD, fScale, langType);
			}
			else {
				m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(langType), tesseract::RIL_SYMBOL, fScale, langType);
			}
			for (int i = 0; i < ocrTmp.size(); i++) {
				ocrTmp[i].type = langType;
			}
		}
		else {
			ocrres.fConfidence = 0.1f;
			ocrres.rect.x = 0;
			ocrres.rect.y = 0;
			//ocrres.rect.width += 2;
			//ocrres.rect.height += 2;
			ocrTmp.push_back(ocrres);
			int cnt = 0;
			DoOCCorrection(imgword, r, m_pSelectPageForCNS, ocrTmp, cnt);
		}







		//switch (langType) {
		//case __CHI:
		//	if (searchType == 0) {
		//		m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_CHAR);
		//	}
		//	else {
		//		if (m_extractionSetting.IsVerti) {
		//			m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
		//		}
		//		else {
		//			m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_BLOCK);
		//		}
		//	}			
		//	fScale = (float)m_extractionSetting.chiSize / 32.0f;
		//	m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(__CHI), tesseract::RIL_SYMBOL, fScale, __CHI);
		//	for (int i = 0; i < ocrTmp.size(); i++) {
		//		ocrTmp[i].type = __CHI;
		//	}
		//	break;
		//case __KOR:
		//	fScale = (float)m_extractionSetting.chiSize / 32.0f;
		//	if (searchType == 0) {
		//		m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_CHAR);
		//		m_OCRMng.extractWithOCRSingle(imgword, ocrTmp, m_OCRMng.GetTess(__KOR), tesseract::RIL_SYMBOL, fScale, __KOR);
		//	}
		//	else {
		//		if (m_extractionSetting.IsVerti) {
		//			m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
		//		}
		//		else {
		//			m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_BLOCK);
		//		}
		//		m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(__KOR), tesseract::RIL_SYMBOL, fScale, __KOR);
		//	}		
		//	for (int i = 0; i < ocrTmp.size(); i++) ocrTmp[i].type = __KOR;
		//	break;
		//case __ENG:
		//	if (searchType == 0) {
		//		m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_WORD);
		//	}
		//	else {
		//		if (m_extractionSetting.IsVerti) {
		//			m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);
		//		}
		//		else {
		//			m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_BLOCK);
		//		}
		//	}
		//	fScale = (float)m_extractionSetting.chiSize / 32.0f;
		//	//m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetEngTess(), tesseract::RIL_WORD, fScale, __ENG);
		//	m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetTess(__ENG), tesseract::RIL_WORD, fScale, __ENG);
		//	for (int i = 0; i < ocrTmp.size(); i++) ocrTmp[i].type = __ENG;
		//	break;
		//default:   // From DB
		//	ocrres.fConfidence = 0.1f;
		//	ocrres.rect.x = 0;
		//	ocrres.rect.y = 0;
		//	//ocrres.rect.width += 2;
		//	//ocrres.rect.height += 2;
		//	ocrTmp.push_back(ocrres);
		//	DoOCCorrection(imgword, r, m_pSelectPageForCNS, ocrTmp);
		//	break;
	
		//}

		// OCR Correction //
		//DoOCCorrection(imgword, r, m_pSelectPageForCNS, ocrTmp);


		//m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_CHAR);
		//std::vector<_stOCRResult> ocrTmp;
		//float fScale = (float)m_extractionSetting.chiSize / 32.0f;
		//m_OCRMng.extractWithOCR(imgword, ocrTmp, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, fScale, __CHI);

		// Update OCR Res //
		if (ocrTmp.size() >= 0) {
			m_pSelectPageForCNS->DeleteSelOCRRes(selline, seltext);
			for (int i = 0; i < ocrTmp.size(); i++) {
				if (ocrTmp[i].type < 100) {
					ocrTmp[i].rect.x += r.x;
					ocrTmp[i].rect.y += r.y;
					m_pSelectPageForCNS->AddOCRResult(selline, ocrTmp[i]);
				}
			}
		//	m_selOCRId = -1;
		// Reset Selection  //
			std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
			for (; iter != m_mapSelectionInfo.end(); iter++) {
				iter->second.vecTextId.swap(std::vector<int>());
			}
			m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		}
	}


	MakeList_DrawOCRResText();
}

void CMNView::ProcDoSearchBySelection()
{
	InitCamera(false);
	int selline = -1, seltext = -1;
	std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
	if (iter != m_mapSelectionInfo.end() && (iter->second.vecTextId.size() > 0)) {
		selline = iter->second.lineid;
		seltext = iter->second.vecTextId[0];
	}

	if ((m_pSelectPageForCNS) && (selline >=0) && (seltext>=0)){

		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		// Load Full image //
		//for (size_t i = 0; i < imgVec.size(); i++) {
		//	imgVec[i]->LoadFullImage();
		//	imgVec[i]->SetIsSearched(false);
		//}

		_stOCRResult ocrres = m_pSelectPageForCNS->GetOCRResult(selline, seltext);
		// Prepare Cut&Search ==========================================//
		RECT2D rect;
		rect.set(ocrres.rect.x, ocrres.rect.x + ocrres.rect.width, ocrres.rect.y, ocrres.rect.y + ocrres.rect.height);
		
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
		//==============================================================//


		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		m_fThreshold = pM->GetThreshold()*0.01f;
		COLORREF resColor = pM->GetMatchingColor();
		m_resColor.r = (float)GetRValue(resColor)*0.00392f;
		m_resColor.g = (float)GetGValue(resColor)*0.00392f;
		m_resColor.b = (float)GetBValue(resColor)*0.00392f;

		
		CWinThread* pl;
		m_bIsThreadEnd = false;
		pl = AfxBeginThread(ThreadDoSearch, this);
		SetTimer(_DO_SEARCH, 100, NULL);
	}
}

void CMNView::ProcDoSearchBySelectionAll()
{
//	InitCamera(false);
	if ((m_pSelectPageForCNS)) {

		m_pSelectPageForCNSAll = m_pSelectPageForCNS;
		//std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
		// Load Full image //
		//for (size_t i = 0; i < imgVec.size(); i++) {
		//	imgVec[i]->LoadFullImage();
		//}

		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		m_fThreshold = pM->GetThreshold()*0.01f;
		COLORREF resColor = pM->GetMatchingColor();
		m_resColor.r = (float)GetRValue(resColor)*0.00392f;
		m_resColor.g = (float)GetGValue(resColor)*0.00392f;
		m_resColor.b = (float)GetBValue(resColor)*0.00392f;


		CWinThread* pl;
		m_bIsThreadEnd = false;
		pl = AfxBeginThread(ThreadDoSearchSegment, this);
		SetTimer(_DO_SEARCH, 100, NULL);
	}
}

void CMNView::ProcExportDB(CString strFolder, bool IsHtml)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	CString str;
	str.Format(_T("Export DB.....start"));
	pM->AddOutputString(str, false);

	m_strExportDBFoler = strFolder;

	CWinThread* pl;
	m_bIsThreadEnd = false;

	if (IsHtml) {
		pl = AfxBeginThread(ThreadDoExportDBHtml, this);
	}
	else {
		pl = AfxBeginThread(ThreadDoExportDB, this);
	}
	


	SetTimer(_DO_EXPORT_DB, 100, NULL);
}

void CMNView::DoExportDB()
{
	m_addImgCnt = 0;
	m_bIsThreadEnd = false;
	m_loadedImgCnt = SINGLETON_DataMng::GetInstance()->GetFileTableSize();
	SINGLETON_DataMng::GetInstance()->ExportDatabase(m_strExportDBFoler);
}

void CMNView::DoExportDBHtml()
{
	m_addImgCnt = 0;
	m_bIsThreadEnd = false;
	m_loadedImgCnt = SINGLETON_DataMng::GetInstance()->GetFileTableSize();
	SINGLETON_DataMng::GetInstance()->ExportDatabaseToHtml(m_strExportDBFoler);
}


void CMNView::ProcCNSSegments()
{
//	InitCamera(false);
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	m_fThreshold = pM->GetThreshold()*0.01f;
	
	//DoCNSSegments();	
	//SINGLETON_DataMng::GetInstance()->SetMatchingResults();
	//SINGLETON_DataMng::GetInstance()->SortMatchingResults();
	//pM->AddMatchResult();
	m_extractionSetting = SINGLETON_DataMng::GetInstance()->GetExtractionSetting();

//	pM->AddMatchResultCNS();
	pM->ClearAllResults();
	CWinThread* pl;
	pl = AfxBeginThread(ThreadCNSSegments, this);
	SetTimer(_DO_CNS_SEGMENTS, 100, NULL);
}

bool CMNView::DoCNSSegments()
{
	// generate temporary index that start from 65536 ! , except for unicode scope //
	m_bIsThreadEnd = false;
//	std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();
	m_addImgCnt = 0;
	m_loadedImgCnt = 1;

	SINGLETON_DataMng::GetInstance()->CutNSearchMatching(m_addImgCnt, m_loadedImgCnt, m_fThreshold, m_vecSelPage);
	m_bIsThreadEnd = true;

	//auto i = 0;
	//// Prepare Cut&Search matching //
	//std::vector<_stCNSResult> vecCns;
	//for (i = 0; i < imgVec.size(); i++) {
	//	std::vector<_stOCRResult> ocrRes = imgVec[i]->GetVecOCRResult();
	//	for (auto j = 0; j < ocrRes.size(); j++) {

	//		_stCNSResult cns;
	//		cns.uuid = 0;
	//		cns.pKey = nullptr;
	//		cns.pageid = i;
	//		cns.objid = j;
	//	}		
	//}

	//for (i = 0; i < imgVec.size(); i++) {
	//	std::vector<_stOCRResult> ocrRes = imgVec[i]->GetVecOCRResult();
	//	imgVec[i]->SetSelection(true);
	//	if (i > 0)	imgVec[i - 1]->SetSelection(false);

	//	m_resColor.r = (float)(rand() % 255)*0.00392f;
	//	m_resColor.g = (float)(rand() % 255)*0.00392f;
	//	m_resColor.b = (float)(rand() % 255)*0.00392f;


	//	for (auto j = 0; j < ocrRes.size(); j++) {
	//		m_addImgCnt = 0;

	//		cv::Mat srcImg = imgVec[i]->GetSrcPageGrayImg();
	//		cv::Mat cutSrc, cutDst;
	//		cv::Rect nRect;
	//		if (srcImg.ptr()) {
	//			cutSrc.release();
	//			// Normalize //
	//			nRect = SINGLETON_DataMng::GetInstance()->GetNomalizedWordSize(ocrRes[j].rect);
	//			cutSrc = srcImg(ocrRes[j].rect).clone();
	//			cv::resize(cutSrc, cutSrc, cv::Size(nRect.width, nRect.height));

	//			// 1. Generate Unique Index //

	//


	//			auto k = 0;
	//			for (k = 0; k < imgVec.size(); k++) {
	//				std::vector<_stOCRResult> ocrResDst = imgVec[k]->GetVecOCRResult();
	//				for (auto l = 0; l < ocrResDst.size(); l++) {

	//				//	if ((i == k) && (j == l)) continue;			// skip same character!!

	//					cv::Mat dstImg = imgVec[k]->GetSrcPageGrayImg();
	//					if (dstImg.ptr()) {
	//						cutDst.release();
	//						nRect = SINGLETON_DataMng::GetInstance()->GetNomalizedWordSize(ocrResDst[l].rect);
	//						cutDst = dstImg(ocrResDst[l].rect).clone();
	//						cv::resize(cutDst, cutDst, cv::Size(nRect.width, nRect.height));

	//						// Match cutSrc with cutDst
	//						// Add to matching group //
	//						if (cutSrc.cols == cutDst.cols) {
	//							cv::Mat result(1, 1, CV_32FC1);
	//							cv::matchTemplate(cutSrc, cutDst, result, CV_TM_CCOEFF_NORMED);
	//							float fD1 = result.at<float>(0, 0);
	//							//cv::matchTemplate(cutDst, cutSrc, result, CV_TM_CCOEFF_NORMED);
	//							//float fD2 = result.at<float>(0, 0);
	//							//if (fD1  > ocrResDst[l].fConfidence) {
	//							if (fD1  > 0.85f) {
	//								imgVec[k]->UpdateOCRCode(L"T", fD1, l);
	//							}								
	//						}
	//						//cv::imshow("cutSrc", cutSrc);
	//						//cv::imshow("cutDst", cutDst);



	//					}// End of if srcimg is ture !!
	//				}



	//				imgVec[k]->SetIsSearched(true);
	//				if (k > 0)	imgVec[k - 1]->SetIsSearched(false);
	//				m_addImgCnt++;
	//			}
	//			imgVec[k - 1]->SetIsSearched(false);
	//		}  // End of if srcimg is ture !!


	//		cutSrc.release();
	//		cutDst.release();




	//	}	
	//}
	//imgVec[i-1]->SetSelection(false);	
	//m_bIsThreadEnd = true;
//==============================//





	//std::vector<_stOCRResult> ocrRes = m_pSelectPageForCNSAll->GetVecOCRResult();
	//m_addImgCnt = 0;
	//m_loadedImgCnt = ocrRes.size();

	//for (auto j = 0; j < ocrRes.size(); j++) {
	//	_stOCRResult ocrres = ocrRes[j];
	//	// Prepare Cut&Search ==========================================//
	//	RECT2D rect;
	//	rect.set(ocrres.rect.x, ocrres.rect.x + ocrres.rect.width, ocrres.rect.y, ocrres.rect.y + ocrres.rect.height);

	//	if ((rect.width > 16) && (rect.height > 16)) {

	//		// Update Font Size ==//
	//		m_Extractor.SetFontSize(rect.width, rect.height);

	//		if (m_pCut.ptr() != NULL) {
	//			m_pCut.release();
	//		}
	//		CString strpath = m_pSelectPageForCNSAll->GetPath();
	//		cv::Mat srcImg = m_pSelectPageForCNSAll->GetSrcPageGrayImg();
	//		if (srcImg.ptr()) {
	//			m_pCut = srcImg(cv::Rect(rect.x1, rect.y1, rect.width, rect.height));
	//		}


	//		m_cutInfo.fileid = getHashCode((CStringA)m_pSelectPageForCNSAll->GetPath());
	//		m_cutInfo.posid = (int)rect.x1 * 10000 + (int)rect.y1;
	//		CString strId;
	//		strId.Format(L"%u%u", m_cutInfo.fileid, m_cutInfo.posid);
	//		m_cutInfo.id = getHashCode((CStringA)strId);
	//		m_cutInfo.th = m_fThreshold;

	//		//==============================================================//


	//		if (m_pCut.ptr() == NULL)
	//			return false;
	//		int search_size = (m_pCut.cols > m_pCut.rows) ? m_pCut.cols : m_pCut.rows;
	//		std::vector<CMNPageObject*> imgVec = SINGLETON_DataMng::GetInstance()->GetVecImgData();

	//		auto i = 0;
	//		for (i = 0; i < imgVec.size(); i++) {
	//			imgVec[i]->SetIsSearched(false);
	//		}

	//		for (i = 0; i < imgVec.size(); i++) {
	//			cv::Mat grayImg = imgVec[i]->GetSrcPageGrayImg();
	//			//if (SINGLETON_DataMng::GetInstance()->LoadImageData(imgVec[i]->GetPath(), grayImg, true)) {
	//			if (grayImg.ptr()) {
	//				int result_cols = grayImg.cols - m_pCut.cols + 1;
	//				int result_rows = grayImg.rows - m_pCut.rows + 1;
	//				cv::Mat result(result_rows, result_cols, CV_32FC1);

	//				cv::matchTemplate(grayImg, m_pCut, result, CV_TM_CCOEFF_NORMED);
	//				//cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	//				for (int y = 0; y < result.rows; y++) {
	//					for (int x = 0; x < result.cols; x++) {
	//						float fD = result.at<float>(y, x);
	//						if (fD > m_fThreshold) {
	//							stMatchInfo mInfo;
	//							mtSetPoint3D(&mInfo.pos, x + m_pCut.cols*0.5f, y + m_pCut.rows*0.5f, 0.0f);
	//							mInfo.accuracy = fD * 100;
	//							mInfo.strAccracy.Format(L"%d", (int)(fD));
	//							mInfo.rect = cv::Rect(x, y, m_pCut.cols, m_pCut.rows);
	//							mInfo.searchId = m_cnsSearchId;
	//							mInfo.cInfo = m_cutInfo;
	//							mInfo.strCode = "-";

	//							m_resColor.a = ((fD)*m_colorAccScale)*0.5f;
	//							mInfo.color = m_resColor;
	//							mInfo.IsAdded = false;
	//							imgVec[i]->AddMatchedPoint(std::move(mInfo), search_size);
	//						}
	//					}
	//				}
	//				//grayImg.release();
	//				result.release();

	//				imgVec[i]->SetIsSearched(true);
	//				if (i > 0) {
	//					imgVec[i - 1]->SetIsSearched(false);
	//				}
	//			}

	//			imgVec[i]->SetFitCurArea();
	//		}

	//		imgVec[i - 1]->SetIsSearched(false);
	//		m_cnsSearchId++;
	//	}
	//	m_addImgCnt++;
	//}
	//return true;




	//m_bIsThreadEnd = false;
	return true;
}


_stOCRResult CMNView::GetCORResult(cv::Mat& cutImg, bool IsMooN)
{
	_stOCRResult ocrres;
	ocrres.init();	
	
	// Seach DB First //
	if (IsMooN) {
		SINGLETON_DataMng::GetInstance()->MatchingFromDB(cutImg, ocrres);
		if (ocrres.fConfidence > 0.9f) {
			return ocrres;
		}
	}

	// Search OCR //
	_LANGUAGE_TYPE* pOrder = SINGLETON_DataMng::GetInstance()->GetOCROrder();
	for (int i = 0; i < _NUM_LANGUAGE_TYPE; i++) {
		if (pOrder[i] != __LANG_NONE) {
			_stOCRResult ocrtmp;
			if (pOrder[i] == __ENG) {
				m_OCRMng.SetOCRDetectMode(pOrder[i], tesseract::PSM_SINGLE_WORD);
				ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetTess(__ENG), tesseract::RIL_WORD, 1.0f, pOrder[i]);
				ocrtmp.type = __ENG;
			}
			else {
				m_OCRMng.SetOCRDetectMode(pOrder[i], tesseract::PSM_SINGLE_WORD);
				ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetTess(pOrder[i]), tesseract::RIL_SYMBOL, 1.0f, pOrder[i]);
				ocrtmp.type = pOrder[i];
			}	
						
			if (ocrtmp.fConfidence > 0.9f) {
				return ocrtmp;
			}
			else {
				if (ocrtmp.fConfidence > ocrres.fConfidence) {
					ocrres = ocrtmp;
				}
			}
		}
	}

	//if (m_extractionSetting.isChi) {
	////	m_OCRMng.SetOCRDetectModeChi(tesseract::PSM_SINGLE_CHAR);
	//	m_OCRMng.SetOCRDetectMode(__CHI, tesseract::PSM_SINGLE_CHAR);
	////	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL, 1.0f, __CHI);
	//	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetTess(__CHI), tesseract::RIL_SYMBOL, 1.0f, __CHI);
	//	if (ocrtmp.fConfidence > 0.9f) {
	//		return ocrtmp;
	//	}
	//	else {
	//		if (ocrtmp.fConfidence > ocrres.fConfidence) {
	//			ocrres = ocrtmp;
	//		}
	//	}
	//}

	//if (m_extractionSetting.isKor) {
	////	m_OCRMng.SetOCRDetectModeKor(tesseract::PSM_SINGLE_CHAR);
	//	m_OCRMng.SetOCRDetectMode(__KOR, tesseract::PSM_SINGLE_CHAR);
	////	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetKorTess(), tesseract::RIL_SYMBOL, 1.0f, __KOR);
	//	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetTess(__KOR), tesseract::RIL_SYMBOL, 1.0f, __KOR);
	//	if (ocrtmp.fConfidence > 0.9f) {
	//		return ocrtmp;
	//	}
	//	else {
	//		if (ocrtmp.fConfidence > ocrres.fConfidence) {
	//			ocrres = ocrtmp;
	//		}
	//	}
	//}

	//if (m_extractionSetting.isEng) {
	////	m_OCRMng.SetOCRDetectModeEng(tesseract::PSM_SINGLE_WORD);
	//	m_OCRMng.SetOCRDetectMode(__ENG, tesseract::PSM_SINGLE_WORD);
	////	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetEngTess(), tesseract::RIL_WORD, 1.0f, __ENG);
	//	_stOCRResult ocrtmp = m_OCRMng.getOcrResFromSingleCut(cutImg, m_OCRMng.GetTess(__ENG), tesseract::RIL_WORD, 1.0f, __ENG);
	//	if (ocrtmp.fConfidence > 0.9f) {
	//		return ocrtmp;
	//	}
	//	else {
	//		if (ocrtmp.fConfidence > ocrres.fConfidence) {
	//			ocrres = ocrtmp;
	//		}
	//	}
	//}

	return ocrres;
}

void CMNView::MergeSelectedTextBox()
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();
		for (; iter != m_mapSelectionInfo.end(); iter++) {

			// Calculate Merger box //
			cv::Rect mergeRect;
			for (auto i = 0; i < iter->second.vecTextId.size(); i++) {
				_stOCRResult res = m_pSelectPageForCNS->GetOCRResult(iter->second.lineid, iter->second.vecTextId[i]);
				if (i == 0)	mergeRect = res.rect;
				else		mergeRect = mergeRect | res.rect;
			}

			// sorting selected id //
			std::vector<int> vecSort = iter->second.vecTextId;
			for (auto i = 0; i < vecSort.size() - 1; i++) {
				for (auto j = i + 1; j < vecSort.size(); j++) {
					if (vecSort[i] < vecSort[j]) {
						int tmp = vecSort[i];
						vecSort[i] = vecSort[j];
						vecSort[j] = tmp;
					}
				}
			}

			// Delere item on the list //
			for (auto i = 0; i < vecSort.size(); i++) {
				unsigned int uuid = m_pSelectPageForCNS->DeleteSelOCRRes(iter->second.lineid, vecSort[i]);
				pM->DeleteMatchList(uuid);		
			}

			// Add a merge box as a new box //
			AddNewTextBox(mergeRect);
		}
		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		m_pSelectPageForCNS->SetSelMatchItem(-1);
	}
}

void CMNView::MergeSelectedLineBox()
{
	if ((m_pSelectPageForCNS)) {
		std::map<int, _stLineTextSelectionInfo>::iterator iter = m_mapSelectionInfo.begin();

		std::vector<int> vecSort;
		
		for (; iter != m_mapSelectionInfo.end(); iter++) {
			vecSort.push_back(iter->first);			
		}
			
		// sorting selected id //
		for (auto i = 0; i < vecSort.size() - 1; i++) {
			for (auto j = i + 1; j < vecSort.size(); j++) {
				if (vecSort[i] < vecSort[j]) {
					int tmp = vecSort[i];
					vecSort[i] = vecSort[j];
					vecSort[j] = tmp;
				}
			}
		}

		// Delere item on the list //
		cv::Rect mergeRect;
		for (auto i = 0; i < vecSort.size(); i++) {
			if (i == 0) {
				mergeRect = m_pSelectPageForCNS->GetSelParaRect(vecSort[i]);
			}
			else {
				mergeRect = mergeRect | m_pSelectPageForCNS->GetSelParaRect(vecSort[i]);
			}
			m_pSelectPageForCNS->DeleteSelPara(vecSort[i]);
		}

			// Add a merge box as a new box //
		AddEditedLineBox(mergeRect);

		m_mapSelectionInfo.swap(std::map<int, _stLineTextSelectionInfo>());
		m_pSelectPageForCNS->SetSelMatchItem(-1);
	}
}





void CMNView::AddListToTraining()
{
	m_loadedImgCnt = 1;
	m_addImgCnt = 0;
	m_bIsThreadEnd = false;
//	pM->AddListToTraining(m_addImgCnt, m_loadedImgCnt, m_bIsThreadEnd);
	SINGLETON_DataMng::GetInstance()->ProcAddListToDB(m_addImgCnt, m_loadedImgCnt, m_bIsThreadEnd);
}







// Thread============//
static UINT ThreadListToTraining(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->AddListToTraining();
	return 0L;
}

static UINT ThreadDoExportDB(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoExportDB();
	return 0L;
}
static UINT ThreadDoExportDBHtml(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoExportDBHtml();
	return 0L;
}

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

static UINT ThreadDoSearchSegment(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoSearchSegment();
	return 0L;
}

static UINT ThreadCNSSegments(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoCNSSegments();
	return 0L;
}

static UINT ThreadDoExtraction(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	//	pClass->DoExtractBoundaryForSelected();
	pClass->DoExtractBoundaryAuto();
	return 0L;
}

static UINT ThreadDoOCR(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoOCR();
	return 0L;
}

static UINT ThreadDoOCRByMooN(LPVOID lpParam)
{
	CMNView* pClass = (CMNView*)lpParam;
	pClass->DoOCRFromMooN();
	return 0L;
}


