// LDVLib.cpp : Defines the entry point for the DLL application.
//

#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLModel.h>
#include <LDLib/LDInputHandler.h>
//#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "LDVLib.h"

struct ViewerInfo;

class AlertHandler : public TCObject
{
public:
	AlertHandler(ViewerInfo *viewerInfo);
private:
	void captureAlertCallback(TCAlert *alert);
	void releaseAlertCallback(TCAlert *alert);
	void redrawAlertCallback(TCAlert *alert);
	void captureMouse(void);
	void releaseMouse(void);

	ViewerInfo *m_viewerInfo;
};

struct ViewerInfo
{
	LDrawModelViewer *modelViewer;
	HWND hwnd;
	HDC hdc;
	HWND hwndParent;
	HGLRC hglrc;
	int backgroundR;
	int backgroundG;
	int backgroundB;
	BOOL inputEnabled;
	AlertHandler *alertHandler;
};

static HINSTANCE g_hModule;
static int g_initCount = 0;

#define OGL_WINDOW_CLASS_NAME "LDVLibOGLWindow"
#define REDRAW_TIMER 42

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  /*ul_reason_for_call*/, 
                       LPVOID /*lpReserved*/
					 )
{
	g_hModule = (HINSTANCE)hModule;
    return TRUE;
}

static ViewerInfo *getViewerInfo(void *pLDV)
{
	return (ViewerInfo *)pLDV;
}

static void requestRedraw(HWND hWnd)
{
	::SetTimer(hWnd, REDRAW_TIMER, 0, NULL);
}

static void requestRedraw(void *pLDV)
{
	requestRedraw(getViewerInfo(pLDV)->hwnd);
}

AlertHandler::AlertHandler(ViewerInfo *viewerInfo)
	: m_viewerInfo(viewerInfo)
{
	m_viewerInfo->alertHandler = this;
	TCAlertManager::registerHandler(LDInputHandler::captureAlertClass(), this,
		(TCAlertCallback)&AlertHandler::captureAlertCallback);
	TCAlertManager::registerHandler(LDInputHandler::releaseAlertClass(), this,
		(TCAlertCallback)&AlertHandler::releaseAlertCallback);
	TCAlertManager::registerHandler(LDrawModelViewer::redrawAlertClass(), this,
		(TCAlertCallback)&AlertHandler::redrawAlertCallback);
}

void AlertHandler::captureMouse(void)
{
	SetCapture(m_viewerInfo->hwnd);
}

void AlertHandler::releaseMouse(void)
{
	ReleaseCapture();
}

void AlertHandler::captureAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_viewerInfo->modelViewer->getInputHandler())
	{
		captureMouse();
	}
}

void AlertHandler::releaseAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_viewerInfo->modelViewer->getInputHandler())
	{
		releaseMouse();
	}
}

void AlertHandler::redrawAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_viewerInfo->modelViewer)
	{
		requestRedraw(m_viewerInfo->hwnd);
	}
}

static LDrawModelViewer *getModelViewer(void *pLDV)
{
	return getViewerInfo(pLDV)->modelViewer;
}

static BOOL makeCurrent(void *pLDV)
{
	ViewerInfo *viewerInfo = getViewerInfo(pLDV);
	return wglMakeCurrent(viewerInfo->hdc, viewerInfo->hglrc);
}

static LRESULT doPaint(ViewerInfo *viewerInfo, HWND hWnd)
{
	if (viewerInfo != NULL && viewerInfo->modelViewer != NULL)
	{
		PAINTSTRUCT paintStruct;

		memset(&paintStruct, 0, sizeof(paintStruct));
		if (::BeginPaint(hWnd, &paintStruct))
		{
			makeCurrent(viewerInfo);
			viewerInfo->modelViewer->update();
			SwapBuffers(viewerInfo->hdc);
			::EndPaint(hWnd, &paintStruct);
		}
		return 0;
	}
	return DefWindowProc(hWnd, WM_PAINT, 0, 0);
}

static LRESULT doEraseBackground(ViewerInfo *viewerInfo, HWND hWnd, HDC hdc)
{
	int r = 0;
	int g = 0;
	int b = 0;

	if (viewerInfo != NULL)
	{
		r = viewerInfo->backgroundR;
		g = viewerInfo->backgroundG;
		b = viewerInfo->backgroundB;
	}
	HBRUSH hBgBrush = ::CreateSolidBrush(RGB(r, g, b));
	RECT updateRect;
	::GetUpdateRect(hWnd, &updateRect, FALSE);
	::FillRect(hdc, &updateRect, hBgBrush);
	::DeleteObject(hBgBrush);
	return 1;
}

