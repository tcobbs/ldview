#include "ModelWindow.h"
#include <LDLib/LDrawModelViewer.h>
#include <TCFoundation/TCMacros.h>
//#include <LDLib/TGLStudLogo.h>
#include "LDVExtensionsSetup.h"
#include "LDViewWindow.h"
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <LDLoader/LDLError.h>
#include <LDLoader/LDLModel.h>
#include <LDLib/LDLibraryUpdater.h>
#include "AppResources.h"
#include <Commctrl.h>
#include "UserDefaultsKeys.h"
#include <CUI/CUIWindowResizer.h>
#include <TRE/TREMainModel.h>
#include <windowsx.h>

#define DISTANCE_MULT 1.325f

#define POLL_INTERVAL 500
#define POLL_TIMER 1

#define FSAA_UPDATE_TIMER 2

#define PNG_IMAGE_TYPE_INDEX 1
#define BMP_IMAGE_TYPE_INDEX 2

#define MAX_SNAPSHOT_WIDTH 10000
#define MAX_SNAPSHOT_HEIGHT 10000

#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) \
      ListView_SetItemState(hwndLV, i, \
      INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif

ControlInfo::ControlInfo(void)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "ControlInfo");
#endif
}

ErrorInfo::ErrorInfo(void)
	:m_typeName(NULL)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "ErrorInfo");
#endif
}

ErrorInfo::~ErrorInfo(void)
{
}

void ErrorInfo::dealloc(void)
{
	delete m_typeName;
	TCObject::dealloc();
}

void ErrorInfo::setTypeName(const char *typeName)
{
	if (typeName != m_typeName)
	{
		delete m_typeName;
		m_typeName = copyString(typeName);
	}
}

ModelWindow::ModelWindow(CUIWindow* parentWindow, int x, int y,
						 int width, int height)
			:CUIOGLWindow(parentWindow, x, y, width, height),
			 modelViewer(new LDrawModelViewer(width, height)),
			 numFramesSinceReference(0),
			 firstFPSPass(true),
			 rotationSpeed(0.0f),
			 lButtonDown(0),
			 rButtonDown(0),
			 hPrefsWindow(NULL),
			 captureCount(0),
			 redrawCount(0),
			 hProgressWindow(NULL),
			 lastProgressUpdate(0),
			 loading(false),
			 needsRecompile(false),
			 hErrorWindow(NULL),
			 errors(new LDLErrorArray),
			 errorTreePopulated(false),
			 errorInfos(NULL),
			 prefs(new LDViewPreferences(parentWindow->getHInstance(),
				modelViewer)),
			 applyingPrefs(false),
			 offscreenActive(false),
			 hPBuffer(NULL),
			 hPBufferDC(NULL),
			 hPBufferGLRC(NULL),
			 hPrintDialog(NULL),
			 hStatusBar(NULL),
			 hProgressBar(NULL),
			 windowShown(false),
			 hCurrentDC(NULL),
			 hCurrentGLRC(NULL),
			 errorWindowResizer(NULL),
			 savingFromCommandLine(false),
			 skipErrorUpdates(false)
{
	char *programPath = LDViewPreferences::getLDViewPath();
	HRSRC hStudLogoResource = FindResource(NULL,
		MAKEINTRESOURCE(IDR_STUDLOGO_PNG), "PNG");
	HRSRC hFontResource = FindResource(NULL, MAKEINTRESOURCE(IDR_SANS_FONT),
		"TCFont");

	loadSettings();
	if (hStudLogoResource)
	{
		HGLOBAL hStudLogo = LoadResource(NULL, hStudLogoResource);

		if (hStudLogo)
		{
			TCByte *data = (TCByte *)LockResource(hStudLogo);

			if (data)
			{
				DWORD length = SizeofResource(NULL, hStudLogoResource);

				if (length)
				{
					TREMainModel::setStudTextureData(data, length);
				}
			}
		}
	}
	if (hFontResource)
	{
		HGLOBAL hFont = LoadResource(NULL, hFontResource);

		if (hFont)
		{
			TCByte *data = (TCByte *)LockResource(hFont);

			if (data)
			{
				DWORD length = SizeofResource(NULL, hFontResource);

				if (length)
				{
					modelViewer->setFontData(data, length);
				}
			}
		}
	}
	windowStyle = windowStyle & ~WS_VISIBLE;
	TCAlertManager::registerHandler(LDLError::alertClass(), this,
		(TCAlertCallback)ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
		(TCAlertCallback)progressAlertCallback);
/*
	modelViewer->setProgressCallback(staticProgressCallback, this);
	modelViewer->setErrorCallback(staticErrorCallback, this);
*/
	if (programPath)
	{
		modelViewer->setProgramPath(programPath);
		TCUserDefaults::setStringForKey(programPath, INSTALL_PATH_KEY, false);
		delete programPath;
	}
}

ModelWindow::~ModelWindow(void)
{
}

void ModelWindow::dealloc(void)
{
	TCAlertManager::unregisterHandler(LDLError::alertClass(), this);
	TCAlertManager::unregisterHandler(TCProgressAlert::alertClass(), this);
	TCObject::release(errorInfos);
	errorInfos = NULL;
	if (prefs)
	{
		prefs->release();
		prefs = NULL;
	}
	if (errors)
	{
		errors->release();
		errors = NULL;
	}
	if (modelViewer)
	{
		modelViewer->release();
		modelViewer = NULL;
	}
	if (errorWindowResizer)
	{
		errorWindowResizer->release();
	}
	stopPolling();
	CUIOGLWindow::dealloc();
}

void ModelWindow::ldlErrorCallback(LDLError *error)
{
	static int errorCount = 0;

	if (error)
	{
		if (!errorCallback(error))
		{
			error->cancelLoad();
		}
	}
/*
		TCStringArray *extraInfo = error->getExtraInfo();

		printf("Error on line %d in: %s\n", error->getLineNumber(),
			error->getFilename());
		indentPrintf(4, "%s\n", error->getMessage());
		indentPrintf(4, "%s\n", error->getFileLine());
		if (extraInfo)
		{
			int i;
			int count = extraInfo->getCount();

			for (i = 0; i < count; i++)
			{
				indentPrintf(4, "%s\n", (*extraInfo)[i]);
			}
		}
	}
*/
}

void ModelWindow::progressAlertCallback(TCProgressAlert *alert)
{
	if (alert)
	{
		if (strcmp(alert->getSource(), LD_LIBRARY_UPDATER) == 0)
		{
			debugPrintf("Updater progress (%s): %f\n", alert->getMessage(),
				alert->getProgress());
			if (alert->getProgress() == 1.0f)
			{
				if (alert->getExtraInfo())
				{
					if (strcmp((*(alert->getExtraInfo()))[0], "None") == 0)
					{
						PostMessage(hParentWindow, WM_COMMAND,
							MAKELONG(BN_CLICKED, LIBRARY_UPDATE_NONE), 0);
					}
					else
					{
						PostMessage(hParentWindow, WM_COMMAND,
							MAKELONG(BN_CLICKED, LIBRARY_UPDATE_FINISHED), 0);
					}
				}
				else
				{
					PostMessage(hParentWindow, WM_COMMAND,
						MAKELONG(BN_CLICKED, LIBRARY_UPDATE_CANCELED), 0);
				}
			}
			else
			{
				PostMessage(hProgressBar, PBM_SETPOS,
					(int)(alert->getProgress() * 100), 0);
			}
		}
		else
		{
			bool showErrors = true;

			if (strcmp(alert->getSource(), "TCImage") == 0)
			{
				showErrors = false;
			}
			if (!progressCallback(alert->getMessage(), alert->getProgress(),
				showErrors))
			{
				alert->abort();
			}
//			printf("Progress message from %s:\n%s (%f)\n", alert->getSource(),
//				alert->getMessage(), alert->getProgress());
		}
	}
}

void ModelWindow::loadSettings(void)
{
	pollSetting = TCUserDefaults::longForKey(POLL_KEY, 0, false);
	printLeftMargin = TCUserDefaults::longForKey(LEFT_MARGIN_KEY, 500, false) /
		1000.0f;
	printRightMargin = TCUserDefaults::longForKey(RIGHT_MARGIN_KEY, 500,
		false) / 1000.0f;
	printTopMargin = TCUserDefaults::longForKey(TOP_MARGIN_KEY, 500, false) /
		1000.0f;
	printBottomMargin = TCUserDefaults::longForKey(BOTTOM_MARGIN_KEY, 500,
		false) / 1000.0f;
	printOrientation = TCUserDefaults::longForKey(ORIENTATION_KEY,
		DMORIENT_PORTRAIT, false);
	printPaperSize = TCUserDefaults::longForKey(PAPER_SIZE_KEY, DMPAPER_LETTER,
		false);
	saveActualSize = TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false)
		!= 0;
	saveWidth = TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false);
	saveHeight = TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false);
	saveZoomToFit = TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1, false)
		!= 0;
	saveSeries = TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0;
	saveDigits = TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false);
	usePrinterDPI = TCUserDefaults::longForKey(USE_PRINTER_DPI_KEY, 1) != 0;
	printDPI = TCUserDefaults::longForKey(PRINT_DPI_KEY, 300);
	printBackground = TCUserDefaults::longForKey(PRINT_BACKGROUND_KEY, 0, false)
		!= 0;
	saveImageType = TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false);
	saveAlpha = TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0;
	viewMode = (LDVViewMode)TCUserDefaults::longForKey(VIEW_MODE_KEY, 0, false);
}

int ModelWindow::roundUp(int value, int nearest)
{
	return (value + nearest - 1) / nearest * nearest;
}

void ModelWindow::setFilename(const char* value)
{
	modelViewer->setFilename(value);
	checkForPart();
}

char* ModelWindow::getFilename(void)
{
	return modelViewer->getFilename();
}

void ModelWindow::update(void)
{
	doPaint();
}

void ModelWindow::finalSetup(void)
{
}

BOOL ModelWindow::getFileTime(FILETIME* fileTime)
{
	char* filename;

	if (modelViewer && (filename = modelViewer->getFilename()) != NULL)
	{
		WIN32_FIND_DATA findBuf;
		HANDLE findHandle;

		findHandle = FindFirstFile(filename, &findBuf);
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			*fileTime = findBuf.ftLastWriteTime;
			FindClose(findHandle);
			return YES;
		}
/*
		WIN32_FILE_ATTRIBUTE_DATA fileAttributes;

		if (GetFileAttributesEx(filename, GetFileExInfoStandard,
			&fileAttributes))
		{
			*fileTime = fileAttributes.ftLastWriteTime;
			return YES;
		}
*/
	}
	return NO;
}

void ModelWindow::checkFileForUpdates(void)
{
	if (pollTimerRunning)
	{
		FILETIME newWriteTime;

		stopPolling();
		if (getFileTime(&newWriteTime))
		{
			if (lastWriteTime.dwLowDateTime !=newWriteTime.dwLowDateTime ||
				lastWriteTime.dwHighDateTime != newWriteTime.dwHighDateTime)
			{
				BOOL update = YES;

				lastWriteTime = newWriteTime;
				if (pollSetting == POLL_PROMPT)
				{
					char message[1024];

					sprintf(message, "The model has been modified.\n"
						"Do you want to reload?");
					while (captureCount)
					{
						releaseMouse();
					}
					lButtonDown = NO;
					rButtonDown = NO;
					if (MessageBox(hWindow, message, "File updated",
						MB_OKCANCEL | MB_APPLMODAL | MB_ICONQUESTION) !=
						IDOK)
					{
						update = NO;
					}
				}
				if (update)
				{
					reload();
					forceRedraw();
				}
			}
		}
		startPolling();
	}
}

void ModelWindow::updateFSAA()
{
	applyingPrefs = true;
	uncompile();
	closeWindow();
	if (!((LDViewWindow*)parentWindow)->getFullScreen())
	{
		x -= 2;
		y -= 2;
		width += 4;
		height += 4;
	}
	initWindow();
	setNeedsRecompile();
	showWindow(SW_SHOW);
	applyingPrefs = false;
	killTimer(FSAA_UPDATE_TIMER);
	forceRedraw();
	if (hStatusBar)
	{
		// For some unknown reason, the status bar only updates its text on the
		// fly while the model is rotating if it is created after the model
		// window.  Since we just destroyed and recreated the model window, we
		// need to destroy and recreate the status window.
		((LDViewWindow*)parentWindow)->switchStatusBar();
		((LDViewWindow*)parentWindow)->switchStatusBar();
	}
}

LRESULT ModelWindow::doTimer(UINT timerID)
{
	switch (timerID)
	{
	case POLL_TIMER:
		checkFileForUpdates();
		break;
	case FSAA_UPDATE_TIMER:
		updateFSAA();
		break;
	}
	return 0;
}

void ModelWindow::updateSpinRateXY(int xPos, int yPos)
{
	int deltaX = xPos - lastX;
	int deltaY = yPos - lastY;
	float magnitude = (float)sqrt((float)(deltaX * deltaX + deltaY * deltaY));

	lastX = xPos;
	lastY = yPos;
	rotationSpeed = magnitude / 10.0f;
	if (fEq(rotationSpeed, 0.0f))
	{
		rotationSpeed = 0.0f;
		modelViewer->setXRotate(1.0f);
		modelViewer->setYRotate(1.0f);
	}
	else
	{
		modelViewer->setXRotate((float)deltaY);
		modelViewer->setYRotate((float)deltaX);
	}
	modelViewer->setRotationSpeed(rotationSpeed);
}

void ModelWindow::updatePanXY(int xPos, int yPos)
{
	int deltaX = xPos - lastX;
	int deltaY = yPos - lastY;

	lastX = xPos;
	lastY = yPos;
	modelViewer->panXY(deltaX, deltaY);
}

void ModelWindow::setXRotate(float value)
{
	modelViewer->setXRotate(value);
}

void ModelWindow::setYRotate(float value)
{
	modelViewer->setYRotate(value);
}

void ModelWindow::setRotationSpeed(float value)
{
	rotationSpeed = value;
	modelViewer->setRotationSpeed(rotationSpeed);
}

void ModelWindow::updateZoom(int yPos)
{
	float magnitude = (float)(yPos - originalZoomY);

//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%d %d\n", originalZoomY, yPos);
	modelViewer->setZoomSpeed(magnitude / 2.0f);
}

void ModelWindow::updateHeadXY(int xPos, int yPos)
{
	float magnitude = (float)(xPos - lastX);
	float denom = 5000.0f;

	if (modelViewer)
	{
		float fov = modelViewer->getFov();

		denom /= (float)tan(deg2rad(fov));
	}
	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
		denom /= 2.0f;
	}
	modelViewer->setCameraXRotate(magnitude / denom);
	magnitude = (float)(yPos - lastY);
	modelViewer->setCameraYRotate(magnitude / -denom);
}

void ModelWindow::updateSpinRate(void)
{
	if (lButtonDown)
	{
		DWORD thisMoveTime = timeGetTime();

		if (thisMoveTime - lastMoveTime >= 100)
		{
			updateSpinRateXY(lastX, lastY);
			lButtonDown = 1;
		}
	}
}

void ModelWindow::captureMouse(void)
{
	oldMouseWindow = GetCapture();
	if (oldMouseWindow != hWindow)
	{
		SetCapture(hWindow);
	}
	captureCount++;
}

void ModelWindow::releaseMouse(void)
{
	if (captureCount)
	{
		captureCount--;
		if (!captureCount)
		{
			SetCapture(oldMouseWindow);
		}
	}
}

