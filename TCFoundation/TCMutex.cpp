#include <errno.h>

#include "TCMutex.h"

#ifdef WIN32
#include <stdio.h>

int TCMutex::mutexCounter = 0;
CRITICAL_SECTION TCMutex::criticalSection;
TCMutex::TCMutexSetupCleanup TCMutex::mutexSetupCleanup;

TCMutex::TCMutexSetupCleanup::TCMutexSetupCleanup(void)
{
	InitializeCriticalSection(&criticalSection);
}

TCMutex::TCMutexSetupCleanup::~TCMutexSetupCleanup(void)
{
	DeleteCriticalSection(&criticalSection);
}
#endif // WIN32

TCMutex::TCMutex(int mutexType)
		  :mutexType(mutexType)
{
#ifdef WIN32
	lockCount = 0;
	mutex = CreateMutex(NULL, FALSE, NULL);
#else // WIN32
#ifdef _QT
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	switch (mutexType)
	{
		case TC_NORMAL_MUTEX:
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
			break;
		case TC_RECURSIVE_MUTEX:
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			break;
		case TC_ERROR_CHECKING_MUTEX:
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
			break;
		default:
			break;
	}
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif // _QT
#endif // WIN32
}

TCMutex::~TCMutex(void)
{
}

void TCMutex::dealloc(void)
{
#ifdef WIN32
	ReleaseMutex(mutex);
	CloseHandle(mutex);
#else // WIN32
#ifdef _QT
	pthread_mutex_destroy(&mutex);
#endif // _QT
#endif // WIN32
	TCObject::dealloc();
}

#ifdef WIN32
int TCMutex::checkLockCount(int lockError)
{
	if (mutexType != TC_RECURSIVE_MUTEX)
	{
		// Windows only supports recursive mutexes, so simulate non-recursive
		// ones if that's what was asked for.
		EnterCriticalSection(&criticalSection);
		lockCount++;
		LeaveCriticalSection(&criticalSection);
		if (mutexType == TC_NORMAL_MUTEX)
		{
			// I'm not sure I should really be doing this, but it does match
			// the Linux behavior.  This infinite loop will deadlock the thread
			// (and the mutex).
			while (1)
			{
				Sleep(60000);
			}
		}
		else if (mutexType == TC_ERROR_CHECKING_MUTEX)
		{
			if (lockCount > 1)
			{
				unlock();
				return lockError;
			}
		}
	}
	return 0;
}
#endif // WIN32

int TCMutex::lock(void)
{
#ifdef WIN32
	switch (WaitForSingleObject(mutex, INFINITE))
	{
	case WAIT_OBJECT_0:
		return checkLockCount(TC_MUTEX_DEADLOCK);
		break;
	default:
		return -1;
	}
#else // WIN32
#ifdef _QT
	switch (pthread_mutex_lock(&mutex))
	{
		case 0:
			return 0;
			break;
		case EAGAIN:
			return TC_MUTEX_TOO_DEEP;
			break;
		case EDEADLK:
			return TC_MUTEX_DEADLOCK;
			break;
		default:
			return -1;
			break;
	}
#endif // _QT
#endif // WIN32
}

int TCMutex::tryLock(void)
{
#ifdef WIN32
	switch (WaitForSingleObject(mutex, 0))
	{
	case WAIT_OBJECT_0:
		return checkLockCount(TC_MUTEX_BUSY);
		break;
	case WAIT_TIMEOUT:
		return TC_MUTEX_BUSY;
		break;
	default:
		return -1;
	}
#else // WIN32
#ifdef _QT
	switch (pthread_mutex_trylock(&mutex))
	{
		case 0:
			return 0;
			break;
		case EBUSY:
			return TC_MUTEX_BUSY;
			break;
		case EAGAIN:
			return TC_MUTEX_TOO_DEEP;
			break;
		default:
			return -1;
			break;
	}
#endif // _QT
#endif // WIN32
}

int TCMutex::unlock(void)
{
#ifdef WIN32
	if (ReleaseMutex(mutex))
	{
		EnterCriticalSection(&criticalSection);
		lockCount--;
		LeaveCriticalSection(&criticalSection);
		return 0;
	}
	else
	{
		return TC_MUTEX_NOT_LOCKED;
	}
#else // WIN32
#ifdef _QT
	switch (pthread_mutex_unlock(&mutex))
	{
		case 0:
			return 0;
			break;
		case EAGAIN:
			return TC_MUTEX_TOO_DEEP;
			break;
		case EPERM:
			return TC_MUTEX_NOT_LOCKED;
			break;
		default:
			return -1;
			break;
	}
#endif // _QT
#endif // WIN32
}
