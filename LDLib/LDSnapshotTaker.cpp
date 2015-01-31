#include "LDSnapshotTaker.h"
#include "LDUserDefaultsKeys.h"
#include "LDrawModelViewer.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCStringArray.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDPreferences.h>
#include <LDLib/LDViewPoint.h>
#include <TRE/TREGLExtensions.h>
#include <gl2ps.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

using namespace TREGLExtensionsNS;

#define FBO_SIZE 1024

#ifndef GL_EXT_packed_depth_stencil
#define GL_DEPTH_STENCIL_EXT              0x84F9
#define GL_UNSIGNED_INT_24_8_EXT          0x84FA
#define GL_DEPTH24_STENCIL8_EXT           0x88F0
#define GL_TEXTURE_STENCIL_SIZE_EXT       0x88F1
#endif

class FBOHelper
{
public:
	FBOHelper(bool useFBO, bool b16BPC) :
		m_useFBO(useFBO),
		m_16BPC(b16BPC),
		m_stencilBuffer(0)
	{
		if (m_useFBO)
		{
			GLint depthBits, stencilBits;
			GLenum colorFormat = GL_RGBA8;

			glGetIntegerv(GL_DEPTH_BITS, &depthBits);
			glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
			glGenFramebuffersEXT(1, &m_fbo);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
			if (depthBits == 24 && stencilBits == 8 &&
				TREGLExtensions::checkForExtension(
				"GL_EXT_packed_depth_stencil"))
			{
				// nVidia cards come here.  This part sucks.
				// Packed Depth/Stencil buffer
				glGenRenderbuffersEXT(1, &m_depthBuffer);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffer);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
					GL_DEPTH_STENCIL_EXT, FBO_SIZE, FBO_SIZE);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
					GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
					m_depthBuffer);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
					GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
					m_depthBuffer);
				if (m_16BPC)
				{
					// Note: GL_RGBA16 doesn't work right on nVidia cards, but
					// GL_RGBA12 does.  So use 12, so that at least we get 12
					// bits worth, which is still better than 8.
					colorFormat = GL_RGBA12;
				}
			}
			else
			{
				// Depth buffer
				glGenRenderbuffersEXT(1, &m_depthBuffer);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffer);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
					GL_DEPTH_COMPONENT24, FBO_SIZE, FBO_SIZE);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
					GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
					m_depthBuffer);

				// Stencil buffer
				glGenRenderbuffersEXT(1, &m_stencilBuffer);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_stencilBuffer);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
					GL_STENCIL_INDEX, FBO_SIZE, FBO_SIZE);
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
					GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
					m_stencilBuffer);
				if (m_16BPC)
				{
					// Note: this doesn't work on nVidia cards; hence it's
					// not being done above.
					colorFormat = GL_RGBA16;
				}
			}

			// Color buffer
			glGenRenderbuffersEXT(1, &m_colorBuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_colorBuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, colorFormat,
				FBO_SIZE, FBO_SIZE);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
				GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_colorBuffer);

			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
			if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) !=
				GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				debugPrintf("FBO Failed!\n");
			}
		}
		sm_active = true;
	}
	~FBOHelper()
	{
		if (m_useFBO)
		{
			glDeleteFramebuffersEXT(1, &m_fbo);
			glDeleteRenderbuffersEXT(1, &m_depthBuffer);
			if (m_stencilBuffer != 0)
			{
				glDeleteRenderbuffersEXT(1, &m_stencilBuffer);
			}
			glDeleteRenderbuffersEXT(1, &m_colorBuffer);
			glReadBuffer(GL_BACK);
		}
		sm_active = false;
	}
	static bool isActive(void) { return sm_active; }
	bool m_useFBO;
	bool m_16BPC;
	GLuint m_fbo;
	GLuint m_depthBuffer;
	GLuint m_stencilBuffer;
	GLuint m_colorBuffer;
	static bool sm_active;
};

bool FBOHelper::sm_active = false;

