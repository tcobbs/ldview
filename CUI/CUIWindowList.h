#ifndef __CUIWINDOWLIST_H__
#define __CUIWINDOWLIST_H__

#include <CUI/CUIWindow.h>

class CUIWindowListEnumerator;

class CUIWindowList
{
	public:
		CUIExport CUIWindowList(void);

		CUIExport void addWindow(CUIWindow*);
		CUIExport void removeWindow(CUIWindow*);
		CUIExport CUIWindowListEnumerator* enumerator(void);
		CUIExport CUIWindowListEnumerator* reverseEnumerator(void);
	protected:
		CUIExport ~CUIWindowList(void);

		struct CUIWindowListItem
		{
			CUIWindow* window;
			CUIWindowListItem* next;
			CUIWindowListItem* previous;
		};
		CUIWindowListItem* head;
		CUIWindowListItem* tail;
	private:
		friend class CUIWindowListEnumerator;
};

#endif