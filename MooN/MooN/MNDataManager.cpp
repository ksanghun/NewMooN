#include "stdafx.h"
#include "MNDataManager.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// For convert image file to base64 //
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
//===================================//

CMNDataManager::CMNDataManager()
{
	memset(m_bSlot, 0x00, sizeof(m_bSlot));
	m_xOffset = DEFAULT_X_OFFSET;
	m_yOffset = DEFAULT_Y_OFFSET;

	float scale = 1.0f / 4.147f; // sum of sin vlaue between 0 ~ 90
	float fstepRad = (3.141529f*0.5f) / (float)ANI_FRAME_CNT;
	for (int i = 0; i < ANI_FRAME_CNT; i++) {
		m_fAniAcceration[i] = (1.0f - sin(fstepRad*i))*scale;
	}

	mtSetPoint3D(&m_AccColor[9], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[8], 1.0f, 0.2f, 0.0f);
	mtSetPoint3D(&m_AccColor[7], 1.0f, 0.4f, 0.0f);
	mtSetPoint3D(&m_AccColor[6], 1.0f, 0.6f, 0.0f);
	mtSetPoint3D(&m_AccColor[5], 1.0f, 0.8f, 0.0f);
	mtSetPoint3D(&m_AccColor[4], 1.0f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[3], 0.8f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[2], 0.6f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[1], 0.4f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[0], 0.2f, 1.0f, 0.0f);

	m_maxCutWidth = _NORMALIZE_SIZE_W + 5;
}


CMNDataManager::~CMNDataManager()
{	
	for (size_t i = 0; i < m_vecImgData.size(); i++) {
		GLuint tex = m_vecImgData[i]->GetThumbnailTex();
		GLuint texfull = m_vecImgData[i]->GetTexId();
		if (tex > 0) {
			glDeleteTextures(1, &tex);
		}
		if (texfull > 0) {
			glDeleteTextures(1, &texfull);
		}
		delete m_vecImgData[i];
	}

	m_vecImgData.clear();
	m_mapImageData.clear();
	m_mapGrupImg.clear();

}

CMNPageObject* CMNDataManager::GetPageByOrderID(int idx)
{
	if (idx < m_vecImgData.size()) {
		return m_vecImgData[idx];
	}
	return NULL;
}

void CMNDataManager::Test()
{
	//cv::Mat img = cv::imread("D:\\test.jpg", CV_LOAD_IMAGE_COLOR);
	//cv::imshow("JPGTEST", img);
	//img.release();

	//cv::Mat pimg;
	//m_pdf.LoadPDF(L"D:\\test.jpg", pimg);

	//cv::imshow("PDF", pimg);
	//pimg.release();

}

bool CMNDataManager::LoadImageData(CString strPath, cv::Mat& pimg, bool IsGray)
{
	CString str = PathFindExtension(strPath);
	if ((str == L".pdf") || (str == L".PDF")) {
			return m_pdf.LoadPDF(strPath, pimg, IsGray);	
	}
	else {
		USES_CONVERSION;	
		char* sz = T2A(strPath);
		pimg = cv::imread(sz);
		if (pimg.data == NULL) {
			return false;
		}
		else {
			if (IsGray) {
				cv::cvtColor(pimg, pimg, CV_BGR2GRAY);
			}
			else {
				if (str != L".jp2") {
					cv::cvtColor(pimg, pimg, CV_BGR2RGB);
				}
			}
			return true;
		}
	}
}

void CMNDataManager::InitData()
{

}

void CMNDataManager::PopImageDataSet(unsigned long _pcode)
{
	// Remove from map image data
	std::map<unsigned long, CMNPageObject*>::iterator iter_map = m_mapImageData.begin();
	for (; iter_map != m_mapImageData.end();) {
		if (iter_map->second->GetPCode() == _pcode) {
			m_mapImageData.erase(iter_map++);
		}
		else {
			++iter_map;
		}
	}

	// Remove from map group image data
	std::map<unsigned long, stPageGroup>::iterator iter_gr;
	iter_gr = m_mapGrupImg.find(_pcode);
	if (iter_gr != m_mapGrupImg.end()) {
		if (iter_gr->second.nSlot != -1) {
			SelectPages(_pcode);
		}
		iter_gr->second.nSlot = -1;
		iter_gr->second.imgVec.clear();
		m_mapGrupImg.erase(iter_gr);
	}

	// Remove from vector <- real data!
	std::vector<CMNPageObject*>::iterator iter_vec = m_vecImgData.begin();
	for (; iter_vec != m_vecImgData.end();) {
		if ((*iter_vec)->GetPCode() == _pcode) {
			GLuint texth = (*iter_vec)->GetThumbnailTex();
			GLuint texfull = (*iter_vec)->GetTexId();
			if (texth > 0) {
				glDeleteTextures(1, &texth);
			}
			if (texfull > 0) {
				glDeleteTextures(1, &texfull);
			}
			delete (*iter_vec);
			iter_vec = m_vecImgData.erase(iter_vec);
		}
		else {
			++iter_vec;
		}
	}	
}



