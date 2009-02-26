#ifndef __CUIWINDOWRESIZER_H__
#define __CUIWINDOWRESIZER_H__

#include <CUI/CUIDefines.h>
#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <windows.h>

class CUISubWindowInfo;

typedef TCTypedObjectArray<CUISubWindowInfo> CUISubWindowInfoArray;


CUIExport extern const DWORD CUIFloatLeft;
CUIExport extern const DWORD CUIFloatRight;
CUIExport extern const DWORD CUIFloatTop;
CUIExport extern const DWORD CUIFloatBottom;
CUIExport extern const DWORD CUISizeHorizontal;
CUIExport extern const DWORD CUISizeVertical;

class CUIExport CUIWindowResizer : public TCObject
{
public:
	CUIWindowResizer(void);
	virtual void setHWindow(HWND value);
	HWND getHWindow(void) { return hWindow; }
	virtual void resize(int newWidth, int newHeight);
	virtual void addSubWindow(HWND hSubWindow, DWORD resizeMask);
	virtual void addSubWindow(int controlID, DWORD resizeMask);
	virtual void removeSubWindow(HWND hSubWindow);
	virtual void removeSubWindow(int controlID);
	virtual void setOriginalWidth(int value) { originalWidth = value; }
	virtual int getOriginalWidth(void) const { return originalWidth; }
	virtual void setOriginalHeight(int value) { originalHeight = value; }
	virtual int getOriginalHeight(void) const { return originalHeight; }
	virtual void addResizeGrip(void);
	static HWND createResizeGrip(HWND hParent, int x = 0, int y = 0,
		int width = 10, int height = 10);
protected:
	virtual ~CUIWindowResizer(void);
	virtual void dealloc(void);
	virtual void resizeSubWindow(CUISubWindowInfo *subWindowInfo,
		int widthDelta, int heightDelta);

	HWND hWindow;
	int originalWidth;
	int originalHeight;
	CUISubWindowInfoArray *subWindowInfos;
};

#endif // __CUIWINDOWRESIZER_H__