LRESULT ModelWindow::doLButtonDown(WPARAM /*keyFlags*/, int xPos, int yPos)
{
	forceRedraw();
	if (rButtonDown)
	{
		return 1;
	}
	else
	{
		lastX = xPos;
		lastY = yPos;
		if (viewMode == LDVViewExamine)
		{
			updateSpinRateXY(xPos, yPos);
		}
		lButtonDown = 1;
		captureMouse();
		return 0;
	}
}

LRESULT ModelWindow::doLButtonUp(WPARAM /*keyFlags*/, int /*xPos*/, int /*yPos*/)
{
	forceRedraw();
	if (lButtonDown)
	{
		lButtonDown = 0;
		releaseMouse();
		modelViewer->setCameraXRotate(0.0f);
		modelViewer->setCameraYRotate(0.0f);
		return 0;
	}
	else
	{
		return 1;
	}
}

LRESULT ModelWindow::doRButtonDown(WPARAM /*keyFlags*/, int /*xPos*/, int yPos)
{
	forceRedraw();
	if (lButtonDown)
	{
		return 1;
	}
	else
	{
		if (viewMode == LDVViewExamine)
		{
			originalZoomY = yPos;
			rButtonDown = 1;
			captureMouse();
		}
		return 0;
	}
}

LRESULT ModelWindow::doRButtonUp(WPARAM /*keyFlags*/, int /*xPos*/, int /*yPos*/)
{
	forceRedraw();
	if (rButtonDown)
	{
		rButtonDown = 0;
		modelViewer->setZoomSpeed(0.0f);
		releaseMouse();
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
LRESULT ModelWindow::doMButtonUp(WPARAM keyFlags, int xPos, int yPos)
{
	forceRedraw();
	if (showPreferences())
	{
		return 0;
	}
	return 1;
}
*/

LRESULT ModelWindow::doMouseMove(WPARAM keyFlags, int xPos, int yPos)
{
	forceRedraw();
	if (lButtonDown || rButtonDown)
	{
		lastMoveTime = timeGetTime();
	}
	if (lButtonDown)
	{
		if (viewMode == LDVViewExamine)
		{
			if (keyFlags & MK_CONTROL)
			{
				updatePanXY(xPos, yPos);
			}
			else
			{
				updateSpinRateXY(xPos, yPos);
			}
		}
		else
		{
			updateHeadXY(xPos, yPos);
		}
		return 0;
	}
	if (rButtonDown)
	{
		if (keyFlags & MK_CONTROL)
		{
			modelViewer->setClipZoom(YES);
		}
		else
		{
			modelViewer->setClipZoom(NO);
		}
		updateZoom(yPos);
		return 0;
	}
	return 1;
}

LRESULT ModelWindow::doEraseBackground(RECT* updateRect)
{
	CUIOGLWindow::doEraseBackground(updateRect);
	return 1;
}

LRESULT ModelWindow::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	RECT rect;
	POINT point;
	LRESULT retValue;

	GetWindowRect(hWindow, &rect);
	point.x = rect.left;
	point.y = rect.top;
	ScreenToClient(hParentWindow, &point);
	if (modelViewer && !hPBufferGLRC)
	{
		modelViewer->setWidth(newWidth);
		modelViewer->setHeight(newHeight);
	}
	if (hPBufferGLRC)
	{
		wglMakeCurrent(hdc, hglrc);
	}
	retValue = CUIOGLWindow::doSize(sizeType, newWidth, newHeight);
	if (hPBufferGLRC)
	{
		makeCurrent();
	}
	return retValue;
}

void ModelWindow::sizeView(void)
{
}

void ModelWindow::perspectiveView(void)
{
	modelViewer->perspectiveView();
}

void ModelWindow::forceRedraw(int count)
{
	if (hWindow)
	{
		InvalidateRect(hWindow, NULL, FALSE);
	}
	redrawCount += count;
}

/*
void ModelWindow::getBackgroundRGB(int& r, int& g, int& b)
{
	r = backgroundColor & 0xFF;
	g = (backgroundColor >> 8) & 0xFF;
	b = (backgroundColor >> 16) & 0xFF;
}
*/

void ModelWindow::setZoomSpeed(float value)
{
	modelViewer->setZoomSpeed(value);
}

float ModelWindow::getZoomSpeed(void)
{
	return modelViewer->getZoomSpeed();
}

/*
BOOL ModelWindow::doDialogVScroll(HWND hDlg, int scrollCode, int position,
								  HWND hScrollBar)
{
	if (hDlg == hPrefsWindow)
	{
//		return prefs->doVScroll(hDlg, scrollCode, position, hScrollBar);
	}
	return FALSE;
}
*/

void ModelWindow::resetView(LDVAngle viewAngle)
{
	modelViewer->resetView(viewAngle);
	rotationSpeed = 0.0f;
	modelViewer->setXRotate(1.0f);
	modelViewer->setYRotate(1.0f);
	modelViewer->setRotationSpeed(0.0f);
	modelViewer->setZoomSpeed(0.0f);
	fps = 0.0f;
	forceRedraw();
}

void ModelWindow::saveDefaultView(void)
{
	prefs->saveDefaultView();
}

void ModelWindow::resetDefaultView(void)
{
	prefs->resetDefaultView();
}

void ModelWindow::reload(void)
{
	clearErrors();
	modelViewer->reload();
}

void ModelWindow::recompile(void)
{
	modelViewer->recompile();
}

void ModelWindow::uncompile(void)
{
	if (modelViewer)
	{
		modelViewer->uncompile();
//		glDeleteLists(modelViewer->getFontListBase(), 256);
	}
}

BOOL ModelWindow::doErrorOK(void)
{
	doDialogClose(hErrorWindow);
	return TRUE;
}

void ModelWindow::doProgressCancel(void)
{
	cancelLoad = true;
}

void ModelWindow::getTreeViewLine(HWND hTreeView, HTREEITEM hItem,
								  TCStringArray *lines)
{
	TVITEM item;
	char buf1[1024];
	char buf2[1024] = "";
	int depth = 0;
	HTREEITEM hParentItem = hItem;

	while ((hParentItem = TreeView_GetParent(hTreeView, hParentItem)) != NULL)
	{
		depth++;
	}
	item.mask = TVIF_TEXT;
	item.hItem = hItem;
	item.pszText = buf1;
	item.cchTextMax = 1024 - depth;
	if (TreeView_GetItem(hTreeView, &item))
	{
		int i;

		for (i = 0; i < depth; i++)
		{
			strcat(buf2, "\t");
		}
		strcat(buf2, buf1);
		lines->addString(buf2);
	}
}

BOOL ModelWindow::doErrorTreeCopy(void)
{
	HTREEITEM hItem = TreeView_GetSelection(hErrorTree);

	if (hItem)
	{
		HTREEITEM hParentItem = TreeView_GetParent(hErrorTree, hItem);
		TCStringArray* textLines = new TCStringArray;
		int i;
		int count;

		while (hParentItem)
		{
			hItem = hParentItem;
			hParentItem = TreeView_GetParent(hErrorTree, hItem);
		}
		getTreeViewLine(hErrorTree, hItem, textLines);
		hItem = TreeView_GetChild(hErrorTree, hItem);
		while (hItem)
		{
			getTreeViewLine(hErrorTree, hItem, textLines);
			hItem = TreeView_GetNextSibling(hErrorTree, hItem);
		}
		count = textLines->getCount();
		if (count)
		{
			int len = 1;
			char *buf;

			for (i = 0; i < count; i++)
			{
				len += strlen((*textLines)[i]) + 2;
			}
			buf = new char[len];
			buf[0] = 0;
			for (i = 0; i < count; i++)
			{
				strcat(buf, (*textLines)[i]);
				strcat(buf, "\r\n");
			}
			if (copyToClipboard(buf))
			{
				delete buf;
				SetWindowLong(hErrorWindow, DWL_MSGRESULT, TRUE);
				return TRUE;
			}
			delete buf;
		}
	}
	return FALSE;
}

BOOL ModelWindow::doErrorTreeKeyDown(LPNMTVKEYDOWN notification)
{
//	debugPrintf("ModelWindow::doErrorTreeKeyDown: 0x%08X, 0x%04X, 0x%08X\n",
//		notification->hdr.code, notification->wVKey, notification->flags);
	if (notification->wVKey == 'C' && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		return doErrorTreeCopy();
	}
	return FALSE;
}

BOOL ModelWindow::doErrorWindowNotify(LPNMHDR notification)
{
//	debugPrintf("ModelWindow::doErrorWindowNotify: %d\n", notification->code);
	switch (notification->code)
	{
	case LVN_ITEMCHANGED:
		LPNMLISTVIEW info = (LPNMLISTVIEW)notification;
		if (info->uNewState != info->uOldState)
		{
			BOOL value = ListView_GetCheckState(hErrorList, info->iItem);
			ErrorInfo *errorInfo = (*errorInfos)[info->iItem];

			TCUserDefaults::setLongForKey(value,
				getErrorKey(errorInfo->getType()), false);
			if (!skipErrorUpdates)
			{
				clearErrorTree();
				populateErrorTree();
			}
		}
		break;
	}
	return FALSE;
}

BOOL ModelWindow::doErrorTreeNotify(LPNMHDR notification)
{
//	debugPrintf("ModelWindow::doErrorTreeNotify: %d\n", notification->code);
	if (notification->code == NM_DBLCLK)
	{
		HTREEITEM hSelectedItem = TreeView_GetSelection(hErrorTree);

//		debugPrintf("double click!\n");
		if (hSelectedItem)
		{
			TVITEM selectedItem;
			char buf[1024];

			selectedItem.mask = TVIF_TEXT;
			selectedItem.hItem = hSelectedItem;
			selectedItem.pszText = buf;
			selectedItem.cchTextMax = 1024;
			if (TreeView_GetItem(hErrorTree, &selectedItem))
			{
				if (stringHasPrefix(buf, "File: "))
				{
					ShellExecute(hWindow, NULL, "notepad.exe", buf + 6, ".",
						SW_SHOWNORMAL);
				}
			}
		}
		else
		{
//			debugPrintf("No selection.\n");
		}
	}
	else if (notification->code == TVN_SELCHANGED)
	{
//		LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)notification;
	}
	else if (notification->code == TVN_KEYDOWN)
	{
		return doErrorTreeKeyDown((LPNMTVKEYDOWN)notification);
	}
	return FALSE;
}

BOOL ModelWindow::doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification)
{
//	debugPrintf("ModelWindow::doDialogNotify: 0x%04X, 0x%04X, 0x%04x\n", hDlg,
//		controlId, notification->code);
	if (hDlg == hErrorWindow)
	{
		if (controlId == IDC_ERROR_TREE)
		{
			return doErrorTreeNotify(notification);
		}
		else
		{
			return doErrorWindowNotify(notification);
		}
	}
	else if (hDlg == hSaveDialog)
	{
		return doSaveNotify(controlId, (LPOFNOTIFY)notification);
	}
	return FALSE;
}

BOOL ModelWindow::doErrorSize(WPARAM sizeType, int newWidth, int newHeight)
{
	RECT sizeRect;

	SendMessage(hErrorStatusWindow, WM_SIZE, sizeType,
		MAKELPARAM(newWidth, newHeight));
	GetClientRect(hErrorWindow, &sizeRect);
	errorWindowResizer->resize(newWidth, newHeight);
	return FALSE;
}

BOOL ModelWindow::doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
							   int newHeight)
{
//	debugPrintf("ModelWindow::doDialogSize(%d, %d, %d)\n", sizeType, newWidth,
//		newHeight);
	if (hDlg == hErrorWindow)
	{
		return doErrorSize(sizeType, newWidth, newHeight);
	}
	return FALSE;
}

BOOL ModelWindow::doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo)
{
	if (hDlg == hErrorWindow)
	{
		calcSystemSizes();
		minMaxInfo->ptMaxSize.x = systemMaxWidth;
		minMaxInfo->ptMaxSize.y = systemMaxHeight;
		minMaxInfo->ptMinTrackSize.x = 475;
		minMaxInfo->ptMinTrackSize.y = 260;
		minMaxInfo->ptMaxTrackSize.x = systemMaxTrackWidth;
		minMaxInfo->ptMaxTrackSize.y = systemMaxTrackHeight;
		return TRUE;
	}
	return FALSE;
}

BOOL ModelWindow::doProgressClick(int controlId, HWND /*controlHWnd*/)
{
	if (controlId == IDCANCEL)
	{
		doProgressCancel();
	}
	return TRUE;
}

char* ModelWindow::getErrorKey(LDLErrorType errorType)
{
	static char key[128];

	sprintf(key, "%s/LDLError%02d", SHOW_ERRORS_KEY, errorType);
	return key;
}

BOOL ModelWindow::doErrorClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
	case IDCANCEL:
		return doErrorOK();
		break;
	case IDC_COPY_ERROR:
		if (!doErrorTreeCopy())
		{
			MessageBeep(MB_OK);
		}
		break;
	case IDC_SHOW_WARNINGS:
		int value;

		value = SendDlgItemMessage(hErrorWindow, controlId, BM_GETCHECK,
			0, 0);
		TCUserDefaults::setLongForKey(value, SHOW_WARNINGS_KEY, false);
		clearErrorTree();
		populateErrorTree();
		break;
	case IDC_ERROR_SHOW_ALL:
		return setAllErrorsSelected(true);
		break;
	case IDC_ERROR_SHOW_NONE:
		return setAllErrorsSelected(false);
		break;
	}
	return TRUE;
}

BOOL ModelWindow::setAllErrorsSelected(bool selected)
{
	int i;
	int count = errorInfos->getCount();
	BOOL state = selected ? TRUE : FALSE;

	skipErrorUpdates = true;
	for (i = 0; i < count; i++)
	{
		ListView_SetCheckState(hErrorList, i, state);
	}
	skipErrorUpdates = false;
	clearErrorTree();
	populateErrorTree();
	return TRUE;
}

BOOL ModelWindow::doPageSetupClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
	case IDC_PRINT_BACKGROUND:
		printBackground = SendDlgItemMessage(hPageSetupDialog,
			IDC_PRINT_BACKGROUND, BM_GETCHECK, 0, 0) ? true : false;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void ModelWindow::applyPrefs(void)
{
	bool antialiasChanged = TCUserDefaults::longForKey(FSAA_MODE_KEY) !=
		currentAntialiasType;

	loadSettings();
	((LDViewWindow*)parentWindow)->applyPrefs();
	if (LDVExtensionsSetup::haveMultisampleExtension() && antialiasChanged)
	{
		setTimer(FSAA_UPDATE_TIMER, 0);
	}
	forceRedraw();
}

LRESULT ModelWindow::doCommand(int /*itemId*/, int notifyCode, HWND controlHWnd)
{
	if (controlHWnd == hPrefsWindow && controlHWnd != NULL)
	{
		switch (notifyCode)
		{
		case CUI_OK:
			rotationSpeed = 0.0f;
			modelViewer->setRotationSpeed(0.0f);
			modelViewer->setZoomSpeed(0.0f);
			prefs->closePropertySheet();
			hPrefsWindow = NULL;
			applyPrefs();
			break;
		case CUI_CANCEL:
			rotationSpeed = 0.0f;
			modelViewer->setRotationSpeed(0.0f);
			modelViewer->setZoomSpeed(0.0f);
			prefs->closePropertySheet();
			hPrefsWindow = NULL;
			break;
		case CUI_APPLY:
			applyPrefs();
			break;
		}
	}
	return 1;
}

