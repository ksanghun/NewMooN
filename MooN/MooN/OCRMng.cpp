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
	//m_tessEng.End();
	//m_tessChi.End();
	//m_tessKor.End();
	for (int i = 0; i < _NUM_LANGUAGE_TYPE; i++) {
		m_tess[i].End();
	}
}


bool COCRMng::InitOCRMng()
{
	//if (m_tessEng.Init("./tessdata/", "eng")){
	//	return false;
	//}
	//if (m_tessChi.Init("./tessdata/", "chi_tra")){
	//	return false;
	//}
	//if (m_tessKor.Init("./tessdata/", "kor")) {
	//	return false;
	//}

	m_tess[__ENG].Init("./tessdata/", "eng");
	m_tess[__CHI].Init("./tessdata/", "chi_tra");
	m_tess[__KOR].Init("./tessdata/", "kor");
//	m_tess[__JAP].Init("./tessdata/", "jap");


//	m_tessEng.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//	m_tessEng.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz~!@#$%^&*()_-+={}[]:;'<>?,./\|");
//	m_tessChi.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
//	m_tessChi.SetVariable("tessedit_char_blacklist", "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(){}[]~!@#$%^&*()_+-=,.?;:'\"");

	for (int i = 0; i < _NUM_LANGUAGE_TYPE; i++) {
		SetOCRDetectMode((_LANGUAGE_TYPE)i, tesseract::PSM_SINGLE_BLOCK);
	}
	//m_tess[__CHI].SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	


	//SetOCRDetectModeEng(tesseract::PSM_SINGLE_BLOCK);
	//SetOCRDetectModeChi(tesseract::PSM_SINGLE_BLOCK);
	//SetOCRDetectModeKor(tesseract::PSM_SINGLE_BLOCK);
//	m_tessEng.SetVariable("tessedit_char_blacklist", ":;()[]{}!?");
//	m_tessChi.SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
//	m_tessChi.SetVariable("tessedit_char_blacklist", "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(){}[]~!@#$%^&*()_+-=,.?;:'\"");
//	m_tessKor.SetVariable("tessedit_char_blacklist", "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(){}[]~!@#$%^&*()_+-=,.?;:'\"");

	return true;
}

//void COCRMng::SetOCRDetectModeEng(tesseract::PageSegMode mode)
//{
//	m_tessEng.SetPageSegMode(mode);
//}
//
//void COCRMng::SetOCRDetectModeChi(tesseract::PageSegMode mode)
//{
//	m_tessChi.SetPageSegMode(mode);
//}
//void COCRMng::SetOCRDetectModeKor(tesseract::PageSegMode mode)
//{
//	m_tessKor.SetPageSegMode(mode);
//}

void COCRMng::SetOCRDetectMode(_LANGUAGE_TYPE _type, tesseract::PageSegMode mode)
{
	m_tess[_type].SetPageSegMode(mode);
}

float COCRMng::extractWithOCR(cv::Mat& image, std::vector<_stOCRResult>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale, int langType)
{	
	// Preprocessing before OCR ====//
//	fScale = 1.0f;
	//cv::threshold(image, image, 200, 255, cv::THRESH_OTSU);
	//cv::Mat resizeImg;
	//cv::resize(image, resizeImg, cv::Size(32, 32));
	//cv::imshow("resize1", resizeImg);
	//cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
	//	cv::Size(3, 3),
	//	cv::Point(-1, -1));
	//cv::dilate(image, image, element);
	//cv::imshow("resize2", resizeImg);

	//======================//

//	tess.SetImage((uchar*)resizeImg.data, resizeImg.size().width, resizeImg.size().height, resizeImg.channels(), resizeImg.step1());
//	tess.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
	tess.SetImage((uchar*)image.data, image.cols, image.rows, image.channels(), static_cast<int>(image.step1()));
//	tess.SetRectangle(0, 0, image.cols, image.rows);
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

		//	if (word != 0) {

				//tesseract::ChoiceIterator ci(*ri);
				//do{
				//	const char* choice = ci.GetUTF8Text();
				//	float ciConf = ci.Confidence();
				//	TRACE("Confidence: %s - %3.2f\n", choice, ciConf);
				//} while (ci.Next());


				int x1, y1, x2, y2, w, h;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				//x1 /= fScale;
				//x2 /= fScale;
				//y1 /= fScale;
				//y2 /= fScale;

				w = x2 - x1;
				h = y2 - y1;

				_stOCRResult res;

				res.init();
				res.type = langType;
				res.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
				res.fConfidence = conf*0.01f;

				if ((word) && (w > 2) && (h > 2)) {
					Utf8ToUnicode(word, res.strCode);

					boundRect.push_back(res);
					TRACE(L"Confidence: %s - %3.2f\n", res.strCode, res.fConfidence);
					averConf += conf;
					cnt++;

					delete[] word;
				}

				//res.strCode = tword;						
				//SINGLETON_DataMng::GetInstance()->MultiToUniCode(word, res.strCode);
				//boundRect.push_back(res);
				//TRACE(L"Confidence: %s - %3.2f\n", res.strCode, res.fConfidence);
				//averConf += conf;
				//cnt++;
				
		//	}

		} while (ri->Next(level));

		delete ri;
	}


	tess.Clear();
