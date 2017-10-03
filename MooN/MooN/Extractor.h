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

		textboxForCheck.y -= h;
		if (textboxForCheck.y < 0)	textboxForCheck.y = 0;
		textboxForCheck.height += h * 2;
	};
}_extractBox;




class CExtractor
{
public:
	CExtractor();
	~CExtractor();


	float DeSkewImg(cv::Mat& img);
	_ALIGHN_TYPE AllHoriVertLines(cv::Mat& img);

	// Extract Paragraph //
	void ExtractParagraph(cv::Mat& binaryImg, int mergeWidth, int mergeHeight, std::vector<_extractBox>& vecBox, _LANGUAGE_TYPE languageType);
	void DetectBoundary(std::vector<std::vector<cv::Point> >& contour, std::vector<_extractBox>& vecBox, int maxWidth, int maxHeight, int extX, int extY, _LANGUAGE_TYPE languageType);
	bool RcvMeargingtBoundaryBox(int maxwidth, int maxheight, std::vector<_extractBox>& vecBox, int& depth, int extX, int extY, _LANGUAGE_TYPE languageType);
	int FindOptimalBox(std::vector<_extractBox>& tmp, int i, int maxwidth, int maxheight, _extractBox& resBox);
	int IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2);
	void verifyImgSize(cv::Rect& rect, int imgwidth, int imgheight);
	void TestFunc();


	std::vector<std::vector<cv::Point> > contours_poly;
	std::vector<cv::Vec4i> total_lines;
	std::vector<cv::Point> points;
};