LDSnapshotTaker::LDSnapshotTaker(void):
m_modelViewer(NULL),
m_imageType(ITPng),
m_trySaveAlpha(TCUserDefaults::boolForKey(SAVE_ALPHA_KEY, false, false)),
m_autoCrop(TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false)),
m_fromCommandLine(true),
m_commandLineSaveSteps(false),
m_commandLineStep(false),
m_step(-1),
m_grabSetupDone(false),
m_gl2psAllowed(TCUserDefaults::boolForKey(GL2PS_ALLOWED_KEY, false, false)),
m_useFBO(false),
m_16BPC(false)
{
}

LDSnapshotTaker::LDSnapshotTaker(LDrawModelViewer *m_modelViewer):
m_modelViewer(m_modelViewer),
m_imageType(ITPng),
m_trySaveAlpha(false),
m_autoCrop(false),
m_fromCommandLine(false),
m_commandLineSaveSteps(false),
m_commandLineStep(false),
m_step(-1),
m_grabSetupDone(false),
m_gl2psAllowed(TCUserDefaults::boolForKey(GL2PS_ALLOWED_KEY, false, false)),
m_useFBO(false),
m_16BPC(false)
{
}

LDSnapshotTaker::~LDSnapshotTaker(void)
{
}

int LDSnapshotTaker::getFBOSize(void) const
{
	return FBO_SIZE;
}

void LDSnapshotTaker::dealloc(void)
{
	if (m_fromCommandLine)
	{
		TCObject::release(m_modelViewer);
	}
	TCObject::dealloc();
}

void LDSnapshotTaker::setUseFBO(bool value)
{
	m_useFBO = value && TREGLExtensions::haveFramebufferObjectExtension();
}