void CMNDataManager::PushImageDataSet(CString _strpath, CString _strPName, CString _strName, unsigned long _code, unsigned long _pcode)
{
	//CString str = PathFindExtension(_strName);

	//// Image File Filter ===============//
	//if ((str == L".pdf") || (str == L".PDF") ||
	//	(str == L".jpg") || (str == L".JPG") ||
	//	(str == L".bmp") || (str == L".BMP") ||
	//	(str == L".png") || (str == L".PNG") ||
	//	(str == L".tiff") || (str == L".TIFF")) 
	//{

		std::map<unsigned long, stPageGroup>::iterator iter_gr;
		std::map<unsigned long, CMNPageObject*>::iterator iter;


		CMNPageObject* pimg = new CMNPageObject;
		pimg->SetName(_strpath, _strPName, _strName, _code, _pcode);

		iter = m_mapImageData.find(_code);
		if (iter == m_mapImageData.end()) {

			// Store Data Information ================================//!!!!
			m_mapImageData[_code] = pimg;		// for duplication checking

			// push image data sequecily ..
			m_vecImgData.push_back(pimg);

			// Make Group =========================//
			iter_gr = m_mapGrupImg.find(_pcode);
			if (iter_gr == m_mapGrupImg.end()) {		// New Group
				stVecPageObj vecImg;
				vecImg.push_back(std::move(pimg));
				m_mapGrupImg[_pcode].imgVec = vecImg;
				m_mapGrupImg[_pcode].nSlot = -1;
			}
			else {
				m_mapGrupImg[_pcode].imgVec.push_back(std::move(pimg));
			}
			//======================================//
		}
		else {
			delete pimg;
		}
//	}
}

int CMNDataManager::GetEmptySlot()
{
	for (int i = 0; i < MAX_SLOT_SIZE; i++) {
		if (m_bSlot[i] == false) {
			m_bSlot[i] = true;
			return i;
		}
	}
	return -1;
}

void CMNDataManager::ReturnSlot(int idx)
{
	if (idx < MAX_SLOT_SIZE) {
		m_bSlot[idx] = false;
	}
}

