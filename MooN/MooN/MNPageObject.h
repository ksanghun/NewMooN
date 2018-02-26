#pragma once
#include "math_tool.h"
#include "MNCVMng.h"

#define DEFAULT_PAGE_SIZE 500
#define MAX_CAM_HIGHTLEVEL 5000
#define Z_TRANS 70000

enum PICKINGID { _PICK_PARA = 10000, _PICK_WORD = 10000000, _PICK_MATCH = 100000000 };

struct stMatchInfo
{
	POINT3D pos;
	COLORf color;
	cv::Rect rect;
	CString strAccracy;
	CString strCode;

	unsigned int searchId;
	float accuracy;
	bool IsAdded;

	_CUTINFO cInfo;

	// if exist //
	//int lineid;
	//int objid;
	int uuid;
//	cv::Mat cutImg;
};

struct stParapgraphInfo
{
	cv::Rect rect;
	float deSkewAngle;
	bool IsDeskewed;
	bool IsVerti;
	bool IsCNSed;

	std::vector<_stOCRResult> vecTextBox;

	void init() {
		rect = cv::Rect(0, 0, 0, 0);
		deSkewAngle = 0.0f;
		IsDeskewed = false;
		IsVerti = false;
		IsCNSed = false;
	};	
};

struct stDBSearchRes
{
	cv::Rect rect;
};

//struct stParapgraphInfo
//{
//	cv::Rect rect;
//	cv::Rect deskewRect;
//	float deSkewAngle;
//	_ALIGHN_TYPE alignType;
//	POINT3D color;
//};



class CExtractor;
class CMNPageObject
{
public:
	CMNPageObject();
	~CMNPageObject();

	// Setter //
	void SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _code, unsigned long _pcode);
	void SetSize(unsigned short _w, unsigned short _h, float _size);
	void SetRendomPos();
	void SetCandidate(bool isCan) { m_bCandidate = isCan; m_pos.z = 0.0f; };
	void SetIsNear(bool isnear) { m_bIsNear = isnear; }
	void SetIsSearched(bool _IsSearch) { m_bIsSearching = _IsSearch; }
	void SetFitCurArea();

	void SetSelMatchItem(int _selid);// { m_selMatchItemId = _selid; }
	void DeSkewImg(int pid, float fAngle);
	void RemoveNoise(cv::Rect rect);
	void UnDoDeSkewImg(int pid);
	void UpdateTexture(cv::Mat& texImg);
	void GetIDbyUUID(unsigned int uuid, int&lineid, int&objid);
	unsigned int GetUUIDbyLineObjId(int lineid, int objid);

	bool IsNear() { return m_bIsNear; }
	bool IsSelected() { return m_bIsSelected;  }

	// Getter //
	GLuint GetTexId() { return m_texId; };
//	GLuint GetThumbnailTex() { return m_thumbnailTexId; };
	POINT3D GetPos() { return m_pos; };
	unsigned short GetImgWidth() { return m_nImgWidth; };
	unsigned short GetImgHeight() { return m_nImgHeight; };
	unsigned long GetCode() { return m_nCode; }
	unsigned long GetPCode() { return parentCode; }

	float GetfXScale() { return m_fXScale; }
	float GetfYScale() { return m_fYScale; }
	CString GetPath() { return m_strPath; };
	CString GetName() { return m_strName; };
	std::vector<stMatchInfo>& GetMatchResult() { return m_matched_pos; };
	bool GetPosByMatchID(int mid, POINT3D& pos);
	bool GetRectByMatchID(int mid, cv::Rect& rect, float& fConf);
	void ClearMatchResult();
