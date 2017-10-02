#pragma once


// COGLWnd
#include "Camera2Axis.h"

class COGLWnd : public CWnd
{
	DECLARE_DYNAMIC(COGLWnd)

public:
	COGLWnd();
	virtual ~COGLWnd();

	CDC* m_CDCPtr;	
	HGLRC		m_hRC;	
	HPALETTE	m_hPalette;	
	bool IsFirstSet;
	CCamera2Axis m_cameraPri;	



	// For Anti Aliasing ======//
	bool	arbMultisampleSupported;
	int		arbMultisampleFormat;

	
private:		
	bool WGLisExtensionSupported(const char *extension);
	bool InitMultisample(HDC hDC,PIXELFORMATDESCRIPTOR pfd);

	BOOL gl_init(HDC inhDC, HGLRC &outhRC, HPALETTE &outhPalette);
	virtual void Render(void){};
	virtual void InitGLview(void){};
	virtual void InitCamera(void){};

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	void MouseWheelEvent(UINT nFlags, short zDelta, CPoint pt);
	void gl_PushOrtho(int x1, int x2, int y1, int y2);
	void gl_PopOrtho();

	void gl_DrawText(POINT3D pos, CString strText, LOGFONT font, short align, BITMAPINFO* pBmp, CDC* pDC);	
	GLuint gl_GetTextTexture(CString str, LOGFONT font, BITMAPINFO* pBmp, CDC* pDC);

	HPALETTE gl_GetPalette(HDC inhDC);
	HRESULT gl_GetBitFont(CDC *cDCPtr, LOGFONT *LFont, CString strContent, BITMAP *bitmap, BITMAPINFO *m_pBmpInfo);

//	GLuint LoadBMPTexture(CString fileName);
//	GLuint LoadTgaTexture(CString filename);
};


