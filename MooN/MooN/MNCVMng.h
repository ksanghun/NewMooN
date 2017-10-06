#pragma once

#include "MNSingleton.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

enum _LANGUAGE_TYPE { _ALPHABETIC, _NONALPHABETIC };
enum _ALIGHN_TYPE { _UNKNOWN_ALIGN, _HORIZON_ALIGN, _VERTICAL_ALIGN };
#define _MAX_EXTRACT_ITERATION 20

class CMNCVMng
{
public:
	CMNCVMng();
	~CMNCVMng();
};

typedef CMNSingleton<CMNCVMng> SINGLETON_CVMng;