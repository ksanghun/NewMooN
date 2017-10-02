#include "StdAfx.h"
#include "Camera2Axis.h"

//#define MIN_VERTICAL_ANGLE 285.0f 

#define MIN_VERTICAL_ANGLE 295.0f 
#define MAT(m,r,c) (m)[(c)*4+(r)]
#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }

CCamera2Axis::CCamera2Axis(void)
{
	memset(m_viewport,		 0,	sizeof(m_viewport)		);
	memset(m_matProjection,	 0,	sizeof(m_matProjection)	);
	memset(m_matModelView,	 0,	sizeof(m_matModelView)	);	
	memset(m_matCamera,		 0,	sizeof(m_matCamera)		);
	

	m_fAngleX=0.0f;
	m_fAngleZ=0.0f;

	m_vEye.x=m_vEye.y=m_vEye.z=0.0f;
	m_vLookat.x=m_vLookat.y=m_vLookat.z=0.0f;

	m_levelHeight=10000.55f;
	m_levelScale=1.0f;
	
	m_Cntfovy=0.0f;
	m_Cntfovy_r=0.0f;
	m_Cntfovy_r2=0.0f;
	
	m_aspect=1.0f;
	m_nearPlane=0.1f;
	m_farPlane=30000.0f;

	m_nWidthDisp=0;
	m_nHeightDisp=0;

	m_bNaviCurrent=FALSE;
	m_bMiniMap=FALSE;
	m_bDynamic=FALSE;

	matTemp1 = new double[16];
	matTemp2 = new double[16];
	matTemp3 = new float[16];

	for(int i=0; i<4; i++){
		mtSetPoint3D(&m_ExtPnt[i], 0,0,0);
	}

	m_TerrainMode = false;

}

CCamera2Axis::~CCamera2Axis(void)
{
	delete [] matTemp1;
	delete [] matTemp2;
	delete [] matTemp3;


}



void CCamera2Axis::InitializeCamera(GLfloat fov, GLfloat angleX, GLfloat angleZ, POINT3D pLookAt, int width, int height)
{
	m_Cntfovy = fov;
	m_Cntfovy_r = m_Cntfovy/180.0f*A_PI;
	m_Cntfovy_r2 = m_Cntfovy_r/2;
//	m_fAngleZ=angleZ;
//	m_fAngleX=angleX;
	m_vLookat = pLookAt;

	m_viewport[0]=0;
	m_viewport[1]=0;
	m_viewport[2]=width;
	m_viewport[3]=height;

	m_nWidthDisp=width;
	m_nHeightDisp=height;
	if (height > 0)
		m_aspect = GLfloat(width) / height;
	else
		m_aspect = 1.0f;
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_Cntfovy ,m_aspect, 0.1f,100000.0f);	
	glGetDoublev(GL_PROJECTION_MATRIX, m_matProjection);	

	SetModelViewMatrix(pLookAt, 0, 0);
//	UpdateExtent();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();	
}

BOOL CCamera2Axis::CamStatus()
{
	glViewport(m_viewport[0],m_viewport[1],m_viewport[2],m_viewport[3]);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixd(m_matProjection);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(m_matModelView);
	

	return TRUE;
}


void CCamera2Axis::SetViewPort(int x, int y, int width, int height)
{
	m_viewport[0]=x;
	m_viewport[1]=y;
	m_viewport[2]=width;
	m_viewport[3]=height;
	m_aspect=GLfloat(width)/height;

	glViewport(m_viewport[0],m_viewport[1],m_viewport[2],m_viewport[3]);
}


void CCamera2Axis::UpdateFrustum()
{
	m_farPlane = m_levelHeight*2.0f;
	if(m_farPlane<2000)
		m_farPlane = 2000;

	m_nearPlane = 0.1f;
}

void CCamera2Axis::SetProjectionMatrix(GLfloat fov, GLfloat x, GLfloat y, int width, int height)
{	
	m_Cntfovy = fov;
	m_Cntfovy_r = m_Cntfovy/180.0f*A_PI;
	m_Cntfovy_r2 = m_Cntfovy_r/2;

	m_viewport[0] = (GLint)x;
	m_viewport[1] = (GLint)y;
	m_viewport[2] = width;
	m_viewport[3] = height;

	glViewport(m_viewport[0],m_viewport[1],m_viewport[2],m_viewport[3]);

	m_nWidthDisp=width;
	m_nHeightDisp=height;
	m_aspect=(GLfloat)width/(GLfloat)height;

//	m_farPlane = m_levelHeight*2.0f;
//	m_farPlane = m_levelHeight*10.0f;
	m_farPlane = 2000000;
	if(m_farPlane<7000)
		m_farPlane = 7000;
	m_nearPlane = m_levelHeight*0.1f;
	//	m_nearPlane = 0.5f;

	

	// Set Projection Matrix===========//
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_Cntfovy ,m_aspect, m_nearPlane,m_farPlane);	
	glGetDoublev(GL_PROJECTION_MATRIX, m_matProjection);	

	// Apply Projection Matrix==========//
}


void CCamera2Axis::UpdateExtentByTerrain(POINT3D* pnt)
{
	m_ViewExtentWorld.set(pnt[0].x,pnt[0].x,pnt[0].y,pnt[0].y,0,0);
	for(int i=1; i<4; i++){
		if(m_ViewExtentWorld.x1>pnt[i].x)			m_ViewExtentWorld.x1 = pnt[i].x;
		if(m_ViewExtentWorld.x2<pnt[i].x)			m_ViewExtentWorld.x2 = pnt[i].x;

		if(m_ViewExtentWorld.y1>pnt[i].y)			m_ViewExtentWorld.y1 = pnt[i].y;
		if(m_ViewExtentWorld.y2<pnt[i].y)			m_ViewExtentWorld.y2 = pnt[i].y;
	}

	if(m_ViewExtentWorld.x1>m_vEye.x)			m_ViewExtentWorld.x1 = m_vEye.x;
	if(m_ViewExtentWorld.x2<m_vEye.x)			m_ViewExtentWorld.x2 = m_vEye.x;

	if(m_ViewExtentWorld.y1>m_vEye.y)			m_ViewExtentWorld.y1 = m_vEye.y;
	if(m_ViewExtentWorld.y2<m_vEye.y)			m_ViewExtentWorld.y2 = m_vEye.y;

	m_ViewExtentWorld.x1-=256;
	m_ViewExtentWorld.x2+=256;
	m_ViewExtentWorld.y1-=256;
	m_ViewExtentWorld.y2+=256;

}


