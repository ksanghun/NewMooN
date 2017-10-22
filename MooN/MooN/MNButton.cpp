// MNButton.cpp : implementation file
//

#include "stdafx.h"
#include "MooN.h"
#include "MNButton.h"

// CMNButton
IMPLEMENT_DYNAMIC(CMNButton, CButton)

CMNButton::CMNButton()
{

}

CMNButton::~CMNButton()
{
}


BEGIN_MESSAGE_MAP(CMNButton, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMNButton message handlers




BOOL CMNButton::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

//	return CButton::OnEraseBkgnd(pDC);
	return TRUE;
}
