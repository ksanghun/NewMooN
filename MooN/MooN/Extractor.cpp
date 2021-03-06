#include "stdafx.h"
#include "Extractor.h"
#include "math_tool.h"


CExtractor::CExtractor()
{
	m_fontSize.width = 0; 
	m_fontSize.height = 0;
}


CExtractor::~CExtractor()
{
	InitializeContourVectors();
	total_lines.swap(std::vector<cv::Vec4i>());
//	points.swap(std::vector<cv::Point>());
}

void CExtractor::TestFunc()
{
//	cv::Mat input = cv::imread("D:/Untitled.bmp");
	//cv::Mat input = cv::imread("D:/17_212.jp2");
	//cv::imshow("kim", input);
//	DeSkewImg(input);
//	AllHoriVertLines(input);
//	input.release();
}

float CExtractor::DeSkewImg(cv::Mat& img)
{
	//cv::Mat tmp;
	//cvtColor(img, tmp, CV_BGR2GRAY);
	//cv::threshold(tmp, tmp, 125, 255, cv::THRESH_OTSU);
	//cv::bitwise_not(tmp, tmp);
//	points.swap(std::vector<cv::Point>());

	cv::Mat locations;
	findNonZero(img, locations);
//	findNonZero(img, points);
	cv::RotatedRect box = cv::minAreaRect(locations);

	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	return (float)angle;

	//cv::Point2f vertices[4];
	//box.points(vertices);
	//for (int i = 0; i < 4; ++i)
	//	cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0));


	//imshow("img ", img);

	//// Rotate image //
	//cv::Mat rotMat, rotatedFrame;
	//rotMat = getRotationMatrix2D(cv::Point2f(0, 0), angle, 1);
	//cv::warpAffine(img, rotatedFrame, rotMat, img.size(), cv::INTER_CUBIC);
	//imshow("img_rot", rotatedFrame);
}

_ALIGHN_TYPE CExtractor::CheckHoughLines(cv::Mat& binaryImg)
{
	cv::Mat dst, cdst;
	
//	cvtColor(dst, cdst, CV_GRAY2BGR);


//	cv::medianBlur(tmp, tmp, 5);
//	int erosion_size = 2;
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
//		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
//		cv::Point(erosion_size, erosion_size));
//	cv::erode(tmp, tmp, element);
////	cv::dilate(tmp, tmp, element);

	//cv::Mat resizeImg;
//	float fscale = 256.0f / (float)binaryImg.rows;
	cv::Size size = binaryImg.size();
	cv::resize(binaryImg, dst, cv::Size(size.width/2,size.height/2));
//	cv::Canny(dst, dst, 50, 200, 3);
	cv::GaussianBlur(dst, dst, cv::Size(9, 9), 2, 2);

	total_lines.swap(std::vector<cv::Vec4i>());
	


//	cv::imshow("img", dst);

	size = dst.size();
	cv::HoughLinesP(dst, total_lines, 1, CV_PI / 180, 150, 50,30);

//	cv::Mat disp_lines(size, CV_8UC3, cv::Scalar(0, 0, 0));

	POINT3D vecAlign, vecBase;
	mtSetPoint3D(&vecAlign, 0, 0, 0);
	mtSetPoint3D(&vecBase, 1, 0, 0);


	 for (unsigned i = 0; i < total_lines.size(); ++i)
	 {
		 POINT3D v;
		 v.x = static_cast<float>(total_lines[i][2] - total_lines[i][0]);
		 v.y = static_cast<float>(total_lines[i][3] - total_lines[i][1]);
		 v.z = 0.0f;

		 vecAlign = vecAlign+v;

	     //cv::line(disp_lines, 
	     //         cv::Point(total_lines[i][0], total_lines[i][1]),
	     //         cv::Point(total_lines[i][2], total_lines[i][3]), 
	     //         cv::Scalar(255, 0 ,0));
	 }
//	 cv::imshow("total_lines.png", disp_lines);

	 if (total_lines.size() == 0) {
		 return _UNKNOWN_ALIGN;
	 }
	 else {
		 vecAlign = mtNormalize(vecAlign);
		 float cos = mtDot(vecAlign, vecBase);

		 if (cos < 0)	cos *= -1.0f;
		 if (cos > 0.7f)
			 return _HORIZON_ALIGN;
		 else
			 return _VERTICAL_ALIGN;		 
	 }	
}

