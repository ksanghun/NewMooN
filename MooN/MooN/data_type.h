#pragma once
//#include "stdafx.h"
#include "MNCVMng.h"

#define MAX_TER_HEIGHT 500000
#define PAGE_HSIZE 512
#define PAGE_WSIZE 360
#define _PAGE_SHIFT 360
//  #define PAGE_HSIZE 360
//  #define PAGE_WSIZE 512
//  #define _PAGE_SHIFT 512


#define _PAGE_RES 512
#define _MAX_WORD_SIZE 32
#define THUMBNAIL_SIZE 64
//#define _IMGICON_SIZE 100.0f
//#define _MAX_TREE_DEPTH 7

#pragma pack(1)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef short SHORT;

struct POINT3D
{
    float x,y,z;
};


struct POINT3Di
{
	short x,y,z;
};

struct POINT2D
{
	float x,y;
};

struct POINT2Di
{
	short x,y;
};

struct COLORi
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

struct COLORf
{
	float r;
	float g;
	float b;
	float a;
};

typedef POINT2D UV;

struct VERTEXi
{	
	POINT3Di vertex;
	POINT2Di uv;
};

// building 용 
struct VERTEXb
{	
	POINT3Di vertex;
	WORD reserved;
	POINT2Di uv;
	BYTE	 color[4];
};

struct FACE
{
	int v1,v2,v3;
	int no_material;
	void set(int V1,int V2,int V3, int NoMaterial) {v1=V1; v2=V2; v3=V3; no_material=NoMaterial; };
};

struct POINT2D_D
{
	double x,y,w;
	void set(double X=0, double Y=0, double W=1) { x=X; y=Y; w=W;};
};

struct POINT3D_D
{
	double x,y,z,w;
	void set(double X=0, double Y=0, double Z=0, double W=1) { x=X; y=Y; z=Z; w=W;};

};

struct VERTEX
{	
	POINT3D vertex;
	POINT2D uv;
};


struct RECT3D
{
	float x1,x2;
	float y1,y2;
	float z1,z2;

	RECT3D(float X1,float X2, float Y1,float Y2,float Z1,float Z2) {x1=X1; x2=X2; y1=Y1; y2=Y2; z1=Z1; z2=Z2; };
	RECT3D() {x1=x2=y1=y2=z1=z2=0.0f;};
	inline void set(float X1,float X2, float Y1,float Y2,float Z1,float Z2) {x1=X1; x2=X2; y1=Y1; y2=Y2; z1=Z1; z2=Z2;};

};


struct RECT2D
{
	float x1, x2;
	float y1, y2;
	float width, height;

	RECT2D(float X1, float X2, float Y1, float Y2) { x1 = X1; x2 = X2; y1 = Y1; y2 = Y2; };
	RECT2D() { x1 = x2 = y1 = y2 = width = height = 0.0f; };
	inline void set(float X1, float X2, float Y1, float Y2) { 
		x1 = X1; x2 = X2; y1 = Y1; y2 = Y2; 
		width = x2 - x1;
		height = y2 - y1;
	};
};


struct PLANE3D
{
	float x,y,z;
	float dist;
	inline void set(float X,float Y, float Z,float Dist) {x=X; y=Y; z=Z; dist=Dist; };
};

struct LINE_SEGMENT
{
	POINT3D v1;
	POINT3D v2;
};

struct FRUSTUM
{
	PLANE3D leftPlane;
	PLANE3D rightPlane;
	PLANE3D topPlane;
	PLANE3D bottomPlane;
	PLANE3D nearPlane;
	PLANE3D farPlane;
};

struct GLES_COLOR
{
	float r;
	float g;
	float b;
	float a; 
};

struct EXTENT
{
	int left;
	int bottom;
	int right;
	int top;
};

struct IEXTENT
{
	WORD left;
	WORD bottom;
	WORD right;
	WORD top;
};

struct SEXTENT
{
	SHORT left;
	SHORT bottom;
	SHORT right;
	SHORT top;
};

struct A_FEXTENT
{
	float fLeft;
	float fBottom;
	float fRight;
	float fTop;
};

struct BASE_INDEX
{
	WORD index[3];
};

struct SIDE_INDEX
{
	WORD index;
};

