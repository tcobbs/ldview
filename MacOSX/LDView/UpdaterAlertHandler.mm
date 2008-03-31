/*
 *  UpdaterAlertHandler.mm
 *  LDView
 *
 *  Created by Travis Cobbs on 3/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "UpdaterAlertHandler.h"
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <LDLib/LDLibraryUpdater.h>

#import "Updater.h"

UpdaterAlertHandler::UpdaterAlertHandler(Updater *updater):
updater(updater)
{
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this, (TCAlertCallback)&UpdaterAlertHandler::progressCallback);
}

UpdaterAlertHandler::~UpdaterAlertHandler(void)
{
}

void UpdaterAlertHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::dealloc();
}

void UpdaterAlertHandler::progressCallback(TCProgressAlert *alert)
{
	[updater progressCallback:alert];
}
