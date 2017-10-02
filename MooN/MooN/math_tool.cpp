#include "StdAfx.h"
#include "math_tool.h"

POINT3D mtCross(POINT3D A, POINT3D B)
{
	POINT3D C;
	double x,y,z;

	x=((double)A.y)*B.z-((double)A.z)*B.y;
	y=((double)A.z)*B.x-((double)A.x)*B.z;
	z=((double)A.x)*B.y-((double)A.y)*B.x;

	mtSetPoint3D(&C,(float)x,(float)y,(float)z);
	return C;
}

POINT3D mtLineCross(POINT3D A, POINT3D B)
{
	POINT3D C;
	double x,y,z;

	x=((double)A.y)*B.z-((double)A.z)*B.y;
	y=((double)A.z)*B.x-((double)A.x)*B.z;
	z=((double)A.x)*B.y-((double)A.y)*B.x;

//	ASSERT(abs(y)>0.001f);
	if(abs(y)>0.001f)
		mtSetPoint3D(&C,(float)(x/y),1.0f,(float)(z/y));
	else
		mtSetPoint3D(&C,-1.0f,-1.0f,-1.0f);

	return C;
}

float mtDot(POINT3D A, POINT3D B)
{
	double dot;

	dot=((double)A.x)*B.x+((double)A.y)*B.y+((double)A.z)*B.z;

	return (float)dot;
}

float mtDot(PLANE3D P, POINT3D V)
{
	double dot;

	dot=((double)P.x)*V.x+((double)P.y)*V.y+((double)P.z)*V.z+((double)P.dist);

	return (float)dot;
}

float mtDotXZ(POINT3D A, POINT3D B)
{
	double dot;

	dot=((double)A.x)*B.x+((double)A.z)*B.z;

	return (float)dot;

}

float mtAngle(POINT3D unit_A, POINT3D unit_B)
{
	float cos_theta = mtDot(unit_A,unit_B);
	if(cos_theta<-1.0f) cos_theta = -1.0f;
	else if(cos_theta>1.0f) cos_theta = 1.0f;

//	return acos(cos_theta)*180.0f/A_PI;
	return acos(cos_theta)*180.0f;

}

POINT3D mtNormalize(POINT3D A)
{
	POINT3D B;

	if(A.x == 0.0f && A.y == 0.0f && A.z == 0.0f)
		return A;
	else

	B=A/mtNorm(A);

	return B;
}

POINT3D mtNormalizeMag(POINT3D A)
{
	if(A.x == 0.0f && A.y == 0.0f && A.z == 0.0f)
		return A;
	else
		return mtNormalize(A);
}

float mtDistance(POINT3D A, POINT3D B)
{
	POINT3D C=A-B;

	float fdist=(float)mtSqrt(((double)C.x)*C.x+((double)C.y)*C.y+((double)C.z)*C.z);
	return fdist;
}



float mtDistanceXY(POINT3D A, POINT3D B)
{
	POINT3D C=A-B;

	float fdist=(float)mtSqrt(((double)C.x)*C.x+((double)C.y)*C.y);
	return fdist;

}

float mtManhattanDistanceXZ(POINT3D A, POINT3D B)
{
	float dist = fabs(A.x-B.x)+fabs(A.z-B.z);
	return dist;
}

float mtNorm(POINT3D A)
{
	return (float)mtSqrt(((double)A.x)*A.x+((double)A.y)*A.y+((double)A.z)*A.z);

}

void mtMultMatrixVecf(const float matrix[16], const float in[4], float out[4])
{
	int i;

	for (i=0; i<4; i++) 
	{
		out[i] = in[0] * matrix[0*4+i] +
			in[1] * matrix[1*4+i] +
			in[2] * matrix[2*4+i] +
			in[3] * matrix[3*4+i];
	}

}

void mtMultMatrixVecf(const double matrix[16], const double in[4], double out[4])
{
	int i;

	for (i=0; i<4; i++) 
	{
		out[i] = in[0] * matrix[0*4+i] +
			in[1] * matrix[1*4+i] +
			in[2] * matrix[2*4+i] +
			in[3] * matrix[3*4+i];
	}

}

void mtMultMatrix(const float matrixA[16], const float matrixB[16], float matrixOut[16])
{
	int i4=0;
	for (int i=0; i<4;i++) 
	{
		i4=i*4;
		for(int j=0;j<4;j++)
		{
			matrixOut[i4+j]=	matrixA[i4+0] * matrixB[0 +j] +
				matrixA[i4+1] * matrixB[4 +j] +
				matrixA[i4+2] * matrixB[8 +j] +
				matrixA[i4+3] * matrixB[12+j];
		}
	}
}

void mtMultMatrix(const double matrixA[16], const double matrixB[16], double matrixOut[16])
{
	int i4=0;

	for (int i=0; i<4;i++) 
	{
		i4=i*4;
		for(int j=0;j<4;j++)
		{
			matrixOut[i4+j]= matrixA[i4+0] * matrixB[0 +j] +
				matrixA[i4+1] * matrixB[4 +j] +
				matrixA[i4+2] * matrixB[8 +j] +
				matrixA[i4+3] * matrixB[12+j] ;
		}
	}
}

POINT2D operator * (const POINT2D A, const float value)
{
	POINT2D B;

	B.x=float(A.x*((double)value));
	B.y=float(A.y*((double)value));

	return B;
}

POINT2D operator / (const POINT2D A, const float value)
{
	POINT2D B;

	B.x=float(A.x/((double)value));
	B.y=float(A.y/((double)value));

	return B;
}

POINT2D operator - (const POINT2D A, const POINT2D B)
{
	POINT2D C;

	C.x=float(((double)A.x)-B.x);
	C.y=float(((double)A.y)-B.y);

	return C;
}

POINT2D mtNormalize(POINT2D A)
{
	POINT2D B;

	if(A.x == 0.0f && A.y == 0.0f)
		return A;
	else
		B=A/mtNorm(A);

	return B;
}

float mtDot(POINT2D A, POINT2D B)
{
	double dot;

	dot=((double)A.x)*B.x+((double)A.y)*B.y;

	return (float)dot;
}

float mtNorm(POINT2D A)
{
	return (float)mtSqrt(((double)A.x)*A.x+((double)A.y)*A.y);
}

POINT3D operator + (const POINT3D A, const POINT3D B)
{
	POINT3D C;

	C.x=float(((double)A.x)+B.x);
	C.y=float(((double)A.y)+B.y);
	C.z=float(((double)A.z)+B.z);

	return C;
}

POINT3D operator - (const POINT3D A, const POINT3D B)
{
	POINT3D C;

	C.x=float(((double)A.x)-B.x);
	C.y=float(((double)A.y)-B.y);
	C.z=float(((double)A.z)-B.z);

	return C;
}



POINT3D operator * (const POINT3D A, const POINT3D B)
{
	POINT3D C;

	C.x=float(((double)A.x)*B.x);
	C.y=float(((double)A.y)*B.y);
	C.z=float(((double)A.z)*B.z);

	return C;
}

POINT3D operator / (const POINT3D A, const POINT3D B)
{
	if(B.x==0 || B.y==0 || B.z==0)
		return A;

	POINT3D C;

	C.x=float(((double)A.x)/B.x);
	C.y=float(((double)A.y)/B.y);
	C.z=float(((double)A.z)/B.z);

	return C;
}

POINT3D operator + (const POINT3D A, const float value)
{
	POINT3D B;
	B.x=float(A.x+((double)value));
	B.y=float(A.y+((double)value));
	B.z=float(A.z+((double)value));

	return B;
}

POINT3D operator - (const POINT3D A, const float value)
{
	POINT3D B;
	B.x=float(A.x-((double)value));
	B.y=float(A.y-((double)value));
	B.z=float(A.z-((double)value));

	return B;
}



POINT3D operator * (const POINT3D A, const float value)
{
	POINT3D B;
	B.x=float(A.x*((double)value));
	B.y=float(A.y*((double)value));
	B.z=float(A.z*((double)value));

	return B;
}

POINT3D operator / (const POINT3D A, const float value)
{
	POINT3D B;
	B.x=float(A.x/((double)value));
	B.y=float(A.y/((double)value));
	B.z=float(A.z/((double)value));

	return B;
}


BOOL mtInterSection(RECT3D rect, POINT3D point1, POINT3D point2)
{
	POINT3D U[3];
	double e[3];
	POINT3D W;
	POINT3D D;
	POINT3D V;
	double Rb,Rs,Rd;

	mtSetPoint3D(&U[0],1,0,0);
	mtSetPoint3D(&U[1],0,1,0);
	mtSetPoint3D(&U[2],0,0,1);

	e[0] = (rect.x2-rect.x1)/2.0f;
	e[1] = (rect.y2-rect.y1)/2.0f;
	e[2] = (rect.z2-rect.z1)/2.0f;

	D.x = ((point2.x+point1.x)-(rect.x2+rect.x1))/2.0f;
	D.y = ((point2.y+point1.y)-(rect.y2+rect.y1))/2.0f;
	D.z = ((point2.z+point1.z)-(rect.z2+rect.z1))/2.0f;

	V.x = point2.x - (point2.x+point1.x)/2.0f;
	V.y = point2.y - (point2.y+point1.y)/2.0f;
	V.z = point2.z - (point2.z+point1.z)/2.0f;

	for(int i=0;i<6;i++)
	{
		switch(i)
		{
		case 0 : 
			W = U[0]; 
			Rb = e[0];
			Rs = mtDot(W,U[0]);
			Rd = mtDot(D,U[0]);
			break;
		case 1 : 
			W = U[1]; 
			Rb = e[1];
			Rs = mtDot(W,U[1]);
			Rd = mtDot(D,U[1]);
			break;
		case 2 :  
			W = U[2]; 
			Rb = e[2];
			Rs = mtDot(W,U[2]);
			Rd = mtDot(D,U[2]);
			break;
		case 3 : 
			W = mtCross(V,U[0]); 
			Rb = (e[1]*mtDot(V,U[2])+e[2]*mtDot(V,U[1]))/mtNorm(V);
			Rs = 0;
			Rd = mtDot(U[0],mtCross(V,D))/mtNorm(V);	
			break;
		case 4 : 
			W = mtCross(V,U[1]); 
			Rb = (e[0]*mtDot(V,U[2])+e[2]*mtDot(V,U[0]))/mtNorm(V);
			Rs = 0;
			Rd = mtDot(U[1],mtCross(V,D))/mtNorm(V);	
			break;
		case 5 : 
			W = mtCross(V,U[2]); 
			Rb = (e[0]*mtDot(V,U[1])+e[1]*mtDot(V,U[0]))/mtNorm(V);
			Rs = 0;
			Rd = mtDot(U[2],mtCross(V,D))/mtNorm(V);	
			break;
		}

		if(Rd > Rb+Rs)
			return FALSE;
	}

	return TRUE;
}

// 3D Game Engine Design p207
// Static Object-Object Intersection : Oriented Boxes And Triangles 
BOOL mtInterSection(RECT3D rect, POINT3D* pEN)
{
	POINT3D E[3],N;
	POINT3D C;
	POINT3D D;
	POINT3D a;
	POINT3D A[3];
	float r = 0;
	float p[3];
	POINT3D L;

	mtSetPoint3D(&A[0],1,0,0);
	mtSetPoint3D(&A[1],0,1,0);
	mtSetPoint3D(&A[2],0,0,1);

	//E[0] = U[1] - U[0];
	//E[1] = U[2] - U[0];
	//E[2] = E[1] - E[0];
	//N = mtCross(E[0],E[1]);
	
	E[0] = pEN[1];
	E[1] = pEN[2];
	E[2] = pEN[3];
	N = pEN[4];

	C.x = (rect.x2+rect.x1)/2.0f;
	C.y = (rect.y2+rect.y1)/2.0f;
	C.z = (rect.z2+rect.z1)/2.0f;

	a.x = (C.x-rect.x1);
	a.y = (C.y-rect.y1);
	a.z = (C.z-rect.z1);

	D = pEN[0] - C; 

	// Axis N Test
	r = a.x*fabs(mtDot(N,A[0]))+a.y*fabs(mtDot(N,A[1]))+a.z*fabs(mtDot(N,A[2]));
	p[0] = mtDot(N,D);

	if(fabs(p[0])> r )
		return FALSE;

	// Axes Ak Test
	for(int i=0;i<3;i++)
	{
		switch(i)
		{
		case 0:
			p[0] = D.x;
			p[1] = p[0] + E[0].x; //mtDot(A[0],E[0]);
			p[2] = p[0] + E[1].x; //mtDot(A[0],E[1]);
			r = a.x;
			break;
		case 1:
			p[0] = D.y;
			p[1] = p[0] + E[0].y; //mtDot(A[1],E[0]);
			p[2] = p[0] + E[1].y; //mtDot(A[1],E[1]);
			r = a.y;
			break;
		case 2:
			p[0] = D.z;
			p[1] = p[0] + E[0].z;; //mtDot(A[2],E[0]);
			p[2] = p[0] + E[1].z;; //mtDot(A[2],E[1]);
			r = a.z;
			break;
		}

		if(p[0] > r)
		{
			if(p[1] > r && p[2] >r)
				return FALSE;
		}
		else if(p[0] <-r)
		{
			if(p[1] < -r && p[2] < -r)
				return FALSE;
		}
	}
	
	// Ai x Ej Test
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			L = mtCross(A[i],E[j]);
			
			switch(i)
			{
			case 0:
				r = a.y*fabs(E[j].z/*mtDot(A[2],E[j])*/)+a.z*fabs(E[j].y/*mtDot(A[1],E[j])*/);
				break;
			case 1:
				r = a.x*fabs(E[j].z/*mtDot(A[2],E[j])*/)+a.z*fabs(E[j].x/*mtDot(A[0],E[j])*/);
				break;
			case 2:
				r = a.x*fabs(E[j].y/*mtDot(A[1],E[j])*/)+a.y*fabs(E[j].x/*mtDot(A[0],E[j])*/);
				break;
			}

			p[0] = mtDot(L,D);

			switch(j)
			{
			case 0:
				p[1] = p[0];
				p[2] = p[0] + mtDot(A[i],N);			
				if((p[0]>r && p[2]>r) || (p[0]<-r && p[2]<-r))
					return FALSE;

				break;
			case 1:
				p[1] = p[0] - mtDot(A[i],N);
				p[2] = p[0];
				if((p[0]>r && p[1]>r) || (p[0]<-r && p[1]<-r))
					return FALSE;

				break;
			case 2:
				p[1] = p[0] - mtDot(A[i],N);
				p[2] = p[0] - mtDot(A[i],N);
				if((p[0]>r && p[1]>r) || (p[0]<-r && p[1]<-r))
					return FALSE;
				break;
			}

		}
	}

	return TRUE;

	// 
	// Axes Ai X Ej
	// 필요시 구현.
}

