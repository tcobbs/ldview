#ifndef __MPDDIALOG_H__
#define __MPDDIALOG_H__

class LDLMainModel;
class ModelWindow;
class TCAlert;
class LDrawModelViewer;
class CUIWindowResizer;

#include <CUI/CUIDialog.h>

class MpdDialog: public CUIDialog
{
public:
	MpdDialog(HINSTANCE hInstance);
	void show(ModelWindow *modelWindow);
protected:
	virtual LRESULT doClose(void);

	virtual ~MpdDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual void doOK(void);
	virtual void doCancel(void);

	void modelAlertCallback(TCAlert *alert);
	void setModel(LDLMainModel *model);
	void setModelWindow(ModelWindow *modelWindow);
	void updateData(void);
	void showMpdModel(int index);
	LDrawModelViewer *getModelViewer(void);

	ModelWindow *m_modelWindow;
	LDLMainModel *m_model;
	CUIWindowResizer *m_resizer;
	bool m_okPressed;
};

#endif // __MPDDIALOG_H__