bool LDSnapshotTaker::saveImage(void)
{
	bool retValue = false;
	TCStringArray *unhandledArgs =
		TCUserDefaults::getUnhandledCommandLineArgs();

	if (unhandledArgs)
	{
		int i;
		int count = unhandledArgs->getCount();
		bool saveSnapshots = TCUserDefaults::boolForKey(SAVE_SNAPSHOTS_KEY,
			false, false);
		char *saveDir = NULL;
		const char *imageExt = NULL;
		int width = TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 640, false);
		int height = TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 480, false);
		bool zoomToFit = TCUserDefaults::boolForKey(SAVE_ZOOM_TO_FIT_KEY, true,
			false);
		bool commandLineType = false;
		std::string snapshotSuffix =
			TCUserDefaults::commandLineStringForKey(SNAPSHOT_SUFFIX_KEY);

		if (TCUserDefaults::commandLineStringForKey(SAVE_IMAGE_TYPE_KEY).size()
			> 0)
		{
			m_imageType =
				(ImageType)TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY);
			commandLineType = true;
		}
		if (snapshotSuffix.size()
			> 0)
		{
			m_imageType = typeForFilename(snapshotSuffix.c_str(), m_gl2psAllowed);
			commandLineType = true;
		}
		if (TCUserDefaults::commandLineStringForKey(SAVE_STEPS_KEY).size() > 0)
		{
			m_commandLineSaveSteps = true;
		}
		if (TCUserDefaults::commandLineStringForKey(STEP_KEY).size() > 0)
		{
			m_commandLineStep = true;
		}
		if (saveSnapshots)
		{
			switch (TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false))
			{
			case ITBmp:
				imageExt = ".bmp";
				break;
			case ITJpg:
				imageExt = ".jpg";
				break;
			case ITSvg:
				if (m_gl2psAllowed)
				{
					imageExt = ".svg";
					// NOTE: break is INTENTIONALLY inside the if statement.
					break;
				}
			case ITEps:
				if (m_gl2psAllowed)
				{
					imageExt = ".eps";
					// NOTE: break is INTENTIONALLY inside the if statement.
					break;
				}
			case ITPdf:
				if (m_gl2psAllowed)
				{
					imageExt = ".pdf";
					// NOTE: break is INTENTIONALLY inside the if statement.
					break;
				}
			default:
				imageExt = ".png";
				break;
			}
			saveDir = TCUserDefaults::stringForKey(SAVE_DIR_KEY, NULL, false);
			if (saveDir)
			{
				stripTrailingPathSeparators(saveDir);
			}
		}
		for (i = 0; i < count; i++)
		{
			char *arg = unhandledArgs->stringAtIndex(i);
			char newArg[1024];

			if (stringHasCaseInsensitivePrefix(arg, "-ca"))
			{
				float value;

				if (sscanf(arg + 3, "%f", &value) == 1)
				{
					sprintf(newArg, "-%s=%f", HFOV_KEY, value);
					TCUserDefaults::addCommandLineArg(newArg);
				}
			}
			else if (stringHasCaseInsensitivePrefix(arg, "-cg"))
			{
				sprintf(newArg, "-%s=%s", CAMERA_GLOBE_KEY, arg + 3);
				TCUserDefaults::addCommandLineArg(newArg);
				zoomToFit = true;
			}
		}
		for (i = 0; i < count && (saveSnapshots || !retValue); i++)
		{
			char *arg = unhandledArgs->stringAtIndex(i);
			
			if (arg[0] != '-' && arg[0] != 0)
			{
				std::string imageFilename;

				if (saveSnapshots)
				{
					char *baseFilename = filenameFromPath(arg);
					std::string mpdName;
					size_t mpdSpot;

					if (saveDir)
					{
						imageFilename = saveDir;
						imageFilename += "/";
						imageFilename += baseFilename;
					}
					else
					{
						imageFilename = arg;
					}
#ifdef WIN32
					mpdSpot = imageFilename.find(':', 2);
#else // WIN32
					mpdSpot = imageFilename.find(':');
#endif // WIN32
					if (mpdSpot < imageFilename.size())
					{
						char *baseMpdSpot = strrchr(baseFilename, ':');
						std::string mpdExt;

						mpdName = '-';
						mpdName += imageFilename.substr(mpdSpot + 1);
						imageFilename = imageFilename.substr(0, mpdSpot);
						if (baseMpdSpot != NULL &&
							strlen(baseMpdSpot) == mpdName.size())
						{
							baseMpdSpot[0] = 0;
						}
						mpdSpot = mpdName.rfind('.');
						if (mpdSpot < mpdName.length())
						{
							mpdExt = mpdName.substr(mpdSpot);
							convertStringToLower(&mpdExt[0]);
							if (mpdExt == ".dat" || mpdExt == ".ldr" ||
								mpdExt == ".mpd")
							{
								mpdName = mpdName.substr(0, mpdSpot);
							}
						}
					}
					// Note: we need there to be a dot in the base filename,
					// not the path before that.
					if (strchr(baseFilename, '.'))
					{
						imageFilename = imageFilename.substr(0,
							imageFilename.rfind('.'));
					}
					delete baseFilename;
					imageFilename += mpdName;
					imageFilename += imageExt;
				}
				else
				{
					char *tempFilename = TCUserDefaults::stringForKey(
						SAVE_SNAPSHOT_KEY, NULL, false);

					if (tempFilename != NULL)
					{
						imageFilename = tempFilename;
						delete tempFilename;
					}
					if (imageFilename.size() > 0 && !commandLineType)
					{
						m_imageType = typeForFilename(imageFilename.c_str(),
							m_gl2psAllowed);
					}
				}
				if (imageFilename.size() > 0)
				{
					if (m_modelViewer && m_modelViewer->getFilename())
					{
						m_modelViewer->setFilename(arg);
						m_modelViewer->loadModel();
					}
					else if (m_modelViewer)
					{
						m_modelViewer->setFilename(arg);
					}
					else
					{
						m_modelFilename = arg;
					}
					retValue = saveImage(imageFilename.c_str(), width, height,
						zoomToFit) || retValue;
				}
			}
		}
		delete saveDir;
		unhandledArgs->release();
	}
	return retValue;
}

bool LDSnapshotTaker::shouldZoomToFit(bool zoomToFit)
{
	char *cameraGlobe = TCUserDefaults::stringForKey(CAMERA_GLOBE_KEY, NULL,
		false);
	bool retValue = false;

	if (zoomToFit)
	{
		retValue = true;
	}
	else if (cameraGlobe)
	{
		float globeRadius;

		if (sscanf(cameraGlobe, "%*f,%*f,%f", &globeRadius) == 1)
		{
			retValue = true;
		}
	}
	delete cameraGlobe;
	return retValue;
}

