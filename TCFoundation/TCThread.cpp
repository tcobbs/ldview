//#include <stdio.h>

#include "TCThread.h"
#include "TCThreadManager.h"
#include "TCMutex.h"
#include "TCThreadCondition.h"
#include "mystring.h"

TCThread::TCThread(TCThreadStartFunction startFunction)
			:owner(NULL),
			 startFunction(startFunction),
			 startMemberFunction(NULL),
			 finishMemberFunction(NULL),
			 threadManager(TCThreadManager::threadManager()),
			 started(false),
			 finished(false),
			 userData(NULL),
			 returnValue(NULL)
{
}

// Don't retain owner because it owns us.
TCThread::TCThread(TCObject* owner,
				   TCThreadStartMemberFunction startMemberFunction)
			:owner(owner),
			 startFunction(NULL),
			 startMemberFunction(startMemberFunction),
			 finishMemberFunction(NULL),
			 threadManager(TCThreadManager::threadManager()),
			 started(false),
			 finished(false),
			 userData(NULL),
			 returnValue(NULL)
{
}

TCThread::~TCThread(void)
{
}

void TCThread::dealloc(void)
{
	if (started && !finished)
	{
		terminate(NULL);
	}
	// Don't release owner because it owns us.
	TCObject::dealloc();
}

void TCThread::setUserData(void* value)
{
	userData = value;
}

void TCThread::setStarted(void)
{
	threadManager->lockExitMutex();
	started = true;
	threadManager->registerThreadStart(this);
	threadManager->unlockExitMutex();
}

bool TCThread::getStarted(void)
{
	bool value;

	threadManager->lockExitMutex();
	value = started;
	threadManager->unlockExitMutex();
	return value;
}
#include <stdio.h>
void TCThread::setFinished(void)
{
	TCThreadManager *origThreadManager = threadManager;

	printf("TCThread::setFinished 1\n");
	threadManager->lockExitMutex();
	printf("TCThread::setFinished 2\n");
	finished = YES;
	printf("TCThread::setFinished 3\n");
	threadManager->registerThreadExit(this);
	printf("TCThread::setFinished 4\n");
	threadManager->unlockExitMutex();
	printf("TCThread::setFinished 5\n");
	origThreadManager->getExitCondition()->broadcast();
}

bool TCThread::getFinished(void)
{
	bool value;

	threadManager->lockExitMutex();
	value = finished;
	threadManager->unlockExitMutex();
	return value;
}

THREAD_RET_TYPE START_FUNC_MODS TCThread::threadStartFunction(void* value)
{
	TCThread* tcThread = (TCThread*)value;
	TCThreadStartFunction startFunction = tcThread->getStartFunction();
	THREAD_RET_TYPE rval;

#ifdef WIN32
	debugPrintf("Thread started: 0x%08X\n", GetCurrentThreadId());
#endif
	tcThread->setStarted();
	if (startFunction)
	{
		rval = (*startFunction)(tcThread);
	}
	else
	{
		TCThreadStartMemberFunction startMemberFunction =
		 tcThread->getStartMemberFunction();

		rval = ((*tcThread->getOwner()).*startMemberFunction)(tcThread);
	}
	tcThread->setFinished();
	return rval;
}

int TCThread::run(void)
{
#ifdef WIN32
	DWORD threadID;

	thread = CreateThread(NULL, 0, threadStartFunction, this, 0, &threadID);
	if (thread)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#else // WIN32
#ifdef _QT
	if (pthread_create(&thread, NULL, threadStartFunction, this))
	{
		return 0;
	}
	else
	{
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		pthread_cleanup_push(cancelCleanupFunction, this);
		return 1;
	}
	// WHAT!?!?!?!
	// It doesn't compile without the following extra close brace.
	}
#endif // _QT
#endif // WIN32
}

void TCThread::setReturnValue(THREAD_RET_TYPE value)
{
	returnValue = value;
}

void TCThread::setFinishMemberFunction(TCThreadFinishMemberFunction value)
{
	finishMemberFunction = value;
}

void TCThread::performFinish(void)
{
	if (owner && finishMemberFunction)
	{
		((*owner).*finishMemberFunction)(this);
	}
}

void TCThread::terminate(THREAD_RET_TYPE exitValue)
{
#ifdef WIN32
	if (started && !finished)
	{
		TerminateThread(thread, exitValue);
		setFinished();
	}
#else // WIN32
#ifdef _QT
	if (started && !finished)
	{
		cancelExitValue = exitValue;
		pthread_cancel(thread);
	}
#endif // _QT
#endif // WIN32
}

#ifdef _QT
void TCThread::cancelCleanupFunction(void *arg)
{
	TCThread *thread = (TCThread *)arg;
//	void *retValue = thread->cancelExitValue;

	thread->setFinished();
//	return retValue;
}
#endif // _QT