BOOL ModelWindow::doDialogCommand(HWND hDlg, int controlId, int notifyCode,
								  HWND controlHWnd)
{
//	debugPrintf("ModelWindow::doDialogCommand(0x%08X, 0x%04X, 0x%04X, 0x%08X)\n",
//		hDlg, controlId, notifyCode, controlHWnd);
	if (hDlg == hSaveDialog)
	{
		return doSaveCommand(controlId, notifyCode, controlHWnd);
	}
	else if (hDlg == hPrintDialog)
	{
		return doPrintCommand(controlId, notifyCode, controlHWnd);
	}
	if (notifyCode == BN_CLICKED)
	{
		if (hDlg == hProgressWindow)
		{
			return doProgressClick(controlId, controlHWnd);
		}
		else if (hDlg == hErrorWindow)
		{
			return doErrorClick(controlId, controlHWnd);
		}
		else if (hDlg == hPageSetupDialog)
		{
			return doPageSetupClick(controlId, controlHWnd);
		}
	}
	return FALSE;
}

BOOL ModelWindow::showPreferences(void)
{
	prefs->setHDlgParent(GetParent(hWindow));
	hPrefsWindow = (HWND)prefs->show();
	return TRUE;
}

void ModelWindow::showErrors(void)
{
	showErrorsIfNeeded(FALSE);
}

/*
BOOL ModelWindow::showProgress(void)
{
	if (!loading)
	{
		clearErrors();
		loading = true;
		if (!hProgressWindow)
		{
			createProgress();
		}
		if (hProgressWindow)
		{
			RECT rect;
			int newX, newY, progWidth, progHeight;

			//debugPrintf("hProgressWindow: 0x%04X\n", hProgressWindow);
			setupProgress();
			GetWindowRect(hProgressWindow, &rect);
			progWidth = rect.right - rect.left;
			progHeight = rect.bottom - rect.top;
			GetWindowRect(parentWindow->getHWindow(), &rect);
			newX = rect.left + width / 2 - progWidth / 2;
			newY = rect.top + height / 2 - progHeight / 2;
			MoveWindow(hProgressWindow, newX, newY, progWidth, progHeight,
				FALSE);
			ShowWindow(hProgressWindow, SW_SHOWNORMAL);
			flushDialogModal(hProgressWindow);
			return TRUE;
		}
	}
	return FALSE;
}
*/

void ModelWindow::hideProgress(void)
{
	if (loading)
	{
		SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
		SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)"");
		EnumThreadWindows(GetWindowThreadProcessId(hParentWindow, NULL),
			enableNonModalWindow, (LPARAM)hParentWindow);
		((LDViewWindow*)parentWindow)->setLoading(false);
//		doDialogClose(hProgressWindow);
		loading = false;
		((LDViewWindow*)parentWindow)->forceShowStatusBar(false);
	}
}

HTREEITEM ModelWindow::addErrorLine(HTREEITEM parent, char* line,
									LDLError* error, int imageIndex)
{
	TVINSERTSTRUCT insertStruct;
	TVITEMEX item;

	stripCRLF(line);
	memset(&item, 0, sizeof(item));
	item.mask = TVIF_TEXT | TVIF_PARAM;
	item.pszText = line;
	item.lParam = (LPARAM)error;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = TVI_LAST;
	if (error->getLevel() != LDLAWarning)
	{
		item.mask |= TVIF_STATE;
		item.stateMask = TVIS_BOLD;
		item.state = TVIS_BOLD;
	}
	if (imageIndex >= 0)
	{
        item.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
        item.iImage = imageIndex; 
        item.iSelectedImage = imageIndex; 
	}
	insertStruct.itemex = item;
	return TreeView_InsertItem(hErrorTree, &insertStruct);
}

bool ModelWindow::addError(LDLError* error)
{
	char buf[1024];
	char* string;
	HTREEITEM parent;

	if (!showsError(error))
	{
		return false;
	}
	string = error->getMessage();
	if (string)
	{
		sprintf(buf, "%s", string);
	}
	else
	{
		sprintf(buf, "");
	}
	parent = addErrorLine(NULL, buf, error,
		errorImageIndices[error->getType()]);

	if (parent)
	{
		TCStringArray *extraInfo;

   		string = error->getFilename();
		if (string)
		{
			sprintf(buf, "File: %s", string);
		}
		else
		{
			sprintf(buf, "Unknown filename");
		}
		addErrorLine(parent, buf, error);
		string = error->getFileLine();
		if (string)
		{
			int lineNumber = error->getLineNumber();

			if (lineNumber > 0)
			{
				sprintf(buf, "Line #%d", lineNumber);
			}
			else
			{
				sprintf(buf, "Unknown Line #");
			}
			addErrorLine(parent, buf, error);
			sprintf(buf, "Line: %s", string);
		}
		else
		{
			sprintf(buf, "Unknown Line");
		}
		addErrorLine(parent, buf, error);
		if ((extraInfo = error->getExtraInfo()) != NULL)
		{
			int i;
			int count = extraInfo->getCount();

			for (i = 0; i < count; i++)
			{
				addErrorLine(parent, extraInfo->stringAtIndex(i), error);
			}
		}
	}
	return true;
}

bool ModelWindow::showsError(LDLError *error)
{
	LDLErrorType errorType = error->getType();

	if (error->getLevel() == LDLAWarning)
	{
		if (TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0, false))
		{
			return TCUserDefaults::longForKey(getErrorKey(errorType), 0, false)
				!= 0;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return TCUserDefaults::longForKey(getErrorKey(errorType), 1, false) !=
			0;
	}
}

BOOL ModelWindow::showsErrorType(LDLErrorType errorType)
{
	return TCUserDefaults::longForKey(getErrorKey(errorType), 1, false);
}

void ModelWindow::populateErrorInfos(void)
{
	if (!errorInfos)
	{
		int i;

		errorInfos = new ErrorInfoArray;
		for (i = LDLEGeneral; i <= LDLEMPDError; i++)
		{
			ErrorInfo *errorInfo = new ErrorInfo;
			LDLErrorType type = (LDLErrorType)i;

			errorInfo->setType(type);
			errorInfo->setTypeName(LDLError::getTypeName(type));
			errorInfos->addObject(errorInfo);
			errorInfo->release();
		}
	}
}

void ModelWindow::setupErrorWindow(void)
{
	HIMAGELIST himl;  // handle to image list
	HBITMAP hbmp;     // handle to bitmap
	HBITMAP hMask;
	long showWarnings = TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0, false);

	populateErrorInfos();
	populateErrorList();
	memset(errorImageIndices, 0, sizeof(errorImageIndices));
	if (errorWindowResizer)
	{
		errorWindowResizer->release();
	}
	errorWindowResizer = new CUIWindowResizer;
	errorWindowResizer->setHWindow(hErrorWindow);
	errorWindowResizer->addSubWindow(IDC_ERROR_TREE,
		CUISizeHorizontal | CUISizeVertical);
	errorWindowResizer->addSubWindow(IDC_COPY_ERROR,
		CUIFloatLeft | CUIFloatTop);
	errorWindowResizer->addSubWindow(IDC_ERROR_SHOW_ALL,
		CUIFloatLeft | CUIFloatTop);
	errorWindowResizer->addSubWindow(IDC_ERROR_SHOW_NONE,
		CUIFloatLeft | CUIFloatTop);
	errorWindowResizer->addSubWindow(IDC_SHOW_WARNINGS, CUIFloatTop);
	errorWindowResizer->addSubWindow(IDC_SHOW_ERRORS,
		CUIFloatLeft | CUISizeVertical);
	errorWindowResizer->addSubWindow(IDC_ERROR_LIST,
		CUIFloatLeft | CUISizeVertical);

	SendDlgItemMessage(hErrorWindow, IDC_SHOW_WARNINGS, BM_SETCHECK,
		showWarnings, 0);
	// Create the image list.
	if ((himl = ImageList_Create(16, 16, ILC_COLOR | ILC_MASK, 8, 0)) == NULL)
		return;

	// Add the bitmaps.
	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_INFO));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_INFO_MASK));
	ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_PARSE));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_PARSE_MASK));
	errorImageIndices[LDLEParse] = ImageList_Add(himl, hbmp, hMask);
	errorImageIndices[LDLEGeneral] = errorImageIndices[LDLEParse];
	errorImageIndices[LDLEBFCError] = errorImageIndices[LDLEParse];
	errorImageIndices[LDLEMPDError] = errorImageIndices[LDLEParse];
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_FNF));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_FNF_MASK));
	errorImageIndices[LDLEFileNotFound] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_MATRIX));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_MATRIX_MASK));
	errorImageIndices[LDLEMatrix] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_DETERMINANT));
	hMask = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_DETERMINANT_MASK));
	errorImageIndices[LDLEPartDeterminant] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_NON_FLAT_QUAD));
	hMask = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_NON_FLAT_QUAD_MASK));
	errorImageIndices[LDLENonFlatQuad] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_CONCAVE_QUAD));
	hMask = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_CONCAVE_QUAD_MASK));
	errorImageIndices[LDLEConcaveQuad] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	// Not sure this error is possible.  I can't figure out how to make it
	// happen.
	hbmp = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_CONCAVE_QUAD_SPLIT));
	hMask = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_CONCAVE_QUAD_SPLIT_MASK));
	errorImageIndices[LDLEConcaveQuadSplit] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_MATCHING_POINTS));
	hMask = LoadBitmap(getLanguageModule(),
		MAKEINTRESOURCE(IDB_MATCHING_POINTS_MASK));
	errorImageIndices[LDLEMatchingPoints] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_COLINEAR));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_COLINEAR_MASK));
	errorImageIndices[LDLEColinear] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);

/*
	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_OPENGL));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_OPENGL_MASK));
	errorImageIndices[LDLEOpenGL] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);
*/

/*
	hbmp = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_COLOR));
	hMask = LoadBitmap(getLanguageModule(), MAKEINTRESOURCE(IDB_COLOR_MASK));
	errorImageIndices[LDMEColor] = ImageList_Add(himl, hbmp, hMask);
	DeleteObject(hbmp);
	DeleteObject(hMask);
*/

	// Associate the image list with the tree view control.
	TreeView_SetImageList(hErrorTree, himl, TVSIL_NORMAL);
//	TreeView_SetItemHeight(hErrorTree, 18);
}

void ModelWindow::setupProgress(void)
{
	cancelLoad = false;
}

void ModelWindow::registerErrorWindowClass(void)
{
	WNDCLASSEX windowClass;
	char prefsClassName[1024];

	if (!hProgressWindow)
	{
		createProgress();
	}
	GetClassName(hProgressWindow, prefsClassName, 1024);
	windowClass.cbSize = sizeof(windowClass);
	GetClassInfoEx(hInstance, prefsClassName, &windowClass);
	windowClass.hIcon = LoadIcon(getLanguageModule(),
		MAKEINTRESOURCE(IDI_APP_ICON));
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "LDViewErrorWindow";
	RegisterClassEx(&windowClass);
}

void ModelWindow::initCommonControls(DWORD mask)
{
	static DWORD initializedMask = 0;

	if ((initializedMask & mask) != mask)
	{
		INITCOMMONCONTROLSEX initCtrls;

		initCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		initCtrls.dwICC = mask;
		InitCommonControlsEx(&initCtrls);
		initializedMask |= mask;
	}
}

void ModelWindow::createErrorWindow(void)
{
	if (!hErrorWindow)
	{
		HWND hActiveWindow = GetActiveWindow();
		int parts[] = {-1};

		initCommonControls(ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES);
		registerErrorWindowClass();
		hErrorWindow = createDialog(IDD_ERRORS, FALSE);
		hErrorStatusWindow = CreateStatusWindow(
			WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, "", hErrorWindow,
			2000);
		SendMessage(hErrorStatusWindow, SB_SETPARTS, 1, (LPARAM)parts);
		originalErrorDlgProc = (WNDPROC)GetWindowLong(hErrorWindow, DWL_DLGPROC);
		SetWindowLong(hErrorWindow, GWL_USERDATA, (long)this);
		SetWindowLong(hErrorWindow, DWL_DLGPROC, (long)staticErrorDlgProc);
		hErrorTree = GetDlgItem(hErrorWindow, IDC_ERROR_TREE);
		hErrorList = GetDlgItem(hErrorWindow, IDC_ERROR_LIST);
//		hErrorOk = GetDlgItem(hErrorWindow, IDOK);
		setupErrorWindow();
		SetActiveWindow(hActiveWindow);
	}
}

void ModelWindow::createProgress(void)
{
	initCommonControls(ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS);
	hProgressWindow = createDialog(IDD_LOAD_PROGRESS);
	hProgressMessage = GetDlgItem(hProgressWindow, IDC_LOAD_PROGRESS_MSG);
	hProgressCancelButton = GetDlgItem(hProgressWindow, IDCANCEL);
	hProgress = GetDlgItem(hProgressWindow, IDC_PROGRESS);
}

void ModelWindow::clearErrorTree(void)
{
//	RECT rect;
//	POINT topLeft;

//	SetWindowRedraw(hErrorTree, FALSE);
	if (hErrorWindow)
	{
		TreeView_DeleteAllItems(hErrorTree);
	}
/*
	SetWindowRedraw(hErrorTree, TRUE);
	GetWindowRect(hErrorTree, &rect);
	topLeft.x = rect.left;
	topLeft.y = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	ScreenToClient(GetParent(hErrorTree), &topLeft);
//	MoveWindow(hErrorTree, topLeft.x, topLeft.y, width - 1, height, TRUE);
	MoveWindow(hErrorTree, topLeft.x, topLeft.y, width, height, TRUE);
*/
	errorTreePopulated = false;
}

void ModelWindow::clearErrors(void)
{
	errors->removeAll();
/*
	while (errors->getCount())
	{
		errors->removeObject(0);
	}
*/
	clearErrorTree();
}

void ModelWindow::populateErrorList(void)
{
	int i;
	int count = errorInfos->getCount();
	LVCOLUMN column;
	DWORD exStyle = ListView_GetExtendedListViewStyle(hErrorList);

	skipErrorUpdates = true;
	exStyle |= LVS_EX_CHECKBOXES;
	ListView_SetExtendedListViewStyle(hErrorList, exStyle);
	memset(&column, 0, sizeof(LVCOLUMN));
	column.mask = LVCF_FMT | LVCF_WIDTH;
	column.fmt = LVCFMT_LEFT;
	column.cx = 300;
	ListView_InsertColumn(hErrorList, 0, &column);
	for (i = 0; i < count; i++)
	{
		ErrorInfo *errorInfo = (*errorInfos)[i];
		LVITEM item;
		int state;

		memset(&item, 0, sizeof(item));
		item.mask = TVIF_TEXT | TVIF_PARAM;
		item.pszText = errorInfo->getTypeName();
		item.lParam = errorInfo->getType();
		item.iItem = i;
		item.iSubItem = 0;
		state = TCUserDefaults::longForKey(getErrorKey(errorInfo->getType()),
			1, false);
		ListView_InsertItem(hErrorList, &item);
		ListView_SetCheckState(hErrorList, i, state);
	}
	skipErrorUpdates = false;
	ListView_SetColumnWidth(hErrorList, 0, LVSCW_AUTOSIZE);
}