bool LDSnapshotTaker::saveImage(
	const char *filename,
	int imageWidth,
	int imageHeight,
	bool zoomToFit)
{
	bool steps = false;
	FBOHelper fboHelper(m_useFBO, m_16BPC);

	if (!m_fromCommandLine || m_commandLineSaveSteps)
	{
		steps = TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false, false);
	}
	if (steps || m_commandLineStep)
	{
		char *stepSuffix = TCUserDefaults::stringForKey(SAVE_STEPS_SUFFIX_KEY,
			"-Step", false);
		bool retValue = true;
		int numSteps;
		int origStep;
		LDViewPoint *viewPoint = NULL;

		if (!m_modelViewer)
		{
			grabSetup();
		}
		origStep = m_modelViewer->getStep();
		if (m_modelViewer->getMainModel() == NULL)
		{
			// This isn't very efficient, but it gets the job done.  A
			// number of things need to happen before we can do the initial
			// zoomToFit.  We need to load the model, create the rotation
			// matrix, and set up the camera.  Maybe other things need to be
			// done too.  This update makes sure that things are OK for the
			// zoomToFit to execute properly.
			renderOffscreenImage();
		}
		if (TCUserDefaults::boolForKey(SAVE_STEPS_SAME_SCALE_KEY, true, false)
			&& zoomToFit)
		{
			if (!m_fromCommandLine)
			{
				viewPoint = m_modelViewer->saveViewPoint();
			}
			numSteps = m_modelViewer->getNumSteps();
			m_modelViewer->setStep(numSteps);
			m_modelViewer->zoomToFit();
			zoomToFit = false;
		}
		if (steps)
		{
			numSteps = m_modelViewer->getNumSteps();
		}
		else
		{
			numSteps = 1;
		}
		for (int step = 1; step <= numSteps && retValue; step++)
		{
			std::string stepFilename;

			if (steps)
			{
				stepFilename = removeStepSuffix(filename, stepSuffix);
				stepFilename = addStepSuffix(stepFilename, stepSuffix, step,
					numSteps);
				m_step = step;
			}
			else
			{
				stepFilename = filename;
				m_step = TCUserDefaults::longForKey(STEP_KEY);
			}
			retValue = saveStepImage(stepFilename.c_str(), imageWidth,
				imageHeight, zoomToFit);
		}
		delete stepSuffix;
		m_modelViewer->setStep(origStep);
		if (viewPoint)
		{
			m_modelViewer->restoreViewPoint(viewPoint);
			viewPoint->release();
		}
		return retValue;
	}
	else
	{
		m_step = -1;
		return saveStepImage(filename, imageWidth, imageHeight, zoomToFit);
	}
}

bool LDSnapshotTaker::saveGl2psStepImage(
	const char *filename,
	int /*imageWidth*/,
	int /*imageHeight*/,
	bool zoomToFit)
{
	int bufSize;
	int state = GL2PS_OVERFLOW;
	FILE *file = fopen(filename, "wb");
	bool retValue = false;

	if (file != NULL)
	{
		bool origForceZoomToFit;
		LDViewPoint *viewPoint = NULL;

		grabSetup();
		origForceZoomToFit = m_modelViewer->getForceZoomToFit();
		if (zoomToFit)
		{
			viewPoint = m_modelViewer->saveViewPoint();
			m_modelViewer->setForceZoomToFit(true);
		}
#ifdef COCOA
		// For some reason, we get nothing in the feedback buffer on the Mac if
		// we don't reparse the model.  No idea why that is.
		m_modelViewer->reparse();
#endif // COCOA
		m_modelViewer->setGl2ps(true);
		m_modelViewer->setup();
		m_modelViewer->setHighlightPaths("");
		TCAlertManager::sendAlert(alertClass(), this, _UC("PreRender"));
		TCAlertManager::sendAlert(alertClass(), this, _UC("MakeCurrent"));
		for (bufSize = 1024 * 1024; state == GL2PS_OVERFLOW; bufSize *= 2)
		{
			GLint format;
			GLint options = GL2PS_USE_CURRENT_VIEWPORT
				| GL2PS_OCCLUSION_CULL
				| GL2PS_BEST_ROOT
				| GL2PS_NO_PS3_SHADING;

			if (m_autoCrop)
			{
				options |= GL2PS_TIGHT_BOUNDING_BOX;
			}
			switch (m_imageType)
			{
			case ITEps:
				format = GL2PS_EPS;
				break;
			case ITPdf:
				format = GL2PS_PDF;
				options |= GL2PS_COMPRESS;
				break;
			default:
				format = GL2PS_SVG;
				break;
			}
			state = gl2psBeginPage(filename, "LDView", NULL, format,
				GL2PS_BSP_SORT,	options, GL_RGBA, 0, NULL, 0, 0, 0, bufSize,
				file, filename);
			if (state == GL2PS_ERROR)
			{
				debugPrintf("ERROR in gl2ps routine!");
			}
			else
			{
				renderOffscreenImage();
				glFinish();
				state = gl2psEndPage();
				if (state == GL2PS_ERROR)
				{
					debugPrintf("ERROR in gl2ps routine!");
				}
				else
				{
					retValue = true;
				}
			}
		}
		m_modelViewer->setGl2ps(false);
		if (zoomToFit)
		{
			m_modelViewer->setForceZoomToFit(origForceZoomToFit);
			m_modelViewer->restoreViewPoint(viewPoint);
		}
		fclose(file);
		if (viewPoint != NULL)
		{
			m_modelViewer->restoreViewPoint(viewPoint);
			viewPoint->release();
		}
	}
	return retValue;
}

