#include <errno.h>
//#include <stdio.h>

#include "TCThreadCondition.h"
#include "TCMutex.h"

TCThreadCondition::TCThreadCondition(TCMutex* mutex)
	:mutex((TCMutex*)(mutex->retain())),
	triggered(false)
{
#ifdef WIN32
	waitCount = 0;
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else // WIN32
#ifdef _QT
	pthread_cond_init(&cond, NULL);
#endif // _QT
#endif // WIN32
}

TCThreadCondition::~TCThreadCondition(void)
{
}

void TCThreadCondition::dealloc(void)
{
	mutex->release();
#ifdef WIN32
	CloseHandle(event);
#else // WIN32
#ifdef _QT
	pthread_cond_destroy(&cond);
#endif // _QT
#endif // WIN32
	TCObject::dealloc();
}

void TCThreadCondition::signal(void)
{
#ifdef WIN32
	bool done = false;

	mutex->lock();
	if (waitCount == 0)
	{
		done = true;
	}
	triggered = true;
	mutex->unlock();
	if (!done)
	{
		SetEvent(event);
	}
#else // WIN32
#ifdef _QT
	int needToUnlock = 1;

	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	pthread_cond_signal(&cond);
	if (needToUnlock)
	{
		mutex->unlock();
	}
#endif // _QT
#endif // WIN32
}

void TCThreadCondition::broadcast(void)
{
#ifdef WIN32
	bool done = false;

	while (!done)
	{
		mutex->lock();
		if (waitCount == 0)
		{
			done = true;
		}
		triggered = true;
		mutex->unlock();
		if (!done)
		{
			SetEvent(event);
		}
	}
#else // WIN32
#ifdef _QT
	int needToUnlock = 1;

	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	pthread_cond_broadcast(&cond);
	if (needToUnlock)
	{
		mutex->unlock();
	}
#endif // _QT
#endif // WIN32
}

void TCThreadCondition::wait(void)
{
#ifdef WIN32
	bool done = false;
	bool needToUnlock = true;

	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = false;
	}
	if (triggered)
	{
		done = true;
		triggered = false;
	}
	else
	{
		waitCount++;
	}
	if (needToUnlock)
	{
		mutex->unlock();
	}
	needToUnlock = true;
	if (!done)
	{
		if (WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0)
		{
			if (mutex->lock() == TC_MUTEX_DEADLOCK)
			{
				needToUnlock = false;
			}
			waitCount--;
			triggered = false;
			if (needToUnlock)
			{
				mutex->unlock();
			}
		}
		else
		{
			if (mutex->lock() == TC_MUTEX_DEADLOCK)
			{
				needToUnlock = false;
			}
			waitCount--;
			if (needToUnlock)
			{
				mutex->unlock();
			}
		}
	}
#else // WIN32
#ifdef _QT
	int needToUnlock = 1;

	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	pthread_cond_wait(&cond, mutex->getMutex());
	if (needToUnlock)
	{
		mutex->unlock();
	}
#endif // _QT
#endif // WIN32
}

int TCThreadCondition::timedWait(struct timeval timeout)
{
#ifdef WIN32
	bool done = false;
	bool needToUnlock = true;

	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = false;
	}
	if (triggered)
	{
		done = true;
		triggered = false;
	}
	else
	{
		waitCount++;
	}
	if (needToUnlock)
	{
		mutex->unlock();
	}
	needToUnlock = true;
	if (done)
	{
		return true;
	}
	else
	{
		DWORD msec = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;

		if (WaitForSingleObject(event, msec) == WAIT_OBJECT_0)
		{
			if (mutex->lock() == TC_MUTEX_DEADLOCK)
			{
				needToUnlock = false;
			}
			waitCount--;
			triggered = false;
			if (needToUnlock)
			{
				mutex->unlock();
			}
			return true;
		}
		else
		{
			if (mutex->lock() == TC_MUTEX_DEADLOCK)
			{
				needToUnlock = false;
			}
			waitCount--;
			if (needToUnlock)
			{
				mutex->unlock();
			}
			return false;
		}
	}
#else // WIN32
#ifdef _QT
	int needToUnlock = 1;
	struct timespec endTime;
	struct timeval startTime;
	int timedout = NO;

	gettimeofday(&startTime, NULL);
	endTime.tv_sec = startTime.tv_sec + timeout.tv_sec;
	if (timeout.tv_usec >= 1000000)
	{
		long seconds = timeout.tv_usec / 1000000;

		endTime.tv_sec -= seconds;
		timeout.tv_usec -= seconds * 1000000;
	}
	endTime.tv_nsec = startTime.tv_usec * 1000 + timeout.tv_usec * 1000;
	if (endTime.tv_nsec >= 1000000000)
	{
		endTime.tv_nsec -= 1000000000;
		endTime.tv_sec += 1;
	}
	if (mutex->lock() == TC_MUTEX_DEADLOCK)
	{
		needToUnlock = 0;
	}
	if (pthread_cond_timedwait(&cond, mutex->getMutex(), &endTime) == ETIMEDOUT)
	{
		timedout = YES;
	}
	if (needToUnlock)
	{
		mutex->unlock();
	}
	return !timedout;
#endif // _QT
#endif // WIN32
}
