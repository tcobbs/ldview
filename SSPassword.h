#ifndef __SSPASSWORD_H__
#define __SSPASSWORD_H__

#include <TCFoundation/TCObject.h>

typedef BOOL (FAR PASCAL * VERIFYPWDPROC) (HWND); 

class SSPassword: public TCObject
{
public:
	SSPassword(void);
	bool verifyPassword(HWND hWindow);
	bool getRunningOnNT(void) { return runningOnNT; }
protected:
	virtual ~SSPassword(void);
	virtual void dealloc(void);
	void loadPasswordDLL(void);
	void unloadPasswordDLL(void);
	void hogMachine(BOOL value);

	bool runningOnNT;
	HINSTANCE hInstPwdDLL;
	VERIFYPWDPROC verifyPasswordProc;
	bool checkingPassword;
	DWORD lastCheckTime;
};

#endif // __SSPASSWORD_H__