bool LDSnapshotTaker::saveStepImage(
	const char *filename,
	int imageWidth,
	int imageHeight,
	bool zoomToFit)
{
	bool retValue = false;

	if (m_imageType >= ITSvg && m_imageType <= ITPdf)
	{
		retValue = saveGl2psStepImage(filename, imageWidth, imageHeight,
			zoomToFit);
	}
	else
	{
		bool saveAlpha = false;
		TCByte *buffer = grabImage(imageWidth, imageHeight,
			shouldZoomToFit(zoomToFit), NULL, &saveAlpha);

		if (buffer)
		{
			switch (m_imageType)
			{
			case ITPng:
				retValue = writePng(filename, imageWidth, imageHeight, buffer,
					saveAlpha);
				break;
			case ITBmp:
				retValue = writeBmp(filename, imageWidth, imageHeight, buffer);
				break;
			case ITJpg:
				retValue = writeJpg(filename, imageWidth, imageHeight, buffer);
				break;
			default:
				// Get rid of warning
				break;
			}
			delete buffer;
		}
	}
	return retValue;
}

bool LDSnapshotTaker::staticImageProgressCallback(
	CUCSTR message,
	float progress,
	void* userData)
{
	return ((LDSnapshotTaker*)userData)->imageProgressCallback(message,
		progress);
}

bool LDSnapshotTaker::imageProgressCallback(CUCSTR message, float progress)
{
	bool aborted;
	ucstring newMessage;

	if (message != NULL)
	{
		char *filename = filenameFromPath(m_currentImageFilename.c_str());
		UCSTR ucFilename = mbstoucstring(filename);

		delete filename;
		if (stringHasCaseInsensitivePrefix(message, _UC("Saving")))
		{
			newMessage = TCLocalStrings::get(_UC("SavingPrefix"));
		}
		else
		{
			newMessage = TCLocalStrings::get(_UC("LoadingPrefix"));
		}
		newMessage += ucFilename;
		delete ucFilename;
	}

	TCProgressAlert::send("LDSnapshotTaker", newMessage.c_str(), progress,
		&aborted, this);
	return !aborted;
}

