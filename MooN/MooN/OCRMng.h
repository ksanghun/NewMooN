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
	float COCRMng::extractWithOCR(cv::Mat image, std::vector<_stOCRResult>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level);
	wchar_t * Utf8ToUnicode(char* szU8);
	void SetOCRDetectMode(_DETECT_MODE mode);

	tesseract::TessBaseAPI& GetEngTess() { return m_tessEng; }
	tesseract::TessBaseAPI& GetChiTess() { return m_tessChi; };

	void TestFunc();

private:
	tesseract::TessBaseAPI m_tessEng;
	tesseract::TessBaseAPI m_tessChi;	

};

