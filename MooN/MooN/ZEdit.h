#pragma once


// CZEdit

class CZEdit : public CEdit
{
	DECLARE_DYNAMIC(CZEdit)

public:
	CZEdit();
	virtual ~CZEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};