//	resizeImg.release();

	if (cnt > 0)
		averConf /= cnt;
	if (langType == __CHI)
		averConf += 10;
	return averConf*0.01f;

	// Need to return the lowest value //
}


float COCRMng::extractWithOCRSingle(cv::Mat& image, std::vector<_stOCRResult>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale, int langType)
{
	//cv::Mat resizeImg = cv::Mat(cvSize(image.cols+20, image.rows+20), image.type());
	//resizeImg.setTo(255);
	//image.copyTo(resizeImg(cv::Rect(10, 10, image.cols, image.rows)));

	// Preprocessing before OCR ====//
	//	fScale = 1.0f;
	//cv::threshold(image, image, 200, 255, cv::THRESH_OTSU);
	//cv::Mat resizeImg;
	//cv::resize(image, resizeImg, cv::Size(32, 32));
	//cv::imshow("resize1", resizeImg);
	//cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
	//	cv::Size(3, 3),
	//	cv::Point(-1, -1));
	//cv::dilate(image, image, element);
	//cv::imshow("resize2", resizeImg);

	//======================//

	//tess.SetImage((uchar*)resizeImg.data, resizeImg.cols, resizeImg.rows, resizeImg.channels(), resizeImg.step1());
	//	tess.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
	tess.SetImage((uchar*)image.data, image.cols, image.rows, image.channels(), static_cast<int>(image.step1()));
	tess.SetRectangle(0, 0, image.cols, image.rows);
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

			//	if (word != 0) {

			//tesseract::ChoiceIterator ci(*ri);
			//do{
			//	const char* choice = ci.GetUTF8Text();
			//	float ciConf = ci.Confidence();
			//	TRACE("Confidence: %s - %3.2f\n", choice, ciConf);
			//} while (ci.Next());


			//int x1, y1, x2, y2, w, h;
			//ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			////x1 /= fScale;
			////x2 /= fScale;
			////y1 /= fScale;
			////y2 /= fScale;
			//w = x2 - x1;
			//h = y2 - y1;

			_stOCRResult res;

			res.init();
			res.type = langType;
		//	res.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
			res.rect = cv::Rect(0, 0, image.cols, image.rows);


			res.fConfidence = conf*0.01f;

			if ((word)) {
				Utf8ToUnicode(word, res.strCode);

				boundRect.push_back(res);
				TRACE(L"Confidence: %s - %3.2f\n", res.strCode, res.fConfidence);
				averConf += conf;
				cnt++;

				delete[] word;
			}

			//res.strCode = tword;						
			//SINGLETON_DataMng::GetInstance()->MultiToUniCode(word, res.strCode);
			//boundRect.push_back(res);
			//TRACE(L"Confidence: %s - %3.2f\n", res.strCode, res.fConfidence);
			//averConf += conf;
			//cnt++;

			//	}

		} while (ri->Next(level));

		delete ri;
	}


	tess.Clear();
	//	resizeImg.release();

	if (cnt > 0)
		averConf /= cnt;
	return averConf*0.01f;

	// Need to return the lowest value //
}


void COCRMng::Utf8ToUnicode(char* szU8, wchar_t* strwchar)
{
	int wcsLen =::MultiByteToWideChar(CP_UTF8, NULL, szU8, static_cast<int>(strlen(szU8)), NULL, 0);
//	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, static_cast<int>(strlen(szU8)), strwchar, wcsLen);
	strwchar[wcsLen] = '\0';
//	return wszString;
}


void COCRMng::TestFunc()
{
	//for (int i = 1; i < 3; i++) {
	//	CString strFile;
	//	strFile.Format(L"D:/%d.jpg", i);
	//	USES_CONVERSION;
	//	char* sz = T2A(strFile);

	//	std::vector<_stOCRResult> boundRect;
	//	cv::Mat image = cv::imread(sz);
	//	cv::cvtColor(image, image, CV_BGR2GRAY);
	//	cv::threshold(image, image, 128, 255, cv::THRESH_BINARY);
	////	cv::imshow("Binary", image);

	//	extractWithOCR(image, boundRect, GetEngTess(), tesseract::RIL_WORD, 1.0f, 0);

	//	for (int i = 0; i < boundRect.size(); i++) {
	//		cv::Rect r = boundRect[i].rect;
	//		rectangle(image, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 0, 255), 2);
	//	}

	//	boundRect.swap(std::vector<_stOCRResult>());
	////	cv::imshow(sz, image);
	//	image.release();
	//}

}




_stOCRResult COCRMng::getOcrResFromSingleCut(cv::Mat& image, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale, int langType)
{
	tess.SetImage((uchar*)image.data, image.cols, image.rows, image.channels(), static_cast<int>(image.step1()));
	tess.SetRectangle(0, 0, image.cols, image.rows);
	tess.Recognize(0);

	_stOCRResult res;
	tesseract::ResultIterator* ri = tess.GetIterator();
	if (ri != 0) {
		do {
			char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);

			res.init();
			res.type = langType;
			//	res.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
			res.rect = cv::Rect(0, 0, image.cols, image.rows);


			res.fConfidence = conf*0.01f;

			if ((word)) {
				Utf8ToUnicode(word, res.strCode);
				delete[] word;
			}
		} while (ri->Next(level));

		delete ri;
	}

	tess.Clear();
	return res;
}