short CMNDataManager::SelectPages(unsigned long cCode)
{
	short res = -1;
	std::map<unsigned long, stPageGroup>::iterator iter_gr;
	iter_gr = m_mapGrupImg.find(cCode);
	if (iter_gr != m_mapGrupImg.end()) {
		//	int nSlot = iter_gr->second.nSlot;

		if (iter_gr->second.nSlot == -1) {		// Selection
			iter_gr->second.nSlot = GetEmptySlot();
			float xoffset = DEFAULT_X_OFFSET;
			int i = 0;
			for (i = 0; i < iter_gr->second.imgVec.size(); i++) {
				if (i%MAX_DESP_COLS == 0) {
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset -= DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(iter_gr->second.nSlot, xoffset, m_yOffset, true);
			}
			if (i%MAX_DESP_COLS == 0) {
				xoffset = DEFAULT_X_OFFSET;
				m_yOffset -= DEFAULT_PAGE_SIZE;
			}
			res = 1;
		}
		else {		// Deselection //
			ReturnSlot(iter_gr->second.nSlot);
			iter_gr->second.nSlot = -1;
			float xoffset = DEFAULT_X_OFFSET;
			int i = 0;
			for (i = 0; i < iter_gr->second.imgVec.size(); i++) {
				if (i % MAX_DESP_COLS == 0) {
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset += DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(-1, xoffset, m_yOffset, true);
			}
			if (i % MAX_DESP_COLS == 0) {
				xoffset = DEFAULT_X_OFFSET;
				m_yOffset += DEFAULT_PAGE_SIZE;
			}
			res = 2;
		}
	}
	return res;
}

void CMNDataManager::ApplyDeskewPage()
{
	//for (size_t i = 0; i < m_vecImgData.size(); i++) {
	//	m_vecImgData[i]->DeSkewImg();
	//}
}

float CMNDataManager::GetAniAcceration(int idx)
{
	if (idx < ANI_FRAME_CNT)
		return m_fAniAcceration[idx];
	else
		return 0.0f;
}

void CMNDataManager::UpdatePageStatus(POINT3D camPos)
{
	// Distance between cam and pages//
	// if the distance is less than TH, load high-resolution image for page//
	for (size_t i = 0; i < m_vecImgData.size(); i++) {
		if (m_vecImgData[i]->IsCandidate()) {
			float fDist = mtDistance(camPos, m_vecImgData[i]->GetPos());
			if (fDist < DEFAULT_PAGE_SIZE*2.0f) {
				m_vecImgData[i]->LoadFullImage();
				m_vecImgData[i]->SetIsNear(true);
			}
			else {
				m_vecImgData[i]->SetIsNear(false);
			}
		}
	}
}

CBitmap* CMNDataManager::GetLogCBitmap(cv::Mat& pimg)
{
	if (pimg.ptr() != NULL) {

		CDC dc;
		CDC memDC;

		CBitmap* bmp = new CBitmap;
		CBitmap* pOldBmp;

		if (!dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL))
			return NULL;

		if (!memDC.CreateCompatibleDC(&dc))
			return NULL;

		int w, h;
		int nWidth = pimg.cols;
		int nHeight = pimg.rows;
		BYTE* pSrcBits = (BYTE *)pimg.ptr();
		BYTE* pBmpBits = (BYTE *)malloc(sizeof(BYTE)*nWidth*nHeight * 4);

		// IplImage에 저장된 값을 직접 읽어서 
		// 비트맵 데이터를 만듬 
		for (h = 0; h < nHeight; ++h)
		{
			BYTE* pSrc = pSrcBits + pimg.step.p[0] * h;
			BYTE* pDst = pBmpBits + nWidth * 4 * h;
			for (w = 0; w < nWidth; ++w)
			{
				if (pimg.channels() == 1) {
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pimg.channels() == 3) {
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pimg.channels() == 4) {
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);;
				}
			}
		}
		//		memDC.CreateCompatibleDC(pDC);
		bmp->CreateCompatibleBitmap(&dc, nWidth, nHeight);
		// 위에서 만들어진 데이터를 가지고 
		// 비트맵을 만듬 
		bmp->SetBitmapBits(nWidth*nHeight * 4, pBmpBits);
		pOldBmp = memDC.SelectObject(bmp);


		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
		dc.DeleteDC();

		free(pBmpBits);

		return bmp;
	}
	else {
		return NULL;
	}
}


cv::Rect CMNDataManager::GetNomalizedWordSize(cv::Rect rect)
{
	cv::Rect norRect;
	norRect.x = 0;
	norRect.y = 0;

	float fScale = (float)_NORMALIZE_SIZE_H / (float)rect.height;
	norRect.width = rect.width*fScale;
	norRect.height = rect.height*fScale;

	int wcnt = norRect.width / _NORMALIZE_SIZE_H;
	if (wcnt < 1) {
		wcnt = 1;
	}

	if (wcnt > 10) {
		wcnt = 10;
	}

	norRect.width = _NORMALIZE_SIZE_H*wcnt;

	if (rect.height > _NORMALIZE_SIZE_H) {
		norRect.height = _NORMALIZE_SIZE_H;
	}

	return norRect;
}

