#include "TCDictionary.h"
#include "TCObjectArray.h"
#include "TCSortedStringArray.h"

#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

// Note: this REALLY needs to be made to be hash based.  I needed a dictionary,
// though, and this does work, albeit slowly.

TCDictionary::TCDictionary(bool caseSensitive)
			 :objects(new TCObjectArray),
			  keys(new TCSortedStringArray(0, caseSensitive))
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCDictionary");
#endif
}

TCDictionary::~TCDictionary(void)
{
}

void TCDictionary::dealloc(void)
{
	keys->release();
	objects->release();

	TCObject::dealloc();
}

bool TCDictionary::isCaseSensitve(void)
{
	return keys->isCaseSensitive();
}

void TCDictionary::removeAll(void)
{
	keys->removeAll();
	objects->removeAll();
}

void TCDictionary::setObjectForKey(TCObject* object, const char* key)
{
	ptrdiff_t index = indexOfKey(key);

	if (index == -1)
	{
		index = keys->addString(key);
		objects->insertObject(object, index);
	}
	else
	{
		objects->replaceObject(object, index);
	}
}

TCObject* TCDictionary::objectForKey(const char* key)
{
	ptrdiff_t index = indexOfKey(key);

	if (index == -1)
	{
		return NULL;
	}
	else
	{
		return (*objects)[index];
	}
}

bool TCDictionary::removeObjectForKey(const char* key)
{
	ptrdiff_t index = indexOfKey(key);

	if (index == -1)
	{
		return false;
	}
	else
	{
		keys->removeStringAtIndex(index);
		objects->removeObjectAtIndex(index);
		return true;
	}
}

ptrdiff_t TCDictionary::indexOfKey(const char* key)
{
	return keys->indexOfString(key);
}

TCObject *TCDictionary::copy(void) const
{
	TCDictionary *newDictionary = new TCDictionary(1);

	newDictionary->objects = (TCObjectArray*)objects->copy();
	newDictionary->keys = (TCSortedStringArray*)keys->copy();
	return newDictionary;
}
