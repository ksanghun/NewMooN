// DragEventListener.h: interface for the CDragEventListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGEVENTLISTENER_H__54270A47_B749_49E6_8CFC_53F14E9EA13D__INCLUDED_)
#define AFX_DRAGEVENTLISTENER_H__54270A47_B749_49E6_8CFC_53F14E9EA13D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDragEventListener  
{
public:
	virtual void OnDragMove(POINT point) =0;
	virtual void OnDragRelease(POINT point,HTREEITEM hDropTarget) =0;
	virtual void OnDrag() =0;

};

#endif // !defined(AFX_DRAGEVENTLISTENER_H__54270A47_B749_49E6_8CFC_53F14E9EA13D__INCLUDED_)