void CExtractor::InitializeContourVectors()
{
	//for (auto i = 0; i < contours_poly.size(); i++) {
	//	contours_poly[i].resize(0);
	//}
	//contours_poly = std::vector<std::vector<cv::Point>>();

	contours_poly.swap(std::vector<std::vector<cv::Point>>());
	contours.swap(std::vector<std::vector<cv::Point>>());
	hierarchy.swap(std::vector<cv::Vec4i>());

	//for (auto i = 0; i < contours.size(); i++) {
	//	contours[i].resize(0);
	//}
	//contours.resize(0);
//	contours = std::vector<std::vector<cv::Point>>();
//	hierarchy.clear();
}

void CExtractor::Extraction(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox)
{
	InitializeContourVectors();
	/// Find contours

	cv::findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles
	
	contours_poly.resize(contours.size());
	for (int i = 0; i < contours.size(); i++){
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		_extractBox textBox;
		textBox.init();
		textBox.textbox = cv::boundingRect(cv::Mat(contours_poly[i]));
		textBox.setExtendBox(xMargin, yMargin);			
		//// check aspect ratio //
		float arw = (float)textBox.textbox.width / (float)textBox.textbox.height;
		float arh = (float)textBox.textbox.height / (float)textBox.textbox.width;
		if ((arw < 0.1f) || (arh < 0.1f)) {
			continue;
		}		
		vecBox.push_back(textBox);
	}

	int depth = 0;
	MeargingtBoundaryBox(vecBox, depth);
}

bool CExtractor::MeargingtBoundaryBox(std::vector<_extractBox>& vecBox, int& depth)
{
	std::vector<_extractBox> tmp = vecBox;
	vecBox.swap(std::vector<_extractBox>());

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_extractBox resBox;
	for (int i = 0; i < tmp.size(); i++) {
		//	for (int i = tmp.size()-1; i >=0; i--){
		if (tmp[i].IsMerged) continue;		

		for (int j = i + 1; j < tmp.size(); j++) {
			cv::Rect andRect_overlap = (tmp[i].textboxForCheck & tmp[j].textboxForCheck);

			if (andRect_overlap.area() > 1) {		// intersected
				cv::Rect checkBox = (tmp[i].textboxForCheck | tmp[j].textboxForCheck);
				cv::Rect mergeBox = (tmp[i].textbox | tmp[j].textbox);

				tmp[i].textbox = mergeBox;
				tmp[i].textboxForCheck = checkBox;
				tmp[j].IsMerged = true;
				IsMerged = true;
			}
		}
	}

	for (int i = 0; i < tmp.size(); i++) {
		// Add merged box //
		int minSize = 4;
		float arw = (float)tmp[i].textbox.width / (float)tmp[i].textbox.height;
		float arh = (float)tmp[i].textbox.height / (float)tmp[i].textbox.width;
		if ((tmp[i].textbox.width < minSize) || (tmp[i].textbox.height < minSize)) {
			if ((arw<0.2f) || (arh<0.2f)) {
				continue;
			}
		}
		if (tmp[i].textbox.area() > 16) {
			if (tmp[i].IsMerged == false) {
				vecBox.push_back(tmp[i]);
			}
		}
	}
//	tmp.clear();
	tmp.swap(std::vector<_extractBox>());

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtBoundaryBox(vecBox, depth);
		TRACE("Recursive: %d\n", depth);
	}
	return true;
}




void CExtractor::ExtractLines(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox, _LANGUAGE_TYPE languageType, _ALIGHN_TYPE align)
{
	InitializeContourVectors();
	/// Find contours
	cv::findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles
	contours_poly.resize(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);		
	}


	// Detect Paragraphs //
	//DetectBoundary(contours_poly, vecBox, xMargin, yMargin, languageType, align);
}