int ModelWindow::populateErrorTree(void)
{
	int errorCount = 0;
	int warningCount = 0;
	char buf[128] = "";
//	RECT rect;
//	POINT topLeft;
//	int width;
//	int height;

//	SetWindowRedraw(hErrorTree, FALSE);
	if (!windowShown)
	{
		return 0;
	}
	if (hErrorWindow && !errorTreePopulated)
	{
		for (int i = 0, count = errors->getCount(); i < count; i++)
		{
			LDLError *error = (*errors)[i];

			if (addError(error))
			{
				if (error->getLevel() == LDLAWarning)
				{
					warningCount++;
				}
				else
				{
					errorCount++;
				}
			}
		}
		errorTreePopulated = true;
	}
/*
	SetWindowRedraw(hErrorTree, TRUE);
	GetWindowRect(hErrorTree, &rect);
	topLeft.x = rect.left;
	topLeft.y = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	ScreenToClient(GetParent(hErrorTree), &topLeft);
	if (errorCount > 0 || warningCount > 0)
	{
		MoveWindow(hErrorTree, topLeft.x, topLeft.y, width - 1, height, FALSE);
	}
	MoveWindow(hErrorTree, topLeft.x, topLeft.y, width, height, TRUE);
*/
//	RedrawWindow(hErrorTree, NULL, NULL, RDW_INVALIDATE | RDW_ERASE |
//		RDW_FRAME | RDW_ALLCHILDREN);
	if (errorCount > 0)
	{
		sprintf(buf, "%d Error%s", errorCount, errorCount == 1 ? "" : "s");
		if (warningCount > 0)
		{
			strcat(buf, ", ");
		}
	}
	if (warningCount > 0)
	{
		sprintf(buf + strlen(buf), "%d Warning%s", warningCount,
			warningCount == 1 ? "" : "s");
	}
	SendMessage(hErrorStatusWindow, SB_SETTEXT, 0, (LPARAM)buf);
	return errorCount;
}

void ModelWindow::showErrorsIfNeeded(BOOL onlyIfNeeded)
{
	if (windowShown && !((LDViewWindow*)parentWindow)->getFullScreen())
	{
		if (!hErrorWindow)
		{
			createErrorWindow();
		}
		if (hErrorWindow)
		{
			int errorCount;

			errorCount = populateErrorTree();
			if (!onlyIfNeeded || (errorCount &&
				TCUserDefaults::longForKey(SHOW_ERRORS_KEY, 1, false)))
			{
				ShowWindow(hErrorWindow, SW_SHOWNORMAL);
			}
		}
	}
}

int ModelWindow::loadModel(void)
{
	char* filename = modelViewer->getFilename();
#ifdef _DEBUG
	_CrtMemState ms1, ms2, ms3;
#endif // _DEBUG

	clearErrors();
	stopPolling();
	makeCurrent();
	if (strlen(filename) < 900)
	{
		char title[1024];

		sprintf(title, "LDView: %s", filename);
		parentWindow->setTitle(title);
	}
	else
	{
		parentWindow->setTitle("LDView");
	}
	_CrtMemCheckpoint(&ms1);
	if (modelViewer->loadModel())
	{
		_CrtMemCheckpoint(&ms2);
		_CrtMemDifference(&ms3, &ms1, &ms2);
		_CrtMemDumpStatistics(&ms3);
		forceRedraw();
		rotationSpeed = 0.0f;
		modelViewer->setRotationSpeed(0.0f);
		modelViewer->setZoomSpeed(0.0f);
		if (pollSetting)
		{
			if (getFileTime(&lastWriteTime))
			{
				startPolling();
			}
		}
		return 1;
	}
	else
	{
		parentWindow->setTitle("LDView");
		stopPolling();
		modelViewer->setFilename(NULL);
		return 0;
	}
}

void ModelWindow::closeWindow(void)
{
	destroyWindow();
	CUIOGLWindow::closeWindow();
}

LRESULT ModelWindow::doShowWindow(BOOL showFlag, LPARAM status)
{
	windowShown = true;
	return CUIOGLWindow::doShowWindow(showFlag, status);
}

