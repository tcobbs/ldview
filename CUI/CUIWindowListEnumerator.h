#ifndef __CUIWINDOWLISTENUMERATOR_H__
#define __CUIWINDOWLISTENUMERATOR_H__

#include <CUI/CUIWindowList.h>

class CUIWindowListEnumerator
{
	public:
		CUIExport CUIWindowListEnumerator(CUIWindowList*);
		CUIExport CUIWindow* nextWindow(void);
		CUIExport virtual void setReverse(BOOL);
	protected:
		CUIExport ~CUIWindowListEnumerator(void);

		CUIWindowList* windowList;
		CUIWindowList::CUIWindowListItem* currentItem;
		BOOL reverse;
		BOOL started;
	private:
};

#endif