void CExtractor::DetectBoundary(std::vector<std::vector<cv::Point> >& contour, std::vector<_extractBox>& vecBox, int xMargin, int yMargin, _LANGUAGE_TYPE languageType, _ALIGHN_TYPE align)
{
	int minSize = 4;
	for (int i = 0; i < contour.size(); i++) {
		cv::Rect r = cv::boundingRect(cv::Mat(contour[i]));

		_extractBox textBox;
		textBox.init();
		textBox.textbox = r;
		textBox.setExtendBox(xMargin, yMargin);

		if (textBox.textbox.area() < minSize) continue;

		bool inside = false;
		for (int j = 0; j < contour.size(); j++) {
			if (j == i)continue;
			cv::Rect r2 = cv::boundingRect(cv::Mat(contour[j]));

			if (r2.area() < minSize || r2.area() < r.area())continue;
			if (r.x > r2.x&&r.x + r.width<r2.x + r2.width&&
				r.y>r2.y&&r.y + r.height < r2.y + r2.height) {
				inside = true;
			}
		}
		if (inside)continue;

		//if ((textBox.textbox.width < maxWidth*2.0f) && (textBox.textbox.height < maxHeight*2.0f)) {
		//	textBox.textbox.x -= 1;
		//	textBox.textbox.width += 2;
		//	textBox.textbox.y -= 1;
		//	textBox.textbox.height += 2;
			vecBox.push_back(textBox);
		//}
	}
	// Initial extraction //
	int depth = 0;

	float xth, yth;
	switch (align)
	{
	case _UNKNOWN_ALIGN:
		xth = 10.0f;
		yth = 10.0f;
		break;
	case _HORIZON_ALIGN:
		xth = 1000.0f;
		yth = 1.2f;
		yMargin = 0;
		xMargin = 10;
		break;
	case _VERTICAL_ALIGN:
		xth = 1.2f;
		yth = 10.0f;
	//	languageType = _NONALPHABETIC;
	default:
		break;
	}
	//RcvMeargingtBoundaryBox(vecBox, depth, xth, yth, xMargin, yMargin, languageType);		// Test for horizontal case
}

//bool CExtractor::RcvMeargingtBoundaryBox(std::vector<_extractBox>& vecBox, int& depth, float xTh, float yTh, int xMargin, int yMargin, _LANGUAGE_TYPE languageType)
//{
//	int minArea = m_fontSize.width*m_fontSize.width;
//	if (m_fontSize.height < m_fontSize.width)
//		minArea = m_fontSize.height*m_fontSize.height;
//
//	std::vector<_extractBox> tmp = vecBox;
//	vecBox = std::vector<_extractBox>();
//
//	int nWidth = 0, nHeight = 0;
//	bool IsMerged = false;
//	_extractBox resBox;
//	for (int i = 0; i < tmp.size(); i++) {
//		//	for (int i = tmp.size()-1; i >=0; i--){
//		if (tmp[i].IsMerged) continue;
//
//		resBox.init();
//		int sid = FindOptimalBox(tmp, i, xTh, yTh, resBox);
//		if (sid >= 0) {
//			tmp[i].textbox.x = resBox.textbox.x;
//			tmp[i].textbox.y = resBox.textbox.y;
//			tmp[i].textbox.width = resBox.textbox.width;
//			tmp[i].textbox.height = resBox.textbox.height;
//
//			tmp[sid].IsMerged = true;
//			IsMerged = true;
//		}
//	}
//
//
//
//	for (int i = 0; i < tmp.size(); i++) {
//		if (tmp[i].IsMerged == false) {
//			_extractBox tbox;
//			tbox.init();
//			tbox.textbox = tmp[i].textbox;
//			tbox.setExtendBox(xMargin, yMargin);
//			//			tbox.textSphere.setbyRect(tbox.textbox);
//
//			// Filtering line//
//			float arw = (float)tbox.textbox.width / (float)tbox.textbox.height;
//			float arh = (float)tbox.textbox.height / (float)tbox.textbox.width;
//			//if ((tbox.textbox.height > maxheight*2.0f) || (tbox.textbox.width > maxwidth*2.0f))
//			//{
//
//				if ((arw<0.1f) || (arh<0.1f)) {
//					continue;
//				}
//			//}
//
//			// Adjust Size=====================================//  In case of Chinese, Korean
//			if ((languageType == _NONALPHABETIC)) {		// character detection
//				if (arw > 3) {  // " --- "
//					if (yMargin< 4) {
//						int delta = 2;
//						tbox.textboxForCheck.y -= delta;
//						tbox.textboxForCheck.height += delta * 2;
//					}
//				}
//				if (arh > 3) { // " | " 
//					if (xMargin < 4) {
//						int delta = 2;
//						tbox.textboxForCheck.x -= delta;
//						tbox.textboxForCheck.width += delta * 2;
//					}
//				}
//
//				if (tbox.textbox.area() < minArea*0.5f) {
//					if (yMargin < 4) {
//						int delta = 1;
//						tbox.textboxForCheck.y -= delta;
//						tbox.textboxForCheck.height += delta * 2;
//					}
//					if (xMargin < 4) {
//						int delta = 1;
//						tbox.textboxForCheck.x -= delta;
//						tbox.textboxForCheck.width += delta * 2;
//					}
//				}
//			}
//			//=====================================================================//
//			vecBox.push_back(tbox);
//		}
//	}
//	tmp.clear();
//
//	//if (addcnt > 0) {
//	//	float aWidth = (float)m_averTextSize.width / (float)addcnt;
//	//	float aHeight = (float)m_averTextSize.height / (float)addcnt;
//	//	m_averTextSize.set(0, aWidth, 0, aHeight);
//	//}
//
//	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
//		depth++;
//
//		RcvMeargingtBoundaryBox(vecBox, depth, xTh, yTh, xMargin, yMargin,languageType);
//		TRACE("Recursive: %d\n", depth);
//	}
//
//	return true;
//}


