#ifndef __CAMERALOCATIONDIALOG_H__
#define __CAMERALOCATIONDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include "LocationDialog.h"
#include <LDLib/LDrawModelViewer.h>

class CameraLocationDialog: public LocationDialog
{
public:
	CameraLocationDialog(HINSTANCE hInstance);
	LDVLookAt getLookAt(void) const { return m_lookAt; }
protected:
	virtual ~CameraLocationDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doCommand(int notifyCode, int commandId, HWND control);
	virtual void doOK(void);

	LRESULT doLookAtCheck(int buttonId, LDVLookAt lookAtValue);

	void updateLookAtChecks(void);

	LDVLookAt m_lookAt;
};

#endif // __CAMERALOCATIONDIALOG_H__
