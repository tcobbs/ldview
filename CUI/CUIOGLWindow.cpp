#include "CUIOGLWindow.h"

#include <GL/glu.h>
#include <TCFoundation/mystring.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

CUIOGLWindow::CUIOGLWindow(void)
			 :CUIWindow()
{
	init();
}

CUIOGLWindow::CUIOGLWindow(CUCSTR windowTitle, HINSTANCE hInstance, int x,
						   int y, int width, int height)
			 :initializedGL(FALSE),
			  hasParent(FALSE),
			  CUIWindow(windowTitle, hInstance, x, y, width, height)
{
	init();
}

CUIOGLWindow::CUIOGLWindow(CUIWindow* parentWindow, int x, int y, int width,
						   int height)
			 :initializedGL(FALSE),
			  hasParent(TRUE),
			  CUIWindow(parentWindow, x, y, width, height)
{
	init();
}

CUIOGLWindow::~CUIOGLWindow(void)
{
}

void CUIOGLWindow::dealloc(void)
{
	CUIWindow::dealloc();
}

void CUIOGLWindow::destroyWindow(void)
{
	if (initializedGL && hglrc)
	{
		wglMakeCurrent(NULL, NULL);
		if (!wglDeleteContext(hglrc))
		{
			debugPrintf("wglDeleteContext failed!!!\n");
		}
	}
	hglrc = NULL;
	initializedGL = FALSE;
}

void CUIOGLWindow::init(void)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "CUIOGLWindow");
#endif
	windowStyle = windowStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	windowClassStyle |= CS_OWNDC;
}

BOOL CUIOGLWindow::setPixelFormat(int pfIndex)
{
	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
//	if (DescribePixelFormat(hdc, pfIndex, sizeof(PIXELFORMATDESCRIPTOR),
//		&pfd))
	{
		if (SetPixelFormat(hdc, pfIndex, &pfd))
		{
			return TRUE;
		}
	}
	return FALSE;
}

const char *CUIOGLWindow::interpretGLError(GLenum errorCode)
{
	char* errorString;

	switch (errorCode)
	{
	case GL_NO_ERROR:
		errorString = "GL_NO_ERROR";
		break;
	case GL_INVALID_ENUM:
		errorString = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		errorString = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		errorString = "GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		errorString = "GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		errorString = "GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		errorString = "GL_OUT_OF_MEMORY";
		break;
	default:
		errorString = "Unknown Error";
		break;
	}
	return errorString;
}

BOOL CUIOGLWindow::setupPFD(void)
{
	PIXELFORMATDESCRIPTOR pfd;

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
	pfIndex = ChoosePixelFormat(hdc, &pfd);
	if (pfIndex == 0)
	{ // Try with dest alpha but no stencil
		pfd.cStencilBits = 0;
		pfIndex = ChoosePixelFormat(hdc, &pfd);
	}
	if (pfIndex == 0)
	{ // Try with stencil but no dest alpha
		pfd.cStencilBits = 1;
		pfd.cAlphaBits = 0;
		pfIndex = ChoosePixelFormat(hdc, &pfd);
	}
	if (pfIndex == 0)
	{ // Try with no stencil OR dest alpha
		pfd.cStencilBits = 0;
		pfIndex = ChoosePixelFormat(hdc, &pfd);
	}
	if (pfIndex != 0)
	{
		return setPixelFormat(pfIndex);
	}
	return FALSE;
}

BOOL CUIOGLWindow::setupRenderingContext(void)
{
	BOOL rval = FALSE;

	if (setupPFD())
	{
		if ((hglrc = wglCreateContext(hdc)) != NULL)
		{
			rval = wglMakeCurrent(hdc, hglrc);
		}
	}
	return rval;
}

