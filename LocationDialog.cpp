#include "LocationDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

LocationDialog::LocationDialog(HINSTANCE hInstance, int dialogId):
CUIDialog(hInstance, NULL)
, m_dialogId(dialogId)
, m_x(0.0)
, m_y(0.0)
, m_z(0.0)
{
}

LocationDialog::~LocationDialog(void)
{
}

void LocationDialog::dealloc(void)
{
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	CUIDialog::dealloc();
}

INT_PTR LocationDialog::doModal(ModelWindow *modelWindow)
{
	HWND hParentWnd = GetParent(modelWindow->getHWindow());

	return CUIDialog::doModal(m_dialogId, hParentWnd);
}

BOOL LocationDialog::doInitDialog(HWND /*hKbControl*/)
{
	windowSetValue(IDC_X, m_x);
	windowSetValue(IDC_Y, m_y);
	windowSetValue(IDC_Z, m_z);
	return TRUE;
}

BOOL LocationDialog::verifyField(
	UINT fieldID,
	float &value,
	float min,
	float max,
	bool checkMin /*= true*/,
	bool checkMax /*= true*/)
{
	if (windowGetValue(fieldID, value))
	{
		bool changed = false;

		if (checkMin && value < min)
		{
			value = min;
			changed = true;
		}
		if (checkMax && value > max)
		{
			value = max;
			changed = true;
		}
		if (changed)
		{
			windowSetValue(fieldID, value);
			textFieldSetSelection(fieldID, 0, -1);
			MessageBeep(MB_ICONHAND);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void LocationDialog::updateEnabled(void)
{
	BOOL xOK = verifyField(IDC_X, m_x, 0.0f, 0.0f, false, false);
	BOOL yOK = verifyField(IDC_Y, m_y, 0.0f, 0.0f, false, false);
	BOOL zOK = verifyField(IDC_Z, m_z, 0.0f, 0.0f, false, false);

	EnableWindow(GetDlgItem(hWindow, IDOK), xOK && yOK && zOK);
}

LRESULT LocationDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	updateEnabled();
	return 0;
}
