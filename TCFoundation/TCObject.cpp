#include "TCObject.h"
#include "TCAutoreleasePool.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

TCObject::TCObject(void)
		 :retainCount(1)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCObject");
#endif
}

TCObject::~TCObject(void)
{
}

TCObject* TCObject::retain(void)
{
	retainCount++;
	return this;
}

void TCObject::release(void)
{
	assert(retainCount > 0);
	retainCount--;
	if (!retainCount)
	{
		dealloc();
	}
}

void TCObject::dealloc(void)
{
#if defined WIN32 && defined _DEBUG && defined __THIS_IS_NOT_DEFINED
	TCAutoreleasePool::registerDelete(this);
#else
	delete this;
#endif
//	printf("TCObject::dealloc()\n");
}

TCObject* TCObject::autorelease(void)
{
	TCAutoreleasePool::registerRelease(this);
	return this;
}

int TCObject::isEqual(TCObject& other)
{
	return this == &other;
}

TCObject *TCObject::copy(void)
{
	return NULL;
}

TCObject *TCObject::retain(TCObject *object)
{
	if (object)
	{
		return object->retain();
	}
	else
	{
		return NULL;
	}
}

void TCObject::release(TCObject *object)
{
	// NOTE: object cannot be a TCObject *&, because inheritance won't allow
	// a sub-class to match a reference.  So we cann't have this function also
	// set the original pointer to NULL.
	if (object)
	{
		object->release();
	}
}

TCObject *TCObject::autorelease(TCObject *object)
{
	if (object)
	{
		return object->autorelease();
	}
	else
	{
		return NULL;
	}
}

TCObject *TCObject::copy(TCObject *object)
{
	if (object)
	{
		return object->copy();
	}
	else
	{
		return NULL;
	}
}