void CCamera2Axis::UpdateExtent()
{
	m_ExtPnt[0] = ScreenToWorld(1,1);
	m_ExtPnt[1] = ScreenToWorld(1, m_nHeightDisp-1);
	m_ExtPnt[2] = ScreenToWorld(m_nWidthDisp-1, m_nHeightDisp-1);
	m_ExtPnt[3] = ScreenToWorld(m_nWidthDisp-1, 1);	


	m_ViewExtentWorld.set(m_ExtPnt[0].x,m_ExtPnt[0].x,m_ExtPnt[0].y,m_ExtPnt[0].y,0,0);
	for(int i=1; i<4; i++){
		if(m_ViewExtentWorld.x1>m_ExtPnt[i].x)			m_ViewExtentWorld.x1 = m_ExtPnt[i].x;
		if(m_ViewExtentWorld.x2<m_ExtPnt[i].x)			m_ViewExtentWorld.x2 = m_ExtPnt[i].x;

		if(m_ViewExtentWorld.y1>m_ExtPnt[i].y)			m_ViewExtentWorld.y1 = m_ExtPnt[i].y;
		if(m_ViewExtentWorld.y2<m_ExtPnt[i].y)			m_ViewExtentWorld.y2 = m_ExtPnt[i].y;
	}

	if(m_ViewExtentWorld.x1>m_vEye.x)			m_ViewExtentWorld.x1 = m_vEye.x;
	if(m_ViewExtentWorld.x2<m_vEye.x)			m_ViewExtentWorld.x2 = m_vEye.x;

	if(m_ViewExtentWorld.y1>m_vEye.y)			m_ViewExtentWorld.y1 = m_vEye.y;
	if(m_ViewExtentWorld.y2<m_vEye.y)			m_ViewExtentWorld.y2 = m_vEye.y;

	//m_ViewExtentWorld.x1-=256;
	//m_ViewExtentWorld.x2+=256;
	//m_ViewExtentWorld.y1-=256;
	//m_ViewExtentWorld.y2+=256;
}

void CCamera2Axis::DrawExtent()
{

	
	//glBegin(GL_LINE_STRIP);
	//glColor3f(1,1,0);
	//glVertex3f(m_ExtPnt[0].x, m_ExtPnt[0].y, 0.0f);
	//glVertex3f(m_ExtPnt[1].x, m_ExtPnt[1].y, 0.0f);
	//glVertex3f(m_ExtPnt[2].x, m_ExtPnt[2].y, 0.0f);
	//glVertex3f(m_ExtPnt[3].x, m_ExtPnt[3].y, 0.0f);
	//glVertex3f(m_ExtPnt[0].x, m_ExtPnt[0].y, 0.0f);
	//glEnd();

	
 	glColor3f(1,1,1);
 	glBegin(GL_LINE_STRIP);
 	glVertex3f(m_ViewExtentWorld.x1, m_ViewExtentWorld.y1, 0.0f);
 	glVertex3f(m_ViewExtentWorld.x1, m_ViewExtentWorld.y2, 0.0f);
 	glVertex3f(m_ViewExtentWorld.x2, m_ViewExtentWorld.y2, 0.0f);
 	glVertex3f(m_ViewExtentWorld.x2, m_ViewExtentWorld.y1, 0.0f);
 	glVertex3f(m_ViewExtentWorld.x1, m_ViewExtentWorld.y1, 0.0f);
 	glEnd();
 	glLineWidth(1);


	
	glBegin(GL_LINES);
 	glColor3f(1.0f,1.0f,1.0f);
 	glVertex3f(m_ExtPnt[0].x, m_ExtPnt[0].y, 0.0f);
	glVertex3f(m_ExtPnt[1].x, m_ExtPnt[1].y, 0.0f);
	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);

	glVertex3f(m_ExtPnt[1].x, m_ExtPnt[1].y, 0.0f);
	glVertex3f(m_ExtPnt[2].x, m_ExtPnt[2].y, 0.0f);
	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);


	glVertex3f(m_ExtPnt[2].x, m_ExtPnt[2].y, 0.0f);
	glVertex3f(m_ExtPnt[3].x, m_ExtPnt[3].y, 0.0f);
	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);


	glVertex3f(m_ExtPnt[3].x, m_ExtPnt[3].y, 0.0f);
	glVertex3f(m_ExtPnt[0].x, m_ExtPnt[0].y, 0.0f);
	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);
	glEnd();




 	glColor3f(1,1,0);
 	glPointSize(4);
 	glBegin(GL_POINTS);
 //	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);
 	glVertex3f(m_vLookat.x, m_vLookat.y, 0.0f);
 	glEnd();
 	glPointSize(1);

// 	glBegin(GL_LINES);
// 	glVertex3f(m_vEye.x, m_vEye.y, 0.0f);
// 	glVertex3f(m_vLookat.x, m_vLookat.y, 0.0f);
// 	glEnd();


}


void CCamera2Axis::SetLevelHeight(float fHeight_D)
{
	m_levelHeight += fHeight_D;
	if(m_levelHeight<5)
		m_levelHeight = 5;
	if(m_levelHeight>15000000)
		m_levelHeight = 15000000;
	
	if((fHeight_D>0))
		SetModelViewMatrix(m_vLookat, 1.0,0);
	else
		SetModelViewMatrix(m_vLookat, 0,0);

	SetProjectionMatrix(m_Cntfovy, 0, 0, m_nWidthDisp, m_nHeightDisp);	
}

void CCamera2Axis::SetInitLevelHeight(float fHeight_D)
{
	m_levelHeight = fHeight_D;
	//if(m_levelHeight<2)
	//	m_levelHeight = 2;

	//if((fHeight_D>0))
	//	SetModelViewMatrix(m_vLookat, 1.0,0);
	//else
		SetModelViewMatrix(m_vLookat, 0,0);

	SetProjectionMatrix(m_Cntfovy, 0, 0, m_nWidthDisp, m_nHeightDisp);	
}

