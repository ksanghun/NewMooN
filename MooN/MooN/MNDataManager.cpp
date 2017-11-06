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
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}



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
	mtSetPoint3D(&m_AccColor[8], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[7], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[6], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[5], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[4], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[3], 1.8f, 0.5f, 0.0f);
	mtSetPoint3D(&m_AccColor[2], 1.0f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[1], 0.5f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[0], 0.0f, 1.0f, 0.0f);

	m_maxCutWidth = _NORMALIZE_SIZE_W + 5;

	m_bIsUpdateTable = false;
}


CMNDataManager::~CMNDataManager()
{	
	//for (size_t i = 0; i < m_vecImgData.size(); i++) {
	//	GLuint tex = m_vecImgData[i]->GetThumbnailTex();
	//	GLuint texfull = m_vecImgData[i]->GetTexId();
	//	if (tex > 0) {
	//		glDeleteTextures(1, &tex);
	//	}
	//	if (texfull > 0) {
	//		glDeleteTextures(1, &texfull);
	//	}
	//	delete m_vecImgData[i];
	//}

	//m_vecImgData.clear();
	//m_mapImageData.clear();
	//m_mapGrupImg.clear();
	ClearAllImages();

	for (int i = 0; i < DB_CLASS_NUM; i++) {
		for (int j = 0; j < m_refImgClass[i].vecStr.size(); j++) {
			m_refImgClass[i].img[0].release();
			delete m_refImgClass[i].vecStr[j];
		}
	}

	
	UpdateSDBFiles();
}

POINT3D CMNDataManager::GetColor(float fvalue)
{
	int idx = (10-fvalue * 10);
	if (idx<1)
		idx = 0;
	if (idx>3)
		idx = 9;

	return m_AccColor[idx];
}

void CMNDataManager::ClearAllImages()
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

	memset(m_bSlot, 0x00, sizeof(m_bSlot));
	m_xOffset = DEFAULT_X_OFFSET;
	m_yOffset = DEFAULT_Y_OFFSET;
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
	m_refImgClass[0].init(60, 60, 4);
	m_refImgClass[1].init(30, 60, 8);
	m_refImgClass[2].init(20, 60, 12);
	m_refImgClass[3].init(15, 60, 16);
	m_refImgClass[4].init(12, 60, 20);
	m_refImgClass[5].init(10, 60, 24);
	m_refImgClass[6].init(8, 56, 28);
	m_refImgClass[7].init(7, 56, 32);


	InitDataBaseFiles();
	LoadSDBFiles();
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

CMNPageObject* CMNDataManager::PushImageDataSet(CString _strpath, CString _strPName, CString _strName, unsigned long _code, unsigned long _pcode)
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

		return pimg;
	}
	else {
		delete pimg;
		return iter->second;
	}

		
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
			m_yOffset -= DEFAULT_PAGE_SIZE;  // added
		}
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
			m_yOffset += DEFAULT_PAGE_SIZE;  // added
	//	}
		//else {		// Deselection //
		//	ReturnSlot(iter_gr->second.nSlot);
		//	iter_gr->second.nSlot = -1;
		//	float xoffset = DEFAULT_X_OFFSET;
		//	int i = 0;
		//	for (i = 0; i < iter_gr->second.imgVec.size(); i++) {
		//		if (i % MAX_DESP_COLS == 0) {
		//			xoffset = DEFAULT_X_OFFSET;
		//			m_yOffset += DEFAULT_PAGE_SIZE;
		//		}
		//		xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(-1, xoffset, m_yOffset, true);
		//	}
		//	if (i % MAX_DESP_COLS == 0) {
		//		xoffset = DEFAULT_X_OFFSET;
		//		m_yOffset += DEFAULT_PAGE_SIZE;
		//	}
		//	res = 2;
		//}
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
		
			cv::Mat srcImg = m_vecImgData[i]->GetSrcPageGrayImg();
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
					cv::Mat tmpcut = srcImg(matches[j].rect).clone();
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