static void redraw(void *pLDV)
{
	::RedrawWindow(getViewerInfo(pLDV)->hwnd, NULL, NULL,
		RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
}

static LRESULT doTimer(ViewerInfo *viewerInfo, HWND hWnd, WPARAM timerID)
{
	if (timerID == REDRAW_TIMER)
	{
		::KillTimer(hWnd, timerID);
		redraw(viewerInfo);
	}
	return 0;
}

static TCULong convertKeyModifiers(WPARAM osModifiers)
{
	TCULong retValue = 0;

	if (osModifiers & MK_SHIFT)
	{
		retValue |= LDInputHandler::MKShift;
	}
	if (osModifiers & MK_CONTROL)
	{
		retValue |= LDInputHandler::MKControl;
	}
	return retValue;
}

static LRESULT doButtonDown(
	ViewerInfo *viewerInfo,
	HWND hWnd,
	LDInputHandler::MouseButton button,
	WPARAM keyFlags,
	int xPos,
	int yPos)
{
	if (viewerInfo->inputEnabled &&
		viewerInfo->modelViewer->getInputHandler()->mouseDown(
		convertKeyModifiers(keyFlags), button, xPos, yPos))
	{
		requestRedraw(hWnd);
		::SetFocus(hWnd);
		return 0;
	}
	return 1;
}

static LRESULT doButtonUp(
	ViewerInfo *viewerInfo,
	HWND hWnd,
	LDInputHandler::MouseButton button,
	WPARAM keyFlags,
	int xPos,
	int yPos)
{
	if (viewerInfo->inputEnabled &&
		viewerInfo->modelViewer->getInputHandler()->mouseUp(
		convertKeyModifiers(keyFlags), button, xPos, yPos))
	{
		requestRedraw(hWnd);
		return 0;
	}
	return 1;
}

static LRESULT doMouseMove(
	ViewerInfo *viewerInfo,
	HWND hWnd,
	WPARAM keyFlags,
	int xPos,
	int yPos)
{
	if (viewerInfo->inputEnabled &&
		viewerInfo->modelViewer->getInputHandler()->mouseMove(
		convertKeyModifiers(keyFlags), xPos, yPos))
	{
		requestRedraw(hWnd);
		return 0;
	}
	return 1;
}

static LRESULT doMouseWheel(
	ViewerInfo *viewerInfo,
	HWND /*hWnd*/,
	short keyFlags,
	short zDelta,
	int /*xPos*/,
	int /*yPos*/)
{
	if (viewerInfo->inputEnabled)
	{
		viewerInfo->modelViewer->getInputHandler()->mouseWheel(
			convertKeyModifiers(keyFlags), (TCFloat)zDelta);
	}
	return 0;
}

static LRESULT doSetFocus(
	ViewerInfo * /*viewerInfo*/,
	HWND /*hWnd*/,
	HWND /*hPrev*/)
{
	return 0;
}


static LRESULT CALLBACK staticWindowProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
#pragma warning(push)
#pragma warning(disable:4312)
	ViewerInfo *viewerInfo =
		(ViewerInfo *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
#pragma warning(pop)

	switch (message)
	{
	case WM_CREATE:
	case WM_NCCREATE:
		{
			if (viewerInfo == NULL)
			{
				CREATESTRUCT *createStruct = (CREATESTRUCT *)lParam;

				viewerInfo = (ViewerInfo *)createStruct->lpCreateParams;
				if (viewerInfo != NULL)
				{
#pragma warning(push)
#pragma warning(disable:4244)
					SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)viewerInfo);
#pragma warning(pop)
					viewerInfo->hdc = ::GetDC(hWnd);
				}
			}
		}
		break;
	case WM_PAINT:
		return doPaint(viewerInfo, hWnd);
	case WM_TIMER:
		if (lParam == 0)
		{
			return doTimer(viewerInfo, hWnd, wParam);
		}
		break;
	case WM_ERASEBKGND:
		return doEraseBackground(viewerInfo, hWnd, (HDC)wParam);
	case WM_LBUTTONDOWN:
		return doButtonDown(viewerInfo, hWnd, LDInputHandler::MBLeft, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_LBUTTONUP:
		return doButtonUp(viewerInfo, hWnd, LDInputHandler::MBLeft, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_RBUTTONDOWN:
		return doButtonDown(viewerInfo, hWnd, LDInputHandler::MBRight, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_RBUTTONUP:
		return doButtonUp(viewerInfo, hWnd, LDInputHandler::MBRight, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_MBUTTONDOWN:
		return doButtonDown(viewerInfo, hWnd, LDInputHandler::MBMiddle, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_MBUTTONUP:
		return doButtonUp(viewerInfo, hWnd, LDInputHandler::MBMiddle, wParam,
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_MOUSEMOVE:
		return doMouseMove(viewerInfo, hWnd, wParam, (short)LOWORD(lParam),
			(short)HIWORD(lParam));
	case WM_MOUSEWHEEL:
		return doMouseWheel(viewerInfo, hWnd, LOWORD(wParam), HIWORD(wParam),
			(short)LOWORD(lParam), (short)HIWORD(lParam));
	case WM_SETFOCUS:
		return doSetFocus(viewerInfo, hWnd, (HWND)wParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

static void registerClass(void)
{
	WNDCLASSEX windowClass;

	memset(&windowClass, 0, sizeof(windowClass));
	// Set up and window class
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	windowClass.lpfnWndProc = staticWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = g_hModule;
	windowClass.hIcon = NULL;
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = OGL_WINDOW_CLASS_NAME;
	windowClass.hIconSm = NULL;
	::RegisterClassEx(&windowClass);
}

void *LDVInit(HWND hwnd)
{
	ViewerInfo *viewerInfo = new ViewerInfo;
	RECT clientRect;

	//::MessageBox(NULL, "Attach Debugger", "DEBUG LDVLib.dll", MB_OK);
	if (++g_initCount == 1)
	{
		registerClass();
	}
	memset(viewerInfo, 0, sizeof(*viewerInfo));
	viewerInfo->inputEnabled = TRUE;
	viewerInfo->hwndParent = hwnd;
	::GetClientRect(hwnd, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	viewerInfo->hwnd = ::CreateWindowEx(
		0, OGL_WINDOW_CLASS_NAME, "LDVLib Window",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, width, height, hwnd, NULL, g_hModule, viewerInfo);
	viewerInfo->modelViewer = new LDrawModelViewer(width, height);
	new AlertHandler(viewerInfo);
	return viewerInfo;
}

static void GLDeInit(ViewerInfo *viewerInfo)
{
	if (viewerInfo->hglrc != NULL)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(viewerInfo->hglrc);
		viewerInfo->hglrc = NULL;
	}
}

static BOOL setPixelFormat(HDC hdc, int pfIndex)
{
	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	if (SetPixelFormat(hdc, pfIndex, &pfd))
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL setupPFD(ViewerInfo *viewerInfo)
{
	PIXELFORMATDESCRIPTOR pfd;
	BOOL retValue = FALSE;

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER |
		PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 12;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 1;
	pfd.cAlphaBits = 4;
	int pfIndex = ChoosePixelFormat(viewerInfo->hdc, &pfd);
	if (pfIndex == 0)
	{ // Try with dest alpha but no stencil
		pfd.cStencilBits = 0;
		pfIndex = ChoosePixelFormat(viewerInfo->hdc, &pfd);
	}
	if (pfIndex == 0)
	{ // Try with stencil but no dest alpha
		pfd.cStencilBits = 1;
		pfd.cAlphaBits = 0;
		pfIndex = ChoosePixelFormat(viewerInfo->hdc, &pfd);
	}
	if (pfIndex == 0)
	{ // Try with no stencil OR dest alpha
		pfd.cStencilBits = 0;
		pfIndex = ChoosePixelFormat(viewerInfo->hdc, &pfd);
	}
	if (pfIndex != 0)
	{
		retValue = setPixelFormat(viewerInfo->hdc, pfIndex);
	}
	return retValue;
}

BOOL LDVGLInit(void *pLDV)
{
	BOOL retValue = FALSE;
	ViewerInfo *viewerInfo = getViewerInfo(pLDV);
	GLDeInit(viewerInfo);
	if (setupPFD(viewerInfo))
	{
		viewerInfo->hglrc = wglCreateContext(viewerInfo->hdc);
		if (viewerInfo->hglrc != NULL)
		{
			retValue = wglMakeCurrent(viewerInfo->hdc, viewerInfo->hglrc);
			if (retValue)
			{
				glDrawBuffer(GL_BACK);
			}
		}
	}
	return retValue;
}

void LDVDeInit(void *pLDV)
{
	GLDeInit(getViewerInfo(pLDV));
	TCObject::release(getViewerInfo(pLDV)->alertHandler);
	TCObject::release(getModelViewer(pLDV));
	if (--g_initCount == 0)
	{
		::UnregisterClass(OGL_WINDOW_CLASS_NAME, g_hModule);
	}
}

void LDVSetSize(void *pLDV, int width, int height)
{
	ViewerInfo *viewerInfo = getViewerInfo(pLDV);

	::MoveWindow(viewerInfo->hwnd, 0, 0, width, height, FALSE);
	makeCurrent(pLDV);
	glViewport(0, 0, width, height);
	viewerInfo->modelViewer->setWidth(width);
	viewerInfo->modelViewer->setHeight(height);
	redraw(pLDV);
}

int LDVGetWidth(void *pLDV)
{
	return getModelViewer(pLDV)->getWidth();
}

void LDVSetHeight(void *pLDV, int height)
{
	getModelViewer(pLDV)->setHeight(height);
}

int LDVGetHeight(void *pLDV)
{
	return getModelViewer(pLDV)->getHeight();
}

void LDVSetFilename(void *pLDV, const char *filename)
{
	getModelViewer(pLDV)->setFilename(filename);
}

BOOL LDVLoadModel(void *pLDV, BOOL resetViewpoint)
{
	if (makeCurrent(pLDV))
	{
		if (getModelViewer(pLDV)->loadModel(resetViewpoint ? true : false))
		{
			redraw(pLDV);
			return TRUE;
		}
	}
	return FALSE;
}

void LDVUpdate(void *pLDV)
{
	requestRedraw(pLDV);
}

void LDVSetBackgroundRGB(void *pLDV, int r, int g, int b)
{
	ViewerInfo *viewerInfo = getViewerInfo(pLDV);

	viewerInfo->backgroundR = r;
	viewerInfo->backgroundG = g;
	viewerInfo->backgroundB = b;
	getModelViewer(pLDV)->setBackgroundRGB(r, g, b);
	requestRedraw(pLDV);
}

void LDVSetLDrawDir(const char *path)
{
	LDLModel::setLDrawDir(path);
}

void LDVResetView(void *pLDV, int viewingAngle)
{
	getModelViewer(pLDV)->resetView((LDVAngle)viewingAngle);
	requestRedraw(pLDV);
}

void LDVEnableInput(void *pLDV, BOOL enable)
{
	getViewerInfo(pLDV)->inputEnabled = enable;
}

BOOL LDVIsInputEnabled(void *pLDV)
{
	return getViewerInfo(pLDV)->inputEnabled;
}

void LDVZoomToFit(void *pLDV)
{
	getModelViewer(pLDV)->zoomToFit();
	requestRedraw(pLDV);
}

LDVExport BOOL LDVHasFocus(void *pLDV)
{
	return getViewerInfo(pLDV)->hwnd == ::GetFocus();
}

LDVExport BOOL LDVGetUsesSpecular(void *pLDV)
{
	return getModelViewer(pLDV)->getUsesSpecular() != false;
}

LDVExport void LDVSetUsesSpecular(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setUsesSpecular(value ? true : false);
	requestRedraw(pLDV);
}

LDVExport float LDVGetSeamWidth(void *pLDV)
{
	return getModelViewer(pLDV)->getSeamWidth();
}

LDVExport void LDVSetSeamWidth(void *pLDV, float value)
{
	getModelViewer(pLDV)->setSeamWidth(value);
	requestRedraw(pLDV);
}

LDVExport BOOL LDVGetShowsEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getShowsHighlightLines() != false;
}

LDVExport void LDVSetShowsEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setShowsHighlightLines(value ? true : false);
	requestRedraw(pLDV);
}

LDVExport BOOL LDVGetShowsConditionalEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getDrawConditionalHighlights() != false;
}

LDVExport void LDVSetShowsConditionalEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setDrawConditionalHighlights(value ? true : false);
	requestRedraw(pLDV);
}

LDVExport BOOL LDVGetLowQualityStuds(void *pLDV)
{
	return getModelViewer(pLDV)->getQualityStuds() == false;
}

LDVExport void LDVSetLowQualityStuds(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setQualityStuds(!value ? true : false);
	requestRedraw(pLDV);
}

LDVExport BOOL LDVGetSubduedLighting(void *pLDV)
{
	return getModelViewer(pLDV)->getSubduedLighting() != false;
}

LDVExport void LDVSetSubduedLighting(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setSubduedLighting(value ? true : false);
	requestRedraw(pLDV);
}
