// OGLWnd.cpp : implementation file
//

#include "stdafx.h"
#include "OGLWnd.h"



// COGLWnd

IMPLEMENT_DYNAMIC(COGLWnd, CWnd)

COGLWnd::COGLWnd()
{
	m_CDCPtr = NULL;
	IsFirstSet = false;

	arbMultisampleSupported	= false;
	arbMultisampleFormat	= 0;
}

COGLWnd::~COGLWnd()
{
	//if (m_CDCPtr != NULL)
	//	delete m_CDCPtr;
}


BEGIN_MESSAGE_MAP(COGLWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// COGLWnd message handlers
BOOL COGLWnd::gl_init(HDC inhDC, HGLRC &outhRC, HPALETTE &outhPalette)
{
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		24,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer     
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	};

	// 픽셀 모드 선택
	int pixelFormat;



// First Pass ==========================================//
	if( !(pixelFormat = ChoosePixelFormat(inhDC, &pfd)) )
		return FALSE;


	// 컨텍스트에 대한 픽셀모드 설정
	if( !(SetPixelFormat(inhDC, pixelFormat, &pfd)) )
		return FALSE;

	// 픽셀 포맷을 설정 
	if( DescribePixelFormat(inhDC, pixelFormat, sizeof(pfd), &pfd) == 0 )
		return FALSE;

	// 렌더링 컨텍스트를 생성
	if( (outhRC = wglCreateContext(inhDC)) == NULL )
		return FALSE;

	// 현재의 렌더링 컨텍스트를 만든다.
	if( !(wglMakeCurrent(inhDC, outhRC)) ) 
		return FALSE;

//Second Pass======================================================//
// 	if(!arbMultisampleSupported){
// 		if(InitMultisample(inhDC,pfd)){
// 			FALSE;
// 		}
// 	}

	

	outhPalette = gl_GetPalette(inhDC);

	glClearDepth(1.0f);										   // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);						   // The Type Of Depth Testing To Do


	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_CULL_FACE);	
	glCullFace(GL_BACK);

	/////////// NEW ////////////////////////////
//  	glEnable(GL_COLOR_MATERIAL);
//  	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

//	glShadeModel(GL_FLAT);	
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);					// Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// Really Nice Perspective Calculations
//	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);				// Really Nice Point Smoothing
//	glEnable(GL_BLEND);

// 	glEnable(GL_MULTISAMPLE_ARB);	
 	glShadeModel(GL_SMOOTH);	
	

	glDisable(GL_LIGHTING);

	return TRUE;
}

bool COGLWnd::WGLisExtensionSupported(const char *extension)
{
	const size_t extlen = strlen(extension);
	const char *supported = NULL;
	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if (wglGetExtString)
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

	// If That Failed, Try Standard Opengl Extensions String
	if (supported == NULL)
		supported = (char*)glGetString(GL_EXTENSIONS);

	// If That Failed Too, Must Be No Extensions Supported
	if (supported == NULL)
		return false;

	// Begin Examination At Start Of String, Increment By 1 On False Match
	for (const char* p = supported; ; p++)
	{
		// Advance p Up To The Next Possible Match
		p = strstr(p, extension);

		if (p == NULL)
			return false;															// No Match

		// Make Sure That Match Is At The Start Of The String Or That
		// The Previous Char Is A Space, Or Else We Could Accidentally
		// Match "wglFunkywglExtension" With "wglExtension"

		// Also, Make Sure That The Following Character Is Space Or NULL
		// Or Else "wglExtensionTwo" Might Match "wglExtension"
		if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
			return true;															// Match
	}
}