int CExtractor::FindOptimalBox(std::vector<_extractBox>& tmp, int i, float xTh, float yTh, _extractBox& resBox)
{
	int x, y, x2, y2;
	float width, height;
	//	float areaRatio = 10000.0f;
	//	float mergeArea = 1000000.0f;
	float maxOveralp = 0;
	int selectdId = -1;
	for (int j = 0; j < tmp.size(); j++) {
		//	for (int j = tmp.size()-1; j >=0; j--){
		if (j != i) {
			if (tmp[j].IsMerged) continue;
			//		if (tmp[i].textbox.y > tmp[j].textbox.y)	continue;
			//		int overlap = IsBoxToBoxIntersect(tmp[i].textboxForCheck, tmp[j].textboxForCheck);
			if (IsBoxToBoxIntersect(tmp[i].textboxForCheck, tmp[j].textboxForCheck) != 0) {
				// Merge Two box //				

				if (tmp[i].textbox.x < tmp[j].textbox.x)					x = tmp[i].textbox.x;
				else														x = tmp[j].textbox.x;

				if (tmp[i].textbox.x + tmp[i].textbox.width > tmp[j].textbox.x + tmp[j].textbox.width)					x2 = tmp[i].textbox.x + tmp[i].textbox.width;
				else																									x2 = tmp[j].textbox.x + tmp[j].textbox.width;

				if (tmp[i].textbox.y < tmp[j].textbox.y)					y = tmp[i].textbox.y;
				else														y = tmp[j].textbox.y;

				if (tmp[i].textbox.y + tmp[i].textbox.height > tmp[j].textbox.y + tmp[j].textbox.height)				y2 = tmp[i].textbox.y + tmp[i].textbox.height;
				else																									y2 = tmp[j].textbox.y + tmp[j].textbox.height;

				width = static_cast<float>(x2 - x);
				height = static_cast<float>(y2 - y);

				//if((height < m_fontSize.height * yTh) && (width < m_fontSize.width * xTh)){
				//	if ((width < (maxwidth*1.25f)) && (height < (maxheight*1.25f))){
				//if ((width < (maxwidth*1.05f)) && (height < (maxheight*1.05f))) {
					// case of V_ORDER //
					//	if (maxOveralp <  overlap){
					selectdId = j;
					//		maxOveralp = height;

					resBox.textbox.x = x;
					resBox.textbox.y = y;
					resBox.textbox.width = static_cast<int>(width);
					resBox.textbox.height = static_cast<int>(height);
					break;
					//	}
				//}
				//else {
				//	int a = 0;
				//}
			}
		}
	}
	return selectdId;
}

