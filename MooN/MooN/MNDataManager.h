#pragma once

#include "MNSingleton.h"
#include "MNPDFConverter.h"
#include "MNPageObject.h"

#define MAX_SLOT_SIZE 1024
#define DEFAULT_X_OFFSET -5800;
#define DEFAULT_Y_OFFSET 2500;
#define MAX_DESP_COLS 25

#define _NORMALIZE_SIZE_H 32
#define _NORMALIZE_SIZE_W 32*5
#define ANI_FRAME_CNT 10

typedef std::vector<CMNPageObject*> stVecPageObj;
struct stPageGroup {
	int nSlot;
	stVecPageObj imgVec;
};


struct stMatchResult
{
	cv::Mat cutImg;
	unsigned short searchId;
	unsigned long cutId;
	unsigned long fileId;
	unsigned long posId;
	unsigned long matchId;
	unsigned long matchFile;
	unsigned long matchPos;

	cv::Rect rect;
	float fTh;
	float accuracy;

	CString strBase64;
	CString strCode;

	// For interaction //
	unsigned int id_page;
	unsigned int id_match;

	bool IsOnList;
	//=================//
};

typedef std::vector<stMatchResult> stVecMatchResult;



class CMNDataManager
{
public:
	CMNDataManager();
	~CMNDataManager();

	// TEST //
	void Test();

	bool LoadImageData(CString strPath, cv::Mat& pimg, bool IsGray);
	short SelectPages(unsigned long cCode);
	// Getter ========================//
	int GetEmptySlot();
	CMNPageObject* GetPageByOrderID(int idx);
	std::vector<CMNPageObject*>& GetVecImgData() { return m_vecImgData; }
	std::map<unsigned long, stVecMatchResult>& GetMatchingResults() { return m_mapMatchResults; }
	int GetMaxCutWidth() { return m_maxCutWidth; }
	//==============================================//
	void ReturnSlot(int idx);
	float GetAniAcceration(int idx);
	void UpdatePageStatus(POINT3D camPos);

	void InitData();
	void PushImageDataSet(CString _strpath, CString _strPName, CString _strName, unsigned long _code, unsigned long _pcode);
	void PopImageDataSet(unsigned long _pcode);
	CBitmap* GetLogCBitmap(cv::Mat& pimg);
	cv::Rect GetNomalizedWordSize(cv::Rect rect);
	// Setter //
	void SetMatchingResults();
	void SortMatchingResults();
	CString base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

	void ResetResult();
	void ResetMatchingResult();
	void ApplyDeskewPage();


	CMNPageObject* GetPageByID(int pid);
private:
	CMNPDFConverter m_pdf;
	int m_maxCutWidth;
	

//	vecPageObj m_vecImageData;
	std::vector<CMNPageObject*> m_vecImgData;
	std::map<unsigned long, CMNPageObject*> m_mapImageData;
	std::map<unsigned long, stPageGroup> m_mapGrupImg;
	bool m_bSlot[MAX_SLOT_SIZE];
	float m_xOffset, m_yOffset;

	float m_fAniAcceration[ANI_FRAME_CNT];
	POINT3D m_AccColor[10];

	// matching results //
	std::map<unsigned long, stVecMatchResult> m_mapMatchResults;
};

typedef CMNSingleton<CMNDataManager> SINGLETON_DataMng;