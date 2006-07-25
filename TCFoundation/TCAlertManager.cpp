#include "TCAlertManager.h"
#include "TCAlert.h"

#include <stdlib.h>
#include <string.h>

TCAlertManager *TCAlertManager::sm_defaultAlertManager = NULL;

TCAlertManager::TCAlertManagerCleanup TCAlertManager::sm_alertManagerCleanup;

TCAlertManager::TCAlertManagerCleanup::~TCAlertManagerCleanup(void)
{
	TCObject::release(sm_defaultAlertManager);
}


TCAlertManager::TCAlertManager(void)
	:m_alertClasses(new TCStringArray),
	m_handlers(new TCObjectPointerArrayArray),
	m_callbacks(new TCAlertCallbackArrayArray)
{
}

TCAlertManager::~TCAlertManager(void)
{
}

void TCAlertManager::dealloc(void)
{
	TCObject::release(m_alertClasses);
	TCObject::release(m_handlers);
	TCObject::release(m_callbacks);
	TCObject::dealloc();
}

TCAlertManager *TCAlertManager::defaultAlertManager(void)
{
	if (!sm_defaultAlertManager)
	{
		sm_defaultAlertManager = new TCAlertManager;
	}
	return sm_defaultAlertManager;
}

void TCAlertManager::sendAlert(TCAlert *alert)
{
	defaultAlertManager()->defSendAlert(alert);
}

void TCAlertManager::registerHandler(const char *alertClass, TCObject *handler,
									 TCAlertCallback callback)
{
	defaultAlertManager()->defRegisterHandler(alertClass, handler, callback);
}

void TCAlertManager::unregisterHandler(const char *alertClass, TCObject *handler)
{
	defaultAlertManager()->defUnregisterHandler(alertClass, handler);
}

void TCAlertManager::defSendAlert(TCAlert *alert)
{
	int index = alertClassIndex(alert->getAlertClass());

	if (index >= 0)
	{
		TCObjectPointerArray *handlers = (*m_handlers)[index];
		TCAlertCallbackArray *callbacks = (*m_callbacks)[index];
		int i;
		int count = handlers->getCount();

		for (i = 0; i < count; i++)
		{
			TCAlertCallback *callback = (*callbacks)[i];

			(*(*handlers)[i].**callback)(alert);
		}
	}
}

void TCAlertManager::defRegisterHandler(const char *alertClass,
										TCObject *handler,
										TCAlertCallback callback)
{
	int index = alertClassIndex(alertClass);
	TCAlertCallback *callbackPointer;
	
	if (index < 0)
	{
		TCObjectPointerArray *objectPointerArray = new TCObjectPointerArray;
		TCAlertCallbackArray *alertCallbackArray = new TCAlertCallbackArray;

		m_alertClasses->addString(alertClass);
		m_handlers->addObject(objectPointerArray);
		objectPointerArray->release();
		m_callbacks->addObject(alertCallbackArray);
		alertCallbackArray->release();
		index = m_alertClasses->getCount() - 1;
	}
	// Nobody told me I'd have to use malloc!  For some reason, delete blows
	// chunk when I tried new here and delete later.
	callbackPointer = (TCAlertCallback *)malloc(sizeof(TCAlertCallback *));
	*callbackPointer = callback;
	(*m_handlers)[index]->addPointer(handler);
	(*m_callbacks)[index]->addPointer(callbackPointer);
}

void TCAlertManager::defUnregisterHandler(const char *alertClass,
										  TCObject *handler)
{
	int index = alertClassIndex(alertClass);

	if (index >= 0)
	{
		TCObjectPointerArray *handlers = (*m_handlers)[index];
		TCAlertCallbackArray *callbacks = (*m_callbacks)[index];
		int i;
		int count = handlers->getCount();
		bool done = false;

		for (i = 0; i < count && !done; i++)
		{
			if ((*handlers)[i] == handler)
			{
				TCAlertCallback *callbackPointer = (*callbacks)[i];

				handlers->removePointer(i);
				callbacks->removePointer(i);
				// As mentioned above, when I used new and delete, the delete
				// that I would have performed here crashed.
				free(callbackPointer);
				done = true;
			}
		}
	}
}

int TCAlertManager::alertClassIndex(const char *alertClass)
{
	int i;
	int count = m_alertClasses->getCount();

	for (i = 0; i < count; i++)
	{
		if (strcmp((*m_alertClasses)[i], alertClass) == 0)
		{
			return i;
		}
	}
	return -1;
}
