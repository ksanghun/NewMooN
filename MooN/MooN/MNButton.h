#pragma once


// CMNButton

class CMNButton : public CButton
{
	DECLARE_DYNAMIC(CMNButton)

public:
	CMNButton();
	virtual ~CMNButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


