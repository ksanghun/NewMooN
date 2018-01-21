#pragma once

#include "MNSingleton.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define _NUM_LANGUAGE_TYPE 4
enum _LANGUAGE_TYPE { __ENG = 0, __CHI, __KOR, __JAP, __LANG_NONE, __CNS };
enum _ALIGHN_TYPE { _HORIZON_ALIGN, _VERTICAL_ALIGN, _UNKNOWN_ALIGN};
#define _MAX_EXTRACT_ITERATION 20



class CMNCVMng
{
public:
	CMNCVMng();
	~CMNCVMng();
};

typedef CMNSingleton<CMNCVMng> SINGLETON_CVMng;