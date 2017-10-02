#include "stdafx.h"
#include "ZMatching.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CZMatching::CZMatching()
{
	m_pCut = NULL;
	m_IsReadyToSearch = false;

	m_Threshold = 0.75f;
	m_colorAccScale = 1.0f;
}




CZMatching::~CZMatching()
{
	if (m_pCut != NULL){
		cvReleaseImage(&m_pCut);
	}
}

void CZMatching::SetThreshold(float _th)
{
	if (_th == 1.0)
		_th = 0.9999f;

	m_Threshold = _th;
	m_colorAccScale = 1.0f / (m_Threshold);
}

void CZMatching::NomalizeCutImage(IplImage* pSrc, IplImage* pCut, RECT2D cutRect, unsigned short norSize)
{
	IplImage* tmpCut = cvCreateImage(cvSize(cutRect.width, cutRect.height), pSrc->depth, pSrc->nChannels);
	cvSetImageROI(pSrc, cvRect(cutRect.x1, cutRect.y1, cutRect.width, cutRect.height));		// posx, posy = left - top
	cvCopy(pSrc, tmpCut);

	pCut = cvCreateImage(cvSize(norSize, norSize), pSrc->depth, pSrc->nChannels);
	cvResize(tmpCut, pCut);

	cvReleaseImage(&tmpCut);

}

void CZMatching::FitCutImageRect(IplImage* pSrc, RECT2D& cutRect)
{

	RECT2D oriCut = cutRect;

	cutRect.x1 += 2;
	cutRect.x2 -= 2;
	cutRect.y1 += 2;
	cutRect.y2 -= 2;
	cutRect.width -= 4;
	cutRect.height -= 4;

	// find left edge //
	IplImage* pBinImg = cvCreateImage(cvSize(pSrc->width, pSrc->height), IPL_DEPTH_8U, 1);
	cvThreshold(pSrc, pBinImg, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);


	//	cvShowImage("Before Bin Image", pBinImg);
	//	LetterThining(pBinImg);
	//	cvShowImage("Afger-Bin Image", pBinImg);

	FindHorizonEage(pBinImg, cutRect, 0, 0, oriCut);		// direction 0: left, 1: right
	FindHorizonEage(pBinImg, cutRect, 0, 1, oriCut);		// direction 0: left, 1: right
	FindVerticalEage(pBinImg, cutRect, 0, 0, oriCut);		// direction 0: left, 1: right
	FindVerticalEage(pBinImg, cutRect, 0, 1, oriCut);		// direction 0: left, 1: right

	cutRect.width = cutRect.x2 - cutRect.x1;
	cutRect.height = cutRect.y2 - cutRect.y1;

	//	cvShowImage("Bin Image", pBinImg);

	cvReleaseImage(&pBinImg);
}

bool CZMatching::FindVerticalEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction, RECT2D& oriRect)
{
	int sumPixel = 0;
	for (int x = cutRect.x1; x < cutRect.x2; x++){
		int id;
		if (direction == 0)		id = ((int)cutRect.y1)*(pSrc->widthStep) + x;		// case of top edge
		else					id = ((int)cutRect.y2)*(pSrc->widthStep) + x;	    // case of bot edge

		if ((unsigned char)pSrc->imageData[id] < 1){
			sumPixel++;
		}
	}

	float fMaxExtend = 1.05f;
	float fMinExtend = 0.8f;


	int pixelth = 0;

	switch (type){
	case 0:		// first
		if (sumPixel > pixelth){
			if (direction == 0)				cutRect.y1 -= 1;		// case of top edge
			else							cutRect.y2 += 1;		// case of right edge

			if ((cutRect.y2 - cutRect.y1) > oriRect.height*fMaxExtend){
				return true;
			}
			FindVerticalEage(pSrc, cutRect, 1, direction, oriRect);		// go to the left
		}

		else{
			if (direction == 0)				cutRect.y1 += 1;		// case of left edge
			else							cutRect.y2 -= 1;		// case of right edge

			if ((cutRect.y2 - cutRect.y1) < oriRect.height*fMinExtend){
				return true;
			}
			FindVerticalEage(pSrc, cutRect, 2, direction, oriRect);		// go to the left
		}
		break;
	case 1:  // Go to the negative way
		if (sumPixel > pixelth){
			if (direction == 0)		cutRect.y1 -= 1;		// case of left edge
			else					cutRect.y2 += 1;		// case of right edge

			if ((cutRect.y2 - cutRect.y1) > oriRect.height*fMaxExtend){
				return true;
			}
			FindVerticalEage(pSrc, cutRect, 1, direction, oriRect);		// go to the left
		}
		else{
			return true;
		}
		break;
	case 2:	// Go to the positive way
		if (sumPixel < pixelth){
			if (direction == 0)		cutRect.y1 += 1;		// case of left edge
			else					cutRect.y2 -= 1;		// case of right edge

			if ((cutRect.y2 - cutRect.y1) < oriRect.height*fMinExtend){
				return true;
			}
			FindVerticalEage(pSrc, cutRect, 2, direction, oriRect);		// go to the left
		}
		else{
			return true;
		}
		break;
	}

}