int CMNDataManager::GetNomalizedWordSize(cv::Rect inrect, cv::Rect& outRect)
{
	outRect.x = 0;
	outRect.y = 0;

	float fScale = (float)_NORMALIZE_SIZE_H / (float)inrect.height;
	outRect.width = inrect.width*fScale;
	outRect.height = inrect.height*fScale;

	int wcnt = ((float)outRect.width / (float)_NORMALIZE_SIZE_H)+0.5f;
	if (wcnt < 1) {		wcnt = 1;	}
	if (wcnt > 8) {		wcnt = 8;	}

	outRect.width = _NORMALIZE_SIZE_H*wcnt;

	if (inrect.height > _NORMALIZE_SIZE_H) {
		outRect.height = _NORMALIZE_SIZE_H;
	}
	return wcnt;
}

void CMNDataManager::MatchingFromDB(cv::Mat& cutimg, _stOCRResult& ocrres)
{
	//cv::imshow("before", cutimg);
	//DeSkew(cutimg);
	//cv::imshow("after", cutimg);

	cv::Rect norRect;// = GetNomalizedWordSize(ocrRes[j].rect);
	int classid = GetNomalizedWordSize(ocrres.rect, norRect) - 1;
	int imgid = m_refImgClass[classid].nCurrImgId;
	cv::resize(cutimg, cutimg, cvSize(norRect.width, norRect.height));

	for (int pos = 0; pos < m_refImgClass[classid].vecStr.size(); pos++) {
		int w = (classid + 1) * DB_IMGCHAR_SIZE;
		int h = DB_IMGCHAR_SIZE;
		cv::Rect rect;
		rect.x = (pos % m_refImgClass[classid].wNum)*w;
		rect.y = (pos / m_refImgClass[classid].wNum)*h;
		rect.width = w;
		rect.height = h;


		cv::Mat imgword = cv::Mat(h + 8, w + 8, CV_8UC1, cv::Scalar(255));
		m_refImgClass[classid].img[imgid](rect).copyTo(imgword(cv::Rect(4, 4, w, h)));
		int clen = m_refImgClass[classid].maxCharLen;
		float confi = TemplateMatching(cutimg, imgword)+0.1f;
		if ((ocrres.fConfidence < confi )){  //( confi > 0.80f) && 
			memset(ocrres.strCode, 0x00, sizeof(wchar_t)*_MAX_WORD_SIZE);
			memcpy(ocrres.strCode, m_refImgClass[classid].vecStr[pos], sizeof(wchar_t)*clen);
			ocrres.fConfidence = confi;
			if (ocrres.fConfidence > 1.0f)
				ocrres.fConfidence = 1.0f;
		}

		imgword.release();
	}
}
float CMNDataManager::TemplateMatching(cv::Mat& src, cv::Mat& dst)
{
	//cv::imshow("src", src);
	//cv::imshow("dst", dst);

	int result_cols = dst.cols - src.cols + 1;
	int result_rows = dst.rows - src.rows + 1;
	cv::Mat result(result_rows, result_cols, CV_32FC1);
	cv::matchTemplate(dst, src, result, CV_TM_CCOEFF_NORMED);

	float th = 0.0f;
	float fD = 0.0f;
	for (int y = 0; y < result.rows; y++) {
		for (int x = 0; x < result.cols; x++) {
			fD = result.at<float>(y, x);
			if (fD > th) {
				TRACE("%3.2f\n", fD);
				th = fD;
			}
		}
	}
	return th;
}

bool CMNDataManager::IsNeedToAddDB(cv::Mat& cutimg, wchar_t* strcode, int classid)
{
	for (auto pos=0; pos<m_refImgClass[classid].vecStr.size(); pos++){
		if (wcscmp(strcode, m_refImgClass[classid].vecStr[pos]) == 0) {	
			int imgid = pos / (m_refImgClass[classid].wNum*m_refImgClass[classid].hNum);
			// Template Matching //
			int w = (classid + 1) * DB_IMGCHAR_SIZE;
			int h = DB_IMGCHAR_SIZE;
			cv::Rect rect;
			rect.x = (pos % m_refImgClass[classid].wNum)*w;
			rect.y = (pos / m_refImgClass[classid].wNum)*h;
			rect.width = w;
			rect.height = h;

			cv::Mat imgword = cv::Mat(h + 8, w + 8, CV_8UC1, cv::Scalar(255));
			m_refImgClass[classid].img[imgid](rect).copyTo(imgword(cv::Rect(4, 4, w, h)));

			if (TemplateMatching(cutimg, imgword) > 0.8f) {
				return false;
			}
		}
	}	
	return true;
}

