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
#if (QT_VERSION >= 0x50100) && defined(QOFFSCREEN)
static QOffscreenSurface surf;
static QOpenGLContext ctx;
#endif

SnapshotTaker::SnapshotTaker()
	: ldSnapshotTaker(NULL)
	, snapshotAlertHandler(new SnapshotAlertHandler(this))
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
	if (surf.isValid())
	{
		surf.destroy();
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
		surfaceFormat.setMajorVersion(1);
		surfaceFormat.setMinorVersion(5);
		ctx.setFormat(surfaceFormat);
		ctx.create();
		if (!ctx.isValid())
		{
			return;
		}
		surf.setFormat(surfaceFormat);
		surf.create();
		if (!surf.isValid())
		{
			return;
		}
		ctx.makeCurrent(&surf);
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