void CUIOGLWindow::drawLight(GLenum light, TCFloat x, TCFloat y, TCFloat z)
{
	GLfloat position[4];
	GLfloat direction[4];
//	float fullIntensity[] = {1.0f, 1.0f, 1.0f, 1.0f};

	position[0] = (GLfloat)x;
	position[1] = (GLfloat)y;
	position[2] = (GLfloat)z;
	position[3] = 0.0f;
	direction[0] = (GLfloat)-x;
	direction[1] = (GLfloat)-y;
	direction[2] = (GLfloat)-z;
	direction[3] = 0.0f;
	glLightfv(light, GL_POSITION, position);
	glLightfv(light, GL_SPOT_DIRECTION, direction);
//	glLightfv(light, GL_DIFFUSE, fullIntensity);
//	glLightfv(light, GL_SPECULAR, fullIntensity);
}

void CUIOGLWindow::drawLights(void)
{
//	drawLight(GL_LIGHT0, 0.0f, 0.0f, 10000.0f);
	drawLight(GL_LIGHT1, 0.0f, 0.0f, -10000.0f);
}

void CUIOGLWindow::perspectiveView(void)
{
	setFieldOfView((float)45.0, (float)2.0, (float)2002.0);
	glLoadIdentity();
}

void CUIOGLWindow::orthoView(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
//	gluOrtho2D(-0.5, width - 0.5, -0.5, height - 0.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// The following is supposed to consistently fix pixel alignment.
	glTranslatef(0.375f, 0.375f, 0.0f);
}

void CUIOGLWindow::setupMaterial(void)
{
	float mAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
//	float mSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float mSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
//	float mSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float mEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mEmission);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void CUIOGLWindow::setupLight(GLenum light)
{
//	float lAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float lAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float lDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
//	float lSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float lSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(light, GL_AMBIENT, lAmbient);
	glLightfv(light, GL_SPECULAR, lSpecular);
	glLightfv(light, GL_DIFFUSE, lDiffuse);
	glEnable(light);
}

void CUIOGLWindow::setupLighting(void)
{
	setupLight(GL_LIGHT0);
//	setupLight(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
//	glShadeModel(GL_FLAT);
}

void CUIOGLWindow::sizeView(void)
{
	glViewport(0, 0, width, height);
	perspectiveView();
}

void CUIOGLWindow::initGL(void)
{
	makeCurrent();
	setupMaterial();
	setupLighting();
	sizeView();
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_BACK);
}

void CUIOGLWindow::setFieldOfView(double fov, float nClip, float fClip)
{
	fieldOfView = fov;
	nearClipPlane = nClip;
	farClipPlane = fClip;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, (double)width / height, nClip, fClip);
	glMatrixMode(GL_MODELVIEW);
}

void CUIOGLWindow::makeCurrent(void)
{
	if (initializedGL)
	{
		if (wglGetCurrentContext() != hglrc)
		{
			wglMakeCurrent(hdc, hglrc);
		}
	}
	else
	{
//		MessageBeep(MB_ICONEXCLAMATION);
	}
}

BOOL CUIOGLWindow::initWindow(void)
{
	if (CUIWindow::initWindow())
	{
		if (setupRenderingContext())
		{
			initializedGL = TRUE;
			initGL();
			return TRUE;
		}
		else
		{
			initFail("Could not set up rendering context.");
		}
	}
	return FALSE;
}

LRESULT CUIOGLWindow::doCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	CUIWindow::doCreate(hWnd, lpcs);
	return 0;
}

LRESULT CUIOGLWindow::doNCDestroy(void)
{
	if (!hasParent)
	{
		destroyWindow();
	}
	return CUIWindow::doNCDestroy();
}

LRESULT CUIOGLWindow::doDestroy(void)
{
	if (hasParent)
	{
		destroyWindow();
	}
	return CUIWindow::doDestroy();
}

LRESULT CUIOGLWindow::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	CUIWindow::doSize(sizeType, newWidth, newHeight);
	if (initializedGL)
	{
		makeCurrent();
		if (sizeType != SIZE_MINIMIZED)
		{
			sizeView();
			return 0;
		}
	}
	return 1;
}

void CUIOGLWindow::initFail(char* /*reason*/)
{
/*
	char msg[1024] = "OpenGL init FAILED";

	strcat(msg, ": ");
	strcat(msg, reason);
	MessageBox(hWindow, msg, "Error", MB_OK);
*/
}