void CCamera2Axis::SetModelViewMatrixNoLimit(POINT3D ptPos, GLfloat fAngle_DX, GLfloat fAngle_DZ)
{
//	TRACE("LOOK AT %3.2f, %3.2f, %3.2f\n", ptPos.x, ptPos.y, ptPos.z);
//	bool IsMoveLevelHeight = true;
	m_vLookat=ptPos;
	m_fAngleX+=fAngle_DX;
	m_fAngleZ+=fAngle_DZ;

	if(m_fAngleX<0)		m_fAngleX = m_fAngleX+360;
	if(m_fAngleZ<0)		m_fAngleZ = m_fAngleZ+360;

	if((m_fAngleX>180)&&(m_fAngleX<275)){
		m_fAngleX = 275;
	//	IsMoveLevelHeight = false;
	}
	if((m_fAngleX<180)&&(m_fAngleX>0)){
		m_fAngleX = 0;
	//	IsMoveLevelHeight = false;
	}
		
	if(m_fAngleZ>360)		m_fAngleZ -= 360;
	if(m_fAngleX>360)		m_fAngleX -= 360;


	if(m_levelHeight>MAX_TER_HEIGHT){
		m_fAngleX = 0.0f;
		m_fAngleZ = 0.0f;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f,0.0f,-m_levelHeight);
	glRotatef(m_fAngleX,1.0f, 0.0f, 0.0f); // Local X √‡ø° ¥Î«— »∏¿¸¿∏∑Œ πŸ≤„æﬂ«‘.
	glRotatef(m_fAngleZ, 0.0f, 0.0, 1.0f);
	glTranslatef(-m_vLookat.x, -m_vLookat.y, -m_vLookat.z);

	glGetDoublev(GL_MODELVIEW_MATRIX,m_matModelView);
	memset(m_matCamera,0,sizeof(double)*16);
	mtMultMatrix(m_matModelView, m_matProjection, m_matCamera);

	m_vEye.x=-(float)(m_matModelView[0]*m_matModelView[12]+m_matModelView[1]*m_matModelView[13]+m_matModelView[2] *m_matModelView[14]);
	m_vEye.y=-(float)(m_matModelView[4]*m_matModelView[12]+m_matModelView[5]*m_matModelView[13]+m_matModelView[6] *m_matModelView[14]);
	m_vEye.z=-(float)(m_matModelView[8]*m_matModelView[12]+m_matModelView[9]*m_matModelView[13]+m_matModelView[10]*m_matModelView[14]);

	// Apply Opengl ModelView Matrix //
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(m_matModelView);


	mtSetPoint3D(&m_BaseExtPnt[0], -m_levelHeight*2+m_vLookat.x, m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[1], m_levelHeight*2+m_vLookat.x, m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[2], m_levelHeight*2+m_vLookat.x, -m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[3], -m_levelHeight*2+m_vLookat.x, -m_levelHeight*2+m_vLookat.y, 0);

	UpdateExtent();
///	m_StrCamInfo.Format(L"Camera: Logitude: %3.2f degree, Latitude :%3.2f degree, Altitude: %3.2f m", m_vEye.x, m_vEye.y, m_vEye.z );
	m_StrCamInfo.Format(L"Look At: %3.2f, %3.2f, %3.2f", m_vLookat.x, m_vLookat.y, m_vLookat.z);

}



