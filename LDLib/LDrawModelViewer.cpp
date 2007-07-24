#include "LDrawModelViewer.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCWebClient.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLError.h>
#include <LDLoader/LDLFindFileAlert.h>
#include <LDLoader/LDLPalette.h>
#include "LDModelParser.h"
#include "LDPreferences.h"
#include "LDPartsList.h"
#include "LDViewPoint.h"
#include <TRE/TREMainModel.h>
#include <TRE/TREGL.h>
#include <time.h>

#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 16
#define FONT_IMAGE_WIDTH 128
#define FONT_IMAGE_HEIGHT 256
#define FONT_NUM_CHARACTERS 256
#define DEF_DISTANCE_MULT 1.0f

LDrawModelViewer::LDrawModelViewer(int width, int height)
			:mainTREModel(NULL),
			 mainModel(NULL),
			 lightDirModel(NULL),
			 filename(NULL),
			 programPath(NULL),
			 width(width),
			 height(height),
			 pixelAspectRatio(1.0f),
			 cullBackFaces(0),
			 xRotate(1.0f),
			 yRotate(1.0f),
			 zRotate(0.0f),
			 rotationSpeed(0.0f),
			 cameraXRotate(0.0f),
			 cameraYRotate(0.0f),
			 cameraZRotate(0.0f),
			 zoomSpeed(0.0f),
			 xPan(0.0f),
			 yPan(0.0f),
			 rotationMatrix(NULL),
			 defaultRotationMatrix(NULL),
			 clipAmount(0.0f),
			 nextClipAmount(-1.0f),
			 nextDistance(-1.0f),
			 highlightLineWidth(1.0f),
			 wireframeLineWidth(1.0f),
			 anisoLevel(1.0f),
			 clipZoom(false),
			 fontListBase(0),
			 backgroundR(0.0f),
			 backgroundG(0.0f),
			 backgroundB(0.0f),
			 defaultR(153),
			 defaultG(153),
			 defaultB(153),
			 defaultColorNumber(-1),
//			 progressCallback(NULL),
//			 errorCallback(NULL),
			 xTile(0),
			 yTile(0),
			 numXTiles(1),
			 numYTiles(1),
			 stereoMode(LDVStereoNone),
			 stereoEyeSpacing(50.0f),
			 cutawayMode(LDVCutawayNormal),
			 cutawayAlpha(1.0f),
			 cutawayLineWidth(1.0f),
			 zoomMax(1.99f),
			 curveQuality(2),
			 textureFilterType(GL_LINEAR_MIPMAP_LINEAR),
			 distanceMultiplier(DEF_DISTANCE_MULT),
			 fontImage(NULL),
			 aspectRatio(1.0f),
			 currentFov(45.0f),
			 fov(45.0f),
			 extraSearchDirs(NULL),
			 memoryUsage(2),
			 lightVector(0.0f, 0.0f, 1.0f),
			 preferences(NULL),
			 mouseMode(LDVMouseNone),
			 cameraData(NULL)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDrawModelViewer");
#endif
	flags.qualityLighting = 0;
	flags.showsHighlightLines = 0;
	flags.qualityStuds = 0;
	flags.usesFlatShading = 0;
	flags.usesSpecular = 1;
	flags.drawWireframe = 0;
	flags.useWireframeFog = 0;
	flags.usePolygonOffset = 1;
	flags.useLighting = 1;
	flags.subduedLighting = 0;
	flags.allowPrimitiveSubstitution = true;
	flags.useStipple = 0;
	flags.sortTransparent = 1;
	flags.needsSetup = true;
	flags.needsTextureSetup = true;
	flags.needsMaterialSetup = true;
	flags.needsLightingSetup = true;
	flags.needsReload = false;
	flags.needsRecompile = false;
	flags.needsResize = true;
	flags.paused = 0;
	flags.slowClear = false;
	flags.blackHighlights = false;
	flags.textureStuds = true;
	flags.oneLight = false;
	flags.drawConditionalHighlights = false;
	flags.showAllConditionalLines = false;
	flags.showConditionalControlPoints = false;
	flags.performSmoothing = true;
	flags.lineSmoothing = false;
	flags.constrainZoom = true;
	flags.needsRotationMatrixSetup = true;
	flags.edgesOnly = false;
	flags.hiResPrimitives = false;
	flags.needsViewReset = true;
	flags.processLDConfig = true;
	flags.skipValidation = false;
	flags.autoCenter = true;
	flags.forceZoomToFit = false;
	flags.defaultTrans = false;
	flags.bfc = true;
	flags.redBackFaces = false;
	flags.greenFrontFaces = false;
	flags.defaultLightVector = true;
	flags.overrideModelCenter = false;
	flags.overrideModelSize = false;
	flags.overrideDefaultDistance = false;
	flags.checkPartTracker = true;
	flags.showLight = false;
	flags.drawLightDats = true;
	flags.optionalStandardLight = true;
	flags.noLightGeom = false;
	flags.updating = false;
//	TCAlertManager::registerHandler(LDLError::alertClass(), this,
//		(TCAlertCallback)ldlErrorCallback);
//	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
//		(TCAlertCallback)progressAlertCallback);
	TCAlertManager::registerHandler(LDLFindFileAlert::alertClass(), this,
		(TCAlertCallback)&LDrawModelViewer::findFileAlertCallback);
}

LDrawModelViewer::~LDrawModelViewer(void)
{
}

void LDrawModelViewer::dealloc(void)
{
//	TCAlertManager::unregisterHandler(LDLError::alertClass(), this);
//	TCAlertManager::unregisterHandler(TCProgressAlert::alertClass(), this);
	TCAlertManager::unregisterHandler(LDLFindFileAlert::alertClass(), this);
	TCObject::release(mainTREModel);
	TCObject::release(mainModel);
	TCObject::release(lightDirModel);
	mainTREModel = NULL;
	delete filename;
	filename = NULL;
	delete programPath;
	programPath = NULL;
	delete rotationMatrix;
	rotationMatrix = NULL;
	delete defaultRotationMatrix;
	defaultRotationMatrix = NULL;
	TCObject::release(fontImage);
	fontImage = NULL;
	TCObject::release(extraSearchDirs);
	extraSearchDirs = NULL;
	delete cameraData;
	cameraData = NULL;
	TCObject::dealloc();
}

void LDrawModelViewer::setFilename(const char* value)
{
	delete filename;
	filename = copyString(value);
}

void LDrawModelViewer::setProgramPath(const char *value)
{
	delete programPath;
	programPath = copyString(value);
	stripTrailingPathSeparators(programPath);
}

void LDrawModelViewer::setFileIsPart(bool value)
{
	flags.fileIsPart = value;
}

void LDrawModelViewer::applyTile(void)
{
	if (1 || numXTiles > 1 || numYTiles > 1)
	{
//		GLfloat tileWidth, tileHeight;
		GLint tileLeft, tileRight;
		GLint tileBottom, tileTop;
//		GLint thisTileWidth, thisTileHeight;
		GLfloat xScale, yScale;
		GLfloat xOffset, yOffset;

//		tileWidth = (TCFloat)width / numXTiles;
//		tileHeight = (TCFloat)height / numYTiles;
		tileLeft = (int)(xTile * width);
		tileRight = (int)((xTile + 1) * width);
		tileBottom = (int)((numYTiles - yTile - 1) * height);
		tileTop = (int)(((numYTiles - yTile - 1) + 1) * height);
//		thisTileWidth = tileRight - tileLeft;
//		thisTileHeight = tileTop - tileBottom;
		xScale = (GLfloat)(width * numXTiles) / (GLfloat)width;
		yScale = (GLfloat)(height * numYTiles) / (GLfloat)height;
		xOffset = (-2.0f * tileLeft) / (width * numXTiles) +
			(1 - 1.0f / numXTiles);
		yOffset = (-2.0f * tileBottom) / (height * numYTiles) +
			(1 - 1.0f / numYTiles);
		glScalef(xScale, yScale, 1.0f);
		treGlTranslatef(xOffset, yOffset, 0.0f);
	}
}

TCFloat LDrawModelViewer::getStereoWidthModifier(void)
{
	if (stereoMode == LDVStereoCrossEyed || stereoMode == LDVStereoParallel)
	{
		return 2.0f;
	}
	else
	{
		return 1.0f;
	}
}

void LDrawModelViewer::setFieldOfView(double fov, TCFloat nClip, TCFloat fClip)
{
	GLdouble aspectWidth, aspectHeight;

//	printf("LDrawModelViewer::setFieldOfView(%.5f, %.5f, %.5f)\n", fov, nClip,
//		fClip);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	applyTile();
	aspectWidth = width * numXTiles / getStereoWidthModifier();
	aspectHeight = height * numYTiles * pixelAspectRatio;
	gluPerspective(fov, aspectWidth / aspectHeight, nClip, fClip);
	glMatrixMode(GL_MODELVIEW);
}

void LDrawModelViewer::perspectiveView(void)
{
	perspectiveView(true);
}

void LDrawModelViewer::setFov(TCFloat value)
{
	if (value != fov)
	{
		fov = value;
		flags.needsViewReset = true;
	}
}

TCFloat LDrawModelViewer::getHFov(void)
{
	int actualWidth = width / (int)getStereoWidthModifier();

	return (TCFloat)(2.0 * rad2deg(atan(tan(deg2rad(fov / 2.0)) *
		(actualWidth * numXTiles) / (double)height * numYTiles)));
}

void LDrawModelViewer::updateCurrentFov(void)
{
	int actualWidth = width / (int)getStereoWidthModifier();

	currentFov = TCUserDefaults::floatForKey("HFOV", -1, false);
	if (currentFov == -1)
	{
		currentFov = fov;
		if (actualWidth * numXTiles < height * numYTiles)
		{
			// When the window is taller than it is wide, we want our current
			// FOV to be the horizontal FOV, so we need to calculate the
			// vertical FOV.
			//
			// From Lars Hassing:
			// Vertical FOV = 2*atan(tan(hfov/2)/(width/height))
			currentFov = (TCFloat)(2.0 * rad2deg(atan(tan(deg2rad(fov / 2.0)) *
				(double)height * numYTiles / (actualWidth * numXTiles))));

			if (currentFov > 179.0f)
			{
				currentFov = 179.0f;
			}
			aspectRatio = (TCFloat)height / actualWidth;
		}
		else
		{
			aspectRatio = (TCFloat)actualWidth / height;
		}
	}
	else
	{
		fov = (TCFloat)(2.0 * rad2deg(atan(tan(deg2rad(currentFov / 2.0)) *
			(double)height * numYTiles / (actualWidth * numXTiles))));
		if (actualWidth * numXTiles > height * numYTiles)
		{
			currentFov = fov;
			aspectRatio = (TCFloat)actualWidth / height;
		}
		else
		{
			aspectRatio = (TCFloat)height / actualWidth;
		}
	}
}

TCFloat LDrawModelViewer::getClipRadius(void)
{
	TCFloat clipRadius;

	if (flags.autoCenter)
	{
		clipRadius = size / 1.45f;
	}
	else
	{
		// If we aren't centered, then just double the clip radius, and that
		// guarantees everything will fit.  Remember that the near clip plane
		// has a minimum distance, so even if it ends up initially behind the
		// camera, we clamp it to be in front.
		clipRadius = size;
	}
	return clipRadius;
}

TCFloat LDrawModelViewer::getZDistance(void)
{
	if (flags.autoCenter)
	{
		TCFloat inverseMatrix[16];
		TCVector vector = camera.getPosition();

		camera.getFacing().getInverseMatrix(inverseMatrix);
		vector = vector.transformPoint(inverseMatrix);
		return vector[2];
	}
	else
	{
		return (center - camera.getPosition()).length();
	}
}

void LDrawModelViewer::perspectiveView(bool resetViewport)
{
	TCFloat nClip;
	TCFloat fClip;
	TCFloat clipRadius = getClipRadius();
	int actualWidth = width / (int)getStereoWidthModifier();
	TCFloat zDistance = getZDistance();
	TCFloat aspectAdjust = (TCFloat)tan(1.0f);

	if (flags.forceZoomToFit)
	{
		zoomToFit();
	}
//	zDistance = camera.getPosition().length();
	updateCurrentFov();
	if (resetViewport)
	{
		glViewport(0, 0, actualWidth, height);
		flags.needsResize = false;
	}
//	printf("aspectRatio1: %f ", aspectRatio);
	aspectRatio = (TCFloat)(1.0f / tan(1.0f / aspectRatio)) * aspectAdjust;
	aspectRatio = 1.0f;
//	printf("aspectRatio2: %f\n", aspectRatio);
	nClip = zDistance - clipRadius * aspectRatio + clipAmount * aspectRatio *
		clipRadius;
	if (nClip < size / 1000.0f)
	{
		nClip = size / 1000.0f;
	}
	fClip = zDistance + clipRadius * aspectRatio;
	setFieldOfView(currentFov, nClip, fClip);
	glLoadIdentity();
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, (GLfloat)nClip);
	glFogf(GL_FOG_END, (GLfloat)fClip);
}

void LDrawModelViewer::perspectiveViewToClipPlane(void)
{
	TCFloat nClip;
	TCFloat fClip;
	TCFloat zDistance = getZDistance();
//	TCFloat zDistance = (camera.getPosition()).length();
	TCFloat clipRadius = getClipRadius();
//	TCFloat distance = (camera.getPosition() - center).length();

	nClip = zDistance - size / 2.0f;
//	fClip = distance - size * aspectRatio / 2.0f + clipAmount * aspectRatio *
//		size;
	fClip = zDistance - clipRadius * aspectRatio + clipAmount * aspectRatio *
		clipRadius;
	if (fClip < size / 1000.0f)
	{
		fClip = size / 1000.0f;
	}
	if (nClip < size / 1000.0f)
	{
		nClip = size / 1000.0f;
	}
	setFieldOfView(currentFov, nClip, fClip);
	glLoadIdentity();
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, (GLfloat)nClip);
	glFogf(GL_FOG_END, (GLfloat)fClip);
}

