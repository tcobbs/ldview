#ifndef __ROTATIONCENTERDIALOG_H__
#define __ROTATIONCENTERDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include "LocationDialog.h"
#include <LDLib/LDrawModelViewer.h>

class RotationCenterDialog : public LocationDialog
{
public:
	RotationCenterDialog(HINSTANCE hInstance);
protected:
	virtual ~RotationCenterDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doCommand(int notifyCode, int commandId, HWND control);
	virtual LRESULT doReset(void);
};

#endif // __ROTATIONCENTERDIALOG_H__