void CMNDataManager::ResizeCutImageByRatio(cv::Mat& dstimg, cv::Mat& cutimg, int norWidth, int norHeight)
{
	int basePixel = 8;	
	cv::Rect copyRect = cv::Rect(0,0,norWidth, norHeight);

	if ((cutimg.cols < basePixel) && (cutimg.rows < basePixel)) {// in case of small character: , .  smaller than 8 pixel;
		copyRect.x = 12;
		copyRect.y = 12;
		copyRect.width = basePixel;
		copyRect.height = basePixel;
	}
	else {		// Check aspect ratio //
		float aRatio = (float)cutimg.cols / (float)cutimg.rows;
		//if (aRatio > 2.67f) {  // horizontal
		//	copyRect.x = 0;
		//	copyRect.y = 10;
		//	copyRect.width = norWidth;
		//	copyRect.height = 12;
		//}
		//else 
		if (aRatio < 0.375f) {  // vertical //
			copyRect.x = 10;
			copyRect.y = 0;
			copyRect.width = 12;
			copyRect.height = norHeight;
		}
	}	
	cv::resize(cutimg, cutimg, cvSize(copyRect.width, copyRect.height));
	cutimg.copyTo(dstimg(copyRect));
}


void CMNDataManager::DBTrainingForPage(CMNPageObject* pPage)
{
		std::vector<_stOCRResult> ocrRes = pPage->GetVecOCRResult();
		for (int j = 0; j < ocrRes.size(); j++) {
			if ((ocrRes[j].bNeedToDB) && (ocrRes[j].fConfidence<=1.0f)) {
				cv::Mat cutimg = pPage->GetSrcPageGrayImg()(ocrRes[j].rect).clone();
				cv::Rect norRect;// = GetNomalizedWordSize(ocrRes[j].rect);
				int classid = GetNomalizedWordSize(ocrRes[j].rect, norRect) - 1;
				int imgid = m_refImgClass[classid].nCurrImgId;

				cv::Mat resimg = cv::Mat(cvSize(norRect.width, norRect.height), cutimg.type());
				resimg.setTo(255);
				//cv::resize(cutimg, cutimg, cvSize(norRect.width, norRect.height));
				ResizeCutImageByRatio(resimg, cutimg, norRect.width, norRect.height);


				// Verify to add into DB =====================================//
				bool IsAdd = IsNeedToAddDB(resimg, ocrRes[j].strCode, classid);
				//===========================================================//

				if ((classid < 8) && (classid >= 0) && (IsAdd)) {
					int wordPosId = m_refImgClass[classid].vecStr.size();

					int w = (classid + 1) * DB_IMGCHAR_SIZE;
					int h = DB_IMGCHAR_SIZE;

					norRect.x = (wordPosId % m_refImgClass[classid].wNum)*w;
					norRect.y = (wordPosId / m_refImgClass[classid].wNum)*h;

					resimg.copyTo(m_refImgClass[classid].img[imgid](norRect));

					int clen = m_refImgClass[classid].maxCharLen;
					wchar_t* strcode = new wchar_t[clen];
					memset(strcode, 0x00, sizeof(wchar_t)*clen);
					memcpy(strcode, ocrRes[j].strCode, sizeof(wchar_t)*clen);
					m_refImgClass[classid].vecStr.push_back(strcode);

					m_refImgClass[classid].needToUpdate = true;
					pPage->UpdateOCRResStatus(j, false);
				}
			}
		}

	UpdateImgClassDB();
}

