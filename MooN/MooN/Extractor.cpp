#include "stdafx.h"
#include "Extractor.h"
#include "math_tool.h"


CExtractor::CExtractor()
{
}


CExtractor::~CExtractor()
{
	total_lines.clear();
	contours_poly.clear();
}

void CExtractor::TestFunc()
{
//	cv::Mat input = cv::imread("D:/Untitled.bmp");
	cv::Mat input = cv::imread("D:/Capture.JPG");
//	DeSkewImg(input);
	AllHoriVertLines(input);
	input.release();
}

float CExtractor::DeSkewImg(cv::Mat& img)
{
	//cv::Mat tmp;
	//cvtColor(img, tmp, CV_BGR2GRAY);
	//cv::threshold(tmp, tmp, 125, 255, cv::THRESH_OTSU);
	//cv::bitwise_not(tmp, tmp);

	points.clear();
	findNonZero(img, points);
	cv::RotatedRect box = cv::minAreaRect(points);

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

_ALIGHN_TYPE CExtractor::AllHoriVertLines(cv::Mat& binaryImg)
{
//	cv::medianBlur(tmp, tmp, 5);
//	int erosion_size = 2;
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
//		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
//		cv::Point(erosion_size, erosion_size));
//	cv::erode(tmp, tmp, element);
////	cv::dilate(tmp, tmp, element);

	cv::Mat resizeImg;
	float fscale = 256.0f / (float)binaryImg.rows;
	cv::resize(binaryImg, resizeImg, cv::Size(256,256));

	total_lines.clear();

//	cv::imshow("img", resizeImg);

	cv::Size size = resizeImg.size();
	cv::HoughLinesP(resizeImg, total_lines, 1, CV_PI / 180, 100, size.width / 2.f, 20);

//	cv::Mat disp_lines(size, CV_8UC3, cv::Scalar(0, 0, 0));

	POINT3D vecAlign, vecBase;
	mtSetPoint3D(&vecAlign, 0, 0, 0);
	mtSetPoint3D(&vecBase, 1, 0, 0);
	 for (unsigned i = 0; i < total_lines.size(); ++i)
	 {
		 POINT3D v;
		 v.x = total_lines[i][2] - total_lines[i][0];
		 v.y = total_lines[i][3] - total_lines[i][1];
		 v.z = 0;

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

void CExtractor::ExtractParagraph(cv::Mat& binaryImg, int mergeWidth, int mergeHeight, std::vector<_extractBox>& vecBox, _LANGUAGE_TYPE languageType)
{
	contours_poly.clear();
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	cv::findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles
	
	contours_poly.resize(contours.size());
	for (int i = 0; i < contours.size(); i++){
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}
	// Detect Paragraphs //
	DetectBoundary(contours_poly, vecBox, binaryImg.cols, binaryImg.rows, 16,16, languageType);


	// Deskew page //
	

}

void CExtractor::DetectBoundary(std::vector<std::vector<cv::Point> >& contour, std::vector<_extractBox>& vecBox, int maxWidth, int maxHeight, int extX, int extY, _LANGUAGE_TYPE languageType)
{
	int minSize = 10;
	for (int i = 0; i < contour.size(); i++) {
		cv::Rect r = cv::boundingRect(cv::Mat(contour[i]));

		_extractBox textBox;
		textBox.init();
		textBox.textbox = r;
		textBox.setExtendBox(extX, extY);

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

		if ((textBox.textbox.width < maxWidth*2.0f) && (textBox.textbox.height < maxHeight*2.0f)) {
			textBox.textbox.x -= 1;
			textBox.textbox.width += 2;
			textBox.textbox.y -= 1;
			textBox.textbox.height += 2;
			vecBox.push_back(textBox);
		}
	}
	// Initial extraction //
	int depth = 0;
	RcvMeargingtBoundaryBox(maxWidth, maxHeight, vecBox, depth, extX, extY, languageType);
}

bool CExtractor::RcvMeargingtBoundaryBox(int maxwidth, int maxheight, std::vector<_extractBox>& vecBox, int& depth, int extX, int extY, _LANGUAGE_TYPE languageType)
{
	int minArea = maxwidth*maxwidth;
	if (maxheight < maxwidth)
		minArea = maxheight*maxheight;


	std::vector<_extractBox> tmp = vecBox;
	vecBox = std::vector<_extractBox>();

	//m_averTextSize.width = 0;
	//m_averTextSize.height = 0;
	int addcnt = 0;


	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_extractBox resBox;
	for (int i = 0; i < tmp.size(); i++) {
		//	for (int i = tmp.size()-1; i >=0; i--){
		if (tmp[i].IsMerged) continue;

		resBox.init();
		int sid = FindOptimalBox(tmp, i, maxwidth, maxheight, resBox);
		if (sid >= 0) {
			tmp[i].textbox.x = resBox.textbox.x;
			tmp[i].textbox.y = resBox.textbox.y;
			tmp[i].textbox.width = resBox.textbox.width;
			tmp[i].textbox.height = resBox.textbox.height;

			tmp[sid].IsMerged = true;
			IsMerged = true;
		}
	}



	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i].IsMerged == false) {
			_extractBox tbox;
			tbox.init();
			tbox.textbox = tmp[i].textbox;
			tbox.setExtendBox(extX, extY);
			//			tbox.textSphere.setbyRect(tbox.textbox);

			// Filtering line//
			float arw = (float)tbox.textbox.width / (float)tbox.textbox.height;
			float arh = (float)tbox.textbox.height / (float)tbox.textbox.width;
			if ((tbox.textbox.height > maxheight*2.0f) || (tbox.textbox.width > maxwidth*2.0f))
			{

				if ((arw<0.1f) || (arh<0.1f)) {
					continue;
				}
			}

			// Adjust Size=====================================//  In case of Chinese, Korean
			if ((languageType == _NONALPHABETIC)) {		// character detection
				if (arw > 3) {  // " --- "
					if (extY < 4) {
						int delta = 2;
						tbox.textboxForCheck.y -= delta;
						tbox.textboxForCheck.height += delta * 2;
					}
				}
				if (arh > 3) { // " | " 
					if (extX < 4) {
						int delta = 2;
						tbox.textboxForCheck.x -= delta;
						tbox.textboxForCheck.width += delta * 2;
					}
				}

				if (tbox.textbox.area() < minArea*0.5f) {
					if (extY < 4) {
						int delta = 1;
						tbox.textboxForCheck.y -= delta;
						tbox.textboxForCheck.height += delta * 2;
					}
					if (extX < 4) {
						int delta = 1;
						tbox.textboxForCheck.x -= delta;
						tbox.textboxForCheck.width += delta * 2;
					}
				}
			}
			//=====================================================================//
			vecBox.push_back(tbox);

			//m_averTextSize.width += tmp[i].textbox.width;
			//m_averTextSize.height += tmp[i].textbox.height;
			addcnt++;
		}
	}
	tmp.clear();

	//if (addcnt > 0) {
	//	float aWidth = (float)m_averTextSize.width / (float)addcnt;
	//	float aHeight = (float)m_averTextSize.height / (float)addcnt;
	//	m_averTextSize.set(0, aWidth, 0, aHeight);
	//}

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)) {
		depth++;

		RcvMeargingtBoundaryBox(maxwidth, maxheight, vecBox, depth, extX, extY, languageType);
		TRACE("Recursive: %d\n", depth);
	}

	return true;
}


int CExtractor::FindOptimalBox(std::vector<_extractBox>& tmp, int i, int maxwidth, int maxheight, _extractBox& resBox)
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

				width = x2 - x;
				height = y2 - y;

				//	if ((width < (maxwidth*1.25f)) && (height < (maxheight*1.25f))){
				if ((width < (maxwidth*1.05f)) && (height < (maxheight*1.05f))) {
					// case of V_ORDER //
					//	if (maxOveralp <  overlap){
					selectdId = j;
					//		maxOveralp = height;

					resBox.textbox.x = x;
					resBox.textbox.y = y;
					resBox.textbox.width = width;
					resBox.textbox.height = height;
					break;
					//	}
				}
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