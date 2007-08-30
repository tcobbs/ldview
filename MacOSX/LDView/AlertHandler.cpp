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
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDInputHandler.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "AlertHandler.h"
#include "ModelWindow.h"

AlertHandler::AlertHandler(ModelWindow *modelWindow)
	:modelWindow(modelWindow)
{
	TCAlertManager::registerHandler(LDLError::alertClass(), this, (TCAlertCallback)&AlertHandler::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this, (TCAlertCallback)&AlertHandler::progressAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::alertClass(), this, (TCAlertCallback)&AlertHandler::modelViewerAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::redrawAlertClass(), this, (TCAlertCallback)&AlertHandler::redrawAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::captureAlertClass(), this, (TCAlertCallback)&AlertHandler::captureAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::releaseAlertClass(), this, (TCAlertCallback)&AlertHandler::releaseAlertCallback);
}

AlertHandler::~AlertHandler(void)
{
}

void AlertHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
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

void AlertHandler::redrawAlertCallback(TCAlert *alert)
{
	[modelWindow redrawAlertCallback:alert];
}

void AlertHandler::captureAlertCallback(TCAlert *alert)
{
	[modelWindow captureAlertCallback:alert];
}

void AlertHandler::releaseAlertCallback(TCAlert *alert)
{
	[modelWindow releaseAlertCallback:alert];
}

void AlertHandler::modelViewerAlertCallback(TCAlert *alert)
{
	[modelWindow modelViewerAlertCallback:alert];
}