void CMNDataManager::DBTraining(CMNPageObject* pPage)
{
	//for (int i = 0; i < m_vecImgData.size(); i++) {
	//	std::vector<_stOCRResult> ocrRes = m_vecImgData[i]->GetVecOCRResult();
	std::vector<_stOCRResult> ocrRes = pPage->GetVecOCRResult();

		for (int j = 0; j < ocrRes.size(); j++) {
			if (ocrRes[j].bNeedToDB) {
				cv::Mat cutimg = pPage->GetSrcPageGrayImg()(ocrRes[j].rect).clone();
				cv::Rect norRect;// = GetNomalizedWordSize(ocrRes[j].rect);
				int classid = GetNomalizedWordSize(ocrRes[j].rect, norRect)-1;
				int imgid = m_refImgClass[classid].nCurrImgId;

				cv::resize(cutimg, cutimg, cvSize(norRect.width, norRect.height));
				// Verify to add into DB =====================================//
				bool IsAdd = IsNeedToAddDB(cutimg, ocrRes[j].strCode, classid);
				//===========================================================//

				if ((classid < 8) && (classid >= 0) && (IsAdd)) {
					int wordPosId = m_refImgClass[classid].vecStr.size();					
					
					int w = (classid+1) * DB_IMGCHAR_SIZE;
					int h = DB_IMGCHAR_SIZE;

					norRect.x = (wordPosId % m_refImgClass[classid].wNum)*w;
					norRect.y = (wordPosId / m_refImgClass[classid].wNum)*h;

					cutimg.copyTo(m_refImgClass[classid].img[imgid](norRect));
					
					int clen = m_refImgClass[classid].maxCharLen;
					wchar_t* strcode = new wchar_t[clen];
					memset(strcode, 0x00, sizeof(wchar_t)*clen);
					memcpy(strcode, ocrRes[j].strCode, sizeof(wchar_t)*clen);
					m_refImgClass[classid].vecStr.push_back(strcode);

					m_refImgClass[classid].needToUpdate = true;
					pPage->UpdateOCRResStatus(j, false);
				}
			}
		}
//	}
	UpdateImgClassDB();
}

void CMNDataManager::UpdateImgClassDB()
{
	USES_CONVERSION;
	for (int i = 0; i < DB_CLASS_NUM; i++) {
		if (m_refImgClass[i].needToUpdate) {
			int imgid = m_refImgClass[i].nCurrImgId;
			CString strFile;
			strFile.Format(L"%s//class%02d_%02d.jp2", m_strUserDataFolder, i, imgid);
			char* sz = T2A(strFile);
			cv::imwrite(sz, m_refImgClass[i].img[imgid]);

			strFile.Format(L"%s//class%02d_%02d.jpg", m_strUserDataFolder, i, imgid);
			sz = T2A(strFile);
			cv::imwrite(sz, m_refImgClass[i].img[imgid]);

			// Write image encode //
			strFile.Format(L"%s//class%02d.jp3", m_strUserDataFolder, i);
			sz = T2A(strFile);
			FILE* fp = 0;
			fopen_s(&fp, sz, "wb");

			int num = m_refImgClass[i].vecStr.size();
			int clen = m_refImgClass[i].maxCharLen;
			fwrite(&num, sizeof(int), 1, fp);
			fwrite(&clen, sizeof(int), 1, fp);
			for (int j = 0; j < num; j++) {
				fwrite(m_refImgClass[i].vecStr[j], sizeof(wchar_t)*clen, 1, fp);
			}
			fclose(fp);
		}
	}
}


