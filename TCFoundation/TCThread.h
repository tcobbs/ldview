#ifndef __TCTHREAD_H__
#define __TCTHREAD_H__

#include <TCFoundation/TCObject.h>

#ifdef WIN32
#include <windows.h>
#define THREAD_RET_TYPE DWORD
#define START_FUNC_MODS __stdcall
#else // WIN32
#ifdef _QT
#include <pthread.h>
#define THREAD_RET_TYPE void *
#define START_FUNC_MODS
#endif // _QT
#endif // WIN32

class TCThread;
class TCThreadManager;

typedef THREAD_RET_TYPE (*TCThreadStartFunction)(TCThread*);
typedef THREAD_RET_TYPE (TCObject::*TCThreadStartMemberFunction)(TCThread*);
typedef void (TCObject::*TCThreadFinishMemberFunction)(TCThread*);

class TCThread : public TCObject
{
	public:
		TCThread(TCThreadStartFunction);
		TCThread(TCObject*, TCThreadStartMemberFunction);
		virtual int run(void);
		TCObject* getOwner(void) { return owner; }
		TCThreadStartFunction getStartFunction(void) { return startFunction; }
		TCThreadStartMemberFunction getStartMemberFunction(void)
		{
			return startMemberFunction;
		}
		virtual bool getStarted(void);
		virtual bool getFinished(void);
		void* getUserData(void) { return userData; }
		virtual void setUserData(void*);
#ifdef WIN32
		HANDLE getHThread(void) { return thread; }
#include <windows.h>
#else // WIN32
#ifdef _QT
		pthread_t getPThread(void) { return thread; }
#endif // _QT
#endif // WIN32
		virtual void setReturnValue(THREAD_RET_TYPE);
		THREAD_RET_TYPE getReturnValue(void) { return returnValue; }
		virtual void setFinishMemberFunction(TCThreadFinishMemberFunction);
		TCThreadFinishMemberFunction getFinishMemberFunction(void)
		{
			return finishMemberFunction;
		}
		void performFinish(void);
		void terminate(THREAD_RET_TYPE exitValue);
	protected:
		virtual void setStarted(void);
		virtual void setFinished(void);
		virtual ~TCThread(void);
		virtual void dealloc(void);

		TCObject* owner;
		TCThreadStartFunction startFunction;
		TCThreadStartMemberFunction startMemberFunction;
		TCThreadFinishMemberFunction finishMemberFunction;
		TCThreadManager* threadManager;
		bool started;
		bool finished;
#ifdef WIN32
		HANDLE thread;
#else // WIN32
#ifdef _QT
		pthread_t thread;
		THREAD_RET_TYPE cancelExitValue;
		static THREAD_RET_TYPE cancelCleanupFunction(void *arg);
#endif // _QT
#endif // WIN32
		void* userData;
		THREAD_RET_TYPE returnValue;
		static THREAD_RET_TYPE START_FUNC_MODS threadStartFunction(void*);
};

#endif
