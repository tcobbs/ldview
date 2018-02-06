#include "SnapshotAlertHandler.h"
#include "SnapshotTaker.h"
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDSnapshotTaker.h>

SnapshotAlertHandler::SnapshotAlertHandler(SnapshotTaker *snapshotTaker)
	: snapshotTaker(snapshotTaker)
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
	snapshotTaker->snapshotCallback(alert);
}
