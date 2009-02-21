#ifndef __BOUNDINGBOXDIALOG_H__
#define __BOUNDINGBOXDIALOG_H__

class LDLMainModel;
class ModelWindow;
class TCAlert;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>

class BoundingBoxDialog: public CUIDialog
{
public:
	BoundingBoxDialog(HINSTANCE hInstance);
	void toggle(ModelWindow *modelWindow);
	bool isVisible(void);
protected:
	virtual LRESULT doClose(void);

	virtual ~BoundingBoxDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);

	void modelAlertCallback(TCAlert *alert);
	void setModel(LDLMainModel *model);
	void setModelWindow(ModelWindow *modelWindow);
	void updateData(void);
	void showBoundingBox(bool value);
	LDrawModelViewer *getModelViewer(void);

	ModelWindow *m_modelWindow;
	LDLMainModel *m_model;
};

#endif // __BOUNDINGBOXDIALOG_H__