/*
void LDrawModelViewer::setProgressCallback(LDMProgressCallback callback,
										   void* userData)
{
	progressCallback = callback;
	progressUserData = userData;
}

void LDrawModelViewer::setErrorCallback(LDMErrorCallback callback,
										void* userData)
{
	errorCallback = callback;
	errorUserData = userData;
}
*/

bool LDrawModelViewer::skipCameraPositioning(void)
{
	return defaultRotationMatrix && (defaultRotationMatrix[12] != 0.0f ||
		defaultRotationMatrix[13] != 0.0f || defaultRotationMatrix[14] != 0.0f);
}

TCFloat LDrawModelViewer::calcDefaultDistance(void)
{
	if (flags.overrideDefaultDistance)
	{
		return defaultDistance;
	}
	// Note that the margin is on all sides, so it's on two edges per axis,
	// which is why we multiply by 2.
	TCFloat margin = getWideLineMargin() * 2.0f;
	TCFloat marginAdjust = 1.0f;

	if (margin != 0.0f)
	{
		int actualWidth = width / (int)getStereoWidthModifier() * numXTiles;
		int actualHeight = height * numYTiles;

		if (actualWidth < actualHeight)
		{
			marginAdjust = (actualHeight + margin) / actualHeight;
		}
		else
		{
			marginAdjust = (actualWidth + margin) / actualWidth;
		}
	}
	return (TCFloat)(size / 2.0 / sin(deg2rad(fov / 2.0))) * distanceMultiplier
		* marginAdjust;
/*
	double angle1 = deg2rad(90.0f - (currentFov / 2.0));
	double angle2 = deg2rad(currentFov / 4.0);
	double radius = size / 2.0;

	return (TCFloat)(radius * tan(angle1) + radius * tan(angle2)) *
		distanceMultiplier;
*/
}

void LDrawModelViewer::resetView(LDVAngle viewAngle)
{
	flags.needsViewReset = false;
	flags.autoCenter = true;
	if (!mainTREModel)
	{
		return;
	}
	if (clipAmount != 0.0f)
	{
		clipAmount = 0.0f;
		perspectiveView();
	}
	defaultDistance = calcDefaultDistance();
	if (!skipCameraPositioning())
	{
		// If the user specifies a rotation matrix that includes a translation,
		// then don't move the camera.
		camera.setPosition(TCVector(0.0f, 0.0f, defaultDistance));
	}
	camera.setFacing(TREFacing());
	if (!rotationMatrix)
	{
		rotationMatrix = new TCFloat[16];
	}
	switch (viewAngle)
	{
	case LDVAngleDefault:
		setupDefaultViewAngle();
		break;
	case LDVAngleFront:
		setupFrontViewAngle();
		break;
	case LDVAngleBack:
		setupBackViewAngle();
		break;
	case LDVAngleLeft:
		setupLeftViewAngle();
		break;
	case LDVAngleRight:
		setupRightViewAngle();
		break;
	case LDVAngleTop:
		setupTopViewAngle();
		break;
	case LDVAngleBottom:
		setupBottomViewAngle();
		break;
	case LDVAngleIso:
		setupIsoViewAngle();
		break;
	}
	flags.needsRotationMatrixSetup = true;
	xPan = 0.0f;
	yPan = 0.0f;
	perspectiveView(true);
}

void LDrawModelViewer::setModelCenter(const TCFloat *value)
{
	if (value)
	{
		flags.overrideModelCenter = true;
		center = TCVector(value[0],value[1],value[2]);
		flags.needsSetup = true;
				
	}

}
void LDrawModelViewer::setModelSize(const TCFloat value)
{
	if (value)
	{
		flags.overrideModelSize = true;
		size = value;
		flags.needsSetup = true;
				
	}

}

void LDrawModelViewer::setDefaultDistance(TCFloat value)
{
	if (value > 0)
	{
		defaultDistance = value;
		flags.overrideDefaultDistance = true;
	}
}

void LDrawModelViewer::setDefaultRotationMatrix(const TCFloat *value)
{
	if (value)
	{
		if (!defaultRotationMatrix || memcmp(defaultRotationMatrix, value,
			16 * sizeof(TCFloat)) != 0)
		{
			delete defaultRotationMatrix;
			defaultRotationMatrix = new TCFloat[16];
			memcpy(defaultRotationMatrix, value, 16 * sizeof(TCFloat));
			flags.needsSetup = true;
		}
	}
	else if (defaultRotationMatrix)
	{
		delete defaultRotationMatrix;
		defaultRotationMatrix = NULL;
		flags.needsSetup = true;
	}
}

void LDrawModelViewer::setupDefaultViewAngle(void)
{
	if (defaultRotationMatrix)
	{
		memcpy(rotationMatrix, defaultRotationMatrix, 16 * sizeof(TCFloat));
	}
	else
	{
		setupIsoViewAngle();
	}
}

