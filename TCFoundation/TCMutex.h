#ifndef __TCMUTEX_H__
#define __TCMUTEX_H__

#include <TCFoundation/TCObject.h>

#ifdef WIN32
#include <windows.h>
#else // WIN32
#ifdef _QT
#include <pthread.h>
#endif // _QT
#endif // WIN32

#define TC_NORMAL_MUTEX 0
#define TC_RECURSIVE_MUTEX 1
#define TC_ERROR_CHECKING_MUTEX 2

#define TC_MUTEX_BUSY 1
#define TC_MUTEX_TOO_DEEP 2
#define TC_MUTEX_DEADLOCK 3
#define TC_MUTEX_NOT_LOCKED 4

class TCMutex : public TCObject
{
	public:
		TCMutex(int = TC_ERROR_CHECKING_MUTEX);
		int getType(void) { return mutexType; }
		virtual int lock(void);
		virtual int tryLock(void);
		virtual int unlock(void);
#ifdef WIN32
		HANDLE getMutex(void) { return mutex; }
#else // WIN32
#ifdef _QT
		pthread_mutex_t* getMutex(void) { return &mutex; }
#endif // _QT
#endif // WIN32
	protected:
		virtual ~TCMutex(void);
		virtual void dealloc(void);

		int mutexType;
#ifdef WIN32
		int checkLockCount(int lockError);

		HANDLE mutex;
		int lockCount;

		static int mutexCounter;
		static CRITICAL_SECTION criticalSection;
		static class TCMutexSetupCleanup
		{
		public:
			TCMutexSetupCleanup(void);
			~TCMutexSetupCleanup(void);
		} mutexSetupCleanup;
		friend TCMutexSetupCleanup;
#else // WIN32
#ifdef _QT
		pthread_mutex_t mutex;
#endif // _QT
#endif // WIN32
};

#endif