void CCamera2Axis::SetModelViewMatrix(POINT3D ptPos, GLfloat fAngle_DX, GLfloat fAngle_DZ)
{
	//	TRACE("LOOK AT %3.2f, %3.2f, %3.2f\n", ptPos.x, ptPos.y, ptPos.z);
	//	bool IsMoveLevelHeight = true;

// 	if(m_pDemProj!=NULL){		
// 		m_pDemProj->ProjectPoint(&ptPos, &m_currentBlockid);
// 	}


	m_vLookat=ptPos;
	m_fAngleX+=fAngle_DX;
	m_fAngleZ+=fAngle_DZ;

	if(m_fAngleX<0)		m_fAngleX = m_fAngleX+360;
	if(m_fAngleZ<0)		m_fAngleZ = m_fAngleZ+360;

	if((m_fAngleX>180)&&(m_fAngleX<MIN_VERTICAL_ANGLE)){
		m_fAngleX = MIN_VERTICAL_ANGLE;
		//	IsMoveLevelHeight = false;
	}
	if((m_fAngleX<180)&&(m_fAngleX>0)){
		m_fAngleX = 0;
		//	IsMoveLevelHeight = false;
	}

	if(m_fAngleZ>360)		m_fAngleZ -= 360;
	if(m_fAngleX>360)		m_fAngleX -= 360;


	if(m_levelHeight>MAX_TER_HEIGHT){
		m_fAngleX = 0.0f;
		m_fAngleZ = 0.0f;
		m_TerrainMode = false;
	}
	else{
		m_TerrainMode = true;
	}

	// 
//	m_fAngleX = 

//	float matMv[16];	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
// 	glRotatef(m_fAngleX,1.0f, 0.0f, 0.0f); // Local X √‡ø° ¥Î«— »∏¿¸¿∏∑Œ πŸ≤„æﬂ«‘.
// 	glGetFloatv(GL_MODELVIEW_MATRIX, matMv);
// 
// 	glLoadIdentity();
// 	float distForward = m_levelHeight*0.17f;
// //	glRotatef(-10,1.0f, 0.0f, 0.0f);
// 	glTranslatef(-matMv[4]*distForward, -matMv[5]*distForward, -matMv[6]*distForward);

	glTranslatef(0.0f,0.0f,-m_levelHeight);	
	glRotatef(m_fAngleX,1.0f, 0.0f, 0.0f); 
	glRotatef(m_fAngleZ, 0.0f, 0.0, 1.0f);
	glTranslatef(-m_vLookat.x, -m_vLookat.y, -m_vLookat.z);


	
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();
// 
// 	glTranslatef(0.0f,0.0f,-m_levelHeight);
// 	glRotatef(m_fAngleX,1.0f, 0.0f, 0.0f); // Local X √‡ø° ¥Î«— »∏¿¸¿∏∑Œ πŸ≤„æﬂ«‘.
// 	glRotatef(m_fAngleZ, 0.0f, 0.0, 1.0f);
// 	glTranslatef(-m_vLookat.x, -m_vLookat.y, -m_vLookat.z);









	glGetDoublev(GL_MODELVIEW_MATRIX,m_matModelView);
	memset(m_matCamera,0,sizeof(double)*16);
	mtMultMatrix(m_matModelView, m_matProjection, m_matCamera);

	m_vEye.x=-(float)(m_matModelView[0]*m_matModelView[12]+m_matModelView[1]*m_matModelView[13]+m_matModelView[2] *m_matModelView[14]);
	m_vEye.y=-(float)(m_matModelView[4]*m_matModelView[12]+m_matModelView[5]*m_matModelView[13]+m_matModelView[6] *m_matModelView[14]);
	m_vEye.z=-(float)(m_matModelView[8]*m_matModelView[12]+m_matModelView[9]*m_matModelView[13]+m_matModelView[10]*m_matModelView[14]);

	m_vEyeUp.x = m_vEye.x + (GLfloat)m_matModelView[1] * m_levelHeight;
	m_vEyeUp.y = m_vEye.y + (GLfloat)m_matModelView[5] * m_levelHeight;
	m_vEyeUp.z = m_vEye.z + (GLfloat)m_matModelView[9] * m_levelHeight;

// 	float zDiff = 0;
// 	if(CheckEyeUnderTerrain(m_vEye, &zDiff)){
// 		// Apply Opengl ModelView Matrix //
// 		glMatrixMode(GL_PROJECTION);
// 		glLoadIdentity();
// 		gluPerspective(m_Cntfovy ,m_aspect, m_nearPlane,m_farPlane);	
// 		glGetDoublev(GL_PROJECTION_MATRIX, m_matProjection);	
// 
// 
// 		glMatrixMode(GL_MODELVIEW);
// 		glLoadIdentity();
// 		gluLookAt(m_vEye.x, m_vEye.y, m_vEye.z + zDiff, 
// 			m_vLookat.x, m_vLookat.y, m_vLookat.z + zDiff,
// 			m_matModelView[1], m_matModelView[5], m_matModelView[11]);
// 
// 		glGetDoublev(GL_MODELVIEW_MATRIX,m_matModelView);
// 		memset(m_matCamera,0,sizeof(double)*16);
// 		mtMultMatrix(m_matModelView, m_matProjection, m_matCamera);
// 	}
//	glMultMatrixd(m_matModelView);


	mtSetPoint3D(&m_BaseExtPnt[0], -m_levelHeight*2+m_vLookat.x, m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[1], m_levelHeight*2+m_vLookat.x, m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[2], m_levelHeight*2+m_vLookat.x, -m_levelHeight*2+m_vLookat.y, 0);
	mtSetPoint3D(&m_BaseExtPnt[3], -m_levelHeight*2+m_vLookat.x, -m_levelHeight*2+m_vLookat.y, 0);

	UpdateExtent();
	//m_StrCamInfo.Format(L"Camera: Longi:%3.2f deg, Lati:%3.2f deg, Altitude:%3.2f m\n", m_vEye.x/360000.0f, m_vEye.y/360000.0f, m_vEye.z*0.3334f );
	//CString tmpStr;
	//tmpStr.Format(L"Look At: %3.2f, %3.2f, %3.2f, Level-Height:%3.2f", m_vLookat.x/360000.0f, m_vLookat.y/360000.0f, m_vLookat.z/360000.0f, m_levelHeight);
	//m_StrCamInfo+=tmpStr;
	m_StrCamInfo.Format(L"Look At: %3.2f, %3.2f, %3.2f", m_vLookat.x, m_vLookat.y, m_vLookat.z);
}


void CCamera2Axis::DrawBaseExtRect()
{
	glBegin(GL_QUADS);
	glVertex3f(m_BaseExtPnt[3].x, m_BaseExtPnt[3].y,m_BaseExtPnt[3].z);
	glVertex3f(m_BaseExtPnt[2].x, m_BaseExtPnt[2].y,m_BaseExtPnt[2].z);
	glVertex3f(m_BaseExtPnt[1].x, m_BaseExtPnt[1].y,m_BaseExtPnt[1].z);
	glVertex3f(m_BaseExtPnt[0].x, m_BaseExtPnt[0].y,m_BaseExtPnt[0].z);	
	glEnd();
}


