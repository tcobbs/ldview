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
	TCAlertManager::registerHandler(LDrawModelViewer::redrawAlertClass(), this,
		(TCAlertCallback)&AlertHandler::redrawAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::captureAlertClass(), this,
		(TCAlertCallback)&AlertHandler::captureAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::releaseAlertClass(), this,
		(TCAlertCallback)&AlertHandler::releaseAlertCallback);
	TCAlertManager::registerHandler(
		LDPreferences::lightVectorChangedAlertClass(), this,
		(TCAlertCallback)&AlertHandler::lightVectorChangedAlertCallback);
	TCAlertManager::registerHandler(LDSnapshotTaker::makeCurrentAlertClass(),
		this, (TCAlertCallback)&AlertHandler::makeCurrentAlertCallback);
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

void AlertHandler::redrawAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->redrawAlertCallback(alert);
	}
}

void AlertHandler::captureAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->captureAlertCallback(alert);
	}
}

void AlertHandler::releaseAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->releaseAlertCallback(alert);
	}
}

void AlertHandler::lightVectorChangedAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->lightVectorChangedAlertCallback(alert);
	}
}

void AlertHandler::makeCurrentAlertCallback(TCAlert *alert)
{
	if (m_mvw)
	{
		m_mvw->makeCurrentAlertCallback(alert);
	}
}

