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
}


CMNPageObject::~CMNPageObject()
{
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
	strPath = _strpath;
	strPName = _strpname;
	strName = _strname;
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
	if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, pimg, false)) {
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

//	cv::Mat src;
	if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, m_thumbImg, false))
	{
		SetSize(m_thumbImg.cols, m_thumbImg.rows, DEFAULT_PAGE_SIZE);


		cv::cvtColor(m_thumbImg, m_srcGrayImg, CV_BGR2GRAY);
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

void CMNPageObject::DrawParagraph()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	if (m_paragraph.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(1);
		
		for (int i = 0; i < m_paragraph.size(); i++) {			

			glColor4f(m_paragraph[i].color.x, m_paragraph[i].color.y, m_paragraph[i].color.z, 0.5f);
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
	if (mid <= (int)m_matched_pos.size()) {
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

void CMNPageObject::AddParagraph(cv::Rect rect, _ALIGHN_TYPE type, float deskew)
{
	stParapgraphInfo para;
	para.deSkewAngle = deskew;
	para.deskewRect = rect;
	para.rect = rect;
	para.alignType = type;

	//if (type == _UNKNOWN_ALIGN) {
	//	mtSetPoint3D(&para.color, 0.5f, 0.5f, 0.5f);
	//}
	//else if (type == _HORIZON_ALIGN) {
		mtSetPoint3D(&para.color, 1.0f, 0.0f, 0.0f);
	//}
	//else {
	//	mtSetPoint3D(&para.color, 0.0f, 0.0f, 1.0f);
	//}

	m_paragraph.push_back(para);
}

void CMNPageObject::DeSkewImg()
{
	

	for (int i = 0; i < m_paragraph.size(); i++) {
		// Rotate Image //
		cv::Mat para = m_thumbImg(m_paragraph[i].rect);
		cv::Mat rotMat, rotatedFrame, invRot;
		rotMat = getRotationMatrix2D(cv::Point2f(0, 0), m_paragraph[i].deSkewAngle, 1);
		cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

		// Fill White color //
		//m_thumbImg(m_paragraph[i].rect).setTo(cv::Scalar(255,255,255));
		rotatedFrame.copyTo(m_thumbImg(m_paragraph[i].rect));
	}

//	m_thumbImg.setTo(cv::Scalar(255, 0, 0));

	UpdateTexture();
}

void CMNPageObject::UpdateTexture()
{
	if (m_texId > 0) {
		glDeleteTextures(1, &m_texId);
		m_texId = 0;
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