void CMNDataManager::InitDataBaseFiles()
{
	USES_CONVERSION;	
	CString strFile;
	char* sz = 0;
	for (int i = 0; i < DB_CLASS_NUM; i++) {		

		// Read InfoFile First //
		strFile.Format(L"%s//class%02d.jp3", m_strUserDataFolder, i);
		sz = T2A(strFile);
		FILE* fp = 0;
		fopen_s(&fp, sz, "rb");
		if (fp) {			
			int num = 0, cLen = 0;
			fread(&num, sizeof(int), 1, fp);
			fread(&cLen, sizeof(int), 1, fp);
			for (int j = 0; j < num; j++) {
				wchar_t* code = new wchar_t[cLen];
				memset(code, 0x00, sizeof(wchar_t)*cLen);
				fread(code, sizeof(wchar_t)*cLen, 1, fp);
				m_refImgClass[i].vecStr.push_back(code);
			}
			fclose(fp);
		}
		//=====================================//


		int imgid = m_refImgClass[i].nCurrImgId;
		for (int j = 0; j < (imgid + 1); j++) {
			
			strFile.Format(L"%s//class%02d_%02d.jp2", m_strUserDataFolder, i, j);
			sz = T2A(strFile);

			m_refImgClass[i].img[j] = cv::imread(sz, CV_LOAD_IMAGE_GRAYSCALE);
			if (m_refImgClass[i].img[0].ptr() == NULL) {		// Create New DB Class image File //
				int w = m_refImgClass[i].wNum * DB_IMGCHAR_SIZE*(i + 1);
				int h = m_refImgClass[i].hNum * DB_IMGCHAR_SIZE;
				m_refImgClass[i].img[j] = cv::Mat(w, h, CV_8UC1, cv::Scalar(255));
				m_refImgClass[i].needToUpdate = true;
			}
		}
	}
}


