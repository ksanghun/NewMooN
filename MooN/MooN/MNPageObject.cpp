#include "stdafx.h"

#include "MNPageObject.h"
#include "MNDataManager.h"





CMNPageObject::CMNPageObject()
{
	m_thumbnailTexId = 0;
	m_texId = 0;

	mtSetPoint3D(&m_vBgColor, 0.3f, 0.7f, 0.9f);

	SetRendomPos();
	mtSetPoint3D(&m_targetPos, 0, 0, 0);

	m_fXScale = 1.0f;
	m_fYScale = 1.0f;

	//m_fImgCols = 0.0f;
	//m_fImgRows = 0.0f;

	SetSize(DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE);
	m_bCandidate = false;
	m_bIsSelected = false;
	m_bAniPos = false;

	m_bIsNear = false;
	m_bIsSearching = false;

	m_selMatchItemId = -1;
	m_IsNeedToSave = false;
}


CMNPageObject::~CMNPageObject()
{
	WritePageInfo();


	if (m_texId > 0) {
		glDeleteTextures(1, &m_texId);
	}


	ClearMatchResult();

	m_thumbImg.release();
	m_srcGrayImg.release();
}

void CMNPageObject::SetRendomPos()
{
	float fScale = 20.0f;
	m_pos.x = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
	m_pos.y = (rand() % MAX_CAM_HIGHTLEVEL*0.5f) - MAX_CAM_HIGHTLEVEL*0.5f*0.5f;
	m_pos.z = -((rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f);

	m_pos.x = m_pos.x*fScale;
	m_pos.y = m_pos.y*fScale;
	m_pos.z = m_pos.z*fScale - Z_TRANS;
}

void CMNPageObject::SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _code, unsigned long _pcode)
{
	m_strPath = _strpath;
	m_strPName = _strpname;
	m_strName = _strname;
	parentCode = _pcode;
	nCode = _code;
	//	mtSetPoint3D(&m_pos, 0.0f, 0.0f, 0.0f);
}

void CMNPageObject::SetSize(unsigned short _w, unsigned short _h, float _size)
{
	m_ImgRectSize.set(0, _w, 0, _h);


	m_nImgWidth = _w;
	m_nImgHeight = _h;
	m_fARatio = (float)_w / (float)_h;
	float w, h;

	if (_size > 0) {
		if (m_fARatio <= 1.0f) {
			w = _size*m_fARatio*0.45f;
			h = _size*0.45f;
		}
		else {
			w = _size*0.45f;
			h = (_size / m_fARatio)*0.45f;
		}
	}
	else {
		w = _w*0.5f;
		h = _h*0.5f;
	}

	m_fXScale = w*2.0f / (float)m_nImgWidth;
	m_fYScale = h*2.0f / (float)m_nImgHeight;

	mtSetPoint3D(&m_vertex[0], -w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[0], 0.0f, 1.0f);
	mtSetPoint3D(&m_vertex[1], w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[1], 1.0f, 1.0f);
	mtSetPoint3D(&m_vertex[2], w, h, 0.0f);			mtSetPoint2D(&m_texcoord[2], 1.0f, 0.0f);
	mtSetPoint3D(&m_vertex[3], -w, h, 0.0f);		mtSetPoint2D(&m_texcoord[3], 0.0f, 0.0f);


	mtSetPoint3D(&m_vertexBg[0], -_size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[1], _size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[2], _size*0.5f, _size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[3], -_size*0.5f, _size*0.5f, 0.0f);

//	m_fRectWidth = _size*fARatio;

	m_ImgPlaneSize.set(m_vertex[0].x, m_vertex[1].x, m_vertex[0].y, m_vertex[2].y);
}

GLuint CMNPageObject::ConvertGLTexSize(int _size)
{
	int size = 0;
	if (_size <= 64) {
		size = 64;
	}
	else if ((_size > 64) && (_size <= 128)) {
		size = 128;
	}
	else if ((_size > 128) && (_size <= 256)) {
		size = 256;
	}
	else if ((_size > 256) && (_size <= 512)) {
		size = 512;
	}
	else if ((_size > 512) && (_size <= 1024)) {
		size = 1024;
	}
	else {
		size = 2048;
	}
	return size;
}