bool LDSnapshotTaker::writeImage(
	const char *filename,
	int width,
	int height,
	TCByte *buffer,
	const char *formatName,
	bool saveAlpha)
{
	TCImage *image = new TCImage;
	bool retValue;
	char comment[1024];

	m_currentImageFilename = filename;
	if (saveAlpha)
	{
		if (m_16BPC)
		{
			image->setDataFormat(TCRgba16);
		}
		else
		{
			image->setDataFormat(TCRgba8);
		}
	}
	else if (m_16BPC)
	{
		image->setDataFormat(TCRgb16);
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
	if (m_productVersion.size() > 0)
	{
		strcat(comment, " ");
		strcat(comment, m_productVersion.c_str());
	}
	image->setComment(comment);
	if (m_autoCrop)
	{
		image->autoCrop((TCByte)m_modelViewer->getBackgroundR(),
			(TCByte)m_modelViewer->getBackgroundG(),
			(TCByte)m_modelViewer->getBackgroundB());
	}
	retValue = image->saveFile(filename, staticImageProgressCallback, this);
	debugPrintf("Saved image: %s\n", filename);
	image->release();
	return retValue;
}

bool LDSnapshotTaker::writeJpg(
	const char *filename,
	int width,
	int height,
	TCByte *buffer)
{
	return writeImage(filename, width, height, buffer, "JPG", false);
}

bool LDSnapshotTaker::writeBmp(
	const char *filename,
	int width,
	int height,
	TCByte *buffer)
{
	return writeImage(filename, width, height, buffer, "BMP", false);
}

bool LDSnapshotTaker::writePng(
	const char *filename,
	int width,
	int height,
	TCByte *buffer,
	bool saveAlpha)
{
	return writeImage(filename, width, height, buffer, "PNG", saveAlpha);
}

void LDSnapshotTaker::calcTiling(
	int desiredWidth,
	int desiredHeight,
	int &bitmapWidth,
	int &bitmapHeight,
	int &numXTiles,
	int &numYTiles)
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

bool LDSnapshotTaker::canSaveAlpha(void)
{
	if (m_trySaveAlpha && m_imageType == ITPng)
	{
		GLint alphaBits;

		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		return alphaBits > 0;
	}
	return false;
}

void LDSnapshotTaker::renderOffscreenImage(void)
{
	TCAlertManager::sendAlert(alertClass(), this, _UC("MakeCurrent"));
	if (m_modelViewer->getMainModel() == NULL)
	{
		m_modelViewer->loadModel();
	}
	m_modelViewer->update();
}

void LDSnapshotTaker::grabSetup(void)
{
	if (m_grabSetupDone)
	{
		return;
	}
	TCAlertManager::sendAlert(alertClass(), this, _UC("PreSave"));
	m_grabSetupDone = true;
	if (!m_modelViewer)
	{
		LDPreferences *prefs;
		GLint viewport[4];
		
		glGetIntegerv(GL_VIEWPORT, viewport);
		m_modelViewer = new LDrawModelViewer(viewport[2], viewport[3]);
		m_modelViewer->setFilename(m_modelFilename.c_str());
		m_modelViewer->setNoUI(true);
		m_modelFilename = "";
		prefs = new LDPreferences(m_modelViewer);
		prefs->loadSettings();
		prefs->applySettings();
		prefs->release();
	}
}

TCByte *LDSnapshotTaker::grabImage(
	int &imageWidth,
	int &imageHeight,
	bool zoomToFit,
	TCByte *buffer,
	bool *saveAlpha)
{
	FBOHelper *localHelper = NULL;
	if (!FBOHelper::isActive())
		{
		localHelper = new FBOHelper(m_useFBO, m_16BPC);
		}
	grabSetup();

	GLenum bufferFormat = GL_RGB;
	GLenum componentType = GL_UNSIGNED_BYTE;
	bool origForceZoomToFit = m_modelViewer->getForceZoomToFit();
	StringList origHighlightPaths = m_modelViewer->getHighlightPaths();
	TCVector origCameraPosition = m_modelViewer->getCamera().getPosition();
	TCFloat origXPan = m_modelViewer->getXPan();
	TCFloat origYPan = m_modelViewer->getYPan();
	int origWidth = m_modelViewer->getWidth();
	int origHeight = m_modelViewer->getHeight();
	bool origAutoCenter = m_modelViewer->getAutoCenter();
	GLint viewport[4];
	int newWidth, newHeight;
	int numXTiles, numYTiles;
	int xTile;
	int yTile;
	TCByte *smallBuffer;
	int bytesPerPixel;
	int bytesPerLine;
	int bytesPerChannel = 1;
	int smallBytesPerLine;
	int reallySmallBytesPerLine;
	bool canceled = false;
	bool bufferAllocated = false;

	if (m_16BPC)
	{
		bytesPerChannel = 2;
		componentType = GL_UNSIGNED_SHORT;
	}
	if (m_step > 0)
	{
		m_modelViewer->setStep(m_step);
	}
	if (m_useFBO)
	{
		newWidth = FBO_SIZE;
		newHeight = FBO_SIZE;
	}
	else
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		newWidth = viewport[2];
		newHeight = viewport[3];
	}
	m_modelViewer->setWidth(newWidth);
	m_modelViewer->setHeight(newHeight);
	m_modelViewer->perspectiveView();
	calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
		numYTiles);
	m_modelViewer->setWidth(newWidth);
	m_modelViewer->setHeight(newHeight);
	if (zoomToFit)
	{
		m_modelViewer->setForceZoomToFit(true);
		m_modelViewer->perspectiveView();
	}
	m_modelViewer->setup();
	m_modelViewer->setHighlightPaths("");
	if (canSaveAlpha())
	{
		bytesPerPixel = 4 * bytesPerChannel;
		bufferFormat = GL_RGBA;
		m_modelViewer->setSaveAlpha(true);
		if (saveAlpha)
		{
			*saveAlpha = true;
		}
	}
	else
	{
		bytesPerPixel = 3 * bytesPerChannel;
		if (saveAlpha)
		{
			*saveAlpha = false;
		}
	}
	imageWidth = newWidth * numXTiles;
	imageHeight = newHeight * numYTiles;
	smallBytesPerLine = TCImage::roundUp(newWidth * bytesPerPixel, 4);
	reallySmallBytesPerLine = newWidth * bytesPerPixel;
	bytesPerLine = TCImage::roundUp(imageWidth * bytesPerPixel, 4);
	if (!buffer)
	{
		buffer = new TCByte[bytesPerLine * imageHeight];
		bufferAllocated = true;
	}
	if (numXTiles == 1 && numYTiles == 1)
	{
		smallBuffer = buffer;
	}
	else
	{
		smallBuffer = new TCByte[smallBytesPerLine * newHeight];
	}
	m_modelViewer->setNumXTiles(numXTiles);
	m_modelViewer->setNumYTiles(numYTiles);
	TCAlertManager::sendAlert(alertClass(), this, _UC("PreRender"));
	for (yTile = 0; yTile < numYTiles; yTile++)
	{
		m_modelViewer->setYTile(yTile);
		for (xTile = 0; xTile < numXTiles && !canceled; xTile++)
		{
			m_modelViewer->setXTile(xTile);
			renderOffscreenImage();
			TCProgressAlert::send("LDSnapshotTaker",
				TCLocalStrings::get(_UC("RenderingSnapshot")),
				(float)(yTile * numXTiles + xTile) / (numYTiles * numXTiles),
				&canceled);
			if (!canceled)
			{
				glFinish();
				TCAlertManager::sendAlert(alertClass(), this,
					_UC("RenderDone"));
				glReadPixels(0, 0, newWidth, newHeight, bufferFormat,
					componentType, smallBuffer);
				if (smallBuffer != buffer)
				{
					int y;

					for (y = 0; y < newHeight; y++)
					{
						int smallOffset = y * smallBytesPerLine;
						int offset = (y + (numYTiles - yTile - 1) * newHeight) *
							bytesPerLine + xTile * newWidth * bytesPerPixel;

						memcpy(&buffer[offset], &smallBuffer[smallOffset],
							reallySmallBytesPerLine);
					}
					// We only need to zoom to fit on the first tile; the
					// rest will already be correct.
					m_modelViewer->setForceZoomToFit(false);
				}
			}
			else
			{
				canceled = true;
			}
		}
	}
	m_modelViewer->setXTile(0);
	m_modelViewer->setYTile(0);
	m_modelViewer->setNumXTiles(1);
	m_modelViewer->setNumYTiles(1);
	m_modelViewer->setWidth(origWidth);
	m_modelViewer->setHeight(origHeight);
	m_modelViewer->setSaveAlpha(false);
	m_modelViewer->setHighlightPaths(origHighlightPaths);
	if (canceled && bufferAllocated)
	{
		delete buffer;
		buffer = NULL;
	}
	if (smallBuffer != buffer)
	{
		delete smallBuffer;
	}
	if (zoomToFit)
	{
		m_modelViewer->setForceZoomToFit(origForceZoomToFit);
		m_modelViewer->getCamera().setPosition(origCameraPosition);
		m_modelViewer->setXYPan(origXPan, origYPan);
		m_modelViewer->setAutoCenter(origAutoCenter);
	}
	delete localHelper;
	return buffer;
}