// Daniel 20061228 : Point Layer(TEXT) Culling 용
BOOL mtInterSection(RECT3D rect, POINT3D point)
{
	if( (rect.x1<=point.x && rect.x2>=point.x)
		&&(rect.z1<=point.z && rect.z2>=point.z) )
		return TRUE;
	else
		return FALSE;

}

BOOL mtInterSection(RECT3D rect1, RECT3D rect2)
{
	RECT3D result;

	result.x1 = maximum(rect1.x1, rect2.x1);
	//	result.y1 = maximum(rect1.y1, rect2.y1);
	result.z1 = maximum(rect1.z1, rect2.z1);

	result.x2 = minimum(rect1.x2, rect2.x2);
	//	result.y2 = minimum(rect1.y2, rect2.y2);
	result.z2 = minimum(rect1.z2, rect2.z2);

	return(   (result.x1 <= result.x2)
		//			&&(result.y1 <= result.y2) // 높이는 검사안함
		&&(result.z1 <= result.z2));
}

BOOL mtInterSection(RECT3D rect, PLANE3D plane)
{
	// jkseo
	// rect 이 plane 에 걸려있거나(instect) 앞에있으면(In_front)  TRUE 리턴,
	// 뒤에 있으면, FALSE 리턴

	POINT3D minPoint, maxPoint;

	// build two points based on the direction
	// of the plane vector. minPoint 
	// and maxPoint are the two points
	// on the rectangle furthest away from
	// each other along the plane normal

	if (plane.x > 0.0f)
	{
		minPoint.x = (float)rect.x1;
		maxPoint.x = (float)rect.x2;
	}
	else
	{
		minPoint.x = (float)rect.x2;
		maxPoint.x = (float)rect.x1;
	}

	if (plane.y > 0.0f)
	{
		minPoint.y = (float)rect.y1;
		maxPoint.y = (float)rect.y2;
	}
	else
	{
		minPoint.y = (float)rect.y2;
		maxPoint.y = (float)rect.y1;
	}

	if (plane.z > 0.0f)
	{
		minPoint.z = (float)rect.z1;
		maxPoint.z = (float)rect.z2;
	}
	else
	{
		minPoint.z = (float)rect.z2;
		maxPoint.z = (float)rect.z1;
	}

	//	minPoint.x = rect.x1;
	//	minPoint.y = rect.y1;
	//	minPoint.z = rect.z1;

	//	maxPoint.x = rect.x2;
	//	maxPoint.y = rect.y2;
	//	maxPoint.z = rect.z2;


	// compute the signed distance from 
	// the plane to both points
	float dmin = mtDot(plane,minPoint);
	float dmax = mtDot(plane,maxPoint);

	// the rectangle intersects the plane if
	// one value is positive and the other is negative
	if (dmin * dmax < 0.0f )
		return TRUE;

	else if(dmin > 0.0f)
		return TRUE;

	return FALSE;
}

BOOL mtInterSection(POINT3D point, PLANE3D plane)
{
	// jkseo
	float fsign = mtDot(plane,point);

	// the rectangle intersects the plane if
	// one value is positive and the other is negative
	if(fsign > 0.0f)
		return TRUE;

	return FALSE;
}

