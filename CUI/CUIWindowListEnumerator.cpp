#include "CUIWindowListEnumerator.h"

CUIWindowListEnumerator::CUIWindowListEnumerator(CUIWindowList* windowList)
						:windowList(windowList),
						 currentItem(NULL)
{
	setReverse(FALSE);
}

CUIWindowListEnumerator::~CUIWindowListEnumerator(void)
{
}

void CUIWindowListEnumerator::setReverse(BOOL flag)
{
	reverse = flag;
	if (!started)
	{
		if (reverse)
		{
			currentItem = windowList->tail;
		}
		else
		{
			currentItem = windowList->head;
		}
	}
}

CUIWindow* CUIWindowListEnumerator::nextWindow(void)
{
	CUIWindow* window = NULL;

	started = TRUE;
	if (currentItem)
	{
		window = currentItem->window;
		if (reverse)
		{
			currentItem = currentItem->previous;
		}
		else
		{
			currentItem = currentItem->next;
		}
	}
	return window;
}