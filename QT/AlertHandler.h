#ifndef __ALERTHANDLER_H__
#define __ALERTHANDLER_H__

#include <TCFoundation/TCObject.h>

class ModelViewerWidget;
class LDLError;
class TCProgressAlert;
class TCAlert;

class AlertHandler : public TCObject
{
public:
	AlertHandler(ModelViewerWidget *mvw);
protected:
	~AlertHandler(void);
	virtual void dealloc(void);
	void ldlErrorCallback(LDLError *error);
	void progressAlertCallback(TCProgressAlert *alert);
	void modelViewerAlertCallback(TCAlert *alert);
	void userDefaultChangedAlertCallback(TCAlert *alert);

	ModelViewerWidget *m_mvw;
};

#endif // __ALERTHANDLER_H__