BOOL ModelWindow::initWindow(void)
{
	LDVExtensionsSetup::setup(hInstance);
	if (((LDViewWindow*)parentWindow)->getFullScreen() ||
		((LDViewWindow*)parentWindow)->getScreenSaver())
	{
		exWindowStyle &= ~WS_EX_CLIENTEDGE;
	}
	else
	{
		exWindowStyle |= WS_EX_CLIENTEDGE;
	}
	windowStyle |= WS_CHILD;
	if (CUIOGLWindow::initWindow())
	{
		if (LDVExtensionsSetup::haveMultisampleExtension())
		{
			if (currentAntialiasType)
			{
				if (LDVExtensionsSetup::haveNvMultisampleFilterHintExtension())
				{
					if (LDViewPreferences::getUseNvMultisampleFilter())
					{
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
					}
					else
					{
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
					}
				}
				glEnable(GL_MULTISAMPLE_ARB);
			}
			else
			{
				if (LDVExtensionsSetup::haveNvMultisampleFilterHintExtension())
				{
					glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
				}
				glDisable(GL_MULTISAMPLE_ARB);
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LRESULT CALLBACK ModelWindow::staticErrorDlgProc(HWND hDlg, UINT message,
												 WPARAM wParam, LPARAM lParam)
{
	ModelWindow* modelWindow = (ModelWindow*)GetWindowLong(hDlg, GWL_USERDATA);

	if (modelWindow)
	{
		return modelWindow->errorDlgProc(hDlg, message, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

LRESULT ModelWindow::errorDlgProc(HWND hDlg, UINT message, WPARAM wParam,
								  LPARAM lParam)
{
//	debugPrintf("ModelWindow::errorDlgProc: 0x%04x\n", message);
	return CallWindowProc(originalErrorDlgProc, hDlg, message, wParam, lParam);
}

LRESULT ModelWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam,
							  LPARAM lParam)
{
//	debugPrintf("ModelWindow::windowProc: 0x%04x\n", message);
	return CUIOGLWindow::windowProc(hWnd, message, wParam, lParam);
}

int ModelWindow::progressCallback(char* message, float progress,
								  bool showErrors)
{
	DWORD thisProgressUpdate = GetTickCount();

	if (!windowShown)
	{
		return 1;
	}
	if (progress == 2.0)
	{
		// done
		hideProgress();
		// If the window is closed during load, we will no longer be initialized.
		if (showErrors && initialized && !cancelLoad)
		{
			showErrorsIfNeeded();
		}
		makeCurrent();
		return 1;
	}
	if (!loading)
	{
		clearErrors();
		loading = true;
		setupProgress();
		((LDViewWindow*)parentWindow)->forceShowStatusBar(true);
		if (!flushModal(hParentWindow, false))
		{
			PostQuitMessage(0);
			cancelLoad = true;
		}
		((LDViewWindow*)parentWindow)->setLoading(true);
	}
//	showProgress();
//	debugPrintf("%s: %f\n", message, progress);
	if (message)
	{
//		SendDlgItemMessage(hProgressWindow, IDC_LOAD_PROGRESS_MSG, WM_SETTEXT,
//			0, (LPARAM)message);
		SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)message);
	}
	if (progress >= 0.0f)
	{
//		SendDlgItemMessage(hProgressWindow, IDC_PROGRESS, PBM_SETPOS,
//			(int)(progress * 100), 0);
		SendMessage(hProgressBar, PBM_SETPOS, (int)(progress * 100), 0);
	}
	if (thisProgressUpdate < lastProgressUpdate || thisProgressUpdate >
		lastProgressUpdate + 100 || progress == 1.0f)
	{
		if (!flushModal(hParentWindow, false))
		{
			PostQuitMessage(0);
			cancelLoad = true;
		}
//		flushDialogModal(hProgressWindow);
		lastProgressUpdate = thisProgressUpdate;
	}
	if (cancelLoad)
	{
		hideProgress();
		makeCurrent();
		return 0;
	}
	else
	{
		makeCurrent();
		return 1;
	}
}

int ModelWindow::staticProgressCallback(char* message, float progress,
										void* userData)
{
	return ((ModelWindow*)userData)->progressCallback(message, progress, true);
}

bool ModelWindow::staticImageProgressCallback(char* message, float progress,
											  void* userData)
{
	return ((ModelWindow*)userData)->progressCallback(message, progress) ? true
		: false;
}

int ModelWindow::errorCallback(LDLError* error)
{
//	debugPrintf("Error of type %d: %s\n", error->getType(),
//		error->getMessage());
	if (windowShown)
	{
		errors->addObject(error);
	}
	return 1;
}

/*
int ModelWindow::staticErrorCallback(LDMError* error, void* userData)
{
	return ((ModelWindow*)userData)->errorCallback(error);
}
*/

LRESULT ModelWindow::doCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	if (CUIOGLWindow::doCreate(hWnd, lpcs) == -1)
	{
		return -1;
	}
	else
	{
		DragAcceptFiles(hWnd, TRUE);
		return 0;
	}
}

LRESULT ModelWindow::doDropFiles(HDROP hDrop)
{
	char buf[1024];
	if (DragQueryFile(hDrop, 0, buf, 1024) > 0)
	{
		DragFinish(hDrop);
		((LDViewWindow*)parentWindow)->openModel(buf);
		return 0;
	}
	return 1;
}

void ModelWindow::checkForPart(void)
{
	bool isPart = false;
	char buf[MAX_PATH];

	if (GetCurrentDirectory(MAX_PATH, buf))
	{
		char partsDir[1024];

		strcpy(partsDir, LDLModel::lDrawDir());
		strcat(partsDir, "\\PARTS");
		convertStringToUpper(partsDir);
		convertStringToUpper(buf);
		replaceStringCharacter(buf, '/', '\\');
		replaceStringCharacter(partsDir, '/', '\\');
		if (strcmp(buf, partsDir) == 0)
		{
			isPart = true;
		}
		else
		{
			char shortPath[1024];

			if (GetShortPathName(partsDir, shortPath, 1024))
			{
				if (strcmp(buf, shortPath) == 0)
				{
					isPart = true;
				}
			}
		}
	}
	modelViewer->setFileIsPart(isPart);
}

bool ModelWindow::chDirFromFilename(const char* filename, char* outFilename)
{
	char buf[MAX_PATH];
	char* fileSpot;
	DWORD result = GetFullPathName(filename, MAX_PATH, buf, &fileSpot);

	if (result <= MAX_PATH && result > 0)
	{
//		if (strlen(fileSpot) < strlen(filename))
		{
			strcpy(outFilename, buf);
		}
		*fileSpot = 0;
		if (SetCurrentDirectory(buf))
		{
			return true;
		}
	}
	return false;
}

void ModelWindow::printSystemError(void)
{
#ifdef _DEBUG
	DWORD error = GetLastError();
	char* buf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf,
		0, NULL);
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s\n", buf);
	LocalFree(buf);
#endif
}

void ModelWindow::setupMaterial(void)
{
	// Don't call super.
}

void ModelWindow::setupLighting(void)
{
//	LDrawModelViewer::setPolygonOffsetFunc((GLPolygonOffsetFunc)glPolygonOffset);
	modelViewer->setup();
}

void ModelWindow::setStatusText(HWND hStatus, int part, char *text)
{
	char oldText[1024];

	SendMessage(hStatus, SB_GETTEXT, part, (LPARAM)oldText);
	if (strcmp(text, oldText) != 0)
	{
		SendMessage(hStatus, SB_SETTEXT, part, (LPARAM)text);
		debugPrintf(2, "0x%08X: %s\n", hStatus, text);
	}
}

void ModelWindow::drawFPS(void)
{
	if (prefs->getShowsFPS())
	{
		if (((LDViewWindow*)parentWindow)->getFullScreen() || !hStatusBar)
		{
			if (currentAntialiasType)
			{
				glDisable(GL_MULTISAMPLE_ARB);
			}
			modelViewer->drawFPS(fps);
			if (currentAntialiasType)
			{
				glEnable(GL_MULTISAMPLE_ARB);
			}
		}
		else if (hStatusBar)
		{
			char fpsString[1024] = "";

			if (modelViewer->getMainTREModel())
			{
				if (fps > 0.0f)
				{
					sprintf(fpsString, "FPS: %4.4f", fps);
				}
				else
				{
					strcpy(fpsString, "Spin Model for FPS");
				}
			}
			setStatusText(hStatusBar, 1, fpsString);
		}
	}
	else if (hStatusBar)
	{
		setStatusText(hStatusBar, 1, "");
	}
}

void ModelWindow::doPostPaint(void)
{
	checkFileForUpdates();
	if (hProgressBar)
	{
		if (!RedrawWindow(hProgressBar, NULL, NULL, RDW_ERASE | RDW_INVALIDATE
			| RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN))
		{
			debugPrintf("RedrawWindow failed!\n");
		}
	}
}

void ModelWindow::updateFPS(void)
{
	DWORD thisFrameTime;

	thisFrameTime = timeGetTime();
	numFramesSinceReference++;
	if (firstFPSPass)
	{
		fps = 0.0f;
		referenceFrameTime = thisFrameTime;
		numFramesSinceReference = 0;
		firstFPSPass = false;
	}
	else if (thisFrameTime - referenceFrameTime >= 250)
	{
		if (thisFrameTime > referenceFrameTime)
		{
			fps = 1000.0f / (float)(thisFrameTime - referenceFrameTime) *
				numFramesSinceReference;
		}
		else
		{
			fps = 0.0f;
		}
		referenceFrameTime = thisFrameTime;
		numFramesSinceReference = 0;
		firstFPSPass = false;
	}
	if (!frontBufferFPS())
	{
		drawFPS();
	}
}

bool ModelWindow::frontBufferFPS(void)
{
	if (LDViewPreferences::getUseNvMultisampleFilter())
	{
		static bool checkedSwapInterval = false;
		static int swapInterval = 0;

		if (!checkedSwapInterval)
		{
			int (*wglGetSwapIntervalEXT)(void) =
				(int(*)(void))wglGetProcAddress("wglGetSwapIntervalEXT");

			checkedSwapInterval = true;
			if (wglGetSwapIntervalEXT)
			{
				swapInterval = wglGetSwapIntervalEXT();
			}
			else
			{
				swapInterval = 0;
			}
		}

		if (swapInterval > 0)
		{
			// We only want to draw the FPS text to the front buffer if vsync
			// is turned on.
			return true;
		}
	}
	return false;
}

void ModelWindow::swapBuffers(void)
{
	if (!SwapBuffers(hdc))
	{
		DWORD error = GetLastError();
		char buf[1024];

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buf,
			1024, NULL);
		debugPrintf("swapBuffers error: %s\n", buf);
	}
	if (frontBufferFPS())
	{
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
		glDisable(GL_MULTISAMPLE_ARB);
		glDrawBuffer(GL_FRONT);
		drawFPS();
		glDrawBuffer(GL_BACK);
		glFlush();
		glEnable(GL_MULTISAMPLE_ARB);
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}
}

void ModelWindow::doPaint(void)
{
	bool needsPostRedraw = (modelViewer->getNeedsReload() ||
		modelViewer->getNeedsRecompile()) && modelViewer->getMainTREModel();

	if (offscreenActive || loading)
	{
		static bool looping = false;
		if (!looping)
		{
			looping = true;
			RedrawWindow(hParentWindow, NULL, NULL, RDW_ERASE | RDW_INVALIDATE
				| RDW_ERASENOW | RDW_UPDATENOW);
			ValidateRect(hWindow, NULL);
			looping = false;
		}
		return;
	}
	if (!initializedGL)
	{
		forceRedraw();
		return;
	}
	if (redrawCount > 0)
	{
		forceRedraw();
		redrawCount--;
		return;
	}
	makeCurrent();
	if (needsRecompile)
	{
		needsRecompile = false;
		recompile();
	}
	if (loading)
	{
		modelViewer->clear();
		swapBuffers();
		return;
	}
	forceRedraw();
	modelViewer->update();

	updateSpinRate();
	updateFPS();
	if ((fEq(rotationSpeed, 0.0f) && fEq(modelViewer->getZoomSpeed(), 0.0f)
		&& fEq(modelViewer->getCameraXRotate(), 0.0f) &&
		fEq(modelViewer->getCameraYRotate(), 0.0f) &&
		fEq(modelViewer->getCameraZRotate(), 0.0f) &&
		fEq(modelViewer->getCameraMotion().length(), 0.0f))
		|| modelViewer->getPaused())
	{
		if (redrawCount == 0)
		{
			ValidateRect(hWindow, NULL);
			firstFPSPass = true;
		}
	}
	if (redrawCount > 0)
	{
		redrawCount--;
	}
	swapBuffers();
	if (needsPostRedraw)
	{
		forceRedraw();
	}
}

LRESULT ModelWindow::doNCDestroy(void)
{
	if (prefs && !applyingPrefs)
	{
		prefs->closePropertySheet(true);
		hPrefsWindow = NULL;
	}
	if (hProgressWindow)
	{
		DestroyWindow(hProgressWindow);
		hProgressWindow = NULL;
	}
	if (hErrorWindow)
	{
		DestroyWindow(hErrorWindow);
		hErrorWindow = NULL;
	}
	return CUIOGLWindow::doNCDestroy();
}

LRESULT ModelWindow::doDestroy(void)
{
	cancelLoad = true;
//	RevokeDragDrop(hWindow);
	return CUIOGLWindow::doDestroy();
}

void ModelWindow::zoom(float amount)
{
	if (modelViewer)
	{
		modelViewer->zoom(amount);
		forceRedraw();
	}
}

void ModelWindow::setClipZoom(bool value)
{
	if (modelViewer)
	{
		modelViewer->setClipZoom(value);
	}
}

bool ModelWindow::getClipZoom(void)
{
	if (modelViewer)
	{
		return modelViewer->getClipZoom();
	}
	else
	{
		return false;
	}
}

void ModelWindow::startPolling(void)
{
	if (pollSetting && !pollTimerRunning && !loading)
	{
		setTimer(POLL_TIMER, POLL_INTERVAL);
		pollTimerRunning = true;
	}
}

BOOL ModelWindow::stopPolling(void)
{
	if (loading)
	{
		return TRUE;
	}
	else
	{
		pollTimerRunning = false;
		return killTimer(POLL_TIMER);
	}
}

void ModelWindow::setPollSetting(int value)
{
	if (value != pollSetting)
	{
		pollSetting = value;
		if (pollSetting)
		{
			startPolling();
		}
		else
		{
			stopPolling();
		}
	}
}

bool ModelWindow::writeImage(char *filename, int width, int height,
							 BYTE *buffer, char *formatName)
{
	TCImage *image = new TCImage;
	bool retValue;
	const char *version = ((LDViewWindow *)parentWindow)->getProductVersion();
	char comment[1024];

	if (canSaveAlpha())
	{
		image->setDataFormat(TCRgba8);
	}
	image->setSize(width, height);
	image->setLineAlignment(4);
	image->setImageData(buffer);
	image->setFormatName(formatName);
	image->setFlipped(true);
	if (strcasecmp(formatName, "PNG") == 0)
	{
		strcpy(comment, "Software:!:!:LDView");
	}
	else
	{
		strcpy(comment, "Created by LDView");
	}
	if (version)
	{
		strcat(comment, " ");
		strcat(comment, version);
	}
	image->setComment(comment);
	retValue = image->saveFile(filename, staticImageProgressCallback, this);
	image->release();
	return retValue;
}

bool ModelWindow::writeBmp(char *filename, int width, int height, BYTE *buffer)
{
	return writeImage(filename, width, height, buffer, "BMP");
}

bool ModelWindow::writePng(char *filename, int width, int height, BYTE *buffer)
{
	return writeImage(filename, width, height, buffer, "PNG");
}

bool ModelWindow::setupPBuffer(int imageWidth, int imageHeight,
							   bool antialias)
{
	if (LDVExtensionsSetup::havePixelBufferExtension())
	{
		GLint intValues[] = {
			WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
			WGL_RED_BITS_ARB, 8,
			WGL_GREEN_BITS_ARB, 8,
			WGL_BLUE_BITS_ARB, 8,
			WGL_ALPHA_BITS_ARB, 8,
			0, 0,
			0, 0,
			0, 0
		};
		int index;

		if (antialias)
		{
			int offset = sizeof(intValues) / sizeof(GLint) - 6;

			intValues[offset++] = WGL_SAMPLES_EXT;
			intValues[offset++] = LDViewPreferences::getFSAAFactor();
			intValues[offset++] = WGL_SAMPLE_BUFFERS_EXT;
			intValues[offset++] = GL_TRUE;
		}
		index = LDVExtensionsSetup::choosePixelFormat(hdc, intValues);
		if (index >= 0)
		{
			PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB =
				LDVExtensionsSetup::getWglCreatePbufferARB();
			PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB =
				LDVExtensionsSetup::getWglGetPbufferDCARB();
			PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivARB =
				LDVExtensionsSetup::getWglGetPixelFormatAttribivARB();

			if (wglCreatePbufferARB && wglGetPbufferDCARB &&
				wglGetPixelFormatAttribivARB)
			{
				int pfSizeAttribs[3] = {
					WGL_MAX_PBUFFER_WIDTH_ARB,
					WGL_MAX_PBUFFER_HEIGHT_ARB,
					WGL_MAX_PBUFFER_PIXELS_ARB
				};
				int attribValues[3];

				wglGetPixelFormatAttribivARB(hdc, index, 0, 3, pfSizeAttribs,
					attribValues);
				// This shouldn't be necessary, but ATI returns a PBuffer even
				// if we ask for one that is too big, so we can't rely on their
				// failure to trigger failure.  The one it returns CLAIMS to be
				// the size we asked for; it just doesn't work right.
				if (attribValues[0] >= imageWidth &&
					attribValues[1] >= imageHeight &&
					attribValues[2] >= imageWidth * imageHeight)
				{
					// Given the above check, the following shouldn't really
					// matter, but I'll leave it in anyway.
					GLint intValues[] = { 
						WGL_PBUFFER_LARGEST_ARB, 0,
						0, 0
					};
					hPBuffer = wglCreatePbufferARB(hdc, index, imageWidth,
						imageHeight, intValues);

					if (hPBuffer)
					{
						hPBufferDC = wglGetPbufferDCARB(hPBuffer);
						if (hPBufferDC)
						{
							hPBufferGLRC = wglCreateContext(hPBufferDC);

							if (hPBufferGLRC)
							{
							wglShareLists(hglrc, hPBufferGLRC);
								hCurrentDC = hPBufferDC;
								hCurrentGLRC = hPBufferGLRC;
								makeCurrent();
								if (currentAntialiasType && antialias &&
									LDVExtensionsSetup::
									haveNvMultisampleFilterHintExtension())
								{
									if (LDViewPreferences::
										getUseNvMultisampleFilter())
									{
										glHint(GL_MULTISAMPLE_FILTER_HINT_NV,
											GL_NICEST);
									}
									else
									{
										glHint(GL_MULTISAMPLE_FILTER_HINT_NV,
											GL_FASTEST);
									}
									glEnable(GL_MULTISAMPLE_ARB);
								}
								setupMaterial();
								setupLighting();
								glDepthFunc(GL_LEQUAL);
								glEnable(GL_DEPTH_TEST);
								glDrawBuffer(GL_FRONT);
								glReadBuffer(GL_FRONT);
								modelViewer->setWidth(imageWidth);
								modelViewer->setHeight(imageHeight);
								modelViewer->setup();
								modelViewer->pause();
								if (modelViewer->getNeedsReload())
								{
									return true;
								}
								else
								{
									// No need to recompile as before, because
									// we're sharing display lists.
									return true;
								}
							}
						}
					}
				}
			}
		}
		cleanupPBuffer();
		if (antialias)
		{
			return setupPBuffer(imageWidth, imageHeight, false);
		}
	}
	return false;
}

void ModelWindow::renderOffscreenImage(void)
{
	if (hPBufferGLRC)
	{
		makeCurrent();
//		wglMakeCurrent(hPBufferDC, hPBufferGLRC);
	}
	modelViewer->update();
	if (canSaveAlpha())
	{
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
		glEnable(GL_BLEND);
		orthoView();
		glColor4ub(0, 0, 0, 255 - 28);	// 255 - (110 / 4), which equals 2 faces
										// worth of standard alpha blending.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glDisable(GL_LIGHTING);
		glDepthRange(0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f((float)width, 0.0f, -1.0f);
			glVertex3f((float)width, (float)height, -1.0f);
			glVertex3f(0.0f, (float)height, -1.0f);
/*
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f(640.0f, 0.0f, -1.0f);
			glVertex3f(640.0f, 480.0f, -1.0f);
			glVertex3f(0.0f, 480.0f, -1.0f);
*/
		glEnd();
		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
}

void ModelWindow::cleanupPBuffer(void)
{
	if (hPBuffer)
	{
		PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB =
			LDVExtensionsSetup::getWglReleasePbufferDCARB();
		PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB =
			LDVExtensionsSetup::getWglDestroyPbufferARB();

		if (hPBufferDC)
		{
			hCurrentDC = hdc;
			hCurrentGLRC = hglrc;
			if (hPBufferGLRC)
			{
				wglDeleteContext(hPBufferGLRC);
				hPBufferGLRC = NULL;
				if (!savingFromCommandLine)
				{
					// If we're saving from the command line, there's no need to
					// put things back for regular rendering (particularly
					// recompiling the model, which takes quite a bit of extra
					// time.
					makeCurrent();
					modelViewer->setWidth(width);
					modelViewer->setHeight(height);
					modelViewer->recompile();
					modelViewer->unpause();
					modelViewer->setup();
				}
			}
			wglReleasePbufferDCARB(hPBuffer, hPBufferDC);
			hPBufferDC = NULL;
		}
		wglDestroyPbufferARB(hPBuffer);
		hPBuffer = NULL;
	}
}

bool ModelWindow::canSaveAlpha(void)
{
	if (saveAlpha && saveImageType == PNG_IMAGE_TYPE_INDEX)
	{
		int alphaBits;

		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		return alphaBits > 0;
	}
	return false;
}

BYTE *ModelWindow::grabImage(int &imageWidth, int &imageHeight, bool zoomToFit,
							 BYTE *buffer)
{
	RECT rect = {0, 0, 0, 0};
	HWND hParentWindow = GetParent(hWindow);
	bool needReset = false;
	bool oldSlowClear = modelViewer->getSlowClear();
	GLenum bufferFormat = GL_RGB;
	currentAntialiasType = TCUserDefaults::longForKey(FSAA_MODE_KEY);
	bool origForceZoomToFit = modelViewer->getForceZoomToFit();
	TCVector origCameraPosition = modelViewer->getCamera().getPosition();
	int newWidth = 1600;
	int newHeight = 1200;
	int numXTiles, numYTiles;
	int xTile;
	int yTile;
	BYTE *smallBuffer;
	int bytesPerPixel = 3;
	int bytesPerLine;
	int smallBytesPerLine;
	bool canceled = false;
	bool bufferAllocated = false;

	offscreenActive = true;
	if (zoomToFit)
	{
		modelViewer->setForceZoomToFit(true);
	}
	calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
		numYTiles);
	imageWidth = newWidth * numXTiles;
	imageHeight = newHeight * numYTiles;
	if (!setupPBuffer(newWidth, newHeight, currentAntialiasType > 0))
	{
		newWidth = width;		// width is OpenGL window width
		newHeight = height;		// height is OpenGL window height
		calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
			numYTiles);
		modelViewer->setSlowClear(true);
		GetWindowRect(hParentWindow, &rect);
		needReset = true;
		MoveWindow(hParentWindow, 0, 0, rect.right - rect.left,
			rect.bottom - rect.top, TRUE);
		modelViewer->setWidth(newWidth);
		modelViewer->setHeight(newHeight);
		makeCurrent();
		glReadBuffer(GL_BACK);
	}
	if (canSaveAlpha())
	{
		bytesPerPixel = 4;
		bufferFormat = GL_RGBA;
	}
	smallBytesPerLine = roundUp(newWidth * bytesPerPixel, 4);
	bytesPerLine = roundUp(imageWidth * bytesPerPixel, 4);
	if (!buffer)
	{
		buffer = new BYTE[bytesPerLine * imageHeight];
		bufferAllocated = true;
	}
	if (numXTiles == 1 && numYTiles == 1)
	{
		smallBuffer = buffer;
	}
	else
	{
		smallBuffer = new BYTE[smallBytesPerLine * newHeight];
	}
	modelViewer->setNumXTiles(numXTiles);
	modelViewer->setNumYTiles(numYTiles);
	for (yTile = 0; yTile < numYTiles; yTile++)
	{
		modelViewer->setYTile(yTile);
		for (xTile = 0; xTile < numXTiles && !canceled; xTile++)
		{
			modelViewer->setXTile(xTile);
			renderOffscreenImage();
			if (progressCallback("Rendering snapshot.",
				(float)(yTile * numXTiles + xTile) / (numYTiles * numXTiles)))
			{
				glReadPixels(0, 0, newWidth, newHeight, bufferFormat,
					GL_UNSIGNED_BYTE, smallBuffer);
				if (smallBuffer != buffer)
				{
					int x;
					int y;

					for (y = 0; y < newHeight; y++)
					{
						int smallOffset = y * smallBytesPerLine;
						int offset = (y + (numYTiles - yTile - 1) * newHeight) *
							bytesPerLine;

						for (x = 0; x < newWidth; x++)
						{
							int spot = offset + x * bytesPerPixel +
								xTile * newWidth * bytesPerPixel;
							int smallSpot = smallOffset + x * bytesPerPixel;

							buffer[spot] = smallBuffer[smallSpot];
							buffer[spot + 1] = smallBuffer[smallSpot + 1];
							buffer[spot + 2] = smallBuffer[smallSpot + 2];
						}
						// We only need to zoom to fit on the first tile; the
						// rest will already be correct.
						modelViewer->setForceZoomToFit(false);
					}
				}
			}
			else
			{
				canceled = true;
			}
		}
	}
	modelViewer->setXTile(0);
	modelViewer->setYTile(0);
	modelViewer->setNumXTiles(1);
	modelViewer->setNumYTiles(1);
	if (canceled && bufferAllocated)
	{
		delete buffer;
		buffer = NULL;
	}
	if (smallBuffer != buffer)
	{
		delete smallBuffer;
	}
	if (hPBuffer)
	{
		cleanupPBuffer();
	}
	else
	{
		modelViewer->setWidth(width);
		modelViewer->setHeight(height);
	}
	if (needReset)
	{
		MoveWindow(hParentWindow, rect.left, rect.top, rect.right - rect.left,
			rect.bottom - rect.top, TRUE);
		RedrawWindow(hParentWindow, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	}
	if (zoomToFit)
	{
		modelViewer->setForceZoomToFit(origForceZoomToFit);
		modelViewer->getCamera().setPosition(origCameraPosition);
	}
	offscreenActive = false;
	modelViewer->setSlowClear(oldSlowClear);
	return buffer;
}

bool ModelWindow::saveImage(char *filename, int imageWidth, int imageHeight,
							bool zoomToFit)
{
	BYTE *buffer = grabImage(imageWidth, imageHeight, zoomToFit);
	bool retValue = false;

	if (buffer)
	{
		if (saveImageType == PNG_IMAGE_TYPE_INDEX)
		{
			retValue = writePng(filename, imageWidth, imageHeight, buffer);
		}
		else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
		{
			retValue = writeBmp(filename, imageWidth, imageHeight, buffer);
		}
		delete buffer;
	}
	return retValue;
}

BOOL ModelWindow::doDialogInit(HWND hDlg, HWND /*hFocusWindow*/,
							   LPARAM /*lParam*/)
{
	if (hDlg == hPrintDialog)
	{
		setupPrintExtras();
	}
	else if (hDlg == hSaveDialog)
	{
		setupSaveExtras();
	}
	else if (hDlg == hPageSetupDialog)
	{
		setupPageSetupExtras();
	}
	return TRUE;
}

UINT CALLBACK ModelWindow::staticPrintHook(HWND hDlg, UINT uiMsg, WPARAM wParam,
										   LPARAM lParam)
{
	ModelWindow* modelWindow = (ModelWindow*)GetWindowLong(hDlg, GWL_USERDATA);

	if (uiMsg == WM_INITDIALOG)
	{
		modelWindow = (ModelWindow*)((PRINTDLG*)lParam)->lCustData;
		if (modelWindow)
		{
			modelWindow->hPrintDialog = hDlg;
			SetWindowLong(hDlg, GWL_USERDATA, (long)modelWindow);
		}
	}
	if (modelWindow)
	{
		return modelWindow->dialogProc(hDlg, uiMsg, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

bool ModelWindow::selectPrinter(PRINTDLG &pd)
{
	HGLOBAL hDevMode = GlobalAlloc(GHND, sizeof(DEVMODE));
	DEVMODE *devMode = (DEVMODE *)GlobalLock(hDevMode);
	bool retValue;

	devMode->dmFields = DM_ORIENTATION | DM_PAPERSIZE;
	devMode->dmOrientation = (short)printOrientation;
	devMode->dmPaperSize = (short)printPaperSize;
	GlobalUnlock(hDevMode);
	// Initialize the PRINTDLG members. 
 	pd.lStructSize = sizeof(PRINTDLG);
	pd.hDevMode = hDevMode;
	pd.hDevNames = NULL;
	pd.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;
	// | PD_ENABLEPRINTHOOK | PD_ENABLEPRINTTEMPLATE;
	pd.hwndOwner = NULL;//parentWindow->getHWindow();
	pd.hDC = NULL;
	pd.nFromPage = 1;
	pd.nToPage = 1;
	pd.nMinPage = 0;
	pd.nMaxPage = 0;
	pd.nCopies = 1;
	pd.hInstance = getLanguageModule();
	pd.lCustData = (long)this;
	pd.lpfnPrintHook = staticPrintHook;
	pd.lpfnSetupHook = NULL;
	pd.lpPrintTemplateName = MAKEINTRESOURCE(PRINTDLGORD);
	pd.lpSetupTemplateName = NULL;
	pd.hPrintTemplate = NULL;
	pd.hSetupTemplate = NULL;
 
	// Display the PRINT dialog box. 
 
	if (PrintDlg(&pd))
	{
		hPrintDialog = NULL;
		retValue = true;
	}
	else
	{
//		DWORD error = CommDlgExtendedError();
		hPrintDialog = NULL;
		retValue = false;
	}
	GlobalFree(hDevMode);
	return retValue;
}

HBITMAP ModelWindow::createDIBSection(HDC hBitmapDC, int bitmapWidth,
									  int bitmapHeight, int hDPI, int vDPI,
									  BYTE **bmBuffer)
{
	BITMAPINFO bmi;

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = bitmapWidth;
	bmi.bmiHeader.biHeight = bitmapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;//roundUp(bitmapWidth * 3, 4) * bitmapHeight;
	bmi.bmiHeader.biXPelsPerMeter = (long)(hDPI * 39.37);
	bmi.bmiHeader.biYPelsPerMeter = (long)(vDPI * 39.37);
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	return CreateDIBSection(hBitmapDC, &bmi, DIB_RGB_COLORS,
		(void**)bmBuffer, NULL, 0);
}

void ModelWindow::swap(int &left, int &right)
{
	int temp = left;

	left = right;
	right = temp;
}

void ModelWindow::calcTiling(int desiredWidth, int desiredHeight,
							 int &bitmapWidth, int &bitmapHeight,
							 int &numXTiles, int &numYTiles)
{
	if (desiredWidth > bitmapWidth)
	{
		numXTiles = (desiredWidth + bitmapWidth - 1) / bitmapWidth;
	}
	else
	{
		numXTiles = 1;
	}
	bitmapWidth = desiredWidth / numXTiles;
	if (desiredHeight > bitmapHeight)
	{
		numYTiles = (desiredHeight + bitmapHeight - 1) / bitmapHeight;
	}
	else
	{
		numYTiles = 1;
	}
	bitmapHeight = desiredHeight / numYTiles;
}

void ModelWindow::calcTiling(HDC hPrinterDC, int &bitmapWidth,
							 int &bitmapHeight, int &numXTiles, int &numYTiles)
{
	int hRes;
	int vRes;
	int printXOffset;
	int printYOffset;
	int printWidth;
	int printHeight;
	int printRMarginPixels;
	int printBMarginPixels;
	int maxBitmapWidth = bitmapWidth;
	int maxBitmapHeight = bitmapHeight;

	printHDPI = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	printVDPI = GetDeviceCaps(hPrinterDC, LOGPIXELSY);
	printXOffset = GetDeviceCaps(hPrinterDC, PHYSICALOFFSETX);
	printYOffset = GetDeviceCaps(hPrinterDC, PHYSICALOFFSETY);
	printWidth = GetDeviceCaps(hPrinterDC, PHYSICALWIDTH);
	printHeight = GetDeviceCaps(hPrinterDC, PHYSICALHEIGHT);
	hRes = GetDeviceCaps(hPrinterDC, HORZRES);
	vRes = GetDeviceCaps(hPrinterDC, VERTRES);
	printLMarginPixels = (int)(printLeftMargin * printHDPI) - printXOffset;
	printRMarginPixels = (int)(printRightMargin * printHDPI) + hRes -
		printWidth + printXOffset;
	printTMarginPixels = (int)(printTopMargin * printVDPI) - printYOffset;
	printBMarginPixels = (int)(printBottomMargin * printVDPI) + vRes -
		printHeight + printYOffset;
	bitmapWidth = hRes - printLMarginPixels - printRMarginPixels;
	if (bitmapWidth > maxBitmapWidth)
	{
		numXTiles = (bitmapWidth + maxBitmapWidth - 1) / maxBitmapWidth;
	}
	else
	{
		numXTiles = 1;
	}
	bitmapWidth = bitmapWidth / numXTiles;
	bitmapHeight = vRes - printTMarginPixels - printBMarginPixels;
	if (bitmapHeight > maxBitmapHeight)
	{
		numYTiles = (bitmapHeight + maxBitmapHeight - 1) / maxBitmapHeight;
	}
	else
	{
		numYTiles = 1;
	}
	bitmapHeight = bitmapHeight / numYTiles;
}

bool ModelWindow::printPage(const PRINTDLG &pd)
{
	bool retValue = false;
	HDC hBitmapDC = CreateCompatibleDC(pd.hDC);

	if (hBitmapDC)
	{
		int bitmapWidth = 1600;
		int bitmapHeight = 1200;
		int renderWidth;
		int renderHeight;
		int numXTiles;
		int numYTiles;
		bool oldSlowClear = modelViewer->getSlowClear();
		int oldR = modelViewer->getBackgroundR();
		int oldG = modelViewer->getBackgroundG();
		int oldB = modelViewer->getBackgroundB();
		int oldA = modelViewer->getBackgroundA();
		LDVStereoMode oldStereoMode = modelViewer->getStereoMode();
		BYTE *bmBuffer = NULL;
		HBITMAP hBitmap = NULL;
		bool canceled = false;
		bool landscape = printOrientation == DMORIENT_LANDSCAPE;

		calcTiling(pd.hDC, bitmapWidth, bitmapHeight, numXTiles, numYTiles);
		if (landscape)
		{
			renderWidth = bitmapHeight;
			renderHeight = bitmapWidth;
			swap(numXTiles, numYTiles);
		}
		else
		{
			renderWidth = bitmapWidth;
			renderHeight = bitmapHeight;
		}
		offscreenActive = true;
		if (printHDPI != printVDPI)
		{
			modelViewer->setPixelAspectRatio((float)printHDPI / printVDPI);
		}
		if (printBackground)
		{
			modelViewer->setBackgroundRGBA(oldR, oldG, oldB, oldA);
		}
		else
		{
			modelViewer->setBackgroundRGBA(255, 255, 255, 255);
		}
		modelViewer->setStereoMode(LDVStereoNone);
		if (!setupPBuffer(renderWidth, renderHeight))
		{
			if (!modelViewer->getCompiled())
			{
				canceled = true;
			}
			else
			{
				if (landscape)
				{
					bitmapWidth = height;
					bitmapHeight = width;
					swap(numXTiles, numYTiles);
				}
				else
				{
					bitmapWidth = width;
					bitmapHeight = height;
				}
				calcTiling(pd.hDC, bitmapWidth, bitmapHeight, numXTiles,
					numYTiles);
				if (landscape)
				{
					renderWidth = bitmapHeight;
					renderHeight = bitmapWidth;
				}
				else
				{
					renderWidth = bitmapWidth;
					renderHeight = bitmapHeight;
				}
				modelViewer->setWidth(renderWidth);
				modelViewer->setHeight(renderHeight);
				makeCurrent();
				glReadBuffer(GL_BACK);
				modelViewer->setSlowClear(true);
			}
		}
		if (!canceled)
		{
			hBitmap = createDIBSection(hBitmapDC, bitmapWidth, bitmapHeight,
				printHDPI, printVDPI, &bmBuffer);
		}
		if (hBitmap)
		{
			int xTile, yTile;
			int renderLineSize = roundUp(renderWidth * 3, 4);
			BYTE *buffer = new BYTE[renderLineSize * renderHeight];
			float oldHighlightLineWidth = modelViewer->getHighlightLineWidth();
			float oldWireframeLineWidth = modelViewer->getWireframeLineWidth();

			modelViewer->setNumXTiles(numXTiles);
			modelViewer->setNumYTiles(numYTiles);
			if (landscape)
			{
				swap(numXTiles, numYTiles);
			}
			modelViewer->setHighlightLineWidth(printHDPI / 100 *
				oldHighlightLineWidth);
			modelViewer->setWireframeLineWidth(printHDPI / 100 *
				oldWireframeLineWidth);
			for (yTile = 0; yTile < numYTiles && !canceled; yTile++)
			{
				if (landscape)
				{
					modelViewer->setXTile(yTile);
				}
				else
				{
					modelViewer->setYTile(yTile);
				}
				progressCallback("Printing Model.", 0.0f);
				for (xTile = 0; xTile < numXTiles && !canceled; xTile++)
				{
					int x, y;

					if (progressCallback(NULL, (float)(yTile *
						numXTiles + xTile) / (numYTiles * numXTiles)))
					{
						if (landscape)
						{
							modelViewer->setYTile(numXTiles - xTile - 1);
						}
						else
						{
							modelViewer->setXTile(xTile);
						}
						renderOffscreenImage();
						glReadPixels(0, 0, renderWidth, renderHeight, GL_RGB,
							GL_UNSIGNED_BYTE, buffer);
						if (landscape)
						{
							int bitmapLineSize = roundUp(bitmapWidth * 3, 4);

							for (y = 0; y < renderHeight; y++)
							{
								int offset = y * renderLineSize;
								for (x = 0; x < renderWidth; x++)
								{
									int renderSpot = offset + x * 3;
									int bitmapSpot = (renderWidth - x - 1)
										* bitmapLineSize + y * 3;

									bmBuffer[bitmapSpot] =
										buffer[renderSpot + 2];
									bmBuffer[bitmapSpot + 1] =
										buffer[renderSpot + 1];
									bmBuffer[bitmapSpot + 2] =
										buffer[renderSpot];
								}
							}
						}
						else
						{
							for (y = 0; y < bitmapHeight; y++)
							{
								int offset = y * renderLineSize;

								for (x = 0; x < bitmapWidth; x++)
								{
									int spot = offset + x * 3;

									bmBuffer[spot] = buffer[spot + 2];
									bmBuffer[spot + 1] = buffer[spot + 1];
									bmBuffer[spot + 2] = buffer[spot];
								}
							}
						}
						SelectObject(hBitmapDC, hBitmap);
						if (BitBlt(pd.hDC,
							printLMarginPixels + xTile * bitmapWidth,
							printTMarginPixels + yTile * bitmapHeight,
							bitmapWidth, bitmapHeight, hBitmapDC, 0, 0, 
							SRCCOPY))
						{
							retValue = true;
						}
					}
					else
					{
						canceled = true;
						retValue = false;
					}
				}
			}
			progressCallback(NULL, 1.0f);
			DeleteObject(hBitmap);
			modelViewer->setHighlightLineWidth(oldHighlightLineWidth);
			modelViewer->setWireframeLineWidth(oldWireframeLineWidth);
			modelViewer->setXTile(0);
			modelViewer->setYTile(0);
			modelViewer->setNumXTiles(1);
			modelViewer->setNumYTiles(1);
			delete buffer;
			progressCallback(NULL, 2.0f);
		}
		if (hPBuffer)
		{
			cleanupPBuffer();
		}
		else
		{
			modelViewer->setWidth(width);
			modelViewer->setHeight(height);
			modelViewer->setSlowClear(oldSlowClear);
		}
		modelViewer->setBackgroundRGBA(oldR, oldG, oldB, oldA);
		modelViewer->setStereoMode(oldStereoMode);
		modelViewer->setPixelAspectRatio(1.0f);
		DeleteDC(hBitmapDC);
	}
	offscreenActive = false;
	return retValue;
}

bool ModelWindow::parseDevMode(HGLOBAL hDevMode)
{
	DEVMODE *devMode = (DEVMODE *)GlobalLock(hDevMode);
	bool retValue = true;

	if (devMode->dmOrientation != printOrientation)
	{
		printOrientation = devMode->dmOrientation;
		TCUserDefaults::setLongForKey(printOrientation, ORIENTATION_KEY, false);
	}
	if (devMode->dmPaperSize != printPaperSize)
	{
		if (devMode->dmPaperSize < DMPAPER_USER)
		{
			printPaperSize = devMode->dmPaperSize;
			TCUserDefaults::setLongForKey(printPaperSize, PAPER_SIZE_KEY,
				false);
		}
		else
		{
			retValue = false;
		}
	}
	GlobalUnlock(hDevMode);
	return retValue;
}

UINT CALLBACK ModelWindow::staticPageSetupHook(HWND hDlg, UINT uiMsg,
											   WPARAM wParam, LPARAM lParam)
{
	ModelWindow* modelWindow = (ModelWindow*)GetWindowLong(hDlg, GWL_USERDATA);

	if (uiMsg == WM_INITDIALOG)
	{
		modelWindow = (ModelWindow*)((PAGESETUPDLG*)lParam)->lCustData;
		if (modelWindow)
		{
			modelWindow->hPageSetupDialog = hDlg;
			SetWindowLong(hDlg, GWL_USERDATA, (long)modelWindow);
		}
	}
	if (modelWindow)
	{
		return modelWindow->dialogProc(hDlg, uiMsg, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

bool ModelWindow::pageSetup(void)
{
	PAGESETUPDLG psd;
	HGLOBAL hDevMode = GlobalAlloc(GHND, sizeof(DEVMODE));
	DEVMODE *devMode = (DEVMODE *)GlobalLock(hDevMode);
	bool retValue;

	devMode->dmFields = DM_ORIENTATION | DM_PAPERSIZE;
	devMode->dmOrientation = (short)printOrientation;
	devMode->dmPaperSize = (short)printPaperSize;
	GlobalUnlock(hDevMode);
	psd.lStructSize = sizeof(PAGESETUPDLG);
	psd.hwndOwner = GetParent(hWindow);
	psd.hDevMode = hDevMode;
	psd.hDevNames = NULL;
	psd.Flags = PSD_DISABLEPRINTER | PSD_INTHOUSANDTHSOFINCHES | PSD_MARGINS |
		PSD_ENABLEPAGESETUPTEMPLATE | PSD_ENABLEPAGESETUPHOOK;
	psd.rtMargin.left = (long)(printLeftMargin * 1000);
	psd.rtMargin.right = (long)(printRightMargin * 1000);
	psd.rtMargin.top = (long)(printTopMargin * 1000);
	psd.rtMargin.bottom = (long)(printBottomMargin * 1000);
	psd.lCustData = (long)this;
	psd.hInstance = getLanguageModule();
	psd.lpfnPageSetupHook = staticPageSetupHook;
	psd.lpPageSetupTemplateName = MAKEINTRESOURCE(PAGESETUPDLGORD);

	if (PageSetupDlg(&psd))
	{
		printLeftMargin = psd.rtMargin.left / 1000.0f;
		printRightMargin = psd.rtMargin.right / 1000.0f;
		printTopMargin = psd.rtMargin.top / 1000.0f;
		printBottomMargin = psd.rtMargin.bottom / 1000.0f;
		if (!parseDevMode(psd.hDevMode))
		{
			MessageBox(hParentWindow, "Custom paper sizes are not supported.  "
				"Using previous setting instead.", "Paper Size",
				MB_OK | MB_ICONEXCLAMATION);
		}
		TCUserDefaults::setLongForKey(psd.rtMargin.left, LEFT_MARGIN_KEY,
			false);
		TCUserDefaults::setLongForKey(psd.rtMargin.right, RIGHT_MARGIN_KEY,
			false);
		TCUserDefaults::setLongForKey(psd.rtMargin.top, TOP_MARGIN_KEY, false);
		TCUserDefaults::setLongForKey(psd.rtMargin.bottom, BOTTOM_MARGIN_KEY,
			false);
		TCUserDefaults::setLongForKey(printBackground ? 1 : 0,
			PRINT_BACKGROUND_KEY, false);
		retValue = true;
	}
	else
	{
		retValue = false;
	}
	GlobalFree(hDevMode);
	return retValue;
}

bool ModelWindow::print(void)
{
	PRINTDLG pd;
	bool retValue = false;

	if (selectPrinter(pd))
	{
		DOCINFO di;
		int printJobId;

		parseDevMode(pd.hDevMode);
		TCUserDefaults::setLongForKey(usePrinterDPI ? 1 : 0,
			USE_PRINTER_DPI_KEY);
		TCUserDefaults::setLongForKey(printDPI, PRINT_DPI_KEY);
		TCUserDefaults::setLongForKey(printBackground ? 1 : 0,
			PRINT_BACKGROUND_KEY, false);
			
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = parentWindow->getWindowTitle();
		di.lpszOutput = NULL;
		di.lpszDatatype = NULL;
		di.fwType = 0;
		printJobId = StartDoc(pd.hDC, &di);
		if (printJobId > 0 && StartPage(pd.hDC) > 0)
		{
			if (printPage(pd))
			{
				retValue = true;
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
			}
			else
			{
				EndPage(pd.hDC);
				AbortDoc(pd.hDC);
			}
		}
	}
	return retValue;
}

void ModelWindow::disableSaveSize(void)
{
	EnableWindow(hSaveWidthLabel, FALSE);
	EnableWindow(hSaveWidth, FALSE);
	EnableWindow(hSaveHeightLabel, FALSE);
	EnableWindow(hSaveHeight, FALSE);
	EnableWindow(hSaveZoomToFitButton, FALSE);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_WIDTH, WM_SETTEXT, 0, (LPARAM)"");
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_HEIGHT, WM_SETTEXT, 0, (LPARAM)"");
}

void ModelWindow::enableSaveSize(void)
{
	char buf[128];

	EnableWindow(hSaveWidthLabel, TRUE);
	EnableWindow(hSaveWidth, TRUE);
	EnableWindow(hSaveHeightLabel, TRUE);
	EnableWindow(hSaveHeight, TRUE);
	EnableWindow(hSaveZoomToFitButton, TRUE);
	sprintf(buf, "%d", saveWidth);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_WIDTH, WM_SETTEXT, 0, (LPARAM)buf);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_WIDTH, EM_LIMITTEXT, 4, 0);
	sprintf(buf, "%d", saveHeight);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_HEIGHT, WM_SETTEXT, 0,
		(LPARAM)buf);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_HEIGHT, EM_LIMITTEXT, 4, 0);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_ZOOMTOFIT, BM_SETCHECK,
		saveZoomToFit ? 1 : 0, 0);
}

void ModelWindow::disableSaveSeries(void)
{
	EnableWindow(hSaveDigitsLabel, FALSE);
	EnableWindow(hSaveDigitsField, FALSE);
	EnableWindow(hSaveDigitsSpin, FALSE);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_DIGITS, WM_SETTEXT, 0, (LPARAM)"");
}

