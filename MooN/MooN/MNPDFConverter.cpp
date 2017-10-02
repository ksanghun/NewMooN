#include "stdafx.h"
#include "MNPDFConverter.h"


CMNPDFConverter::CMNPDFConverter()
{
}


CMNPDFConverter::~CMNPDFConverter()
{
}

bool CMNPDFConverter::LoadPDF(CString strpath, cv::Mat& pimg, bool IsGray)
{
	//fz_annot *annot;
	USES_CONVERSION;
	char* sz = T2A(strpath);

	fz_pixmap *pix = NULL;
	pdf_document *pdf = NULL;
	fz_page *page = NULL;
	fz_document *doc = NULL;
	fz_context *ctx = fz_new_context(NULL, NULL, 0);

	int currentpage = 0;

	fz_register_document_handlers(ctx);
	doc = fz_open_document(ctx, sz);
	pdf = pdf_specifics(ctx, doc);
	fz_matrix page_ctm;
	fz_scale(&page_ctm, 2.5, 2.5);

	page = fz_load_page(ctx, doc, currentpage);
	pix = fz_new_pixmap_from_page_contents(ctx, page, &page_ctm, fz_device_rgb(ctx), 0);

	if (pix) {

		pimg = cv::Mat(pix->h, pix->w, CV_8UC3, cv::Scalar(255, 255, 255));
		memcpy(pimg.data, pix->samples, pix->w*pix->h * 3);

		//cv::Mat tmpImg(pix->h, pix->w, CV_8UC3, cv::Scalar(255, 255, 255));
		//memcpy(tmpImg.data, pix->samples, pix->w*pix->h * 3);

		//// Convert RGB to GRAY //
		if (IsGray) {
			cv::cvtColor(pimg, pimg, CV_BGR2GRAY);
		}


		fz_drop_page(ctx, page);
		fz_drop_document(ctx, doc);
		fz_drop_pixmap(ctx, pix);
		fz_drop_context(ctx);

		return true;
	}
	return false;
}