FRUSTUM CCamera2Axis::_ComputeFrustumPerspective(float ratio)
{
	FRUSTUM Frustum;
	double* pMatrix = m_matModelView;

	double a = 1.0f/m_aspect;
	double e = a/tan(m_Cntfovy_r2);

	double n = m_nearPlane;
	double f = m_farPlane;

	double sqrt_e_1=mtSqrt(e*e+1);
	double sqrt_e_a=mtSqrt(e*e+a*a);
	double x,y,z,w;

	// Left clipping plane ===============================///
	x = e/sqrt_e_1; 
	y = 0; 
	z = -1/sqrt_e_1; 
	w = 0;
	Frustum.leftPlane.x = (GLfloat)(x*pMatrix[0] + y*pMatrix[1] + z*pMatrix[2] + w*pMatrix[3]);
	Frustum.leftPlane.y = (GLfloat)(x*pMatrix[4] + y*pMatrix[5] + z*pMatrix[6] + w*pMatrix[7]);
	Frustum.leftPlane.z = (GLfloat)(x*pMatrix[8] + y*pMatrix[9] + z*pMatrix[10] + w*pMatrix[11]);
	Frustum.leftPlane.dist = (GLfloat)(x*pMatrix[12] + y*pMatrix[13] + z*pMatrix[14] + w*pMatrix[15]);

	// Right clipping plane =============================//
	x = -e/sqrt_e_1; 
	y = 0; 
	z = -1/sqrt_e_1; 
	w = 0;
	Frustum.rightPlane.x = (float)(x*pMatrix[0]+y*pMatrix[1]+z*pMatrix[2]+w*pMatrix[3]); 
	Frustum.rightPlane.y = (float)(x*pMatrix[4]+y*pMatrix[5]+z*pMatrix[6]+w*pMatrix[7]);
	Frustum.rightPlane.z = (float)(x*pMatrix[8]+y*pMatrix[9]+z*pMatrix[10]+w*pMatrix[11]);
	Frustum.rightPlane.dist = (float)(x*pMatrix[12]+y*pMatrix[13]+z*pMatrix[14]+w*pMatrix[15]);

	// Top clipping plane ============================//
	x = 0; 
	y = e/sqrt_e_a; 
	z = -a/sqrt_e_a; 
	w = 0;
	Frustum.topPlane.x = (float)(x*pMatrix[0]+y*pMatrix[1]+z*pMatrix[2]+w*pMatrix[3]);
	Frustum.topPlane.y = (float)(x*pMatrix[4]+y*pMatrix[5]+z*pMatrix[6]+w*pMatrix[7]);
	Frustum.topPlane.z = (float)(x*pMatrix[8]+y*pMatrix[9]+z*pMatrix[10]+w*pMatrix[11]);
	Frustum.topPlane.dist = (float)(x*pMatrix[12]+y*pMatrix[13]+z*pMatrix[14]+w*pMatrix[15]);

	// Bottom clipping plane ===========================//
	x = 0; 
	y = -e/sqrt_e_a; 
	z = -a/sqrt_e_a; 
	w = 0;
	Frustum.bottomPlane.x = (float)(x*pMatrix[0]+y*pMatrix[1]+z*pMatrix[2]+w*pMatrix[3]);
	Frustum.bottomPlane.y = (float)(x*pMatrix[4]+y*pMatrix[5]+z*pMatrix[6]+w*pMatrix[7]);
	Frustum.bottomPlane.z = (float)(x*pMatrix[8]+y*pMatrix[9]+z*pMatrix[10]+w*pMatrix[11]);
	Frustum.bottomPlane.dist = (float)(x*pMatrix[12]+y*pMatrix[13]+z*pMatrix[14]+w*pMatrix[15]);

	// Near clipping plane ==========================//
	x = 0; 
	y = 0; 
	z = -1; 
	w = -n;
	Frustum.nearPlane.x = (float)(x*pMatrix[0]+y*pMatrix[1]+z*pMatrix[2]+w*pMatrix[3]);
	Frustum.nearPlane.y = (float)(x*pMatrix[4]+y*pMatrix[5]+z*pMatrix[6]+w*pMatrix[7]);
	Frustum.nearPlane.z = (float)(x*pMatrix[8]+y*pMatrix[9]+z*pMatrix[10]+w*pMatrix[11]);
	Frustum.nearPlane.dist = (float)(x*pMatrix[12]+y*pMatrix[13]+z*pMatrix[14]+w*pMatrix[15]);

	// Far clipping plane ===========================//
	x = 0; 
	y = 0; 
	z = 1; 
	w = f; 
	Frustum.farPlane.x = (float)(x*pMatrix[0]+y*pMatrix[1]+z*pMatrix[2]+w*pMatrix[3]);
	Frustum.farPlane.y = (float)(x*pMatrix[4]+y*pMatrix[5]+z*pMatrix[6]+w*pMatrix[7]);
	Frustum.farPlane.z = (float)(x*pMatrix[8]+y*pMatrix[9]+z*pMatrix[10]+w*pMatrix[11]);
	Frustum.farPlane.dist = (float)(x*pMatrix[12]+y*pMatrix[13]+z*pMatrix[14]+w*pMatrix[15]);

	return Frustum;
}

float CCamera2Axis::_ComputeOffsetVAngle()
{
	float angleX=m_fAngleX;
	float maxOffsetAngle = m_Cntfovy*0.5f - MIN_VERTICAL_ANGLE*0.5f;
	float fOffsetAngle;

	if(angleX<MIN_VERTICAL_ANGLE)
		fOffsetAngle=maxOffsetAngle;
	else if(angleX>=m_fAngleX)
		fOffsetAngle=0;
	else
		fOffsetAngle=(0-maxOffsetAngle)/(m_fAngleX-MIN_VERTICAL_ANGLE)*(angleX-MIN_VERTICAL_ANGLE)+maxOffsetAngle;

	return fOffsetAngle;
}

BOOL CCamera2Axis::_ComputeProjectPoint(const float& x, const float& y, const float& z, POINT2D *sc)
{
	

// 	GLdouble matModel[16];
// 	GLdouble matProj[16];
// 	GLint Viewport[16];
// 	GLdouble winx, winy, winz;
// 	glGetDoublev(GL_MODELVIEW_MATRIX,matModel);
// 	glGetDoublev(GL_PROJECTION_MATRIX,matProj);
// 	glGetIntegerv(GL_VIEWPORT, Viewport); 
// 
// 
// 	m_matModelView;
// 	m_matProjection;
// 
// 	gluProject((double)x, (double)y, (double)z,matModel, matProj, Viewport, &winx, &winy, &winz);
// 	sc->x = (int)winx;
// 	sc->y = (int)winy;
// 
// 	TRACE("%3.2f, %3.2f, %3.2f  - %3.2f, %3.2f\n", x,y,z , winx, winy);

		
	double in[4]={0,};
	double out[4]={0,};

	in[0]=x;
	in[1]=y;
	in[2]=z;
	in[3]=1.0f;

	mtMultMatrixVecf(m_matCamera,in,out);
	if (out[3] == 0.0f) 
		return FALSE;

	out[0] /= out[3];
	out[1] /= out[3];
	out[2] /= out[3];

	// World¡¬«•∏¶ 0-1 range ∑Œ ∫Ø»Ø
	out[0] = out[0] * 0.5f + 0.5f;
	out[1] = out[1] * 0.5f + 0.5f;
	out[2] = out[2] * 0.5f + 0.5f;

	// xy∏¶ ∫‰∆˜∆Æ∑Œ ∫Ø»Ø
	out[0] = out[0] * m_viewport[2] + m_viewport[0];
	out[1] = out[1] * m_viewport[3] + (m_nHeightDisp-(m_viewport[1]+m_viewport[3]));

	(*sc).x = (float)out[0];
	(*sc).y = (float)out[1];
//	(*sc).y = (float)(m_nHeightDisp-out[1]);

//	TRACE("%3.2f, %3.2f, %3.2f  - %3.2f, %3.2f\n", x,y,z , (*sc).x, (*sc).y);

	return TRUE;
}


