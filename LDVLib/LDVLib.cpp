// LDVLib.cpp : Defines the entry point for the DLL application.
//

#include <LDLib/LDrawModelViewer.h>
#include <LDLoader/LDLModel.h>
#include <LDLib/LDInputHandler.h>
//#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TRE/TREMainModel.h>
#include <TRE/TREGLExtensions.h>
#include "LDVLib.h"
#include "resource.h"

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
ViewerInfo *g_sharedViewerInfo;

#define OGL_WINDOW_CLASS_NAME "LDVLibOGLWindow"
#define REDRAW_TIMER 42

#define waitForDebugger() \
	::MessageBox(NULL, "Attach Debugger", "DEBUG LDVLib.dll", MB_OK);

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
	HRSRC hFontResource = FindResource(g_hModule,
		MAKEINTRESOURCE(IDR_SANS_FONT), RT_RCDATA);

	if (++g_initCount == 1)
	{
		HRSRC hStudLogoResource = FindResource(g_hModule,
			MAKEINTRESOURCE(IDR_STUDLOGO_PNG), RT_RCDATA);

		if (hStudLogoResource)
		{
			HGLOBAL hStudLogo = LoadResource(g_hModule, hStudLogoResource);

			if (hStudLogo)
			{
				TCByte *data = (TCByte *)LockResource(hStudLogo);

				if (data)
				{
					DWORD length = SizeofResource(g_hModule, hStudLogoResource);

					if (length)
					{
						TREMainModel::setStudTextureData(data, length);
					}
				}
			}
		}
		registerClass();
		g_sharedViewerInfo = new ViewerInfo;
		memset(g_sharedViewerInfo, 0, sizeof(*g_sharedViewerInfo));
		g_sharedViewerInfo->hwndParent = NULL;
		g_sharedViewerInfo->hwnd = ::CreateWindowEx(
			0, OGL_WINDOW_CLASS_NAME, "LDVLib Window",
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0, 320, 240, hwnd, NULL, g_hModule, g_sharedViewerInfo);
		LDVGLInit(g_sharedViewerInfo);
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
	if (hFontResource)
	{
		HGLOBAL hFont = LoadResource(g_hModule, hFontResource);

		if (hFont)
		{
			TCByte *data = (TCByte *)LockResource(hFont);

			if (data)
			{
				DWORD length = SizeofResource(g_hModule, hFontResource);

				if (length)
				{
					viewerInfo->modelViewer->setFontData(data, length);
				}
			}
		}
	}
	new AlertHandler(viewerInfo);
	return viewerInfo;
}

