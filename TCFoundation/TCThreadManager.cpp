#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "TCThreadManager.h"
#include "TCThread.h"
#include "TCMutex.h"
#include "TCThreadCondition.h"
#include "mystring.h"

TCThreadManager* TCThreadManager::_threadManager = NULL;

TCThreadManager::TCThreadManagerCleanup TCThreadManager::threadManagerCleanup;

TCThreadManager::TCThreadManagerCleanup::~TCThreadManagerCleanup(void)
{
	if (_threadManager)
	{
		_threadManager->release();
	}
}

TCThreadManager::TCThreadManager(void)
					 :runningThreads(new TCThreadArray),
					  exitedThreads(new TCThreadArray),
					  finishedThreads(new TCThreadArray),
					  exitMutex(new TCMutex(TC_RECURSIVE_MUTEX)),
					  exitCondition(new TCThreadCondition(exitMutex))
{
}

TCThreadManager::~TCThreadManager(void)
{
}

void TCThreadManager::dealloc(void)
{
	TCObject::release(runningThreads);
	TCObject::release(exitedThreads);
	TCObject::release(finishedThreads);
	TCObject::release(exitMutex);
	TCObject::release(exitCondition);
	TCObject::dealloc();
}

void TCThreadManager::init(void)
{
//	exitMutex = new TCMutex;
//	exitCondition = new TCThreadCondition(exitMutex);
}

int TCThreadManager::lockExitMutex(void)
{
	int retValue;

	debugPrintf(4, "TCThreadManager::lockExitMutex (pre ):%08X\n",
		GetCurrentThreadId());
	retValue = exitMutex->lock();
	debugPrintf(4, "TCThreadManager::lockExitMutex (post):%08X\n",
		GetCurrentThreadId());
	return retValue;
}

int TCThreadManager::tryLockExitMutex(void)
{
	debugPrintf(4, "TCThreadManager::tryLockExitMutex\n");
	return exitMutex->tryLock();
}

int TCThreadManager::unlockExitMutex(void)
{
	debugPrintf(4, "TCThreadManager::unlockExitMutex:     %08X\n",
		GetCurrentThreadId());
	return exitMutex->unlock();
}

TCThreadManager* TCThreadManager::threadManager(void)
{
	if (!_threadManager)
	{
		_threadManager = new TCThreadManager;
		_threadManager->init();
	}
	return _threadManager;
}

void TCThreadManager::release(void)
{
	TCObject::release();
}

TCObject* TCThreadManager::autorelease(void)
{
	return TCObject::autorelease();
}

void TCThreadManager::registerThreadStart(TCThread* thread)
{
	runningThreads->addObject(thread);
}

void TCThreadManager::registerThreadExit(TCThread* thread)
{
	thread->retain();
	runningThreads->removeObjectIdenticalTo(thread);
	exitedThreads->addObject(thread);
	thread->release();
}

void TCThreadManager::registerThreadFinish(TCThread* thread)
{
	thread->retain();
	exitedThreads->removeObjectIdenticalTo(thread);
	finishedThreads->addObject(thread);
	thread->release();
}

int TCThreadManager::removeFinishedThread(TCThread* thread)
{
	int result;
	int needToUnlock = 1;

	if (lockExitMutex() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	result = finishedThreads->removeObjectIdenticalTo(thread);
	if (needToUnlock)
	{
		unlockExitMutex();
	}
	return result;
}

int TCThreadManager::isThreadRunning(TCThread* thread)
{
	int result;
	int needToUnlock = 1;

	if (lockExitMutex() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	result = runningThreads->indexOfObjectIdenticalTo(thread);
	if (needToUnlock)
	{
		unlockExitMutex();
	}
	return result != -1;
}

int TCThreadManager::isThreadFinished(TCThread* thread)
{
	int result;
	int needToUnlock = 1;

	if (lockExitMutex() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	result = finishedThreads->indexOfObjectIdenticalTo(thread);
	if (needToUnlock)
	{
		unlockExitMutex();
	}
	return result != -1;
}

TCThread* TCThreadManager::getFinishedThread(void)
{
	TCThread* result = NULL;
	int needToUnlock = 1;

	if (lockExitMutex() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	if (finishedThreads->getCount())
	{
		result = finishedThreads->objectAtIndex(0);
	}
	if (needToUnlock)
	{
		unlockExitMutex();
	}
	if (result)
	{
		result->performFinish();
	}
	return result;
}

int TCThreadManager::getRunningThreadCount(void)
{
	int result;
	int needToUnlock = 1;

	if (lockExitMutex() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	result = runningThreads->getCount();
	if (needToUnlock)
	{
		unlockExitMutex();
	}
	return result;
}

void TCThreadManager::joinExitedThreads(void)
{
	while (exitedThreads->getCount())
	{
		TCThread* exitedThread = (*exitedThreads)[0];
		THREAD_RET_TYPE threadReturnValue;

//		printf("about to join: %ld\n", exitedThread->getPThread());
#ifdef WIN32
		if (GetExitCodeThread(exitedThread->getHThread(), &threadReturnValue))
		{
			exitedThread->setReturnValue(threadReturnValue);
		}
		else
		{
			debugPrintf("GetExitCodeThread returned 0.\n");
		}
#else // WIN32
#ifdef _QT
		if (pthread_join(exitedThread->getPThread(), &threadReturnValue) == 0)
		{
			if (threadReturnValue)
			{
				exitedThread->setReturnValue(threadReturnValue);
			}
		}
#endif // _QT
#endif // WIN32
		registerThreadFinish(exitedThread);
	}
}

void TCThreadManager::waitForFinishedThread(void)
{
	lockExitMutex();
	if (!finishedThreads->getCount())
	{
		unlockExitMutex();
		exitCondition->wait();
		lockExitMutex();
	}
	joinExitedThreads();
	unlockExitMutex();
}

int TCThreadManager::timedWaitForFinishedThread(int timeoutMsec)
{
	struct timeval timeout;

	timeout.tv_sec = timeoutMsec / 1000;
	timeout.tv_usec = (timeoutMsec % 1000) * 1000;
	return timedWaitForFinishedThread(timeout);
}

int TCThreadManager::timedWaitForFinishedThread(struct timeval timeout)
{
	int result = 1;

	lockExitMutex();
	if (!finishedThreads->getCount())
	{
		unlockExitMutex();
		result = exitCondition->timedWait(timeout);
		lockExitMutex();
	}
	joinExitedThreads();
	unlockExitMutex();
	return result;
}