bool CZMatching::FindHorizonEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction, RECT2D& oriRect)
{
	int sumPixel = 0;
	for (int y = cutRect.y1; y < cutRect.y2; y++){
		int id;
		if (direction == 0)		id = (y)*(pSrc->widthStep) + cutRect.x1;		// case of left edge
		else					id = (y)*(pSrc->widthStep) + cutRect.x2;	// case of right edge
		
		if ((unsigned char)pSrc->imageData[id] < 1){
			sumPixel++;
		}
	}


	float fMaxExtend = 1.05f;
	float fMinExtend = 0.8f;


	int pixelth  = 1;

	switch (type){
	case 0:		// first
		if (sumPixel > pixelth){
			if (direction == 0)			cutRect.x1 -= 1;		// case of left edge				
			else						cutRect.x2 += 1;		// case of right edge				

			if ((cutRect.x2 - cutRect.x1) > oriRect.width*fMaxExtend){
				return true;
			}

			FindHorizonEage(pSrc, cutRect, 1, direction, oriRect);		// go to the left
		}		

		else{
			if (direction == 0)		cutRect.x1 += 1;		// case of left edge				
			else					cutRect.x2 -= 1;		// case of right edge				

			if ((cutRect.x2 - cutRect.x1) < oriRect.width*fMinExtend){
				return true;
			}

			FindHorizonEage(pSrc, cutRect, 2, direction, oriRect);		// go to the left
		}
		break;
	case 1:  // Go to the negative way
		if (sumPixel > pixelth){
			if (direction == 0)		cutRect.x1 -= 1;		// case of left edge
			else					cutRect.x2 += 1;		// case of right edge

			if ((cutRect.x2 - cutRect.x1) > oriRect.width*fMaxExtend){
				return true;
			}

			FindHorizonEage(pSrc, cutRect, 1, direction, oriRect);		// go to the left
		}
		else{
			return true;
		}
		break;
	case 2:	// Go to the positive way
		if (sumPixel < pixelth){
			if (direction == 0)		cutRect.x1 += 1;		// case of left edge
			else					cutRect.x2 -= 1;		// case of right edge

			if ((cutRect.x2 - cutRect.x1) < oriRect.width*fMinExtend){
				return true;
			}

			FindHorizonEage(pSrc, cutRect, 2, direction, oriRect);		// go to the left
		}
		else{
			return true;
		}
		break;
	}

}

//unsigned int FindHorizentalEdge(IplImage* pSrc, RECT2D cutRect, short direction)
//{
//	int sumPixel = 0;
//	for (int y = cutRect.y1; y < cutRect.y2; y++){
//		int id = (y - 1)*(pSrc->width-1) + cutRect.x1;
//		sumPixel += (unsigned char)pSrc->imageData[id];
//	}
//}

void CZMatching::PrepareCutNSearch(CZPageObject* pSelPage, RECT2D selRect)
{
	m_cutRect = selRect;
	m_IsReadyToSearch = false;
	if ((pSelPage == NULL) || (selRect.width==0)){
		return;
	}

	if (m_pCut != NULL){
		cvReleaseImage(&m_pCut);
		m_pCut = NULL;
	}

	CString strpath = pSelPage->GetPath();
	USES_CONVERSION;
	char* sz = T2A(strpath);
	IplImage *pSrc = SINGLETON_TMat::GetInstance()->LoadIplImage(strpath, 1);

	if (pSrc != NULL){

//		if (0){  // partial matching

	//	FitCutImageRect(pSrc, selRect);

			m_pCut = cvCreateImage(cvSize(selRect.width, selRect.height), pSrc->depth, pSrc->nChannels);
			cvSetImageROI(pSrc, cvRect(selRect.x1, selRect.y1, selRect.width, selRect.height));		// posx, posy = left - top
			cvCopy(pSrc, m_pCut);
//		}
		//else{
		//	NomalizeCutImage(pSrc, m_pCut, selRect, 64);
		//}
		m_IsReadyToSearch = true;


		m_cutInfo.fileid = getHashCode((CStringA)pSelPage->GetPath());
		m_cutInfo.posid = (int)selRect.x1 * 10000 + (int)selRect.y1;
		

		CString strId;
		strId.Format(L"%u%u", m_cutInfo.fileid, m_cutInfo.posid);
		m_cutInfo.id = getHashCode((CStringA)strId);
		m_cutInfo.th = m_Threshold;


	//	cvShowImage("Cut Image", m_pCut);
						
	}


	cvReleaseImage(&pSrc);

	

}

