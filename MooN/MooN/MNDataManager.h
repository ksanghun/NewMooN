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

#define DB_CLASS_NUM 8
#define DB_IMGCHAR_SIZE 32
#define DB_MAXNUM_IMGDB 10

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


struct stDBRefImage {
	cv::Mat img[DB_MAXNUM_IMGDB];
	int nCurrImgId;
	bool needToUpdate;

	unsigned short wNum, hNum, maxCharLen;
	std::vector<wchar_t*> vecStr;

	void init(unsigned short _w, unsigned short _h, unsigned short _len) {
		nCurrImgId = 0;
		wNum = _w;
		hNum = _h;
		maxCharLen = _len;
		vecStr = std::vector<wchar_t*>();
		needToUpdate = false;
	};
};


enum DB_CHK {SDB_ADD=0, SDB_SKIP, SDB_UPDATE};

class CMNDataManager
{
public:
	CMNDataManager();
	~CMNDataManager();

	// TEST //
	void Test();
	POINT3D GetColor(float fvalue);

	void ClearAllImages();
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
	CMNPageObject* PushImageDataSet(CString _strpath, CString _strPName, CString _strName, unsigned long _code, unsigned long _pcode);
	void PopImageDataSet(unsigned long _pcode);
	CBitmap* GetLogCBitmap(cv::Mat& pimg);
	cv::Rect GetNomalizedWordSize(cv::Rect rect);
	// Setter //
	void SetMatchingResults();
	void SortMatchingResults();
	CString base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	void ExportDatabase(CString _strFolder);
	void ExportDatabase();
	CString GetEditFilePath(CString strExtension, CString strOrigin);


	void ResetResult();
	void ResetMatchingResult();
	void ApplyDeskewPage();
	void MultiToUniCode(char* char_str, wchar_t* str_unicode);
//	void DBTraining(CMNPageObject* pPage);
	void DBTrainingForPage(CMNPageObject* pPage);
	void DBTrainingFromCutSearch(cv::Mat& cutimg, CString _strCode);
	void ResizeCutImageByRatio(cv::Mat& dstimg, cv::Mat& cutimg, int norWidth, int norHeight);
	DB_CHK IsNeedToAddDB(cv::Mat& cutimg, wchar_t* strcode, int classid);
	float TemplateMatching(cv::Mat& src, cv::Mat& dst);
	void MatchingFromDB(cv::Mat& cutimg, _stOCRResult& ocrres);

	CMNPageObject* GetPageByID(int pid);

	void SetExtractionSetting(_stExtractionSetting _set) {		m_extractonInfo = _set;	}
	_stExtractionSetting GetExtractionSetting();// { return m_extractonInfo; }

	void SetUserDBFolder(CString str);// { m_strUserDataFolder = str; }
	int GetNomalizedWordSize(cv::Rect inrect, cv::Rect& outRect);

	void AddSDBTable(unsigned int hcode, wchar_t* strCode);
	void UpdateSDBFiles();
	void LoadSDBFiles();
	void DoKeywordSearch(CString strKeyword);

	void InitSDB(CString strPath, CString strName);
private:
	CMNPDFConverter m_pdf;
	int m_maxCutWidth;
	CString m_strUserDataFolder;
	

//	vecPageObj m_vecImageData;
	std::vector<CMNPageObject*> m_vecImgData;
	std::map<unsigned long, CMNPageObject*> m_mapImageData;
	std::map<unsigned long, stPageGroup> m_mapGrupImg;


	// For Search DB ======================================//
	std::map<unsigned int, _stSDBWordTable> m_mapWordTable;
	std::map<unsigned int, CString> m_mapFilePathTable;

	std::map<unsigned int, _stSDB> m_mapGlobalSDB;
	//=====================================================//

	bool m_bIsUpdateTable;	

	bool m_bSlot[MAX_SLOT_SIZE];
	float m_xOffset, m_yOffset;

	float m_fAniAcceration[ANI_FRAME_CNT];
	POINT3D m_AccColor[10];

	// matching results //
	std::map<unsigned long, stVecMatchResult> m_mapMatchResults;
	_stExtractionSetting m_extractonInfo;

	//=====For DataBase==============================//
	stDBRefImage m_refImgClass[DB_CLASS_NUM];
	void InitDataBaseFiles();
	void UpdateImgClassDB();
//	void UpdateImgClassImgCodes();
	void DeSkew(cv::Mat& img);
	//=====================//


	std::vector<cv::Point> points;

};

typedef CMNSingleton<CMNDataManager> SINGLETON_DataMng;