//	cv::Mat& GetSrcPageImg() { return m_thumbImg; }
	cv::Mat& GetSrcPageGrayImg() { return m_srcGrayImg; }

	bool IsCandidate() { return m_bCandidate; }
	RECT2D ConvertVec3DtoImgateCoord(POINT3D v1, POINT3D v2);

	void AnimatePos(bool IsZvalue);
	bool LoadThumbImage(unsigned short resolution);
	GLuint LoadFullImage();
	GLuint ConvertGLTexSize(int _size);
	void UploadThumbImage();

	// Draw Functions ======================//
	void DrawThumbNail(float fAlpha);
	void DrawMatchItem();
	void DrawForPicking();
	void DrawMatchItemForPick();
	void DrawParagraph(int selid, bool IsMergeMode);
	void DrawOCRResForPick();
	void DrawParagraphForPick();
	void DrawSelectedParagraph(int selid);
	void DrawSDBItem();
	//====================================//

	void RotatePos(float fSpeed);
	float SetSelectionPosition(int nSlot, float xOffset, float yOffset, bool IsAni);
	void SetSelection(bool _isSel);
	bool AddMatchedPoint(stMatchInfo info, int search_size);
	void AddParagraph(CExtractor& extractor, cv::Mat& paraImg, cv::Rect rect, bool IsVerti, float deskew, bool IsAlphabetic);	
	bool IsDuplicate(stMatchInfo& info, int search_size);

//	float GetDeskewParam(int pid);
	_stOCRResult GetOCRResult(int _lineid, int _id);
	void CMNPageObject::SetOCRResult(int _lineid, int _id, _stOCRResult _re);

	// Edit Paragraph box //
	void DeleteSelPara(int selid);
	void DeleteOCRResByRect(cv::Rect rect);
	void CleanUpOCRres();
	cv::Rect GetSelParaRect(int selid);
	unsigned int DeleteSelOCRRes(int lid, int selid);
	void ConfirmOCRRes(int lid, int selid);
	bool UpdateOCRCode(CString _strCode, float _fConfi, int lid, int selid);
	void UpdateOCRResStatus(int lid, int selid, bool IsUpdate, int _type);
	void UpdateLineStatus(int _id, bool IsCNS);
	
	// OCR //
	std::vector<stParapgraphInfo>& GetVecParagraph() { return m_paragraph; }
//	std::vector<_stOCRResult>& GetVecOCRResult() { return m_ocrResult; }
	void AddOCRResult(int lineid, _stOCRResult res);
	void AddDBSearchResult(cv::Rect _rect);
	void ClearDBSearchResult();


	CString GetPInfoPath(CString strExtension);
	void WritePageInfo();
	void WriteSearchDBFile();
	void UpdateDataBaseFiles();
	bool LoadPageInfo(unsigned short& width, unsigned short& height);
	void EncodeTexBoxHori();
	void EncodeTexBoxVerti(CFile& cfile);

	void ClearParagraph();
	void ClearOCRResult();
	void DeleteAllOcrRes();
	void DeleteAllOcrResInLine(int lineid);

	bool IsNeedToExtract();


	stParapgraphInfo GetLineBoxInfo(int pid);
	void SortLines(bool IsVerti);
private:
	// Basic Information //
	CString m_strPath;
	CString m_strPName;
	CString m_strName;
	unsigned long parentCode;
	unsigned long m_nCode;

//	GLuint m_thumbnailTexId;
	GLuint m_texId;
//	GLuint m_texIdforExtract;

	RECT2D m_ImgPlaneSize;
	RECT2D m_ImgRectSize;

	unsigned short m_nImgWidth;
	unsigned short m_nImgHeight;
	float m_fARatio;

	// Geometry Data ================//
	POINT3D m_vertex[4];
	POINT3D m_vertexBg[4];
	POINT2D m_texcoord[4];
	POINT3D m_vBgColor;
	bool	m_bIsSelected;
	bool	m_bIsSearching;

	float m_fXScale, m_fYScale;

	POINT3D m_pos;
	POINT3D m_targetPos;
	//================================//

	bool m_bAniPos;
	POINT3D m_MoveVec;
	short m_nAniCnt;

	bool m_bCandidate;
	bool m_bIsNear;
	bool m_IsNeedToSave;
	bool m_bImageChanged;

	std::vector<stMatchInfo> m_matched_pos;
	std::vector<stParapgraphInfo> m_paragraph;
//	std::vector<_stOCRResult> m_ocrResult;
	std::vector<stDBSearchRes> m_sdbResult;
	
	

	cv::Mat m_fullImg;
//	cv::Mat m_thumbImg;
	cv::Mat m_srcGrayImg;
	int m_selMatchItemId;
	bool m_IsTbimg;
};