bool LDSnapshotTaker::doCommandLine(void)
{
	LDSnapshotTaker *snapshotTaker = new LDSnapshotTaker;
	bool retValue = snapshotTaker->saveImage();

	snapshotTaker->release();
	return retValue;
}

// Note: static method
std::string LDSnapshotTaker::removeStepSuffix(
	const std::string &filename,
	const std::string &stepSuffix)
{
	if (stepSuffix.size() == 0)
	{
		return filename;
	}
	char *dirPart = directoryFromPath(filename.c_str());
	char *filePart = filenameFromPath(filename.c_str());
	std::string fileString = filePart;
	size_t suffixLoc;
	std::string tempSuffix = stepSuffix;
	std::string newString;

	newString = dirPart;
	delete dirPart;
#if defined(WIN32) || defined(__APPLE__)
	// case-insensitive file systems
	convertStringToLower(&fileString[0]);
	convertStringToLower(&tempSuffix[0]);
#endif // WIN32 || __APPLE__
	suffixLoc = fileString.rfind(tempSuffix);
	if (suffixLoc < fileString.size())
	{
		size_t i;

		for (i = suffixLoc + tempSuffix.size(); isdigit(fileString[i]); i++)
		{
			// Don't do anything
		}
#if defined(WIN32) || defined(__APPLE__)
		// case-insensitive file systems
		// Restore filename to original case
		fileString = filePart;
		delete filePart;
#endif // WIN32 || __APPLE__
		fileString.erase(suffixLoc, i - suffixLoc);
	}
	else
	{
		delete filePart;
		return filename;
	}
	if (newString.size() > 0)
	{
		newString += "/";
	}
	newString += fileString;
	filePart = cleanedUpPath(newString.c_str());
	newString = filePart;
	delete filePart;
	return newString;
}

