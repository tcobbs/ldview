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

TCProgressAlert::~TCProgressAlert(void)
{
}

void TCProgressAlert::dealloc(void)
{
	delete m_source;
	TCAlert::dealloc();
}

void TCProgressAlert::send(const char *source, const char *message,
						   float progress, bool *aborted)
{
	TCProgressAlert *alert = new TCProgressAlert(source, message, progress);

	TCAlertManager::sendAlert(alert);
	if (aborted)
	{
		*aborted = alert->getAborted();
	}
	alert->release();
}
