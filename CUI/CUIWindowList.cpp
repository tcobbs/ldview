#include "CUIWindowList.h"
#include "CUIWindowListEnumerator.h"

CUIWindowList::CUIWindowList(void)
			  :head(NULL),
			   tail(NULL)
{
}

CUIWindowList::~CUIWindowList(void)
{
}

void CUIWindowList::addWindow(CUIWindow* window)
{
	CUIWindowListItem* thisItem = new CUIWindowListItem;

	thisItem->window = window;
	thisItem->next = NULL;
	thisItem->previous = NULL;
	if (tail)
	{
		tail->next = thisItem;
		thisItem->previous = tail;
	}
	else
	{
		head = thisItem;
	}
	tail = thisItem;
}

void CUIWindowList::removeWindow(CUIWindow* window)
{
	CUIWindowListItem* thisItem;

	for (thisItem = head; thisItem != NULL; thisItem = thisItem->next)
	{
		if (thisItem->window == window)
		{
			if (thisItem->previous)
			{
				thisItem->previous->next = thisItem->next;
			}
			else
			{
				head = thisItem->next;
			}
			if (thisItem->next)
			{
				thisItem->next->previous = thisItem->previous;
			}
			delete thisItem;
		}
	}
}

CUIWindowListEnumerator* CUIWindowList::enumerator(void)
{
	return new CUIWindowListEnumerator(this);
}

CUIWindowListEnumerator* CUIWindowList::reverseEnumerator(void)
{
	CUIWindowListEnumerator* enumerator = new CUIWindowListEnumerator(this);

	enumerator->setReverse(TRUE);
	return enumerator;
}