void CMNDataManager::SetMatchingResults()
{
	//	ResetMatchingResult();

	m_maxCutWidth = _NORMALIZE_SIZE_W + 5;

	stVecMatchResult vecMatchRes;
	bool IsAdded = false;
	int cnt = 0;

	for (size_t i = 0; i < m_vecImgData.size(); i++) {
		std::vector<stMatchInfo>& matches = m_vecImgData[i]->GetMatchResult();
		if (matches.size() > 0) {
			unsigned int matchFile = getHashCode((CStringA)m_vecImgData[i]->GetPath());
		
			cv::Mat srcImg = m_vecImgData[i]->GetSrcPageImg();
	//		if (LoadImageData(m_vecImgData[i]->GetPath(), srcImg, false)) {
			if (srcImg.ptr()) {

				for (int j = 0; j < matches.size(); j++) {

					if (matches[j].IsAdded == true)	continue;

					unsigned int matchPos = (int)matches[j].rect.x * 10000 + (int)matches[j].rect.y;

					CString strId;
					strId.Format(L"%u%u", matchFile, matchPos);
					unsigned int matchId = getHashCode((CStringA)strId);

					stMatchResult matchRes;

					matchRes.searchId = matches[j].searchId;
					matchRes.cutId = matches[j].cInfo.id;
					matchRes.fileId = matches[j].cInfo.fileid;
					matchRes.posId = matches[j].cInfo.posid;
					matchRes.matchId = matchId;
					matchRes.matchFile = matchFile;
					matchRes.matchPos = matchPos;
					matchRes.accuracy = matches[j].accuracy;
					matchRes.fTh = matches[j].cInfo.th;
					matchRes.rect = matches[j].rect;
					matchRes.strCode = "unknown";

					// hold cut image data --> should be deleted //
					cv::Rect nRect = GetNomalizedWordSize(matches[j].rect);
					cv::Mat tmpcut = srcImg(matches[j].rect);
					cv::resize(tmpcut, tmpcut, cv::Size(nRect.width, nRect.height));

					matchRes.cutImg = cv::Mat(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H),srcImg.type());
					matchRes.cutImg.setTo(255);
					tmpcut.copyTo(matchRes.cutImg(nRect));
					//tmpcut.release();					

					if (m_maxCutWidth > nRect.width)
						m_maxCutWidth = nRect.width + 5;

					//Encode image file to base64 //
					std::vector<uchar> data_encode(_NORMALIZE_SIZE_W*_NORMALIZE_SIZE_H * 4);
					cv::imencode(".bmp", tmpcut, data_encode);
					matchRes.strBase64 = base64_encode((unsigned char*)&data_encode[0], data_encode.size());
					data_encode.clear();
					tmpcut.release();
					////===========================================//

					// Add Results==========================================//
					matchRes.id_page = i;
					matchRes.id_match = j;
					matchRes.IsOnList = false;
					m_mapMatchResults[matchRes.searchId].push_back(matchRes);
					matches[j].IsAdded = true;
					IsAdded = true;
				}

				srcImg.release();
			}
		}
	}
}

void CMNDataManager::SortMatchingResults()
{
	// bubble sort //

	std::map<unsigned long, stVecMatchResult>::iterator iter_gr = m_mapMatchResults.begin();

	for (; iter_gr != m_mapMatchResults.end(); iter_gr++) {
		size_t numItem = iter_gr->second.size();
		if (numItem > 1) {
			for (int i = 0; i < numItem - 1; i++)
			{
				for (int j = 0; j < numItem - i - 1; j++)
				{
					if (iter_gr->second[j].accuracy < iter_gr->second[j + 1].accuracy) /* For decreasing order use < */
					{
						stMatchResult swap = iter_gr->second[j];
						iter_gr->second[j] = iter_gr->second[j + 1];
						iter_gr->second[j + 1] = swap;
					}
				}
			}
		}
	}
}

CString CMNDataManager::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	CString strEncode(ret.c_str());
	return strEncode;
}


CMNPageObject* CMNDataManager::GetPageByID(int pid)
{
	if ((m_vecImgData.size()>0) && (pid <= (int)m_vecImgData.size())) {
		return m_vecImgData[pid];
	}
	else {
		return NULL;
	}
}

void CMNDataManager::ResetResult()
{
	for (int i = 0; i < m_vecImgData.size(); i++) {
		m_vecImgData[i]->ClearMatchResult();
	}
	ResetMatchingResult();
}

void CMNDataManager::ResetMatchingResult()
{
	std::map<unsigned long, stVecMatchResult>::iterator iter_gr = m_mapMatchResults.begin();
	for (; iter_gr != m_mapMatchResults.end(); iter_gr++) {
		for (int j = 0; j < iter_gr->second.size(); j++) {
			if (iter_gr->second[j].cutImg.ptr() != NULL) {
				iter_gr->second[j].cutImg.release();
			}
		}
		iter_gr->second.clear();
	}
	m_mapMatchResults.clear();
}

void CMNDataManager::MultiToUniCode(char* char_str, wchar_t* str_unicode)
{
	//char char_str[_MAX_WORD_SIZE] = { 0, };
	//wchar_t strUnicode[_MAX_WORD_SIZE] = { 0, };
	// Multi to Unicode //
	int nLen = MultiByteToWideChar(CP_ACP, 0, &char_str[0], strlen(char_str), NULL, NULL);		
	MultiByteToWideChar(CP_ACP, 0, char_str, strlen(char_str), str_unicode, nLen);
}

_stExtractionSetting CMNDataManager::GetExtractionSetting() 
{ 
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->GetCurrSetting();
	return m_extractonInfo; 
}