typedef DWORD MESH_LEVEL;

#pragma pack(1)

#define		MDC_HEADER_SIZE		50
#define		DRM_SIZE			320

struct LAYER_HEADER
{
	//	void Load(const char* pMem);

	WORD   level;		// not used
	BYTE   compress;	// 압축 유무 ( 0 : 1 )
	BYTE   drm;			// DRM 유무 ( 0: 1 )
	BYTE   objtype;		// 
	BYTE   indexed;		//
	DWORD  filesize;	// 전체파일크기
	EXTENT extent;		// Origin~Mesh*MeshNum
	DWORD  numobj;		// 
	WORD   numhmesh;	// 48(6레벨경우)
	WORD   numvmesh;	// 66(6레벨경우)
	DWORD  meshwidth;	// 12000(6레벨경우)
	DWORD  meshheight;	// 10000(6레벨경우)
	DWORD  originx;		// 
	DWORD  originy;		// 
};




template<class X>
class A_auto_array
{
public:
	A_auto_array(X* p)
	{
		m_p = p;
	}

	~A_auto_array()
	{
		if (m_p)
			delete[] m_p;
	}

	void auto_alloc(X* p)
	{
		m_p = p;
	}

	inline X* get(void)
	{
		return m_p;
	}

	X* m_p;
};

//리소스 data ===============
 #pragma pack(push, 1)

//landmark, template, texture, sky&mount, exlandmark, extemplate
enum RES_TYPE
{
	RES_TEXTURE,
	RES_TEMPLATE,
	RES_LANDMARK,
	RES_EXTEMPLATE,
	RES_EXLANDMARK,
	RES_SKY
}; 


/*----------------------------------------------------------------------------------------------
일반 tga 압축
IMG_888		:: Image type : RGB,	Image format : GL_UNSIGNED_BYTE
IMG_8888	:: Image type : RGBA,	Image format : GL_UNSIGNED_BYTE
IMG_565		:: Image type : RGB,	Image format : GL_UNSIGNED_SHORT_5_6_5
IMG_4444	:: Image type : RGBA,	Image format : GL_UNSIGNED_SHORT_4_4_4_4
IMG_5551	:: Image type : RGBA,	Image format : GL_UNSIGNED_SHORT_5_5_5_1

PVRTC Compress
IMG_PVRTC_RGB_2BPP	:: Image type : RGB,	Image format : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
IMG_PVRTC_RGBA_2BPP	:: Image type : RGBA,	Image format : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
IMG_PVRTC_RGB_4BPP	:: Image type : RGB,	Image format : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
IMG_PVRTC_RGBA_4BPP	:: Image type : RGBA,	Image format : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
-----------------------------------------------------------------------------------------------*/
enum IMG_TYPE
{
	IMG_888,
	IMG_8888,
	IMG_565,
	IMG_4444,
	IMG_5551,
	IMG_PVRTC_RGB_2BPP,
	IMG_PVRTC_RGBA_2BPP,
	IMG_PVRTC_RGB_4BPP,
	IMG_PVRTC_RGBA_4BPP
};

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG			0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG			0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG			0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG			0x8C03


//==============================
#pragma pack(pop)
//======================

#define DP_MAX_PATH 260

#define MAX_NUM_BUFFER 3000
#define A_PI 3.1415926535897932384626433832795f
#define A_PI_2 1.5707963267948966192313216916398f
#define A_TWO_PI 6.283185307179586476925286766559f
#define METER(A) A*0.91f
















// 색 변환
#define RGB565_TO_RGBA4444(C,A) ((C & 0xf000) | ((C & 0x0780) << 1) | ((C & 0x001e) << 3)  | (A>>4))

#define RGB888_TO_RGB565(C)	(((C & 0xf80000) >> 8) | \
	((C & 0x00fc00) >> 5) | \
	((C & 0x0000f8) >> 3))

#define BGR888_TO_RGB565(C)	(((C & 0xf80000) >> 19) | \
	((C & 0x00fc00) >> 5) | \
	((C & 0x0000f8) << 8 ))









#pragma pack(1)


