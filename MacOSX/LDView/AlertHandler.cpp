/*
 *  AlertHandler.cpp
 *  LDView
 *
 *  Created by Travis Cobbs on 7/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include <LDLoader/LDLError.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "AlertHandler.h"
#include "ModelWindow.h"

AlertHandler::AlertHandler(ModelWindow *modelWindow)
	:modelWindow(modelWindow)
{
	TCAlertManager::registerHandler(LDLError::alertClass(), this,
		(TCAlertCallback)&AlertHandler::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
		(TCAlertCallback)&AlertHandler::progressAlertCallback);
}

AlertHandler::~AlertHandler(void)
{
}

void AlertHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(LDLError::alertClass(), this);
	TCAlertManager::unregisterHandler(TCProgressAlert::alertClass(), this);
	TCObject::dealloc();
}

void AlertHandler::ldlErrorCallback(LDLError *error)
{
	[modelWindow ldlErrorCallback:error];
}

void AlertHandler::progressAlertCallback(TCProgressAlert *alert)
{
	[modelWindow progressAlertCallback:alert];
}
