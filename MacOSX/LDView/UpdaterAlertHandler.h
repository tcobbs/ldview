/*
 *  UpdaterAlertHandler.h
 *  LDView
 *
 *  Created by Travis Cobbs on 3/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

// The following is necessary to get rid of some truly screwed up warnings
#pragma GCC visibility push(default)

#include <TCFoundation/TCObject.h>

@class Updater;

class TCProgressAlert;

class UpdaterAlertHandler: public TCObject
{
public:
	UpdaterAlertHandler(Updater *updater);
protected:
	virtual ~UpdaterAlertHandler(void);
	virtual void dealloc(void);
	void progressCallback(TCProgressAlert *alert);
	
	Updater *updater;
};