GLuint CMNPageObject::LoadFullImage()
{
	if (m_texId != 0) {
		return 0;
	}
	cv::Mat pimg;
	if (SINGLETON_DataMng::GetInstance()->LoadImageData(m_strPath, pimg, false)) {
		// Save original size //
		m_nImgHeight = pimg.rows;
		m_nImgWidth = pimg.cols;
		// resize for texture //
		int w = ConvertGLTexSize(pimg.cols);
		int h = ConvertGLTexSize(pimg.rows);
		cv::resize(pimg, pimg, cvSize(w, h));

		glGenTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg.cols, pimg.rows, GL_RGB, GL_UNSIGNED_BYTE, pimg.ptr());
		pimg.release();
	}

//	SINGLETON_DataMng::GetInstance()->GetWordBoundaryByPageCode(nCode, m_wordBoundary);
	return 1;
}
bool CMNPageObject::LoadThumbImage(unsigned short resolution)
{
	//if (m_thumbnailTexId != 0) {
	//	return false;
	//}

	if (m_thumbImg.ptr() != NULL) {
		return false;
	}

	//==============================================
	CString strPath = m_strPath;
	CString str = PathFindExtension(m_strPath);
	if ((str == L".pdf") || (str == L".PDF")) {

		CString tmpStr = GetPInfoPath(L".jp2");
		if (PathFileExists(tmpStr)) {
			strPath = tmpStr;
		}
	}
	// If .jp2 exist, load .jp2 file instead of .pdf file //

	if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, m_thumbImg, false))
	{
		SetSize(m_thumbImg.cols, m_thumbImg.rows, DEFAULT_PAGE_SIZE);
		cv::cvtColor(m_thumbImg, m_srcGrayImg, CV_BGR2GRAY);

		LoadPageInfo();


		//cv::threshold(m_binaryImg, m_binaryImg, 125, 255, cv::THRESH_OTSU);
		//cv::bitwise_not(m_binaryImg, m_binaryImg);
		//// resizeing //
		//cv::resize(m_thumbImg, m_thumbImg, cvSize(resolution, resolution));

		// glupload Image - Thumnail image=======================================================//
		//glGenTextures(1, &m_thumbnailTexId);
		//glBindTexture(GL_TEXTURE_2D, m_thumbnailTexId);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, resolution, resolution, GL_RGB, GL_UNSIGNED_BYTE, m_thumbImg.data);

//		cv::imshow("LoadThumbImage", src);
//		src.release();
	}
	return false;
}

void CMNPageObject::UploadThumbImage()
{
	//if (m_thumbnailTexId != 0) {
	//	return;
	//}

	//glGenTextures(1, &m_thumbnailTexId);
	//glBindTexture(GL_TEXTURE_2D, m_thumbnailTexId);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_thumbImg.cols, m_thumbImg.rows, GL_RGB, GL_UNSIGNED_BYTE, m_thumbImg.data);
//	m_thumbImg.release();


	if (m_texId != 0) {
		return;
	}
	if (m_thumbImg.ptr()) {
		// Save original size //
		m_nImgHeight = m_thumbImg.rows;
		m_nImgWidth = m_thumbImg.cols;
		// resize for texture //
		int w = ConvertGLTexSize(m_thumbImg.cols);
		int h = ConvertGLTexSize(m_thumbImg.rows);
		cv::resize(m_thumbImg, m_thumbImg, cvSize(w, h));

		glGenTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_thumbImg.cols, m_thumbImg.rows, GL_RGB, GL_UNSIGNED_BYTE, m_thumbImg.ptr());

		cv::resize(m_thumbImg, m_thumbImg, cvSize(m_nImgWidth, m_nImgHeight));
	}

}

void CMNPageObject::RotatePos(float fSpeed)
{
	if (!m_bCandidate) {
		float fCos = cos(0.001);
		float fSin = sin(0.001);

		POINT3D tmpV = m_pos;
		tmpV.z += Z_TRANS;

		m_pos.x = tmpV.x*fCos - tmpV.z*fSin;
		m_pos.z = (tmpV.x*fSin + tmpV.z*fCos) - Z_TRANS;
	}
}
void CMNPageObject::AnimatePos(bool IsZvalue)
{
	float fDelta = SINGLETON_DataMng::GetInstance()->GetAniAcceration(m_nAniCnt);
	m_pos.x = m_pos.x + m_MoveVec.x*fDelta;
	m_pos.y = m_pos.y + m_MoveVec.y*fDelta;
	m_pos.z = m_pos.z + m_MoveVec.z*fDelta;
	m_nAniCnt++;
	if (m_nAniCnt >= ANI_FRAME_CNT) {
		m_bAniPos = false;
		if (IsZvalue) m_pos.z = 0.0f;
	}
}

