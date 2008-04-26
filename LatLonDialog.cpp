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

BOOL LatLonDialog::doInitDialog(HWND /*hKbControl*/)
{
	windowSetValue(IDC_LATITUDE,
		TCUserDefaults::floatForKey(LAST_LAT_KEY, 30.0f, false));
	windowSetValue(IDC_LONGITUDE,
		TCUserDefaults::floatForKey(LAST_LON_KEY, 45.0f, false));
	updateEnabled();
	return TRUE;
}

void LatLonDialog::updateEnabled(void)
{
	BOOL haveLat = FALSE;
	BOOL haveLon = FALSE;

	if (windowGetValue(IDC_LATITUDE, m_lat))
	{
		bool changed = false;

		haveLat = TRUE;
		if (m_lat > 90.0f)
		{
			m_lat = 90.0f;
			changed = true;
		}
		else if (m_lat < -90.0f)
		{
			m_lat = -90.0f;
			changed = true;
		}
		if (changed)
		{
			windowSetValue(IDC_LATITUDE, m_lat);
			textFieldSetSelection(IDC_LATITUDE, 0, -1);
			MessageBeep(MB_ICONHAND);
		}
	}
	if (windowGetValue(IDC_LONGITUDE, m_lon))
	{
		bool changed = false;

		haveLon = TRUE;
		if (m_lon > 180.0f)
		{
			m_lon = 180.0f;
			changed = true;
		}
		else if (m_lon < -180.0f)
		{
			m_lon = -180.0f;
			changed = true;
		}
		if (changed)
		{
			windowSetValue(IDC_LONGITUDE, m_lon);
			textFieldSetSelection(IDC_LONGITUDE, 0, -1);
			MessageBeep(MB_ICONHAND);
		}
	}
	EnableWindow(GetDlgItem(hWindow, IDOK), haveLat && haveLon);
}

LRESULT LatLonDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	updateEnabled();
	return 0;
}

void LatLonDialog::doOK(void)
{
	TCUserDefaults::setFloatForKey(m_lat, LAST_LAT_KEY, false);
	TCUserDefaults::setFloatForKey(m_lon, LAST_LON_KEY, false);
	CUIDialog::doOK();
}