BOOL CCamera2Axis::_ComputeUnProjectPoint2(float winx, float winy, float winz, float *unProjPnt)
{



	double fovy=m_Cntfovy_r2;

	double* modelView	= m_matModelView;
	double* Inverse		= matTemp2;

	memset(Inverse,0,sizeof(double)*16);

	// Image Plane ÔøΩÔøΩ«• ÔøΩÔøΩÔø?
	// screen ÔøΩÔøΩ«• m_TouchPosX,m_TouchPosY
	int width=m_viewport[2];
	int height=m_viewport[3];
	double fl=height/2/tan(fovy);
	double dx=(winx-width/2-m_viewport[0])/fl;
	double dy=(winy-height/2-m_viewport[1])/fl;

	// ÔøΩÔøΩÔøΩÔøΩÔø?ÔøΩÔøΩÔø?: ÔøΩÔøΩ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»ØÔøΩÔøΩÔø?ÔøΩÔøΩÔøΩÔøΩ»Ø

	Inverse[0]=modelView[0];
	Inverse[1]=modelView[4];
	Inverse[2]=modelView[8];

	Inverse[4]=modelView[1];
	Inverse[5]=modelView[5];
	Inverse[6]=modelView[9];

	Inverse[8]=modelView[2];
	Inverse[9]=modelView[6];
	Inverse[10]=modelView[10];

	Inverse[12]=-(Inverse[0]*modelView[12]+Inverse[4]*modelView[13]+Inverse[8] *modelView[14]);
	Inverse[13]=-(Inverse[1]*modelView[12]+Inverse[5]*modelView[13]+Inverse[9] *modelView[14]);
	Inverse[14]=-(Inverse[2]*modelView[12]+Inverse[6]*modelView[13]+Inverse[10]*modelView[14]);

	Inverse[15]=1.0;

	// ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?
	double new_x,new_y,new_z,new_w;
	double new_eye_x,new_eye_y,new_eye_z;

	new_w= dx*Inverse[3]+dy*Inverse[7]+1.0*Inverse[11]+1.0*Inverse[15];
	//	ASSERT(new_w);
	new_x=(dx*Inverse[0]+dy*Inverse[4]+1.0*Inverse[8] +1.0*Inverse[12])/new_w;
	new_y=(dx*Inverse[1]+dy*Inverse[5]+1.0*Inverse[9] +1.0*Inverse[13])/new_w;
	new_z=(dx*Inverse[2]+dy*Inverse[6]+1.0*Inverse[10]+1.0*Inverse[14])/new_w;

	// ƒ´ÔøΩﬁ∂ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?eye=(0,0,0,1);
	new_eye_x=1.0*Inverse[12];
	new_eye_y=1.0*Inverse[13];
	new_eye_z=1.0*Inverse[14];

	// ÔøΩÔøΩÔøΩÔøΩÔøΩ«•ÔøΩËø°ÔøΩÔø?ƒ´ÔøΩﬁ∂Ûø°ºÔøΩ ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	double dir_x,dir_y,dir_z;

	dir_x=new_x-(new_eye_x);
	dir_y=new_y-(new_eye_y);
	dir_z=new_z-(new_eye_z);

	double norm2 = dir_x*dir_x+dir_y*dir_y+dir_z*dir_z;
	//	if(!norm2)
	//		DEBUG_PRINT(L"=== [_ComputeUnProjectPoint] : norm2 == 0");

	double norm=sqrt(norm2);
	//	ASSERT(norm);
	dir_x/=norm;
	dir_y/=norm;
	dir_z/=norm;

	//	ASSERT(dir_y);
	if(dir_y==0)
	return false;
	// x-z plane ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔø?
	double t=(winz-1.0*new_eye_z)/(dir_z);

	//	double t=(m_vLookat.y-1.0*new_eye_y)/(dir_y);
	//if(t>0)
	//	return FALSE;

	unProjPnt[0]=static_cast<float>((new_eye_x)+dir_x*t);
	unProjPnt[1]=static_cast<float>((new_eye_y)+dir_y*t);
	unProjPnt[2]=static_cast<float>((new_eye_z)+dir_z*t);


	return true;

}