int CExtractor::IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2)
{
	cv::Rect a1, a2;
	a1 = b1;
	a2 = b2;

	if (b2.area() < b1.area()) {
		a1 = b2;
		a2 = b1;
	}

	if (((a1.x + a1.width) <= a2.x) || (a1.x >= (a2.x + a2.width)) || ((a1.y + a1.height) <= a2.y) || (a1.y >= (a2.y + a2.height))) {
		return 0;
	}

	return 1;
}

void CExtractor::verifyImgSize(cv::Rect& rect, int imgwidth, int imgheight)
{
	if (rect.x < 0)
		rect.x = 0;
	if (rect.y < 0)
		rect.y = 0;

	if (rect.x + rect.width >= imgwidth) {
		rect.width = imgwidth - rect.x;
	}
	if (rect.y + rect.height >= imgheight) {
		rect.height = imgheight - rect.y;
	}

}

cv::Mat CExtractor::GetLinesbyHistogram(cv::Mat& img, std::vector<_extractBox>& vecline, int t)
{
	//col or row histogram?
	int sz = (t) ? img.rows : img.cols;
	cv::Mat mhist = cv::Mat::zeros(1, sz, CV_8U);

	//count nonzero value and check max V
	int max = -100;
	for (int j = 0; j < sz; ++j)
	{
		cv::Mat data = (t) ? img.row(j) : img.col(j);
		int v = cv::countNonZero(data);
		mhist.at< unsigned char >(j) = v;
		if (v > max)
			max = v;
	}

	cv::Mat histo;
	int width, height;
	if (t)
	{
		width = max;
		height = sz;
		histo = cv::Mat::zeros(cv::Size(width, height), CV_8U);

		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < mhist.at< unsigned char >(i); ++j)
				histo.at< unsigned char >(i, j) = 255;
		}

	}
	else {
		width = sz;
		height = max;
		histo = cv::Mat::zeros(cv::Size(width, height), CV_8U);

		for (int i = 0; i< width; ++i)
		{
			for (int j = 0; j< mhist.at< unsigned char >(i); ++j)
				histo.at< unsigned char >(max - j - 1, i) = 255;
		}
	}

	return histo;

}

void CExtractor::SortBoundaryBox(std::vector<_extractBox>& vecBox)
{
	// TEST  : Sort by Size min to max ============/
	_extractBox tmp;
	for (int i = 0; i < vecBox.size(); i++){
		for (int j = 0; j < vecBox.size(); j++){
			if (i != j){
				if (vecBox[j].textbox.y < vecBox[i].textbox.y){
					tmp = vecBox[i];
					vecBox[i] = vecBox[j];
					vecBox[j] = tmp;
				}
			}
		}
	}
	//==================================//
}

void CExtractor::ExtractionText(cv::Mat& binaryImg, int xMargin, int yMargin, std::vector<_extractBox>& vecBox, bool IsVerti)
{
	InitializeContourVectors();
	/// Find contours
	cv::findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles

	contours_poly.resize(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		_extractBox textBox;
		textBox.init();
		textBox.textbox = cv::boundingRect(cv::Mat(contours_poly[i]));
		//textBox.setExtendBox(xMargin, yMargin);
		//// check aspect ratio //
		//float arw = (float)textBox.textbox.width / (float)textBox.textbox.height;
		//float arh = (float)textBox.textbox.height / (float)textBox.textbox.width;
		//if ((arw < 0.2f) || (arh < 0.2f)) {
		//	continue;
		//}		
		vecBox.push_back(textBox);
	}

	int depth = 0;
	if (IsVerti) {
		MeargingtBoundaryBoxText((int)binaryImg.cols, -1, (int)binaryImg.cols, vecBox, depth);		// Merge horizontal
	//	MeargingtBoundaryBoxText(-1, -1, (int)binaryImg.cols, vecBox, depth);		// Merge horizontal
		depth = 0;
		//	SortBoundaryBox(vecBox);
		for (int i = 0; i < 5; i++) {
			MeargingtBoundaryBoxTextVerti(i, i, (int)binaryImg.cols, vecBox, depth);		// Merge Vertical
		}
	}
	else {
		MeargingtBoundaryBoxText(4, -1, (int)binaryImg.rows, vecBox, depth);
	}
}