// Note: static method
std::string LDSnapshotTaker::addStepSuffix(
	const std::string &filename,
	const std::string &stepSuffix,
	int step,
	int numSteps)
{
	size_t dotSpot = filename.rfind('.');
	std::string newString;
	char format[32];
	char buf[32];
	int digits = 1;

	while ((numSteps = numSteps / 10) != 0)
	{
		digits++;
	}
	sprintf(format, "%%0%dd", digits);
	sprintf(buf, format, step);
	newString = filename.substr(0, dotSpot);
	newString += stepSuffix;
	newString += buf;
	if (dotSpot < filename.size())
	{
		newString += filename.substr(dotSpot);
	}
	return newString;
}

// Note: static method
LDSnapshotTaker::ImageType LDSnapshotTaker::typeForFilename(
	const char *filename,
	bool gl2psAllowed)
{
	if (stringHasCaseInsensitiveSuffix(filename, ".png"))
	{
		return ITPng;
	}
	else if (stringHasCaseInsensitiveSuffix(filename, ".bmp"))
	{
		return ITBmp;
	}
	else if (stringHasCaseInsensitiveSuffix(filename, ".jpg"))
	{
		return ITJpg;
	}
	else if (stringHasCaseInsensitivePrefix(filename, ".svg") && gl2psAllowed)
	{
		return ITSvg;
	}
	else if (stringHasCaseInsensitivePrefix(filename, ".eps") && gl2psAllowed)
	{
		return ITEps;
	}
	else if (stringHasCaseInsensitivePrefix(filename, ".pdf") && gl2psAllowed)
	{
		return ITPdf;
	}
	else
	{
		// PNG is the default;
		return ITPng;
	}
}

// Note: static method
std::string LDSnapshotTaker::extensionForType(
	ImageType type,
	bool includeDot /*= false*/)
{
	if (includeDot)
	{
		std::string retValue(".");
		
		retValue += extensionForType(type, false);
		return retValue;
	}
	switch (type)
	{
	case ITPng:
		return "png";
	case ITBmp:
		return "bmp";
	case ITJpg:
		return "jpg";
	case ITSvg:
		return "svg";
	case ITEps:
		return "eps";
	case ITPdf:
		return "pdf";
	default:
		return "png";
	}
}

// Note: static method
LDSnapshotTaker::ImageType LDSnapshotTaker::lastImageType(void)
{
	if (TCUserDefaults::boolForKey(GL2PS_ALLOWED_KEY, false, false))
	{
		return ITLast;
	}
	else
	{
		return ITJpg;
	}
}