BOOL CCamera2Axis::_ComputeUnProjectPoint(float winx, float winy, float winz, float *unProjPnt)
{

	double m[16];
	double in[4], out[4];
	//Calculation for inverting a matrix, compute projection x modelview
	//Now compute the inverse of matrix A
	if(InvertMatrixf2(m_matCamera, m)==0)
		return FALSE;

	//Transformation of normalized coordinates between -1 and 1
	in[0]=(winx-(float)m_viewport[0])/(float)m_viewport[2]*2.0-1.0;
	in[1]=(winy-(float)m_viewport[1])/(float)m_viewport[3]*2.0-1.0;
	in[2]=2.0*winz-1.0;
	in[3]=1.0;
	//Objects coordinates
	MultiplyMatrixByVector4by4(out, m, in);
	if(out[3]==0.0)
		return 0;
	out[3]=1.0/out[3];
	unProjPnt[0]= (float)(out[0]*out[3]);
	unProjPnt[1]= (float)(out[1]*out[3]);
	unProjPnt[2]= (float)(out[2]*out[3]);

	/*

	double fovy=m_Cntfovy_r2;

	double* modelView	= m_matModelView;
	double* Inverse		= matTemp2;

	memset(Inverse,0,sizeof(double)*16);

	// Image Plane ÔøΩÔøΩ«• ÔøΩÔøΩÔø?
	// screen ÔøΩÔøΩ«• m_TouchPosX,m_TouchPosY
	int width=m_viewport[2];
	int height=m_viewport[3];
	double fl=height/2/tan(fovy);
	double dx=(winx-width/2-m_viewport[0])/fl;
	double dy=(winy-height/2-m_viewport[1])/fl;

	// ÔøΩÔøΩÔøΩÔøΩÔø?ÔøΩÔøΩÔø?: ÔøΩÔøΩ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩ»ØÔøΩÔøΩÔø?ÔøΩÔøΩÔøΩÔøΩ»Ø

	Inverse[0]=modelView[0];
	Inverse[1]=modelView[4];
	Inverse[2]=modelView[8];

	Inverse[4]=modelView[1];
	Inverse[5]=modelView[5];
	Inverse[6]=modelView[9];

	Inverse[8]=modelView[2];
	Inverse[9]=modelView[6];
	Inverse[10]=modelView[10];

	Inverse[12]=-(Inverse[0]*modelView[12]+Inverse[4]*modelView[13]+Inverse[8] *modelView[14]);
	Inverse[13]=-(Inverse[1]*modelView[12]+Inverse[5]*modelView[13]+Inverse[9] *modelView[14]);
	Inverse[14]=-(Inverse[2]*modelView[12]+Inverse[6]*modelView[13]+Inverse[10]*modelView[14]);

	Inverse[15]=1.0;

	// ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?
	double new_x,new_y,new_z,new_w;
	double new_eye_x,new_eye_y,new_eye_z;

	new_w= dx*Inverse[3]+dy*Inverse[7]+1.0*Inverse[11]+1.0*Inverse[15];
	//	ASSERT(new_w);
	new_x=(dx*Inverse[0]+dy*Inverse[4]+1.0*Inverse[8] +1.0*Inverse[12])/new_w;
	new_y=(dx*Inverse[1]+dy*Inverse[5]+1.0*Inverse[9] +1.0*Inverse[13])/new_w;
	new_z=(dx*Inverse[2]+dy*Inverse[6]+1.0*Inverse[10]+1.0*Inverse[14])/new_w;

	// ƒ´ÔøΩﬁ∂ÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ?eye=(0,0,0,1);
	new_eye_x=1.0*Inverse[12];
	new_eye_y=1.0*Inverse[13];
	new_eye_z=1.0*Inverse[14];

	// ÔøΩÔøΩÔøΩÔøΩÔøΩ«•ÔøΩËø°ÔøΩÔø?ƒ´ÔøΩﬁ∂Ûø°ºÔøΩ ≈¨ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	double dir_x,dir_y,dir_z;

	dir_x=new_x-(new_eye_x);
	dir_y=new_y-(new_eye_y);
	dir_z=new_z-(new_eye_z);

	double norm2 = dir_x*dir_x+dir_y*dir_y+dir_z*dir_z;
	//	if(!norm2)
	//		DEBUG_PRINT(L"=== [_ComputeUnProjectPoint] : norm2 == 0");

	double norm=sqrt(norm2);
	//	ASSERT(norm);
	dir_x/=norm;
	dir_y/=norm;
	dir_z/=norm;

	//	ASSERT(dir_y);
	if(dir_y==0)
		return false;
	// x-z plane ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔø?
	double t=(winz-1.0*new_eye_z)/(dir_z);

	//	double t=(m_vLookat.y-1.0*new_eye_y)/(dir_y);
	//if(t>0)
	//	return FALSE;

	unProjPnt[0]=static_cast<float>((new_eye_x)+dir_x*t);
	unProjPnt[1]=static_cast<float>((new_eye_y)+dir_y*t);
	unProjPnt[2]=static_cast<float>((new_eye_z)+dir_z*t);

	*/
	return true;

}

POINT3D CCamera2Axis::ScreenToWorld(int x, int y)
{
	//POINT3D hPnt;
	//GLfloat winX, winY, winZ=0;

	//winX = (float)x;
	//winY = (float)(m_viewport[3]-y);
	//glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	//float fResult[3];
	//_ComputeUnProjectPoint(winX, winY, winZ, fResult);
	//mtSetPoint3D(&hPnt, fResult[0], fResult[1], fResult[2]);

	POINT3D hPnt;
	POINT3D pO, pN;
	mtSetPoint3D(&pO, 0,0,0);
	mtSetPoint3D(&pN, 0,0,1);

	InsetsectRayToPlane(pN, pO, x, y, hPnt);
	return hPnt;
}


GLfloat CCamera2Axis::GetFocalLength()
{
	double fovy = m_Cntfovy_r2;
	double fl = m_nHeightDisp/2/tan(fovy);
	return (float)fl;
}

FRUSTUM CCamera2Axis::GetFrustum(float ratio/* =1.0f */)
{
	return _ComputeFrustumPerspective(ratio);
}