void CMNPageObject::DrawThumbNail(float fAlpha)
{
	if (m_texId == 0) {
		return;
	}
	if (m_bCandidate) {
		fAlpha = 0.95f;
	}
	if (m_bAniPos) {
		AnimatePos(m_bCandidate);
	}
	glPointSize(3);
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);


	//// Background//
	//if (m_matched_pos.size() > 0) {
	////	glColor4f(0.3f, 0.7f, 1.0f, 0.3f);
	//	glColor4f(0.0f, 0.99f, 0.5f, 0.99f);
	//	glBegin(GL_LINE_STRIP);
	//	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	//	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	//	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	//	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	//	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	//	glEnd();
	//}	//Matched Page//


	glEnable(GL_TEXTURE_2D);
	//if (m_texId == 0) {		
	//	glBindTexture(GL_TEXTURE_2D, m_thumbnailTexId);
	//}
	//else {
		glBindTexture(GL_TEXTURE_2D, m_texId);
	//}


	glColor4f(1.0f, 1.0f, 1.0f, fAlpha);
	glBegin(GL_QUADS);
	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	
	glColor4f(m_vBgColor.x, m_vBgColor.y, m_vBgColor.z, 0.2f);
	if (m_bIsSelected) {
		glColor4f(m_vBgColor.x, m_vBgColor.y, m_vBgColor.z, 1.0f);
	}
	if (m_matched_pos.size() > 0) {
		glColor4f(0.0f, 0.99f, 0.0f, 0.99f);
	}	

	glBegin(GL_LINE_STRIP);
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glEnd();

	if (m_bIsSearching) {
		glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
		glBegin(GL_QUADS);
		glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
		glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
		glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
		glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
		glEnd();
	}



	// Show Result ==============================//
	glPointSize(1);
	glPopMatrix();
	//	glColor4f(0.3f, 0.7f, 0.9f, 0.7f);	

	glLineWidth(1);

}


float CMNPageObject::SetSelectionPosition(int nSlot, float xOffset, float yOffset, bool IsAni)
{
	m_bAniPos = IsAni;
	POINT3D targetPos;
	if (nSlot >= 0) {
		targetPos.x = xOffset;
		targetPos.y = yOffset;
		targetPos.z = 0.0f;


		if (m_bAniPos) {
			m_nAniCnt = 0;
			m_MoveVec = targetPos - m_pos;
		}
		else {
			m_pos = targetPos;
		}
		m_bCandidate = true;
		return DEFAULT_PAGE_SIZE + 2;
	}
	else {
		float fScale = 20.0f;
		targetPos.x = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
		targetPos.y = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
		targetPos.z = -((rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f);

		targetPos.x = targetPos.x*fScale;
		targetPos.y = targetPos.y*fScale;
		targetPos.z = targetPos.z*fScale - Z_TRANS;



		if (IsAni) {
			m_nAniCnt = 0;
			m_MoveVec = targetPos - m_pos;
		}
		else {
			m_pos = targetPos;
		}
		m_bCandidate = false;
		return 0.0f;
	}
}

void CMNPageObject::SetSelection(bool _isSel)
{
	if (_isSel == true) {
		mtSetPoint3D(&m_vBgColor, 0.99f, 0.0f, 0.0f);
	}
	else {
		mtSetPoint3D(&m_vBgColor, 0.3f, 0.7f, 0.9f);
	}
	m_bIsSelected = _isSel;
}

void CMNPageObject::SetSelMatchItem(int _selid)
{ 
	m_selMatchItemId = _selid; 
}

void CMNPageObject::DrawOCRResForPick()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glLineWidth(2);
	if (m_ocrResult.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(1);

		for (int i = 0; i < m_ocrResult.size(); i++) {
			glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
			glPushName(i + _PICK_WORD);
			glBegin(GL_QUADS);
			glVertex3f(m_ocrResult[i].rect.x, m_nImgHeight - m_ocrResult[i].rect.y, 0.0f);
			glVertex3f(m_ocrResult[i].rect.x, m_nImgHeight - (m_ocrResult[i].rect.y + m_ocrResult[i].rect.height), 0.0f);
			glVertex3f(m_ocrResult[i].rect.x + m_ocrResult[i].rect.width, m_nImgHeight - (m_ocrResult[i].rect.y + m_ocrResult[i].rect.height), 0.0f);
			glVertex3f(m_ocrResult[i].rect.x + m_ocrResult[i].rect.width, m_nImgHeight - m_ocrResult[i].rect.y, 0.0f);
			glEnd();
			glPopName();
		}
		glPopMatrix();
	}

	glPointSize(1);
	glPopMatrix();
	glLineWidth(1);
}
void CMNPageObject::DrawParagraph(int selid)
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glPointSize(2);
	if (m_paragraph.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(3);
		
		for (int i = 0; i < m_paragraph.size(); i++) {			

			glColor4f(0.5f, 0.5f, 1.0f, 0.5f);
			if(i==selid)
				glColor4f(0.1f, 0.2f, 1.0f, 0.99f);

			glBegin(GL_LINE_STRIP);
			//glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			glVertex3f(m_paragraph[i].rect.x,						          m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[i].rect.x,								  m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width,	  m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width,	  m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[i].rect.x,								  m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glEnd();
		}
		glPopMatrix();
	}

	glPointSize(1);
	glPopMatrix();
	glLineWidth(1);
}