bool CZMatching::DoSearch(unsigned int& sCnt, unsigned int sId, CUT_INFO info)
{
	if (m_pCut == NULL)
		return false;

		bool IsFinish = true;
		int search_size = m_pCut->width;
		if (search_size > m_pCut->height)		search_size = m_pCut->height;


		_vecPageObj pImgVec = SINGLETON_TMat::GetInstance()->GetImgVec();
		int cnt = 0;
		for (; sCnt < pImgVec.size(); sCnt++){
			if (pImgVec[sCnt]->GetThTex() == 0)
				continue;

			USES_CONVERSION;
			char* sz = T2A(pImgVec[sCnt]->GetPath());
			IplImage *gray = SINGLETON_TMat::GetInstance()->LoadIplImage(pImgVec[sCnt]->GetPath(), 1);
			IplImage *result_img = cvCreateImage(cvSize(gray->width - m_pCut->width + 1, gray->height - m_pCut->height + 1), IPL_DEPTH_32F, 1);
			cvMatchTemplate(gray, m_pCut, result_img, CV_TM_CCOEFF_NORMED);

			float* d = (float*)result_img->imageData;
			for (int y = 0; y < result_img->height; y++){
				for (int x = 0; x < result_img->width; x++){
					float fD = *(d + y*result_img->width + x);
					if (fD > m_Threshold)	{
						//POINT3D left_top;
						_MATCHInfo mInfo;
						mInfo.pos.x = x + m_pCut->width*0.5f;
						mInfo.pos.y = y + m_pCut->height*0.5f;
						mInfo.pos.z = 0;
						mInfo.accuracy = fD*100;
						mInfo.strAccracy.Format(L"%d", (int)(fD));
						mInfo.rect.set(x, x + m_pCut->width, y, y + m_pCut->height);
						mInfo.searchId = sId;
						mInfo.cInfo = info;
						mInfo.strCode = "-";

						// Fit Area to a character size //
						//FitCutImageRect(gray, mInfo.rect);
						//mInfo.pImgCut = cvCreateImage(cvSize(mInfo.rect.width, mInfo.rect.height), gray->depth, gray->nChannels);
						//cvSetImageROI(gray, cvRect(mInfo.rect.x1, mInfo.rect.y1, mInfo.rect.width, mInfo.rect.height));		// posx, posy = left - top
						//cvCopy(gray, mInfo.pImgCut);


					//	FitCutImageRect(gray, mInfo.rect);

						//	mInfo.color = SINGLETON_TMat::GetInstance()->GetColor((fD)*1.1f);
						m_resColor.a = ((fD)*m_colorAccScale)*0.5f;
						mInfo.color = m_resColor;
						mInfo.IsAdded = false;
						pImgVec[sCnt]->AddMatchedPoint(std::move(mInfo), search_size);
					}
				}
			}
			cvReleaseImage(&result_img);
			cvReleaseImage(&gray);

			cnt++;
			if (cnt > 10){
				IsFinish = false;
				break;
			}
		}
		return IsFinish;

}

void CZMatching::SetResColor(POINT3D _color) 
{ 
	m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; 
	SINGLETON_TMat::GetInstance()->SetResColor(m_resColor);
}


void CZMatching::LetterThining(IplImage* matBinary)
{
	for (int y = 1; y < matBinary->height - 1; y++){
		for (int x = 0; x < matBinary->width; x++){
			int pId = (y - 1)*matBinary->widthStep + x;
			int id = y*matBinary->widthStep + x;
			int nId = (y + 1)*matBinary->widthStep + x;

			//if (matBinary->imageData[id] < 100){		// black
			//	if (matBinary->imageData[pId] > 128){
			//		matBinary->imageData[id] = 100;
			//	}
			//}

			if ((unsigned char)matBinary->imageData[id] < 128){		// black
				if ((unsigned char)matBinary->imageData[nId] > 128){
					matBinary->imageData[id] = (char)255;
				}
			}


			//if (matBinary->imageData[id] == 100){
			//	matBinary->imageData[id] = 255;
			//}


		}
	}
}