// Math For Camera //
GLint CCamera2Axis::InvertMatrixf2(double *m, double *out)
{
	double wtmp[4][8];
	double m0, m1, m2, m3, s;
	double *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
		r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
		r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
		r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
		r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
		r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
		r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
		r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
		r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
		r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
		r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
		r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (fabsf((float)r3[0]) > fabsf((float)r2[0]))
		SWAP_ROWS_DOUBLE(r3, r2);
	if (fabsf((float)r2[0]) > fabsf((float)r1[0]))
		SWAP_ROWS_DOUBLE(r2, r1);
	if (fabsf((float)r1[0]) > fabsf((float)r0[0]))
		SWAP_ROWS_DOUBLE(r1, r0);
	if (0.0 == r0[0])
		return 0;

	/* eliminate first variable     */
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];
	s = r0[1];
	r1[1] -= m1 * s;
	r2[1] -= m2 * s;
	r3[1] -= m3 * s;
	s = r0[2];
	r1[2] -= m1 * s;
	r2[2] -= m2 * s;
	r3[2] -= m3 * s;
	s = r0[3];
	r1[3] -= m1 * s;
	r2[3] -= m2 * s;
	r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) {
		r1[4] -= m1 * s;
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r0[5];
	if (s != 0.0) {
		r1[5] -= m1 * s;
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r0[6];
	if (s != 0.0) {
		r1[6] -= m1 * s;
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r0[7];
	if (s != 0.0) {
		r1[7] -= m1 * s;
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (fabsf((float)r3[1]) > fabsf((float)r2[1]))
		SWAP_ROWS_DOUBLE(r3, r2);
	if (fabsf((float)r2[1]) > fabsf((float)r1[1]))
		SWAP_ROWS_DOUBLE(r2, r1);
	if (0.0 == r1[1])
		return 0;

	/* eliminate second variable */
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];
	s = r1[4];
	if (0.0 != s) {
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r1[5];
	if (0.0 != s) {
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r1[6];
	if (0.0 != s) {
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r1[7];
	if (0.0 != s) {
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (fabsf((float)r3[2]) > fabsf((float)r2[2]))
		SWAP_ROWS_DOUBLE(r3, r2);
	if (0.0 == r2[2])
		return 0;

	/* eliminate third variable */
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3])
		return 0;

	s = 1.0 / r3[3];		/* now back substitute row 3 */
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;

	m2 = r2[3];			/* now back substitute row 2 */
	s = 1.0 / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];			/* now back substitute row 1 */
	s = 1.0 / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];			/* now back substitute row 0 */
	s = 1.0 / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out, 0, 0) = r0[4];
	MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
	MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
	MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
	MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
	MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
	MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
	MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
	MAT(out, 3, 3) = r3[7];

	return 1;
}

void  CCamera2Axis::MultiplyMatrixByVector4by4(double *resultvector, const double *matrix, const double *pvector)
{
	resultvector[0]=matrix[0]*pvector[0]+matrix[4]*pvector[1]+matrix[8]*pvector[2]+matrix[12]*pvector[3];
	resultvector[1]=matrix[1]*pvector[0]+matrix[5]*pvector[1]+matrix[9]*pvector[2]+matrix[13]*pvector[3];
	resultvector[2]=matrix[2]*pvector[0]+matrix[6]*pvector[1]+matrix[10]*pvector[2]+matrix[14]*pvector[3];
	resultvector[3]=matrix[3]*pvector[0]+matrix[7]*pvector[1]+matrix[11]*pvector[2]+matrix[15]*pvector[3];
}


void CCamera2Axis::gl_PushOrtho2D(int x1, int x2, int y1, int y2)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(x1,x2,y1,y2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void CCamera2Axis::gl_PopOrtho2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CCamera2Axis::gl_PushOrtho3D(float z1, float z2)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,m_nWidthDisp,0,m_nHeightDisp, z1, z2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void CCamera2Axis::gl_PopOrtho3D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



BOOL CCamera2Axis::InsetsectRayToPlane(POINT3D pN, POINT3D pO, int x, int y, POINT3D&hitPos)
{
	POINT3D v, rayOrigin, rayDir;
	
	double invM[16];
	memset(invM,0,sizeof(double)*16);
	invM[0]=m_matModelView[0];
	invM[1]=m_matModelView[4];
	invM[2]=m_matModelView[8];

	invM[4]=m_matModelView[1];
	invM[5]=m_matModelView[5];
	invM[6]=m_matModelView[9];

	invM[8]=m_matModelView[2];
	invM[9]=m_matModelView[6];
	invM[10]=m_matModelView[10];

	invM[12]=-(invM[0]*m_matModelView[12]+invM[4]*m_matModelView[13]+invM[8] *m_matModelView[14]);
	invM[13]=-(invM[1]*m_matModelView[12]+invM[5]*m_matModelView[13]+invM[9] *m_matModelView[14]);
	invM[14]=-(invM[2]*m_matModelView[12]+invM[6]*m_matModelView[13]+invM[10]*m_matModelView[14]);
	invM[15]=1.0;


	if((x>=0)&&(y>=0)){
		v.x = (((2.0f*x)/(float)m_nWidthDisp)-1) / (float)m_matProjection[0];
		v.y = -(((2.0f*y)/(float)m_nHeightDisp)-1) / (float)m_matProjection[5];
		v.z = -1.0f;

		rayDir.x = (float)(v.x*invM[0] +  v.y*invM[4] + v.z*invM[8]);
		rayDir.y = (float)(v.x*invM[1] +  v.y*invM[5] + v.z*invM[9]);
		rayDir.z = (float)(v.x*invM[2] +  v.y*invM[6] + v.z*invM[10]);
		rayDir = mtNormalize(rayDir);
		
		rayOrigin.x = (float)invM[12];
		rayOrigin.y = (float)invM[13];
		rayOrigin.z = (float)invM[14];

		float distFormO = -(pO.x*pN.x + pO.y*pN.y + pO.z*pN.z);
		float upper = mtDot(pN, rayOrigin)+distFormO;
		float down = mtDot(pN, rayDir);

		if(down==0){
			hitPos.x = rayOrigin.x + rayDir.x*m_farPlane;
			hitPos.y = rayOrigin.y + rayDir.y*m_farPlane;
			hitPos.z = rayOrigin.z + rayDir.z*m_farPlane;
			return TRUE;
		}

		float interTime = -upper/down;
		if(interTime>m_farPlane)
			interTime = m_farPlane;

		if(interTime>0){
			hitPos.x = rayOrigin.x + rayDir.x*interTime;
			hitPos.y = rayOrigin.y + rayDir.y*interTime;
			hitPos.z = rayOrigin.z + rayDir.z*interTime;
			return TRUE;
		}
		else{
			hitPos.x = rayOrigin.x + rayDir.x*m_farPlane;
			hitPos.y = rayOrigin.y + rayDir.y*m_farPlane;
			hitPos.z = rayOrigin.z + rayDir.z*m_farPlane;
			return TRUE;
		}
	}
	return TRUE;	
}

int CCamera2Axis::GetDemLevel()
{
	int level=-1;
 	float levelHeight = GetLevelHeight();

 	if((0<=levelHeight)&&(levelHeight<10000))
 		level = 4;
  	else if((10000<=levelHeight)&&(levelHeight<40000))
  		level = 3;
  	else if((40000<=levelHeight)&&(levelHeight<80000))
  		level = 2;
 	else if((80000<=levelHeight)&&(levelHeight<200000))
  		level = 1;
 	else if((200000<=levelHeight)&&(levelHeight<1200000))
 		level = 0;
 	else
 		level= -1;

	return level;
}