bool COGLWnd::InitMultisample(HDC hDC,PIXELFORMATDESCRIPTOR pfd)
{

	/*
	if (!WGLisExtensionSupported("WGL_ARB_multisample"))
	{
		arbMultisampleSupported=false;
		return false;
	}

	// Get Our Pixel Format
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");	
	if (!wglChoosePixelFormatARB) 
	{
		arbMultisampleSupported=false;
		return false;
	}

	// Get Our Current Device Context
	int		pixelFormat;
	int		valid;
	UINT	numFormats;
	float	fAttributes[] = {0,0};

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To 
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware.
	int iAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,16,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
		WGL_SAMPLES_ARB,4,
		0,0
	};

	// First We Check To See If We Can Get A Pixel Format For 4 Samples
	valid = wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats);

	// If We Returned True, And Our Format Count Is Greater Than 1
	if (valid && numFormats >= 1)
	{
		arbMultisampleSupported = true;
		arbMultisampleFormat = pixelFormat;	
		return arbMultisampleSupported;
	}

	// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
	iAttributes[19] = 2;
	valid = wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats);
	if (valid && numFormats >= 1)
	{
		arbMultisampleSupported = true;
		arbMultisampleFormat = pixelFormat;	 
		return arbMultisampleSupported;
	}

	// Return The Valid Format
	return  arbMultisampleSupported;

	*/
	return 0;
}


HPALETTE COGLWnd::gl_GetPalette(HDC inhDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
	// Range for each color entry (7,7,and 3)

	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(inhDC);
	DescribePixelFormat(inhDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
	}

	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(inhDC,hRetPal,FALSE);
	RealizePalette(inhDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;

}



int COGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_CDCPtr = new CClientDC(this);
	gl_init(m_CDCPtr->m_hDC, m_hRC, m_hPalette);
	

//	InitGLview();	

	return 0;
}

void COGLWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here	
	

}

void COGLWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here
	if(wglMakeCurrent(m_CDCPtr->GetSafeHdc(),NULL)==FALSE) {}
//		AfxMessageBox("PIE wglMakeCurrent failed\n");
	if(m_hRC && (wglDeleteContext(m_hRC))==FALSE) {}
//		AfxMessageBox("PIE wglDeleteContext failed\n");

	if(m_CDCPtr)
		delete m_CDCPtr;
}

void COGLWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	Render();
}

BOOL COGLWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return 0;
}

void COGLWnd::gl_PushOrtho(int x1, int x2, int y1, int y2)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(x1,x2,y1,y2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void COGLWnd::gl_PopOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

GLuint COGLWnd::gl_GetTextTexture(CString str, LOGFONT font, BITMAPINFO* pBmp, CDC* pDC)
{
	GLuint texid = 0;

	BITMAP bm;
	memset(&bm, 0, sizeof(BITMAP));
	gl_GetBitFont(pDC, &font, str, &bm, pBmp);

	
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bm.bmWidth, bm.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte *)bm.bmBits);

	return texid;	
}

void COGLWnd::gl_DrawText(POINT3D pos, CString strText, LOGFONT font, short align, BITMAPINFO* pBmp, CDC* pDC)
{
	BITMAP bm;
	switch(align){
		case 0:	// LEFT			
			memset(&bm, 0, sizeof(BITMAP));
			gl_GetBitFont(pDC, &font, strText, &bm, pBmp);
			glRasterPos3f(pos.x, pos.y, pos.z);
			glBitmap( bm.bmWidth, bm.bmHeight,  (GLfloat)(bm.bmWidth+10), (GLfloat)(bm.bmHeight)*0.5f ,
				0.0f, (GLfloat)(bm.bmHeight*0.5f), (GLubyte *)bm.bmBits);			
			break;

		case 1:	// CENTER
			memset(&bm, 0, sizeof(BITMAP));
			gl_GetBitFont(pDC, &font, strText, &bm, pBmp);
			glRasterPos3f(pos.x, pos.y, pos.z);
			glBitmap( bm.bmWidth, bm.bmHeight,  (GLfloat)(bm.bmWidth*0.5f), (GLfloat)(bm.bmHeight*0.5f) ,
				(GLfloat)(bm.bmWidth*0.5f), (GLfloat)(bm.bmHeight*0.5f), (GLubyte *)bm.bmBits);
			break;

		case 2: // RIGHT
			memset(&bm, 0, sizeof(BITMAP));
			gl_GetBitFont(pDC, &font, strText, &bm, pBmp);
			glRasterPos3f(pos.x, pos.y, pos.z);
			glBitmap( bm.bmWidth, bm.bmHeight,  -2.0f, (GLfloat)(bm.bmHeight)*0.5f,
				(GLfloat)bm.bmWidth, (GLfloat)(bm.bmHeight*0.5f), (GLubyte *)bm.bmBits);
			break;
	}
	delete bm.bmBits;
}



