#pragma once

#include "MNSingleton.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

enum _LANGUAGE_TYPE { __ENG, __CHI,__KOR, _NONE, _CNS };
enum _ALIGHN_TYPE { _HORIZON_ALIGN, _VERTICAL_ALIGN, _UNKNOWN_ALIGN};
#define _MAX_EXTRACT_ITERATION 20


class CMNCVMng
{
public:
	CMNCVMng();
	~CMNCVMng();
};

typedef CMNSingleton<CMNCVMng> SINGLETON_CVMng;