int CExtractor::MinDist(cv::Rect r1, cv::Rect r2)
{
	int dist = 10000;
	int d[4];

	d[0] = r1.y - r2.y;
	if (d[0] < 0) d[0] *= -1;

	d[1] = (r1.y+r1.height) - (r2.y+r2.height);
	if (d[1] < 0) d[1] *= -1;

	d[2] = (r1.y + r1.height) - (r2.y);
	if (d[2] < 0) d[2] *= -1;

	d[3] = (r1.y) - (r2.y+r2.height);
	if (d[3] < 0) d[3] *= -1;

	for (int i = 0; i < 4; i++) {
		if (d[i] < dist) {
			dist = d[i]; 
		}
	}
	return dist;
}

bool CExtractor::MeargingtBoundaryBoxTextVerti(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth)
{
	std::vector<_extractBox> tmp = vecBox;
	vecBox.swap(std::vector<_extractBox>());

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_extractBox resBox;
	for (int i = 0; i < tmp.size(); i++) {
		//	for (int i = tmp.size()-1; i >=0; i--){
		tmp[i].setExtendBox(xMargin, yMargin);
		if (tmp[i].IsMerged) continue;

		int boxDist1 = 0, boxDist2=0;
		for (int j = i + 1; j < tmp.size(); j++) {
			tmp[j].setExtendBox(xMargin, yMargin);
			if (tmp[j].IsMerged) continue;

			cv::Rect andRect_overlap = (tmp[i].textboxForCheck & tmp[j].textboxForCheck);

			if (andRect_overlap.area() > 1) {		// intersected

				//if (j < tmp.size()-1) {
				//	boxDist1 = MinDist(tmp[i].textbox, tmp[j].textbox);
				//	boxDist2 = MinDist(tmp[j].textbox, tmp[j+1].textbox);

				//	if (boxDist1 < boxDist2) {
				//		// Add //
				//		cv::Rect checkBox = (tmp[i].textboxForCheck | tmp[j].textboxForCheck);
				//		cv::Rect mergeBox = (tmp[i].textbox | tmp[j].textbox);
				//		if (mergeBox.height < _maxLength*1.1f) {
				//			tmp[i].textbox = mergeBox;
				//			tmp[i].textboxForCheck = checkBox;
				//			tmp[j].IsMerged = true;
				//			IsMerged = true;
				//		}
				//		break;
				//	}
				//}
				//else {
					cv::Rect checkBox = (tmp[i].textboxForCheck | tmp[j].textboxForCheck);
					cv::Rect mergeBox = (tmp[i].textbox | tmp[j].textbox);
					if (mergeBox.height < _maxLength*1.2f) {
						tmp[i].textbox = mergeBox;
						tmp[i].textboxForCheck = checkBox;
						tmp[j].IsMerged = true;
						IsMerged = true;
					}

					break;
				//}
			}
		}
	}

//==================================================================//		
//	int maxLength = 0;
	for (int i = 0; i < tmp.size(); i++) {
		// Add merged box //
		if (tmp[i].IsMerged == false) {
			vecBox.push_back(tmp[i]);
		//	maxLength += tmp[i].textbox.width;
		}
		//}			
	}

//	maxLength /= (int)tmp.size();
//	tmp.clear();
	tmp.swap(std::vector<_extractBox>());

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtBoundaryBoxTextVerti(xMargin, yMargin, _maxLength, vecBox, depth);
		TRACE("Recursive: %d\n", depth);
	}
	return true;
}

