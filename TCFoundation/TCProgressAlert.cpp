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
								 float progress,
								 const ucstringVector &extraInfo)
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

void TCProgressAlert::send(
	const char *source,
	const char *message,
	float progress,
	TCStringArray *extraInfo,
	TCObject *sender)
{
	send(source, message, progress, NULL, extraInfo, sender);
}

void TCProgressAlert::send(
	const char *source,
	const wchar_t *message,
	float progress,
	const ucstringVector &extraInfo,
	TCObject *sender)
{
	send(source, message, progress, NULL, extraInfo, sender);
}

void TCProgressAlert::send(
	const char *source,
	const char *message,
	float progress,
	bool *aborted,
	TCStringArray *extraInfo,
	TCObject *sender)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress,
		extraInfo);

	TCAlertManager::sendAlert(alert, sender);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}

void TCProgressAlert::send(
	const char *source,
	const char *message,
	float progress,
	bool *aborted,
	TCObject *sender)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress);

	TCAlertManager::sendAlert(alert, sender);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}

void TCProgressAlert::send(
	const char *source,
	const char *message,
	float progress,
	TCObject *sender)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress);

	TCAlertManager::sendAlert(alert, sender);
	alert->release();
}

void TCProgressAlert::send(
	const char *source,
	const wchar_t *message,
	float progress,
	bool *aborted,
	TCObject *sender)
{
	send(source, message, progress, aborted, ucstringVector(), sender);
}

void TCProgressAlert::send(
	const char *source,
	const wchar_t *message,
	float progress,
	TCObject *sender)
{
	send(source, message, progress, NULL, ucstringVector(), sender);
}

void TCProgressAlert::send(
	const char *source,
	const wchar_t *message,
	float progress,
	bool *aborted,
	const ucstringVector &extraInfo,
	TCObject *sender)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress,
		extraInfo);

	TCAlertManager::sendAlert(alert, sender);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}
