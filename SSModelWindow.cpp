#include "SSModelWindow.h"
#include "SSConfigure.h"
#include "LDViewWindow.h"
#include <LDLib/LDUserDefaultsKeys.h>

#include <TCFoundation/myString.h>
#include <TCFoundation/TCUserDefaults.h>
#include <math.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

void debugOut(char *fmt, ...);

SSModelWindow::SSModelWindow(CUIWindow* parentWindow, int x, int y,
							 int width, int height)
	: ModelWindow(parentWindow, x, y, width, height)
	, shownOnce(FALSE)
	, screenWidth(GetSystemMetrics(SM_CXFULLSCREEN))
	, screenHeight(GetSystemMetrics(SM_CYFULLSCREEN) +
		GetSystemMetrics(SM_CYCAPTION))
	, ssSize(TCUserDefaults::longForKey(SS_SIZE_KEY, DEFAULT_SS_SIZE))
	, ssSpeed(TCUserDefaults::longForKey(SS_SPEED_KEY,
		DEFAULT_SS_SPEED))
	, ssRotationSpeed(TCUserDefaults::longForKey(SS_ROTATION_SPEED_KEY,
		DEFAULT_SS_ROT_SPEED))
#ifdef LDVIEW_NO_64
	, startTick(GetTickCount())
#else
	, startTick(GetTickCount64())
#endif
	, powerSaveTimeout(0)
	, ssFileMode(TCUserDefaults::longForKey(SS_FILE_MODE_KEY,
		DEFAULT_SS_FILE_MODE))
	, ssSleepWorkaround(TCUserDefaults::longForKey(
		SS_SLEEP_WORKAROUND_KEY, DEFAULT_SS_SLEEP_WORKAROUND) != 0)
	, ssRandomPrefSet(TCUserDefaults::longForKey(
		SS_RANDOM_PREF_SET_KEY, DEFAULT_SS_RANDOM_PREF_SET, false)
		!= 0)
{
#pragma warning(push)
#pragma warning(disable: 28159)
	srand(GetTickCount());
#pragma warning(pop)
	if (ssRandomPrefSet)
	{
		TCStringArray *sessionNames = TCUserDefaults::getAllSessionNames();

		if (sessionNames)
		{
			int count = sessionNames->getCount();

			if (count)
			{
				int index = rand() % count;

				TCUserDefaults::setSessionName((*sessionNames)[index]);
				if (prefs)
				{
					prefs->loadSettings();
					prefs->applySettings();
				}
			}
			sessionNames->release();
		}
	}
	modelViewer->setDistanceMultiplier(1.01f);
}

SSModelWindow::~SSModelWindow(void)
{
}

void SSModelWindow::dealloc(void)
{
	ModelWindow::dealloc();
}

void SSModelWindow::getMatchingFiles(char *dir, char *filespec,
									 TCStringArray *filenames)
{
	UCCHAR fullFilespec[MAX_PATH];
	UCCHAR fullPath[MAX_PATH];
	WIN32_FIND_DATA findData;
	HANDLE hFind;

	sucprintf(fullFilespec, COUNT_OF(fullFilespec), _UC("%s\\%s"), dir, filespec);
	hFind = FindFirstFile(fullFilespec, &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			sucprintf(fullPath, COUNT_OF(fullPath), _UC("%s\\%s"), dir, findData.cFileName);
			std::string utf8Path;
			ucstringtoutf8(utf8Path, fullPath);
			filenames->addString(utf8Path.c_str());
		}
		while (FindNextFile(hFind, &findData))
		{
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				sucprintf(fullPath, COUNT_OF(fullPath), _UC("%s\\%s"), dir, findData.cFileName);
				std::string utf8Path;
				ucstringtoutf8(utf8Path, fullPath);
				filenames->addString(utf8Path.c_str());
			}
		}
		FindClose(hFind);
	}
}

void SSModelWindow::initSSFilename(void)
{
	if (ssFileMode == 0)
	{
		ssFilename = TCUserDefaults::stringForKeyUC(SS_FILENAME_KEY);
	}
	else
	{
		char *dir = TCUserDefaults::stringForKey(SS_DIRECTORY_KEY);

		if (dir)
		{
			TCStringArray *filenames = new TCStringArray;
			int count;

			getMatchingFiles(dir, "*.dat", filenames);
			getMatchingFiles(dir, "*.mpd", filenames);
			getMatchingFiles(dir, "*.ldr", filenames);
			count = filenames->getCount();
			if (count)
			{
				utf8toucstring(ssFilename, (*filenames)[rand() % count]);
			}
			filenames->release();
			delete dir;
		}
	}
}

int SSModelWindow::loadModel(void)
{
	int retVal = ModelWindow::loadModel();

	if (retVal)
	{
		modelViewer->setXRotate(1.0f);
		modelViewer->setYRotate(1.0f);
		modelViewer->setRotationSpeed(ssRotationSpeed / 100.0f);
	}
	return retVal;
}