void SetRegistryAppName(const char *appName)
{
	TCUserDefaults::setAppName(appName);
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
				if (pLDV == g_sharedViewerInfo)
				{
					TREGLExtensions::setup();
				}
				else
				{
					wglShareLists(g_sharedViewerInfo->hglrc, viewerInfo->hglrc);
					glDrawBuffer(GL_BACK);
				}
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


static bool chDirFromFilename(const char* filename, char* outFilename)
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

BOOL LDVLoadModel(void *pLDV, BOOL resetViewpoint)
{
	BOOL retValue = FALSE;
	if (makeCurrent(pLDV))
	{
		LDrawModelViewer *modelViewer = getModelViewer(pLDV);
		char origDir[1024];
		char fullPath[1024];
		bool dirChanged;

		GetCurrentDirectory(sizeof(origDir), origDir);
		dirChanged = chDirFromFilename(modelViewer->getFilename(),
			fullPath);
		if (dirChanged)
		{
			modelViewer->setFilename(fullPath);
		}
		if (modelViewer->loadModel(resetViewpoint ? true : false))
		{
			redraw(pLDV);
			retValue = TRUE;
		}
		if (dirChanged)
		{
			SetCurrentDirectory(origDir);
		}
	}
	return retValue;
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

void LDVGetBackgroundRGB(void *pLDV, int *r, int *g, int *b)
{
	ViewerInfo *viewerInfo = getViewerInfo(pLDV);

	*r = viewerInfo->backgroundR;
	*g = viewerInfo->backgroundG;
	*b = viewerInfo->backgroundB;
}

void LDVSetDefaultRGB(void *pLDV, int r, int g, int b, BOOL transparent)
{
	getModelViewer(pLDV)->setDefaultRGB(r, g, b, transparent ? true : false);
	requestRedraw(pLDV);
}

void LDVGetDefaultRGB(void *pLDV, int *r, int *g, int *b, BOOL *transparent)
{
	TCByte rTemp, gTemp, bTemp;
	bool transTemp;

	getModelViewer(pLDV)->getDefaultRGB(rTemp, gTemp, bTemp, transTemp);
	*r = rTemp;
	*g = gTemp;
	*b = bTemp;
	*transparent = transTemp ? TRUE : FALSE;
}

void LDVSetLDrawDir(const char *path)
{
	LDLModel::setLDrawDir(path);
}

void LDVResetView(void *pLDV, LDVViewingAngle viewingAngle)
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

BOOL LDVHasFocus(void *pLDV)
{
	return getViewerInfo(pLDV)->hwnd == ::GetFocus();
}

BOOL LDVGetUsesSpecular(void *pLDV)
{
	return getModelViewer(pLDV)->getUsesSpecular() != false;
}

void LDVSetUsesSpecular(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setUsesSpecular(value ? true : false);
	requestRedraw(pLDV);
}

float LDVGetSeamWidth(void *pLDV)
{
	return getModelViewer(pLDV)->getSeamWidth();
}

void LDVSetSeamWidth(void *pLDV, float value)
{
	getModelViewer(pLDV)->setSeamWidth(value);
	requestRedraw(pLDV);
}

BOOL LDVGetShowsEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getShowsHighlightLines() != false;
}

void LDVSetShowsEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setShowsHighlightLines(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetBlackEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getBlackHighlights() != false;
}

void LDVSetBlackEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setBlackHighlights(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetShowsConditionalEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getDrawConditionalHighlights() != false;
}

void LDVSetShowsConditionalEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setDrawConditionalHighlights(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetLowQualityStuds(void *pLDV)
{
	return getModelViewer(pLDV)->getQualityStuds() == false;
}

void LDVSetLowQualityStuds(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setQualityStuds(!value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetLighting(void *pLDV)
{
	return getModelViewer(pLDV)->getUseLighting() != false;
}

void LDVSetLighting(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setUseLighting(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetSubduedLighting(void *pLDV)
{
	return getModelViewer(pLDV)->getSubduedLighting() != false;
}

void LDVSetSubduedLighting(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setSubduedLighting(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetTextureStuds(void *pLDV)
{
	return getModelViewer(pLDV)->getTextureStuds() != false;
}

void LDVSetTextureStuds(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setTextureStuds(value ? true : false);
	requestRedraw(pLDV);
}

LDVTextureFilterType LDVGetTextureFilterType(void *pLDV)
{
	switch (getModelViewer(pLDV)->getTextureFilterType())
	{
	case GL_NEAREST_MIPMAP_NEAREST:
		return LDVTFTNearest;
	case GL_LINEAR_MIPMAP_NEAREST:
		return LDVTFTBilinear;
	case GL_LINEAR_MIPMAP_LINEAR:
		return LDVTFTTrilinear;
	default:
		return LDVTFTBilinear;
	}
}

void LDVSetTextureFilterType(void *pLDV, LDVTextureFilterType value)
{
	int textureFilterType;

	switch (value)
	{
	case LDVTFTNearest:
		textureFilterType = GL_NEAREST_MIPMAP_NEAREST;
		break;
	case LDVTFTTrilinear:
		textureFilterType = GL_LINEAR_MIPMAP_LINEAR;
		break;
	case LDVTFTBilinear:
	default:
		textureFilterType = GL_LINEAR_MIPMAP_NEAREST;
		break;
	}
	getModelViewer(pLDV)->setTextureFilterType(textureFilterType);
	requestRedraw(pLDV);
}

float LDVGetAnisoLevel(void *pLDV)
{
	LDrawModelViewer *modelViewer = getModelViewer(pLDV);

	if (modelViewer->getTextureFilterType() == GL_LINEAR_MIPMAP_LINEAR)
	{
		return modelViewer->getAnisoLevel();
	}
	else
	{
		return 1.0;
	}
}

void LDVSetAnisoLevel(void *pLDV, float value)
{
	LDrawModelViewer *modelViewer = getModelViewer(pLDV);

	if (modelViewer->getTextureFilterType() != GL_LINEAR_MIPMAP_LINEAR)
	{
		value = 1.0;
	}
	modelViewer->setAnisoLevel(value);
	requestRedraw(pLDV);
}

float LDVGetMaxAnisoLevel(void)
{
	return TREGLExtensions::getMaxAnisoLevel();
}

BOOL LDVGetDrawWireframe(void *pLDV)
{
	return getModelViewer(pLDV)->getDrawWireframe() != false;
}

void LDVSetDrawWireframe(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setDrawWireframe(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetWireframeFog(void *pLDV)
{
	return getModelViewer(pLDV)->getUseWireframeFog() != false;
}

void LDVSetWireframeFog(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setUseWireframeFog(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetRemoveHiddenLines(void *pLDV)
{
	return getModelViewer(pLDV)->getRemoveHiddenLines() != false;
}

void LDVSetRemoveHiddenLines(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setRemoveHiddenLines(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetPrimitiveSubstitution(void *pLDV)
{
	return getModelViewer(pLDV)->getAllowPrimitiveSubstitution() != false;
}

void LDVSetPrimitiveSubstitution(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setAllowPrimitiveSubstitution(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetSmoothing(void *pLDV)
{
	return getModelViewer(pLDV)->getPerformSmoothing() != false;
}

void LDVSetSmoothing(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setPerformSmoothing(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetLineSmoothing(void *pLDV)
{
	return getModelViewer(pLDV)->getLineSmoothing() != false;
}

void LDVSetLineSmoothing(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setLineSmoothing(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetEdgesOnly(void *pLDV)
{
	return getModelViewer(pLDV)->getEdgesOnly() != false;
}

void LDVSetEdgesOnly(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setEdgesOnly(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetBFC(void *pLDV)
{
	return getModelViewer(pLDV)->getBfc() != false;
}

void LDVSetBFC(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setBfc(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetRedBackFaces(void *pLDV)
{
	return getModelViewer(pLDV)->getRedBackFaces() != false;
}

void LDVSetRedBackFaces(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setRedBackFaces(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetGreenFrontFaces(void *pLDV)
{
	return getModelViewer(pLDV)->getGreenFrontFaces() != false;
}

void LDVSetGreenFrontFaces(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setGreenFrontFaces(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetBlueNeutralFaces(void *pLDV)
{
	return getModelViewer(pLDV)->getBlueNeutralFaces() != false;
}

void LDVSetBlueNeutralFaces(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setBlueNeutralFaces(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetCheckPartsTracker(void *pLDV)
{
	return getModelViewer(pLDV)->getCheckPartTracker() != false;
}

void LDVSetCheckPartsTracker(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setCheckPartTracker(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetShowsAxes(void *pLDV)
{
	return getModelViewer(pLDV)->getShowAxes() != false;
}

void LDVSetShowsAxes(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setShowAxes(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetShowsBoundingBox(void *pLDV)
{
	return getModelViewer(pLDV)->getShowBoundingBox() != false;
}

void LDVSetShowsBoundingBox(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setShowBoundingBox(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetBoundingBoxesOnly(void *pLDV)
{
	return getModelViewer(pLDV)->getBoundingBoxesOnly() != false;
}

void LDVSetBoundingBoxesOnly(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setBoundingBoxesOnly(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetOBI(void *pLDV)
{
	return getModelViewer(pLDV)->getObi() != false;
}

void LDVSetOBI(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setObi(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetRandomColors(void *pLDV)
{
	return getModelViewer(pLDV)->getRandomColors() != false;
}

void LDVSetRandomColors(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setRandomColors(value ? true : false);
	requestRedraw(pLDV);
}

BOOL LDVGetTexmaps(void *pLDV)
{
	return getModelViewer(pLDV)->getTexmaps() != false;
}

void LDVSetTexmaps(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setTexmaps(value ? true : false);
	requestRedraw(pLDV);
}

float LDVGetFOV(void *pLDV)
{
	return getModelViewer(pLDV)->getFov();
}

void LDVSetFOV(void *pLDV, float value)
{
	getModelViewer(pLDV)->setFov(value);
	requestRedraw(pLDV);
}

BOOL LDVGetShowsAllConditionalEdges(void *pLDV)
{
	return getModelViewer(pLDV)->getShowAllConditionalLines() != false;
}

void LDVSetShowsAllConditionalEdges(void *pLDV, BOOL value)
{
	getModelViewer(pLDV)->setShowAllConditionalLines(value ? true : false);
	requestRedraw(pLDV);
}
