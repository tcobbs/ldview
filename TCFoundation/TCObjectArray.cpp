#include "TCObjectArray.h"
#include "mystring.h"

#include <string.h>
#include <stdio.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TCObjectArray::TCObjectArray(size_t allocated)
				  :TCArray<>(allocated)
{
	if (allocated)
	{
		memset(items, 0, allocated * sizeof(void *));
	}
#ifdef _LEAK_DEBUG
	strcpy(className, "TCObjectArray");
#endif
}

TCObjectArray::TCObjectArray(const TCObjectArray &other)
	:TCArray<>(other.count)
{
	size_t i;

	count = allocated;
	for (i = 0; i < count; i++)
	{
		TCObject *object = (TCObject*)other.items[i];

		if (object)
		{
			TCObject *newObject = object->copy();
			items[i] = newObject;
		}
		else
		{
			items[i] = NULL;
		}
	}
}

TCObjectArray::~TCObjectArray(void)
{
}

void TCObjectArray::dealloc(void)
{
	for (size_t i = 0; i < count; i++)
	{
		TCObject::release(objectAtIndex(i));
	}
	TCArray<>::dealloc();
}

void TCObjectArray::addObject(TCObject* newObject)
{
	insertObject(newObject, count);
}

bool TCObjectArray::replaceObject(TCObject* newObject, size_t index)
{
	if (index < count)
	{
		TCObject* oldObject = objectAtIndex(index);

		if (oldObject != newObject)
		{
			TCObject::release(oldObject);
			items[index] = newObject;
			TCObject::retain(newObject);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void TCObjectArray::insertObject(TCObject* newObject, size_t index)
{
	TCObject::retain(newObject);
	insertItem(newObject, index);
}

ptrdiff_t TCObjectArray::indexOfObjectIdenticalTo(TCObject* object) const
{
	return indexOfItem(object);
}

ptrdiff_t TCObjectArray::indexOfObject(TCObject* object) const
{
	for (size_t i = 0; i < count; i++)
	{
		if (object && items[i])
		{
			if (object->isEqual(*((TCObject*)(items[i]))))
			{
				return i;
			}
		}
		else if (!object && !items[i])
		{
			return i;
		}
	}
	return -1;
}

bool TCObjectArray::removeObject(TCObject* object)
{
	return removeObjectAtIndex(indexOfObject(object));
}

bool TCObjectArray::removeObjectIdenticalTo(TCObject* object)
{
	return removeObjectAtIndex(indexOfObjectIdenticalTo(object));
}

bool TCObjectArray::removeObjectAtIndex(size_t index)
{
	if (index < count)
	{
		TCObject::release(objectAtIndex(index));
		return removeItemAtIndex(index);
	}
	else
	{
		return 0;
	}
}

void TCObjectArray::removeAll(void)
{
	while (count)
	{
		removeObjectAtIndex(count - 1);
	}
}

TCObject* TCObjectArray::objectAtIndex(size_t index)
{
	return (TCObject*)itemAtIndex(index);
}

const TCObject* TCObjectArray::objectAtIndex(size_t index) const
{
	return (TCObject*)itemAtIndex(index);
}

TCObject* TCObjectArray::operator[](size_t index)
{
	return objectAtIndex(index);
}

const TCObject* TCObjectArray::operator[](size_t index) const
{
	return objectAtIndex(index);
}

TCObject *TCObjectArray::copy(void) const
{
	return new TCObjectArray(*this);
}

void TCObjectArray::sort(void)
{
	sortUsingFunction(sortFunction);
}

int TCObjectArray::sortFunction(const void *left, const void *right)
{
	return (*(const TCObject **)left)->compare(*(const TCObject **)right);
}
