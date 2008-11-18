#include "StepDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <TCFoundation/TCAlertManager.h>
#include <LDLoader/LDLMainModel.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

StepDialog::StepDialog(CUIWindow *parentWindow, LDrawModelViewer *modelViewer):
CUIDialog(parentWindow),
m_modelViewer(modelViewer),
m_step(-1)
{
}

StepDialog::~StepDialog(void)
{
}

void StepDialog::dealloc(void)
{
	CUIDialog::dealloc();
}

INT_PTR StepDialog::doModal(void)
{
	return CUIDialog::doModal(IDD_STEP, parentWindow->getHWindow());
}

BOOL StepDialog::doInitDialog(HWND hKbControl)
{
	windowSetValue(IDC_STEP_FIELD, (long)m_modelViewer->getStep());
	return CUIDialog::doInitDialog(hKbControl);
}

void StepDialog::doOK(void)
{
	long value;

	if (windowGetValue(IDC_STEP_FIELD, value))
	{
		m_step = (int)value;
		CUIDialog::doOK();
	}
	else
	{
		MessageBeep(MB_ICONASTERISK);
	}
}
