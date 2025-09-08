//
//  AlertHandler.cpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/7/25.
//

#include "AlertHandler.h"
#include <LDLoader/LDLError.h>
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <iostream>

AlertHandler::AlertHandler()
{
	TCAlertManager::registerHandler(LDLError::alertClass(), this, (TCAlertCallback)&AlertHandler::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this, (TCAlertCallback)&AlertHandler::progressAlertCallback);
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
	std::cerr << error->getMessage();
}

void AlertHandler::progressAlertCallback(TCProgressAlert *alert)
{
	int progress = alert->getProgress() * 100.0;
	if (progress != lastProgress) {
		std::cout << progress << "%\r" << std::flush;
		lastProgress = progress;
	}
}
