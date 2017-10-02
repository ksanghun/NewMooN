#pragma once

#include "mupdf/pdf.h" /* for pdf specifics and forms */
#include "mupdf/fitz.h"
#include "mupdf/ucdn.h"

#include "MNCVMng.h"
class CMNPDFConverter
{
public:
	CMNPDFConverter();
	~CMNPDFConverter();


	bool LoadPDF(CString strpath, cv::Mat& pimg, bool IsGray);
};