/*
bool CExtractor::MeargingtBoundaryBoxTextVerti(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth)
{
	std::vector<_extractBox> tmp = vecBox;
	vecBox = std::vector<_extractBox>();

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_extractBox resBox;
	for (int i = 0; i < tmp.size(); i++) {
		//	for (int i = tmp.size()-1; i >=0; i--){
		tmp[i].setExtendBox(xMargin, yMargin);
		if (tmp[i].IsMerged) continue;

		for (int j = i + 1; j < tmp.size(); j++) {
			tmp[j].setExtendBox(xMargin, yMargin);
			if (tmp[j].IsMerged) continue;

			cv::Rect andRect_overlap = (tmp[i].textboxForCheck & tmp[j].textboxForCheck);

			if (andRect_overlap.area() > 1) {		// intersected
				cv::Rect checkBox = (tmp[i].textboxForCheck | tmp[j].textboxForCheck);
				cv::Rect mergeBox = (tmp[i].textbox | tmp[j].textbox);

				//int maxLength = tmp[i].textbox.width > tmp[j].textbox.width ? tmp[i].textbox.width : tmp[j].textbox.width;
				if (mergeBox.height < _maxLength*1.1f) {
					tmp[i].textbox = mergeBox;
					tmp[i].textboxForCheck = checkBox;

					tmp[j].IsMerged = true;
			//		tmp[j].textboxForCheck = cv::Rect(0, 0, 0, 0);
					IsMerged = true;
			//		break;
				}
			}
		}
	}


	int maxLength = 0;
	for (int i = 0; i < tmp.size(); i++) {
		// Add merged box //
		//int minSize = 4;
		//float arw = (float)tmp[i].textbox.width / (float)tmp[i].textbox.height;
		//float arh = (float)tmp[i].textbox.height / (float)tmp[i].textbox.width;
		//if ((tmp[i].textbox.width < minSize) || (tmp[i].textbox.height < minSize)) {
		//	if ((arw<0.2f) || (arh<0.2f)) {
		//		continue;
		//	}
		//}
		//if (tmp[i].textbox.area() > 16) {
			if (tmp[i].IsMerged == false) {
				vecBox.push_back(tmp[i]);
				maxLength += tmp[i].textbox.width;
			}
		//}			
	}

	maxLength /= (int)tmp.size();
	tmp.clear();

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtBoundaryBoxTextVerti(xMargin, yMargin, maxLength, vecBox, depth);
		TRACE("Recursive: %d\n", depth);
	}
	return true;
}
*/
bool CExtractor::MeargingtBoundaryBoxText(int xMargin, int yMargin, int _maxLength, std::vector<_extractBox>& vecBox, int& depth)
{
	std::vector<_extractBox> tmp = vecBox;
	vecBox.swap(std::vector<_extractBox>());

	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_extractBox resBox;
	for (int i = 0; i < tmp.size(); i++) {
		//	for (int i = tmp.size()-1; i >=0; i--){
		tmp[i].setExtendBox(xMargin, yMargin);
		if (tmp[i].IsMerged) continue;

		for (int j = i + 1; j < tmp.size(); j++) {
			tmp[j].setExtendBox(xMargin, yMargin);
			if (tmp[j].IsMerged) continue;

			cv::Rect andRect_overlap = (tmp[i].textboxForCheck & tmp[j].textboxForCheck);

			if (andRect_overlap.area() > 1) {		// intersected
				cv::Rect checkBox = (tmp[i].textboxForCheck | tmp[j].textboxForCheck);
				cv::Rect mergeBox = (tmp[i].textbox | tmp[j].textbox);

				//int maxLength = tmp[i].textbox.width > tmp[j].textbox.width ? tmp[i].textbox.width : tmp[j].textbox.width;
				if (mergeBox.height < _maxLength*1.1f) {
					tmp[i].textbox = mergeBox;
					tmp[i].textboxForCheck = checkBox;
					tmp[j].IsMerged = true;
					IsMerged = true;
					//break;
				}
			}
		}
	}


	int maxLength = 0;
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i].IsMerged == false) {
			vecBox.push_back(tmp[i]);
			maxLength += tmp[i].textbox.width;
		}
		//}			
	}

	maxLength /= (int)tmp.size();
//	tmp.clear();
	tmp.swap(std::vector<_extractBox>());

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;
		MeargingtBoundaryBoxText(xMargin, yMargin, _maxLength, vecBox, depth);
		TRACE("Recursive: %d\n", depth);
	}
	return true;
}

