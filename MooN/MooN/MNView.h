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
static bool  m_bIsThreadEnd;


class CMNView : public COGLWnd
{
public:
	CMNView();
	~CMNView();

	// Thread Functions===========//
	void ProcGenerateThumbnail();
	void ProcDoSearch();
	void ProcExtractBoundary();
	void GenerateThumbnail();
	bool DoSearch();
	void DoExtractBoundary();
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
	void InitCamera(bool movexy = true);
	int SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode);
	void IDragMap(int x, int y, short sFlag);

	// For Picking =============//
	void DrawImageByOrderForPicking();
	void DrawMatchItemForPicking();

	// Set values ==================//
	//void SetThreshold(float _th) {		m_fThreshold = _th;	}
	//void SetResColor(POINT3D _color) { m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; }
	void MoveCameraPos(POINT3D target, int zoom);

	void MovePrePage();
	void MoveNextPage();
	void MoveNextDown();
	void MoveNextUp();

	RECT2D GetSelectedAreaForCNS();
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


	//For Template Matching=====//
	cv::Mat m_pCut;
	float m_fThreshold;
	float m_colorAccScale;
	COLORf m_resColor;
	_CUTINFO m_cutInfo;

	// For Extracting Box =====//
	int m_cnsSearchId;

	CMNPageObject* m_pSelectPageForCNS;
	COCRMng m_OCRMng;
	CExtractor m_Extractor;
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
};