BOOL mtInterSection(FRUSTUM Frustum, RECT3D rect)
{
	if ( 
		mtInterSection(rect, Frustum.leftPlane)		
		&& mtInterSection(rect, Frustum.rightPlane) 
		&& mtInterSection(rect, Frustum.topPlane)	
		&& mtInterSection(rect, Frustum.bottomPlane)
//		&& mtInterSection(rect, Frustum.nearPlane)	
		&& mtInterSection(rect, Frustum.farPlane)	
		)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL mtInterSection(FRUSTUM Frustum, POINT3D point)
{
	if ( mtInterSection(point, Frustum.leftPlane)		
		&& mtInterSection(point, Frustum.rightPlane) 
		&& mtInterSection(point, Frustum.topPlane)	
		&& mtInterSection(point, Frustum.bottomPlane)
		&& mtInterSection(point, Frustum.nearPlane)	
		&& mtInterSection(point, Frustum.farPlane)	
		)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL mtPointInPolygon(POINT3D point, POINT3D* vertex, int num_vertex)
{
	POINT3D newPoint, oldPoint;
	POINT3D left, right;

	BOOL inside = FALSE;

	// polygon은 적어도 삼각형이어야 한다.
	if(num_vertex < 3)   return false;

	oldPoint = vertex[num_vertex-1];
	for(int i = 0 ; i < num_vertex ; i++)
	{
		newPoint = vertex[i];
		if(newPoint.x > oldPoint.x)
		{
			left  = oldPoint;
			right = newPoint;
		}
		else
		{
			left = newPoint;
			right = oldPoint;
		}

		// 점이 변의 왼쪽에 있다면 다각형 바깥에 있는 것이다.
		if((newPoint.x < point.x) == (point.x <= oldPoint.x) &&
			(point.z - left.z) * (right.x - left.x) < (right.z - left.z) * (point.x - left.x))
		{
			inside =! inside;
		}

		oldPoint = newPoint;
	}

	return (inside);
}

POINT3D mtRect3DSize(RECT3D rect)
{
	POINT3D vecSize;
	vecSize.x=rect.x2-rect.x1;
	vecSize.y=rect.y2-rect.y1;
	vecSize.z=rect.z2-rect.z1;

	return vecSize;
}

POINT3D mtRect3DMinPoint(RECT3D rect)
{
	POINT3D minPoint;

	minPoint.x=rect.x1;
	minPoint.y=rect.y1;
	minPoint.z=rect.z1;

	return minPoint;

}

POINT3D mtRect3DCenter(RECT3D rect)
{
	POINT3D center;

	center.x=(rect.x1+rect.x2)*0.5f;
	center.y=(rect.y1+rect.y2)*0.5f;
	center.z=(rect.z1+rect.z2)*0.5f;

	return center;
}


RECT3D operator + (const RECT3D Rect, const POINT3D Vec)
{
	RECT3D result;

	result.x1 = float(Rect.x1+((double)Vec.x)); 
	result.x2 = float(Rect.x2+((double)Vec.x)); 
	result.y1 = float(Rect.y1+((double)Vec.y)); 
	result.y2 = float(Rect.y2+((double)Vec.y)); 
	result.z1 = float(Rect.z1+((double)Vec.z)); 
	result.z2 = float(Rect.z2+((double)Vec.z)); 

	return result;



}

RECT3D operator - (const RECT3D Rect, const POINT3D Vec)
{
	RECT3D result;

	result.x1 = float(Rect.x1-((double)Vec.x)); 
	result.x2 = float(Rect.x2-((double)Vec.x)); 
	result.y1 = float(Rect.y1-((double)Vec.y)); 
	result.y2 = float(Rect.y2-((double)Vec.y)); 
	result.z1 = float(Rect.z1-((double)Vec.z)); 
	result.z2 = float(Rect.z2-((double)Vec.z)); 

	return result;
}

RECT3D operator * (const RECT3D Rect, const POINT3D Vec)
{
	RECT3D result;

	result.x1 = float(Rect.x1*((double)Vec.x)); 
	result.x2 = float(Rect.x2*((double)Vec.x)); 
	result.y1 = float(Rect.y1*((double)Vec.y)); 
	result.y2 = float(Rect.y2*((double)Vec.y)); 
	result.z1 = float(Rect.z1*((double)Vec.z)); 
	result.z2 = float(Rect.z2*((double)Vec.z)); 

	return result;

}

RECT3D operator / (const RECT3D Rect, const POINT3D Vec)
{
	RECT3D result;

	result.x1 = float(Rect.x1/((double)Vec.x)); 
	result.x2 = float(Rect.x2/((double)Vec.x)); 
	result.y1 = float(Rect.y1/((double)Vec.y)); 
	result.y2 = float(Rect.y2/((double)Vec.y)); 
	result.z1 = float(Rect.z1/((double)Vec.z)); 
	result.z2 = float(Rect.z2/((double)Vec.z)); 

	return result;

}

RECT3D operator * (const RECT3D Rect, const float value)
{
	RECT3D result;

	result.x1 = float(Rect.x1*((double)value)); 
	result.x2 = float(Rect.x2*((double)value)); 
	result.y1 = float(Rect.y1*((double)value)); 
	result.y2 = float(Rect.y2*((double)value)); 
	result.z1 = float(Rect.z1*((double)value)); 
	result.z2 = float(Rect.z2*((double)value)); 

	return result;

}

RECT3D operator / (const RECT3D Rect, const float value)
{
	RECT3D result;

	result.x1 = float(Rect.x1/((double)value)); 
	result.x2 = float(Rect.x2/((double)value)); 
	result.y1 = float(Rect.y1/((double)value)); 
	result.y2 = float(Rect.y2/((double)value)); 
	result.z1 = float(Rect.z1/((double)value)); 
	result.z2 = float(Rect.z2/((double)value)); 

	return result;
}

double mtSqrt(double f)
{
	return sqrt(f);

}

float mtSqrt(float f)
{
	return (float) sqrt(double(f));
	if (f <= 0.0f) return 0.0f;

	static const float sqrt_table[] = 
	{
		0.000000f, 0.316228f, 0.447214f, 0.547723f, 0.632456f, 0.707107f, 0.774597f, 0.836660f, 0.894427f, 0.948683f, //  0.0
		1.000000f, 1.048809f, 1.095445f, 1.140175f, 1.183216f, 1.224745f, 1.264911f, 1.303840f, 1.341641f, 1.378405f, //  1.0
		1.414214f, 1.449138f, 1.483240f, 1.516575f, 1.549193f, 1.581139f, 1.612452f, 1.643168f, 1.673320f, 1.702939f, //  2.0
		1.732051f, 1.760682f, 1.788854f, 1.816590f, 1.843909f, 1.870829f, 1.897367f, 1.923538f, 1.949359f, 1.974842f, //  3.0
		2.000000f, 2.024846f, 2.049390f, 2.073644f, 2.097618f, 2.121320f, 2.144761f, 2.167948f, 2.190890f, 2.213594f, //  4.0
		2.236068f, 2.258318f, 2.280351f, 2.302173f, 2.323790f, 2.345208f, 2.366432f, 2.387467f, 2.408319f, 2.428992f, //  5.0
		2.449490f, 2.469818f, 2.489980f, 2.509980f, 2.529822f, 2.549510f, 2.569046f, 2.588436f, 2.607681f, 2.626785f, //  6.0
		2.645751f, 2.664583f, 2.683282f, 2.701851f, 2.720294f, 2.738613f, 2.756810f, 2.774887f, 2.792848f, 2.810694f, //  7.0
		2.828427f, 2.846050f, 2.863564f, 2.880972f, 2.898275f, 2.915476f, 2.932576f, 2.949576f, 2.966479f, 2.983287f, //  8.0
		3.000000f, 3.016621f, 3.033150f, 3.049590f, 3.065942f, 3.082207f, 3.098387f, 3.114482f, 3.130495f, 3.146427f, //  9.0
		3.162278f, 3.178050f, 3.193744f, 3.209361f, 3.224903f, 3.240370f, 3.255764f, 3.271085f, 3.286335f, 3.301515f, // 10.0
		3.316625f, 3.331666f, 3.346640f, 3.361547f, 3.376389f, 3.391165f, 3.405877f, 3.420526f, 3.435113f, 3.449638f, // 11.0
		3.464102f, 3.478505f, 3.492850f, 3.507136f, 3.521363f, 3.535534f, 3.549648f, 3.563706f, 3.577709f, 3.591657f, // 12.0
		3.605551f, 3.619392f, 3.633180f, 3.646917f, 3.660601f, 3.674235f, 3.687818f, 3.701351f, 3.714835f, 3.728270f, // 13.0
		3.741657f, 3.754997f, 3.768289f, 3.781534f, 3.794733f, 3.807887f, 3.820995f, 3.834058f, 3.847077f, 3.860052f, // 14.0
		3.872983f, 3.885872f, 3.898718f, 3.911521f, 3.924283f, 3.937004f, 3.949684f, 3.962323f, 3.974921f, 3.987480f, // 15.0
		4.000000f, 4.012481f, 4.024922f, 4.037326f, 4.049691f, 4.062019f, 4.074310f, 4.086563f, 4.098780f, 4.110961f, // 16.0
		4.123106f, 4.135215f, 4.147288f, 4.159327f, 4.171331f, 4.183300f, 4.195235f, 4.207137f, 4.219005f, 4.230839f, // 17.0
		4.242641f, 4.254410f, 4.266146f, 4.277850f, 4.289522f, 4.301163f, 4.312772f, 4.324350f, 4.335897f, 4.347413f, // 18.0
		4.358899f, 4.370355f, 4.381781f, 4.393177f, 4.404543f, 4.415880f, 4.427189f, 4.438468f, 4.449719f, 4.460942f, // 19.0
		4.472136f, 4.483302f, 4.494441f, 4.505552f, 4.516636f, 4.527693f, 4.538722f, 4.549725f, 4.560702f, 4.571652f, // 20.0
		4.582576f, 4.593474f, 4.604346f, 4.615192f, 4.626013f, 4.636809f, 4.647580f, 4.658326f, 4.669047f, 4.679744f, // 21.0
		4.690416f, 4.701064f, 4.711688f, 4.722288f, 4.732864f, 4.743416f, 4.753946f, 4.764452f, 4.774935f, 4.785394f, // 22.0
		4.795832f, 4.806246f, 4.816638f, 4.827007f, 4.837355f, 4.847680f, 4.857983f, 4.868265f, 4.878524f, 4.888763f, // 23.0
		4.898979f, 4.909175f, 4.919350f, 4.929503f, 4.939636f, 4.949747f, 4.959839f, 4.969910f, 4.979960f, 4.989990f, // 24.0
		5.000000f, 5.009990f, 5.019960f, 5.029911f, 5.039841f, 5.049752f, 5.059644f, 5.069517f, 5.079370f, 5.089204f, // 25.0
		5.099020f, 5.108816f, 5.118594f, 5.128353f, 5.138093f, 5.147815f, 5.157519f, 5.167204f, 5.176872f, 5.186521f, // 26.0
		5.196152f, 5.205766f, 5.215362f, 5.224940f, 5.234501f, 5.244044f, 5.253570f, 5.263079f, 5.272571f, 5.282045f, // 27.0
		5.291503f, 5.300943f, 5.310367f, 5.319775f, 5.329165f, 5.338539f, 5.347897f, 5.357238f, 5.366563f, 5.375872f, // 28.0
		5.385165f, 5.394442f, 5.403703f, 5.412948f, 5.422177f, 5.431390f, 5.440588f, 5.449771f, 5.458938f, 5.468089f, // 29.0
		5.477226f, 5.486347f, 5.495453f, 5.504544f, 5.513619f, 5.522681f, 5.531727f, 5.540758f, 5.549775f, 5.558777f, // 30.0
		5.567764f, 5.576737f, 5.585696f, 5.594640f, 5.603570f, 5.612486f, 5.621388f, 5.630275f, 5.639149f, 5.648008f, // 31.0
		5.656854f, 5.665686f, 5.674504f, 5.683309f, 5.692100f, 5.700877f, 5.709641f, 5.718391f, 5.727128f, 5.735852f, // 32.0
		5.744563f, 5.753260f, 5.761944f, 5.770615f, 5.779273f, 5.787918f, 5.796551f, 5.805170f, 5.813777f, 5.822371f, // 33.0
		5.830952f, 5.839521f, 5.848077f, 5.856620f, 5.865151f, 5.873670f, 5.882177f, 5.890671f, 5.899152f, 5.907622f, // 34.0
		5.916080f, 5.924525f, 5.932959f, 5.941380f, 5.949790f, 5.958188f, 5.966574f, 5.974948f, 5.983310f, 5.991661f, // 35.0
		6.000000f, 6.008328f, 6.016644f, 6.024948f, 6.033241f, 6.041523f, 6.049794f, 6.058053f, 6.066300f, 6.074537f, // 36.0
		6.082763f, 6.090977f, 6.099180f, 6.107373f, 6.115554f, 6.123724f, 6.131884f, 6.140033f, 6.148170f, 6.156298f, // 37.0
		6.164414f, 6.172520f, 6.180615f, 6.188699f, 6.196773f, 6.204837f, 6.212890f, 6.220932f, 6.228965f, 6.236987f, // 38.0
		6.244998f, 6.252999f, 6.260990f, 6.268971f, 6.276942f, 6.284903f, 6.292853f, 6.300794f, 6.308724f, 6.316645f, // 39.0
		6.324555f, 6.332456f, 6.340347f, 6.348228f, 6.356100f, 6.363961f, 6.371813f, 6.379655f, 6.387488f, 6.395311f, // 40.0
		6.403124f, 6.410928f, 6.418723f, 6.426508f, 6.434283f, 6.442049f, 6.449806f, 6.457554f, 6.465292f, 6.473021f, // 41.0
		6.480741f, 6.488451f, 6.496153f, 6.503845f, 6.511528f, 6.519202f, 6.526868f, 6.534524f, 6.542171f, 6.549809f, // 42.0
		6.557439f, 6.565059f, 6.572671f, 6.580273f, 6.587868f, 6.595453f, 6.603030f, 6.610598f, 6.618157f, 6.625708f, // 43.0
		6.633250f, 6.640783f, 6.648308f, 6.655824f, 6.663333f, 6.670832f, 6.678323f, 6.685806f, 6.693280f, 6.700746f, // 44.0
		6.708204f, 6.715654f, 6.723095f, 6.730527f, 6.737952f, 6.745369f, 6.752777f, 6.760178f, 6.767570f, 6.774954f, // 45.0
		6.782330f, 6.789698f, 6.797058f, 6.804410f, 6.811755f, 6.819091f, 6.826419f, 6.833740f, 6.841052f, 6.848358f, // 46.0
		6.855655f, 6.862944f, 6.870226f, 6.877499f, 6.884766f, 6.892024f, 6.899275f, 6.906519f, 6.913754f, 6.920983f, // 47.0
		6.928203f, 6.935417f, 6.942622f, 6.949820f, 6.957011f, 6.964194f, 6.971370f, 6.978539f, 6.985700f, 6.992854f, // 48.0
		7.000000f, 7.007139f, 7.014271f, 7.021396f, 7.028513f, 7.035624f, 7.042727f, 7.049823f, 7.056911f, 7.063993f, // 49.0
		7.071068f, 7.078136f, 7.085196f, 7.092249f, 7.099296f, 7.106335f, 7.113368f, 7.120393f, 7.127412f, 7.134424f, // 50.0
		7.141428f, 7.148427f, 7.155418f, 7.162402f, 7.169379f, 7.176350f, 7.183314f, 7.190271f, 7.197222f, 7.204166f, // 51.0
		7.211103f, 7.218033f, 7.224957f, 7.231874f, 7.238785f, 7.245688f, 7.252586f, 7.259477f, 7.266361f, 7.273239f, // 52.0
		7.280110f, 7.286975f, 7.293833f, 7.300685f, 7.307530f, 7.314369f, 7.321202f, 7.328028f, 7.334848f, 7.341662f, // 53.0
		7.348469f, 7.355270f, 7.362065f, 7.368853f, 7.375636f, 7.382412f, 7.389181f, 7.395945f, 7.402702f, 7.409454f, // 54.0
		7.416198f, 7.422938f, 7.429670f, 7.436397f, 7.443118f, 7.449832f, 7.456541f, 7.463243f, 7.469940f, 7.476630f, // 55.0
		7.483315f, 7.489993f, 7.496666f, 7.503333f, 7.509993f, 7.516648f, 7.523297f, 7.529940f, 7.536577f, 7.543209f, // 56.0
		7.549834f, 7.556454f, 7.563068f, 7.569676f, 7.576279f, 7.582875f, 7.589467f, 7.596052f, 7.602631f, 7.609205f, // 57.0
		7.615773f, 7.622336f, 7.628892f, 7.635444f, 7.641989f, 7.648529f, 7.655064f, 7.661593f, 7.668116f, 7.674634f, // 58.0
		7.681146f, 7.687653f, 7.694154f, 7.700649f, 7.707140f, 7.713624f, 7.720104f, 7.726578f, 7.733046f, 7.739509f, // 59.0
		7.745967f, 7.752419f, 7.758866f, 7.765307f, 7.771744f, 7.778175f, 7.784600f, 7.791021f, 7.797435f, 7.803845f, // 60.0
		7.810250f, 7.816649f, 7.823043f, 7.829432f, 7.835815f, 7.842194f, 7.848567f, 7.854935f, 7.861298f, 7.867655f, // 61.0
		7.874008f, 7.880355f, 7.886698f, 7.893035f, 7.899367f, 7.905694f, 7.912016f, 7.918333f, 7.924645f, 7.930952f, // 62.0
		7.937254f, 7.943551f, 7.949843f, 7.956130f, 7.962412f, 7.968689f, 7.974961f, 7.981228f, 7.987490f, 7.993748f, // 63.0
		8.000000f, 8.006247f, 8.012491f, 8.018728f, 8.024961f, 8.031189f, 8.037412f, 8.043631f, 8.049845f, 8.056054f, // 64.0
		8.062258f, 8.068457f, 8.074652f, 8.080842f, 8.087027f, 8.093207f, 8.099383f, 8.105554f, 8.111720f, 8.117882f, // 65.0
		8.124038f, 8.130191f, 8.136339f, 8.142481f, 8.148620f, 8.154753f, 8.160882f, 8.167007f, 8.173127f, 8.179242f, // 66.0
		8.185353f, 8.191459f, 8.197561f, 8.203658f, 8.209750f, 8.215838f, 8.221922f, 8.228001f, 8.234076f, 8.240146f, // 67.0
		8.246211f, 8.252272f, 8.258329f, 8.264382f, 8.270429f, 8.276473f, 8.282512f, 8.288547f, 8.294577f, 8.300602f, // 68.0
		8.306624f, 8.312641f, 8.318654f, 8.324662f, 8.330666f, 8.336666f, 8.342661f, 8.348653f, 8.354640f, 8.360622f, // 69.0
		8.366600f, 8.372574f, 8.378544f, 8.384510f, 8.390471f, 8.396428f, 8.402381f, 8.408329f, 8.414274f, 8.420214f, // 70.0
		8.426150f, 8.432082f, 8.438010f, 8.443933f, 8.449852f, 8.455767f, 8.461678f, 8.467586f, 8.473488f, 8.479387f, // 71.0
		8.485281f, 8.491172f, 8.497059f, 8.502941f, 8.508819f, 8.514693f, 8.520563f, 8.526430f, 8.532292f, 8.538150f, // 72.0
		8.544004f, 8.549854f, 8.555700f, 8.561542f, 8.567380f, 8.573214f, 8.579044f, 8.584871f, 8.590693f, 8.596511f, // 73.0
		8.602325f, 8.608136f, 8.613942f, 8.619745f, 8.625544f, 8.631338f, 8.637129f, 8.642916f, 8.648700f, 8.654479f, // 74.0
		8.660254f, 8.666026f, 8.671794f, 8.677557f, 8.683317f, 8.689074f, 8.694826f, 8.700575f, 8.706320f, 8.712061f, // 75.0
		8.717798f, 8.723531f, 8.729261f, 8.734987f, 8.740709f, 8.746428f, 8.752143f, 8.757854f, 8.763561f, 8.769265f, // 76.0
		8.774964f, 8.780660f, 8.786353f, 8.792042f, 8.797727f, 8.803408f, 8.809086f, 8.814761f, 8.820431f, 8.826098f, // 77.0
		8.831761f, 8.837420f, 8.843077f, 8.848729f, 8.854378f, 8.860023f, 8.865664f, 8.871302f, 8.876937f, 8.882567f, // 78.0
		8.888194f, 8.893818f, 8.899438f, 8.905055f, 8.910668f, 8.916277f, 8.921883f, 8.927486f, 8.933085f, 8.938680f, // 79.0
		8.944272f, 8.949860f, 8.955446f, 8.961027f, 8.966605f, 8.972179f, 8.977750f, 8.983318f, 8.988882f, 8.994443f, // 80.0
		9.000000f, 9.005554f, 9.011105f, 9.016651f, 9.022195f, 9.027735f, 9.033272f, 9.038805f, 9.044335f, 9.049862f, // 81.0
		9.055385f, 9.060905f, 9.066422f, 9.071935f, 9.077445f, 9.082951f, 9.088454f, 9.093954f, 9.099451f, 9.104944f, // 82.0
		9.110434f, 9.115920f, 9.121404f, 9.126884f, 9.132360f, 9.137833f, 9.143303f, 9.148771f, 9.154234f, 9.159694f, // 83.0
		9.165151f, 9.170605f, 9.176056f, 9.181503f, 9.186947f, 9.192388f, 9.197826f, 9.203261f, 9.208692f, 9.214120f, // 84.0
		9.219544f, 9.224966f, 9.230385f, 9.235800f, 9.241212f, 9.246621f, 9.252027f, 9.257430f, 9.262829f, 9.268225f, // 85.0
		9.273618f, 9.279008f, 9.284396f, 9.289779f, 9.295160f, 9.300538f, 9.305912f, 9.311284f, 9.316652f, 9.322017f, // 86.0
		9.327379f, 9.332738f, 9.338094f, 9.343447f, 9.348797f, 9.354143f, 9.359487f, 9.364828f, 9.370166f, 9.375500f, // 87.0
		9.380832f, 9.386160f, 9.391486f, 9.396808f, 9.402127f, 9.407444f, 9.412757f, 9.418068f, 9.423375f, 9.428680f, // 88.0
		9.433981f, 9.439280f, 9.444575f, 9.449868f, 9.455157f, 9.460444f, 9.465728f, 9.471009f, 9.476286f, 9.481561f, // 89.0
		9.486833f, 9.492102f, 9.497368f, 9.502631f, 9.507892f, 9.513149f, 9.518403f, 9.523655f, 9.528904f, 9.534149f, // 90.0
		9.539392f, 9.544632f, 9.549869f, 9.555104f, 9.560335f, 9.565563f, 9.570789f, 9.576012f, 9.581232f, 9.586449f, // 91.0
		9.591663f, 9.596874f, 9.602083f, 9.607289f, 9.612492f, 9.617692f, 9.622889f, 9.628084f, 9.633276f, 9.638465f, // 92.0
		9.643651f, 9.648834f, 9.654015f, 9.659193f, 9.664368f, 9.669540f, 9.674709f, 9.679876f, 9.685040f, 9.690201f, // 93.0
		9.695360f, 9.700515f, 9.705669f, 9.710819f, 9.715966f, 9.721111f, 9.726253f, 9.731393f, 9.736529f, 9.741663f, // 94.0
		9.746794f, 9.751923f, 9.757049f, 9.762172f, 9.767292f, 9.772410f, 9.777525f, 9.782638f, 9.787748f, 9.792855f, // 95.0
		9.797959f, 9.803061f, 9.808160f, 9.813256f, 9.818350f, 9.823441f, 9.828530f, 9.833616f, 9.838699f, 9.843780f, // 96.0
		9.848858f, 9.853933f, 9.859006f, 9.864076f, 9.869144f, 9.874209f, 9.879271f, 9.884331f, 9.889388f, 9.894443f, // 97.0
		9.899495f, 9.904544f, 9.909592f, 9.914636f, 9.919677f, 9.924717f, 9.929753f, 9.934788f, 9.939819f, 9.944848f, // 98.0
		9.949874f, 9.954898f, 9.959920f, 9.964939f, 9.969955f, 9.974969f, 9.979980f, 9.984989f, 9.989995f, 9.994999f  // 99.0
	};

	float a = 1.0f;
	while (f >= 100.0f)
	{
		a *= 10.0f;
		f *= 0.01f;
	}

	while (f < 1.0f)
	{
		a *= 0.1f;
		f *= 100.0f;
	}

	int index = (int) (10.0f * f);
	return a * sqrt_table[index];

}

float mtSin(float f)
{
	static const float sine_table[] = 
	{
		0.000000f, 0.010000f, 0.019999f, 0.029995f, 0.039989f, 0.049979f, 0.059964f, 0.069943f, 0.079915f, 0.089879f,	//  0.00
		0.099833f, 0.109778f, 0.119712f, 0.129634f, 0.139543f, 0.149438f, 0.159318f, 0.169182f, 0.179030f, 0.188859f,	//  0.10
		0.198669f, 0.208460f, 0.218230f, 0.227978f, 0.237703f, 0.247404f, 0.257081f, 0.266731f, 0.276356f, 0.285952f,	//  0.20
		0.295520f, 0.305059f, 0.314567f, 0.324043f, 0.333487f, 0.342898f, 0.352274f, 0.361615f, 0.370920f, 0.380188f,	//  0.30
		0.389418f, 0.398609f, 0.407760f, 0.416871f, 0.425939f, 0.434966f, 0.443948f, 0.452886f, 0.461779f, 0.470626f,	//  0.40
		0.479426f, 0.488177f, 0.496880f, 0.505533f, 0.514136f, 0.522687f, 0.531186f, 0.539632f, 0.548024f, 0.556361f,	//  0.50
		0.564642f, 0.572867f, 0.581035f, 0.589145f, 0.597195f, 0.605186f, 0.613117f, 0.620986f, 0.628793f, 0.636537f,	//  0.60
		0.644218f, 0.651834f, 0.659385f, 0.666870f, 0.674288f, 0.681639f, 0.688921f, 0.696135f, 0.703279f, 0.710353f,	//  0.70
		0.717356f, 0.724287f, 0.731146f, 0.737931f, 0.744643f, 0.751280f, 0.757843f, 0.764329f, 0.770739f, 0.777072f,	//  0.80
		0.783327f, 0.789504f, 0.795602f, 0.801620f, 0.807558f, 0.813415f, 0.819192f, 0.824886f, 0.830497f, 0.836026f,	//  0.90
		0.841471f, 0.846832f, 0.852108f, 0.857299f, 0.862404f, 0.867423f, 0.872355f, 0.877201f, 0.881958f, 0.886627f,	//  1.00
		0.891207f, 0.895699f, 0.900100f, 0.904412f, 0.908633f, 0.912764f, 0.916803f, 0.920751f, 0.924606f, 0.928369f,	//  1.10
		0.932039f, 0.935616f, 0.939099f, 0.942489f, 0.945784f, 0.948985f, 0.952090f, 0.955101f, 0.958016f, 0.960835f,	//  1.20
		0.963558f, 0.966185f, 0.968715f, 0.971148f, 0.973485f, 0.975723f, 0.977865f, 0.979908f, 0.981854f, 0.983701f,	//  1.30
		0.985450f, 0.987100f, 0.988652f, 0.990105f, 0.991458f, 0.992713f, 0.993868f, 0.994924f, 0.995881f, 0.996738f,	//  1.40
		0.997495f, 0.998152f, 0.998710f, 0.999168f, 0.999526f, 0.999784f, 0.999942f, 1.000000f, 0.999958f, 0.999816f,	//  1.50
		0.999574f, 0.999232f, 0.998790f, 0.998248f, 0.997606f, 0.996865f, 0.996024f, 0.995083f, 0.994043f, 0.992904f,	//  1.60
		0.991665f, 0.990327f, 0.988890f, 0.987354f, 0.985719f, 0.983986f, 0.982154f, 0.980224f, 0.978197f, 0.976071f,	//  1.70
		0.973848f, 0.971527f, 0.969109f, 0.966594f, 0.963983f, 0.961275f, 0.958471f, 0.955572f, 0.952576f, 0.949486f,	//  1.80
		0.946300f, 0.943020f, 0.939645f, 0.936177f, 0.932615f, 0.928960f, 0.925212f, 0.921371f, 0.917438f, 0.913413f,	//  1.90
		0.909297f, 0.905091f, 0.900793f, 0.896406f, 0.891929f, 0.887362f, 0.882707f, 0.877964f, 0.873133f, 0.868215f,	//  2.00
		0.863209f, 0.858118f, 0.852941f, 0.847678f, 0.842330f, 0.836899f, 0.831383f, 0.825785f, 0.820104f, 0.814341f,	//  2.10
		0.808496f, 0.802571f, 0.796565f, 0.790480f, 0.784316f, 0.778073f, 0.771753f, 0.765355f, 0.758881f, 0.752331f,	//  2.20
		0.745705f, 0.739005f, 0.732231f, 0.725384f, 0.718465f, 0.711473f, 0.704411f, 0.697278f, 0.690075f, 0.682803f,	//  2.30
		0.675463f, 0.668056f, 0.660581f, 0.653041f, 0.645435f, 0.637765f, 0.630031f, 0.622234f, 0.614374f, 0.606454f,	//  2.40
		0.598472f, 0.590431f, 0.582331f, 0.574172f, 0.565956f, 0.557684f, 0.549355f, 0.540972f, 0.532535f, 0.524044f,	//  2.50
		0.515501f, 0.506907f, 0.498262f, 0.489567f, 0.480823f, 0.472030f, 0.463191f, 0.454306f, 0.445375f, 0.436399f,	//  2.60
		0.427380f, 0.418318f, 0.409214f, 0.400069f, 0.390885f, 0.381661f, 0.372399f, 0.363100f, 0.353764f, 0.344394f,	//  2.70
		0.334988f, 0.325549f, 0.316078f, 0.306575f, 0.297041f, 0.287478f, 0.277886f, 0.268266f, 0.258619f, 0.248947f,	//  2.80
		0.239249f, 0.229528f, 0.219784f, 0.210017f, 0.200230f, 0.190423f, 0.180596f, 0.170752f, 0.160890f, 0.151013f,	//  2.90
		0.141120f, 0.131213f, 0.121293f, 0.111361f, 0.101418f, 0.091465f, 0.081502f, 0.071532f, 0.061554f, 0.051570f,	//  3.00
		0.041581f, 0.031588f, 0.021591f, 0.011592f, 0.001593f,-0.008407f,-0.018406f,-0.028404f,-0.038398f,-0.048389f,	//  3.10
		-0.058374f,-0.068354f,-0.078327f,-0.088292f,-0.098249f,-0.108195f,-0.118131f,-0.128055f,-0.137966f,-0.147863f,	//  3.20
		-0.157746f,-0.167612f,-0.177462f,-0.187295f,-0.197108f,-0.206902f,-0.216675f,-0.226426f,-0.236155f,-0.245861f,	//  3.30
		-0.255541f,-0.265196f,-0.274825f,-0.284426f,-0.293998f,-0.303542f,-0.313054f,-0.322536f,-0.331985f,-0.341401f,	//  3.40
		-0.350783f,-0.360130f,-0.369441f,-0.378715f,-0.387951f,-0.397148f,-0.406306f,-0.415423f,-0.424498f,-0.433531f,	//  3.50
		-0.442520f,-0.451466f,-0.460366f,-0.469220f,-0.478027f,-0.486787f,-0.495497f,-0.504159f,-0.512769f,-0.521329f,	//  3.60
		-0.529836f,-0.538291f,-0.546691f,-0.555037f,-0.563327f,-0.571561f,-0.579738f,-0.587857f,-0.595917f,-0.603918f,	//  3.70
		-0.611858f,-0.619737f,-0.627554f,-0.635308f,-0.642999f,-0.650625f,-0.658186f,-0.665682f,-0.673111f,-0.680473f,	//  3.80
		-0.687766f,-0.694991f,-0.702146f,-0.709231f,-0.716246f,-0.723188f,-0.730058f,-0.736856f,-0.743579f,-0.750228f,	//  3.90
		-0.756802f,-0.763301f,-0.769723f,-0.776068f,-0.782336f,-0.788525f,-0.794636f,-0.800667f,-0.806618f,-0.812488f,	//  4.00
		-0.818277f,-0.823984f,-0.829609f,-0.835151f,-0.840609f,-0.845984f,-0.851273f,-0.856478f,-0.861597f,-0.866630f,	//  4.10
		-0.871576f,-0.876435f,-0.881206f,-0.885889f,-0.890484f,-0.894989f,-0.899406f,-0.903732f,-0.907967f,-0.912112f,	//  4.20
		-0.916166f,-0.920128f,-0.923998f,-0.927776f,-0.931461f,-0.935053f,-0.938551f,-0.941955f,-0.945266f,-0.948481f,	//  4.30
		-0.951602f,-0.954628f,-0.957558f,-0.960392f,-0.963131f,-0.965773f,-0.968319f,-0.970767f,-0.973119f,-0.975373f,	//  4.40
		-0.977530f,-0.979589f,-0.981550f,-0.983413f,-0.985178f,-0.986844f,-0.988411f,-0.989880f,-0.991249f,-0.992520f,	//  4.50
		-0.993691f,-0.994763f,-0.995735f,-0.996608f,-0.997381f,-0.998054f,-0.998628f,-0.999102f,-0.999476f,-0.999749f,	//  4.60
		-0.999923f,-0.999997f,-0.999971f,-0.999845f,-0.999619f,-0.999293f,-0.998867f,-0.998341f,-0.997715f,-0.996990f,	//  4.70
		-0.996165f,-0.995240f,-0.994216f,-0.993092f,-0.991869f,-0.990547f,-0.989125f,-0.987605f,-0.985986f,-0.984269f,	//  4.80
		-0.982453f,-0.980538f,-0.978526f,-0.976416f,-0.974208f,-0.971903f,-0.969501f,-0.967001f,-0.964405f,-0.961713f,	//  4.90
		-0.958924f,-0.956040f,-0.953060f,-0.949984f,-0.946814f,-0.943549f,-0.940189f,-0.936736f,-0.933189f,-0.929548f,	//  5.00
		-0.925815f,-0.921989f,-0.918071f,-0.914060f,-0.909959f,-0.905767f,-0.901484f,-0.897110f,-0.892648f,-0.888096f,	//  5.10
		-0.883455f,-0.878725f,-0.873908f,-0.869004f,-0.864012f,-0.858934f,-0.853771f,-0.848522f,-0.843188f,-0.837770f,	//  5.20
		-0.832267f,-0.826682f,-0.821014f,-0.815264f,-0.809433f,-0.803520f,-0.797527f,-0.791455f,-0.785303f,-0.779073f,	//  5.30
		-0.772764f,-0.766379f,-0.759917f,-0.753379f,-0.746765f,-0.740077f,-0.733315f,-0.726480f,-0.719572f,-0.712592f,	//  5.40
		-0.705540f,-0.698418f,-0.691227f,-0.683966f,-0.676637f,-0.669240f,-0.661776f,-0.654246f,-0.646651f,-0.638990f,	//  5.50
		-0.631267f,-0.623479f,-0.615630f,-0.607719f,-0.599747f,-0.591716f,-0.583625f,-0.575475f,-0.567269f,-0.559005f,	//  5.60
		-0.550686f,-0.542311f,-0.533882f,-0.525400f,-0.516866f,-0.508279f,-0.499642f,-0.490955f,-0.482218f,-0.473434f,	//  5.70
		-0.464602f,-0.455724f,-0.446800f,-0.437832f,-0.428819f,-0.419764f,-0.410667f,-0.401529f,-0.392350f,-0.383133f,	//  5.80
		-0.373877f,-0.364583f,-0.355253f,-0.345888f,-0.336488f,-0.327055f,-0.317589f,-0.308091f,-0.298562f,-0.289003f,	//  5.90
		-0.279415f,-0.269800f,-0.260158f,-0.250489f,-0.240795f,-0.231078f,-0.221337f,-0.211574f,-0.201790f,-0.191986f,	//  6.00
		-0.182163f,-0.172321f,-0.162462f,-0.152587f,-0.142697f,-0.132792f,-0.122874f,-0.112944f,-0.103002f,-0.093050f,	//  6.10
		-0.083090f,-0.073120f,-0.063143f,-0.053160f,-0.043172f,-0.033179f,-0.023183f,-0.013185f,-0.003185f				//  6.20
	};

	if (f > A_TWO_PI || f < 0.0f) {
		int quotient = (int) (f / A_TWO_PI);

		if (f < 0.0f)
			quotient --;

		f -= quotient * A_TWO_PI;	// f becomes the remainder
	}

	int index = (int) (f * 100.0f);

	return sine_table[index];
}


/*
float FastCosf(float f)
{
return FastSinf(f + PI_2);
}
*/


float mtCos(float f)
{
	static const float cosine_table[] = 
	{
		1.000000f, 0.999950f, 0.999800f, 0.999550f, 0.999200f, 0.998750f, 0.998201f, 0.997551f, 0.996802f, 0.995953f,	//  0.00
		0.995004f, 0.993956f, 0.992809f, 0.991562f, 0.990216f, 0.988771f, 0.987227f, 0.985585f, 0.983844f, 0.982004f,	//  0.10
		0.980067f, 0.978031f, 0.975897f, 0.973666f, 0.971338f, 0.968912f, 0.966390f, 0.963771f, 0.961055f, 0.958244f,	//  0.20
		0.955336f, 0.952334f, 0.949235f, 0.946042f, 0.942755f, 0.939373f, 0.935897f, 0.932327f, 0.928665f, 0.924909f,	//  0.30
		0.921061f, 0.917121f, 0.913089f, 0.908966f, 0.904752f, 0.900447f, 0.896052f, 0.891568f, 0.886995f, 0.882333f,	//  0.40
		0.877583f, 0.872745f, 0.867819f, 0.862807f, 0.857709f, 0.852525f, 0.847255f, 0.841901f, 0.836463f, 0.830941f,	//  0.50
		0.825336f, 0.819648f, 0.813878f, 0.808028f, 0.802096f, 0.796084f, 0.789992f, 0.783822f, 0.777573f, 0.771246f,	//  0.60
		0.764842f, 0.758362f, 0.751806f, 0.745174f, 0.738469f, 0.731689f, 0.724836f, 0.717911f, 0.710914f, 0.703845f,	//  0.70
		0.696707f, 0.689498f, 0.682221f, 0.674876f, 0.667463f, 0.659983f, 0.652437f, 0.644827f, 0.637151f, 0.629412f,	//  0.80
		0.621610f, 0.613746f, 0.605820f, 0.597834f, 0.589788f, 0.581683f, 0.573520f, 0.565300f, 0.557023f, 0.548690f,	//  0.90
		0.540302f, 0.531861f, 0.523366f, 0.514819f, 0.506220f, 0.497571f, 0.488872f, 0.480124f, 0.471328f, 0.462485f,	//  1.00
		0.453596f, 0.444662f, 0.435682f, 0.426660f, 0.417595f, 0.408487f, 0.399340f, 0.390152f, 0.380925f, 0.371660f,	//  1.10
		0.362358f, 0.353019f, 0.343646f, 0.334238f, 0.324796f, 0.315322f, 0.305817f, 0.296281f, 0.286715f, 0.277121f,	//  1.20
		0.267499f, 0.257850f, 0.248175f, 0.238476f, 0.228753f, 0.219007f, 0.209239f, 0.199450f, 0.189641f, 0.179813f,	//  1.30
		0.169967f, 0.160104f, 0.150226f, 0.140332f, 0.130424f, 0.120503f, 0.110570f, 0.100626f, 0.090672f, 0.080708f,	//  1.40
		0.070737f, 0.060759f, 0.050775f, 0.040785f, 0.030791f, 0.020795f, 0.010796f, 0.000796f,-0.009204f,-0.019203f,	//  1.50
		-0.029200f,-0.039194f,-0.049184f,-0.059169f,-0.069148f,-0.079121f,-0.089085f,-0.099041f,-0.108987f,-0.118922f,	//  1.60
		-0.128845f,-0.138755f,-0.148651f,-0.158532f,-0.168397f,-0.178246f,-0.188077f,-0.197889f,-0.207681f,-0.217452f,	//  1.70
		-0.227202f,-0.236929f,-0.246632f,-0.256311f,-0.265964f,-0.275590f,-0.285189f,-0.294759f,-0.304300f,-0.313811f,	//  1.80
		-0.323290f,-0.332736f,-0.342150f,-0.351529f,-0.360873f,-0.370181f,-0.379452f,-0.388685f,-0.397879f,-0.407033f,	//  1.90
		-0.416147f,-0.425219f,-0.434248f,-0.443234f,-0.452176f,-0.461073f,-0.469923f,-0.478726f,-0.487482f,-0.496189f,	//  2.00
		-0.504846f,-0.513453f,-0.522008f,-0.530511f,-0.538962f,-0.547358f,-0.555699f,-0.563985f,-0.572215f,-0.580387f,	//  2.10
		-0.588501f,-0.596557f,-0.604552f,-0.612488f,-0.620362f,-0.628174f,-0.635923f,-0.643608f,-0.651230f,-0.658786f,	//  2.20
		-0.666276f,-0.673700f,-0.681056f,-0.688344f,-0.695563f,-0.702713f,-0.709792f,-0.716801f,-0.723738f,-0.730602f,	//  2.30
		-0.737394f,-0.744111f,-0.750755f,-0.757323f,-0.763815f,-0.770231f,-0.776570f,-0.782832f,-0.789015f,-0.795119f,	//  2.40
		-0.801144f,-0.807088f,-0.812952f,-0.818735f,-0.824435f,-0.830054f,-0.835589f,-0.841040f,-0.846408f,-0.851691f,	//  2.50
		-0.856889f,-0.862001f,-0.867027f,-0.871966f,-0.876818f,-0.881582f,-0.886258f,-0.890846f,-0.895344f,-0.899753f,	//  2.60
		-0.904072f,-0.908301f,-0.912438f,-0.916485f,-0.920440f,-0.924302f,-0.928073f,-0.931750f,-0.935335f,-0.938825f,	//  2.70
		-0.942222f,-0.945525f,-0.948733f,-0.951846f,-0.954865f,-0.957787f,-0.960614f,-0.963345f,-0.965979f,-0.968517f,	//  2.80
		-0.970958f,-0.973302f,-0.975549f,-0.977698f,-0.979749f,-0.981702f,-0.983557f,-0.985314f,-0.986972f,-0.988532f,	//  2.90
		-0.989992f,-0.991354f,-0.992617f,-0.993780f,-0.994844f,-0.995808f,-0.996673f,-0.997438f,-0.998104f,-0.998669f,	//  3.00
		-0.999135f,-0.999501f,-0.999767f,-0.999933f,-0.999999f,-0.999965f,-0.999831f,-0.999597f,-0.999263f,-0.998829f,	//  3.10
		-0.998295f,-0.997661f,-0.996928f,-0.996095f,-0.995162f,-0.994130f,-0.992998f,-0.991767f,-0.990437f,-0.989008f,	//  3.20
		-0.987480f,-0.985853f,-0.984128f,-0.982304f,-0.980382f,-0.978362f,-0.976244f,-0.974028f,-0.971715f,-0.969305f,	//  3.30
		-0.966798f,-0.964194f,-0.961494f,-0.958698f,-0.955806f,-0.952818f,-0.949735f,-0.946557f,-0.943285f,-0.939918f,	//  3.40
		-0.936457f,-0.932902f,-0.929254f,-0.925513f,-0.921680f,-0.917755f,-0.913737f,-0.909629f,-0.905429f,-0.901139f,	//  3.50
		-0.896758f,-0.892288f,-0.887729f,-0.883081f,-0.878345f,-0.873521f,-0.868609f,-0.863611f,-0.858526f,-0.853356f,	//  3.60
		-0.848100f,-0.842759f,-0.837334f,-0.831826f,-0.826234f,-0.820559f,-0.814803f,-0.808965f,-0.803046f,-0.797047f,	//  3.70
		-0.790968f,-0.784810f,-0.778573f,-0.772259f,-0.765867f,-0.759399f,-0.752855f,-0.746236f,-0.739541f,-0.732773f,	//  3.80
		-0.725932f,-0.719018f,-0.712033f,-0.704976f,-0.697848f,-0.690651f,-0.683385f,-0.676050f,-0.668648f,-0.661179f,	//  3.90
		-0.653644f,-0.646043f,-0.638378f,-0.630649f,-0.622857f,-0.615002f,-0.607087f,-0.599110f,-0.591073f,-0.582978f,	//  4.00
		-0.574824f,-0.566612f,-0.558345f,-0.550021f,-0.541642f,-0.533209f,-0.524723f,-0.516184f,-0.507593f,-0.498952f,	//  4.10
		-0.490261f,-0.481521f,-0.472733f,-0.463897f,-0.455015f,-0.446087f,-0.437115f,-0.428100f,-0.419041f,-0.409941f,	//  4.20
		-0.400799f,-0.391618f,-0.382397f,-0.373138f,-0.363842f,-0.354509f,-0.345141f,-0.335738f,-0.326302f,-0.316833f,	//  4.30
		-0.307333f,-0.297802f,-0.288241f,-0.278651f,-0.269033f,-0.259389f,-0.249718f,-0.240023f,-0.230303f,-0.220561f,	//  4.40
		-0.210796f,-0.201010f,-0.191204f,-0.181379f,-0.171536f,-0.161676f,-0.151800f,-0.141908f,-0.132003f,-0.122084f,	//  4.50
		-0.112153f,-0.102210f,-0.092258f,-0.082296f,-0.072326f,-0.062348f,-0.052365f,-0.042376f,-0.032383f,-0.022387f,	//  4.60
		-0.012389f,-0.002389f, 0.007611f, 0.017610f, 0.027607f, 0.037602f, 0.047593f, 0.057579f, 0.067560f, 0.077533f,	//  4.70
		0.087499f, 0.097456f, 0.107404f, 0.117340f, 0.127265f, 0.137177f, 0.147076f, 0.156959f, 0.166827f, 0.176679f,	//  4.80
		0.186512f, 0.196327f, 0.206123f, 0.215897f, 0.225651f, 0.235381f, 0.245089f, 0.254771f, 0.264428f, 0.274059f,	//  4.90
		0.283662f, 0.293237f, 0.302783f, 0.312298f, 0.321782f, 0.331234f, 0.340653f, 0.350038f, 0.359387f, 0.368701f,	//  5.00
		0.377978f, 0.387217f, 0.396417f, 0.405578f, 0.414698f, 0.423777f, 0.432813f, 0.441806f, 0.450755f, 0.459659f,	//  5.10
		0.468517f, 0.477328f, 0.486091f, 0.494806f, 0.503470f, 0.512085f, 0.520649f, 0.529161f, 0.537619f, 0.546024f,	//  5.20
		0.554374f, 0.562669f, 0.570908f, 0.579089f, 0.587213f, 0.595277f, 0.603283f, 0.611228f, 0.619112f, 0.626934f,	//  5.30
		0.634693f, 0.642389f, 0.650020f, 0.657587f, 0.665088f, 0.672522f, 0.679889f, 0.687188f, 0.694418f, 0.701579f,	//  5.40
		0.708670f, 0.715690f, 0.722638f, 0.729514f, 0.736317f, 0.743047f, 0.749702f, 0.756282f, 0.762786f, 0.769215f,	//  5.50
		0.775566f, 0.781840f, 0.788035f, 0.794152f, 0.800189f, 0.806147f, 0.812023f, 0.817819f, 0.823533f, 0.829164f,	//  5.60
		0.834713f, 0.840178f, 0.845559f, 0.850855f, 0.856067f, 0.861192f, 0.866232f, 0.871185f, 0.876051f, 0.880829f,	//  5.70
		0.885520f, 0.890121f, 0.894634f, 0.899057f, 0.903390f, 0.907633f, 0.911786f, 0.915846f, 0.919816f, 0.923693f,	//  5.80
		0.927478f, 0.931171f, 0.934770f, 0.938276f, 0.941688f, 0.945005f, 0.948229f, 0.951357f, 0.954390f, 0.957328f,	//  5.90
		0.960170f, 0.962916f, 0.965566f, 0.968120f, 0.970576f, 0.972935f, 0.975197f, 0.977362f, 0.979429f, 0.981398f,	//  6.00
		0.983268f, 0.985041f, 0.986715f, 0.988290f, 0.989766f, 0.991144f, 0.992422f, 0.993601f, 0.994681f, 0.995661f,	//  6.10
		0.996542f, 0.997323f, 0.998004f, 0.998586f, 0.999068f, 0.999449f, 0.999731f, 0.999913f, 0.999995f				//  6.20
	};

	if (f > A_TWO_PI || f < 0.0f) {
		int quotient = (int) (f / A_TWO_PI);

		if (f < 0.0f)
			quotient --;

		f -= quotient * A_TWO_PI;	// f becomes the remainder
	}

	int index = (int) (f * 100.0f);

	return cosine_table[index];
}


float mtTan(float f)
{
	static const float tan_table[] = 
	{
		0.000000f, 0.001000f, 0.002000f, 0.003000f, 0.004000f, 0.005000f, 0.006000f, 0.007000f, 0.008000f, 0.009000f,	// 0.000
		0.010000f, 0.011000f, 0.012001f, 0.013001f, 0.014001f, 0.015001f, 0.016001f, 0.017002f, 0.018002f, 0.019002f,	// 0.010
		0.020003f, 0.021003f, 0.022004f, 0.023004f, 0.024005f, 0.025005f, 0.026006f, 0.027007f, 0.028007f, 0.029008f,	// 0.020
		0.030009f, 0.031010f, 0.032011f, 0.033012f, 0.034013f, 0.035014f, 0.036016f, 0.037017f, 0.038018f, 0.039020f,	// 0.030
		0.040021f, 0.041023f, 0.042025f, 0.043027f, 0.044028f, 0.045030f, 0.046032f, 0.047035f, 0.048037f, 0.049039f,	// 0.040
		0.050042f, 0.051044f, 0.052047f, 0.053050f, 0.054053f, 0.055056f, 0.056059f, 0.057062f, 0.058065f, 0.059069f,	// 0.050
		0.060072f, 0.061076f, 0.062080f, 0.063083f, 0.064088f, 0.065092f, 0.066096f, 0.067100f, 0.068105f, 0.069110f,	// 0.060
		0.070115f, 0.071120f, 0.072125f, 0.073130f, 0.074135f, 0.075141f, 0.076147f, 0.077153f, 0.078159f, 0.079165f,	// 0.070
		0.080171f, 0.081178f, 0.082184f, 0.083191f, 0.084198f, 0.085205f, 0.086213f, 0.087220f, 0.088228f, 0.089236f,	// 0.080
		0.090244f, 0.091252f, 0.092260f, 0.093269f, 0.094278f, 0.095287f, 0.096296f, 0.097305f, 0.098315f, 0.099325f,	// 0.090
		0.100335f, 0.101345f, 0.102355f, 0.103366f, 0.104377f, 0.105388f, 0.106399f, 0.107410f, 0.108422f, 0.109434f,	// 0.100
		0.110446f, 0.111458f, 0.112471f, 0.113483f, 0.114496f, 0.115510f, 0.116523f, 0.117537f, 0.118551f, 0.119565f,	// 0.110
		0.120579f, 0.121594f, 0.122609f, 0.123624f, 0.124639f, 0.125655f, 0.126671f, 0.127687f, 0.128704f, 0.129720f,	// 0.120
		0.130737f, 0.131755f, 0.132772f, 0.133790f, 0.134808f, 0.135826f, 0.136845f, 0.137864f, 0.138883f, 0.139902f,	// 0.130
		0.140922f, 0.141942f, 0.142962f, 0.143983f, 0.145004f, 0.146025f, 0.147046f, 0.148068f, 0.149090f, 0.150113f,	// 0.140
		0.151135f, 0.152158f, 0.153182f, 0.154205f, 0.155229f, 0.156253f, 0.157278f, 0.158303f, 0.159328f, 0.160354f,	// 0.150
		0.161379f, 0.162406f, 0.163432f, 0.164459f, 0.165486f, 0.166514f, 0.167542f, 0.168570f, 0.169599f, 0.170628f,	// 0.160
		0.171657f, 0.172686f, 0.173716f, 0.174747f, 0.175778f, 0.176809f, 0.177840f, 0.178872f, 0.179904f, 0.180937f,	// 0.170
		0.181970f, 0.183003f, 0.184037f, 0.185071f, 0.186105f, 0.187140f, 0.188175f, 0.189211f, 0.190247f, 0.191283f,	// 0.180
		0.192320f, 0.193357f, 0.194395f, 0.195433f, 0.196471f, 0.197510f, 0.198549f, 0.199589f, 0.200629f, 0.201669f,	// 0.190
		0.202710f, 0.203751f, 0.204793f, 0.205835f, 0.206878f, 0.207921f, 0.208964f, 0.210008f, 0.211052f, 0.212097f,	// 0.200
		0.213142f, 0.214188f, 0.215234f, 0.216281f, 0.217328f, 0.218375f, 0.219423f, 0.220472f, 0.221520f, 0.222570f,	// 0.210
		0.223619f, 0.224670f, 0.225720f, 0.226772f, 0.227823f, 0.228875f, 0.229928f, 0.230981f, 0.232035f, 0.233089f,	// 0.220
		0.234143f, 0.235198f, 0.236254f, 0.237310f, 0.238367f, 0.239424f, 0.240481f, 0.241539f, 0.242598f, 0.243657f,	// 0.230
		0.244717f, 0.245777f, 0.246838f, 0.247899f, 0.248960f, 0.250023f, 0.251085f, 0.252149f, 0.253213f, 0.254277f,	// 0.240
		0.255342f, 0.256407f, 0.257473f, 0.258540f, 0.259607f, 0.260675f, 0.261743f, 0.262812f, 0.263881f, 0.264951f,	// 0.250
		0.266022f, 0.267093f, 0.268164f, 0.269236f, 0.270309f, 0.271383f, 0.272456f, 0.273531f, 0.274606f, 0.275682f,	// 0.260
		0.276758f, 0.277835f, 0.278913f, 0.279991f, 0.281069f, 0.282149f, 0.283229f, 0.284309f, 0.285390f, 0.286472f,	// 0.270
		0.287554f, 0.288637f, 0.289721f, 0.290805f, 0.291890f, 0.292976f, 0.294062f, 0.295149f, 0.296236f, 0.297324f,	// 0.280
		0.298413f, 0.299502f, 0.300592f, 0.301683f, 0.302774f, 0.303866f, 0.304959f, 0.306052f, 0.307146f, 0.308241f,	// 0.290
		0.309336f, 0.310432f, 0.311529f, 0.312626f, 0.313724f, 0.314823f, 0.315923f, 0.317023f, 0.318124f, 0.319225f,	// 0.300
		0.320328f, 0.321430f, 0.322534f, 0.323639f, 0.324744f, 0.325849f, 0.326956f, 0.328063f, 0.329171f, 0.330280f,	// 0.310
		0.331389f, 0.332500f, 0.333611f, 0.334722f, 0.335835f, 0.336948f, 0.338062f, 0.339176f, 0.340292f, 0.341408f,	// 0.320
		0.342525f, 0.343643f, 0.344761f, 0.345880f, 0.347000f, 0.348121f, 0.349243f, 0.350365f, 0.351488f, 0.352612f,	// 0.330
		0.353737f, 0.354862f, 0.355989f, 0.357116f, 0.358244f, 0.359373f, 0.360502f, 0.361632f, 0.362764f, 0.363896f,	// 0.340
		0.365029f, 0.366162f, 0.367297f, 0.368432f, 0.369568f, 0.370705f, 0.371843f, 0.372982f, 0.374121f, 0.375262f,	// 0.350
		0.376403f, 0.377545f, 0.378688f, 0.379832f, 0.380977f, 0.382122f, 0.383269f, 0.384416f, 0.385564f, 0.386713f,	// 0.360
		0.387863f, 0.389014f, 0.390166f, 0.391319f, 0.392472f, 0.393627f, 0.394782f, 0.395938f, 0.397096f, 0.398254f,	// 0.370
		0.399413f, 0.400573f, 0.401734f, 0.402896f, 0.404058f, 0.405222f, 0.406387f, 0.407552f, 0.408719f, 0.409886f,	// 0.380
		0.411055f, 0.412224f, 0.413395f, 0.414566f, 0.415739f, 0.416912f, 0.418086f, 0.419261f, 0.420438f, 0.421615f,	// 0.390
		0.422793f, 0.423972f, 0.425153f, 0.426334f, 0.427516f, 0.428700f, 0.429884f, 0.431069f, 0.432255f, 0.433443f,	// 0.400
		0.434631f, 0.435821f, 0.437011f, 0.438203f, 0.439395f, 0.440589f, 0.441783f, 0.442979f, 0.444176f, 0.445374f,	// 0.410
		0.446573f, 0.447773f, 0.448974f, 0.450176f, 0.451379f, 0.452583f, 0.453789f, 0.454995f, 0.456203f, 0.457411f,	// 0.420
		0.458621f, 0.459832f, 0.461044f, 0.462257f, 0.463471f, 0.464687f, 0.465903f, 0.467121f, 0.468340f, 0.469559f,	// 0.430
		0.470781f, 0.472003f, 0.473226f, 0.474451f, 0.475676f, 0.476903f, 0.478131f, 0.479360f, 0.480591f, 0.481822f,	// 0.440
		0.483055f, 0.484289f, 0.485524f, 0.486760f, 0.487998f, 0.489237f, 0.490477f, 0.491718f, 0.492960f, 0.494204f,	// 0.450
		0.495449f, 0.496695f, 0.497942f, 0.499191f, 0.500441f, 0.501692f, 0.502944f, 0.504198f, 0.505452f, 0.506709f,	// 0.460
		0.507966f, 0.509225f, 0.510485f, 0.511746f, 0.513008f, 0.514272f, 0.515537f, 0.516804f, 0.518071f, 0.519341f,	// 0.470
		0.520611f, 0.521883f, 0.523156f, 0.524430f, 0.525706f, 0.526983f, 0.528261f, 0.529541f, 0.530822f, 0.532104f,	// 0.480
		0.533388f, 0.534673f, 0.535960f, 0.537248f, 0.538537f, 0.539828f, 0.541120f, 0.542414f, 0.543708f, 0.545005f,	// 0.490
		0.546302f, 0.547602f, 0.548902f, 0.550204f, 0.551508f, 0.552813f, 0.554119f, 0.555427f, 0.556736f, 0.558047f,	// 0.500
		0.559359f, 0.560672f, 0.561987f, 0.563304f, 0.564622f, 0.565942f, 0.567263f, 0.568585f, 0.569909f, 0.571235f,	// 0.510
		0.572562f, 0.573890f, 0.575221f, 0.576552f, 0.577885f, 0.579220f, 0.580556f, 0.581894f, 0.583234f, 0.584575f,	// 0.520
		0.585917f, 0.587261f, 0.588607f, 0.589954f, 0.591303f, 0.592653f, 0.594005f, 0.595359f, 0.596714f, 0.598071f,	// 0.530
		0.599430f, 0.600790f, 0.602152f, 0.603515f, 0.604880f, 0.606247f, 0.607615f, 0.608985f, 0.610357f, 0.611730f,	// 0.540
		0.613105f, 0.614482f, 0.615860f, 0.617241f, 0.618622f, 0.620006f, 0.621391f, 0.622778f, 0.624167f, 0.625557f,	// 0.550
		0.626950f, 0.628344f, 0.629739f, 0.631137f, 0.632536f, 0.633937f, 0.635340f, 0.636744f, 0.638151f, 0.639559f,	// 0.560
		0.640969f, 0.642380f, 0.643794f, 0.645209f, 0.646626f, 0.648046f, 0.649466f, 0.650889f, 0.652314f, 0.653740f,	// 0.570
		0.655169f, 0.656599f, 0.658031f, 0.659465f, 0.660901f, 0.662338f, 0.663778f, 0.665219f, 0.666663f, 0.668108f,	// 0.580
		0.669556f, 0.671005f, 0.672456f, 0.673909f, 0.675365f, 0.676822f, 0.678281f, 0.679742f, 0.681205f, 0.682670f,	// 0.590
		0.684137f, 0.685606f, 0.687077f, 0.688550f, 0.690025f, 0.691502f, 0.692981f, 0.694463f, 0.695946f, 0.697431f,	// 0.600
		0.698919f, 0.700408f, 0.701900f, 0.703394f, 0.704890f, 0.706387f, 0.707888f, 0.709390f, 0.710894f, 0.712400f,	// 0.610
		0.713909f, 0.715420f, 0.716933f, 0.718448f, 0.719965f, 0.721484f, 0.723006f, 0.724530f, 0.726056f, 0.727584f,	// 0.620
		0.729115f, 0.730648f, 0.732182f, 0.733720f, 0.735259f, 0.736801f, 0.738345f, 0.739891f, 0.741440f, 0.742991f,	// 0.630
		0.744544f, 0.746099f, 0.747657f, 0.749217f, 0.750780f, 0.752345f, 0.753912f, 0.755481f, 0.757053f, 0.758628f,	// 0.640
		0.760204f, 0.761784f, 0.763365f, 0.764949f, 0.766535f, 0.768124f, 0.769715f, 0.771309f, 0.772905f, 0.774504f,	// 0.650
		0.776105f, 0.777709f, 0.779315f, 0.780923f, 0.782534f, 0.784148f, 0.785764f, 0.787383f, 0.789004f, 0.790628f,	// 0.660
		0.792254f, 0.793883f, 0.795515f, 0.797149f, 0.798786f, 0.800425f, 0.802067f, 0.803712f, 0.805359f, 0.807009f,	// 0.670
		0.808661f, 0.810317f, 0.811975f, 0.813635f, 0.815299f, 0.816965f, 0.818634f, 0.820305f, 0.821979f, 0.823656f,	// 0.680
		0.825336f, 0.827019f, 0.828704f, 0.830392f, 0.832083f, 0.833777f, 0.835474f, 0.837173f, 0.838875f, 0.840580f,	// 0.690
		0.842288f, 0.843999f, 0.845713f, 0.847430f, 0.849149f, 0.850872f, 0.852597f, 0.854326f, 0.856057f, 0.857791f,	// 0.700
		0.859529f, 0.861269f, 0.863012f, 0.864759f, 0.866508f, 0.868260f, 0.870016f, 0.871774f, 0.873536f, 0.875300f,	// 0.710
		0.877068f, 0.878839f, 0.880613f, 0.882390f, 0.884170f, 0.885953f, 0.887740f, 0.889529f, 0.891322f, 0.893118f,	// 0.720
		0.894918f, 0.896720f, 0.898526f, 0.900335f, 0.902147f, 0.903962f, 0.905781f, 0.907603f, 0.909429f, 0.911258f,	// 0.730
		0.913090f, 0.914925f, 0.916764f, 0.918606f, 0.920451f, 0.922300f, 0.924153f, 0.926009f, 0.927868f, 0.929730f,	// 0.740
		0.931597f, 0.933466f, 0.935339f, 0.937216f, 0.939096f, 0.940980f, 0.942867f, 0.944758f, 0.946652f, 0.948550f,	// 0.750
		0.950452f, 0.952357f, 0.954265f, 0.956178f, 0.958094f, 0.960014f, 0.961937f, 0.963864f, 0.965795f, 0.967730f,	// 0.760
		0.969668f, 0.971611f, 0.973556f, 0.975506f, 0.977460f, 0.979417f, 0.981378f, 0.983343f, 0.985312f, 0.987285f,	// 0.770
		0.989262f, 0.991242f, 0.993227f, 0.995215f, 0.997208f, 0.999204f, 1.001204f, 1.003209f, 1.005217f, 1.007230f,	// 0.780
		1.009246f, 1.011267f, 1.013292f, 1.015321f, 1.017353f, 1.019390f, 1.021432f, 1.023477f, 1.025527f, 1.027581f,	// 0.790
		1.029639f, 1.031701f, 1.033767f, 1.035838f, 1.037913f, 1.039993f, 1.042077f, 1.044165f, 1.046257f, 1.048354f,	// 0.800
		1.050455f, 1.052561f, 1.054671f, 1.056786f, 1.058905f, 1.061028f, 1.063156f, 1.065289f, 1.067426f, 1.069568f,	// 0.810
		1.071714f, 1.073865f, 1.076020f, 1.078180f, 1.080345f, 1.082515f, 1.084689f, 1.086868f, 1.089051f, 1.091240f,	// 0.820
		1.093433f, 1.095631f, 1.097834f, 1.100042f, 1.102254f, 1.104471f, 1.106694f, 1.108921f, 1.111153f, 1.113390f,	// 0.830
		1.115632f, 1.117880f, 1.120132f, 1.122389f, 1.124651f, 1.126919f, 1.129191f, 1.131469f, 1.133752f, 1.136040f,	// 0.840
		1.138333f, 1.140631f, 1.142935f, 1.145244f, 1.147558f, 1.149878f, 1.152203f, 1.154533f, 1.156868f, 1.159209f,	// 0.850
		1.161556f, 1.163908f, 1.166265f, 1.168628f, 1.170997f, 1.173371f, 1.175750f, 1.178136f, 1.180526f, 1.182923f,	// 0.860
		1.185325f, 1.187733f, 1.190146f, 1.192566f, 1.194991f, 1.197422f, 1.199858f, 1.202301f, 1.204749f, 1.207204f,	// 0.870
		1.209664f, 1.212130f, 1.214603f, 1.217081f, 1.219565f, 1.222056f, 1.224552f, 1.227055f, 1.229564f, 1.232078f,	// 0.880
		1.234600f, 1.237127f, 1.239660f, 1.242201f, 1.244747f, 1.247299f, 1.249858f, 1.252423f, 1.254995f, 1.257574f,	// 0.890
		1.260158f, 1.262750f, 1.265347f, 1.267952f, 1.270563f, 1.273180f, 1.275805f, 1.278436f, 1.281074f, 1.283718f,	// 0.900
		1.286369f, 1.289028f, 1.291693f, 1.294365f, 1.297043f, 1.299729f, 1.302422f, 1.305122f, 1.307829f, 1.310543f,	// 0.910
		1.313264f, 1.315992f, 1.318728f, 1.321470f, 1.324220f, 1.326977f, 1.329742f, 1.332514f, 1.335293f, 1.338080f,	// 0.920
		1.340874f, 1.343676f, 1.346485f, 1.349302f, 1.352126f, 1.354958f, 1.357798f, 1.360645f, 1.363501f, 1.366364f,	// 0.930
		1.369235f, 1.372113f, 1.375000f, 1.377895f, 1.380797f, 1.383708f, 1.386627f, 1.389553f, 1.392488f, 1.395431f,	// 0.940
		1.398383f, 1.401342f, 1.404310f, 1.407287f, 1.410271f, 1.413264f, 1.416266f, 1.419276f, 1.422295f, 1.425322f,	// 0.950
		1.428358f, 1.431402f, 1.434456f, 1.437518f, 1.440588f, 1.443668f, 1.446757f, 1.449854f, 1.452961f, 1.456077f,	// 0.960
		1.459201f, 1.462335f, 1.465478f, 1.468630f, 1.471792f, 1.474963f, 1.478143f, 1.481333f, 1.484532f, 1.487740f,	// 0.970
		1.490958f, 1.494186f, 1.497424f, 1.500671f, 1.503928f, 1.507194f, 1.510471f, 1.513757f, 1.517054f, 1.520360f,	// 0.980
		1.523677f, 1.527004f, 1.530340f, 1.533687f, 1.537045f, 1.540413f, 1.543791f, 1.547179f, 1.550578f, 1.553988f,	// 0.990
		1.557408f, 1.560839f, 1.564281f, 1.567733f, 1.571196f, 1.574670f, 1.578155f, 1.581651f, 1.585158f, 1.588677f,	// 1.000
		1.592206f, 1.595747f, 1.599299f, 1.602862f, 1.606437f, 1.610024f, 1.613622f, 1.617231f, 1.620853f, 1.624486f,	// 1.010
		1.628131f, 1.631787f, 1.635456f, 1.639137f, 1.642830f, 1.646535f, 1.650252f, 1.653981f, 1.657723f, 1.661477f,	// 1.020
		1.665244f, 1.669023f, 1.672816f, 1.676620f, 1.680438f, 1.684268f, 1.688111f, 1.691967f, 1.695837f, 1.699719f,	// 1.030
		1.703615f, 1.707524f, 1.711446f, 1.715382f, 1.719331f, 1.723294f, 1.727271f, 1.731261f, 1.735265f, 1.739283f,	// 1.040
		1.743316f, 1.747362f, 1.751422f, 1.755497f, 1.759586f, 1.763689f, 1.767807f, 1.771939f, 1.776087f, 1.780248f,	// 1.050
		1.784425f, 1.788617f, 1.792823f, 1.797045f, 1.801282f, 1.805534f, 1.809802f, 1.814085f, 1.818384f, 1.822698f,	// 1.060
		1.827028f, 1.831375f, 1.835736f, 1.840114f, 1.844508f, 1.848919f, 1.853346f, 1.857788f, 1.862248f, 1.866724f,	// 1.070
		1.871218f, 1.875728f, 1.880254f, 1.884798f, 1.889360f, 1.893938f, 1.898533f, 1.903146f, 1.907777f, 1.912426f,	// 1.080
		1.917092f, 1.921776f, 1.926478f, 1.931199f, 1.935938f, 1.940695f, 1.945470f, 1.950264f, 1.955077f, 1.959909f,	// 1.090
		1.964760f, 1.969630f, 1.974519f, 1.979427f, 1.984355f, 1.989302f, 1.994270f, 1.999257f, 2.004264f, 2.009291f,	// 1.100
		2.014338f, 2.019406f, 2.024495f, 2.029603f, 2.034733f, 2.039884f, 2.045055f, 2.050249f, 2.055462f, 2.060698f,	// 1.110
		2.065955f, 2.071235f, 2.076536f, 2.081859f, 2.087204f, 2.092571f, 2.097962f, 2.103375f, 2.108810f, 2.114269f,	// 1.120
		2.119750f, 2.125255f, 2.130784f, 2.136336f, 2.141912f, 2.147512f, 2.153135f, 2.158784f, 2.164456f, 2.170153f,	// 1.130
		2.175876f, 2.181622f, 2.187395f, 2.193192f, 2.199015f, 2.204863f, 2.210737f, 2.216638f, 2.222564f, 2.228518f,	// 1.140
		2.234498f, 2.240503f, 2.246537f, 2.252597f, 2.258685f, 2.264801f, 2.270944f, 2.277115f, 2.283315f, 2.289542f,	// 1.150
		2.295799f, 2.302084f, 2.308398f, 2.314742f, 2.321114f, 2.327517f, 2.333949f, 2.340411f, 2.346904f, 2.353427f,	// 1.160
		2.359982f, 2.366566f, 2.373183f, 2.379831f, 2.386510f, 2.393222f, 2.399965f, 2.406741f, 2.413550f, 2.420392f,	// 1.170
		2.427267f, 2.434176f, 2.441117f, 2.448094f, 2.455103f, 2.462149f, 2.469228f, 2.476343f, 2.483493f, 2.490678f,	// 1.180
		2.497900f, 2.505158f, 2.512451f, 2.519783f, 2.527150f, 2.534555f, 2.541999f, 2.549479f, 2.556998f, 2.564555f,	// 1.190
		2.572152f, 2.579788f, 2.587463f, 2.595178f, 2.602933f, 2.610728f, 2.618565f, 2.626442f, 2.634361f, 2.642322f,	// 1.200
		2.650325f, 2.658371f, 2.666459f, 2.674591f, 2.682766f, 2.690985f, 2.699249f, 2.707557f, 2.715911f, 2.724310f,	// 1.210
		2.732754f, 2.741246f, 2.749783f, 2.758369f, 2.767001f, 2.775681f, 2.784410f, 2.793188f, 2.802014f, 2.810890f,	// 1.220
		2.819816f, 2.828793f, 2.837821f, 2.846899f, 2.856031f, 2.865213f, 2.874449f, 2.883739f, 2.893081f, 2.902479f,	// 1.230
		2.911930f, 2.921437f, 2.931001f, 2.940619f, 2.950295f, 2.960027f, 2.969818f, 2.979668f, 2.989575f, 2.999543f,	// 1.240
		3.009570f, 3.019658f, 3.029807f, 3.040017f, 3.050291f, 3.060627f, 3.071026f, 3.081489f, 3.092017f, 3.102611f,	// 1.250
		3.113270f, 3.123995f, 3.134789f, 3.145649f, 3.156579f, 3.167578f, 3.178646f, 3.189786f, 3.200996f, 3.212279f,	// 1.260
		3.223634f, 3.235062f, 3.246566f, 3.258142f, 3.269796f, 3.281527f, 3.293333f, 3.305219f, 3.317184f, 3.329226f,	// 1.270
		3.341351f, 3.353556f, 3.365844f, 3.378215f, 3.390668f, 3.403208f, 3.415832f, 3.428544f, 3.441343f, 3.454230f,	// 1.280
		3.467207f, 3.480273f, 3.493431f, 3.506682f, 3.520025f, 3.533463f, 3.546996f, 3.560626f, 3.574353f, 3.588178f,	// 1.290
		3.602103f, 3.616130f, 3.630256f, 3.644487f, 3.658821f, 3.673261f, 3.687808f, 3.702461f, 3.717225f, 3.732097f,	// 1.300
		3.747082f, 3.762180f, 3.777390f, 3.792717f, 3.808159f, 3.823721f, 3.839403f, 3.855204f, 3.871128f, 3.887175f,	// 1.310
		3.903349f, 3.919649f, 3.936076f, 3.952635f, 3.969325f, 3.986146f, 4.003104f, 4.020196f, 4.037428f, 4.054799f,	// 1.320
		4.072311f, 4.089967f, 4.107766f, 4.125715f, 4.143812f, 4.162057f, 4.180457f, 4.199010f, 4.217721f, 4.236591f,	// 1.330
		4.255619f, 4.274811f, 4.294167f, 4.313691f, 4.333385f, 4.353248f, 4.373287f, 4.393502f, 4.413893f, 4.434467f,	// 1.340
		4.455222f, 4.476166f, 4.497297f, 4.518617f, 4.540133f, 4.561843f, 4.583755f, 4.605868f, 4.628183f, 4.650708f,	// 1.350
		4.673442f, 4.696391f, 4.719557f, 4.742940f, 4.766548f, 4.790380f, 4.814444f, 4.838741f, 4.863272f, 4.888045f,	// 1.360
		4.913061f, 4.938322f, 4.963836f, 4.989602f, 5.015629f, 5.041918f, 5.068471f, 5.095298f, 5.122396f, 5.149777f,	// 1.370
		5.177441f, 5.205389f, 5.233634f, 5.262172f, 5.291015f, 5.320166f, 5.349625f, 5.379404f, 5.409502f, 5.439930f,	// 1.380
		5.470692f, 5.501788f, 5.533232f, 5.565023f, 5.597173f, 5.629684f, 5.662560f, 5.695815f, 5.729450f, 5.763469f,	// 1.390
		5.797887f, 5.832702f, 5.867929f, 5.903573f, 5.939635f, 5.976133f, 6.013065f, 6.050449f, 6.088287f, 6.126585f,	// 1.400
		6.165359f, 6.204610f, 6.244356f, 6.284601f, 6.325351f, 6.366624f, 6.408421f, 6.450762f, 6.493653f, 6.537099f,	// 1.410
		6.581123f, 6.625730f, 6.670926f, 6.716735f, 6.763158f, 6.810219f, 6.857925f, 6.906284f, 6.955322f, 7.005041f,	// 1.420
		7.055467f, 7.106610f, 7.158479f, 7.211102f, 7.264483f, 7.318652f, 7.373619f, 7.429397f, 7.486016f, 7.543482f,	// 1.430
		7.601829f, 7.661070f, 7.721219f, 7.782311f, 7.844361f, 7.907385f, 7.971421f, 8.036478f, 8.102598f, 8.169797f,	// 1.440
		8.238096f, 8.307538f, 8.378134f, 8.449932f, 8.522954f, 8.597222f, 8.672788f, 8.749666f, 8.827911f, 8.907550f,	// 1.450
		8.988611f, 9.071152f, 9.155194f, 9.240799f, 9.328001f, 9.416835f, 9.507369f, 9.599640f, 9.693690f, 9.789591f,	// 1.460
		9.887378f, 9.987129f, 10.088893f, 10.192719f, 10.298695f, 10.406865f, 10.517322f, 10.630128f, 10.745344f, 10.863077f,	// 1.470
		10.983382f, 11.106373f, 11.232127f, 11.360723f, 11.492290f, 11.626900f, 11.764693f, 11.905768f, 12.050224f, 12.198223f,	// 1.480
		12.349876f, 12.505304f, 12.664687f, 12.828140f, 12.995860f, 13.167998f, 13.344708f, 13.526220f, 13.712688f, 13.904363f,	// 1.490
		14.101444f, 14.304138f, 14.512740f, 14.727461f, 14.948626f, 15.176505f, 15.411379f, 15.653634f, 15.903565f, 16.161604f,	// 1.500
		16.428121f, 16.703509f, 16.988286f, 17.282907f, 17.587854f, 17.903758f, 18.231140f, 18.570722f, 18.923155f, 19.289140f,	// 1.510
		19.669567f, 20.065212f, 20.477109f, 20.906228f, 21.353620f, 21.820590f, 22.308336f, 22.818401f, 23.352288f, 23.911639f,	// 1.520
		24.498465f, 25.114692f, 25.762746f, 26.445076f, 27.164388f, 27.923961f, 28.727077f, 29.577801f, 30.480386f, 31.439608f,	// 1.530
		32.461224f, 33.551395f, 34.717119f, 35.966843f, 37.309656f, 38.756725f, 40.320488f, 42.015449f, 43.859304f, 45.872077f,	// 1.540
		48.078648f, 50.508125f, 53.195748f, 56.185731f, 59.531299f, 63.300859f, 67.579928f, 72.478678f, 78.143696f, 84.768329f,	// 1.550
		92.621028f, 102.076945f, 113.681353f, 128.264298f, 147.138847f, 172.522825f, 208.495803f, 263.413436f, 357.621063f, 556.729700f,	// 1.560
		1255.848311f, -4907.436183f		// 1.570
	};

	if (f > A_PI || f < 0.0f) {
		int quotient = (int) (f / A_PI);

		if (f < 0.0f)
			quotient --;

		f -= quotient * A_PI;	// f becomes the remainder
	}

	int index;
	if (f < A_PI_2)
	{
		index = (int) (f * 1000.0f);
		return tan_table[index];
	}
	else
	{
		f = A_PI - f;
		index = (int) (f * 1000.0f);
		return -tan_table[index];
	}
}


float mtAtan(float f)
{
	static const float atan_table[] = 
	{
		0.000000f, 0.001000f, 0.002000f, 0.003000f, 0.004000f, 0.005000f, 0.006000f, 0.007000f, 0.008000f, 0.009000f, // 0.00
		0.010000f, 0.011000f, 0.011999f, 0.012999f, 0.013999f, 0.014999f, 0.015999f, 0.016998f, 0.017998f, 0.018998f, // 0.01
		0.019997f, 0.020997f, 0.021996f, 0.022996f, 0.023995f, 0.024995f, 0.025994f, 0.026993f, 0.027993f, 0.028992f, // 0.02
		0.029991f, 0.030990f, 0.031989f, 0.032988f, 0.033987f, 0.034986f, 0.035984f, 0.036983f, 0.037982f, 0.038980f, // 0.03
		0.039979f, 0.040977f, 0.041975f, 0.042974f, 0.043972f, 0.044970f, 0.045968f, 0.046965f, 0.047963f, 0.048961f, // 0.04
		0.049958f, 0.050956f, 0.051953f, 0.052950f, 0.053948f, 0.054945f, 0.055942f, 0.056938f, 0.057935f, 0.058932f, // 0.05
		0.059928f, 0.060925f, 0.061921f, 0.062917f, 0.063913f, 0.064909f, 0.065904f, 0.066900f, 0.067895f, 0.068891f, // 0.06
		0.069886f, 0.070881f, 0.071876f, 0.072871f, 0.073865f, 0.074860f, 0.075854f, 0.076848f, 0.077842f, 0.078836f, // 0.07
		0.079830f, 0.080824f, 0.081817f, 0.082810f, 0.083803f, 0.084796f, 0.085789f, 0.086781f, 0.087774f, 0.088766f, // 0.08
		0.089758f, 0.090750f, 0.091742f, 0.092733f, 0.093725f, 0.094716f, 0.095707f, 0.096697f, 0.097688f, 0.098678f, // 0.09
		0.099669f, 0.100659f, 0.101648f, 0.102638f, 0.103627f, 0.104617f, 0.105606f, 0.106594f, 0.107583f, 0.108571f, // 0.10
		0.109560f, 0.110547f, 0.111535f, 0.112523f, 0.113510f, 0.114497f, 0.115484f, 0.116470f, 0.117457f, 0.118443f, // 0.11
		0.119429f, 0.120415f, 0.121400f, 0.122385f, 0.123370f, 0.124355f, 0.125339f, 0.126324f, 0.127308f, 0.128292f, // 0.12
		0.129275f, 0.130258f, 0.131241f, 0.132224f, 0.133207f, 0.134189f, 0.135171f, 0.136152f, 0.137134f, 0.138115f, // 0.13
		0.139096f, 0.140077f, 0.141057f, 0.142037f, 0.143017f, 0.143996f, 0.144976f, 0.145955f, 0.146933f, 0.147912f, // 0.14
		0.148890f, 0.149868f, 0.150845f, 0.151823f, 0.152800f, 0.153776f, 0.154753f, 0.155729f, 0.156705f, 0.157680f, // 0.15
		0.158655f, 0.159630f, 0.160605f, 0.161579f, 0.162553f, 0.163527f, 0.164500f, 0.165473f, 0.166446f, 0.167418f, // 0.16
		0.168390f, 0.169362f, 0.170333f, 0.171304f, 0.172275f, 0.173246f, 0.174216f, 0.175186f, 0.176155f, 0.177124f, // 0.17
		0.178093f, 0.179061f, 0.180030f, 0.180997f, 0.181965f, 0.182932f, 0.183899f, 0.184865f, 0.185831f, 0.186797f, // 0.18
		0.187762f, 0.188727f, 0.189692f, 0.190656f, 0.191620f, 0.192583f, 0.193546f, 0.194509f, 0.195472f, 0.196434f, // 0.19
		0.197396f, 0.198357f, 0.199318f, 0.200279f, 0.201239f, 0.202199f, 0.203158f, 0.204117f, 0.205076f, 0.206034f, // 0.20
		0.206992f, 0.207950f, 0.208907f, 0.209864f, 0.210820f, 0.211776f, 0.212732f, 0.213687f, 0.214642f, 0.215596f, // 0.21
		0.216550f, 0.217504f, 0.218457f, 0.219410f, 0.220362f, 0.221314f, 0.222266f, 0.223217f, 0.224168f, 0.225118f, // 0.22
		0.226068f, 0.227018f, 0.227967f, 0.228916f, 0.229864f, 0.230812f, 0.231759f, 0.232706f, 0.233653f, 0.234599f, // 0.23
		0.235545f, 0.236490f, 0.237435f, 0.238380f, 0.239324f, 0.240267f, 0.241210f, 0.242153f, 0.243095f, 0.244037f, // 0.24
		0.244979f, 0.245920f, 0.246860f, 0.247800f, 0.248740f, 0.249679f, 0.250618f, 0.251556f, 0.252494f, 0.253431f, // 0.25
		0.254368f, 0.255305f, 0.256241f, 0.257176f, 0.258111f, 0.259046f, 0.259980f, 0.260914f, 0.261847f, 0.262780f, // 0.26
		0.263712f, 0.264644f, 0.265575f, 0.266506f, 0.267436f, 0.268366f, 0.269296f, 0.270225f, 0.271153f, 0.272081f, // 0.27
		0.273009f, 0.273936f, 0.274862f, 0.275788f, 0.276714f, 0.277639f, 0.278564f, 0.279488f, 0.280412f, 0.281335f, // 0.28
		0.282257f, 0.283180f, 0.284101f, 0.285022f, 0.285943f, 0.286863f, 0.287783f, 0.288702f, 0.289621f, 0.290539f, // 0.29
		0.291457f, 0.292374f, 0.293291f, 0.294207f, 0.295122f, 0.296038f, 0.296952f, 0.297866f, 0.298780f, 0.299693f, // 0.30
		0.300606f, 0.301518f, 0.302429f, 0.303340f, 0.304251f, 0.305161f, 0.306070f, 0.306979f, 0.307888f, 0.308796f, // 0.31
		0.309703f, 0.310610f, 0.311516f, 0.312422f, 0.313327f, 0.314232f, 0.315136f, 0.316040f, 0.316943f, 0.317846f, // 0.32
		0.318748f, 0.319649f, 0.320550f, 0.321451f, 0.322350f, 0.323250f, 0.324149f, 0.325047f, 0.325945f, 0.326842f, // 0.33
		0.327739f, 0.328635f, 0.329530f, 0.330425f, 0.331320f, 0.332214f, 0.333107f, 0.334000f, 0.334892f, 0.335784f, // 0.34
		0.336675f, 0.337565f, 0.338455f, 0.339345f, 0.340234f, 0.341122f, 0.342010f, 0.342897f, 0.343784f, 0.344670f, // 0.35
		0.345556f, 0.346441f, 0.347325f, 0.348209f, 0.349092f, 0.349975f, 0.350857f, 0.351739f, 0.352620f, 0.353500f, // 0.36
		0.354380f, 0.355259f, 0.356138f, 0.357016f, 0.357894f, 0.358771f, 0.359647f, 0.360523f, 0.361398f, 0.362273f, // 0.37
		0.363147f, 0.364021f, 0.364894f, 0.365766f, 0.366638f, 0.367509f, 0.368379f, 0.369250f, 0.370119f, 0.370988f, // 0.38
		0.371856f, 0.372724f, 0.373591f, 0.374457f, 0.375323f, 0.376189f, 0.377053f, 0.377918f, 0.378781f, 0.379644f, // 0.39
		0.380506f, 0.381368f, 0.382229f, 0.383090f, 0.383950f, 0.384809f, 0.385668f, 0.386526f, 0.387384f, 0.388241f, // 0.40
		0.389097f, 0.389953f, 0.390808f, 0.391663f, 0.392517f, 0.393370f, 0.394223f, 0.395075f, 0.395927f, 0.396778f, // 0.41
		0.397628f, 0.398478f, 0.399327f, 0.400175f, 0.401023f, 0.401871f, 0.402717f, 0.403563f, 0.404409f, 0.405254f, // 0.42
		0.406098f, 0.406942f, 0.407785f, 0.408627f, 0.409469f, 0.410310f, 0.411151f, 0.411991f, 0.412830f, 0.413669f, // 0.43
		0.414507f, 0.415344f, 0.416181f, 0.417018f, 0.417853f, 0.418688f, 0.419523f, 0.420356f, 0.421189f, 0.422022f, // 0.44
		0.422854f, 0.423685f, 0.424516f, 0.425346f, 0.426175f, 0.427004f, 0.427832f, 0.428660f, 0.429487f, 0.430313f, // 0.45
		0.431139f, 0.431964f, 0.432788f, 0.433612f, 0.434435f, 0.435258f, 0.436080f, 0.436901f, 0.437722f, 0.438542f, // 0.46
		0.439361f, 0.440180f, 0.440998f, 0.441815f, 0.442632f, 0.443448f, 0.444264f, 0.445079f, 0.445893f, 0.446707f, // 0.47
		0.447520f, 0.448332f, 0.449144f, 0.449955f, 0.450766f, 0.451576f, 0.452385f, 0.453194f, 0.454002f, 0.454809f, // 0.48
		0.455616f, 0.456422f, 0.457227f, 0.458032f, 0.458836f, 0.459640f, 0.460443f, 0.461245f, 0.462046f, 0.462847f, // 0.49
		0.463648f, 0.464447f, 0.465246f, 0.466045f, 0.466842f, 0.467640f, 0.468436f, 0.469232f, 0.470027f, 0.470822f, // 0.50
		0.471616f, 0.472409f, 0.473201f, 0.473993f, 0.474785f, 0.475576f, 0.476366f, 0.477155f, 0.477944f, 0.478732f, // 0.51
		0.479519f, 0.480306f, 0.481092f, 0.481878f, 0.482663f, 0.483447f, 0.484231f, 0.485014f, 0.485796f, 0.486578f, // 0.52
		0.487359f, 0.488139f, 0.488919f, 0.489698f, 0.490476f, 0.491254f, 0.492031f, 0.492808f, 0.493584f, 0.494359f, // 0.53
		0.495133f, 0.495907f, 0.496680f, 0.497453f, 0.498225f, 0.498996f, 0.499767f, 0.500537f, 0.501306f, 0.502075f, // 0.54
		0.502843f, 0.503611f, 0.504377f, 0.505144f, 0.505909f, 0.506674f, 0.507438f, 0.508202f, 0.508965f, 0.509727f, // 0.55
		0.510488f, 0.511249f, 0.512010f, 0.512769f, 0.513528f, 0.514287f, 0.515044f, 0.515801f, 0.516558f, 0.517313f, // 0.56
		0.518069f, 0.518823f, 0.519577f, 0.520330f, 0.521082f, 0.521834f, 0.522586f, 0.523336f, 0.524086f, 0.524835f, // 0.57
		0.525584f, 0.526332f, 0.527079f, 0.527826f, 0.528572f, 0.529317f, 0.530062f, 0.530806f, 0.531549f, 0.532292f, // 0.58
		0.533034f, 0.533776f, 0.534516f, 0.535257f, 0.535996f, 0.536735f, 0.537473f, 0.538211f, 0.538948f, 0.539684f, // 0.59
		0.540420f, 0.541154f, 0.541889f, 0.542622f, 0.543356f, 0.544088f, 0.544820f, 0.545551f, 0.546281f, 0.547011f, // 0.60
		0.547740f, 0.548469f, 0.549196f, 0.549924f, 0.550650f, 0.551376f, 0.552101f, 0.552826f, 0.553550f, 0.554273f, // 0.61
		0.554996f, 0.555718f, 0.556439f, 0.557160f, 0.557880f, 0.558599f, 0.559318f, 0.560036f, 0.560754f, 0.561471f, // 0.62
		0.562187f, 0.562902f, 0.563617f, 0.564331f, 0.565045f, 0.565758f, 0.566470f, 0.567182f, 0.567893f, 0.568603f, // 0.63
		0.569313f, 0.570022f, 0.570731f, 0.571439f, 0.572146f, 0.572852f, 0.573558f, 0.574263f, 0.574968f, 0.575672f, // 0.64
		0.576375f, 0.577078f, 0.577780f, 0.578481f, 0.579182f, 0.579882f, 0.580582f, 0.581280f, 0.581979f, 0.582676f, // 0.65
		0.583373f, 0.584069f, 0.584765f, 0.585460f, 0.586154f, 0.586848f, 0.587541f, 0.588233f, 0.588925f, 0.589616f, // 0.66
		0.590307f, 0.590997f, 0.591686f, 0.592374f, 0.593062f, 0.593750f, 0.594436f, 0.595122f, 0.595808f, 0.596493f, // 0.67
		0.597177f, 0.597860f, 0.598543f, 0.599225f, 0.599907f, 0.600588f, 0.601268f, 0.601948f, 0.602627f, 0.603305f, // 0.68
		0.603983f, 0.604660f, 0.605337f, 0.606013f, 0.606688f, 0.607362f, 0.608036f, 0.608710f, 0.609382f, 0.610055f, // 0.69
		0.610726f, 0.611397f, 0.612067f, 0.612737f, 0.613406f, 0.614074f, 0.614741f, 0.615409f, 0.616075f, 0.616741f, // 0.70
		0.617406f, 0.618070f, 0.618734f, 0.619398f, 0.620060f, 0.620722f, 0.621384f, 0.622044f, 0.622705f, 0.623364f, // 0.71
		0.624023f, 0.624681f, 0.625339f, 0.625996f, 0.626652f, 0.627308f, 0.627963f, 0.628618f, 0.629272f, 0.629925f, // 0.72
		0.630578f, 0.631230f, 0.631881f, 0.632532f, 0.633182f, 0.633832f, 0.634481f, 0.635129f, 0.635777f, 0.636424f, // 0.73
		0.637070f, 0.637716f, 0.638361f, 0.639006f, 0.639650f, 0.640293f, 0.640936f, 0.641578f, 0.642220f, 0.642861f, // 0.74
		0.643501f, 0.644141f, 0.644780f, 0.645418f, 0.646056f, 0.646693f, 0.647330f, 0.647966f, 0.648601f, 0.649236f, // 0.75
		0.649870f, 0.650504f, 0.651137f, 0.651769f, 0.652401f, 0.653032f, 0.653663f, 0.654293f, 0.654922f, 0.655551f, // 0.76
		0.656179f, 0.656806f, 0.657433f, 0.658059f, 0.658685f, 0.659310f, 0.659935f, 0.660558f, 0.661182f, 0.661804f, // 0.77
		0.662426f, 0.663048f, 0.663669f, 0.664289f, 0.664908f, 0.665527f, 0.666146f, 0.666764f, 0.667381f, 0.667998f, // 0.78
		0.668614f, 0.669229f, 0.669844f, 0.670458f, 0.671072f, 0.671685f, 0.672297f, 0.672909f, 0.673520f, 0.674131f, // 0.79
		0.674741f, 0.675350f, 0.675959f, 0.676568f, 0.677175f, 0.677782f, 0.678389f, 0.678995f, 0.679600f, 0.680205f, // 0.80
		0.680809f, 0.681412f, 0.682015f, 0.682618f, 0.683219f, 0.683821f, 0.684421f, 0.685021f, 0.685621f, 0.686219f, // 0.81
		0.686818f, 0.687415f, 0.688012f, 0.688609f, 0.689205f, 0.689800f, 0.690395f, 0.690989f, 0.691583f, 0.692175f, // 0.82
		0.692768f, 0.693360f, 0.693951f, 0.694542f, 0.695132f, 0.695721f, 0.696310f, 0.696898f, 0.697486f, 0.698073f, // 0.83
		0.698660f, 0.699246f, 0.699831f, 0.700416f, 0.701000f, 0.701584f, 0.702167f, 0.702750f, 0.703332f, 0.703913f, // 0.84
		0.704494f, 0.705074f, 0.705654f, 0.706233f, 0.706812f, 0.707390f, 0.707967f, 0.708544f, 0.709120f, 0.709696f, // 0.85
		0.710271f, 0.710846f, 0.711420f, 0.711993f, 0.712566f, 0.713138f, 0.713710f, 0.714281f, 0.714852f, 0.715422f, // 0.86
		0.715991f, 0.716560f, 0.717128f, 0.717696f, 0.718263f, 0.718830f, 0.719396f, 0.719962f, 0.720527f, 0.721091f, // 0.87
		0.721655f, 0.722218f, 0.722781f, 0.723343f, 0.723905f, 0.724466f, 0.725026f, 0.725586f, 0.726146f, 0.726704f, // 0.88
		0.727263f, 0.727820f, 0.728378f, 0.728934f, 0.729490f, 0.730046f, 0.730601f, 0.731155f, 0.731709f, 0.732262f, // 0.89
		0.732815f, 0.733367f, 0.733919f, 0.734470f, 0.735021f, 0.735571f, 0.736120f, 0.736669f, 0.737217f, 0.737765f, // 0.90
		0.738313f, 0.738859f, 0.739406f, 0.739951f, 0.740496f, 0.741041f, 0.741585f, 0.742128f, 0.742671f, 0.743214f, // 0.91
		0.743756f, 0.744297f, 0.744838f, 0.745378f, 0.745918f, 0.746457f, 0.746995f, 0.747534f, 0.748071f, 0.748608f, // 0.92
		0.749145f, 0.749681f, 0.750216f, 0.750751f, 0.751285f, 0.751819f, 0.752352f, 0.752885f, 0.753417f, 0.753949f, // 0.93
		0.754480f, 0.755011f, 0.755541f, 0.756071f, 0.756600f, 0.757128f, 0.757656f, 0.758184f, 0.758710f, 0.759237f, // 0.94
		0.759763f, 0.760288f, 0.760813f, 0.761337f, 0.761861f, 0.762384f, 0.762907f, 0.763429f, 0.763951f, 0.764472f, // 0.95
		0.764993f, 0.765513f, 0.766033f, 0.766552f, 0.767070f, 0.767588f, 0.768106f, 0.768623f, 0.769139f, 0.769655f, // 0.96
		0.770171f, 0.770686f, 0.771200f, 0.771714f, 0.772228f, 0.772741f, 0.773253f, 0.773765f, 0.774276f, 0.774787f, // 0.97
		0.775298f, 0.775807f, 0.776317f, 0.776826f, 0.777334f, 0.777842f, 0.778349f, 0.778856f, 0.779362f, 0.779868f, // 0.98
		0.780373f, 0.780878f, 0.781382f, 0.781886f, 0.782389f, 0.782892f, 0.783394f, 0.783896f, 0.784397f, 0.784898f, // 0.99
		0.785398f
	};

	if (f <= -1.0f)
	{
		int index = (int) (-1000.0f / f);
		return atan_table[index] - A_PI_2;
	}
	else if (f < 0.0f)
	{
		int index = (int) (f * (-1000.0f));
		return -1.0f * atan_table[index];
	}
	else if (f < 1.0f)
	{
		int index = (int) (f * 1000.0f);
		return atan_table[index];
	}
	else
	{
		int index = (int) (1000.0f / f);
		return A_PI_2 - atan_table[index];
	}
}

void mtSetVertex(VERTEX *pVertex, float x, float y, float z, float u, float v )  
{
	pVertex->vertex.x=x;
	pVertex->vertex.y=y;
	pVertex->vertex.z=z;	
	pVertex->uv.x=u;
	pVertex->uv.y=v;
}
void mtSetPoint3D(POINT3D *pPoint, float x, float y, float z )
{
	pPoint->x=x;
	pPoint->y=y;
	pPoint->z=z;
}


void mtSetPoint2D(POINT2D *pPoint, float x, float y )
{
	pPoint->x=x;
	pPoint->y=y;
}

RECT3D mtComputeExtent(POINT3D *points, int num_point)
{
	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;
	
	min_x=max_x=points[0].x;
	min_y=max_y=points[0].y;
	min_z=max_z=points[0].z;

	for(int i=1;i<num_point;i++)
	{
		if(min_x>points[i].x) min_x = points[i].x;
		else if(max_x<points[i].x) max_x = points[i].x;

		if(min_y>points[i].y) min_y = points[i].y;
		else if(max_y<points[i].y) max_y = points[i].y;

		if(min_z>points[i].z) min_z = points[i].z;
		else if(max_z<points[i].z) max_z = points[i].z;
	}

	RECT3D boundingbox;
	boundingbox.set(
		min_x, max_x,
		min_y, max_y,
		min_z, max_z);

	return boundingbox;
}



unsigned int getHashCode(const char * szText)
{

	unsigned int hash = 5381;
	int c;

	while (c = *szText++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}