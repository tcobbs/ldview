#include "RotationCenterDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

RotationCenterDialog::RotationCenterDialog(HINSTANCE hInstance):
LocationDialog(hInstance, IDD_ROTATION_CENTER)
{
}

RotationCenterDialog::~RotationCenterDialog(void)
{
}

void RotationCenterDialog::dealloc(void)
{
	LocationDialog::dealloc();
}

BOOL RotationCenterDialog::doInitDialog(HWND hKbControl)
{
	LocationDialog::doInitDialog(hKbControl);
	updateEnabled();
	setAutosaveName("RotationCenterDialog");
	return TRUE;
}

LRESULT RotationCenterDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (notifyCode == BN_CLICKED)
	{
		switch (commandId)
		{
		case IDC_RESET:
			return doReset();
		}
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

LRESULT RotationCenterDialog::doReset(void)
{
	EndDialog(hWindow, (INT_PTR)IDC_RESET);
	return 0;
}