void LDrawModelViewer::setupIsoViewAngle(void)
{
	rotationMatrix[0] = (TCFloat)(sqrt(2.0) / 2.0);
	rotationMatrix[1] = (TCFloat)(sqrt(2.0) / 4.0);
	rotationMatrix[2] = (TCFloat)(-sqrt(1.5) / 2.0);
	rotationMatrix[3] = 0.0f;
	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = (TCFloat)(sin(M_PI / 3.0));
	rotationMatrix[6] = 0.5f;
	rotationMatrix[7] = 0.0f;
	rotationMatrix[8] = (TCFloat)(sqrt(2.0) / 2.0);
	rotationMatrix[9] = (TCFloat)(-sqrt(2.0) / 4.0);
	rotationMatrix[10] = (TCFloat)(sqrt(1.5) / 2.0);
	rotationMatrix[11] = 0.0f;
	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupFrontViewAngle(void)
{
	rotationMatrix[0] = 1.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = 0.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 1.0f;
	rotationMatrix[6] = 0.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = 0.0f;
	rotationMatrix[9] = 0.0f;
	rotationMatrix[10] = 1.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupBackViewAngle(void)
{
	rotationMatrix[0] = -1.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = 0.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 1.0f;
	rotationMatrix[6] = 0.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = 0.0f;
	rotationMatrix[9] = 0.0f;
	rotationMatrix[10] = -1.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupLeftViewAngle(void)
{
	rotationMatrix[0] = 0.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = -1.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 1.0f;
	rotationMatrix[6] = 0.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = 1.0f;
	rotationMatrix[9] = 0.0f;
	rotationMatrix[10] = 0.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupRightViewAngle(void)
{
	rotationMatrix[0] = 0.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = 1.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 1.0f;
	rotationMatrix[6] = 0.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = -1.0f;
	rotationMatrix[9] = 0.0f;
	rotationMatrix[10] = 0.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupTopViewAngle(void)
{
	rotationMatrix[0] = 1.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = 0.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 0.0f;
	rotationMatrix[6] = 1.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = 0.0f;
	rotationMatrix[9] = -1.0f;
	rotationMatrix[10] = 0.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

void LDrawModelViewer::setupBottomViewAngle(void)
{
	rotationMatrix[0] = 1.0f;
	rotationMatrix[1] = 0.0f;
	rotationMatrix[2] = 0.0f;
	rotationMatrix[3] = 0.0f;

	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = 0.0f;
	rotationMatrix[6] = -1.0f;
	rotationMatrix[7] = 0.0f;

	rotationMatrix[8] = 0.0f;
	rotationMatrix[9] = 1.0f;
	rotationMatrix[10] = 0.0f;
	rotationMatrix[11] = 0.0f;

	rotationMatrix[12] = 0.0f;
	rotationMatrix[13] = 0.0f;
	rotationMatrix[14] = 0.0f;
	rotationMatrix[15] = 1.0f;
}

/*
void LDrawModelViewer::ldlErrorCallback(LDLError *error)
{
//	static int errorCount = 0;

	if (error)
	{
		TCStringArray *extraInfo = error->getExtraInfo();

		if (getDebugLevel() > 0)
		{
			printf("Error on line %d in: %s\n", error->getLineNumber(),
				error->getFilename());
			indentPrintf(4, "%s\n", error->getMessage());
			indentPrintf(4, "%s\n", error->getFileLine());
		}
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
}

void LDrawModelViewer::progressAlertCallback(TCProgressAlert *alert)
{
	if (alert)
	{
		printf("Progress message from %s:\n%s (%f)\n", alert->getSource(),
			alert->getMessage(), alert->getProgress());
	}
}
*/

bool LDrawModelViewer::haveLightDats(void)
{
	return flags.drawLightDats && mainTREModel &&
		mainTREModel->getLightLocations().size() > 0;
}

bool LDrawModelViewer::haveStandardLight(void)
{
	return flags.useLighting && (!haveLightDats() || !flags.optionalStandardLight);
}

bool LDrawModelViewer::forceOneLight(void)
{
	return flags.oneLight || flags.usesSpecular || !flags.defaultLightVector ||
		(flags.bfc && (flags.redBackFaces | flags.greenFrontFaces)) ||
		haveLightDats();
}

int LDrawModelViewer::loadModel(bool resetViewpoint)
{
	int retValue = 0;

	if (filename && filename[0])
	{
		TCObject::release(mainModel);
		mainModel = new LDLMainModel;
		if (clipAmount != 0.0f && resetViewpoint)
		{
			clipAmount = 0.0f;
			perspectiveView();
		}

		// First, release the current TREModel, if it exists.
		TCObject::release(mainTREModel);
		mainTREModel = NULL;
		TCObject::release(lightDirModel);
		lightDirModel = NULL;
		mainModel->setLowResStuds(!flags.qualityStuds);
		mainModel->setBlackEdgeLines(flags.blackHighlights);
		mainModel->setExtraSearchDirs(extraSearchDirs);
		mainModel->setProcessLDConfig(flags.processLDConfig);
		mainModel->setSkipValidation(flags.skipValidation);
		if (mainModel->load(filename))
		{
			LDModelParser *modelParser = new LDModelParser;

			switch (memoryUsage)
			{
			case 0:
				modelParser->setCompilePartsFlag(false);
				modelParser->setCompileAllFlag(false);
				break;
			case 1:
				modelParser->setCompilePartsFlag(true);
				modelParser->setCompileAllFlag(false);
				break;
			case 2:
				modelParser->setCompilePartsFlag(true);
				modelParser->setCompileAllFlag(true);
				break;
			}
			modelParser->setSeamWidth(seamWidth);
			modelParser->setPrimitiveSubstitutionFlag(
				flags.allowPrimitiveSubstitution);
			modelParser->setCurveQuality(curveQuality);
			modelParser->setLightingFlag(flags.useLighting);
			modelParser->setTwoSidedLightingFlag(forceOneLight());
			modelParser->setNoLightGeomFlag(flags.noLightGeom);
			modelParser->setAALinesFlag(flags.lineSmoothing);
			modelParser->setSortTransparentFlag(flags.sortTransparent);
			modelParser->setStippleFlag(flags.useStipple);
			modelParser->setWireframeFlag(flags.drawWireframe);
			modelParser->setBFCFlag(flags.bfc);
			modelParser->setRedBackFacesFlag(flags.redBackFaces);
			modelParser->setGreenFrontFacesFlag(flags.greenFrontFaces);
			if (flags.showsHighlightLines)
			{
				// Note that the default for all of these is false.
				modelParser->setEdgeLinesFlag(true);
				modelParser->setConditionalLinesFlag(
					flags.drawConditionalHighlights);
				modelParser->setShowAllConditionalFlag(
					flags.showAllConditionalLines);
				modelParser->setConditionalControlPointsFlag(
					flags.showConditionalControlPoints);
				modelParser->setEdgesOnlyFlag(flags.edgesOnly);
			}
			modelParser->setSmoothCurvesFlag(flags.performSmoothing);
			modelParser->setFileIsPartFlag(flags.fileIsPart);
			modelParser->setStudLogoFlag(flags.textureStuds);
			modelParser->setDefaultRGB(defaultR, defaultG, defaultB,
				flags.defaultTrans);
			modelParser->setPolygonOffsetFlag(flags.usePolygonOffset);
			modelParser->setEdgeLineWidth(highlightLineWidth);
			modelParser->setStudTextureFilter(textureFilterType);
			modelParser->setStudAnisoLevel(anisoLevel);
			if (defaultColorNumber != -1)
			{
				modelParser->setDefaultColorNumber(defaultColorNumber);
			}
			if (modelParser->parseMainModel(mainModel))
			{
				bool abort;

				mainTREModel = modelParser->getMainTREModel();
				mainTREModel->retain();
				TCProgressAlert::send("LDrawModelViewer",
					TCLocalStrings::get(_UC("CalculatingSizeStatus")), 0.0f,
					&abort);
				if (!abort)
				{
					mainTREModel->getBoundingBox(boundingMin, boundingMax);
					TCProgressAlert::send("LDrawModelViewer",
						TCLocalStrings::get(_UC("CalculatingSizeStatus")), 0.5f,
						&abort);
				}
				if (!abort)
				{
					if (!flags.overrideModelCenter)
					{
						center = (boundingMin + boundingMax) / 2.0f;
					}
					if (!flags.overrideModelSize)
					{
						size = mainTREModel->getMaxRadius(center) * 2.0f;
					}
					TCProgressAlert::send("LDrawModelViewer",
						TCLocalStrings::get(_UC("CalculatingSizeStatus")), 1.0f,
						&abort);
				}
				if (!abort)
				{
					flags.needsRecompile = false;
					retValue = 1;
				}
			}
			modelParser->release();
		}
		initLightDirModel();
		TCProgressAlert::send("LDrawModelViewer", TCLocalStrings::get(
			_UC("Done")), 2.0f);
		if (resetViewpoint)
		{
			resetView();
		}
	}
	// This shouldn't be necessary, but something is occasionally setting the
	// needsReload flag during loading.
	flags.needsReload = false;
	flags.needsLightingSetup = true;
	return retValue;
}

void LDrawModelViewer::setShowsHighlightLines(bool value)
{
	if (value != flags.showsHighlightLines)
	{
		flags.showsHighlightLines = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setDrawConditionalHighlights(bool value)
{
	if (flags.drawConditionalHighlights != value)
	{
		flags.drawConditionalHighlights = value;
		if (flags.showsHighlightLines)
		{
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::setPerformSmoothing(bool value)
{
	if (flags.performSmoothing != value)
	{
		flags.performSmoothing = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setForceZoomToFit(bool value)
{
	if (flags.forceZoomToFit != value)
	{
		flags.forceZoomToFit = value;
	}
}

void LDrawModelViewer::setSkipValidation(bool value)
{
	if (flags.skipValidation != value)
	{
		flags.skipValidation = value;
		if (!value)
		{
			// If they turn the flag on, there's no point reloading.  If, on the
			// other hand, they turn the flag off, we need to reload so that we
			// can work around any problems the model might have.
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::setProcessLDConfig(bool value)
{
	if (flags.processLDConfig != value)
	{
		flags.processLDConfig = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setMemoryUsage(int value)
{
	if (value < 0 || value > 2)
	{
		value = 2;
	}
	if (value != memoryUsage)
	{
		memoryUsage = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setLineSmoothing(bool value)
{
	if (flags.lineSmoothing != value)
	{
		flags.lineSmoothing = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setSubduedLighting(bool value)
{
	if (value != flags.subduedLighting)
	{
		flags.subduedLighting = value;
		flags.needsLightingSetup = true;
	}
}

bool LDrawModelViewer::recompile(void)
{
	if (mainTREModel)
	{
		if (lightDirModel)
		{
			// It crashes after an FSAA change if we simply recompile here.
			// I suspect it's related to the VBO/VAR extensions.  Deleting the
			// lightDirModel object and recreating it causes the vertex buffers
			// to be reloaded from scratch for both models.
			lightDirModel->release();
			lightDirModel = NULL;
			initLightDirModel();
		}
		mainTREModel->recompile();
		flags.needsRecompile = false;
		TCProgressAlert::send("LDrawModelViewer", TCLocalStrings::get(
			_UC("Done")), 2.0f);
	}
	return true;
}

void LDrawModelViewer::uncompile(void)
{
	if (fontListBase)
	{
		glDeleteLists(fontListBase, 128);
		fontListBase = 0;
	}
	if (mainTREModel)
	{
		mainTREModel->uncompile();
	}
	if (lightDirModel)
	{
		lightDirModel->uncompile();
	}
}

void LDrawModelViewer::reload(void)
{
	if (filename)
	{
		loadModel(false);
	}
	flags.needsReload = false;
}

void LDrawModelViewer::setUsesSpecular(bool value)
{
	if (value != flags.usesSpecular)
	{
		flags.usesSpecular = value;
		flags.needsMaterialSetup = true;
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setOneLight(bool value)
{
	if (value != flags.oneLight)
	{
		flags.oneLight = value;
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setupMaterial(void)
{
//	GLfloat mAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat mAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
//	GLfloat mSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
//	GLfloat mSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};

	// Note: default emission is <0,0,0,1>, which is what we want.
	if (!flags.usesSpecular)
	{
		mSpecular[0] = mSpecular[1] = mSpecular[2] = 0.0f;
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	flags.needsMaterialSetup = false;
}

void LDrawModelViewer::setupLight(GLenum light, const TCVector &color)
{
	GLfloat lDiffuse[4];
	GLfloat lSpecular[4];
	int i;

	if (flags.subduedLighting)
	{
		for (i = 0; i < 3; i++)
		{
			lDiffuse[i] = color.get(i) * 0.5f;
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			lDiffuse[i] = color.get(i);
		}
	}
	if (!flags.usesSpecular)
	{
		lSpecular[0] = lSpecular[1] = lSpecular[2] = 0.0f;
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			lSpecular[i] = color.get(i);
		}
	}
	lDiffuse[3] = 1.0f;
	lSpecular[3] = 1.0f;
	glLightfv(light, GL_SPECULAR, lSpecular);
	glLightfv(light, GL_DIFFUSE, lDiffuse);
	glEnable(light);
}

void LDrawModelViewer::setupLighting(void)
{
	glDisable(GL_NORMALIZE);
	if (flags.useLighting)
	{
		GLint maxLights;
		int i;
		bool lightDats = haveLightDats();
		GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};

		if (flags.subduedLighting)
		{
			ambient[0] = ambient[1] = ambient[2] = 0.7f;
		}
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glEnable(GL_LIGHTING);
		glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
		for (i = 0; i < maxLights; i++)
		{
			glDisable(GL_LIGHT0 + i);
		}
		if (!lightDats || !flags.optionalStandardLight)
		{
			setupLight(GL_LIGHT0);
			glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
			glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
			glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
		}
		if (lightDats)
		{
			const TCULongList &lightColors = mainTREModel->getLightColors();
			TCULongList::const_iterator itColor = lightColors.begin();
			float scale = 1.0f / 255.0f;
			float atten = (float)sqr(size);
			int start = 0;

			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
			if (!flags.optionalStandardLight)
			{
				start = 1;
			}
			for (i = start; i < maxLights && itColor != lightColors.end(); i++)
			{
				TCByte rgb[4];
				TCULong color = htonl(*itColor);
				float minBrightness = 0.5f;
				float brightness;

				memcpy(rgb, &color, 4);
				brightness = std::max(std::max(rgb[0] * scale,
					rgb[1] * scale), rgb[2] * scale) * (1.0f - minBrightness) +
					minBrightness;
				setupLight(GL_LIGHT0 + i, TCVector(rgb[0] * scale,
					rgb[1] * scale, rgb[2] * scale));
				glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0.5f);
				glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.0f);//1.0f / size);
				glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 2.0f / atten /
					brightness);
				itColor++;
			}
		}
		else
		{
			if (forceOneLight())
			{
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
			}
			else
			{
				setupLight(GL_LIGHT1);
				glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
			}
		}
		flags.needsLightingSetup = false;
	}
	else
	{
		glDisable(GL_LIGHTING);
		flags.needsLightingSetup = false;
	}
}

void LDrawModelViewer::setFontData(TCByte *fontData, long length)
{
	if (length == 4096)
	{
		int i, j;
		TCByte *imageData = NULL;
		int rowSize;
		int imageSize;

		fontImage = new TCImage;
		fontImage->setFlipped(true);
		fontImage->setLineAlignment(4);
		fontImage->setDataFormat(TCRgba8);
		fontImage->setSize(FONT_IMAGE_WIDTH, FONT_IMAGE_HEIGHT);
		fontImage->allocateImageData();
		imageData = fontImage->getImageData();
		rowSize = fontImage->getRowSize();
		imageSize = rowSize * FONT_IMAGE_HEIGHT;
		for (i = 0; i < imageSize; i++)
		{
			if (i % 4 == 3)
			{
				imageData[i] = 0x00;	// Init alpha channel to clear.
			}
			else
			{
				imageData[i] = 0xFF;	// Init color channels to white.
			}
		}
		for (i = 0; i < 4096; i++)
		{
			int row = i / 256 * 16 + i % 16;	// logical row (kinda)
			int col = i / 16 % 16;				// logical column
			int yOffset = (FONT_IMAGE_HEIGHT - row - 1) * rowSize;
			TCByte fontByte = fontData[i];

			for (j = 0; j < 8; j++)
			{
				if (fontByte & (1 << (7 - j)))
				{
					int offset = yOffset + (col * FONT_CHAR_WIDTH + j) * 4 + 3;

					// That spot should be on, so set the bit.
					if (offset < 131072)
					{
						imageData[offset] = 0xFF;
					}
					else
					{
						printf("Huh?\n");
					}
				}
			}
		}
	}
}

// Loads a font file in the format of VGA text-mode font data.
void LDrawModelViewer::loadVGAFont(char *fontFilename)
{
	if (!fontImage)
	{
		FILE *fontFile = fopen(fontFilename, "rb");

		if (fontFile)
		{
			TCByte fontData[4096];

			if (fread(fontData, 1, 4096, fontFile) == 4096)
			{
				setFontData(fontData, 4096);
			}
			fclose(fontFile);
		}
	}
}

void LDrawModelViewer::setupFont(char *fontFilename)
{
//	printf("LDrawModelViewer::setupFont\n");
	if (fontFilename)
	{
		loadVGAFont(fontFilename);
	}
	if (fontImage)
	{
		int i;

		glGenTextures(1, &fontTextureID);
		glBindTexture(GL_TEXTURE_2D, fontTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, FONT_IMAGE_WIDTH, FONT_IMAGE_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE, fontImage->getImageData());
		if (fontListBase)
		{
			glDeleteLists(fontListBase, FONT_NUM_CHARACTERS);
		}
		fontListBase = glGenLists(FONT_NUM_CHARACTERS);
		for (i = 0; i < FONT_NUM_CHARACTERS; i++)
		{
			TCFloat cx, cy;
			TCFloat wx, hy;
			TCFloat tx, ty;

			cx = (TCFloat)(i % 16) * FONT_CHAR_WIDTH /
				(TCFloat)(FONT_IMAGE_WIDTH);
			cy = (TCFloat)(i / 16) * FONT_CHAR_HEIGHT /
				(TCFloat)(FONT_IMAGE_HEIGHT);
			wx = (TCFloat)FONT_CHAR_WIDTH / FONT_IMAGE_WIDTH;
			hy = (TCFloat)FONT_CHAR_HEIGHT / FONT_IMAGE_HEIGHT;
			glNewList(fontListBase + i, GL_COMPILE);
				glBegin(GL_QUADS);
					tx = cx;
					ty = 1.0f - cy - hy;
					treGlTexCoord2f(tx, ty);			// Bottom Left
					glVertex2i(0, 0);
					tx = cx + wx;
					ty = 1.0f - cy - hy;
					treGlTexCoord2f(tx, ty);			// Bottom Right
					glVertex2i(FONT_CHAR_WIDTH, 0);
					tx = cx + wx;
					ty = 1 - cy;
					treGlTexCoord2f(tx, ty);			// Top Right
					glVertex2i(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
					tx = cx;
					ty = 1 - cy;
					treGlTexCoord2f(tx , ty);			// Top Left
					glVertex2i(0, FONT_CHAR_HEIGHT);
				glEnd();
				glTranslated(FONT_CHAR_WIDTH + 1, 0, 0);
			glEndList();
		}
	}
}

void LDrawModelViewer::setupTextures(void)
{
	if (programPath)
	{
		char textureFilename[1024];

		sprintf(textureFilename, "%s/StudLogo.png", programPath);
		TREMainModel::loadStudTexture(textureFilename);
//		sprintf(textureFilename, "%s/Font.png", programPath);
		sprintf(textureFilename, "%s/SansSerif.fnt", programPath);
		setupFont(textureFilename);
	}
	else if (fontImage)
	{
		setupFont(NULL);
	}
	flags.needsTextureSetup = false;
}

void LDrawModelViewer::setup(void)
{
	glEnable(GL_DEPTH_TEST);
	setupLighting();
	setupMaterial();
	setupTextures();
	flags.needsSetup = false;
}

void LDrawModelViewer::drawBoundingBox(void)
{
	int lightingEnabled = glIsEnabled(GL_LIGHTING);

	if (lightingEnabled)
	{
		glDisable(GL_LIGHTING);
	}
	glColor3ub(255, 255, 255);
	glBegin(GL_LINE_STRIP);
		treGlVertex3fv(boundingMin);
		treGlVertex3f(boundingMax[0], boundingMin[1], boundingMin[2]);
		treGlVertex3f(boundingMax[0], boundingMax[1], boundingMin[2]);
		treGlVertex3f(boundingMin[0], boundingMax[1], boundingMin[2]);
		treGlVertex3fv(boundingMin);
		treGlVertex3f(boundingMin[0], boundingMin[1], boundingMax[2]);
		treGlVertex3f(boundingMax[0], boundingMin[1], boundingMax[2]);
		treGlVertex3fv(boundingMax);
		treGlVertex3f(boundingMin[0], boundingMax[1], boundingMax[2]);
		treGlVertex3f(boundingMin[0], boundingMin[1], boundingMax[2]);
	glEnd();
	glBegin(GL_LINES);
		treGlVertex3f(boundingMin[0], boundingMax[1], boundingMin[2]);
		treGlVertex3f(boundingMin[0], boundingMax[1], boundingMax[2]);
		treGlVertex3f(boundingMax[0], boundingMax[1], boundingMin[2]);
		treGlVertex3f(boundingMax[0], boundingMax[1], boundingMax[2]);
		treGlVertex3f(boundingMax[0], boundingMin[1], boundingMin[2]);
		treGlVertex3f(boundingMax[0], boundingMin[1], boundingMax[2]);
	glEnd();
	if (lightingEnabled)
	{
		glEnable(GL_LIGHTING);
	}
}

void LDrawModelViewer::orthoView(void)
{
	int actualWidth = width;
	const char *glVendor = "";
	const GLubyte *origVendorString = glGetString(GL_VENDOR);

	if (origVendorString)
	{
		glVendor = (const char *)origVendorString;
	}
	if (stereoMode == LDVStereoCrossEyed || stereoMode == LDVStereoParallel)
	{
		glViewport(0, 0, width, height);
//		actualWidth = width / 2;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, actualWidth, 0.0, height);
//	gluOrtho2D(-0.5, actualWidth - 0.5, -0.5, height - 0.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (strncmp(glVendor, "ATI Technologies Inc.", 3) != 0)
	{
		// This doesn't work right on ATI video cards, so skip.
		treGlTranslatef(0.375f, 0.375f, 0.0f);
		//debugPrintf("Not an ATI.\n");
	}
}

void LDrawModelViewer::setSeamWidth(TCFloat value)
{
	if (!fEq(value, seamWidth))
	{
		seamWidth = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::drawString(TCFloat xPos, TCFloat yPos, char* string)
{
	if (!fontListBase)
	{
		setupTextures();
	}
	if (!fontListBase)
	{
		// Font file wasn't found; bail.
		return;
	}
	if ((backgroundR + backgroundG + backgroundB) / 3.0f > 0.5)
	{
		glColor3ub(0, 0, 0);
	}
	else
	{
		glColor3ub(255, 255, 255);
	}
	orthoView();
	treGlTranslatef(xPos, yPos, 0);
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, fontTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glListBase(fontListBase);
	glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
	glPopAttrib();
	perspectiveView();
}

void LDrawModelViewer::drawFPS(TCFloat fps)
{
	if (mainTREModel)
	{
		char fpsString[1024];
		int lightingEnabled = glIsEnabled(GL_LIGHTING);
		int zBufferEnabled = glIsEnabled(GL_DEPTH_TEST);
		//TCFloat xMult = (TCFloat)width / (TCFloat)height;

		if (lightingEnabled)
		{
			glDisable(GL_LIGHTING);
		}
		if (zBufferEnabled)
		{
			glDisable(GL_DEPTH_TEST);
		}
		if (fps > 0.0f)
		{
			sprintf(fpsString, "%4.4f", fps);
		}
		else
		{
			// NOTE: Unicode NOT supported for this string.
			strcpy(fpsString, TCLocalStrings::get("FPSSpinPromptGL"));
/*
			for (int i = 0; i < 128; i++)
			{
				fpsString[i] = i + 64;//28;
			}
			fpsString[i] = 0;
*/
		}
		drawString(2.0f, 0.0f, fpsString);
		if (lightingEnabled)
		{
			glEnable(GL_LIGHTING);
		}
		if (zBufferEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
	}
}

void LDrawModelViewer::drawLight(GLenum light, TCFloat x, TCFloat y, TCFloat z)
{
	GLfloat position[4];
	GLfloat direction[4];
	//TCFloat fullIntensity[] = {1.0f, 1.0f, 1.0f, 1.0f};

	position[0] = (GLfloat)x;
	position[1] = (GLfloat)y;
	position[2] = (GLfloat)z;
	position[3] = 0.0f;
	direction[0] = (GLfloat)-x;
	direction[1] = (GLfloat)-y;
	direction[2] = (GLfloat)-z;
	direction[3] = 0.0f;
	glLightfv(light, GL_POSITION, position);
	//glLightfv(light, GL_SPOT_DIRECTION, direction);
//	glLightfv(light, GL_DIFFUSE, fullIntensity);
//	glLightfv(light, GL_SPECULAR, fullIntensity);
}

void LDrawModelViewer::drawLights(void)
{
//	drawLight(GL_LIGHT1, 0.0f, 0.0f, -10000.0f);
	drawLight(GL_LIGHT0, lightVector[0], lightVector[1], lightVector[2]);
	if (!forceOneLight())
	{
		drawLight(GL_LIGHT1, -lightVector[0], -lightVector[1], -lightVector[2]);
	}
}

void LDrawModelViewer::setLightVector(const TCVector &value)
{
	TCFloat length = value.length();
	bool oldForce = forceOneLight();

	if (length)
	{
		lightVector = value / length;
	}
	if (lightVector.approxEquals(TCVector(0.0f, 0.0f, 1.0f), 0.000001f))
	{
		flags.defaultLightVector = true;
	}
	else
	{
		flags.defaultLightVector = false;
	}
	if (forceOneLight() != oldForce)
	{
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setBackgroundRGB(int r, int g, int b)
{
	setBackgroundRGBA(r, g, b, 255);
/*
	backgroundR = (GLclampf)r / 255.0f;
	backgroundG = (GLclampf)g / 255.0f;
	backgroundB = (GLclampf)b / 255.0f;
	backgroundA = 1.0f;
*/
}

void LDrawModelViewer::setDefaultRGB(TCByte r, TCByte g, TCByte b,
									 bool transparent)
{
	if (defaultR != r || defaultG != g || defaultB != b ||
		flags.defaultTrans != transparent)
	{
		defaultR = r;
		defaultG = g;
		defaultB = b;
		flags.defaultTrans = transparent;
		if (mainTREModel && defaultColorNumber == -1)
		{
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::getDefaultRGB(TCByte &r, TCByte &g, TCByte &b,
									 bool &transparent)
{
	r = defaultR;
	g = defaultG;
	b = defaultB;
	transparent = flags.defaultTrans;
}

void LDrawModelViewer::setDefaultColorNumber(int value)
{
	if (value != defaultColorNumber)
	{
		defaultColorNumber = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setBackgroundRGBA(int r, int g, int b, int a)
{
	backgroundR = (GLclampf)r / 255.0f;
	backgroundG = (GLclampf)g / 255.0f;
	backgroundB = (GLclampf)b / 255.0f;
	backgroundA = (GLclampf)a / 255.0f;
}

void LDrawModelViewer::setRedBackFaces(bool value)
{
	if (value != flags.redBackFaces)
	{
		flags.redBackFaces = value;
		if (flags.bfc)
		{
			flags.needsReload = true;
			flags.needsLightingSetup = true;
		}
	}
}

void LDrawModelViewer::setGreenFrontFaces(bool value)
{
	if (value != flags.greenFrontFaces)
	{
		flags.greenFrontFaces = value;
		if (flags.bfc)
		{
			flags.needsReload = true;
			flags.needsLightingSetup = true;
		}
	}
}

void LDrawModelViewer::setBfc(bool value)
{
	if (value != flags.bfc)
	{
		flags.bfc = value;
		flags.needsReload = true;
		if (flags.redBackFaces || flags.greenFrontFaces)
		{
			flags.needsMaterialSetup = true;
			flags.needsLightingSetup = true;
		}
	}
}

void LDrawModelViewer::setDrawWireframe(bool value)
{
	if (value != flags.drawWireframe)
	{
		flags.drawWireframe = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setUseWireframeFog(bool value)
{
	flags.useWireframeFog = value;
}

void LDrawModelViewer::setDrawLightDats(bool value)
{
	if (value != flags.drawLightDats)
	{
		flags.drawLightDats = value;
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setOptionalStandardLight(bool value)
{
	if (value != flags.optionalStandardLight)
	{
		flags.optionalStandardLight = value;
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setNoLightGeom(bool value)
{
	if (value != flags.noLightGeom)
	{
		flags.noLightGeom = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setRemoveHiddenLines(bool value)
{
	if (value != flags.removeHiddenLines)
	{
		flags.removeHiddenLines = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setEdgesOnly(bool value)
{
	if (value != flags.edgesOnly)
	{
		flags.edgesOnly = value;
	}
	if (mainTREModel)
	{
		bool realValue = flags.edgesOnly && flags.showsHighlightLines;

		if (realValue != mainTREModel->getEdgesOnlyFlag())
		{
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::setHiResPrimitives(bool value)
{
	if (value != flags.hiResPrimitives)
	{
		flags.hiResPrimitives = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setUsePolygonOffset(bool value)
{
	if (value != flags.usePolygonOffset)
	{
		flags.usePolygonOffset = value;
		if (mainTREModel)
		{
			mainTREModel->setPolygonOffsetFlag(flags.usePolygonOffset);
			if (!flags.usePolygonOffset)
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
			}
		}
	}
}

void LDrawModelViewer::setBlackHighlights(bool value)
{
	if (value != flags.blackHighlights)
	{
		flags.blackHighlights = value;
		if (flags.showsHighlightLines)
		{
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::setShowAllConditionalLines(bool value)
{
	if (value != flags.showAllConditionalLines)
	{
		flags.showAllConditionalLines = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setShowConditionalControlPoints(bool value)
{
	if (value != flags.showConditionalControlPoints)
	{
		flags.showConditionalControlPoints = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setCurveQuality(int value)
{
	if (value != curveQuality)
	{
		curveQuality = value;
		if (flags.allowPrimitiveSubstitution)
		{
			flags.needsReload = true;
		}
	}
}

void LDrawModelViewer::setTextureStuds(bool value)
{
	if (value != flags.textureStuds)
	{
		flags.textureStuds = value;
//		TGLStudLogo::setTextureStuds(value);
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setTextureFilterType(int value)
{
	if (value != textureFilterType)
	{
		textureFilterType = value;
		if (mainTREModel)
		{
			mainTREModel->setStudTextureFilter(value);
		}
//		TGLStudLogo::setTextureFilterType(value);
//		flags.needsRecompile = true;
//		flags.needsTextureSetup = true;
	}
}

void LDrawModelViewer::setWidth(int value)
{
	if (value != width)
	{
		width = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setHeight(int value)
{
	if (value != height)
	{
		height = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setXTile(int value)
{
	if (value != xTile)
	{
		xTile = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setYTile(int value)
{
	if (value != yTile)
	{
		yTile = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setNumXTiles(int value)
{
	if (value != numXTiles)
	{
		numXTiles = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setNumYTiles(int value)
{
	if (value != numYTiles)
	{
		numYTiles = value;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setStereoMode(LDVStereoMode mode)
{
	if (mode != stereoMode)
	{
		stereoMode = mode;
		flags.needsResize = true;
	}
}

void LDrawModelViewer::setCutawayMode(LDVCutawayMode mode)
{
	cutawayMode = mode;
}

void LDrawModelViewer::setCutawayLineWidth(TCFloat32 value)
{
	cutawayLineWidth = value;
}

void LDrawModelViewer::setCutawayAlpha(TCFloat32 value)
{
	cutawayAlpha = value;
}

void LDrawModelViewer::setUseLighting(bool value)
{
	if (value != flags.useLighting)
	{
		flags.useLighting = value;
		if (mainTREModel)
		{
			mainTREModel->setLightingFlag(value);
		}
		flags.needsRecompile = true;
		flags.needsLightingSetup = true;
	}
}

void LDrawModelViewer::setUseStipple(bool value)
{
	if (value != flags.useStipple)
	{
		flags.useStipple = value;
		if (mainTREModel)
		{
			mainTREModel->setStippleFlag(value);
		}
		if (flags.sortTransparent)
		{
			flags.needsReload = true;
		}
		else
		{
//			flags.needsRecompile = true;
		}
	}
}

void LDrawModelViewer::setSortTransparent(bool value)
{
	if (value != flags.sortTransparent)
	{
		flags.sortTransparent = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setHighlightLineWidth(TCFloat32 value)
{
	if (value != highlightLineWidth)
	{
		highlightLineWidth = value;
		if (mainTREModel)
		{
			mainTREModel->setEdgeLineWidth(value);
		}
	}
}

void LDrawModelViewer::setWireframeLineWidth(TCFloat32 value)
{
	wireframeLineWidth = value;
}

void LDrawModelViewer::setAnisoLevel(TCFloat32 value)
{
	if (value != anisoLevel)
	{
		anisoLevel = value;
		if (mainTREModel)
		{
			mainTREModel->setStudAnisoLevel(anisoLevel);
		}
	}
}

void LDrawModelViewer::setQualityStuds(bool value)
{
	if (value != flags.qualityStuds)
	{
		flags.qualityStuds = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::setAllowPrimitiveSubstitution(bool value)
{
	if (value != flags.allowPrimitiveSubstitution)
	{
		flags.allowPrimitiveSubstitution = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::updateCameraPosition(void)
{
	camera.move(cameraMotion * size / 100.0f);
	camera.rotate(TCVector(cameraXRotate, cameraYRotate, cameraZRotate));
}

void LDrawModelViewer::zoom(TCFloat amount, bool apply)
{
	if (flags.paused)
	{
		return;
	}
	if (clipZoom)
	{
		nextClipAmount = clipAmount - amount / 1000.0f;

		if (nextClipAmount > aspectRatio * 2)
		{
			nextClipAmount = aspectRatio * 2;
		}
/*
		if (nextClipAmount > 1.0f)
		{
			nextClipAmount = 1.0f;
		}
*/
		else if (nextClipAmount < 0.0f)
		{
			nextClipAmount = 0.0f;
		}
	}
	else
	{
		TCFloat distance = (camera.getPosition()).length();
//		TCFloat distance = (camera.getPosition() - center).length();

		nextDistance = distance + (amount * distance / 300.0f);
		if (flags.constrainZoom && !skipCameraPositioning())
		{
			if (nextDistance < size / zoomMax)
			{
				nextDistance = size / zoomMax;
			}
		}
		// We may as well always constrain the maximum zoom, since there not
		// really any reason to move too far away.
		if (nextDistance > defaultDistance * 10.0f && !skipCameraPositioning())
		{
			nextDistance = defaultDistance * 10.0f;
		}
	}
	if (apply)
	{
		applyZoom();
	}
}

void LDrawModelViewer::applyZoom(void)
{
	if (flags.paused)
	{
		return;
	}
	if (clipZoom)
	{
		if (!fEq(clipAmount, nextClipAmount))
		{
			clipAmount = nextClipAmount;
			perspectiveView(false);
		}
	}
	else
	{
		TCFloat distance = (camera.getPosition()).length();

		if (!fEq(distance, nextDistance))
		{
			camera.move(TCVector(0.0f, 0.0f, nextDistance - distance));
//			distance = newDistance;
//			camera.move(TCVector(0.0f, 0.0f, amount * size / 300.0f));
			perspectiveView(false);
		}
	}
}

void LDrawModelViewer::clearBackground(void)
{
	GLfloat backgroundColor[3];

	if (cullBackFaces)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	glClearDepth(1.0);
	backgroundColor[0] = (GLfloat)backgroundR;
	backgroundColor[1] = (GLfloat)backgroundG;
	backgroundColor[2] = (GLfloat)backgroundB;
	glFogfv(GL_FOG_COLOR, backgroundColor);
	if (flags.slowClear)
	{
		GLint oldDepthFunc;
		bool oldBlendEnabled = false;
		bool oldPolygonOffsetEnabled = false;
		bool oldLightingEnabled = false;

		orthoView();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
		glDepthFunc(GL_ALWAYS);
		glColor4f(backgroundR, backgroundG, backgroundB, backgroundA);
		if (glIsEnabled(GL_BLEND))
		{
			glDisable(GL_BLEND);
			oldBlendEnabled = true;
		}
		if (glIsEnabled(GL_POLYGON_OFFSET_FILL))
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			oldPolygonOffsetEnabled = true;
		}
		if (glIsEnabled(GL_LIGHTING))
		{
			glDisable(GL_LIGHTING);
			oldLightingEnabled = true;
		}
		glDepthMask(1);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -1);
		glVertex3i(width + 1, -1, -1);
		glVertex3i(width + 1, height + 1, -1);
		glVertex3i(-1, height + 1, -1);
		glEnd();
		glDepthFunc(oldDepthFunc);
		if (oldBlendEnabled)
		{
			glEnable(GL_BLEND);
		}
		if (oldPolygonOffsetEnabled)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
		}
		if (oldLightingEnabled)
		{
			glEnable(GL_LIGHTING);
		}
		perspectiveView();
	}
	else
	{
		glClearColor(backgroundR, backgroundG, backgroundB, backgroundA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);
	}
	if (flags.drawWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(wireframeLineWidth);
		if (flags.useWireframeFog)
		{
			glEnable(GL_FOG);
		}
		else
		{
			glDisable(GL_FOG);
		}
	}
	else
	{
		glLineWidth(highlightLineWidth);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_FOG);
	}
}

void LDrawModelViewer::drawSetup(TCFloat eyeXOffset)
{
	glLoadIdentity();
	if (flags.qualityLighting)
	{
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	}
	else
	{
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
	}
	if (flags.usesFlatShading)
	{
		glShadeModel(GL_FLAT);
	}
	else
	{
		glShadeModel(GL_SMOOTH);
	}
	drawLights();
	glLoadIdentity();
	if (mainTREModel)
	{
		applyZoom();
//		camera.move(cameraMotion * size / 100.0f);
		perspectiveView(false);
//		camera.rotate(TCVector(cameraXRotate, cameraYRotate, cameraZRotate));
		camera.project(TCVector(-eyeXOffset - xPan, -yPan, 0.0f));
//		treGlTranslatef(eyeXOffset + xPan, yPan, -distance);
	}
	else
	{
		camera.project(TCVector(-eyeXOffset - xPan, -yPan, 10.0f));
//		treGlTranslatef(eyeXOffset + xPan, yPan, -10.0f);
	}
}

void LDrawModelViewer::drawToClipPlaneUsingStencil(TCFloat eyeXOffset)
{
	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
		GL_LINE_BIT | GL_STENCIL_BUFFER_BIT);
	perspectiveViewToClipPlane();
	glDisable(GL_DEPTH_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, ~0u);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(cutawayLineWidth);
	glDisable(GL_FOG);
	glLoadIdentity();
	camera.project(TCVector(-eyeXOffset - xPan, -yPan, 0.0f));
//	treGlTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		treGlRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		treGlMultMatrixf(rotationMatrix);
		treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		treGlMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		treGlTranslatef(-center[0], -center[1], -center[2]);
	}
	showLight();
	mainTREModel->draw();
	glDisable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if ((backgroundR + backgroundG + backgroundB) / 3.0f > 0.5)
	{
		glColor4f(0.0f, 0.0f, 0.0f, cutawayAlpha);
	}
	else
	{
		glColor4f(1.0f, 1.0f, 1.0f, cutawayAlpha);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_EQUAL, 1, ~0u);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glRectf(0.0, 0.0, 1.0, 1.0);
	perspectiveView();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

/*
void LDrawModelViewer::drawToClipPlaneUsingAccum(GLfloat eyeXOffset)
{
	TCFloat weight = 0.25f;
	TCFloat oldZoomSpeed = zoomSpeed;

	glReadBuffer(GL_BACK);
	glAccum(GL_LOAD, 1.0f - weight);
	glClearColor(backgroundR, backgroundG, backgroundB, backgroundA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	zoomSpeed = 0.0f;
	clearBackground();
	drawSetup();
	zoomSpeed = oldZoomSpeed;
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		treGlMultMatrixf(rotationMatrix);
		treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		treGlMultMatrixf(rotationMatrix);
	}
	treGlTranslatef(-center[0], -center[1], -center[2]);
	glColor3ub(192, 192, 192);
	mainTREModel->draw();

	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	perspectiveViewToClipPlane();
	glClearDepth(1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(cutawayLineWidth);
	glDisable(GL_FOG);
	glLoadIdentity();
	treGlTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		treGlMultMatrixf(rotationMatrix);
		treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		treGlMultMatrixf(rotationMatrix);
	}
	treGlTranslatef(-center[0], -center[1], -center[2]);
	mainTREModel->draw();
	perspectiveView();
	glAccum(GL_ACCUM, weight);
	glAccum(GL_RETURN, 1.0f);
}
*/

void LDrawModelViewer::drawToClipPlaneUsingDestinationAlpha(TCFloat eyeXOffset)
{
	TCFloat32 weight = cutawayAlpha;

	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
		GL_LINE_BIT);
	perspectiveViewToClipPlane();
	glClearDepth(1.0);
	glClearColor(weight, weight, weight, weight);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(cutawayLineWidth);
	glDisable(GL_FOG);
	glLoadIdentity();
	camera.project(TCVector(-eyeXOffset - xPan, -yPan, 0.0f));
//	treGlTranslatef(eyeXOffset + xPan, yPan, -distance);
	//treGlTranslatef(0.0f, 0.0f, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		treGlRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		treGlMultMatrixf(rotationMatrix);
		treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		treGlMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		treGlTranslatef(-center[0], -center[1], -center[2]);
	}
	showLight();
	mainTREModel->setCutawayDrawFlag(true);
	mainTREModel->draw();
	mainTREModel->setCutawayDrawFlag(false);
	perspectiveView();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_BLEND);
	glPopAttrib();
}

void LDrawModelViewer::drawToClipPlaneUsingNoEffect(TCFloat eyeXOffset)
{
	glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
		GL_LINE_BIT);
	perspectiveViewToClipPlane();
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(cutawayLineWidth);
	glDisable(GL_FOG);
	glLoadIdentity();
	camera.project(TCVector(-eyeXOffset - xPan, -yPan, 0.0f));
//	treGlTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		treGlRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		treGlMultMatrixf(rotationMatrix);
		treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		treGlMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		treGlTranslatef(-center[0], -center[1], -center[2]);
	}
	showLight();
	mainTREModel->draw();
	perspectiveView();
	glPopAttrib();
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void LDrawModelViewer::drawToClipPlane(TCFloat eyeXOffset)
{
	switch (cutawayMode)
	{
	case LDVCutawayNormal:
		// Don't do anything
		break;
	case LDVCutawayWireframe:
		if (fEq(cutawayAlpha, 1.0f))
		{
			drawToClipPlaneUsingNoEffect(eyeXOffset);
		}
		else
		{
			drawToClipPlaneUsingDestinationAlpha(eyeXOffset);
		}
		break;
	case LDVCutawayStencil:
		drawToClipPlaneUsingStencil(eyeXOffset);
		break;
	}
//	drawToClipPlaneUsingAccum(eyeXOffset);
}

void LDrawModelViewer::clear(void)
{
	glClearStencil(0);
	glClearDepth(1.0);
	glClearColor(backgroundR, backgroundG, backgroundB, backgroundA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool LDrawModelViewer::getLDrawCommandLineMatrix(char *matrixString,
												 int bufferLength)
{
	if (rotationMatrix && bufferLength)
	{
		char buf[1024];
		TCFloat matrix[16];
		int i;
		TCVector point;
		
		if (flags.autoCenter)
		{
			point = -center;
		}
		for (i = 0; i < 16; i++)
		{
			if (fEq(rotationMatrix[i], 0.0f))
			{
				matrix[i] = 0.0f;
			}
			else if (i == 0 || i  == 4 || i == 8)
			{
				matrix[i] = rotationMatrix[i];
			}
			else
			{
				matrix[i] = -rotationMatrix[i];
			}
		}
		point = point.transformPoint(matrix);
		sprintf(buf, "-a%.2g,%.2g,%.2g,%.2g,%.2g,%.2g,%.2g,%.2g,%.2g",
			matrix[0], matrix[4], matrix[8],
			matrix[1], matrix[5], matrix[9],
			matrix[2], matrix[6], matrix[10]);
		strncpy(matrixString, buf, bufferLength);
		matrixString[bufferLength - 1] = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool LDrawModelViewer::getLDGLiteCommandLine(char *commandString,
											 int bufferLength)
{
	if (rotationMatrix && bufferLength)
	{
		char buf[1024];
		char matrixString[512];
		TCVector cameraPoint = camera.getPosition();
//		TCVector cameraPoint = TCVector(0.0f, 0.0f, distance);
		TCVector lookAt;

		if (flags.autoCenter)
		{
			lookAt = center;
		}
		int i;
		TCFloat transformationMatrix[16];

		if (!getLDrawCommandLineMatrix(matrixString, 512))
		{
			return false;
		}
		TCVector::invertMatrix(rotationMatrix, transformationMatrix);
		lookAt = lookAt.transformPoint(rotationMatrix);
		cameraPoint += lookAt;
//		cameraPoint = TGLShape::transformPoint(cameraPoint, rotationMatrix);
		for (i = 0; i < 3; i++)
		{
			if (fEq(cameraPoint[i], 0.0f))
			{
				cameraPoint[i] = 0.0f;
			}
		}
		sprintf(buf, "ldglite -J -v%d,%d "
			"-cc%.4f,%.4f,%.4f -co%.4f,%.4f,%.4f "
			"-cu0,1,0 %s \"%s\"", width, height,
			cameraPoint[0], cameraPoint[1], cameraPoint[2],
			lookAt[0], lookAt[1], lookAt[2],
			matrixString, filename);
		strncpy(commandString, buf, bufferLength);
		commandString[bufferLength - 1] = 0;
		return true;
	}
	return false;
}

bool LDrawModelViewer::getLDrawCommandLine(char *shortFilename,
										   char *commandString,
										   int bufferLength)
{
	if (rotationMatrix && bufferLength)
	{
		char buf[1024];
		TCFloat matrix[16];
		int i;
		TCVector point;
		TCFloat scaleFactor = 500.0f;
		TCFloat distance = (camera.getPosition()).length();
//		TCFloat distance = (camera.getPosition() - center).length();

		if (flags.autoCenter)
		{
			point = -center;
		}
		for (i = 0; i < 16; i++)
		{
			if (fEq(rotationMatrix[i], 0.0f))
			{
				matrix[i] = 0.0f;
			}
			else if (i == 0 || i  == 4 || i == 8)
			{
				matrix[i] = rotationMatrix[i];
			}
			else
			{
				matrix[i] = -rotationMatrix[i];
			}
		}
		point = point.transformPoint(matrix);
		sprintf(buf, "ldraw -s%.4g -o%d,%d "
			"-a%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g,%.4g "
			"%s",
			scaleFactor / distance,
			(int)(point[0] * scaleFactor / distance),
			(int)(point[1] * scaleFactor / distance),
			matrix[0], matrix[4], matrix[8],
			matrix[1], matrix[5], matrix[9],
			matrix[2], matrix[6], matrix[10],
			shortFilename);
		strncpy(commandString, buf, bufferLength);
		commandString[bufferLength - 1] = 0;
		return true;
	}
	else
	{
		return false;
	}
}

void LDrawModelViewer::update(void)
{
	static GLubyte stipplePattern[128];
	static bool stipplePatternSet = false;
	TCFloat eyeXOffset = 0.0f;

	flags.updating = true;
	if (!stipplePatternSet)
	{
		int i;

		for (i = 0; i < 32; i++)
		{
			if (i % 2)
			{
				memset(stipplePattern + i * 4, 0xAA, 4);
			}
			else
			{
				memset(stipplePattern + i * 4, 0x55, 4);
			}
		}
		stipplePatternSet = true;
	}
	if (!rotationMatrix)
	{
		rotationMatrix = new TCFloat[16];
		setupDefaultViewAngle();
		flags.needsRotationMatrixSetup = true;
	}
	if (flags.needsSetup)
	{
		setup();
	}
	if (flags.needsTextureSetup)
	{
		setupTextures();
	}
	if (flags.needsLightingSetup)
	{
		setupLighting();
	}
	if (flags.needsMaterialSetup)
	{
		setupMaterial();
	}
	if (flags.needsReload)
	{
		reload();
	}
	else if (flags.needsRecompile)
	{
		recompile();
	}
	if (flags.needsViewReset)
	{
		perspectiveView();
		resetView();
	}
	if (flags.needsResize)
	{
		perspectiveView();
	}
	glPolygonStipple(stipplePattern);
	if (flags.needsRotationMatrixSetup)
	{
		setupRotationMatrix();
	}
	clearBackground();
	if (!mainTREModel)
	{
		flags.updating = false;
		return;
	}
/*
	if (!mainTREModel->getCompiled() && (mainTREModel->getCompileAllFlag() ||
		mainTREModel->getCompilePartsFlag()))
	{
		if (!mainTREModel->getCompiling())
		{
			drawString(2.0f, height - 16.0f, "Model Compile Canceled");
		}
		return;
	}
*/
	if (stereoMode == LDVStereoCrossEyed || stereoMode == LDVStereoParallel)
	{
		TCFloat distance = (camera.getPosition()).length();
//		TCFloat distance = (camera.getPosition() - center).length();

		eyeXOffset = stereoEyeSpacing * 2.0f / (TCFloat)pow((double)distance,
			0.25);
		if (stereoMode == LDVStereoCrossEyed)
		{
			eyeXOffset = -eyeXOffset;
		}
	}
	if (rotationMatrix)
	{
		if (!flags.paused)
		{
			TCFloat matrix[16];
			TCVector rotation = TCVector(xRotate, yRotate, zRotate);

			camera.getFacing().getInverseMatrix(matrix);
			glPushMatrix();
			glLoadIdentity();
			rotation = rotation.mult(matrix);
//			printf("[%f %f %f] [%f %f %f]\n", xRotate, yRotate, zRotate,
//				rotation[0], rotation[1], rotation[2]);
//			treGlRotatef(rotationSpeed, xRotate, yRotate, zRotate);
			treGlRotatef(rotationSpeed, rotation[0], rotation[1], rotation[2]);
			treGlMultMatrixf(rotationMatrix);
			treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
			glPopMatrix();
		}
	}
	updateCameraPosition();
	zoom(zoomSpeed, false);
	if (flags.drawWireframe && flags.removeHiddenLines)
	{
		removeHiddenLines();
		drawModel(0.0f);
	}
	else
	{
		drawModel(eyeXOffset);
	}
	if (stereoMode == LDVStereoCrossEyed || stereoMode == LDVStereoParallel)
	{
		eyeXOffset = -eyeXOffset;
		glViewport(width / 2, 0, width / 2, height);
		if (flags.slowClear)
		{
			clearBackground();
			glViewport(width / 2, 0, width / 2, height);
		}
		if (flags.drawWireframe && flags.removeHiddenLines)
		{
			removeHiddenLines();
			drawModel(0.0f);
		}
		else
		{
			drawModel(eyeXOffset);
		}
		glViewport(0, 0, width / 2, height);
	}
	flags.updating = false;
}

void LDrawModelViewer::removeHiddenLines(TCFloat eyeXOffset)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	mainTREModel->setLightingFlag(false);
	mainTREModel->setRemovingHiddenLines(true);
	if (flags.usePolygonOffset)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(POLYGON_OFFSET_FACTOR, POLYGON_OFFSET_UNITS);
	}
	drawModel(eyeXOffset);
	// Not sure why the following is necessary.
	glLineWidth(wireframeLineWidth);
	if (flags.usePolygonOffset)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, 0.0f);
	}
	mainTREModel->setRemovingHiddenLines(false);
	mainTREModel->setLightingFlag(flags.useLighting);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void LDrawModelViewer::setupRotationMatrix(void)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	treGlRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	treGlRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	treGlMultMatrixf(rotationMatrix);
	treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	glPopMatrix();
	flags.needsRotationMatrixSetup = false;
/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	treGlRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	treGlRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	treGlRotatef(50.0f, 0.0f, -1.5f, 1.0f);
	treGlGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	glPopMatrix();
*/
}

void LDrawModelViewer::initLightDirModel(void)
{
	if (lightDirModel == NULL)
	{
		TREModel *subModel = new TREModel;
		lightDirModel = new TREMainModel;
		TCFloat identityMatrix[16];
		TCULong color = LDLPalette::colorForRGBA(255, 255, 255, 255);
		float length = size / 1.5f;
		float radius = size / 100.0f;
		int segments = 32;
		float coneLength = radius * 4.0f;
		float coneRadius = radius * 2.0f;
		float offset = length / 3.0f;

		lightDirModel->setLightingFlag(true);
		TCVector::initIdentityMatrix(identityMatrix);
		subModel->setMainModel(lightDirModel);
		subModel->addCylinder(TCVector(0.0f, coneLength + offset, 0.0f), radius, length - coneLength - offset, segments);
		subModel->addDisc(TCVector(0.0, coneLength + offset, 0.0), coneRadius, segments);
		subModel->addDisc(TCVector(0.0, length, 0.0), radius, segments);
		subModel->addCone(TCVector(0.0, coneLength + offset, 0.0), coneRadius, -coneLength, segments);
		lightDirModel->addSubModel(color, color, identityMatrix, subModel, false);
		subModel->release();
		lightDirModel->postProcess();
	}
}

void LDrawModelViewer::showLight(void)
{
	if (flags.showLight)
	{
		TCVector oldLightVector = lightVector;
		TCFloat rotInverse[16];
		TREFacing facing;
		bool oldSpecular = flags.usesSpecular;
		bool oldSubdued = flags.subduedLighting;

		TCVector::invertMatrix(rotationMatrix, rotInverse);
		flags.usesSpecular = false;
		flags.subduedLighting = false;
		glPushMatrix();
		glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
			GL_LINE_BIT | GL_LIGHTING_BIT);
		facing.setFacing(TCVector(1.0f, 0.0f, 0.0f), (TCFloat)M_PI / 2.0f);
		facing.pointAt(oldLightVector);
		treGlTranslatef(center[0], center[1], center[2]);
		treGlMultMatrixf(rotInverse);
		treGlMultMatrixf(camera.getFacing().getMatrix());
		lightVector = TCVector(0.0f, 0.0f, 1.0f);
		setupLight(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		glPushMatrix();
		glLoadIdentity();
		drawLights();
		glPopMatrix();
		lightVector = oldLightVector;
		treGlMultMatrixf(facing.getMatrix());
		lightDirModel->draw();
		glPopAttrib();
		glPopMatrix();
		flags.usesSpecular = oldSpecular;
		flags.subduedLighting = oldSubdued;
	}
}

void LDrawModelViewer::drawLightDats(void)
{
	if (haveLightDats())
	{
		const TCVectorList &lightLocs = mainTREModel->getLightLocations();
		GLint maxLights;

		glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
		if (lightLocs.size() > 0)
		{
			int i;
			TCVectorList::const_iterator itLoc = lightLocs.begin();
			GLfloat position[4];
			int start = 0;

			if (!flags.optionalStandardLight)
			{
				start = 1;
			}
			position[3] = 1.0f;
			for (i = start; i < maxLights && itLoc != lightLocs.end(); i++)
			{
				const TCVector &lightLoc = *itLoc;

				position[0] = (GLfloat)lightLoc.get(0);
				position[1] = (GLfloat)lightLoc.get(1);
				position[2] = (GLfloat)lightLoc.get(2);
				glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
				itLoc++;
			}
		}
	}
}

void LDrawModelViewer::drawModel(TCFloat eyeXOffset)
{
	drawSetup(eyeXOffset);
	if (rotationMatrix)
	{
		treGlMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		treGlTranslatef(-center[0], -center[1], -center[2]);
	}
	drawLightDats();
//	drawBoundingBox();
	glColor3ub(192, 192, 192);
	if (mainTREModel)
	{
		showLight();
		mainTREModel->draw();
		if (clipAmount > 0.01)
		{
			drawToClipPlane(eyeXOffset);
		}
	}
}

void LDrawModelViewer::pause(void)
{
	flags.paused = true;
}

void LDrawModelViewer::unpause(void)
{
	flags.paused = false;
}

void LDrawModelViewer::setRotationSpeed(TCFloat value)
{
	rotationSpeed = value;
	if (value)
	{
		flags.paused = false;
	}
}

void LDrawModelViewer::setZoomSpeed(TCFloat value)
{
	zoomSpeed = value;
	if (value)
	{
		flags.paused = false;
	}
}

void LDrawModelViewer::setExtraSearchDirs(TCStringArray *value)
{
	if (extraSearchDirs != value)
	{
		TCObject::release(extraSearchDirs);
		extraSearchDirs = value;
		TCObject::retain(extraSearchDirs);
	}
	// Since it is a string array, the contents might have changed, even if
	// the array pointer itself didn't.
	flags.needsReload = true;
}

void LDrawModelViewer::panXY(int xValue, int yValue)
{
	TCFloat adjustment;
	TCFloat distance = (camera.getPosition()).length();
//	TCFloat distance = (camera.getPosition() - center).length();

	if (width > height)
	{
		adjustment = (TCFloat)width;
	}
	else
	{
		adjustment = (TCFloat)height;
	}
//	xPan += xValue / (TCFloat)pow(distance, 0.001);
//	yPan -= yValue / (TCFloat)pow(distance, 0.001);
	xPan += xValue / adjustment / (TCFloat)pow(2.0 / distance, 1.1) *
		(TCFloat)(sin(deg2rad(fov)) / sin(deg2rad(45.0)));
	yPan -= yValue / adjustment / (TCFloat)pow(2.0 / distance, 1.1) *
		(TCFloat)(sin(deg2rad(fov)) / sin(deg2rad(45.0)));
}

void LDrawModelViewer::setXYPan(TCFloat xValue, TCFloat yValue)
{
	xPan = xValue;
	yPan = yValue;
}

void LDrawModelViewer::openGlWillEnd(void)
{
	if (mainTREModel)
	{
		mainTREModel->openGlWillEnd();
	}
}

bool LDrawModelViewer::getCompiled(void)
{
	if (mainTREModel)
	{
		return mainTREModel->getCompiled();
	}
	else
	{
		return false;
	}
}

bool LDrawModelViewer::connectionFailure(TCWebClient *webClient)
{
	// I'm not sure if we should add more errors here or not.  Hopefully this is
	// enough.  In reality, the hostname lookup will fail if the user doesn't
	// have an internet connection.  If they have a mis-configured proxy, they
	// should either get TCNCE_CONNECT or TCNCE_CONNECTION_REFUSED.
	switch (webClient->getErrorNumber())
	{
	case TCNCE_HOSTNAME_LOOKUP:
	case TCNCE_NO_PORT:
	case TCNCE_CONNECT:
	case TCNCE_CONNECTION_REFUSED:
		return true;
	default:
		return false;
	}
}

void LDrawModelViewer::findFileAlertCallback(LDLFindFileAlert *alert)
{
	char *filename = copyString(alert->getFilename());
	int len = strlen(filename);
	char *url;
	char *partOutputFilename = copyString(LDLModel::lDrawDir(), len + 32);
	char *primitiveOutputFilename = copyString(LDLModel::lDrawDir(), len + 32);
	bool primitive = false;
	bool part = false;
	//const char *partUrlBase = "http://media.peeron.com/tmp/";
	const char *partUrlBase = "http://www.ldraw.org/library/unofficial/parts/";
	const char *primitiveUrlBase = "http://www.ldraw.org/library/unofficial/p/";
	bool found = false;
	char *key = new char[strlen(filename) + 128];

	replaceStringCharacter(partOutputFilename, '\\', '/');
	replaceStringCharacter(primitiveOutputFilename, '\\', '/');
	strcat(partOutputFilename, "/Unofficial/parts/");
	strcat(primitiveOutputFilename, "/Unofficial/p/");
	convertStringToLower(filename);
	replaceStringCharacter(filename, '\\', '/');
	if (stringHasPrefix(filename, "48/"))
	{
		primitive = true;
		url = copyString(primitiveUrlBase, len + 2);
	}
	else
	{
		if (stringHasPrefix(filename, "s/"))
		{
			// The only thing this is used for is to prevent it from checking
			// for the file as a primitive if it's not found as a part.
			part = true;
		}
		url = copyString(partUrlBase, len + 2);
	}
	strcat(partOutputFilename, filename);
	strcat(primitiveOutputFilename, filename);
	if (fileExists(partOutputFilename))
	{
		primitive = false;
		found = true;
		alert->setPartFlag(true);
	}
	else if (!part && fileExists(primitiveOutputFilename))
	{
		primitive = true;
		found = true;
		delete url;
		url = copyString(primitiveUrlBase, len + 2);
	}
	if (canCheckForUnofficialPart(filename, found))
	{
		TCWebClient *webClient;
		// FIX: dynamically allocate and use local string AND handle abort
		UCCHAR message[1024];
		bool abort;
		UCSTR ucFilename = mbstoucstring(filename);

		sprintf(key, "UnofficialPartChecks/%s/LastModified", filename);
		if (found)
		{
			sucprintf(message, COUNT_OF(message),
				TCLocalStrings::get(_UC("CheckingForUpdates")), ucFilename);
		}
		else
		{
			sucprintf(message, COUNT_OF(message),
				TCLocalStrings::get(_UC("TryingToDownload")), ucFilename);
		}
		delete ucFilename;
		TCProgressAlert::send("LDrawModelViewer", message, -1.0f, &abort);
		strcat(url, filename);
		webClient = new TCWebClient(url);
		if (found)
		{
			char *lastModified = TCUserDefaults::stringForKey(key, NULL, false);

			if (lastModified)
			{
				webClient->setLastModifiedString(lastModified);
				delete lastModified;
			}
		}
		if (primitive)
		{
			*strrchr(primitiveOutputFilename, '/') = 0;
			webClient->setOutputDirectory(primitiveOutputFilename);
			primitiveOutputFilename[strlen(primitiveOutputFilename)] = '/';
		}
		else
		{
			*strrchr(partOutputFilename, '/') = 0;
			webClient->setOutputDirectory(partOutputFilename);
			partOutputFilename[strlen(partOutputFilename)] = '/';
		}
		if (webClient->fetchURL() ||
			webClient->getErrorNumber() == WCE_NOT_MODIFIED)
		{
			found = true;
			if (!primitive)
			{
				alert->setPartFlag(true);
			}
		}
		else if (connectionFailure(webClient))
		{
			// If we had a connection failure, we probably don't have an
			// internet connection, or our proxy is mis-configured.  Don't try
			// to connect again for now, and let the user know that auto part
			// updates have been disabled.
			TCAlert *alert = new TCAlert(alertClass(),
				TCLocalStrings::get(_UC("PartCheckDisabled")));

			preferences->setCheckPartTracker(false, true);
			flags.checkPartTracker = false;
			TCAlertManager::sendAlert(alert);
			alert->release();
		}
		else
		{
			if (!primitive && !part)
			{
				// We don't know if it's a primitive or a part.  The part
				// download failed, so try as a primitive.
				delete url;
				url = copyString(primitiveUrlBase, len + 2);
				strcat(url, filename);
				webClient->release();
				webClient = new TCWebClient(url);
				*strrchr(primitiveOutputFilename, '/') = 0;
				webClient->setOutputDirectory(primitiveOutputFilename);
				primitiveOutputFilename[strlen(primitiveOutputFilename)] = '/';
				if (webClient->fetchURL() ||
					webClient->getErrorNumber() == WCE_NOT_MODIFIED)
				{
					primitive = true;
					found = true;
				}
			}
		}
		if (webClient->getLastModifiedString())
		{
			TCUserDefaults::setStringForKey(
				webClient->getLastModifiedString(), key, false);
		}
		webClient->release();
		sprintf(key, "UnofficialPartChecks/%s/LastUpdateCheckTime",
			filename);
		TCUserDefaults::setLongForKey((long)time(NULL), key, false);
		if (!found)
		{
			unofficialPartNotFound(filename);
		}
	}
	if (found)
	{
		alert->setFileFound(true);
		if (primitive)
		{
			alert->setFilename(primitiveOutputFilename);
		}
		else
		{
			alert->setFilename(partOutputFilename);
		}
		setUnofficialPartPrimitive(filename, primitive);
	}
	delete key;
	delete filename;
	delete url;
	delete partOutputFilename;
	delete primitiveOutputFilename;
}

LDPartsList *LDrawModelViewer::getPartsList(void)
{
	if (mainModel)
	{
		LDPartsList *partsList = new LDPartsList;

		partsList->scanModel(mainModel, defaultColorNumber);
		return partsList;
	}
	else
	{
		return NULL;
	}
}

// NOTE: static function
bool LDrawModelViewer::fileExists(char* filename)
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

// NOTE: static function
void LDrawModelViewer::setUnofficialPartPrimitive(const char *filename,
												  bool primitive)
{
	char *key = new char[strlen(filename) + 128];

	sprintf(key, "UnofficialPartChecks/%s/Primitive", filename);
	TCUserDefaults::setLongForKey(primitive ? 1 : 0, key, false);
	delete key;
}

LDViewPoint *LDrawModelViewer::saveViewPoint(void) const
{
	LDViewPoint *viewPoint = new LDViewPoint;

	viewPoint->setCamera(camera);
	viewPoint->setRotation(TCVector(xRotate, yRotate, zRotate));
	viewPoint->setCameraRotation(TCVector(cameraXRotate, cameraYRotate,
		cameraZRotate));
	viewPoint->setPan(TCVector(xPan, yPan, 0.0f));
	viewPoint->setRotationMatrix(rotationMatrix);
	viewPoint->setRotationSpeed(rotationSpeed);
	viewPoint->setAutoCenter(flags.autoCenter != false);
	viewPoint->setBackgroundColor(backgroundR, backgroundG, backgroundB,
		backgroundA);
	viewPoint->setStereoMode(stereoMode);
	viewPoint->setClipAmount(clipAmount);
	viewPoint->setDefaultDistance(defaultDistance);
	return viewPoint;
}

void LDrawModelViewer::restoreViewPoint(const LDViewPoint *viewPoint)
{
	TCVector tempVector;

	camera = viewPoint->getCamera();
	tempVector = viewPoint->getRotation();
	xRotate = tempVector[0];
	yRotate = tempVector[1];
	zRotate = tempVector[2];
	tempVector = viewPoint->getCameraRotation();
	cameraXRotate = tempVector[0];
	cameraYRotate = tempVector[1];
	cameraZRotate = tempVector[2];
	tempVector = viewPoint->getPan();
	xPan = tempVector[0];
	yPan = tempVector[1];
	memcpy(rotationMatrix, viewPoint->getRotationMatrix(),
		16 * sizeof(rotationMatrix[0]));
	rotationSpeed = viewPoint->getRotationSpeed();
	flags.autoCenter = viewPoint->getAutoCenter();
	viewPoint->getBackgroundColor(backgroundR, backgroundG, backgroundB,
		backgroundA);
	stereoMode = viewPoint->getStereoMode();
	clipAmount = viewPoint->getClipAmount();
	defaultDistance = viewPoint->getDefaultDistance();
}

bool LDrawModelViewer::canCheckForUnofficialPart(const char *filename,
												 bool exists)
{
	bool retValue = false;

	if (flags.checkPartTracker)
	{
		char *key = new char[strlen(filename) + 128];
		time_t lastCheck;
		time_t now = time(NULL);
		int days;

		if (exists)
		{
			sprintf(key, "UnofficialPartChecks/%s/LastUpdateCheckTime",
				filename);
			days = updatedPartWait;
		}
		else
		{
			sprintf(key, "UnofficialPartChecks/%s/LastCheckTime", filename);
			days = missingPartWait;
		}
		lastCheck = (time_t)TCUserDefaults::longForKey(key, 0, false);
		if (days < 1)
		{
			days = 1;
		}
		if (now - lastCheck > 24 * 3600 * days)
		{
			retValue = true;
		}
		delete key;
	}
	return retValue;
}

void LDrawModelViewer::unofficialPartNotFound(const char *filename)
{
	if (flags.checkPartTracker)
	{
		char *key = new char[strlen(filename) + 128];
		time_t now = time(NULL);

		sprintf(key, "UnofficialPartChecks/%s/LastCheckTime", filename);
		TCUserDefaults::setLongForKey((long)now, key, false);
		delete key;
	}
}

// NOTE: static function
void LDrawModelViewer::cleanupFloats(TCFloat *array, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (fabs(array[i]) < 1e-6)
		{
			array[i] = 0.0f;
		}
	}
}

void LDrawModelViewer::getPovCameraInfo(UCCHAR *&userMessage, char *&povCamera)
{
	TCFloat tmpMatrix[16];
	TCFloat matrix[16];
	TCFloat rotationMatrix[16];
	TCFloat centerMatrix[16];
	TCFloat positionMatrix[16];
	TCFloat cameraMatrix[16];
	TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
	UCCHAR locationString[1024];
	UCCHAR lookAtString[1204];
	UCCHAR upString[1024];
	UCCHAR message[4096];
	TCVector directionVector = TCVector(0.0f, 0.0f, 1.0f);
	TCVector locationVector;
	TCVector lookAtVector;
	TCVector upVector = TCVector(0.0f, -1.0f, 0.0f);
	double direction[3];
	double up[3];
	double location[3];
	TREFacing facing;
	UCCHAR cameraString[4096];
	double lookAt[3];
	double tempV[3];

	if (!mainTREModel)
	{
		userMessage = NULL;
		povCamera = NULL;
		return;
	}
	TRECamera &camera = getCamera();
	TCVector cameraPosition = camera.getPosition();
	TCVector boundingMin, boundingMax, center;

	memcpy(rotationMatrix, getRotationMatrix(), sizeof(rotationMatrix));
	getMainTREModel()->getBoundingBox(boundingMin, boundingMax);
	center = (boundingMin + boundingMax) / 2.0f;
	TCVector::initIdentityMatrix(positionMatrix);
	positionMatrix[12] = cameraPosition[0] - getXPan();
	positionMatrix[13] = -cameraPosition[1] + getYPan();
	positionMatrix[14] = -cameraPosition[2];
	TCVector::initIdentityMatrix(centerMatrix);
	if (getAutoCenter())
	{
		centerMatrix[12] = center[0];
		centerMatrix[13] = center[1];
		centerMatrix[14] = center[2];
	}
	TCVector::multMatrix(otherMatrix, rotationMatrix, tmpMatrix);
	TCVector::invertMatrix(tmpMatrix, cameraMatrix);
	TCVector::multMatrix(centerMatrix, cameraMatrix, tmpMatrix);
	TCVector::multMatrix(tmpMatrix, positionMatrix, matrix);

	facing = camera.getFacing();
	facing[0] = -facing[0];
	facing.getInverseMatrix(cameraMatrix);
	TCVector::multMatrix(matrix, cameraMatrix, tmpMatrix);
	memcpy(matrix, tmpMatrix, sizeof(matrix));
	cleanupFloats(matrix);
	locationVector = TCVector(matrix[12], matrix[13], matrix[14]);
	location[0] = (double)matrix[12];
	location[1] = (double)matrix[13];
	location[2] = (double)matrix[14];
	cleanupFloats(matrix);
	// Note that the location accuracy isn't nearly as important as the
	// directional accuracy, so we don't have to re-do this string prior
	// to putting it on the clipboard in the POV code copy.
	sucprintf(locationString, COUNT_OF(locationString), _UC("%g,%g,%g"),
		location[0], location[1], location[2]);

	matrix[12] = matrix[13] = matrix[14] = 0.0f;
	directionVector = directionVector.transformPoint(matrix);
	upVector = upVector.transformPoint(matrix);
	// Grab the values prior to normalization.  That will make the
	// normalization more accurate in double precision.
	directionVector.upConvert(direction);
	lookAtVector = locationVector + directionVector *
		locationVector.length();
	upVector.upConvert(up);
	directionVector = directionVector.normalize();
	upVector = upVector.normalize();
	cleanupFloats(directionVector, 3);
	cleanupFloats(upVector, 3);
	// The following 3 strings will get re-done later at higher accuracy
	// for POV-Ray.
	sucprintf(lookAtString, COUNT_OF(lookAtString), _UC("%g,%g,%g"),
		lookAtVector[0], lookAtVector[1], lookAtVector[2]);
	sucprintf(upString, COUNT_OF(upString), _UC("%g,%g,%g"), upVector[0],
		upVector[1], upVector[2]);
	sucprintf(message, COUNT_OF(message),
		TCLocalStrings::get(_UC("PovCameraMessage")), locationString,
		lookAtString, upString);
	TCVector::doubleNormalize(up);
	TCVector::doubleNormalize(direction);
	TCVector::doubleMultiply(direction, tempV,
		TCVector::doubleLength(location));
	TCVector::doubleAdd(location, tempV, lookAt);
	// Re-do the strings with higher accuracy, so they'll be
	// accepted by POV-Ray.
	sucprintf(upString, COUNT_OF(upString), _UC("%.20g,%.20g,%.20g"), up[0],
		up[1], up[2]);
	sucprintf(lookAtString, COUNT_OF(lookAtString), _UC("%.20g,%.20g,%.20g"),
		lookAt[0], lookAt[1], lookAt[2]);
	sucprintf(cameraString, COUNT_OF(cameraString),
		_UC("camera\n")
		_UC("{\n")
		_UC("\t#declare ASPECT = 4/3;\n")
		_UC("\tlocation < %s >\n")
		_UC("\tsky < %s >\n")
		_UC("\tright ASPECT * < -1,0,0 >\n")
		_UC("\tlook_at < %s >\n")
		_UC("\tangle %g\n")
		_UC("}\n"),
		locationString, upString, lookAtString, getHFov());
	userMessage = copyString(message);
	povCamera = ucstringtombs(cameraString);
}

bool LDrawModelViewer::mouseDown(LDVMouseMode mode, int x, int y)
{
	if ((mouseMode != LDVMouseNone && mouseMode != mode) ||
		mode == LDVMouseNone)
	{
		return false;
	}
	if (mode != LDVMouseLight)
	{
		debugPrintf("LDVMouseLight is the only mode currently supported.\n");
		return false;
	}
	if (mode == LDVMouseLight && !flags.useLighting)
	{
		// Allowing this will likely just lead to confusion.
		return false;
	}
	lastMouseX = x;
	lastMouseY = y;
	mouseMode = mode;
	switch (mouseMode)
	{
	case LDVMouseLight:
		if (haveStandardLight())
		{
			flags.showLight = true;
		}
		break;
	default:
		break;
	}
	return true;
}

bool LDrawModelViewer::mouseUp(int x, int y)
{
	int deltaX = x - lastMouseX;
	int deltaY = y - lastMouseY;

	if (mouseMode != LDVMouseLight)
	{
		debugPrintf("LDVMouseLight is the only mode currently supported.\n");
		return false;
	}
	lastMouseX = x;
	lastMouseY = y;
	switch (mouseMode)
	{
	case LDVMouseLight:
		if (haveStandardLight())
		{
			mouseMoveLight(deltaX, deltaY);
			preferences->setLightVector(lightVector, true);
			flags.showLight = false;
		}
		break;
	default:
		break;
	}
	mouseMode = LDVMouseNone;
	return true;
}

bool LDrawModelViewer::mouseMove(int x, int y)
{
	int deltaX = x - lastMouseX;
	int deltaY = y - lastMouseY;

	if (mouseMode != LDVMouseLight)
	{
		//debugPrintf("LDVMouseLight is the only mode currently supported.\n");
		return false;
	}
	lastMouseX = x;
	lastMouseY = y;
	switch (mouseMode)
	{
	case LDVMouseLight:
		mouseMoveLight(deltaX, deltaY);
		break;
	default:
		break;
	}
	return true;
}

#define myMin(a, b) ((a) < (b) ? (a) : (b))

void LDrawModelViewer::mouseMoveLight(int deltaX, int deltaY)
{
	TCFloat matrix[16];
	double scale = myMin(width, height) / 10.0;
	double angle = deltaX / scale;
	TCVector newLightVector;

	TCVector::initIdentityMatrix(matrix);
	matrix[0] = (float)cos(angle);
	matrix[2] = (float)-sin(angle);
	matrix[8] = (float)sin(angle);
	matrix[10] = (float)cos(angle);
	newLightVector = lightVector.transformPoint(matrix);
	angle = deltaY / scale;
	TCVector::initIdentityMatrix(matrix);
	matrix[5] = (float)cos(angle);
	matrix[6] = (float)sin(angle);
	matrix[9] = (float)-sin(angle);
	matrix[10] = (float)cos(angle);
	setLightVector(newLightVector.transformPoint(matrix));
}

UCSTR LDrawModelViewer::getOpenGLDriverInfo(int &numExtensions)
{
	UCSTR vendorString = mbstoucstring((const char*)glGetString(GL_VENDOR));
	UCSTR rendererString = mbstoucstring((const char*)glGetString(GL_RENDERER));
	UCSTR versionString = mbstoucstring((const char*)glGetString(GL_VERSION));
	const char *extensionsString = (const char*)glGetString(GL_EXTENSIONS);
	UCSTR extensionsList;
	int len;
	UCSTR message;

	numExtensions = 0;
	if (!vendorString)
	{
		vendorString = copyString(TCLocalStrings::get(_UC("*Unknown*")));
	}
	if (!rendererString)
	{
		rendererString = copyString(TCLocalStrings::get(_UC("*Unknown*")));
	}
	if (!versionString)
	{
		versionString = copyString(TCLocalStrings::get(_UC("*Unknown*")));
	}
	if (extensionsString)
	{
		char *temp = stringByReplacingSubstring(extensionsString, " ",
			"\r\n");

		stripCRLF(temp);
		numExtensions = countStringLines(temp);
		extensionsList = mbstoucstring(temp);
		delete temp;
	}
	else
	{
		extensionsList = copyString(TCLocalStrings::get(_UC("*None*")));
	}
	len = ucstrlen(vendorString) + ucstrlen(rendererString) +
		ucstrlen(versionString) + ucstrlen(extensionsList) + 128;
	message = new UCCHAR[len];
	sucprintf(message, len, TCLocalStrings::get(_UC("OpenGlInfo")),
		vendorString, rendererString, versionString, extensionsList);
	delete vendorString;
	delete rendererString;
	delete versionString;
	delete extensionsList;
	return message;
}

TCFloat LDrawModelViewer::getWideLineMargin(void)
{
	TCFloat margin = 0.0f;

	if (flags.showsHighlightLines && highlightLineWidth > 1.0f)
	{
		margin = highlightLineWidth / 2.0f;
	}
	if (flags.drawWireframe && wireframeLineWidth > 1.0)
	{
		if (wireframeLineWidth / 2.0f > margin)
		{
			margin = wireframeLineWidth / 2.0f;
		}
	}
	return margin;
}

static int _numPoints = 0;

// This is conversion of Lars Hassing's auto camera code from L3P.  It computes
// the correct distance and pan amount for the camera so that the viewing
// pyramid will be positioned in the closest possible position, such that the
// model just touches the edges of the view on either the top and bottom, the
// left and right, or all four.
// After processing all the model data for the current camera angle, it ends up
// with 6 equations with 6 unknowns.  It uses a matrix solving routine to solve
// these.  The 6 values seem to be the X, Y, and Z coordinates of two points.
// Once it has the values, it decides which point is the correct point, and then
// uses that as the camera location.
void LDrawModelViewer::zoomToFit(void)
{
	if (mainTREModel)
	{
		TCFloat d;
		TCFloat a[6][6];
		TCFloat b[6];
		TCFloat x[6];
		TCVector tmpVec;
		TCVector location;
		TCVector cameraDir;
		TCFloat tmpMatrix[16];
		TCFloat tmpMatrix2[16];
		TCFloat transformationMatrix[16];
		TCFloat margin;
		char *cameraGlobe = TCUserDefaults::stringForKey("CameraGlobe", NULL,
			false);
		TCFloat globeRadius;

		TCVector::initIdentityMatrix(tmpMatrix);
		tmpMatrix[12] = center[0];
		tmpMatrix[13] = center[1];
		tmpMatrix[14] = center[2];
		TCVector::multMatrix(tmpMatrix, rotationMatrix, tmpMatrix2);
		tmpMatrix[12] = -center[0];
		tmpMatrix[13] = -center[1];
		tmpMatrix[14] = -center[2];
		TCVector::multMatrix(tmpMatrix2, tmpMatrix, transformationMatrix);
		zoomToFitWidth = width * numXTiles / getStereoWidthModifier();
		zoomToFitHeight = (TCFloat)(height * numYTiles);
		margin = getWideLineMargin() * 2.0f;
		preCalcCamera();
		_numPoints = 0;
		mainTREModel->scanPoints(this,
			(TREScanPointCallback)&LDrawModelViewer::scanCameraPoint,
			transformationMatrix);
		debugPrintf("num points: %d\n", _numPoints);
//		char message[1024];
//		sprintf(message, "num points: %d", _numPoints);
//		MessageBox(NULL, message, "Points", MB_OK);
		d = zoomToFitWidth / zoomToFitHeight;
//		dh = (cameraData->horMax - cameraData->horMin) / d;
//		dv = cameraData->verMax - cameraData->verMin;
		memset(a, 0, sizeof(a));
		memset(b, 0, sizeof(b));
		a[0][0] = cameraData->normal[0][0];
		a[0][1] = cameraData->normal[0][1];
		a[0][2] = cameraData->normal[0][2];
		b[0] = cameraData->dMin[0];
		a[1][0] = cameraData->normal[1][0];
		a[1][1] = cameraData->normal[1][1];
		a[1][2] = cameraData->normal[1][2];
		b[1] = cameraData->dMin[1];
		a[2][3] = cameraData->normal[2][0];
		a[2][4] = cameraData->normal[2][1];
		a[2][5] = cameraData->normal[2][2];
		b[2] = cameraData->dMin[2];
		a[3][3] = cameraData->normal[3][0];
		a[3][4] = cameraData->normal[3][1];
		a[3][5] = cameraData->normal[3][2];
		b[3] = cameraData->dMin[3];
		if (cameraData->direction[0] == 0.0)
		{
			a[4][1] = -cameraData->direction[2];
			a[4][2] = cameraData->direction[1];
			a[4][4] = cameraData->direction[2];
			a[4][5] = -cameraData->direction[1];
			if (cameraData->direction[1] == 0.0)
			{
				a[5][0] = -cameraData->direction[2];
				a[5][2] = cameraData->direction[0];
				a[5][3] = cameraData->direction[2];
				a[5][5] = -cameraData->direction[0];
			}
			else
			{
				a[5][0] = -cameraData->direction[1];
				a[5][1] = cameraData->direction[0];
				a[5][3] = cameraData->direction[1];
				a[5][4] = -cameraData->direction[0];
			}
		}
		else
		{
			a[4][0] = -cameraData->direction[2];
			a[4][2] = cameraData->direction[0];
			a[4][3] = cameraData->direction[2];
			a[4][5] = -cameraData->direction[0];
			if (cameraData->direction[1] == 0.0 && cameraData->direction[2]
				!= 0.0)
			{
				a[5][1] = -cameraData->direction[2];
				a[5][2] = cameraData->direction[1];
				a[5][4] = cameraData->direction[2];
				a[5][5] = -cameraData->direction[1];
			}
			else
			{
				a[5][0] = -cameraData->direction[1];
				a[5][1] = cameraData->direction[0];
				a[5][3] = cameraData->direction[1];
				a[5][4] = -cameraData->direction[0];
			}
		}
		if (!L3Solve6(x, a, b))
		{
			// Singular matrix; can't work
			// (We shouldn't ever get here, so I'm not going to bother with an
			// error message.  I'd have to first come up with some mechanism
			// for communicating the error with the non-portable part of the
			// app so that it could be displayed to the user.)
			return;
		}
		tmpVec = TCVector(x[3], x[4], x[5]) - TCVector(x[0], x[1], x[2]);
		cameraDir = TCVector(cameraData->direction[0], cameraData->direction[1],
			cameraData->direction[2]);
		d = cameraDir.dot(tmpVec);
		if (d > 0.0)
		{
			location[0] = x[0];
			location[1] = x[1];
			location[2] = x[2] * (zoomToFitHeight + margin) / zoomToFitHeight;
		}
		else
		{
			location[0] = x[3];
			location[1] = x[4];
			location[2] = x[5] * (zoomToFitWidth + margin) / zoomToFitWidth;
		}
		if (cameraGlobe && sscanf(cameraGlobe, "%*f,%*f,%f", &globeRadius) == 1)
		{
			if (globeRadius >= 0)
			{
				location[2] = globeRadius;
			}
			else
			{
				location[2] *= 1.0f - globeRadius / 100.0f;
			}
		}
		else
		{
			location[2] *= distanceMultiplier;
		}
		camera.setPosition(location);
		xPan = 0.0f;
		yPan = 0.0f;
	}
}

// More of Lars' L3P auto camera positioning code.
void LDrawModelViewer::scanCameraPoint(const TCVector &point)
{
	TCFloat d;
	int i;

	for (i = 0; i < 4; i++)
	{
		d = cameraData->normal[i].dot(point - center);
		if (d < cameraData->dMin[i])
		{
			cameraData->dMin[i] = d;
		}
	}
/*
	d = cameraData->horizontal.dot(point);
	if (d - zoomToFitMargin < cameraData->horMin)
	{
		cameraData->horMin = d - zoomToFitMargin;
	}
	if (d + zoomToFitMargin > cameraData->horMax)
	{
		cameraData->horMax = d + zoomToFitMargin;
	}
	d = cameraData->vertical.dot(point);
	if (d - zoomToFitMargin < cameraData->verMin)
	{
		cameraData->verMin = d - zoomToFitMargin;
	}
	if (d + zoomToFitMargin > cameraData->verMax)
	{
		cameraData->verMax = d + zoomToFitMargin;
	}
*/
	_numPoints++;
}

// More of Lars' L3P auto camera positioning code.
void LDrawModelViewer::preCalcCamera(void)
{
	TCFloat d;
	int i;

	delete cameraData;
	cameraData = new CameraData;
	if (zoomToFitWidth > zoomToFitHeight)
	{
		cameraData->fov = (TCFloat)(2.0 * rad2deg(atan(tan(deg2rad(fov / 2.0)) *
			(double)zoomToFitWidth / (double)zoomToFitHeight)));
	}
	else
	{
		cameraData->fov = fov;
	}
	d = (TCFloat)(1.0 / tan(deg2rad(cameraData->fov / 2.0)));
	cameraData->normal[2] = cameraData->direction -
		(cameraData->horizontal * d);
	cameraData->normal[3] = cameraData->direction +
		(cameraData->horizontal * d);
	d *= zoomToFitWidth / zoomToFitHeight;
	cameraData->normal[0] = cameraData->direction -
		(cameraData->vertical * d);
	cameraData->normal[1] = cameraData->direction +
		(cameraData->vertical * d);
	for (i = 0; i < 4; i++)
	{
		cameraData->normal[i].normalize();
	}
}


/* The following L3Solve6 was extracted by Lars C. Hassing in May 2005
  from jama_lu.h, part of the "JAMA/C++ Linear Algebra Package" (JAva MAtrix)
  found together with the "Template Numerical Toolkit (TNT)"
  at http://math.nist.gov/tnt/download.html
  Their disclaimer:
  "This software was developed at the National Institute of Standards and
   Technology (NIST) by employees of the Federal Government in the course
   of their official duties. Pursuant to title 17 Section 105 of the
   United States Code this software is not subject to copyright protection
   and is in the public domain. NIST assumes no responsibility whatsoever
   for its use by other parties, and makes no guarantees, expressed or
   implied, about its quality, reliability, or any other characteristic."
*/

/* Solve A*x=b, returns 1 if OK, 0 if A is singular */
int LDrawModelViewer::L3Solve6(TCFloat x[L3ORDERN],
            const TCFloat A[L3ORDERM][L3ORDERN],
            const TCFloat b[L3ORDERM])
{
  TCFloat          LU_[L3ORDERM][L3ORDERN];
  int            pivsign;
  int            piv[L3ORDERM];/* pivot permutation vector                  */
  int            i;
  int            j;
  int            k;
  int            p;
  TCFloat         *LUrowi;
  TCFloat          LUcolj[L3ORDERM];
  int            kmax;
  double         s;
  TCFloat          t;

  /** LU Decomposition.
  For an m-by-n matrix A with m >= n, the LU decomposition is an m-by-n
  unit lower triangular matrix L, an n-by-n upper triangular matrix U,
  and a permutation vector piv of length m so that A(piv,:) = L*U.
  If m < n, then L is m-by-m and U is m-by-n.

  The LU decompostion with pivoting always exists, even if the matrix is
  singular, so the constructor will never fail.  The primary use of the
  LU decomposition is in the solution of square systems of simultaneous
  linear equations.  This will fail if isNonsingular() returns false.
                                                                            */
  memcpy(LU_, A, sizeof(LU_));

  /* Use a "left-looking", dot-product, Crout/Doolittle algorithm. */
  for (i = 0; i < L3ORDERM; i++)
     piv[i] = i;
  pivsign = 1;

  /* Outer loop. */
  for (j = 0; j < L3ORDERN; j++)
  {
     /* Make a copy of the j-th column to localize references. */
     for (i = 0; i < L3ORDERM; i++)
        LUcolj[i] = LU_[i][j];

     /* Apply previous transformations. */
     for (i = 0; i < L3ORDERM; i++)
     {
        LUrowi = LU_[i];
        /* Most of the time is spent in the following dot product. */
        kmax = i < j ? i : j;  /* min(i, j)                                 */
        s = 0.0;
        for (k = 0; k < kmax; k++)
           s += LUrowi[k] * LUcolj[k];
        LUrowi[j] = LUcolj[i] -= (TCFloat)s;
     }

     /* Find pivot and exchange if necessary. */
     p = j;
     for (i = j + 1; i < L3ORDERM; i++)
     {
        if (fabs(LUcolj[i]) > fabs(LUcolj[p]))
           p = i;
     }
     if (p != j)
     {
        for (k = 0; k < L3ORDERN; k++)
        {
           t = LU_[p][k];
           LU_[p][k] = LU_[j][k];
           LU_[j][k] = t;
        }
        k = piv[p];
        piv[p] = piv[j];
        piv[j] = k;
        pivsign = -pivsign;
     }

     /* Compute multipliers. */
     if ((j < L3ORDERM) && (LU_[j][j] != 0.0))
     {
        for (i = j + 1; i < L3ORDERM; i++)
           LU_[i][j] /= LU_[j][j];
     }
  }

  /* LCH: This was LU Decomposition. Now solve: */

  /** Solve A*x = b, where x and b are vectors of length equal
        to the number of rows in A.

  @param  b   a vector (Array1D> of length equal to the first dimension of A.
  @return x   a vector (Array1D> so that L*U*x = b(piv), if B is nonconformant,
              returns 0x0 (null) array.
                                                                            */
  /* Is the matrix nonsingular? I.e. is upper triangular factor U (and hence
     A) nonsingular (isNonsingular())                                       */
  for (j = 0; j < L3ORDERN; j++)
  {
     if (LU_[j][j] == 0)
        return 0;
  }

  for (i = 0; i < L3ORDERN; i++)
     x[i] = b[piv[i]];

  /* Solve L*Y = B(piv) */
  for (k = 0; k < L3ORDERN; k++)
  {
     for (i = k + 1; i < L3ORDERN; i++)
        x[i] -= x[k] * LU_[i][k];
  }

  /* Solve U*X = Y; */
  for (k = L3ORDERN - 1; k >= 0; k--)
  {
     x[k] /= LU_[k][k];
     for (i = 0; i < k; i++)
        x[i] -= x[k] * LU_[i][k];
  }

  return 1;
}                               /* Solve6                                    */