void CMNPageObject::DrawSelectedParagraph(int selid)
{
	if (m_ocrResult.size() > 0) {
		return;
	}

	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	if (selid < m_paragraph.size()) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		int i = selid;
		glColor4f(1.0f, 1.0f, 0.0f, 0.3f);
		glBegin(GL_QUADS);
		glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
		glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
		glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
		glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
		glEnd();

		glPopMatrix();
	}
	glPopMatrix();
	glLineWidth(1);
}

void CMNPageObject::DrawParagraphForPick()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	if (m_paragraph.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		
		for (int i = 0; i < m_paragraph.size(); i++) {
			glPushName(i + _PICK_PARA);
			glBegin(GL_QUADS);
			glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glEnd();
			glPopName();
		}
		
		glPopMatrix();
	}
	glPopMatrix();
	glLineWidth(1);
}



void CMNPageObject::DrawMatchItem()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Show Result ==============================//
	glPointSize(8);
	if (m_matched_pos.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(3);
		for (int i = 0; i < m_matched_pos.size(); i++) {

			//	if (m_matched_pos[i].searchId == m_selMatchItemId) {
			if (i == m_selMatchItemId) {
				glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
				glBegin(GL_QUADS);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glEnd();
			}


			glColor4f(m_matched_pos[i].color.r, m_matched_pos[i].color.g, m_matched_pos[i].color.b, 0.5f);
			glBegin(GL_LINE_STRIP);
			//glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			glEnd();
		}
		glPopMatrix();
	}

	glPointSize(1);
	glPopMatrix();
	glLineWidth(1);
}


void CMNPageObject::DrawForPicking()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	glEnd();
	//==================//
	glPopMatrix();
}