void ModelWindow::enableSaveSeries(void)
{
	EnableWindow(hSaveDigitsLabel, TRUE);
	EnableWindow(hSaveDigitsField, TRUE);
	EnableWindow(hSaveDigitsSpin, TRUE);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_DIGITS_SPIN, UDM_SETPOS, 0,
		MAKELONG(saveDigits, 0));
}

void ModelWindow::updatePrintDPIField(void)
{
	if (usePrinterDPI)
	{
		EnableWindow(hPrintDPI, FALSE);
		SendDlgItemMessage(hPrintDialog, IDC_PRINT_DPI, WM_SETTEXT, 0,
			(LPARAM)"");
	}
	else
	{
		char buf[128];

		sprintf(buf, "%d", printDPI);
		EnableWindow(hPrintDPI, TRUE);
		SendDlgItemMessage(hPrintDialog, IDC_PRINT_DPI, WM_SETTEXT, 0,
			(LPARAM)buf);
	}
}

void ModelWindow::setupPrintExtras(void)
{
	HWND hIconWindow = GetDlgItem(hPrintDialog, 1086);
	RECT iconRect;
	POINT iconPoint;

	hPrintDPI = GetDlgItem(hPrintDialog, IDC_PRINT_DPI);
	GetWindowRect(hIconWindow, &iconRect);
	iconPoint.x = iconRect.left;
	iconPoint.y = iconRect.top;
	ScreenToClient(hPrintDialog, &iconPoint);
	MoveWindow(hIconWindow, iconPoint.x, iconPoint.y, 114, 36, TRUE);
	SendDlgItemMessage(hPrintDialog, IDC_PRINT_PRINTER_DPI, BM_SETCHECK,
		usePrinterDPI ? 1 : 0, 0);
	SendDlgItemMessage(hPrintDialog, IDC_PRINT_SPECIFY_DPI, BM_SETCHECK,
		usePrinterDPI ? 0 : 1, 0);
	SendDlgItemMessage(hPrintDialog, IDC_PRINT_BACKGROUND, BM_SETCHECK,
		printBackground ? 1 : 0, 0);
	SendDlgItemMessage(hPrintDialog, IDC_PRINT_DPI, EM_LIMITTEXT, 4, 0);
	updatePrintDPIField();
}

