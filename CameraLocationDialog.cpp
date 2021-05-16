#include "CameraLocationDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

CameraLocationDialog::CameraLocationDialog(HINSTANCE hInstance):
CUIDialog(hInstance, NULL)
, m_x(0.0)
, m_y(0.0)
, m_z(0.0)
{
}

CameraLocationDialog::~CameraLocationDialog(void)
{
}

void CameraLocationDialog::dealloc(void)
{
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	CUIDialog::dealloc();
}

INT_PTR CameraLocationDialog::doModal(ModelWindow *modelWindow)
{
	HWND hParentWnd = GetParent(modelWindow->getHWindow());

	return CUIDialog::doModal(IDD_CAMERA_LOCATION, hParentWnd);
}

BOOL CameraLocationDialog::doInitDialog(HWND /*hKbControl*/)
{
	windowSetValue(IDC_X, m_x);
	windowSetValue(IDC_Y, m_y);
	windowSetValue(IDC_Z, m_z);
	m_lookAt = (LDVLookAt)TCUserDefaults::longForKey(
		CAMERA_LOCATION_LOOK_AT_KEY, LDVLookAtModel, false);
	updateLookAtChecks();
	updateEnabled();
	setAutosaveName("CameraLocationDialog");
	return TRUE;
}

void CameraLocationDialog::updateLookAtChecks(void)
{
	checkSet(IDC_LOOKATMODEL_CHECK, m_lookAt == LDVLookAtModel);
	checkSet(IDC_LOOKATORIGIN_CHECK, m_lookAt == LDVLookAtOrigin);
}

BOOL CameraLocationDialog::verifyField(
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

void CameraLocationDialog::updateEnabled(void)
{
	BOOL xOK = verifyField(IDC_X, m_x, 0.0f, 0.0f, false, false);
	BOOL yOK = verifyField(IDC_Y, m_y, 0.0f, 0.0f, false, false);
	BOOL zOK = verifyField(IDC_Z, m_z, 0.0f, 0.0f, false, false);

	EnableWindow(GetDlgItem(hWindow, IDOK), xOK && yOK && zOK);
}

LRESULT CameraLocationDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	updateEnabled();
	return 0;
}

LRESULT CameraLocationDialog::doLookAtCheck(int buttonId, LDVLookAt lookAtValue)
{
	if (checkGet(buttonId))
	{
		m_lookAt = lookAtValue;
	}
	else
	{
		m_lookAt = LDVLookAtNone;
	}
	updateLookAtChecks();
	return 0;
}

LRESULT CameraLocationDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (notifyCode == BN_CLICKED)
	{
		switch (commandId)
		{
		case IDC_LOOKATMODEL_CHECK:
			return doLookAtCheck(IDC_LOOKATMODEL_CHECK, LDVLookAtModel);
		case IDC_LOOKATORIGIN_CHECK:
			return doLookAtCheck(IDC_LOOKATORIGIN_CHECK, LDVLookAtOrigin);
		}
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void CameraLocationDialog::doOK(void)
{
	TCUserDefaults::setLongForKey(m_lookAt, CAMERA_LOCATION_LOOK_AT_KEY, false);
	CUIDialog::doOK();
}
