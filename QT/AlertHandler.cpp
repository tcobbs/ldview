#include "AlertHandler.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLoader/LDLError.h>

AlertHandler::AlertHandler(ModelViewerWidget *mvw)
	:m_mvw(mvw)
{
	TCAlertManager::registerHandler(LDLError::alertClass(), this,
		(TCAlertCallback)&AlertHandler::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
		(TCAlertCallback)&AlertHandler::progressAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::alertClass(), this,
		(TCAlertCallback)&AlertHandler::modelViewerAlertCallback);
	TCAlertManager::registerHandler(TCUserDefaults::alertClass(), this,
		(TCAlertCallback)&AlertHandler::userDefaultChangedAlertCallback);
}

AlertHandler::~AlertHandler(void)
{
}

void AlertHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(LDLError::alertClass(), this);
	TCAlertManager::unregisterHandler(TCProgressAlert::alertClass(), this);
	TCAlertManager::unregisterHandler(LDrawModelViewer::alertClass(), this);
	TCAlertManager::unregisterHandler(TCUserDefaults::alertClass(), this);
	TCObject::dealloc();
}

void AlertHandler::ldlErrorCallback(LDLError *error)
{
	if (m_mvw)
	{
		m_mvw->ldlErrorCallback(error);
	}
}

void AlertHandler::progressAlertCallback(TCProgressAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->progressAlertCallback(alert);
	}
}

void AlertHandler::modelViewerAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->modelViewerAlertCallback(alert);
	}
}

void AlertHandler::userDefaultChangedAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->userDefaultChangedAlertCallback(alert);
	}
}
