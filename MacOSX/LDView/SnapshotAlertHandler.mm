/*
 *  SnapshotAlertHandler.mm
 *  LDView
 *
 *  Created by Travis Cobbs on 3/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "SnapshotAlertHandler.h"
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDSnapshotTaker.h>

#import "SnapshotTaker.h"

SnapshotAlertHandler::SnapshotAlertHandler(SnapshotTaker *snapshotTaker):
snapshotTaker(snapshotTaker)
{
	TCAlertManager::registerHandler(LDSnapshotTaker::alertClass(), this, (TCAlertCallback)&SnapshotAlertHandler::snapshotCallback);
}

SnapshotAlertHandler::~SnapshotAlertHandler(void)
{
}

void SnapshotAlertHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::dealloc();
}

void SnapshotAlertHandler::snapshotCallback(TCAlert *alert)
{
	[snapshotTaker snapshotCallback:alert];
}