HRESULT COGLWnd::gl_GetBitFont(CDC *cDCPtr, LOGFONT *LFont, CString strContent, BITMAP *bitmap, BITMAPINFO *m_pBmpInfo)
{
	CBitmap MemCBitmap, *OldMapPtr;
	CDC		MemCDC;

	CRect drawRect(0, 0, 0, 0);

	// -------------- calc drawRect -----------------------
	CFont font;
	//	VERIFY(font.CreateFontIndirect(LFont));
	font.CreateFontIndirect(LFont);
	CFont* def_font = cDCPtr->SelectObject(&font);

	cDCPtr->SetBkMode(OPAQUE);
	cDCPtr->DrawText(strContent, drawRect, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
	if (LFont->lfItalic)
		drawRect.right += (6 * GetDeviceCaps(cDCPtr->m_hDC, LOGPIXELSY)) / 72;

	cDCPtr->SelectObject(def_font);
	font.DeleteObject();

	//--------------- create bitmap ---------------------------
	MemCBitmap.CreateCompatibleBitmap(cDCPtr, drawRect.Width(), drawRect.Height());
	MemCDC.CreateCompatibleDC(NULL);

	OldMapPtr = MemCDC.SelectObject(&MemCBitmap);

	// ---------- draw ---------------------
	font.CreateFontIndirect(LFont);
	def_font = MemCDC.SelectObject(&font);
	MemCDC.FillSolidRect(drawRect, RGB(0, 0, 0));
	MemCDC.SetTextColor(RGB(255, 255, 255));
	MemCDC.DrawText(strContent, drawRect, DT_LEFT | DT_NOPREFIX);

	//---------------------- Get BitMap ------------------------------
	void* pre_ptr;
	pre_ptr = bitmap->bmBits;
	//	int pre_size = bitmap.bmHeight*bitmap.bmWidthBytes;

	MemCBitmap.GetBitmap(bitmap);
	bitmap->bmBitsPixel = 1;  // 1 bit

	div_t over = div((int)(bitmap->bmWidth*bitmap->bmBitsPixel), 8);
	bitmap->bmWidthBytes = over.quot;
	if (over.rem) bitmap->bmWidthBytes++;
	bitmap->bmWidthBytes = ((bitmap->bmWidthBytes + 3)&(~3));

	//	bitmap.bmWidthBytes = (bitmap.bmWidth*bitmap.bmBitsPixel)/8;
	//	if( (bitmap.bmWidth*bitmap.bmBitsPixel)%8 ) bitmap.bmWidthBytes++;
	//	bitmap.bmWidthBytes = ((bitmap.bmWidthBytes + 3)&(~3));

	//	bitmap.bmWidthBytes = ((bitmap.bmWidth*bitmap.bmBitsPixel/8 + 3)&(~3));	// 1 byte

	if (pre_ptr)
	{
		bitmap->bmBits = realloc(pre_ptr, bitmap->bmHeight * bitmap->bmWidthBytes);
		//bitmap.bmBits = LocalReAlloc((HLOCAL)pre_ptr, bitmap.bmHeight*bitmap.bmWidthBytes, LMEM_MOVEABLE);
	}
	else
	{
		bitmap->bmBits = malloc(bitmap->bmHeight * bitmap->bmWidthBytes);
		//bitmap.bmBits = LocalAlloc(LPTR, bitmap.bmHeight*bitmap.bmWidthBytes);
		//bitmap.bmBits = (unsigned char*)HeapAlloc(GetProcessHeap(), 0, bitmap.bmHeight*bitmap.bmWidthBytes);
	}

	memset(m_pBmpInfo, 0, sizeof(BITMAPINFOHEADER));
	BITMAPINFOHEADER* bmHedPtr = (BITMAPINFOHEADER*)m_pBmpInfo;

	bmHedPtr->biSize = sizeof(BITMAPINFOHEADER);
	bmHedPtr->biWidth = bitmap->bmWidth;
	bmHedPtr->biHeight = bitmap->bmHeight;
	bmHedPtr->biPlanes = bitmap->bmPlanes;
	bmHedPtr->biBitCount = bitmap->bmBitsPixel;

	int iR = GetDIBits(MemCDC.m_hDC,
		(HBITMAP)MemCBitmap.GetSafeHandle(),	// source CBitmap
		0L,
		bitmap->bmHeight,
		(LPBYTE)bitmap->bmBits,
		(LPBITMAPINFO)m_pBmpInfo,
		(DWORD)DIB_RGB_COLORS);


	MemCDC.SelectObject(OldMapPtr);
	MemCDC.SelectObject(def_font);
	//	MemCDC.DeleteDC();
	//	MemCBitmap.DeleteObject();
	//	font.DeleteObject();

	return S_OK;
}



/*
GLuint COGLWnd::LoadBMPTexture(CString fileName)
{

	USES_CONVERSION;
	char* sz = T2A(fileName);
 	FILE* fp = fopen(sz, "r");
	if(fp==NULL)
	{		
 		return 0;
	}
	fclose(fp);	

	
	GLuint tex;
	tex=0;
	AUX_RGBImageRec* m_texture;
	m_texture = auxDIBImageLoad(fileName);

	if(!m_texture)
	{
		return tex;
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);


	//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_texture->sizeX,m_texture->sizeY, GL_RGB, GL_UNSIGNED_BYTE, m_texture->data);


	//	delete m_texture->data;
	//	delete m_texture;
	free(m_texture->data);
	free(m_texture);
	return tex;

}
GLuint COGLWnd::LoadTgaTexture(CString filename)
{
	GLubyte	*imageData;											// Image Data (Up To 32 Bits)
	GLuint	bpp;												// Image Color Depth In Bits Per Pixel.
	GLuint	width;												// Image Width
	GLuint	height;												// Image Height
	GLuint	texID;												// Texture ID Used To Select A Texture



	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	GLubyte		TGAcompare[12];									// Used To Compare TGA Header
	GLubyte		header[6];										// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;									// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;										// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;											// Temporary Variable
	GLuint		type=GL_RGBA;									// Set The Default GL Mode To RBGA (32 BPP)


	USES_CONVERSION;
	char* sz = T2A(filename);

	FILE *file = fopen(sz, "rb");							// Open The TGA File

	if(	file==NULL ||											// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)										// Did The File Even Exist? *Added Jim Strong*
			return FALSE;										// Return False
		else													// Otherwise
		{
			fclose(file);										// If Anything Failed, Close The File
			return FALSE;										// Return False
		}
	}

	width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)

	if(	width	<=0	||									// Is The Width Less Than Or Equal To Zero
		height	<=0	||									// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))						// Is The TGA 24 or 32 Bit?
	{
		fclose(file);											// If Anything Failed, Close The File
		return FALSE;											// Return False
	}

	bpp	= header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= bpp/8;							// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= width*height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	imageData=(GLubyte *)malloc(imageSize);			// Reserve Memory To Hold The TGA Data

	if(	imageData==NULL ||								// Does The Storage Memory Exist?
		fread(imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(imageData!=NULL)							// Was Image Data Loaded
			free(imageData);							// If So, Release The Image Data

		fclose(file);											// Close The File
		return FALSE;											// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)			// Loop Through The Image Data
	{															// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=imageData[i];								// Temporarily Store The Value At Image Data 'i'
		imageData[i] = imageData[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
		imageData[i + 2] = temp;						// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);												// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texID);						// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texID);				// Bind Our Texture

	//	glTexParameterf(GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);






	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

	if (bpp==24)										// Was The TGA 24 Bits
	{
		type=GL_RGB;											// If So Set The 'type' To GL_RGB
	}
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, type, GL_UNSIGNED_BYTE, imageData);

	//int nSizeX = width;
	//int nSizeY = height;
	//for(int mipmapLevel = 0; mipmapLevel<=6; mipmapLevel++, nSizeX = max(nSizeX/2,1),nSizeY = max(nSizeY/2,1)){
	//	glTexImage2D(GL_TEXTURE_2D, mipmapLevel, type, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);
	//}

	free(imageData);
	return texID;												// Texture Building Went Ok, Return True

}
*/