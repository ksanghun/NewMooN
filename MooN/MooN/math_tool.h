#ifndef MATH_TOOL_H
#define MATH_TOOL_H

#include "data_type.h"
#include <math.h>

// Fast Math-Function=========================
float mtSqrt(float f);
float mtSin(float f);
float mtCon(float f);
float mtTan(float f);
float mtATan(float f);
double mtSqrt(double f);

//POINT2D & Scalar operation =========================
POINT2D operator * (const POINT2D A, const float value);
POINT2D operator / (const POINT2D A, const float value);

//POINT2D & POINT2D operation =========================
POINT2D operator - (const POINT2D A, const POINT2D B);
POINT2D mtNormalize(POINT2D A);
float mtDot(POINT2D A, POINT2D B);
float mtNorm(POINT2D A);

//POINT3D& Scalar operation ==========================
POINT3D operator + (const POINT3D A, const float value);
POINT3D operator - (const POINT3D A, const float value);
POINT3D operator * (const POINT3D A, const float value);
POINT3D operator / (const POINT3D A, const float value);
//====================================================

//POINT3D & POINT3D operation ==========================
POINT3D operator + (const POINT3D A, const POINT3D B);
POINT3D operator - (const POINT3D A, const POINT3D B);
POINT3D operator * (const POINT3D A, const POINT3D B);
POINT3D operator / (const POINT3D A, const POINT3D B);
POINT3D	mtCross(POINT3D A, POINT3D B);
float	mtDot(POINT3D A, POINT3D B);
float	mtDot(PLANE3D P, POINT3D V);
POINT3D	mtNormalize(POINT3D A);
float   mtNorm(POINT3D A);
float   mtDistance(POINT3D A, POINT3D B);
POINT3D mtNormalizeMag(POINT3D A);
POINT3D mtLineCross(POINT3D A, POINT3D B);
float	mtAngle(POINT3D A, POINT3D B);
float	mtDotXZ(POINT3D A, POINT3D B);
float   mtDistanceXY(POINT3D A, POINT3D B);
float	mtManhattanDistanceXZ(POINT3D A, POINT3D B);
//====================================================

//POINT3D & Matrix operation(Daniel) ==================
void mtMultMatrixVecf(const float matrix[16], const float in[4], float out[4]);
void mtMultMatrixVecf(const double matrix[16], const double in[4], double out[4]);
//====================================================

//Matrix & Matrix operation ==================
void mtMultMatrix(const float matrixA[16], const float matrixB[16], float matrixOut[16]);
void mtMultMatrix(const double matrixA[16], const double matrixB[16], double matrixOut[16]);
//====================================================

//Maximum & Minimum ==================================
template <class T> const T& minimum(const T& a, const T& b);
template <class T> const T& maximum(const T& a, const T& b);
//====================================================

// RECT3D Fuction ========================
POINT3D mtRect3DSize(RECT3D rect);
POINT3D mtRect3DMinPoint(RECT3D rect);
POINT3D mtRect3DCenter(RECT3D rect);

RECT3D operator + (const RECT3D Rect, const POINT3D Vec);
RECT3D operator - (const RECT3D Rect, const POINT3D Vec);
RECT3D operator * (const RECT3D Rect, const POINT3D Vec);
RECT3D operator / (const RECT3D Rect, const POINT3D Vec);
RECT3D operator * (const RECT3D Rect, const float value);
RECT3D operator / (const RECT3D Rect, const float value);

// ========================================

// Intersection Test==================================
BOOL mtInterSection(RECT3D rect1, RECT3D rect2);
BOOL mtInterSection(RECT3D rect, PLANE3D plane);
BOOL mtInterSection(POINT3D point, PLANE3D plane);
BOOL mtInterSection(FRUSTUM Frustum, RECT3D rect);
BOOL mtInterSection(FRUSTUM Frustum, POINT3D point);
BOOL mtInterSection(RECT3D rect, POINT3D point); // Daniel 20061228 : TEXT(GDI) ¿µ¿ª culling ¿ë
BOOL mtInterSection(RECT3D rect, POINT3D* pEN);
// ===================================================

// ==============================
BOOL mtPointInPolygon(POINT3D point, POINT3D* vertex, int num_vertex);
// ==============================

// QuadTree function ============
int mtRealToInt32_floor(float input);
template <class T> inline T clamp(T value, T low, T high);
template<class T> inline int highestBitSet(T input);
// =====================


