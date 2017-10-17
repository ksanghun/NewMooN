#pragma once

// tessract library //
#include <baseapi.h>
#include <allheaders.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "data_type.h"


enum _DETECT_MODE { _DET_WORD, _DET_CHAR};


class COCRMng
{
public:
	COCRMng();
	~COCRMng();

	bool InitOCRMng();
	float COCRMng::extractWithOCR(cv::Mat image, std::vector<_stOCRResult>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale);
	void Utf8ToUnicode(char* szU8, wchar_t* strwchar);
	void SetOCRDetectModeEng(tesseract::PageSegMode mode);
	void SetOCRDetectModeChi(tesseract::PageSegMode mode);
	void SetOCRDetectModeKor(tesseract::PageSegMode mode);

	tesseract::TessBaseAPI& GetEngTess() { return m_tessEng; }
	tesseract::TessBaseAPI& GetChiTess() { return m_tessChi; };
	tesseract::TessBaseAPI& GetKorTess() { return m_tessKor; };

	void TestFunc();

private:
	tesseract::TessBaseAPI m_tessEng;
	tesseract::TessBaseAPI m_tessChi;	
	tesseract::TessBaseAPI m_tessKor;

};

