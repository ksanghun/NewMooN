#pragma once
#include "MNCVMng.h"

typedef struct _extractBox {
	cv::Rect textbox;
	cv::Rect textboxForCheck;
	bool IsMerged;

	void init() {
		IsMerged = false;
	};

	void setExtendBox(int w, int h)
	{
		textboxForCheck = textbox;
		textboxForCheck.x -= w;
		if (textboxForCheck.x < 0)	textboxForCheck.x = 0;
		textboxForCheck.width += w * 2;
		if (textboxForCheck.width < 1)	textboxForCheck.width= 1;

		textboxForCheck.y -= h;
		if (textboxForCheck.y < 0)	textboxForCheck.y = 0;
		textboxForCheck.height += h * 2;
		if (textboxForCheck.height < 1)	textboxForCheck.height = 1;
	};
}_extractBox;




class CExtractor
{
public:
	CExtractor();
	~CExtractor();


	float DeSkewImg(cv::Mat& img);
	_ALIGHN_TYPE CheckHoughLines(cv::Mat& img);

	// Extract Paragraph //
	
	void ExtractLines(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox, _LANGUAGE_TYPE languageType, _ALIGHN_TYPE align);
	void DetectBoundary(std::vector<std::vector<cv::Point> >& contour, std::vector<_extractBox>& vecBox, int xMargin, int yMargin, _LANGUAGE_TYPE languageType, _ALIGHN_TYPE align);
//	bool RcvMeargingtBoundaryBox(std::vector<_extractBox>& vecBox, int& depth, float xTh, float yTh, int xMargin, int yMargin, _LANGUAGE_TYPE languageType);
	int FindOptimalBox(std::vector<_extractBox>& tmp, int i, float xTh, float yTh, _extractBox& resBox);
	int IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2);
	void verifyImgSize(cv::Rect& rect, int imgwidth, int imgheight);

	// Histogram ==//
	cv::Mat GetLinesbyHistogram(cv::Mat& img, std::vector<_extractBox>& vecline, int t);

	// Extraction //
	void Extraction(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox);
	void ExtractionText(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox, bool IsVerti);

	bool MeargingtBoundaryBox(std::vector<_extractBox>& vecBox, int& depth);
	bool MeargingtBoundaryBoxTextVerti(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth);
	//bool MeargingtBoundaryBoxTextHori(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth);
	bool MeargingtBoundaryBoxText(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth);

	void SortBoundaryBox(std::vector<_extractBox>& vecBox);
	int MinDist(cv::Rect r1, cv::Rect r2);

	void TestFunc();
	void InitializeContourVectors();

	
	std::vector<cv::Vec4i> total_lines;
	std::vector<cv::Point> points;
	std::vector<std::vector<cv::Point> > contours_poly;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	void SetFontSize(int _w, int _h) { m_fontSize.width = _w; m_fontSize.height = _h; }
	cv::Size GetFontSize() { return m_fontSize; }
	cv::Size m_fontSize;

};