typedef struct VA_DRAG_OPER{
	POINT3D pos, xDir, yDir;
	float xPixelDiff;
	float yPixelDiff;
	int px, py;
	bool IsDrag;

	void init()
	{
		IsDrag = false;
	//	mtSetPoint3D(&pos, 0,0,0);
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		px=0;
		py=0;
		xPixelDiff = 0;
		yPixelDiff = 0;

		xDir.x = 0;
		xDir.y = 0;
		xDir.z = 0;
		yDir.x = 0;
		yDir.y = 0;
		yDir.z = 0;

	};
}VA_DRAG_OPER;


typedef struct _BOOK_DESC
{
	CString strName;
	CString strDate;
	CString strBookTitle;
	CString strAddress1;
	CString strRole;
	CString strAddress2;
	POINT3D pos;

}_BOOK_DESC;


typedef struct _COMPARE_INFO
{
	POINT2D pos;
	float fValue;
	POINT3D color;
	float fAlpha;
	float fSize;
	int level;
	bool	IsRefine;
}_COMPARE_INFO;



typedef struct _SNIMAGE
{
	CString strPath;
	CString strPName;
	CString strName;

	unsigned long parentCode;
	unsigned long nCode;
	
	unsigned short nWidth;
	unsigned short nHeight;
	float fARatio;

	GLuint thTexId, texId;	
}_SNIMAGE;





struct WORD_POS{
	unsigned short x1, x2, y1, y2;
	unsigned long pagecode;
	unsigned long wid;
};

struct _CUTINFO{
	unsigned long id;
	unsigned long fileid;
	unsigned long posid;
	float th;

	void init() {
		id = 0;
		fileid = 0;
		posid = 0;
		th = 0;
	}
};


struct _stExtractionSetting {
	bool isEng;
	bool isChi;
	bool isKor;
//	int nAlign;
	bool IsVerti;

	int engSize;
	int chiSize;
	int korSize;

	void init()
	{
		isEng = false;
		isChi = false;
		isKor = false;
		IsVerti = true;

		engSize = 1;
		chiSize = 1;
		korSize = 1;
	};
};

struct _stOCRResult {
	cv::Rect rect;
	float fConfidence;
	wchar_t strCode[_MAX_WORD_SIZE];
	unsigned int hcode_s;
	int type;  // 0 Eng, 1: chi, 2: Kor   + line information without changing format
	bool bNeedToDB;


	bool IsMatched;
//	unsigned short lineid;

	void init() {
		fConfidence = 0.0f;
		rect = cv::Rect(0, 0, 0, 0);
		type = 0;
		memset(strCode, 0x00, sizeof(strCode));
		bNeedToDB = false;
		hcode_s = 5381;
	};

	void set(cv::Rect _r, float _fConfi, wchar_t* _code){
		rect = _r;
		fConfidence = _fConfi;
		memcpy(strCode, _code, sizeof(wchar_t)*_MAX_WORD_SIZE);
		type = 0;
		bNeedToDB = false;
		hcode_s = 5381;
//		lineid = _line;
	};
};

//struct _stOCRResultDB {
//	cv::Rect rect;
//	float fConfidence;
//	wchar_t strCode[_MAX_WORD_SIZE];
////	unsigned short lineId;
//	
//	// Reserve Code //
//	char reserve[64];
//	void set(cv::Rect _r, float _fConfi, wchar_t* _code /*unsigned short _line*/) {
//		rect = _r;
//		fConfidence = _fConfi;
//		memcpy(strCode, _code, sizeof(wchar_t)*_MAX_WORD_SIZE);
////		lineId = _line;
//		memset(reserve, 0x00, 64);
//	};
//};

struct _stSDBWord {
	unsigned int filecode;
	unsigned int strcode;
	cv::Rect rect;
	float fConfi;
	float fDiff;
};

struct _stCNSResult {
	unsigned int searchid;
	unsigned int pageid;
	unsigned int objid;
	unsigned int lineid;
//	_stCNSResult* pKey;
	cv::Mat cutimg;
	cv::Rect rect;

	float fConfi;
};

typedef std::vector<_stSDBWord> _stSDB;

struct _stSDBWordTable {
	wchar_t str[_MAX_WORD_SIZE];
};

struct _stParaInfoFormat {
	unsigned short x, y, width, height;	// position and size
	float desKew;
	unsigned char isDeskew;
	unsigned short nAlign;
};



#pragma pack()