void ModelWindow::setupPageSetupExtras(void)
{
	SendDlgItemMessage(hPageSetupDialog, IDC_PRINT_BACKGROUND, BM_SETCHECK,
		printBackground ? 1 : 0, 0);
}

void ModelWindow::setupSaveExtras(void)
{
	hSaveWidthLabel = GetDlgItem(hSaveDialog, IDC_SAVE_WIDTH_LABEL);
	hSaveWidth = GetDlgItem(hSaveDialog, IDC_SAVE_WIDTH);
	hSaveHeightLabel = GetDlgItem(hSaveDialog, IDC_SAVE_HEIGHT_LABEL);
	hSaveHeight = GetDlgItem(hSaveDialog, IDC_SAVE_HEIGHT);
	hSaveZoomToFitButton = GetDlgItem(hSaveDialog, IDC_SAVE_ZOOMTOFIT);
	hSaveDigitsLabel = GetDlgItem(hSaveDialog, IDC_SAVE_DIGITS_LABEL);
	hSaveDigitsField = GetDlgItem(hSaveDialog, IDC_SAVE_DIGITS);
	hSaveDigitsSpin = GetDlgItem(hSaveDialog, IDC_SAVE_DIGITS_SPIN);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_ACTUAL_SIZE, BM_SETCHECK,
		saveActualSize ? 0 : 1, 0);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_SERIES, BM_SETCHECK,
		saveSeries ? 1 : 0, 0);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_DIGITS_SPIN, UDM_SETRANGE32, 1, 5);
	SendDlgItemMessage(hSaveDialog, IDC_SAVE_DIGITS_SPIN, UDM_SETPOS, 0,
		MAKELONG(saveDigits, 0));
	if (saveActualSize)
	{
		disableSaveSize();
	}
	else
	{
		enableSaveSize();
	}
	if (saveSeries)
	{
		enableSaveSeries();
	}
	else
	{
		disableSaveSeries();
	}
}

void ModelWindow::updateSaveWidth(void)
{
	if (!saveActualSize)
	{
		char buf[128];
		int temp;

		SendDlgItemMessage(hSaveDialog, IDC_SAVE_WIDTH, WM_GETTEXT, 128,
			(LPARAM)buf);
		if (sscanf(buf, "%d", &temp) == 1 && temp > 0 &&
			temp <= MAX_SNAPSHOT_WIDTH)
		{
			saveWidth = temp;
		}
		else
		{
			sprintf(buf, "%d", saveWidth);
			SendDlgItemMessage(hSaveDialog, IDC_SAVE_WIDTH, WM_SETTEXT, 0,
				(LPARAM)buf);
			MessageBeep(MB_OK);
		}
	}
}

void ModelWindow::updateSaveHeight(void)
{
	if (!saveActualSize)
	{
		char buf[128];
		int temp;

		SendDlgItemMessage(hSaveDialog, IDC_SAVE_HEIGHT, WM_GETTEXT, 128,
			(LPARAM)buf);
		if (sscanf(buf, "%d", &temp) == 1 && temp > 0 &&
			temp <= MAX_SNAPSHOT_HEIGHT)
		{
			saveHeight = temp;
		}
		else
		{
			sprintf(buf, "%d", saveHeight);
			SendDlgItemMessage(hSaveDialog, IDC_SAVE_HEIGHT, WM_SETTEXT, 0,
				(LPARAM)buf);
			MessageBeep(MB_OK);
		}
	}
}

void ModelWindow::updateSaveDigits(void)
{
	char buf[1024];
	int temp;

	SendDlgItemMessage(hSaveDialog, IDC_SAVE_DIGITS, WM_GETTEXT, 128,
		(LPARAM)buf);
	if (sscanf(buf, "%d", &temp) == 1 && temp > 0 && temp <= 6)
	{
		saveDigits = temp;
	}
	if (calcSaveFilename(buf, 1024))
	{
		SendMessage(GetParent(hSaveDialog), CDM_SETCONTROLTEXT, edt1,
			(LPARAM)buf);
	}
}

