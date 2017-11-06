#pragma once

#include "OGLWnd.h"
#include "MNPageObject.h"
#include "OCRMng.h"
#include "Extractor.h"

enum _PICKMODE { _PICK_SELECT = 0 };
#define SEL_BUFF_SIZE 1024

static UINT ThreadGenThumbnailImg(LPVOID lpParam);
static UINT ThreadDoSearch(LPVOID lpParam);
static UINT ThreadDoExtraction(LPVOID lpParam);
static UINT ThreadDoOCR(LPVOID lpParam);

static bool  m_bIsThreadEnd;


class CMNView : public COGLWnd
{
public:
	CMNView();
	~CMNView();

	// Thread Functions===========//
	void ProcGenerateThumbnail();
	void ProcDoSearch();
	void ProcDoSearchSelection();
	void ProcExtractBoundary();
	void ProcOCR(bool IsAll);
	void ProcTrainingOCRResbyConfidence(float fConfi);
	void GenerateThumbnail();
	bool DoSearch();
	void DoExtractBoundary();
	void DoOCR();
	void DoOCRForPage(CMNPageObject* pPage);
	void DoOCRFromMooN();

	// OCR POP-UP Menu //
	//void OcrEnglishword();
	//void OcrEnglishChar();
	//void OcrChiChar();
	//void OcrChiWord();
	//void OcrKorWord();
	//void OcrKorChar();
	void OcrFromTextBox(_LANGUAGE_TYPE langType, int searchType);  // search type 0: character, 1: word
	void RemoveNoise();

	void DoOCRForCutImg(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage);
	
	void DoOCinResults(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage, std::vector<_stOCRResult>& ocrRes, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level, float fScale, int langType);
	void DoOCCorrection(cv::Mat& img, cv::Rect rect, CMNPageObject* pPage, std::vector<_stOCRResult>& ocrRes);
	bool MeargingtTextBox(std::vector<_stOCRResult>& vecBox, int& depth);
	bool MeargingtLineBox(std::vector<stParapgraphInfo>& vecBox, int& depth);
	void TrimTextBox(std::vector<_stOCRResult>& ocrRes, cv::Rect _rect);
	bool IsSymbol(wchar_t ch);
	void DoExtractBoundaryForSelected();
	//============================//

	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);	
	void UploadThumbnail();
	
	void Render();
	void Render2D();

	void EnableCutSearchMode(bool IsEnable, bool bKey);

	void DrawCNSRect(float r, float g, float b, float a);
	void DrawBGPageAni();
	void DrawBGPage();	
	void DrawOCRRes();

	void InitCamera(bool movexy = true);
	int SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode);
	int SelectObject3DForMouseOver(int x, int y, int rect_width, int rect_height, int selmode);
	void IDragMap(int x, int y, short sFlag);

	// For Picking =============//
	void DrawImageByOrderForPicking();
	void DrawMatchItemForPicking();
	void DrawParagrphForPicking();
	void DrawOCRForPicking();

	// Set values ==================//
	//void SetThreshold(float _th) {		m_fThreshold = _th;	}
	//void SetResColor(POINT3D _color) { m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; }
	void MoveCameraPos(POINT3D target, int zoom);

	void MovePrePage();
	void MoveNextPage();
	void MoveNextDown();
	void MoveNextUp();

	// Extraction Editing //
	void DeleteSelParagraph();
	bool DeleteSelOCRRes();
	void DeleteAllLines();
	void DeleteAllOCRRes();
	void AddOCRRes();
	void AddParagraph();
	void DeskewParagraph(float fAngle);
	void UndoDeskewParagraph();
	void ReExtractParagraph();
	void ExtractBox(cv::Mat& img, std::vector<_extractBox>& vecBox, bool IsVerti, _LANGUAGE_TYPE lang);
	void EncodePage();

	void ConfirmOCRRes();
	void UpdateOCRCode(CString _strCode);

	RECT2D GetSelectedAreaForCNS();
	void EnableShowLine(bool Isshow) {		m_bIsShowParagraph = Isshow;	}
	void SetDispConfidence(long nConfi) { m_dispConfi = (float)nConfi*0.01f; }
	void SetDBTreshold(long nDbth) { m_dbTreshold = (float)nDbth*0.01f; }
private:
	CPoint m_mousedown;
	CPoint m_preMmousedown;
	short m_mouseMode;

	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;

	// thumbnail image thread //
	unsigned int m_addImgCnt;
	size_t		 m_loadedImgCnt;
	//==================================//

	// Camera Animation //
	bool m_isAnimation;
	CPoint m_stratPnt, m_moveVec;
	short m_nAniCnt;
	POINT3D  m_AniMoveVec;
	float  m_fAniMoveSca;

	// For Cut & Search  //
	POINT3D m_PN, m_PO, m_CNSRectStart, m_CNSRectEnd;
	bool m_bIsCutNSearchMode;
	bool m_stateKeyDown;

	bool m_IsSearchMatchItems;
	int m_selMatchItemId;
	bool m_bIsShowParagraph;


	//For Template Matching=====//
	cv::Mat m_pCut;
	float m_fThreshold;
	float m_colorAccScale;
	COLORf m_resColor;
	_CUTINFO m_cutInfo;

	// For Extracting Box =====//
	int m_cnsSearchId;
	int m_selParaId;
	int m_selOCRId;
	int m_selOCRIdforMouseHover;

	CMNPageObject* m_pSelectPageForCNS;
	COCRMng m_OCRMng;
	bool m_bIsAllOCR;
	CExtractor m_Extractor;
	_stExtractionSetting m_extractionSetting;

	float m_dispConfi;
	float m_dbTreshold;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