void SSModelWindow::verifySsFilename(void)
{
	if (ssFilename.empty())
	{
		ssFilename = SSConfigure::defaultFilename();
	}
}

/*
WNDCLASSEX SSModelWindow::getWindowClass(void)
{
	WNDCLASSEX windowClass = CUIOGLWindow::getWindowClass();

	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hCursor = NULL;
	return windowClass;
}
*/

void SSModelWindow::finalSetup(void)
{
	if (!shownOnce)
	{
		shownOnce = TRUE;
		initSSFilename();
		resize(ssSize, ssSize);
		verifySsFilename();
		((LDViewWindow*)parentWindow)->openModel(ssFilename.c_str());
		ShowCursor(FALSE);
	}
}

/*
LRESULT SSModelWindow::doShowWindow(BOOL showFlag, LPARAM status)
{
	LRESULT retVal = ModelWindow::doShowWindow(showFlag, status);

	if (!shownOnce)
	{
		shownOnce = TRUE;
		parentWindow->maximize();
		resize(ssSize, ssSize);
	}
	return retVal;
}
*/

void SSModelWindow::doPaint(void)
{
//	RECT rect;
	double xScale = ((double)screenWidth - width) / 2.0;
	double yScale = ((double)screenHeight - height) / 2.0;
	double newX;
	double newY;
#ifdef LDVIEW_NO_64
	DWORD t = GetTickCount();
#else
	ULONGLONG t = GetTickCount64();
#endif

//	rect.left = x;
//	rect.right = x + width;
//	rect.top = y;
//	rect.bottom = y + height;
	newX = (sin(t * (ssSpeed + 3.0) / 15000.0) + 1.0) * xScale;
	newY = (sin(t * (ssSpeed + 3.0) / 13456.0) + 1.0) * yScale;
	modelViewer->setXRotate((GLfloat)sin(t / 1000.0) + 1.0f);
	modelViewer->setYRotate((GLfloat)cos(t / 1000.0) + 1.0f);
	MoveWindow(hWindow, (int)newX, (int)newY, width, height, FALSE);
	ModelWindow::doPaint();
	reallySwapBuffers();
	if (powerSaveTimeout > 0 &&
		(int)((t - startTick) / 1000) > powerSaveTimeout + 3)
	{
		// If we get here, we're running in Windows 98, and the power save
		// timeout has been reached (plus 3 seconds for good measure).
		SendMessage(hWindow, WM_CLOSE, 0, 0);
		showWindow(SW_HIDE);
	}
	InvalidateRect(parentWindow->getHWindow(), NULL, TRUE);
//	InvalidateRect(parentWindow->getHWindow(), &rect, TRUE);
}

BOOL SSModelWindow::showProgress(void)
{
	loading = TRUE;
	cancelLoad = FALSE;
	return TRUE;
}

void SSModelWindow::hideProgress(void)
{
	loading = FALSE;
}

int SSModelWindow::progressCallback(char* /*message*/, float /*progress*/)
{
	return 1;
}

void SSModelWindow::swapBuffers(void)
{
}

void SSModelWindow::reallySwapBuffers(void)
{
	ModelWindow::swapBuffers();
}

void SSModelWindow::showErrorsIfNeeded(BOOL /*onlyIfNeeded*/)
{
}

bool SSModelWindow::checkForExit(HWND hWnd, UINT message, WPARAM wParam,
								 LPARAM lParam)
{
	if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) ||
		(message >= WM_KEYFIRST && message <= WM_KEYLAST))
	{
		debugOut("SSModelWindow::checkForExit 0x%04X 0x%04X 0x%04X 0x%04X\n",
			hWnd, message, wParam, lParam);
		if (message == WM_MOUSEMOVE &&
			!((LDViewWindow*)parentWindow)->checkMouseMove(hWnd, lParam))
		{
			return false;
		}
		return true;
	}
	return false;
}

LRESULT SSModelWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam,
								  LPARAM lParam)
{
	if (checkForExit(hWnd, message, wParam, lParam))
	{
/*
		if (initializedGL && hglrc)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			hglrc = NULL;
			initializedGL = FALSE;
		}
*/
		((LDViewWindow*)parentWindow)->shutdown();
		return 1;
	}
	return CUIOGLWindow::windowProc(hWnd, message, wParam, lParam);
}

void SSModelWindow::closeWindow(void)
{
//	ShowWindow(hWindow, SW_HIDE);
	ModelWindow::closeWindow();
}

LRESULT SSModelWindow::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	LRESULT retValue;

	if (modelViewer)
	{
		modelViewer->setWidth(newWidth);
		modelViewer->setHeight(newHeight);
	}
	retValue = CUIOGLWindow::doSize(sizeType, newWidth, newHeight);
	return retValue;
}

