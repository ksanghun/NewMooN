#pragma once

#include "math_tool.h"
//#include "DEMProjector.h"

class CCamera2Axis
{
public:
	CCamera2Axis(void);
	~CCamera2Axis(void);

	FRUSTUM	GetFrustum(float ratio=1.0f);
	RECT3D	GetViewExtentWolrd() { return m_ViewExtentWorld; }
	RECT	GetViewExtentScreen() {return m_ViewExtentScreen; }
	RECT	GetViewportRect();
	int*	GetViewport() {return m_viewport; }
	GLfloat GetFocalLength();
	GLfloat GetLevelHeight() {return m_levelHeight; }
	POINT3D GetLookAt() {return m_vLookat; }
	GLfloat GetAngleX()  { return m_fAngleX;	}
	GLfloat GetAngleZ() { return m_fAngleZ;  }
	POINT3D GetEye()  { return m_vEye;	}
	POINT3D GetEyeUp()	{ return m_vEyeUp;}
	float   GetFarPlane()	{ return m_farPlane;	}
	float   GetNearPlane()	{ return m_nearPlane;	}
	float   GetLevelScale() { return m_levelScale;	}
	int		GetDemLevel();

	void SetViewPort(int x, int y, int width, int height);	
	void UpdateExtent();
	void DrawExtent();
	void UpdateExtentByTerrain(POINT3D* pnt);
	void UpdateFrustum();
	void InitializeCamera(GLfloat fov, GLfloat angleX, GLfloat angleZ, POINT3D pLookAt,int width, int height);
	BOOL CamStatus();

	void SetProjectionMatrix(GLfloat fov, GLfloat x, GLfloat y, int width, int height);
	void SetModelViewMatrix(POINT3D ptPos, GLfloat fAngle_DX, GLfloat fAngle_DZ);
	void SetModelViewMatrixNoLimit(POINT3D ptPos, GLfloat fAngle_DX, GLfloat fAngle_DZ);
	void SetLevelHeight(float fHeight_D);
	void SetInitLevelHeight(float fHeight_D);
	POINT3D ScreenToWorld(int x, int y);
	void DrawBaseExtRect();

	void gl_PushOrtho3D(float z1, float z2);
	void gl_PopOrtho3D();
	void gl_PushOrtho2D(int x1, int x2, int y1, int y2);
	void gl_PopOrtho2D();

	int m_currentBlockid;
	POINT3D m_ExtPnt[4];
	POINT3D m_BaseExtPnt[4];
	int m_nWidthDisp;
	int m_nHeightDisp;
	GLdouble m_matModelView[16];
	GLdouble	m_matCamera[16];			// m_matProjection*m_matModelView;
	GLdouble m_matProjection[16];

	BOOL _ComputeProjectPoint(const float& x, const float& y, const float& z, POINT2D *sc);
	BOOL _ComputeUnProjectPoint(float winx, float winy, float winz, float *unProjPnt);
	BOOL _ComputeUnProjectPoint2(float winx, float winy, float winz, float *unProjPnt);

	CString GetCamInfo() {return m_StrCamInfo;};
	BOOL InsetsectRayToPlane(POINT3D pN, POINT3D pO, int x, int y, POINT3D&hitPos);
//	void SetDemProj(CDEMProjector* pDemProj);

	bool m_TerrainMode;
	float m_Cntfovy;
	float m_aspect;


private:
	
	FRUSTUM _ComputeFrustumPerspective(float ratio);
	float _ComputeOffsetVAngle();
	
	GLint InvertMatrixf2(double *m, double *out);
	void  MultiplyMatrixByVector4by4(double *resultvector, const double *matrix, const double *pvector);




	
private:
	GLint	m_viewport[4];
	
	

	GLfloat m_fAngleZ;
	GLfloat m_fAngleX;	

	POINT3D m_vEyeUp;
	POINT3D m_vEye;
	POINT3D m_vLookat;	
	GLfloat m_levelHeight;

	
	float m_Cntfovy_r2;
	float m_Cntfovy_r;



	
	float m_nearPlane;
	float m_farPlane;

	GLfloat m_levelScale;
	
	BOOL m_bNaviCurrent;
	BOOL m_bMiniMap;
	BOOL m_bDynamic;
	UINT m_bDRVUp;

	float m_fShiftVal;

	// ViewExtent
	RECT m_ViewExtentScreen;
	RECT3D m_ViewExtentWorld;
	float m_ExtentSize;

	// Temp
	double* matTemp1;
	double* matTemp2;
	float*  matTemp3;

	// For Terrain Loading
	int m_level;
	CString m_StrCamInfo;
	
};
