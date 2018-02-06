/*
 *  SnapshotAlertHandler.h
 *  LDView
 *
 *  Created by Travis Cobbs on 3/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <TCFoundation/TCObject.h>

class SnapshotTaker;
class TCAlert;

class SnapshotAlertHandler: public TCObject
{
public:
	SnapshotAlertHandler(SnapshotTaker *snapshotTaker);
protected:
	virtual ~SnapshotAlertHandler(void);
	virtual void dealloc(void);
	void snapshotCallback(TCAlert *alert);
	
	SnapshotTaker *snapshotTaker;
};
