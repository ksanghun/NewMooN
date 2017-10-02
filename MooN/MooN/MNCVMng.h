#pragma once

#include "MNSingleton.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class CMNCVMng
{
public:
	CMNCVMng();
	~CMNCVMng();
};

typedef CMNSingleton<CMNCVMng> SINGLETON_CVMng;