void CMNPageObject::DrawMatchItemForPick()
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);


	if (m_matched_pos.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(3);
		for (int i = 0; i < m_matched_pos.size(); i++) {
			glPushName(m_matched_pos[i].searchId);
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			glBegin(GL_QUADS);
			glVertex3f(m_matched_pos[i].rect.x,									m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			glVertex3f(m_matched_pos[i].rect.x,									m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width,	m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width,	m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			glEnd();
			glPopName();

		}
		glPopMatrix();
	}

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

RECT2D CMNPageObject::ConvertVec3DtoImgateCoord(POINT3D v1, POINT3D v2)
{
	v1 = v1 - m_pos;
	v2 = v2 - m_pos;

	v1.x -= m_ImgPlaneSize.x1;
	v1.y -= m_ImgPlaneSize.y1;
	v2.x -= m_ImgPlaneSize.x1;
	v2.y -= m_ImgPlaneSize.y1;

	v1.y = m_ImgPlaneSize.height - v1.y;
	v2.y = m_ImgPlaneSize.height - v2.y;

	RECT2D selRect;
	if (v1.x < v2.x) {
		selRect.x1 = v1.x;		selRect.x2 = v2.x;
	}
	else {
		selRect.x1 = v2.x;		selRect.x2 = v1.x;
	}
	if (v1.y < v2.y) {
		selRect.y1 = v1.y;		selRect.y2 = v2.y;
	}
	else {
		selRect.y1 = v2.y;		selRect.y2 = v1.y;
	}

	if (selRect.x1 < 0)	selRect.x1 = 0;
	if (selRect.x2 > m_ImgPlaneSize.width - 1)	selRect.x2 = m_ImgPlaneSize.width - 1;
	if (selRect.y1 < 0)	selRect.y1 = 0;
	if (selRect.y2 > m_ImgPlaneSize.height - 1)	selRect.y2 = m_ImgPlaneSize.height - 1;


	selRect.width = selRect.x2 - selRect.x1;
	selRect.height = selRect.y2 - selRect.y1;

	if ((selRect.width == 0) || (selRect.height == 0)) {
		return RECT2D(0, 0, 0, 0);
	}

	float xScale = (float)m_nImgWidth / m_ImgPlaneSize.width;
	float yScale = (float)m_nImgHeight / m_ImgPlaneSize.height;

	selRect.x1 *= xScale;
	selRect.x2 *= xScale;
	selRect.y1 *= yScale;
	selRect.y2 *= yScale;

	selRect.width *= xScale;
	selRect.height *= yScale;

	return selRect;

}

bool CMNPageObject::IsDuplicate(stMatchInfo& info, int search_size)
{
	bool IsDup = false;
	for (int i = 0; i < m_matched_pos.size(); i++) {
		float fDist = mtDistance(info.pos, m_matched_pos[i].pos);
		if (fDist < search_size) {
			IsDup = true;
			if (info.accuracy > m_matched_pos[i].accuracy) {  // take more accurate one !!
				m_matched_pos[i] = info;
			}
		}
	}
	return IsDup;
}

bool CMNPageObject::AddMatchedPoint(stMatchInfo info, int search_size)
{
	if (search_size > 0) {
		if (IsDuplicate(info, search_size) == false) {
			m_matched_pos.push_back(info);
		}
	}
	else {
		m_matched_pos.push_back(info);
	}
	return true;
}

bool CMNPageObject::GetPosByMatchID(int mid, POINT3D& pos)
{
	mtSetPoint3D(&pos, 0.0f, 0.0f, 0.0f);
	if ((mid>=0) &&(mid <= (int)m_matched_pos.size())) {
		pos = m_matched_pos[mid].pos;

		pos.x = (pos.x - m_nImgWidth*0.5f)*m_fXScale + m_pos.x;
		pos.y = (m_nImgHeight - pos.y - m_nImgHeight*0.5f)*m_fYScale + m_pos.y;
		pos.z += m_pos.z;

		return true;
	}
	return false;
}

void CMNPageObject::ClearMatchResult()
{
	m_matched_pos.clear();
}

void CMNPageObject::ClearParagraph()
{
	m_paragraph.clear();
}

void CMNPageObject::ClearOCRResult()
{
	m_ocrResult.clear();
}

bool CMNPageObject::IsNeedToExtract()
{
	if (m_paragraph.size() == 0) {
		return true;
	}
	return false;
}
void CMNPageObject::AddParagraph(cv::Rect rect, bool IsHori, float deskew)
{
	m_IsNeedToSave = true;
	stParapgraphInfo para;
	para.deSkewAngle = deskew;
	para.rect = rect;
	para.IsHori = IsHori;
	para.IsDeskewed = false;

	//if (type == _UNKNOWN_ALIGN) {
	//	mtSetPoint3D(&para.color, 0.5f, 0.5f, 0.5f);
	//}
	//else if (type == _HORIZON_ALIGN) {
	//	mtSetPoint3D(&para.color, 1.0f, 0.0f, 0.0f);
	//}
	//else {
	//	mtSetPoint3D(&para.color, 0.0f, 0.0f, 1.0f);
	//}

	m_paragraph.push_back(para);
}

void CMNPageObject::DeSkewImg(int pid, float fAngle)
{
	m_IsNeedToSave = true;
	if ((pid < m_paragraph.size()) && (pid >= 0)) {

		if (m_paragraph[pid].IsDeskewed == false) {
			m_paragraph[pid].deSkewAngle = fAngle;
			cv::Mat para = m_thumbImg(m_paragraph[pid].rect);
			cv::Mat rotMat, rotatedFrame, invRot;
			rotMat = getRotationMatrix2D(cv::Point2f(para.cols*0.5f, para.rows*0.5f), m_paragraph[pid].deSkewAngle, 1);
			cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
			rotatedFrame.copyTo(m_thumbImg(m_paragraph[pid].rect));

			// Update Gray Scale Image//
			m_srcGrayImg.release();
			cv::cvtColor(m_thumbImg, m_srcGrayImg, CV_BGR2GRAY);

			m_paragraph[pid].IsDeskewed = true;
			rotatedFrame.release();		
		}
	}	
}

void CMNPageObject::UnDoDeSkewImg(int pid)
{
	if ((pid < m_paragraph.size()) && (pid >= 0)) {
		if (m_paragraph[pid].IsDeskewed) {
			cv::Mat para = m_thumbImg(m_paragraph[pid].rect);
			cv::Mat rotMat, rotatedFrame, invRot;
			rotMat = getRotationMatrix2D(cv::Point2f(para.cols*0.5f, para.rows*0.5f), -m_paragraph[pid].deSkewAngle, 1);
			cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
			rotatedFrame.copyTo(m_thumbImg(m_paragraph[pid].rect));

			m_paragraph[pid].IsDeskewed = false;
			rotatedFrame.release();
		}		
	}


	UpdateTexture();
}

void CMNPageObject::UpdateTexture()
{
	if (m_texId > 0) {
		glDeleteTextures(1, &m_texId);
		m_texId = 0;
	}

	if (m_thumbImg.ptr()) {

		//m_srcGrayImg.release();
		//cv::cvtColor(m_thumbImg, m_srcGrayImg, CV_BGR2GRAY);

		// Save original size //
		cv::Mat timg = m_thumbImg.clone();
		m_nImgHeight = m_thumbImg.rows;
		m_nImgWidth = m_thumbImg.cols;
		// resize for texture //
		int w = ConvertGLTexSize(timg.cols);
		int h = ConvertGLTexSize(timg.rows);
		cv::resize(timg, timg, cvSize(w, h));

		glGenTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, timg.cols, timg.rows, GL_RGB, GL_UNSIGNED_BYTE, timg.ptr());

	//	cv::resize(m_thumbImg, m_thumbImg, cvSize(m_nImgWidth, m_nImgHeight));
		timg.release();
	}
}

float CMNPageObject::GetDeskewParam(int pid)
{
	if (pid < m_paragraph.size()) {
		return m_paragraph[pid].deSkewAngle;
	}
	return 0;
}

void CMNPageObject::SetOCRResult(int _id, _stOCRResult _res)
{
	if ((_id >= 0) && (_id < m_ocrResult.size())) {
		m_ocrResult[_id] = _res;
	}
}

_stOCRResult CMNPageObject::GetOCRResult(int _id)
{
	if ((_id>=0) && (_id < m_ocrResult.size())) {
		return m_ocrResult[_id];
	}
	return _stOCRResult();
}

void CMNPageObject::DeleteSelPara(int selid)
{	
	m_IsNeedToSave = true;
	if ((selid < m_paragraph.size())&&(selid >=0)) {
		DeleteOCRResByRect(m_paragraph[selid].rect);
		m_paragraph.erase(m_paragraph.begin() + selid);
	}
}

void CMNPageObject::DeleteOCRResByRect(cv::Rect rect)
{
	std::vector<_stOCRResult>::iterator iter = m_ocrResult.begin();
	for (; iter != m_ocrResult.end();) {

		cv::Rect andRect_overlap = ((*iter).rect & rect);
		if (andRect_overlap.area() > 1) {
			iter = m_ocrResult.erase(iter);
		}
		else {
			++iter;
		}
	}
}


bool CMNPageObject::DeleteSelOCRRes(int selid)
{
	m_IsNeedToSave = true;
	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
		m_ocrResult.erase(m_ocrResult.begin() + selid);
		return true;
	}
	return false;
}

