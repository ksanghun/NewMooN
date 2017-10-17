#include "stdafx.h"
#include "OCRMng.h"
#include "MNCVMng.h"
#include "MNDataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COCRMng::COCRMng()
{
}


COCRMng::~COCRMng()
{
	m_tessEng.End();
	m_tessChi.End();
	m_tessKor.End();
}


bool COCRMng::InitOCRMng()
{
	if (m_tessEng.Init("./tessdata/", "eng")){
		return false;
	}
	if (m_tessChi.Init("./tessdata/", "chi_tra")){
		return false;
	}
	if (m_tessKor.Init("./tessdata/", "kor")) {
		return false;
	}

//	m_tessEng.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//	m_tessEng.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz~!@#$%^&*()_-+={}[]:;'<>?,./\|");
//	m_tessChi.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
//	m_tessChi.SetVariable("tessedit_char_blacklist", "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(){}[]~!@#$%^&*()_+-=,.?;:'\"");

	SetOCRDetectModeEng(tesseract::PSM_SINGLE_BLOCK);
	SetOCRDetectModeChi(tesseract::PSM_SINGLE_BLOCK);
	SetOCRDetectModeKor(tesseract::PSM_SINGLE_BLOCK);

	return true;
}


void COCRMng::SetOCRDetectModeEng(tesseract::PageSegMode mode)
{
	m_tessEng.SetPageSegMode(mode);
}

void COCRMng::SetOCRDetectModeChi(tesseract::PageSegMode mode)
{
	m_tessChi.SetPageSegMode(mode);
}
void COCRMng::SetOCRDetectModeKor(tesseract::PageSegMode mode)
{
	m_tessKor.SetPageSegMode(mode);
}

float COCRMng::extractWithOCR(cv::Mat image, std::vector<_stOCRResult>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale)
{	
	// Preprocessing before OCR ====//
//	fScale = 1.0f;

	//cv::threshold(image, image, 200, 255, cv::THRESH_OTSU);
	cv::Mat resizeImg;
	cv::resize(image, resizeImg, cv::Size(image.cols * fScale, image.rows * fScale));
	//cv::imshow("resize1", resizeImg);
	//cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
	//	cv::Size(3, 3),
	//	cv::Point(-1, -1));
	//cv::dilate(image, image, element);
	//cv::imshow("resize2", resizeImg);

	//======================//

	tess.SetImage((uchar*)resizeImg.data, resizeImg.size().width, resizeImg.size().height, resizeImg.channels(), resizeImg.step1());
//	tess.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
	tess.Recognize(0);

	//	const char* out = tess.GetUTF8Text();
		//tesseract::PageIteratorLevel level;
		//level = tesseract::RIL_SYMBOL;

	float averConf = 0.0f;
	int cnt = 0;

	tesseract::ResultIterator* ri = tess.GetIterator();
	if (ri != 0) {
		do {
			char* word = ri->GetUTF8Text(level);

			float conf = ri->Confidence(level);
			//	if (conf > 94.99f) continue;

			if (word != 0) {

				//tesseract::ChoiceIterator ci(*ri);
				//do{
				//	const char* choice = ci.GetUTF8Text();
				//	float ciConf = ci.Confidence();
				//	TRACE("Confidence: %s - %3.2f\n", choice, ciConf);
				//} while (ci.Next());


				int x1, y1, x2, y2, w, h;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				x1 /= fScale;
				x2 /= fScale;
				y1 /= fScale;
				y2 /= fScale;

				w = x2 - x1;
				h = y2 - y1;

				if ((word) && (w > 2) && (h > 2)) {
					_stOCRResult res;
					res.init();
					

					res.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
					res.fConfidence = conf*0.01f;

					Utf8ToUnicode(word, res.strCode);
					//res.strCode = tword;						
					//SINGLETON_DataMng::GetInstance()->MultiToUniCode(word, res.strCode);
					boundRect.push_back(res);

					TRACE(L"Confidence: %s - %3.2f\n", res.strCode, res.fConfidence);
				}

				averConf += conf;
				delete[] word;
			}

		} while (ri->Next(level));

		delete ri;
	}


	tess.Clear();
	resizeImg.release();

	if (cnt > 0)
		averConf /= cnt;
	return averConf;
}


void COCRMng::Utf8ToUnicode(char* szU8, wchar_t* strwchar)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
//	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), strwchar, wcsLen);
	strwchar[wcsLen] = '\0';
//	return wszString;
}


void COCRMng::TestFunc()
{
	for (int i = 1; i < 3; i++) {
		CString strFile;
		strFile.Format(L"D:/%d.jpg", i);
		USES_CONVERSION;
		char* sz = T2A(strFile);

		std::vector<_stOCRResult> boundRect;
		cv::Mat image = cv::imread(sz);
		cv::cvtColor(image, image, CV_BGR2GRAY);
		cv::threshold(image, image, 125, 255, cv::THRESH_BINARY);
	//	cv::imshow("Binary", image);

		extractWithOCR(image, boundRect, GetEngTess(), tesseract::RIL_WORD, 1.0f);

		for (int i = 0; i < boundRect.size(); i++) {
			cv::Rect r = boundRect[i].rect;
			rectangle(image, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 0, 255), 2);
		}

		boundRect.clear();
		cv::imshow(sz, image);
		image.release();
	}

}