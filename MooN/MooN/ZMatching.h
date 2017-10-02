#pragma once

#include "ZDataManager.h"

class CZMatching
{
public:
	CZMatching();
	~CZMatching();

	void PrepareCutNSearch(CZPageObject* pSelPage, RECT2D selRect);
	void SetCutImage(IplImage* pCut);
	bool DoSearch(unsigned int& sCnt, unsigned int sId, CUT_INFO cInfo);
	void SetThreshold(float _th);
	void SetResColor(POINT3D _color);// { m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; }

	void LetterThining(IplImage* matBinary);

	IplImage* GetCutImg() { return m_pCut; }

	//unsigned int GetCutId(){ return m_iCutId; }
	//unsigned int GetFileId() { return m_iFileId; }
	//unsigned int GetPosId() { return m_iPosId; }
	//float GetThreshold(){ return m_Threshold; }

	CUT_INFO GetCutInfo(){ return m_cutInfo; }

private:
	IplImage *m_pCut;
	bool m_IsReadyToSearch;
	RECT2D m_cutRect;

	float m_Threshold;
	float m_colorAccScale;
	COLORf m_resColor;


//	unsigned int m_iCutId, m_iFileId, m_iPosId;
	CUT_INFO m_cutInfo;

	void NomalizeCutImage(IplImage* pSrc, IplImage* pCut, RECT2D cutRect, unsigned short norSize);
	void FitCutImageRect(IplImage* pSrc,  RECT2D& cutRect);
	bool FindHorizonEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction, RECT2D& oriRect);
	bool FindVerticalEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction, RECT2D& oriRect);

};