// Vertex Function ============================
void mtSetVertex(VERTEX *pVertex, float x, float y, float z, float u, float v) ;
void mtSetPoint3D(POINT3D *pPoint, float x, float y, float z );

void mtSetPoint2D(POINT2D *pPoint, float x, float y);
RECT3D mtComputeExtent(POINT3D *points, int num_point);
// =====================

// Math Fuction =================================
float mtSqrt(float f);
float mtSin(float f);
float mtCos(float f);
float mtTan(float f);
float mtAtan(float f);
//==============================================

unsigned int getHashCode(const char * szText);

/*	
-----------------------------------------------------------------
    
	Floating Point Macros
    
-----------------------------------------------------------------
*/
// reinterpret a float as an int32
#define fpBits(f) (*reinterpret_cast<const UINT*>(&(f))) 

// reinterpret an int32 as a float
#define intBits(i) (*reinterpret_cast<const float*>(&(i))) 

// return 0 or -1 based on the sign of the float
#define fpSign(f) (fpBits(f)>>31) 

// extract the 8 bits of exponent as a signed integer
// by masking out this bits, shifting down by 23,
// and subtracting the bias value of 127
#define fpExponent(f) (((fpBits(f)&0x7fffffff)>>23)-127) 

// return 0 or -1 based on the sign of the exponent
#define fpExponentSign(f) (fpExponent(f)>>31) 

// get the 23 bits of mantissa without the implied bit
#define fpPureMantissa(f) ((fpBits(f)&0x7fffff)) 

// get the 23 bits of mantissa with the implied bit replaced
#define fpMantissa(f) (fpPureMantissa(f) | (1<<23)) 

#define fpOneBits 0x3F800000

// flipSign is a helper Macro to
// invert the sign of i if flip equals -1, 
// if flip equals 0, it does nothing
#define flipSign(i, flip) ((i^ flip) - flip)


//:	realToInt32_floor
//----------------------------------------------------------------------------------------
//
//	Convert a float value to the next-lowest int32 value.
//	realToInt32_chop(2.35) = 2; realToInt32_chop(-2.35) = -3
//
//-------------------------------------------------------------------------------------://

inline int mtRealToInt32_floor(float input)
{ 
	// read the exponent and decide how much we need to shift the mantissa down
	int shift = 23-fpExponent(input);
	// read the mantissa and shift it down to remove all fractional values
	int result = fpMantissa(input)>>shift;
	// set the sign of the new result
	result = flipSign(result, fpSign(input));
	// if the exponent was negative, (-1<input<1) we must return zero
	result &= ~fpExponentSign(input);

	// if the original value is negative, and any fractional values are present,
	// decrement the result by one
	result -= fpSign(input) && (fpExponentSign(input) || (fpPureMantissa(input) & ((1<<shift)-1)));

	// return the result
	return result;                  
}

//:	clamp
//----------------------------------------------------------------------------------------
//
//	Returns value clamped between low and high so that (low <= value <= high)
//
//-------------------------------------------------------------------------------------://
template <class T> 
inline T clamp(T value, T low, T high)
{
	if(value < low)
	{
		return low;
	}

	if(value > high)
	{
		return high;
	} 

	return value;
}

//:	highestBitSet
//----------------------------------------------------------------------------------------
//
//	Returns the index of the highest bit set in the input value.
//
//-------------------------------------------------------------------------------------://
template<class T>
inline int highestBitSet(T input)
{
#ifdef _WIN32_WCE
	if (input & 128) 
		return 7;

	if (input & 64) 
		return 6;

	if (input & 32) 
		return 5;

	if (input & 16) 
		return 4;

	if (input & 8) 
		return 3;

	if (input & 4) 
		return 2;

	if (input & 2) 
		return 1;

	if (input & 1) 
		return 0;

	return 0;
#else

	register int result;
	//    assert(input); // zero is invalid input!
	//   assert(sizeof(T)==4); // 32bit data only!
	_asm bsr eax, input
		_asm mov result, eax

		return result;
#endif


}

template <class T> 
inline const T& maximum(const T& a, const T& b)
{
	return a > b ? a : b;
}

template <class T> 
inline const T& minimum(const T& a, const T& b)
{
	return a < b ? a : b;
}


#endif