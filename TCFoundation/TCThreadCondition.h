#ifndef __TCTHREADCONDITION_H__
#define __TCTHREADCONDITION_H__

#include <TCFoundation/TCObject.h>

#ifdef WIN32
#include <windows.h>
#else // WIN32
#ifdef _QT
#include <pthread.h>
#include <sys/time.h>
#endif // _QT
#endif // WIN32

class TCMutex;

class TCThreadCondition : public TCObject
{
	public:
		TCThreadCondition(TCMutex*);
		virtual void signal(void);
		virtual void broadcast(void);
		virtual void wait(void);
		virtual int timedWait(struct timeval timeout);
	protected:
		virtual ~TCThreadCondition(void);
		virtual void dealloc(void);

#ifdef WIN32
		HANDLE event;
		int waitCount;
		bool triggered;
#else // WIN32
#ifdef _QT
		pthread_cond_t cond;
#endif // _QT
#endif // WIN32
		TCMutex* mutex;
};

#endif
