#ifndef __TCOBJECT_H__
#define __TCOBJECT_H__

#include <TCFoundation/TCDefines.h>

class TCAutoreleasePool;

class TCExport TCObject
{
	public:
		TCObject(void);
		virtual TCObject* retain(void);
		virtual void release(void);
		virtual TCObject* autorelease(void);
		virtual int isEqual(TCObject& other);
		virtual TCObject *copy(void);

		static TCObject *retain(TCObject *object);
		static void release(TCObject *object);
		static TCObject *autorelease(TCObject *object);
		static TCObject *copy(TCObject *object);
	protected:
		virtual ~TCObject(void);
		virtual void dealloc(void);

#ifdef _LEAK_DEBUG
		char className[32];
#endif
		int retainCount;
		friend TCAutoreleasePool;
};

#endif