BOOL ModelWindow::doSaveClick(int controlId, HWND /*hControlWnd*/)
{
	switch (controlId)
	{
	case IDC_SAVE_ACTUAL_SIZE:
		saveActualSize = SendDlgItemMessage(hSaveDialog,
			IDC_SAVE_ACTUAL_SIZE, BM_GETCHECK, 0, 0) ? false : true;
		if (saveActualSize)
		{
			disableSaveSize();
		}
		else
		{
			enableSaveSize();
		}
		break;
	case IDC_SAVE_SERIES:
		saveSeries = SendDlgItemMessage(hSaveDialog, IDC_SAVE_SERIES,
			BM_GETCHECK, 0, 0) ? true : false;
		if (saveSeries)
		{
			enableSaveSeries();
		}
		else
		{
			disableSaveSeries();
		}
		break;
	case IDC_SAVE_ZOOMTOFIT:
		saveZoomToFit = SendDlgItemMessage(hSaveDialog, IDC_SAVE_ZOOMTOFIT,
			BM_GETCHECK, 0, 0) ? true : false;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL ModelWindow::doSaveKillFocus(int controlId, HWND /*hControlWnd*/)
{
	switch (controlId)
	{
	case IDC_SAVE_WIDTH:
		updateSaveWidth();
		break;
	case IDC_SAVE_HEIGHT:
		updateSaveHeight();
		break;
	case IDC_SAVE_DIGITS:
		updateSaveDigits();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL ModelWindow::doSaveNotify(int /*controlId*/, LPOFNOTIFY notification)
{
	if (notification->hdr.code == CDN_TYPECHANGE)
	{
		char buf[1024];

		saveImageType = notification->lpOFN->nFilterIndex;
		if (calcSaveFilename(buf, 1024))
		{
			SendMessage(GetParent(hSaveDialog), CDM_SETCONTROLTEXT, edt1,
				(LPARAM)buf);
		}
	}
	return FALSE;
}

BOOL ModelWindow::doSaveCommand(int controlId, int notifyCode, HWND hControlWnd)
{
//	debugPrintf("ModelWindow::doSaveCommand: 0x%04X, 0x%04X, 0x%04x\n",
//		controlId, notifyCode, hControlWnd);
	switch (notifyCode)
	{
	case BN_CLICKED:
		return doSaveClick(controlId, hControlWnd);
		break;
	case EN_KILLFOCUS:
		return doSaveKillFocus(controlId, hControlWnd);
		break;
	case EN_CHANGE:
		if (controlId == IDC_SAVE_DIGITS)
		{
			updateSaveDigits();
		}
		break;
	}
	return FALSE;
}

BOOL ModelWindow::doPrintClick(int controlId, HWND /*hControlWnd*/)
{
	switch (controlId)
	{
	case IDC_PRINT_PRINTER_DPI:
		usePrinterDPI = true;
		updatePrintDPIField();
		break;
	case IDC_PRINT_SPECIFY_DPI:
		usePrinterDPI = false;
		updatePrintDPIField();
		break;
	case IDC_PRINT_BACKGROUND:
		printBackground = SendDlgItemMessage(hPrintDialog, IDC_PRINT_BACKGROUND,
			BM_GETCHECK, 0, 0) ? true : false;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void ModelWindow::updatePrintDPI(void)
{
	char buf[1024];
	int temp;

	SendDlgItemMessage(hPrintDialog, IDC_PRINT_DPI, WM_GETTEXT, 128,
		(LPARAM)buf);
	if (sscanf(buf, "%d", &temp) == 1 && temp > 0 && temp <= 2400)
	{
		printDPI = temp;
	}
	else
	{
		sprintf(buf, "%d", printDPI);
		SendDlgItemMessage(hPrintDialog, IDC_PRINT_DPI, WM_SETTEXT, 0,
			(LPARAM)buf);
		MessageBeep(MB_OK);
	}
}

BOOL ModelWindow::doPrintKillFocus(int controlId, HWND /*hControlWnd*/)
{
	switch (controlId)
	{
	case IDC_PRINT_DPI:
		updatePrintDPI();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL ModelWindow::doPrintCommand(int controlId, int notifyCode,
								 HWND hControlWnd)
{
	switch (notifyCode)
	{
	case BN_CLICKED:
		return doPrintClick(controlId, hControlWnd);
		break;
	case EN_KILLFOCUS:
		return doPrintKillFocus(controlId, hControlWnd);
		break;
	}
	return FALSE;
}

UINT CALLBACK ModelWindow::staticSaveHook(HWND hDlg, UINT uiMsg, WPARAM wParam,
										  LPARAM lParam)
{
	ModelWindow* modelWindow = (ModelWindow*)GetWindowLong(hDlg, GWL_USERDATA);

	if (uiMsg == WM_INITDIALOG)
	{
		modelWindow = (ModelWindow*)((OPENFILENAME*)lParam)->lCustData;
		if (modelWindow)
		{
			modelWindow->hSaveDialog = hDlg;
			SetWindowLong(hDlg, GWL_USERDATA, (long)modelWindow);
		}
	}
	if (modelWindow)
	{
		return modelWindow->dialogProc(hDlg, uiMsg, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

bool ModelWindow::calcSaveFilename(char* saveFilename, int /*len*/)
{
	char* filename = modelViewer->getFilename();

	if (filename)
	{
		char baseFilename[1024];

		if (strrchr(filename, '/'))
		{
			filename = strrchr(filename, '/') + 1;
		}
		if (strrchr(filename, '\\'))
		{
			filename = strrchr(filename, '\\') + 1;
		}
		strcpy(baseFilename, filename);
		if (strchr(baseFilename, '.'))
		{
			*strchr(baseFilename, '.') = 0;
		}
		if (saveSeries)
		{
			int max = (int)(pow(10.0, saveDigits) + 0.1);
			int i;
			char format[32];

			sprintf(format, "%%s%%0%dd.%%s", saveDigits);
			for (i = 1; i < max; i++)
			{
				if (saveImageType == PNG_IMAGE_TYPE_INDEX)
				{
					sprintf(saveFilename, format, baseFilename, i, "png");
				}
				else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
				{
					sprintf(saveFilename, format, baseFilename, i, "bmp");
				}
				if (!fileExists(saveFilename))
				{
					return true;
				}
			}
		}
		else
		{
			sprintf(saveFilename, "%s.%s", baseFilename, "png");
		}
		return true;
	}
	return false;
}

bool ModelWindow::getSaveFilename(char* saveFilename, int len)
{
	OPENFILENAME openStruct;
	char fileTypes[1024];
	char* initialDir = ".";

	if (calcSaveFilename(saveFilename, len))
	{
		int maxImageType = 2;

		if (saveImageType < 1 || saveImageType > maxImageType)
		{
			saveImageType = 1;
		}
		memset(fileTypes, 0, 2);
		LDViewWindow::addFileType(fileTypes, "PNG: Portable Network Graphics",
			"*.png");
		LDViewWindow::addFileType(fileTypes, "BMP: Windows Bitmap", "*.bmp");
		memset(&openStruct, 0, sizeof(OPENFILENAME));
		openStruct.lStructSize = sizeof(OPENFILENAME);
		openStruct.hwndOwner = hWindow;
		openStruct.lpstrFilter = fileTypes;
		openStruct.nFilterIndex = saveImageType;
		openStruct.lpstrFile = saveFilename;
		openStruct.nMaxFile = len;
		openStruct.lpstrInitialDir = initialDir;
		openStruct.lpstrTitle = "Save Snapshot";
		openStruct.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE
			| OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT;
		openStruct.lpstrDefExt = NULL;
		openStruct.lCustData = (long)this;
		openStruct.hInstance = getLanguageModule();
		openStruct.lpTemplateName = MAKEINTRESOURCE(IDD_SAVE_OPTIONS);
		openStruct.lpfnHook = staticSaveHook;
		if (GetSaveFileName(&openStruct))
		{
			int index = (int)openStruct.nFilterIndex;

			if (index > 0 && index <= maxImageType)
			{
				TCUserDefaults::setLongForKey(saveImageType,
					SAVE_IMAGE_TYPE_KEY, false);
			}
			if (!strchr(saveFilename, '.') &&
				(int)strlen(saveFilename) < len - 5)
			{
				if (saveImageType == PNG_IMAGE_TYPE_INDEX)
				{
					strcat(saveFilename, ".png");
				}
				else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
				{
					strcat(saveFilename, ".bmp");
				}

			}
			return true;
		}
	}
	return false;
}

bool ModelWindow::fileExists(char* filename)
{
	FILE* file = fopen(filename, "r");

	if (file)
	{
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}

bool ModelWindow::shouldOverwriteFile(char* filename)
{
	char buf[256];

	sprintf(buf, "%s\nThis file already exists.\nReplace existing file?",
		filename);
	if (MessageBox(hWindow, buf, "LDView", MB_YESNO | MB_ICONQUESTION) ==
		IDYES)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ModelWindow::saveSnapshot(void)
{
	char saveFilename[1024] = "";
	bool retValue = saveSnapshot(saveFilename);

	forceRedraw();
	return retValue;
}

bool ModelWindow::saveSnapshot(char *saveFilename, bool fromCommandLine)
{
	bool externalFilename = saveFilename[0] != 0;

	savingFromCommandLine = fromCommandLine;
	if (saveFilename[0])
	{
		if (stringHasCaseInsensitiveSuffix(saveFilename, ".png"))
		{
			saveImageType = PNG_IMAGE_TYPE_INDEX;
		}
		else if (stringHasCaseInsensitiveSuffix(saveFilename, ".bmp"))
		{
			saveImageType = BMP_IMAGE_TYPE_INDEX;
		}
		else
		{
			return false;
		}
	}
	if (saveFilename[0] || getSaveFilename(saveFilename, 1024))
	{
		if (saveSeries)
		{
			TCUserDefaults::setLongForKey(1, SAVE_SERIES_KEY, false);
			TCUserDefaults::setLongForKey(saveDigits, SAVE_DIGITS_KEY, false);
		}
		else
		{
			TCUserDefaults::setLongForKey(0, SAVE_SERIES_KEY, false);
		}
		if (saveActualSize)
		{
			TCUserDefaults::setLongForKey(1, SAVE_ACTUAL_SIZE_KEY, false);
			return saveImage(saveFilename, width, height, externalFilename &&
				saveZoomToFit);
		}
		else
		{
			TCUserDefaults::setLongForKey(0, SAVE_ACTUAL_SIZE_KEY, false);
			TCUserDefaults::setLongForKey(saveWidth, SAVE_WIDTH_KEY, false);
			TCUserDefaults::setLongForKey(saveHeight, SAVE_HEIGHT_KEY, false);
			TCUserDefaults::setLongForKey(saveZoomToFit, SAVE_ZOOM_TO_FIT_KEY,
				false);
			return saveImage(saveFilename, saveWidth, saveHeight,
				saveZoomToFit);
		}
	}
	else
	{
		return false;
	}
}

BOOL ModelWindow::setupPFD(void)
{
	currentAntialiasType = TCUserDefaults::longForKey(FSAA_MODE_KEY);

	if (currentAntialiasType && LDVExtensionsSetup::haveMultisampleExtension())
	{
		GLint intValues[] = {
			WGL_SAMPLE_BUFFERS_EXT, GL_TRUE,
			WGL_SAMPLES_EXT, 1,
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_ALPHA_BITS_ARB, 4,
			WGL_STENCIL_BITS_ARB, 1,
			0, 0
		};
		int numIntValues = sizeof(intValues) / sizeof(intValues[0]);

		if (currentAntialiasType)
		{
			intValues[3] = LDViewPreferences::getFSAAFactor();
		}
		else
		{
			// Remove the first four items in the array.
			memmove(intValues, intValues + 4, sizeof(intValues) -
				sizeof(intValues[0]) * 4);
			numIntValues -= 4;
		}
		pfIndex = LDVExtensionsSetup::choosePixelFormat(hdc, intValues);
		if (pfIndex < 0)
		{
			// Try with dest alpha but no stencil by clearing the last two used
			// elements in the array (there are two terminating zeros after
			// them).
			intValues[numIntValues - 4] = 0;
			intValues[numIntValues - 3] = 0;
			pfIndex = LDVExtensionsSetup::choosePixelFormat(hdc, intValues);
		}
		if (pfIndex < 0)
		{
			// Try with stencil but no dest alpha by changing the last two used
			// elements in the array back to their original values that specify
			// the stencil request.
			intValues[numIntValues - 6] = WGL_STENCIL_BITS_ARB;
			intValues[numIntValues - 5] = 1;
			pfIndex = LDVExtensionsSetup::choosePixelFormat(hdc, intValues);
		}
		if (pfIndex < 0)
		{
			// Try with no stencil OR dest alpha by clearing two more elements
			// at the end of the array.
			intValues[numIntValues - 6] = 0;
			intValues[numIntValues - 5] = 0;
			pfIndex = LDVExtensionsSetup::choosePixelFormat(hdc, intValues);
		}
		if (pfIndex >= 0)
		{
			if (setPixelFormat(pfIndex))
			{
				return TRUE;
			}
		}
	}
	currentAntialiasType = 0;
	return CUIOGLWindow::setupPFD();
}

void ModelWindow::setViewMode(LDVViewMode mode)
{
	viewMode = mode;
	TCUserDefaults::setLongForKey(viewMode, VIEW_MODE_KEY, false);
	if (modelViewer)
	{
		if (viewMode == LDVViewExamine)
		{
			modelViewer->setConstrainZoom(true);
		}
		else
		{
			modelViewer->setConstrainZoom(false);
		}
	}
}

LRESULT ModelWindow::processKeyDown(int keyCode, long /*keyData*/)
{
	if (keyCode == VK_ESCAPE)
	{
		cancelLoad = true;
	}
	if (modelViewer && viewMode == LDVViewFlythrough)
	{
		TCVector cameraMotion = modelViewer->getCameraMotion();
		float motionAmount = 1.0f;
		float rotationAmount = 0.01f;
		float strafeAmount = 1.0f;
		int i;

		if (modelViewer)
		{
			float fov = modelViewer->getFov();

			motionAmount = modelViewer->getDefaultDistance() / 400.0f;
			rotationAmount *= (float)tan(deg2rad(fov));
			strafeAmount *= (float)sqrt(fov / 45.0f);
		}
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			motionAmount *= 2.0f;
			strafeAmount *= 2.0f;
		}
		switch (keyCode)
		{
		case 'W':
			cameraMotion[2] = -motionAmount;
			break;
		case 'S':
			cameraMotion[2] = motionAmount;
			break;
		case 'A':
			cameraMotion[0] = -strafeAmount;
			break;
		case 'D':
			cameraMotion[0] = strafeAmount;
			break;
		case 'R':
			cameraMotion[1] = strafeAmount;
			break;
		case 'F':
			cameraMotion[1] = -strafeAmount;
			break;
		case 'E':
			modelViewer->setCameraZRotate(0.01f);
			break;
		case 'Q':
			modelViewer->setCameraZRotate(-0.01f);
			break;
		case VK_UP:
			modelViewer->setCameraYRotate(rotationAmount);
			break;
		case VK_DOWN:
			modelViewer->setCameraYRotate(-rotationAmount);
			break;
		case VK_LEFT:
			modelViewer->setCameraXRotate(-rotationAmount);
			break;
		case VK_RIGHT:
			modelViewer->setCameraXRotate(rotationAmount);
			break;
		case VK_SHIFT:
			for (i = 0; i < 3; i++)
			{
				if (cameraMotion[i] > 0.0f)
				{
					cameraMotion[i] = 2.0;
				}
				else if (cameraMotion[i] < 0.0f)
				{
					cameraMotion[i] = -2.0;
				}
			}
			break;
		default:
			return 1;
			break;
		}
		modelViewer->setCameraMotion(cameraMotion);
		forceRedraw(2);
		return 0;
	}
	return 1;
}

LRESULT ModelWindow::processKeyUp(int keyCode, long /*keyData*/)
{
	if (modelViewer && viewMode == LDVViewFlythrough)
	{
		TCVector cameraMotion = modelViewer->getCameraMotion();
		int i;

		switch (keyCode)
		{
		case 'W':
			cameraMotion[2] = 0.0f;
			break;
		case 'S':
			cameraMotion[2] = 0.0f;
			break;
		case 'A':
			cameraMotion[0] = 0.0f;
			break;
		case 'D':
			cameraMotion[0] = 0.0f;
			break;
		case 'R':
			cameraMotion[1] = 0.0f;
			break;
		case 'F':
			cameraMotion[1] = 0.0f;
			break;
		case 'E':
			modelViewer->setCameraZRotate(0.0f);
			break;
		case 'Q':
			modelViewer->setCameraZRotate(0.0f);
			break;
		case VK_UP:
			modelViewer->setCameraYRotate(0.0f);
			break;
		case VK_DOWN:
			modelViewer->setCameraYRotate(0.0f);
			break;
		case VK_LEFT:
			modelViewer->setCameraXRotate(0.0f);
			break;
		case VK_RIGHT:
			modelViewer->setCameraXRotate(0.0f);
			break;
		case VK_SHIFT:
			for (i = 0; i < 3; i++)
			{
				if (cameraMotion[i] > 0.0f)
				{
					cameraMotion[i] = 1.0;
				}
				else if (cameraMotion[i] < 0.0f)
				{
					cameraMotion[i] = -1.0;
				}
			}
			break;
		default:
			return 1;
			break;
		}
		modelViewer->setCameraMotion(cameraMotion);
		forceRedraw(2);
		return 0;
	}
	return 1;
}

LRESULT ModelWindow::doMove(int newX, int newY)
{
	return CUIOGLWindow::doMove(newX, newY);
}

void ModelWindow::makeCurrent(void)
{
	if (initializedGL)
	{
		if (hCurrentDC && hCurrentGLRC)
		{
			wglMakeCurrent(hCurrentDC, hCurrentGLRC);
		}
		else
		{
			CUIOGLWindow::makeCurrent();
		}
	}
}

bool ModelWindow::performHotKey(int hotKeyIndex)
{
	if (prefs)
	{
		return prefs->performHotKey(hotKeyIndex);
	}
	else
	{
		return false;
	}
}

void ModelWindow::initFail(char * /*reason*/)
{
	MessageBox(hWindow, "LDView was unable to initialize OpenGL.\n"
		"Hit OK to exit.", "Fatal Error", MB_OK | MB_ICONERROR);
	PostQuitMessage(-1);
}

void ModelWindow::drawLight(GLenum /*light*/, float /*x*/, float /*y*/,
							float /*z*/)
{
	// Don't call super.
}

void ModelWindow::drawLights(void)
{
	// Don't call super.
}

void ModelWindow::orthoView(void)
{
	// Don't call super.
}

void ModelWindow::setupLight(GLenum /*light*/)
{
	// Don't call super.
}
