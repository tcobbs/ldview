#ifndef __TCALERTMANAGER_H__
#define __TCALERTMANAGER_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCTypedPointerArray.h>
#include <TCFoundation/TCTypedValueArray.h>

class TCAlert;

typedef void (TCObject::*TCAlertCallback)(TCAlert* alert);

typedef TCTypedPointerArray<TCObject *> TCObjectPointerArray;
typedef TCTypedObjectArray<TCObjectPointerArray> TCObjectPointerArrayArray;
typedef TCTypedPointerArray<TCAlertCallback *> TCAlertCallbackArray;
typedef TCTypedObjectArray<TCAlertCallbackArray> TCAlertCallbackArrayArray;

class TCAlertManager : public TCObject
{
public:
	static void sendAlert(TCAlert *alert);
	static void registerHandler(TCULong alertClass, TCObject *handler,
		TCAlertCallback alertCallback);
	static void unregisterHandler(TCULong alertClass, TCObject *handler);
protected:
	TCAlertManager(void);
	virtual ~TCAlertManager(void);
	virtual void dealloc(void);
	static TCAlertManager *defaultAlertManager(void);
	int alertClassIndex(TCULong alertClass);
	void defSendAlert(TCAlert *alert);
	void defRegisterHandler(TCULong alertClass, TCObject *handler,
		TCAlertCallback callback);
	void defUnregisterHandler(TCULong alertClass, TCObject *handler);

	TCULongArray *m_alertClasses;
	TCObjectPointerArrayArray *m_handlers;
	TCAlertCallbackArrayArray *m_callbacks;

	static TCAlertManager *sm_defaultAlertManager;
	static class TCAlertManagerCleanup
	{
	public:
		~TCAlertManagerCleanup(void);
	} sm_alertManagerCleanup;
	friend TCAlertManagerCleanup;
};

#endif __TCALERTMANAGER_H__
