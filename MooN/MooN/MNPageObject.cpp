#include "stdafx.h"

#include "MNPageObject.h"
#include "MNDataManager.h"
#include "Extractor.h"
#include "MainFrm.h"


CMNPageObject::CMNPageObject()
{
//	m_thumbnailTexId = 0;
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
	m_bImageChanged = false;
	m_IsTbimg = false;
	
}


CMNPageObject::~CMNPageObject()
{
//	WritePageInfo();
	UpdateDataBaseFiles();


	if (m_texId > 0) {
		glDeleteTextures(1, &m_texId);
	}


	ClearMatchResult();

	m_fullImg.release();
//	m_thumbImg.release();
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
	m_nCode = _code;


	//	mtSetPoint3D(&m_pos, 0.0f, 0.0f, 0.0f);
	// Check data filder //
	CString path;
	path = m_strPName + L"\\moon_db";
	if (PathFileExists(path) == 0) {
		CreateDirectory(path, NULL);
	}
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

	CString strPath = m_strPath;
	CString str = PathFindExtension(m_strPath);
	CString tmpStr = GetPInfoPath(L".jp2");

	m_bImageChanged = true;
	if (PathFileExists(tmpStr)) {
		strPath = tmpStr;
		m_bImageChanged = false;
	}

	cv::Mat pimg;
	if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, m_fullImg, false)) {

		cv::cvtColor(m_fullImg, m_srcGrayImg, CV_BGR2GRAY);
		cv::threshold(m_srcGrayImg, m_srcGrayImg, 125, 255, cv::THRESH_OTSU);

		// Save original size //
		m_nImgHeight = m_fullImg.rows;
		m_nImgWidth = m_fullImg.cols;
		// resize for texture //
		int w = ConvertGLTexSize(m_fullImg.cols);
		int h = ConvertGLTexSize(m_fullImg.rows);
		cv::resize(m_fullImg, pimg, cvSize(w, h));


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
	if (m_fullImg.ptr() != NULL) {
		return false;
	}

	//==============================================
	CString strPath = m_strPath;
	CString str = PathFindExtension(m_strPath);

	CString tmpStr = GetPInfoPath(L".jp2");
	if (PathFileExists(tmpStr)) {
		strPath = tmpStr;
		m_IsTbimg = true;
	}

	// If .jp2 exist, load .jp2 file instead of .pdf file //
	if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, m_fullImg, false))
	{
		cv::cvtColor(m_fullImg, m_srcGrayImg, CV_BGR2GRAY);
		cv::threshold(m_srcGrayImg, m_srcGrayImg, 100, 255, cv::THRESH_OTSU);

		unsigned short width = 0, height = 0;
		LoadPageInfo(width, height);
		//if (LoadPageInfo(width, height)) {
		//	SetSize(width, height, DEFAULT_PAGE_SIZE);
		//}
		//else {
			SetSize(m_fullImg.cols, m_fullImg.rows, DEFAULT_PAGE_SIZE);
		//	cv::resize(m_thumbImg, m_thumbImg, cvSize(resolution, resolution));
		//}
	}
	return false;


	//if (m_thumbImg.ptr() != NULL) {
	//	return false;
	//}

	////==============================================
	//CString strPath = m_strPath;
	//CString str = PathFindExtension(m_strPath);

	//CString tmpStr = GetPInfoPath(L"_tb.jp2");
	//if (PathFileExists(tmpStr)) {
	//	strPath = tmpStr;
	//	m_IsTbimg = true;
	//}

	//// If .jp2 exist, load .jp2 file instead of .pdf file //
	//if (SINGLETON_DataMng::GetInstance()->LoadImageData(strPath, m_thumbImg, false))
	//{
	//	unsigned short width = 0, height = 0;
	//	if (LoadPageInfo(width, height)) {
	//		SetSize(width, height, DEFAULT_PAGE_SIZE);
	//	}
	//	else {
	//		SetSize(m_thumbImg.cols, m_thumbImg.rows, DEFAULT_PAGE_SIZE);
	//		cv::resize(m_thumbImg, m_thumbImg, cvSize(resolution, resolution));
	//	}		
	//}
	//return false;
}

