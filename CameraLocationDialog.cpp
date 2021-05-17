#include "CameraLocationDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

CameraLocationDialog::CameraLocationDialog(HINSTANCE hInstance):
LocationDialog(hInstance, IDD_CAMERA_LOCATION)
, m_lookAt(LDVLookAtModel)
{
}

CameraLocationDialog::~CameraLocationDialog(void)
{
}

void CameraLocationDialog::dealloc(void)
{
	LocationDialog::dealloc();
}

BOOL CameraLocationDialog::doInitDialog(HWND hKbControl)
{
	LocationDialog::doInitDialog(hKbControl);
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