void CMNPageObject::ConfirmOCRRes(int selid)
{
	m_IsNeedToSave = true;
	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
		m_ocrResult[selid].fConfidence = 0.9f;
	}
}

void CMNPageObject::UpdateOCRCode(CString _strCode, int selid)
{
	m_IsNeedToSave = true;
	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
		m_ocrResult[selid].fConfidence = 0.9f;
		wsprintf(m_ocrResult[selid].strCode, _strCode);

	//	m_ocrResult[selid].strCode = _strCode;
	}
}

cv::Rect CMNPageObject::GetSelParaRect(int selid)
{
	if ((selid < m_paragraph.size()) && (selid >= 0)) {
		return m_paragraph[selid].rect;
	}
	return cv::Rect();
}

void CMNPageObject::AddOCRResult(_stOCRResult res)
{
	m_IsNeedToSave = true;
	m_ocrResult.push_back(res);
}

CString CMNPageObject::GetPInfoPath(CString strExtension)
{
	CString path;
	int nIndex = m_strPath.ReverseFind(_T('.'));
	if (nIndex > 0) {
		path = m_strPath.Left(nIndex);
	}
	path += strExtension;
	return path;
}


void CMNPageObject::LoadPageInfo()
{
	CString path = GetPInfoPath(L".pinfo");
	USES_CONVERSION;
	char* sz = T2A(path);

	FILE* fp = 0;
	fopen_s(&fp, sz, "rb");
	if (fp) {

		int pnum=0, wnum=0;
		fread(&wnum, sizeof(int), 1, fp);
		fread(&pnum, sizeof(int), 1, fp);
		


		for (int i = 0; i < wnum; i++) {
			_stOCRResult res;
			fread(&res, sizeof(_stOCRResult), 1, fp);
			m_ocrResult.push_back(res);
		}

		for (int i = 0; i < pnum; i++) {
			stParapgraphInfo data;
			fread(&data, sizeof(stParapgraphInfo), 1, fp);	
			m_paragraph.push_back(data);			
		}



		//char char_str[_MAX_WORD_SIZE] = { 0, };
		//wchar_t strUnicode[_MAX_WORD_SIZE] = { 0, };

		//fread(char_str, _MAX_WORD_SIZE, 1, fp);

		//// Multi to Unicode //
		//int nLen = MultiByteToWideChar(CP_ACP, 0, &char_str[0], strlen(char_str), NULL, NULL);		
		//MultiByteToWideChar(CP_ACP, 0, char_str, strlen(char_str), strUnicode, nLen);

		//CString strRead = CString(strUnicode);

		fclose(fp);
	}

	m_IsNeedToSave = false;
}


