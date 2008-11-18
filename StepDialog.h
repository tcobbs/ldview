#ifndef __StepDialog_H__
#define __StepDialog_H__

class LDLMainModel;
class ModelWindow;
class TCAlert;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>

class StepDialog: public CUIDialog
{
public:
	StepDialog(CUIWindow *parentWindow, LDrawModelViewer *modelViewer);
	INT_PTR doModal(void);
	int getStep(void) const { return m_step; }
protected:
	virtual ~StepDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND hKbControl);
	virtual void doOK(void);

	LDrawModelViewer *m_modelViewer;
	int m_step;
};

#endif // __StepDialog_H__
