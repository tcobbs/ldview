#include "TCProgressAlert.h"
#include "TCStringArray.h"
#include "TCAlertManager.h"
#include "mystring.h"

TCProgressAlert::TCProgressAlert(const char *source, const char *message,
								 float progress, TCStringArray *extraInfo)
	:TCAlert(TCProgressAlert::alertClass(), message, extraInfo),
	m_source(copyString(source)),
	m_progress(progress),
	m_aborted(false)
{
}

TCProgressAlert::TCProgressAlert(const char *source, const wchar_t *message,
								 float progress, const WStringList &extraInfo)
	:TCAlert(TCProgressAlert::alertClass(), message, extraInfo),
	m_source(copyString(source)),
	m_progress(progress),
	m_aborted(false)
{
}

TCProgressAlert::~TCProgressAlert(void)
{
}

void TCProgressAlert::dealloc(void)
{
	delete m_source;
	TCAlert::dealloc();
}

void TCProgressAlert::send(const char *source, const char *message,
						   float progress, TCStringArray *extraInfo)
{
	send(source, message, progress, NULL, extraInfo);
}

void TCProgressAlert::send(const char *source, const wchar_t *message,
						   float progress, const WStringList &extraInfo)
{
	send(source, message, progress, NULL, extraInfo);
}

void TCProgressAlert::send(const char *source, const char *message,
						   float progress, bool *aborted,
						   TCStringArray *extraInfo)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress,
		extraInfo);

	TCAlertManager::sendAlert(alert);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}

void TCProgressAlert::send(const char *source, const wchar_t *message,
						   float progress, bool *aborted,
						   const WStringList &extraInfo)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress,
		extraInfo);

	TCAlertManager::sendAlert(alert);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}
