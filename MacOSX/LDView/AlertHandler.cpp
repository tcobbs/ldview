/*
 *  AlertHandler.cpp
 *  LDView
 *
 *  Created by Travis Cobbs on 7/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
#include <LDLoader/LDLError.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "AlertHandler.h"
#include "ModelWindow.h"
#include "LDViewController.h"

AlertHandler::AlertHandler(ModelWindow *modelWindow)
	:modelWindow(modelWindow)
{
	TCAlertManager::registerHandler(LDLError::alertClass(), this, (TCAlertCallback)&AlertHandler::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this, (TCAlertCallback)&AlertHandler::progressAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::alertClass(), this, (TCAlertCallback)&AlertHandler::modelViewerAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::redrawAlertClass(), this, (TCAlertCallback)&AlertHandler::redrawAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::loadAlertClass(), this, (TCAlertCallback)&AlertHandler::loadAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::captureAlertClass(), this, (TCAlertCallback)&AlertHandler::captureAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::releaseAlertClass(), this, (TCAlertCallback)&AlertHandler::releaseAlertCallback);
	TCAlertManager::registerHandler(LDPreferences::lightVectorChangedAlertClass(), this, (TCAlertCallback)&AlertHandler::lightVectorChangedAlertCallback);
//	TCAlertManager::registerHandler(LDInputHandler::peekMouseUpAlertClass(), this, (TCAlertCallback)&AlertHandler::peekMouseUpAlertCallback);
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

void AlertHandler::modelViewerAlertCallback(TCAlert *alert)
{
	[modelWindow modelViewerAlertCallback:alert];
}

void AlertHandler::progressAlertCallback(TCProgressAlert *alert)
{
	[modelWindow progressAlertCallback:alert];
}

void AlertHandler::redrawAlertCallback(TCAlert *alert)
{
	[modelWindow redrawAlertCallback:alert];
}

void AlertHandler::loadAlertCallback(TCAlert *alert)
{
	[modelWindow loadAlertCallback:alert];
}

void AlertHandler::captureAlertCallback(TCAlert *alert)
{
	[modelWindow captureAlertCallback:alert];
}

void AlertHandler::releaseAlertCallback(TCAlert *alert)
{
	[modelWindow releaseAlertCallback:alert];
}

void AlertHandler::lightVectorChangedAlertCallback(TCAlert *alert)
{
	[modelWindow lightVectorChanged:alert];
}

//void AlertHandler::peekMouseUpAlertCallback(TCAlert *alert)
//{
//	[modelWindow peekMouseUpAlertCallback:alert];
//}