void CMNDataManager::DeSkew(cv::Mat& img)
{
	cv::Mat tmpimg = img.clone();
	cv::bitwise_not(tmpimg, tmpimg);
	
	points.clear();
	findNonZero(tmpimg, points);
	cv::RotatedRect box = cv::minAreaRect(points);

	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	cv::Mat rotMat, rotatedFrame, invRot;
	rotMat = getRotationMatrix2D(cv::Point2f(tmpimg.cols*0.5f, tmpimg.rows*0.5f), angle, 1);
	cv::warpAffine(img, img, rotMat, img.size(), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
//	rotatedFrame.copyTo(img);

}

void CMNDataManager::AddSDBTable(unsigned int hcode, wchar_t* strCode)
{
	// Update HashTable //
	if (m_mapWordTable.find(hcode) == m_mapWordTable.end()) {
		_stSDBWordTable htable;
		memcpy(htable.str, strCode, sizeof(wchar_t)*(_MAX_WORD_SIZE));
		m_mapWordTable[hcode] = htable;
		m_bIsUpdateTable = true;
	}

	//// Add Filename code //
	//if (m_mapWordTable.find(_pcode) == m_mapWordTable.end()) {
	//	_stSDBWordTable htable;
	//	memcpy(htable.str, strPName.GetBuffer(), sizeof(wchar_t)*(_MAX_WORD_SIZE));
	//	m_mapWordTable[_pcode] = htable;
	//	m_bIsUpdateTable = true;
	//}

	//// Add SDB item //
	//if (m_mapSDB.find(item.strcode) == m_mapSDB.end()) {
	//	_stSDB sdb;
	//	sdb.push_back(item);
	//	m_mapSDB[item.strcode] = sdb;
	//}
	//else {
	//	m_mapSDB[item.strcode].push_back(item);
	//}
}

void CMNDataManager::LoadSDBFiles()
{
	USES_CONVERSION;
	CString strFile;
	char* sz = 0;


	// Read InfoFile First //
	strFile.Format(L"%s\\wtable.htbl", m_strUserDataFolder);
	sz = T2A(strFile);
	FILE* fp = 0;
	fopen_s(&fp, sz, "rb");
	if (fp) {
		int num = 0, cLen = 0;
		fread(&num, sizeof(int), 1, fp);
		unsigned int hcode;
		_stSDBWordTable strCode;	
		for (int j = 0; j < num; j++) {
			fread(&hcode, sizeof(unsigned int), 1, fp);
			fread(&strCode.str, sizeof(wchar_t)*_MAX_WORD_SIZE, 1, fp);
			m_mapWordTable[hcode] = strCode;

		}
		fclose(fp);
	}

	//=====================================//
	//strFile.Format(L"%s\\wdb.sdb", m_strUserDataFolder);
	//sz = T2A(strFile);

	//FILE* fpb = 0;
	//fopen_s(&fpb, sz, "rb");
	//if (fpb) {
	//	int wnum = 0;
	//	fread(&wnum, sizeof(int), 1, fpb);

	//	for (int i = 0; i < wnum; i++) {
	//		int pnum = 0;
	//		unsigned int hcode = 0;
	//		fread(&pnum, sizeof(int), 1, fpb);
	//		fread(&hcode, sizeof(unsigned int), 1, fpb);

	//		for (int j = 0; j < pnum; j++) {
	//			_stSDBWord sword;
	//			fread(&sword, sizeof(_stSDBWord), 1, fpb);
	//			m_mapSDB[hcode].push_back(sword);
	//		}
	//	}
	//	fclose(fpb);
	//}
}

void CMNDataManager::UpdateSDBFiles()
{
	USES_CONVERSION;

	CString strFile;
	char* sz = 0;
	// Write code Table if it is needed //
	if (m_bIsUpdateTable) {		
		strFile.Format(L"%s\\wtable.htbl", m_strUserDataFolder);
		sz = T2A(strFile);

		FILE* fp = 0;
		fopen_s(&fp, sz, "wb");
		if (fp) {
			int wnum = m_mapWordTable.size();
			std::map<unsigned int, _stSDBWordTable>::iterator iter= m_mapWordTable.begin();

			fwrite(&wnum, sizeof(int), 1, fp);
			for (; iter != m_mapWordTable.end(); iter++) {
				fwrite(&iter->first, sizeof(unsigned int), 1, fp);
				fwrite(&iter->second, sizeof(wchar_t)*_MAX_WORD_SIZE, 1, fp);
			}
			fclose(fp);
		}
	}
	//=================================================================

	//strFile.Format(L"%s\\wdb.sdb", m_strUserDataFolder);
	//sz = T2A(strFile);

	//FILE* fp = 0;
	//fopen_s(&fp, sz, "wb");
	//if (fp) {
	//	int wnum = m_mapSDB.size();
	//	fwrite(&wnum, sizeof(int), 1, fp);
	//	std::map<unsigned int, _stSDB>::iterator iter = m_mapSDB.begin();
	//	for (; iter != m_mapSDB.end(); iter++) {

	//		int pnum = iter->second.size();
	//		fwrite(&pnum, sizeof(unsigned int), 1, fp);
	//		fwrite(&iter->first, sizeof(unsigned int), 1, fp);

	//		for (int i=0; i<pnum; i++) {
	//			fwrite(&iter->second[i], sizeof(_stSDBWord), 1, fp);
	//		}
	//	}
	//	fclose(fp);
	//}
}

void CMNDataManager::DoKeywordSearch(CString strKeyword)
{
	// clear previous results //
	ClearAllImages();
	std::vector<CString> vecWord;

	int nIndex = strKeyword.ReverseFind(_T(' '));
	while (nIndex > 0) {
		vecWord.push_back(strKeyword.Left(nIndex));

		int len = strKeyword.GetLength();
		strKeyword = strKeyword.Right(len - (nIndex + 1));
		nIndex = strKeyword.ReverseFind(_T(' '));
	}
	vecWord.push_back(strKeyword);
	

	std::map<unsigned long, CMNPageObject*>::iterator iter = m_mapImageData.begin();
	for (; iter != m_mapImageData.end(); iter++) {
		iter->second->ClearDBSearchResult();
	}
	//=========================================================//

	USES_CONVERSION;

	char char_str[_MAX_WORD_SIZE * 2];
	memset(char_str, 0x00, _MAX_WORD_SIZE * 2);
	int char_str_len = WideCharToMultiByte(CP_ACP, 0, strKeyword.GetBuffer(), -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strKeyword.GetBuffer(), -1, char_str, char_str_len, 0, 0);

	unsigned int hcode = getHashCode(char_str);

	if (m_mapGlobalSDB.find(hcode) != m_mapGlobalSDB.end()) {
		for (int i = 0; i < m_mapGlobalSDB[hcode].size(); i++) {
			_stSDBWord res = m_mapGlobalSDB[hcode][i];

			if (m_mapFilePathTable.find(res.filecode) != m_mapFilePathTable.end()) {
				CString strPath = m_mapFilePathTable[res.filecode];
				if (m_mapWordTable.find(res.strcode) != m_mapWordTable.end()) {
					_stSDBWordTable tmpstr = m_mapWordTable[res.strcode];

					//==================================================================//
					if (m_mapImageData.find(res.filecode) != m_mapImageData.end()) {
						CMNPageObject* pPage = m_mapImageData[res.filecode];
						pPage->AddDBSearchResult(res.rect);
					}
					else {
						// Load file and get pointer !!//

						CString strPName, strName;
						int nIndex = strPath.ReverseFind(_T('\\'));
						int len = strPath.GetLength();
						if (nIndex > 0) {
							strPName = strPath.Left(nIndex);
							strName = strPath.Right(len - (nIndex + 1));
						}
						char* sz = 0;
						sz = T2A(strPName);
						unsigned long pCode = getHashCode(sz);

						CMNPageObject* pPage = PushImageDataSet(strPath, strPName, strName, res.filecode, pCode);
						pPage->LoadThumbImage(THUMBNAIL_SIZE);
						pPage->UploadThumbImage();
						SelectPages(pCode);

						pPage->AddDBSearchResult(res.rect);
					}
					//================================================================//
				}
			}			
		}
	}	
}

void CMNDataManager::InitSDB(CString strPath, CString strName)
{
	USES_CONVERSION;
	char* sz = T2A(strPath);
	unsigned long hcode = getHashCode(sz);

	if (m_mapFilePathTable.find(hcode) == m_mapFilePathTable.end()) {
		m_mapFilePathTable[hcode] = strPath;
	}
	else {
		AfxMessageBox(L"Assert!! Error in generating file path table.");
	}	
	
	// Generate SDB //
	CString path, filename, sdbPath;
	int nIndex = strPath.ReverseFind(_T('\\'));
	int len = strPath.GetLength();
	if (nIndex > 0) {
		path = strPath.Left(nIndex);
	}

	nIndex = strName.ReverseFind(_T('.'));
	if (nIndex > 0) {
		filename = strName.Left(nIndex);
	}
	sdbPath = path + L"\\moon_db\\" + filename + L".sdb";

	sz = T2A(sdbPath);
	FILE* fpb = 0;
	fopen_s(&fpb, sz, "rb");
	if (fpb) {
		int wnum = 0;
		fread(&wnum, sizeof(int), 1, fpb);

		for (int i = 0; i < wnum; i++) {
			int pnum = 0;
			unsigned int hcode = 0;
			fread(&pnum, sizeof(int), 1, fpb);
			fread(&hcode, sizeof(unsigned int), 1, fpb);

			for (int j = 0; j < pnum; j++) {
				_stSDBWord sword;
				fread(&sword, sizeof(_stSDBWord), 1, fpb);
				m_mapGlobalSDB[hcode].push_back(sword);
			}
		}
		fclose(fpb);
	}
}


CString CMNDataManager::GetEditFilePath(CString strExtension, CString strOrigin)
{
	CString path, strFolder, strFile;
	int nIndex = strOrigin.ReverseFind(_T('\\'));
	if (nIndex > 0) {
		strFolder = strOrigin.Left(nIndex);
	}
	int len = strOrigin.GetLength();
	strFile = strOrigin.Right(len - (nIndex + 1));

	nIndex = strFile.ReverseFind(_T('.'));
	if (nIndex > 0) {
		strFile = strFile.Left(nIndex);
	}	
	
	//	filename += strExtension;	
	path = strFolder + L"\\moon_db\\" + strFile + strExtension;
	return path;
}









void CMNDataManager::ExportDatabase()
{
	CFileDialog dlg(FALSE, L"*.csv", NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, L"CSV Files(*.csv)|*.csv|");
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		CFile cfile;
		if (!cfile.Open(strPath, CFile::modeWrite | CFile::modeCreate))
		{
			return;
		}
		USHORT nShort = 0xfeff;  // 유니코드 바이트 오더마크.
		cfile.Write(&nShort, 2);

		CString strRecord = L"CODE POS_X POS_Y SIZE CONFIDENCE DIFFERENCE BASE64\n";
		int len = wcslen(strRecord.GetBuffer()) * 2;		// word code //
		cfile.Write(strRecord.GetBuffer(), len);

		//====================================================================//
		std::map<unsigned int, _stSDB>::iterator iter = m_mapGlobalSDB.begin();
		unsigned int filecode = 0;
		for (; iter != m_mapGlobalSDB.end(); iter++) {

			cv::Mat pageImg;
			CString strPath, filePath, strtmp;

			for (int i = 0; i < iter->second.size(); i++) {
				//	for (int i = 0; i < 1; i++) {
				_stSDBWord res = iter->second[i];

				if (filecode != res.filecode) {		// new filecode;
					pageImg.release();
					filecode = res.filecode;
				}

				if (m_mapFilePathTable.find(res.filecode) != m_mapFilePathTable.end()) {
					if (pageImg.ptr() == NULL) {
						strPath = m_mapFilePathTable[res.filecode];
						filePath = GetEditFilePath(L".jp2", strPath);
						LoadImageData(filePath, pageImg, true);
					}

					if (m_mapWordTable.find(res.strcode) != m_mapWordTable.end()) {
						cv::Mat cutImg = pageImg(res.rect).clone();

						std::vector<uchar> data_encode;
						imencode(".bmp", cutImg, data_encode);
						CString strBase64 = base64_encode((unsigned char*)&data_encode[0], data_encode.size());
						data_encode.clear();

						_stSDBWordTable tmpstr = m_mapWordTable[res.strcode];
						CString strWord = tmpstr.str;
						//		TRACE(L"%s---%s, %d, %d, %d, %3.2f, %3.2f, \n", filePath, strWord, res.rect.x, res.rect.y, res.rect.width, res.fConfi, res.fDiff);

						int len = wcslen(strWord.GetBuffer()) * 2;		// word code //
						cfile.Write(strWord.GetBuffer(), len);
						cfile.Write(L" ", 2);

						strtmp.Format(L"%d", res.rect.x);		// position x
						len = wcslen(strtmp.GetBuffer()) * 2;
						cfile.Write(strtmp.GetBuffer(), len);
						cfile.Write(L" ", 2);

						strtmp.Format(L"%d", res.rect.y);		// position y
						len = wcslen(strtmp.GetBuffer()) * 2;
						cfile.Write(strtmp.GetBuffer(), len);
						cfile.Write(L" ", 2);

						strtmp.Format(L"%d", res.rect.width);		// size
						len = wcslen(strtmp.GetBuffer()) * 2;
						cfile.Write(strtmp.GetBuffer(), len);
						cfile.Write(L" ", 2);

						strtmp.Format(L"%3.2f", res.fConfi);		// confidence
						len = wcslen(strtmp.GetBuffer()) * 2;
						cfile.Write(strtmp.GetBuffer(), len);
						cfile.Write(L" ", 2);

						strtmp.Format(L"%3.2f", res.fDiff);		// confidence
						len = wcslen(strtmp.GetBuffer()) * 2;
						cfile.Write(strtmp.GetBuffer(), len);
						cfile.Write(L" ", 2);

						len = wcslen(strBase64.GetBuffer()) * 2;
						cfile.Write(strBase64.GetBuffer(), len);
						//cfile.Write(L",", 2);

						cfile.Write(L"\r\n", 4);
						cutImg.release();
					}
				}
			}
			pageImg.release();
		}
		cfile.Close();

		AfxMessageBox(L"A csv file was exported");
	}		
}

