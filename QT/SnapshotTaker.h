#ifndef __SNAPSHOTTAKER_H__
#define __SNAPSHOTTAKER_H__

#include <TCFoundation/TCObject.h>

class TCAlert;
class SnapshotAlertHandler;
class LDSnapshotTaker;

class SnapshotTaker: public TCObject
{
public:
	SnapshotTaker();
	bool doCommandLine();
	void snapshotCallback(TCAlert *alert);
protected:
	virtual ~SnapshotTaker(void);
	virtual void dealloc(void);
	bool getUseFBO();
	void cleanupContext();
	
	LDSnapshotTaker *ldSnapshotTaker;
	SnapshotAlertHandler *snapshotAlertHandler;
};

#endif // __SNAPSHOTTAKER_H__
