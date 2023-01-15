#ifndef __TCARRAY_H__
#define __TCARRAY_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/mystring.h>
#include <cstdlib>

typedef int (*TCArraySortFunction)(const void*, const void*);

template <class Type = void*>
class TCExport TCArray : public TCObject
{
public:
	explicit TCArray(size_t allocated = 0):
		items(NULL),
		count(0),
		allocated(allocated)
	{
		if (allocated)
		{
			items = new Type[allocated];
		}
#ifdef _LEAK_DEBUG
		strcpy(className, "TCArray");
#endif
	}

	TCArray (const TCArray &other):
		items(NULL),
		count(other.count),
		allocated(other.count)
	{
		if (allocated)
		{
			items = new Type[count];
		}
		memcpy(items, other.items, count * sizeof(Type));
	}

	virtual void addItem(Type newItem)
	{
		insertItem(newItem, count);
	}

	virtual void insertItem(Type newItem, size_t index = 0)
	{
		bool expanded = false;

		if (index > count)
		{
			return;
		}
		if (count + 1 > allocated)
		{
			Type* newItems;

			expanded = true;
			if (allocated)
			{
				allocated *= 2;
			}
			else
			{
				allocated = 1;
			}
			newItems = new Type[allocated];
			if (index)
			{
				memcpy(newItems, items, index * sizeof (Type));
			}
			if (index < count)
			{
				memcpy(newItems + index + 1, items + index,
				 (count - index) * sizeof (Type));
			}
			delete[] items;
			items = newItems;
		}
		if (!expanded && index < count)
		{
			memmove(items + index + 1, items + index,
			 (count - index) * sizeof (Type));
		}
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 6386)
#endif // WIN32
		items[index] = newItem;
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32
		count++;
	}

	virtual bool replaceItem(Type newItem, size_t index)
	{
		if (index < count)
		{
			items[index] = newItem;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual ptrdiff_t indexOfItem(Type item) const
	{
		for (size_t i = 0; i < count; i++)
		{
			if (items[i] == item)
			{
				return i;
			}
		}
		return -1;
	}

	virtual bool removeItem(Type item)
	{
		return removeItemAtIndex(indexOfItem(item));
	}

	virtual bool removeItemAtIndex(size_t index)
	{
		if (index < count)
		{
			--count;
			if (index < count)
			{
				memmove(items + index, items + index + 1, ((size_t)count - index) *
					sizeof(Type));
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool removeItems(ptrdiff_t index, size_t numItems)
	{
		ptrdiff_t i;

		for (i = index + numItems - 1; i >= index; i--)
		{
			if (!removeItemAtIndex(i))
			{
				return false;
			}
		}
		return true;
	}

	virtual void removeAll(void)
	{
		while (count)
		{
			removeItemAtIndex(count - 1);
		}
	}

	virtual Type itemAtIndex(size_t index)
	{
		if (index < count)
		{
			return items[index];
		}
		else
		{
			Type retValue;

			memset(&retValue, 0, sizeof(retValue));
			return retValue;
		}
	}

	virtual const Type itemAtIndex(size_t index) const
	{
		if (index < count)
		{
			return items[index];
		}
		else
		{
			Type retValue;

			memset(&retValue, 0, sizeof(retValue));
			return retValue;
		}
	}

	size_t getCount(void) const
	{
		return count;
	}

	virtual void shrinkToFit(void)
	{
		setCapacity(count);
	}

	virtual size_t setCapacity(size_t newCapacity, bool updateCount = false, bool clear = false)
	{
		if (newCapacity >= count)
		{
			Type* newItems;

			allocated = newCapacity;
			newItems = new Type[allocated];
			if (count)
			{
				memcpy(newItems, items, count * sizeof (Type));
			}
			delete[] items;
			items = newItems;
			if (updateCount)
			{
				if (clear)
				{
					memset(&newItems[count], 0, (newCapacity - count) * sizeof (Type));
				}
				count = newCapacity;
			}
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void sortUsingFunction(TCArraySortFunction function)
	{
		qsort(items, count, sizeof (Type), function);
	}

	Type *getItems(void)
	{
		return items;
	}

	const Type *getItems(void) const
	{
		return items;
	}

	virtual TCObject *copy(void) const
	{
		return new TCArray(*this);
	}

protected:
	virtual ~TCArray(void)
	{
	}

	virtual void dealloc(void)
	{
		delete[] items;
		TCObject::dealloc();
	}

	Type* items;
	size_t count;
	size_t allocated;
};

#endif // __TCARRAY_H__
