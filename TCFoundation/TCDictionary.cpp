#include <stdlib.h>
#include <string.h>

#include "TCDictionary.h"

#include "TCObjectArray.h"
#include "TCSortedStringArray.h"

// Note: this REALLY needs to be made to be hash based.  I needed a dictionary,
// though, and this does work, albeit slowly.

TCDictionary::TCDictionary(int caseSensitive)
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

int TCDictionary::isCaseSensitve(void)
{
	return keys->isCaseSensitive();
}

void TCDictionary::setObjectForKey(TCObject* object, const char* key)
{
	int index = indexOfKey(key);

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
	int index = indexOfKey(key);

	if (index == -1)
	{
		return NULL;
	}
	else
	{
		return (*objects)[index];
	}
}

int TCDictionary::removeObjectForKey(const char* key)
{
	int index = indexOfKey(key);

	if (index == -1)
	{
		return 0;
	}
	else
	{
		keys->removeString(index);
		objects->removeObject(index);
		return 1;
	}
}

int TCDictionary::indexOfKey(const char* key)
{
	return keys->indexOfString(key);
}

TCObject *TCDictionary::copy(void)
{
	TCDictionary *newDictionary = new TCDictionary(true);

	newDictionary->objects = (TCObjectArray*)objects->copy();
	newDictionary->keys = (TCSortedStringArray*)keys->copy();
	return newDictionary;
}
