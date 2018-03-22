#include "LatLonDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

LatLonDialog::LatLonDialog(HINSTANCE hInstance):
CUIDialog(hInstance, NULL)
{
}

LatLonDialog::~LatLonDialog(void)
{
}

void LatLonDialog::dealloc(void)
{
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	CUIDialog::dealloc();
}

INT_PTR LatLonDialog::doModal(ModelWindow *modelWindow)
{
	HWND hParentWnd = GetParent(modelWindow->getHWindow());

	return CUIDialog::doModal(IDD_LAT_LON, hParentWnd);
}

void LatLonDialog::updateDistanceField(void)
{
	if (m_haveDistance)
	{
		windowSetValue(IDC_DISTANCE, m_distance);
	}
	else
	{
		windowSetText(IDC_DISTANCE, _UC(""));
	}
}

BOOL LatLonDialog::doInitDialog(HWND /*hKbControl*/)
{
	windowSetValue(IDC_LATITUDE,
		TCUserDefaults::floatForKey(LAST_LAT_KEY, 30.0f, false));
	windowSetValue(IDC_LONGITUDE,
		TCUserDefaults::floatForKey(LAST_LON_KEY, 45.0f, false));
	m_haveDistance = TCUserDefaults::boolForKey(LAST_HAVE_DIST_KEY, false,
		false);
	checkSet(IDC_DISTANCE_CHECK, m_haveDistance);
	m_distance = TCUserDefaults::floatForKey(LAST_DIST_KEY, m_defaultDistance,
		false);
	updateDistanceField();
	updateEnabled();
	setAutosaveName("LatLonDialog");
	return TRUE;
}

BOOL LatLonDialog::verifyField(
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

void LatLonDialog::updateEnabled(void)
{
	BOOL latOK = verifyField(IDC_LATITUDE, m_lat, -90.0f, 90.0f);
	BOOL lonOK = verifyField(IDC_LONGITUDE, m_lon, -180.0f, 180.0f);
	BOOL distOK = TRUE;
	BOOL distEnabled = FALSE;

	if (checkGet(IDC_DISTANCE_CHECK))
	{
		distOK = verifyField(IDC_DISTANCE, m_distance, 0.0f, 0.0f, true, false);
		distEnabled = TRUE;
	}
	EnableWindow(GetDlgItem(hWindow, IDC_DISTANCE), distEnabled);
	EnableWindow(GetDlgItem(hWindow, IDC_DEFAULT), distEnabled);
	EnableWindow(GetDlgItem(hWindow, IDC_CURRENT), distEnabled);
	//if (windowGetValue(IDC_LATITUDE, m_lat))
	//{
	//	bool changed = false;

	//	haveLat = TRUE;
	//	if (m_lat > 90.0f)
	//	{
	//		m_lat = 90.0f;
	//		changed = true;
	//	}
	//	else if (m_lat < -90.0f)
	//	{
	//		m_lat = -90.0f;
	//		changed = true;
	//	}
	//	if (changed)
	//	{
	//		windowSetValue(IDC_LATITUDE, m_lat);
	//		textFieldSetSelection(IDC_LATITUDE, 0, -1);
	//		MessageBeep(MB_ICONHAND);
	//	}
	//}
	//if (windowGetValue(IDC_LONGITUDE, m_lon))
	//{
	//	bool changed = false;

	//	haveLon = TRUE;
	//	if (m_lon > 180.0f)
	//	{
	//		m_lon = 180.0f;
	//		changed = true;
	//	}
	//	else if (m_lon < -180.0f)
	//	{
	//		m_lon = -180.0f;
	//		changed = true;
	//	}
	//	if (changed)
	//	{
	//		windowSetValue(IDC_LONGITUDE, m_lon);
	//		textFieldSetSelection(IDC_LONGITUDE, 0, -1);
	//		MessageBeep(MB_ICONHAND);
	//	}
	//}
	EnableWindow(GetDlgItem(hWindow, IDOK), latOK && lonOK);
}

LRESULT LatLonDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	updateEnabled();
	return 0;
}

LRESULT LatLonDialog::doDistanceCheck(void)
{
	m_haveDistance = checkGet(IDC_DISTANCE_CHECK);
	updateDistanceField();
	updateEnabled();
	return 0;
}

LRESULT LatLonDialog::doDistanceReset(float value)
{
	m_distance = value;
	updateDistanceField();
	return 0;
}

LRESULT LatLonDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (notifyCode == BN_CLICKED)
	{
		switch (commandId)
		{
		case IDC_DISTANCE_CHECK:
			return doDistanceCheck();
		case IDC_DEFAULT:
			return doDistanceReset(m_defaultDistance);
		case IDC_CURRENT:
			return doDistanceReset(m_currentDistance);
		}
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void LatLonDialog::doOK(void)
{
	TCUserDefaults::setFloatForKey(m_lat, LAST_LAT_KEY, false);
	TCUserDefaults::setFloatForKey(m_lon, LAST_LON_KEY, false);
	TCUserDefaults::setBoolForKey(m_haveDistance, LAST_HAVE_DIST_KEY, false);
	if (m_haveDistance)
	{
		TCUserDefaults::setFloatForKey(m_distance, LAST_DIST_KEY, false);
	}
	CUIDialog::doOK();
}

float LatLonDialog::getDistance(void) const
{
	if (m_haveDistance)
	{
		return m_distance;
	}
	else
	{
		return -1.0;
	}
}
