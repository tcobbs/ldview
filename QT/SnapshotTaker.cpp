#include <QtGlobal>
#include "SnapshotTaker.h"
#include "SnapshotAlertHandler.h"
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCAlert.h>
#include <LDLib/LDSnapshotTaker.h>
#include <TRE/TREGLExtensions.h>
#include <TRE/TREGL.h>
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
#include <QtOpenGL>
#else
#include <GL/glx.h>

static Display *display = NULL;
static GLXContext context = NULL;
static GLXPbuffer pbuffer = 0;
#endif

SnapshotTaker::SnapshotTaker()
	: ldSnapshotTaker(NULL)
	, snapshotAlertHandler(new SnapshotAlertHandler(this))
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
	, qSurf(NULL)
	, qOglCtx(NULL)
	, qFbo(NULL)
#endif
{
}

SnapshotTaker::~SnapshotTaker(void)
{
}

void SnapshotTaker::dealloc(void)
{
	TCObject::release(snapshotAlertHandler);
	TCObject::release(ldSnapshotTaker);
	cleanupContext();
	TCObject::dealloc();
}

void SnapshotTaker::cleanupContext(void)
{
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
	if (qFbo != NULL)
	{
		delete qFbo;
		qFbo = NULL;
	}
	if (qOglCtx != NULL)
	{
		delete qOglCtx;
		qOglCtx = NULL;
	}
	if (qSurf != NULL)
	{
		if (qSurf->isValid())
		{
			qSurf->destroy();
		}
		delete qSurf;
		qSurf = NULL;
	}
#else
	if (pbuffer != 0)
	{
		glXDestroyPbuffer(display, pbuffer);
		pbuffer = 0;
	}
	if (context != NULL)
	{
		glXDestroyContext(display, context);
		context = NULL;
	}
	if (display != NULL)
	{
		XCloseDisplay(display);
		display = NULL;
	}
#endif
}

bool SnapshotTaker::getUseFBO()
{
	return ldSnapshotTaker != NULL && ldSnapshotTaker->getUseFBO();
}

bool SnapshotTaker::doCommandLine()
{
#if (QT_VERSION < 0x50100) || !defined(QOFFSCREEN)
	if (display == NULL)
	{
		display = XOpenDisplay(NULL);
		if (display == NULL)
		{
			return false;
		}
	}
#endif
	return LDSnapshotTaker::doCommandLine(true, true);
}

void SnapshotTaker::snapshotCallback(TCAlert *alert)
{
	if (getUseFBO())
	{
		return;
	}
	if (strcmp(alert->getMessage(), "PreFbo") == 0)
	{
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
		QSurfaceFormat surfaceFormat;
		surfaceFormat.setVersion(1, 5);
		surfaceFormat.setRedBufferSize(8);
		surfaceFormat.setGreenBufferSize(8);
		surfaceFormat.setBlueBufferSize(8);
		surfaceFormat.setAlphaBufferSize(8);
		surfaceFormat.setDepthBufferSize(24);
		surfaceFormat.setStencilBufferSize(8);
		surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
		surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
		qSurf = new QOffscreenSurface;
		qSurf->setFormat(surfaceFormat);
		qSurf->create();
		if (!qSurf->isValid())
		{
			cleanupContext();
			return;
		}
		qOglCtx = new QOpenGLContext(qSurf);
		qOglCtx->setFormat(surfaceFormat);
		qOglCtx->create();
		if (!qOglCtx->isValid())
		{
			cleanupContext();
			return;
		}
		qOglCtx->makeCurrent(qSurf);
		qFbo = new QGLFramebufferObject(1024, 1024,
			QGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D);
		qFbo->bind();
		if (!qFbo->isValid())
		{
			cleanupContext();
			return;
		}
#else
		static int visualAttribs[] = { None };
		int numberOfFramebufferConfigurations = 0;
		GLXFBConfig* fbConfigs = glXChooseFBConfig(display, DefaultScreen(display), visualAttribs, &numberOfFramebufferConfigurations);
		if (fbConfigs == NULL)
		{
			return;
		}
		static int attributeList[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_STENCIL_SIZE, 8, None };
		XVisualInfo *vi = glXChooseVisual(display, DefaultScreen(display), attributeList);
		if (vi == NULL)
		{
			size_t count = sizeof(attributeList) / sizeof(attributeList[0]);
			attributeList[count - 3] = None;
			vi = glXChooseVisual(display, DefaultScreen(display), attributeList);
		}
		if (vi == NULL)
		{
			XFree(fbConfigs);
		}
		context = glXCreateContext(display, vi, 0, True);
		XFree(vi);
		if (context == NULL)
		{
			XFree(fbConfigs);
			return;
		}
		// We're using FBO, but we need a PBuffer to bind something to the
		// context, so create a tiny one.
		int pbufferAttribs[] =
		{
			GLX_PBUFFER_WIDTH,  32,
			GLX_PBUFFER_HEIGHT, 32,
			None
		};
		pbuffer = glXCreatePbuffer(display, fbConfigs[0], pbufferAttribs);
		XFree(fbConfigs);
		XSync(display, False);
		if (!glXMakeContextCurrent(display, pbuffer, pbuffer, context))
		{
			cleanupContext();
			return;
		}
		TREGLExtensions::setup();
		ldSnapshotTaker = (LDSnapshotTaker*)alert->getSender()->retain();
		ldSnapshotTaker->setUseFBO(true);
#endif
	}
}
