#ifndef __TCTHREADMANAGER_H__
#define __TCTHREADMANAGER_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>

#ifdef WIN32
#include <windows.h>
#else // WIN32
#ifdef _QT
#include <pthread.h>
#include <sys/time.h>
#endif // _QT
#endif // WIN32

class TCThread;
class TCThreadCondition;
class TCMutex;

typedef TCTypedObjectArray<TCThread> TCThreadArray;

class TCThreadManager : public TCObject
{
	public:
		static TCThreadManager* threadManager(void);
		virtual int lockExitMutex(void);
		virtual int tryLockExitMutex(void);
		virtual int unlockExitMutex(void);
		TCMutex* getExitMutex(void) { return exitMutex; }
		TCThreadCondition* getExitCondition(void) { return exitCondition; }
		virtual void registerThreadStart(TCThread*);
		virtual void registerThreadExit(TCThread*);
		virtual int removeFinishedThread(TCThread*);
		virtual int isThreadRunning(TCThread*);
		virtual int isThreadFinished(TCThread*);
		virtual int getRunningThreadCount(void);
		virtual TCThread* getFinishedThread(void);
		virtual void waitForFinishedThread(void);
		virtual int timedWaitForFinishedThread(struct timeval timeout);
		virtual int timedWaitForFinishedThread(int timeoutMsec);
	protected:
		TCThreadManager(void);
		virtual ~TCThreadManager(void);
		virtual void release(void);
		virtual TCObject* autorelease(void);
		virtual void init(void);
		virtual void dealloc(void);
		virtual void registerThreadFinish(TCThread*);
		virtual void joinExitedThreads(void);

		TCThreadArray* runningThreads;
		TCThreadArray* exitedThreads;
		TCThreadArray* finishedThreads;
		TCMutex* exitMutex;
		TCThreadCondition* exitCondition;

		static TCThreadManager* _threadManager;
		static class TCThreadManagerCleanup
		{
		public:
			~TCThreadManagerCleanup(void);
		} threadManagerCleanup;
		friend class TCThreadManagerCleanup;
};

#endif