void CMNPageObject::UploadThumbImage()
{
	//if (m_thumbImg.ptr()) {
	//	glGenTextures(1, &m_thumbnailTexId);
	//	glBindTexture(GL_TEXTURE_2D, m_thumbnailTexId);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

	//	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_thumbImg.cols, m_thumbImg.rows, GL_RGB, GL_UNSIGNED_BYTE, m_thumbImg.data);
	//}
//	m_thumbImg.release();





//	cv::imshow("binary", m_srcGrayImg);

	if (m_texId != 0) {
		return;
	}
	if (m_fullImg.ptr()) {
		// Save original size //
		m_nImgHeight = m_fullImg.rows;
		m_nImgWidth = m_fullImg.cols;
		// resize for texture //
		int w = ConvertGLTexSize(m_fullImg.cols);
		int h = ConvertGLTexSize(m_fullImg.rows);

		cv::Mat pimg;
		cv::resize(m_fullImg, pimg, cvSize(w, h));

		glGenTextures(1, &m_texId);
		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg.cols, pimg.rows, GL_RGB, GL_UNSIGNED_BYTE, pimg.ptr());

		pimg.release();
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
	////	glBindTexture(GL_TEXTURE_2D, m_thumbnailTexId);
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
		glColor4f(1.0f, 1.0f, 0.0f, 0.3f);
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
		mtSetPoint3D(&m_vBgColor, 0.99f, 0.99f, 0.0f);
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

void CMNPageObject::GetIDbyUUID(unsigned int uuid, int&lineid, int&objid)
{
	for (auto j = 0; j < m_paragraph.size(); j++) {
		for (auto i = 0; i < m_paragraph[j].vecTextBox.size(); i++) {

			if (m_paragraph[j].vecTextBox[i].uuid == uuid) {
				lineid = j;
				objid = i;
				return;
			}
		}
	}
}

unsigned int CMNPageObject::GetUUIDbyLineObjId(int lineid, int objid)
{
	if ((lineid < m_paragraph.size()) && (objid < m_paragraph[lineid].vecTextBox.size())) {
		return m_paragraph[lineid].vecTextBox[objid].uuid;
	}
	return 0;
}



void CMNPageObject::DrawOCRResForPick()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);
	glLineWidth(1);

	for (auto j = 0; j < m_paragraph.size(); j++) {
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

//		int pid = j * 10000 + _PICK_WORD; // !!

		for (auto i = 0; i < m_paragraph[j].vecTextBox.size(); i++) {
			glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
//			glPushName(pid + i);
			glPushName(m_paragraph[j].vecTextBox[i].uuid + _PICK_WORD);
			glBegin(GL_QUADS);
			glVertex3f(m_paragraph[j].vecTextBox[i].rect.x, m_nImgHeight - m_paragraph[j].vecTextBox[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[j].vecTextBox[i].rect.x, m_nImgHeight - (m_paragraph[j].vecTextBox[i].rect.y + m_paragraph[j].vecTextBox[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[j].vecTextBox[i].rect.x + m_paragraph[j].vecTextBox[i].rect.width, m_nImgHeight - (m_paragraph[j].vecTextBox[i].rect.y + m_paragraph[j].vecTextBox[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[j].vecTextBox[i].rect.x + m_paragraph[j].vecTextBox[i].rect.width, m_nImgHeight - m_paragraph[j].vecTextBox[i].rect.y, 0.0f);
			glEnd();
			glPopName();
		}

		glPopMatrix();
	}

	glPopMatrix();


	//if (m_ocrResult.size() > 0) {
	//	// Draw detected position //
	//	glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
	//	glPushMatrix();
	//	glScalef(m_fXScale, m_fYScale, 1.0f);
	//	glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

	//	//if (m_bIsNear){		
	//	glLineWidth(1);

	//	for (int i = 0; i < m_ocrResult.size(); i++) {
	//		glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
	//		glPushName(i + _PICK_WORD);
	//		glBegin(GL_QUADS);
	//		glVertex3f(m_ocrResult[i].rect.x, m_nImgHeight - m_ocrResult[i].rect.y, 0.0f);
	//		glVertex3f(m_ocrResult[i].rect.x, m_nImgHeight - (m_ocrResult[i].rect.y + m_ocrResult[i].rect.height), 0.0f);
	//		glVertex3f(m_ocrResult[i].rect.x + m_ocrResult[i].rect.width, m_nImgHeight - (m_ocrResult[i].rect.y + m_ocrResult[i].rect.height), 0.0f);
	//		glVertex3f(m_ocrResult[i].rect.x + m_ocrResult[i].rect.width, m_nImgHeight - m_ocrResult[i].rect.y, 0.0f);
	//		glEnd();
	//		glPopName();
	//	}
	//	glPopMatrix();
	//}

	//glPointSize(1);
	//glPopMatrix();
	//glLineWidth(1);
}
void CMNPageObject::DrawParagraph(int selid, bool IsMergeMode)
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glLineWidth(4);
	if (m_paragraph.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		if (m_bIsNear) {
			glEnable(GL_LINE_STIPPLE);
		//	for (auto i = 0; i < m_paragraph.size(); i++) {

			//	glColor4f(0.99f, 0.0f, 0.0f, 0.3f);
			//	if (i == selid) {
			int i = selid;
			glColor4f(0.f, 0.99f, 0.0f, 0.99f);
			if(IsMergeMode)
				glColor4f(0.99f, 0.0f, 0.0f, 0.99f);

			glLineStipple(2, 0xAAAA);
			glBegin(GL_LINE_STRIP);
			//glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - (m_paragraph[i].rect.y + m_paragraph[i].rect.height), 0.0f);
			glVertex3f(m_paragraph[i].rect.x + m_paragraph[i].rect.width, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glVertex3f(m_paragraph[i].rect.x, m_nImgHeight - m_paragraph[i].rect.y, 0.0f);
			glEnd();
		//		}

		//	}
			glDisable(GL_LINE_STIPPLE);			
		}
		glPopMatrix();
	}

	glPointSize(1);
	glPopMatrix();
	glLineWidth(1);
}

void CMNPageObject::DrawSelectedParagraph(int selid)
{
	//if (m_ocrResult.size() > 0) {
	//	return;
	//}

	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

//	if (selid < m_paragraph.size()) {
	for(auto i=0; i<m_paragraph.size(); i++){
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

	//	int i = selid;
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
				glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
				glBegin(GL_QUADS);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glEnd();
			}
			else {
				glColor4f(m_matched_pos[i].color.r, m_matched_pos[i].color.g, m_matched_pos[i].color.b, m_matched_pos[i].color.a);
				glBegin(GL_QUADS);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
				glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
				glEnd();
			}

			//if (m_matched_pos[i].lineid < 0) {
			//	glColor4f(m_matched_pos[i].color.r, m_matched_pos[i].color.g, m_matched_pos[i].color.b, 0.5f);
			//	glBegin(GL_LINE_STRIP);
			//	//glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			//	glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			//	glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			//	glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - (m_matched_pos[i].rect.y + m_matched_pos[i].rect.height), 0.0f);
			//	glVertex3f(m_matched_pos[i].rect.x + m_matched_pos[i].rect.width, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			//	glVertex3f(m_matched_pos[i].rect.x, m_nImgHeight - m_matched_pos[i].rect.y, 0.0f);
			//	glEnd();
			//}
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
//	if (selRect.x2 > m_ImgPlaneSize.width - 1)	selRect.x2 = m_ImgPlaneSize.width - 1;

	if (selRect.x2 > m_ImgPlaneSize.width)	
		selRect.x2 = m_ImgPlaneSize.width;

	if (selRect.y1 < 0)	selRect.y1 = 0;
//	if (selRect.y2 > m_ImgPlaneSize.height - 1)	selRect.y2 = m_ImgPlaneSize.height - 1;
	if (selRect.y2 > m_ImgPlaneSize.height)	
		selRect.y2 = m_ImgPlaneSize.height;


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

		cv::Rect andRect_overlap = (info.rect & m_matched_pos[i].rect);
		if (andRect_overlap.area() > 1) {		// intersected
		//float fDist = mtDistance(info.pos, m_matched_pos[i].pos);
		//if (fDist < search_size) {
			IsDup = true;
			if (info.accuracy > m_matched_pos[i].accuracy) {  // take more accurate one !!
				m_matched_pos[i] = info;
			}
		}
	}
	return IsDup;
}

void CMNPageObject::SetFitCurArea()
{
	for (int i = 0; i < m_matched_pos.size(); i++) {
		cv::Rect rect = m_matched_pos[i].rect;
		SINGLETON_DataMng::GetInstance()->FitCutImageRect(m_srcGrayImg, rect);
		m_matched_pos[i].rect = rect;
	}

}

bool CMNPageObject::AddMatchedPoint(stMatchInfo info, int search_size)
{
	//if ((info.lineid >= 0) && (info.objid >= 0)) {  // Update previous textBox;
	//	m_paragraph[info.lineid].vecTextBox[info.objid].fConfidence = info.accuracy;
	//}

	//else {
		if (search_size > 0) {
			if (IsDuplicate(info, search_size) == false) {
				m_matched_pos.push_back(info);
			}
		}
		else {
			m_matched_pos.push_back(info);
		}
//	}
	return true;
}

bool CMNPageObject::GetRectByMatchID(int mid, cv::Rect& rect, float& fConf)
{
	rect = cv::Rect();
	if ((mid >= 0) && (mid < (int)m_matched_pos.size())) {
		rect = m_matched_pos[mid].rect;
		fConf = m_matched_pos[mid].accuracy;
		return true;
	}
	return false;
}

bool CMNPageObject::GetPosByMatchID(int mid, POINT3D& pos)
{
	mtSetPoint3D(&pos, 0.0f, 0.0f, 0.0f);
	if ((mid>=0) &&(mid < (int)m_matched_pos.size())) {
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
//	m_matched_pos.clear();
	m_matched_pos.swap(std::vector<stMatchInfo>());
}

void CMNPageObject::ClearParagraph()
{
	m_IsNeedToSave = true;

	//for (auto i = 0; i < m_paragraph.size(); i++) {
	//	m_paragraph[i].vecTextBox.clear();
	//}
	m_paragraph.swap(std::vector<stParapgraphInfo>());
//	m_ocrResult.clear();
}

void CMNPageObject::DeleteAllOcrRes()
{
	m_IsNeedToSave = true;
//	m_ocrResult.clear();
	for (auto i = 0; i < m_paragraph.size(); i++) {
	//	m_paragraph[i].vecTextBox.clear();
		m_paragraph[i].vecTextBox.swap(std::vector<_stOCRResult>());
	}
}

void CMNPageObject::DeleteAllOcrResInLine(int lineid)
{
	m_IsNeedToSave = true;
	//	m_ocrResult.clear();
	if((lineid < m_paragraph.size()) && (lineid>=0)){
		m_paragraph[lineid].vecTextBox.swap(std::vector<_stOCRResult>());
	}
}

void CMNPageObject::ClearOCRResult()
{
	m_IsNeedToSave = true;
//	m_ocrResult.clear();
	for (auto j = 0; j < m_paragraph.size(); j++) {
		for (auto i = 0; i < m_paragraph[j].vecTextBox.size(); i++) {
			m_paragraph[j].vecTextBox[i].fConfidence = 0.0f;
			//		m_ocrResult[i].strCode 
			memset(&m_paragraph[j].vecTextBox[i].strCode, 0x00, sizeof(wchar_t)*_MAX_WORD_SIZE);
			wsprintf(m_paragraph[j].vecTextBox[i].strCode, L"");
		}
	}
}

bool CMNPageObject::IsNeedToExtract()
{
	if (m_paragraph.size() == 0) {
		return true;
	}
	return false;
}

void CMNPageObject::AddParagraph(CExtractor& extractor, cv::Mat& paraImg, cv::Rect rect, bool IsVerti, float deskew, bool IsAlphabetic)
{
	//if ((IsVerti) && (rect.width < 32)) return;
	//if ((!IsVerti) && (rect.height < 32)) return;

	for (int j = 0; j < m_paragraph.size(); j++) {
		cv::Rect andRect_overlap = (m_paragraph[j].rect & rect);
		if (andRect_overlap.area() >= (rect.area()*0.75f)) { // Duplicated 
			return;
		}
	}


	m_IsNeedToSave = true;
	stParapgraphInfo para;
	para.init();
	para.deSkewAngle = deskew;
	para.rect = rect;
	para.IsVerti = IsVerti;

	m_paragraph.push_back(para);
	int lineid = static_cast<int>(m_paragraph.size() - 1);

	// Extract text boundary //
	std::vector<_extractBox> vecBox;
	if (IsVerti) {
		extractor.ExtractionText(paraImg, 0, -4, vecBox, IsVerti);
		//extractor.ExtractionText(paraImg, 8, 0, vecBox);
		//// Sort //
		////extractor.SortBoundaryBox(vecBox);
		//extractor.ExtractionText(paraImg, 0, 4, vecBox);

	}
	else {
		extractor.ExtractionText(paraImg, 0, 2, vecBox, IsVerti);
	}
		
	for (auto i = 0; i < vecBox.size(); i++) {
		if (vecBox[i].textbox.area() > 4) {
			_stOCRResult res;
			res.init();
			res.rect = vecBox[i].textbox;
			res.rect.x += rect.x;
			res.rect.y += rect.y;
			AddOCRResult(lineid, res);
		}
	}

	//===========================//
}


void CMNPageObject::RemoveNoise(cv::Rect rect)
{
	if (m_fullImg.ptr()) {
		m_bImageChanged = true;
		m_fullImg(rect).setTo(cv::Scalar(255, 255, 255));
	}

	UpdateTexture(m_fullImg);
}
void CMNPageObject::DeSkewImg(int pid, float fAngle)
{
	if (m_fullImg.ptr()) {
		m_bImageChanged = true;
		if ((pid < m_paragraph.size()) && (pid >= 0)) {

			if (m_paragraph[pid].IsDeskewed == false) {
				m_paragraph[pid].deSkewAngle = fAngle;
				cv::Mat para = m_fullImg(m_paragraph[pid].rect);
				cv::Mat rotMat, rotatedFrame, invRot;
				rotMat = getRotationMatrix2D(cv::Point2f(para.cols*0.5f, para.rows*0.5f), m_paragraph[pid].deSkewAngle, 1);
				cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

				//			cv::addWeighted(rotatedFrame, 1.5, rotatedFrame, -0.5, 0, rotatedFrame);
				rotatedFrame.copyTo(m_fullImg(m_paragraph[pid].rect));

				//			cv::filter2D()
				m_paragraph[pid].IsDeskewed = true;
				rotatedFrame.release();
			}
		}
	}

	UpdateTexture(m_fullImg);
}

void CMNPageObject::UnDoDeSkewImg(int pid)
{
	if ((pid < m_paragraph.size()) && (pid >= 0)) {
		if (m_paragraph[pid].IsDeskewed) {
			cv::Mat para = m_fullImg(m_paragraph[pid].rect);
			cv::Mat rotMat, rotatedFrame, invRot;
			rotMat = getRotationMatrix2D(cv::Point2f(para.cols*0.5f, para.rows*0.5f), -m_paragraph[pid].deSkewAngle, 1);
			cv::warpAffine(para, rotatedFrame, rotMat, para.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
			rotatedFrame.copyTo(m_fullImg(m_paragraph[pid].rect));

			m_paragraph[pid].IsDeskewed = false;
			rotatedFrame.release();
		}		
	}

	UpdateTexture(m_fullImg);
}

void CMNPageObject::UpdateTexture(cv::Mat& texImg)
{
	if (m_texId > 0) {
		glDeleteTextures(1, &m_texId);
		m_texId = 0;
	}

	if (texImg.ptr()) {
		m_srcGrayImg.release();
		cv::cvtColor(m_fullImg, m_srcGrayImg, CV_BGR2GRAY);
		cv::threshold(m_srcGrayImg, m_srcGrayImg, 128, 255, cv::THRESH_OTSU);

		// Save original size //
		cv::Mat timg = texImg.clone();
		m_nImgHeight = texImg.rows;
		m_nImgWidth = texImg.cols;
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

//float CMNPageObject::GetDeskewParam(int pid)
//{
//	if (pid < m_paragraph.size()) {
//		return m_paragraph[pid].deSkewAngle;
//	}
//	return 0;
//}

void CMNPageObject::SetOCRResult(int _lineid, int _id, _stOCRResult _res)
{
	if ((_lineid < m_paragraph.size()) && (_id >= 0) && (_id < m_paragraph[_lineid].vecTextBox.size())) {
	//	m_ocrResult[_id] = _res;
		m_paragraph[_lineid].vecTextBox[_id] = _res;
	}
}

_stOCRResult CMNPageObject::GetOCRResult(int _lineid, int _id)
{
//	if ((_id>=0) && (_id < m_ocrResult.size())) {
	if ((_lineid < m_paragraph.size()) && (_id >= 0) && (_id < m_paragraph[_lineid].vecTextBox.size())) {
	//	return m_ocrResult[_id];
		return m_paragraph[_lineid].vecTextBox[_id];
	}
	return _stOCRResult();
}

void CMNPageObject::DeleteSelPara(int selid)
{	
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	m_IsNeedToSave = true;
	if ((selid < m_paragraph.size())&&(selid >=0)) {
	//	DeleteOCRResByRect(m_paragraph[selid].rect);
		for (auto i = 0; i < m_paragraph[selid].vecTextBox.size(); i++) {
	//		int id = selid * 10000 + i;
			pM->DeleteMatchList(m_paragraph[selid].vecTextBox[i].uuid);
		}
		m_paragraph[selid].vecTextBox.swap(std::vector<_stOCRResult>());
		m_paragraph.erase(m_paragraph.begin() + selid);
	}
}

void CMNPageObject::DeleteOCRResByRect(cv::Rect rect)
{

	for (auto i = 0; i < m_paragraph.size(); i++) {

//		std::vector<_stOCRResult>::iterator iter = m_ocrResult.begin();
		std::vector<_stOCRResult>::iterator iter = m_paragraph[i].vecTextBox.begin();
		for (; iter != m_paragraph[i].vecTextBox.end();) {

			cv::Rect andRect_overlap = ((*iter).rect & rect);
			if (andRect_overlap.area() > 1) {
				iter = m_paragraph[i].vecTextBox.erase(iter);
			}
			else {
				++iter;
			}
		}
	}
}

void CMNPageObject::CleanUpOCRres()
{
	for (auto i = 0; i < m_paragraph.size(); i++) {
		std::vector<_stOCRResult>::iterator iter = m_paragraph[i].vecTextBox.begin();
		for (; iter != m_paragraph[i].vecTextBox.end();) {
			if (iter->type == 100) {
				iter = m_paragraph[i].vecTextBox.erase(iter);
			}
			else {
				++iter;
			}
		}
	}
}


unsigned int CMNPageObject::DeleteSelOCRRes(int lid, int selid)
{
	m_IsNeedToSave = true;
	unsigned int uuid = 0;
//	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
	if ((lid < m_paragraph.size()) && (selid < m_paragraph[lid].vecTextBox.size()) && (selid >= 0)) {
//		m_ocrResult.erase(m_ocrResult.begin() + selid);
		uuid = m_paragraph[lid].vecTextBox[selid].uuid;
		m_paragraph[lid].vecTextBox.erase(m_paragraph[lid].vecTextBox.begin() + selid);
	}
	return uuid;
}

void CMNPageObject::ConfirmOCRRes(int lid, int selid)
{
	m_IsNeedToSave = true;
//	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
	if ((lid < m_paragraph.size()) && (selid < m_paragraph[lid].vecTextBox.size()) && (selid >= 0)) {
		m_paragraph[lid].vecTextBox[selid].fConfidence = 0.9f;
		m_paragraph[lid].vecTextBox[selid].bNeedToDB = true;

		//m_ocrResult[selid].fConfidence = 0.9f;
		//m_ocrResult[selid].bNeedToDB = true;
	}
}

void CMNPageObject::UpdateOCRResStatus(int lid, int selid, bool IsUpdate, int _type)
{
	m_IsNeedToSave = true;

	if ((lid < m_paragraph.size()) && (selid < m_paragraph[lid].vecTextBox.size()) && (selid >= 0)) {
		m_paragraph[lid].vecTextBox[selid].bNeedToDB = IsUpdate;
		m_paragraph[lid].vecTextBox[selid].type = _type;
	}

	//if ((selid < m_ocrResult.size()) && (selid >= 0)) {
	//	m_ocrResult[selid].bNeedToDB = IsUpdate;
	//	m_ocrResult[selid].type = _type;
	//}
}


void CMNPageObject::UpdateLineStatus(int _id, bool IsCNS)
{
	if ((_id >= 0) && (_id < m_paragraph.size())) {
		m_paragraph[_id].IsCNSed = IsCNS;
	}
}


bool CMNPageObject::UpdateOCRCode(CString _strCode, float _fConfi, int lid, int selid)
{
	m_IsNeedToSave = true;
//	if ((selid < m_ocrResult.size()) && (selid >= 0)) {
	if ((lid < m_paragraph.size()) && (selid < m_paragraph[lid].vecTextBox.size()) && (selid >= 0)) {
		m_paragraph[lid].vecTextBox[selid].fConfidence = _fConfi;
		m_paragraph[lid].vecTextBox[selid].bNeedToDB = true;
		wsprintf(m_paragraph[lid].vecTextBox[selid].strCode, _strCode);

		char char_str[_MAX_WORD_SIZE * 2];
		memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
		int char_str_len = WideCharToMultiByte(CP_ACP, 0, m_paragraph[lid].vecTextBox[selid].strCode, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, m_paragraph[lid].vecTextBox[selid].strCode, -1, char_str, char_str_len, 0, 0);
		m_paragraph[lid].vecTextBox[selid].hcode_s = getHashCode(char_str);

//		m_paragraph[lid].vecTextBox[selid].strCode = _strCode;
		return true;
	}
	return false;
}



cv::Rect CMNPageObject::GetSelParaRect(int selid)
{
	if ((selid < m_paragraph.size()) && (selid >= 0)) {
		return m_paragraph[selid].rect;
	}
	return cv::Rect();
}

void CMNPageObject::ClearDBSearchResult()
{
//	m_sdbResult.clear();
	m_sdbResult.swap(std::vector<stDBSearchRes>());
}
void CMNPageObject::AddDBSearchResult(cv::Rect _rect)
{
	stDBSearchRes res;
	res.rect = _rect;
	m_sdbResult.push_back(res);
}



void CMNPageObject::AddOCRResult(int lineid, _stOCRResult res)
{
	// !!!!!need to dup check //
	m_IsNeedToSave = true;

	char char_str[_MAX_WORD_SIZE * 2];
	memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
	int char_str_len = WideCharToMultiByte(CP_ACP, 0, res.strCode, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, res.strCode, -1, char_str, char_str_len, 0, 0);

//	if (lineid < m_paragraph.size()) {
	if (lineid == -1) {  // From CNS
		for (auto i = 0; i < m_paragraph.size(); i++) {
			cv::Rect andRect_overlap = (m_paragraph[i].rect & res.rect);
			if (andRect_overlap.area() >= (res.rect.area()*0.75f)) {
				bool IsUpdated = false;
				for (int j = 0; j < m_paragraph[i].vecTextBox.size(); j++) {
					andRect_overlap = (m_paragraph[i].vecTextBox[j].rect & res.rect);
					if (andRect_overlap.area() >= (res.rect.area()*0.75f)) { // Duplicated !! Update		// Update //
						m_paragraph[i].vecTextBox[j].fConfidence = res.fConfidence;
						memcpy(m_paragraph[i].vecTextBox[j].strCode, res.strCode, sizeof(wchar_t)*_MAX_WORD_SIZE);
						IsUpdated = true;
						break;
					}
				}
				if (IsUpdated == false) {		// Add new
					res.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
					m_paragraph[i].vecTextBox.push_back(std::move(res));
				}
			}
		}
	}
	else {		// And new //
		if (lineid < m_paragraph.size()) {
			res.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
			m_paragraph[lineid].vecTextBox.push_back(std::move(res));
		}
	}
//	}
	

	//if ((lineid < m_paragraph.size()) && (lineid >= 0)) {
	//	res.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
	//	m_paragraph[lineid].vecTextBox.push_back(std::move(res));
	//}
	//else {  // Find line if by rect, line id is -1, unknown cut 

	//	for (auto i = 0; i < m_paragraph.size(); i++) {
	//		cv::Rect andRect_overlap = (m_paragraph[i].rect & res.rect);
	//		if (andRect_overlap.area() >= (res.rect.area()*0.75f)) {

	//			for (int j = 0; j < m_paragraph[i].vecTextBox.size(); j++) {
	//				andRect_overlap = (m_paragraph[i].vecTextBox[j].rect & res.rect);
	//				if (andRect_overlap.area() >= (res.rect.area()*0.75f)) { // Update		// Update //
	//				}
	//				else {		// Add new
	//					res.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
	//					m_paragraph[i].vecTextBox.push_back(std::move(res));
	//				}
	//			}				
	//			break;
	//		}
	//	}		
	//}

//	res.hcode = getHashCode(char_str);
//	m_ocrResult.push_back(res);
}

CString CMNPageObject::GetPInfoPath(CString strExtension)
{
	CString path, filename;
	int nIndex = m_strName.ReverseFind(_T('.'));
	if (nIndex > 0) {
		filename = m_strName.Left(nIndex);
	}
//	filename += strExtension;	
	path = m_strPName + L"\\moon_db\\" + filename + strExtension;
	return path;
}


bool CMNPageObject::LoadPageInfo(unsigned short& width, unsigned short& height)
{
	m_IsNeedToSave = true;
	bool IsLoad = false;
	CString path = GetPInfoPath(L".pinfo");
	USES_CONVERSION;
	char* sz = T2A(path);

	FILE* fp = 0;
	fopen_s(&fp, sz, "rb");
	if (fp) {
		int pnum=0, wnum=0;
		fread(&width, sizeof(unsigned short), 1, fp);
		fread(&height, sizeof(unsigned short), 1, fp);
		fread(&pnum, sizeof(int), 1, fp);
		
		for (int i = 0; i < pnum; i++) {
			stParapgraphInfo data;
			data.init();
			fread(&data.rect, sizeof(cv::Rect), 1, fp);
			fread(&data.IsVerti, sizeof(bool), 1, fp);

			fread(&wnum, sizeof(int), 1, fp);

			for (int j = 0; j < wnum; j++) {
				_stOCRResult textbox;
				fread(&textbox, sizeof(_stOCRResult), 1, fp);
				textbox.uuid = SINGLETON_DataMng::GetInstance()->GetUUID();
				data.vecTextBox.push_back(std::move(textbox));
			}
			m_paragraph.push_back(std::move(data));
		}

	
		//char char_str[_MAX_WORD_SIZE] = { 0, };
		//wchar_t strUnicode[_MAX_WORD_SIZE] = { 0, };

		//fread(char_str, _MAX_WORD_SIZE, 1, fp);
		//// Multi to Unicode //
		//int nLen = MultiByteToWideChar(CP_ACP, 0, &char_str[0], strlen(char_str), NULL, NULL);		
		//MultiByteToWideChar(CP_ACP, 0, char_str, strlen(char_str), strUnicode, nLen);

		//CString strRead = CString(strUnicode);
		m_IsNeedToSave = false;
		IsLoad = true;
		fclose(fp);
	}
	
	// Build word table //
	//for (auto i = 0; i < m_ocrResult.size(); i++) {
	//	SINGLETON_DataMng::GetInstance()->AddSDBTable(m_ocrResult[i].hcode, m_ocrResult[i].strCode);
	//}
	
	return IsLoad;
}

void CMNPageObject::WriteSearchDBFile()
{
	int wnum = 0;
	for (auto i = 0; i < m_paragraph.size(); i++) {
		wnum += static_cast<int>(m_paragraph[i].vecTextBox.size());
	}

	std::map<unsigned int, _stSDB> mapSDB;

//	int wnum = m_ocrResult.size();
	char char_str[_MAX_WORD_SIZE * 2];
//	for (int i = 0; i < wnum; i++) {
	for (auto j = 0; j < m_paragraph.size(); j++) {
		for (auto i = 0; i < m_paragraph[j].vecTextBox.size(); i++) {
			memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
			int char_str_len = WideCharToMultiByte(CP_ACP, 0, m_paragraph[j].vecTextBox[i].strCode, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, m_paragraph[j].vecTextBox[i].strCode, -1, char_str, char_str_len, 0, 0);

			_stSDBWord sdword;
			//	sdword.strcode = m_ocrResult[i].hcode;
			//sdword.strcode = getHashCode(char_str);
			unsigned int strcode = getHashCode(char_str);

			//if (sdword.strcode == 5381) 
			//	continue;

			sdword.filecode = m_nCode;
			sdword.rect = m_paragraph[j].vecTextBox[i].rect;
			sdword.fConfi = m_paragraph[j].vecTextBox[i].fConfidence;
			sdword.fDiff = 0.0f;
			memset(sdword.str, 0x00, sizeof(wchar_t)* _MAX_WORD_SIZE);
			memcpy(sdword.str, m_paragraph[j].vecTextBox[i].strCode, sizeof(wchar_t)* _MAX_WORD_SIZE);

			mapSDB[strcode].push_back(sdword);
			//mapSDB[sdword.strcode].push_back(sdword);
			//	SINGLETON_DataMng::GetInstance()->AddSDBTable(sdword.strcode, m_ocrResult[i].strCode);
		}
	}

	USES_CONVERSION;
	CString path = GetPInfoPath(L".sdb");
	char* sz = T2A(path);

	FILE* fp = 0;
	fopen_s(&fp, sz, "wb");
	if (fp) {
		int wnum = static_cast<int>(mapSDB.size());
		fwrite(&wnum, sizeof(int), 1, fp);
		std::map<unsigned int, _stSDB>::iterator iter = mapSDB.begin();
		for (; iter != mapSDB.end(); iter++) {

			int pnum = static_cast<int>(iter->second.size());
			fwrite(&pnum, sizeof(unsigned int), 1, fp);
			fwrite(&iter->first, sizeof(unsigned int), 1, fp);

			for (int i=0; i<pnum; i++) {
				fwrite(&iter->second[i], sizeof(_stSDBWord), 1, fp);
			}
		}
		fclose(fp);
	}

//	mapSDB.clear();
	mapSDB.swap(std::map<unsigned int, _stSDB>());

	//bool bNeedToUpdateTable = false;

	//USES_CONVERSION;
	//CString path = GetPInfoPath(L".sdb");
	//char* sz = T2A(path);

	//FILE* fp = 0;
	//fopen_s(&fp, sz, "wb");
	//if (fp) {
	//	// Write header 8 byte: paragraph number, word number//
	//	int wnum = m_ocrResult.size();
	//	char char_str[_MAX_WORD_SIZE * 2];
	//	for (int i = 0; i < wnum; i++) {
	//		memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
	//		int char_str_len = WideCharToMultiByte(CP_ACP, 0, m_ocrResult[i].strCode, -1, NULL, 0, NULL, NULL);
	//		WideCharToMultiByte(CP_ACP, 0, m_ocrResult[i].strCode, -1, char_str, char_str_len, 0, 0);
	//		
	//		_stSDBFormat sdword;
	//		sdword.strcode = getHashCode(char_str);
	//		sdword.rect = m_ocrResult[i].rect;
	//		sdword.fConfi = m_ocrResult[i].fConfidence;
	//		sdword.fDiff = 0.0f;		

	//		fwrite(&sdword, sizeof(_stSDBFormat), 1, fp);

	//		if (m_mapWordTable.find(sdword.strcode) == m_mapWordTable.end()) {
	//			_stSDBWord tableWord;
	//			memcpy(tableWord.str, m_ocrResult[i].strCode, sizeof(wchar_t)*(_MAX_WORD_SIZE));
	//			tableWord.hcode = sdword.strcode;
	//			m_mapWordTable[sdword.strcode] = tableWord;
	//			bNeedToUpdateTable = true;
	//		}

	//	}
	//	fclose(fp);
	//}

	//// Update Word Table //
	//if (bNeedToUpdateTable) {
	//	path = GetPInfoPath(L".htbl");
	//	sz = T2A(path);
	//	fopen_s(&fp, sz, "wb");
	//	if (fp) {
	//		int wnum = m_mapWordTable.size();
	//		for (int i = 0; i < wnum; i++) {
	//			fwrite(&m_mapWordTable[i].hcode, sizeof(unsigned int), 1, fp);
	//			fwrite(&m_mapWordTable[i].str, sizeof(wchar_t)*_MAX_WORD_SIZE, 1, fp);
	//		}
	//		fclose(fp);
	//	}
	//}
}

bool CMNPageObject::IsNeedToSave()
{
	return m_IsNeedToSave || m_bImageChanged;
}

void CMNPageObject::UpdateDataBaseFiles()
{
	if (m_IsNeedToSave) {
		WriteSearchDBFile();
		WritePageInfo();	
		m_IsNeedToSave = false;
	}

	if (m_bImageChanged) {
		USES_CONVERSION;
		CString imgpath = GetPInfoPath(L".jp2");
		char* szimg = T2A(imgpath);
		cv::imwrite(szimg, m_srcGrayImg);
		m_bImageChanged = false;
	}
}

void CMNPageObject::WritePageInfo()
{
	USES_CONVERSION;
	CString path = GetPInfoPath(L".pinfo");
	char* sz = T2A(path);

	FILE* fp = 0;
	fopen_s(&fp, sz, "wb");
	if (fp) {
		// Write header 8 byte: paragraph number, word number//
		int pnum = static_cast<int>(m_paragraph.size());
		fwrite(&m_nImgWidth, sizeof(unsigned short), 1, fp);
		fwrite(&m_nImgHeight, sizeof(unsigned short), 1, fp);
		fwrite(&pnum, sizeof(int), 1, fp);

		for (int i = 0; i < pnum; i++) {
			fwrite(&m_paragraph[i].rect, sizeof(cv::Rect), 1, fp);
			fwrite(&m_paragraph[i].IsVerti, sizeof(bool), 1, fp);
			int wnum = static_cast<int>(m_paragraph[i].vecTextBox.size());

			fwrite(&wnum, sizeof(int), 1, fp);

			for (int j = 0; j < wnum; j++) {
				m_paragraph[i].vecTextBox[j].bNeedToDB = false;				
				fwrite(&m_paragraph[i].vecTextBox[j], sizeof(_stOCRResult), 1, fp);
			}
		}		
		fclose(fp);
	}

	//if (m_IsTbimg == false) {		// thumbnail image shoudl followed to info file //
	//	CString imgpath = GetPInfoPath(L"_tb.jp2");
	//	char* szimg = T2A(imgpath);
	//	cv::imwrite(szimg, m_thumbImg);
	//}
}




void CMNPageObject::EncodeTexBoxHori(CFile& cfile)
{
	SortLines(false);

	USHORT nShort = 0xfeff;  // 유니코드 바이트 오더마크.
	cfile.Write(&nShort, 2);

	CString strTmp;
	cfile.Write(m_strName, m_strName.GetLength() * 2);
	cfile.Write(L"\r\n", 4);
	cfile.Write(L"--------------------", 40);
	cfile.Write(L"\r\n", 4);

	for (auto i = 0; i < m_paragraph.size(); i++) {
		strTmp.Format(L"%d	", i + 1);
		cfile.Write(strTmp, strTmp.GetLength() * 2);
		auto len = 0;
		auto cNum = m_paragraph[i].vecTextBox.size();
		for (auto j = 0; j < cNum; j++) {
			len = wcslen(m_paragraph[i].vecTextBox[j].strCode) * 2;
			cfile.Write(m_paragraph[i].vecTextBox[j].strCode, len);

			if (j < cNum - 1) {
				int diff = m_paragraph[i].vecTextBox[j + 1].rect.x - (m_paragraph[i].vecTextBox[j].rect.x + m_paragraph[i].vecTextBox[j].rect.width);
				if (diff > m_paragraph[i].vecTextBox[j].rect.height / 2) {
					cfile.Write(L" ", 2);		// Space
				}
			}

		}
		cfile.Write(L"\r\n", 4);
	}

	cfile.Write(L"\r\n", 4);
}



void CMNPageObject::EncodeTexBoxVerti(CFile& cfile)
{
	SortLines(true);

	USHORT nShort = 0xfeff;  // 유니코드 바이트 오더마크.
	cfile.Write(&nShort, 2);

	CString strTmp;
	cfile.Write(m_strName, m_strName.GetLength() * 2);
	cfile.Write(L"\r\n", 4);
	cfile.Write(L"--------------------", 40);
	cfile.Write(L"\r\n", 4);

	for (auto i = 0; i < m_paragraph.size(); i++) {
		strTmp.Format(L"%d	", i + 1);
		cfile.Write(strTmp, strTmp.GetLength()*2);
		auto len = 0;
		auto cNum = m_paragraph[i].vecTextBox.size();
		for (auto j = 0; j < cNum; j++) {
			len = wcslen(m_paragraph[i].vecTextBox[j].strCode) * 2;
			cfile.Write(m_paragraph[i].vecTextBox[j].strCode, len);

			if (j < cNum - 1) {
				int diff = m_paragraph[i].vecTextBox[j+1].rect.y - (m_paragraph[i].vecTextBox[j].rect.y + m_paragraph[i].vecTextBox[j].rect.height);
				if (diff > m_paragraph[i].vecTextBox[j].rect.width / 2) {
					cfile.Write(L" ", 2);		// Space
				}
			}

		}
		cfile.Write(L"\r\n", 4);
	}

	cfile.Write(L"\r\n", 4);
	//cfile.Close();
	//::ShellExecute(NULL, L"open", L"notepad", strPath, NULL, SW_SHOW);


	//int wordheight = 32;
	//// Sorting by x, y pos //
	////=For Encoding======================//
	//typedef struct _EWORDINFO {
	//	wchar_t* str;
	//	cv::Rect rect;
	//}_EWORDINFO;

	//std::vector<_EWORDINFO> vecEncode;
	//for (int i = 0; i < m_ocrResult.size(); i++) {

	//	if (m_ocrResult[i].fConfidence > 0.1f) {
	//		_EWORDINFO tmp;
	//		tmp.rect = m_ocrResult[i].rect;
	//		tmp.str = m_ocrResult[i].strCode;
	//		vecEncode.push_back(tmp);
	//	}
	//}

	////std::vector<_ENCODETEXT> encodeWord;
	//////==================================//
	//int minY = 10000, minX = 0;
	//RECT2D r1, r2;
	//int y1, y2, x1, x2;

	//int numItem = vecEncode.size();
	//if (numItem > 1) {

	//	for (int i = 0; i < numItem - 1; i++)
	//	{
	//		for (int j = 0; j < numItem - i - 1; j++)
	//		{
	//			if ((vecEncode[j].rect.x + vecEncode[j].rect.width) < (vecEncode[j + 1].rect.x+ vecEncode[j + 1].rect.width)) /* For decreasing order use < */
	//			{
	//				_EWORDINFO swap = vecEncode[j];
	//				vecEncode[j] = vecEncode[j + 1];
	//				vecEncode[j + 1] = swap;
	//			}
	//		}
	//	}

	//	for (int i = 0; i < numItem - 1; i++)
	//	{
	//		for (int j = 0; j < numItem - i - 1; j++)
	//		{
	//			if ((vecEncode[j].rect.y)  > (vecEncode[j + 1].rect.y) ) /* For decreasing order use < */
	//			{
	//				int averWidth = vecEncode[j].rect.width > vecEncode[j + 1].rect.width ? vecEncode[j].rect.width : vecEncode[j + 1].rect.width;
	//				int my1 = vecEncode[j].rect.x + vecEncode[j].rect.width;
	//				int my2 = vecEncode[j + 1].rect.x + vecEncode[j + 1].rect.width;

	//				if (abs(my1 - my2) < (averWidth*0.75f)) {
	//					_EWORDINFO swap = vecEncode[j];
	//					vecEncode[j] = vecEncode[j + 1];
	//					vecEncode[j + 1] = swap;
	//				}
	//			}

	//			//if (vecEncode[j].rect.y > vecEncode[j + 1].rect.y) /* For decreasing order use < */
	//			//{
	//			//	_EWORDINFO swap = vecEncode[j];
	//			//	vecEncode[j] = vecEncode[j + 1];
	//			//	vecEncode[j + 1] = swap;
	//			//}
	//		}
	//	}
	//}

	//CString strPath = GetPInfoPath(L".txt");
	//CFile cfile;
	//if (!cfile.Open(strPath, CFile::modeWrite | CFile::modeCreate))
	//{
	//	return;
	//}

	//USHORT nShort = 0xfeff;  // 유니코드 바이트 오더마크.
	//cfile.Write(&nShort, 2);

	////	file.Write(pStrWideChar, nSize * 2);
	//if (vecEncode.size() > 0) {

	//	int preXPos = 0, xPos = 0;
	//	bool Isfirst = true;
	//	int wNum = vecEncode.size();
	//	int averWidth = 0;// = vecEncode[0].rect.height;
	//	for (int i = 0; i < wNum; i++) {

	//		if (i < wNum - 1) {
	//			averWidth = vecEncode[i].rect.width > vecEncode[i + 1].rect.width ? vecEncode[i].rect.width : vecEncode[i + 1].rect.width;
	//		}
	//		else {
	//			averWidth = vecEncode[i].rect.width;
	//		}

	//		//	wchar_str = vecEncode[i].str.GetBuffer(vecEncode[i].str.GetLength());
	//		xPos = vecEncode[i].rect.x;

	//		if (i == 0) {
	//			preXPos = xPos;
	//		}
	//		else {
	//			int diff = abs(preXPos - xPos);
	//			if (diff > averWidth*0.75f) {
	//				cfile.Write(L"\r\n", 4);
	//			}
	//			//	}
	//			preXPos = xPos;
	//		}
	//		int len = wcslen(vecEncode[i].str) * 2;
	//		cfile.Write(vecEncode[i].str, len);

	//		if (i < wNum - 1) {
	//			int diff = vecEncode[i + 1].rect.y - (vecEncode[i].rect.y + vecEncode[i].rect.height);
	//			if (diff > averWidth / 2) {
	//				cfile.Write(L" ", 2);		// Space
	//			}
	//		}
	//	}
	//}
	//cfile.Close();
	//::ShellExecute(NULL, L"open", L"notepad", strPath, NULL, SW_SHOW);

}


void CMNPageObject::DrawSDBItem()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	if (m_sdbResult.size() > 0) {
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-m_nImgWidth*0.5f, -m_nImgHeight*0.5f, 0.0f);

		
		for (int i = 0; i < m_sdbResult.size(); i++) {
			glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
			glBegin(GL_QUADS);
			glVertex3f(m_sdbResult[i].rect.x, m_nImgHeight - m_sdbResult[i].rect.y, 0.0f);
			glVertex3f(m_sdbResult[i].rect.x, m_nImgHeight - (m_sdbResult[i].rect.y + m_sdbResult[i].rect.height), 0.0f);
			glVertex3f(m_sdbResult[i].rect.x + m_sdbResult[i].rect.width, m_nImgHeight - (m_sdbResult[i].rect.y + m_sdbResult[i].rect.height), 0.0f);
			glVertex3f(m_sdbResult[i].rect.x + m_sdbResult[i].rect.width, m_nImgHeight - m_sdbResult[i].rect.y, 0.0f);
			glEnd();


			glColor4f(1.0f, 1.0f, 0.0f, 0.99f);
			glBegin(GL_LINE_STRIP);
			glVertex3f(m_sdbResult[i].rect.x, m_nImgHeight - m_sdbResult[i].rect.y, 0.0f);
			glVertex3f(m_sdbResult[i].rect.x, m_nImgHeight - (m_sdbResult[i].rect.y + m_sdbResult[i].rect.height), 0.0f);
			glVertex3f(m_sdbResult[i].rect.x + m_sdbResult[i].rect.width, m_nImgHeight - (m_sdbResult[i].rect.y + m_sdbResult[i].rect.height), 0.0f);
			glVertex3f(m_sdbResult[i].rect.x + m_sdbResult[i].rect.width, m_nImgHeight - m_sdbResult[i].rect.y, 0.0f);
			glVertex3f(m_sdbResult[i].rect.x, m_nImgHeight - m_sdbResult[i].rect.y, 0.0f);
			glEnd();
		}

		glPopMatrix();
	}
	glPopMatrix();
}

stParapgraphInfo CMNPageObject::GetLineBoxInfo(int pid)
{
	stParapgraphInfo lineBox;
	lineBox.init();

	
	if (m_paragraph.size() > 0) {
		lineBox = m_paragraph[pid];
	}

	return lineBox;
}

void CMNPageObject::SortLines(bool IsVerti)
{
	int numLine = static_cast<int>(m_paragraph.size());
	if (numLine < 1) return;

	if ((IsVerti)){
		for (auto i = 0; i < numLine - 1; i++)
		{
			for (auto j =0; j < numLine -i - 1; j++)
			{
				if ((m_paragraph[j].rect.x) < (m_paragraph[j + 1].rect.x )) {
					stParapgraphInfo swap = m_paragraph[j];
					m_paragraph[j] = m_paragraph[j + 1];
					m_paragraph[j + 1] = swap;
				}
			}
		}

		//========Sort Characters========From Top to bottom ====//
		for (auto i = 0; i < m_paragraph.size(); i++) {
			if (m_paragraph[i].vecTextBox.size() == 0) continue;

			for(auto j=0; j<m_paragraph[i].vecTextBox.size()-1; j++){
				for (auto k = 0; k < m_paragraph[i].vecTextBox.size()-j-1; k++) {
					if ((m_paragraph[i].vecTextBox[k].rect.y) > (m_paragraph[i].vecTextBox[k + 1].rect.y)) {
						_stOCRResult swap = m_paragraph[i].vecTextBox[k];
						m_paragraph[i].vecTextBox[k] = m_paragraph[i].vecTextBox[k + 1];
						m_paragraph[i].vecTextBox[k + 1] = swap;
					}
				}
			}
		}

	}


	else {		// Horizontal
		for (auto i = 0; i < numLine - 1; i++)
		{
			for (auto j = 0; j < numLine - i - 1; j++)
			{
				if ((m_paragraph[j].rect.y) > (m_paragraph[j + 1].rect.y)) /* For decreasing order use < */
				{
					stParapgraphInfo swap = m_paragraph[j];
					m_paragraph[j] = m_paragraph[j + 1];
					m_paragraph[j + 1] = swap;
				}
			}
		}

		//========Sort Characters========From Left to Top ====//
		for (auto i = 0; i < m_paragraph.size(); i++) {
			for (auto j = 0; j<m_paragraph[i].vecTextBox.size() - 1; j++) {
				for (auto k = 0; k < m_paragraph[i].vecTextBox.size() - j - 1; k++) {
					if ((m_paragraph[i].vecTextBox[k].rect.x) > (m_paragraph[i].vecTextBox[k + 1].rect.x)) {
						_stOCRResult swap = m_paragraph[i].vecTextBox[k];
						m_paragraph[i].vecTextBox[k] = m_paragraph[i].vecTextBox[k + 1];
						m_paragraph[i].vecTextBox[k + 1] = swap;
					}
				}
			}
		}
	}

}