void CMNPageObject::WritePageInfo()
{
	if (m_IsNeedToSave) {
		CString path = GetPInfoPath(L".pinfo");
		USES_CONVERSION;
		char* sz =  T2A(path);

		FILE* fp = 0;
		fopen_s(&fp, sz, "wb");
		if (fp) {
			// Write header 8 byte: paragraph number, word number//
			int pnum = m_paragraph.size();
			int wnum = m_ocrResult.size();

			fwrite(&wnum, sizeof(int), 1, fp);
			fwrite(&pnum, sizeof(int), 1, fp);
			
			for (int i = 0; i < wnum; i++) {
				fwrite(&m_ocrResult[i], sizeof(_stOCRResult), 1, fp);
			}

			for (int i = 0; i < pnum; i++) {
				fwrite(&m_paragraph[i], sizeof(stParapgraphInfo), 1, fp);
			}

		//		wchar_t* wchar_str;
		//		char char_str[_MAX_WORD_SIZE];
		//		memset(char_str, 0x00, sizeof(char_str));
		//		int char_str_len;

		//		// 1. CString to wchar * conversion
		//		CString strTmp = L"TESTFILESAVEFUNCTIONîÜÔðÓðîÜ±è»óÈÆ";
		//		wchar_str = strTmp.GetBuffer(strTmp.GetLength());


		//		char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
		////		char_str = new char[char_str_len];
		//		// 2. wchar_t* to char* conversion
		//		WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0, 0);
		//		fwrite(char_str, _MAX_WORD_SIZE, 1, fp);
			fclose(fp);
		}	


		CString imgpath = GetPInfoPath(L".jp2");
		char* szimg = T2A(imgpath);
		cv::imwrite(szimg, m_thumbImg);
	}
}