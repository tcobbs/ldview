#include "LDrawModelViewer.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCImage.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLError.h>
#include "LDModelParser.h"
#include <TRE/TREMainModel.h>
#include <TRE/TREGL.h>

#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 16
#define FONT_IMAGE_WIDTH 128
#define FONT_IMAGE_HEIGHT 256
#define FONT_NUM_CHARACTERS 256
#define DEF_DISTANCE_MULT 1.0f

LDrawModelViewer::LDrawModelViewer(int width, int height)
			:mainTREModel(NULL),
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
			 clipZoom(false),
			 fontListBase(0),
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
			 memoryUsage(2),
			 extraSearchDirs(NULL),
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
	flags.autoCenter = true;
	flags.forceZoomToFit = false;
	flags.defaultTrans = false;
	flags.bfc = true;
	flags.redBackFaces = false;
	flags.greenFrontFaces = false;
//	TCAlertManager::registerHandler(LDLError::alertClass(), this,
//		(TCAlertCallback)ldlErrorCallback);
//	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
//		(TCAlertCallback)progressAlertCallback);
}

LDrawModelViewer::~LDrawModelViewer(void)
{
}

void LDrawModelViewer::dealloc(void)
{
//	TCAlertManager::unregisterHandler(LDLError::alertClass(), this);
//	TCAlertManager::unregisterHandler(TCProgressAlert::alertClass(), this);
	TCObject::release(mainTREModel);
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

//		tileWidth = (float)width / numXTiles;
//		tileHeight = (float)height / numYTiles;
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
		glTranslatef(xOffset, yOffset, 0.0f);
	}
}

float LDrawModelViewer::getStereoWidthModifier(void)
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

void LDrawModelViewer::setFieldOfView(double fov, float nClip, float fClip)
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

void LDrawModelViewer::setFov(float value)
{
	if (value != fov)
	{
		fov = value;
		flags.needsViewReset = true;
	}
}

void LDrawModelViewer::updateCurrentFov(void)
{
	int actualWidth = width / (int)getStereoWidthModifier();

	currentFov = fov;
	if (actualWidth * numXTiles < height * numYTiles)
	{
		// When the window is taller than it is wide, we want our current FOV to
		// be the horizontal FOV, so we need to calculate the vertical FOV.
		//
		// From Lars Hassing:
		// Vertical FOV = 2*atan(tan(hfov/2)/(width/height))
		currentFov = (float)(2.0 * rad2deg(atan(tan(deg2rad(fov / 2.0)) *
			(double)height * numYTiles / (actualWidth * numXTiles))));

		if (currentFov > 179.0f)
		{
			currentFov = 179.0f;
		}
		aspectRatio = (float)height / actualWidth;
	}
	else
	{
		aspectRatio = (float)actualWidth / height;
	}
}

float LDrawModelViewer::getClipRadius(void)
{
	float clipRadius;

	if (flags.autoCenter)
	{
		clipRadius = size / 2.0f;
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

void LDrawModelViewer::perspectiveView(bool resetViewport)
{
	float nClip;
	float fClip;
	float clipRadius = getClipRadius();
	int actualWidth = width / (int)getStereoWidthModifier();
	float distance;
	float aspectAdjust = (float)tan(1.0f);

	if (flags.forceZoomToFit)
	{
		zoomToFit();
	}
	distance = camera.getPosition().length();
	currentFov = fov;
	updateCurrentFov();
	if (resetViewport)
	{
		glViewport(0, 0, actualWidth, height);
		flags.needsResize = false;
	}
//	printf("aspectRatio1: %f ", aspectRatio);
	aspectRatio = (float)(1.0f / tan(1.0f / aspectRatio)) * aspectAdjust;
	aspectRatio = 1.0f;
//	printf("aspectRatio2: %f\n", aspectRatio);
	nClip = distance - clipRadius * aspectRatio + clipAmount * aspectRatio *
		clipRadius;
	if (nClip < size / 1000.0f)
	{
		nClip = size / 1000.0f;
	}
	fClip = distance + clipRadius * aspectRatio;
	setFieldOfView(currentFov, nClip, fClip);
	glLoadIdentity();
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, nClip);
	glFogf(GL_FOG_END, fClip);
}

void LDrawModelViewer::perspectiveViewToClipPlane(void)
{
	float nClip;
	float fClip;
	float distance = (camera.getPosition()).length();
	float clipRadius = getClipRadius();
//	float distance = (camera.getPosition() - center).length();

	nClip = distance - size / 2.0f;
//	fClip = distance - size * aspectRatio / 2.0f + clipAmount * aspectRatio *
//		size;
	fClip = distance - clipRadius * aspectRatio + clipAmount * aspectRatio *
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
	glFogf(GL_FOG_START, nClip);
	glFogf(GL_FOG_END, fClip);
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

float LDrawModelViewer::calcDefaultDistance(void)
{
	// Note that the margin is on all sides, so it's on two edges per axis,
	// which is why we multiply by 2.
	float margin = getWideLineMargin() * 2.0f;
	float marginAdjust = 1.0f;

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
	return (float)(size / 2.0 / sin(deg2rad(fov / 2.0))) * distanceMultiplier *
		marginAdjust;
/*
	double angle1 = deg2rad(90.0f - (currentFov / 2.0));
	double angle2 = deg2rad(currentFov / 4.0);
	double radius = size / 2.0;

	return (float)(radius * tan(angle1) + radius * tan(angle2)) *
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
		rotationMatrix = new float[16];
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

void LDrawModelViewer::setDefaultRotationMatrix(const float *value)
{
	if (value)
	{
		if (!defaultRotationMatrix || memcmp(defaultRotationMatrix, value,
			16 * sizeof(float)) != 0)
		{
			delete defaultRotationMatrix;
			defaultRotationMatrix = new float[16];
			memcpy(defaultRotationMatrix, value, 16 * sizeof(float));
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
		memcpy(rotationMatrix, defaultRotationMatrix, 16 * sizeof(float));
	}
	else
	{
		setupIsoViewAngle();
	}
}

void LDrawModelViewer::setupIsoViewAngle(void)
{
	rotationMatrix[0] = (float)(sqrt(2.0) / 2.0);
	rotationMatrix[1] = (float)(sqrt(2.0) / 4.0);
	rotationMatrix[2] = (float)(-sqrt(1.5) / 2.0);
	rotationMatrix[3] = 0.0f;
	rotationMatrix[4] = 0.0f;
	rotationMatrix[5] = (float)(sin(M_PI / 3.0));
	rotationMatrix[6] = 0.5f;
	rotationMatrix[7] = 0.0f;
	rotationMatrix[8] = (float)(sqrt(2.0) / 2.0);
	rotationMatrix[9] = (float)(-sqrt(2.0) / 4.0);
	rotationMatrix[10] = (float)(sqrt(1.5) / 2.0);
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

int LDrawModelViewer::loadModel(bool resetViewpoint)
{
	int retValue = 0;

	if (filename)
	{
		LDLMainModel *mainModel = new LDLMainModel;

		if (clipAmount != 0.0f && resetViewpoint)
		{
			clipAmount = 0.0f;
			perspectiveView();
		}

		// First, release the current TREModel, if it exists.
		TCObject::release(mainTREModel);
		mainTREModel = NULL;
		mainModel->setLowResStuds(!flags.qualityStuds);
		mainModel->setBlackEdgeLines(flags.blackHighlights);
		mainModel->setExtraSearchDirs(extraSearchDirs);
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
			modelParser->setTwoSidedLightingFlag(flags.oneLight ||
				flags.usesSpecular);
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
					TCLocalStrings::get("CalculatingSizeStatus"), 0.0f, &abort);
				if (!abort)
				{
					mainTREModel->getBoundingBox(boundingMin, boundingMax);
					TCProgressAlert::send("LDrawModelViewer",
						TCLocalStrings::get("CalculatingSizeStatus"), 0.5f,
						&abort);
				}
				if (!abort)
				{
					center = (boundingMin + boundingMax) / 2.0f;
					size = mainTREModel->getMaxRadius(center) * 2.0f;
					TCProgressAlert::send("LDrawModelViewer",
						TCLocalStrings::get("CalculatingSizeStatus"), 1.0f,
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
		mainModel->release();
		TCProgressAlert::send("LDrawModelViewer", TCLocalStrings::get("Done"),
			2.0f);
		if (resetViewpoint)
		{
			resetView();
		}
	}
	return retValue;

/*
		// Use binary mode to work around problem with fseek on a non-binary
		// file.  My file parsing code will still work fine and strip out the
		// extra data.
		FILE* modelFile = fopen(filename, "rb");

		if (clipAmount != 0.0f && resetViewpoint)
		{
			clipAmount = 0.0f;
			perspectiveView();
		}
		if (modelFile)
		{
			int readResult = 0;
			bool failed = false;

			if (lDrawModel)
			{
				lDrawModel->release();
				lDrawModel = NULL;
			}
			TGLStudLogo::resetTextureCoords();
			TGLShape::setShowAllConditionalLines(flags.showAllConditionalLines);
			TGLShape::setShowConditionalControlPoints(
				flags.showConditionalControlPoints);
			LDrawModel::setDrawWireframe(flags.drawWireframe &&
				(!flags.drawWireframe || !flags.removeHiddenLines));
			LDrawModel::setUsePolygonOffset(flags.usePolygonOffset);
			LDrawModel::setUseLighting(flags.useLighting);
			lDrawModel = new LDrawModel;
			if (progressCallback)
			{
				lDrawModel->setProgressCallback(progressCallback,
					progressUserData);
			}
			if (errorCallback)
			{
				lDrawModel->setErrorCallback(errorCallback, errorUserData);
			}
			lDrawModel->setAllowLowResStuds(!flags.qualityStuds);
			lDrawModel->setAllowLowPrimitiveSubstitution(
				flags.allowPrimitiveSubstitution);
			lDrawModel->setDrawHighlights(flags.showsHighlightLines);
			lDrawModel->setDrawConditionalHighlights(
				flags.drawConditionalHighlights);
			lDrawModel->setPerformSmoothing(flags.performSmoothing);
			lDrawModel->setLineSmoothing(flags.lineSmoothing);
			lDrawModel->setBlackHighlights(flags.blackHighlights);
			lDrawModel->setIsPart(flags.fileIsPart);
			lDrawModel->setUseStipple(flags.useStipple);
			lDrawModel->setSortTransparent(flags.sortTransparent);
			lDrawModel->setFilename(filename);
			lDrawModel->setCurveQuality(curveQuality);
			lDrawModel->setEdgesOnly(flags.edgesOnly &&
				flags.showsHighlightLines);
			lDrawModel->setHiResPrimitives(flags.hiResPrimitives);
			lDrawModel->setEdgeLineWidth(highlightLineWidth);
			lDrawModel->setProcessLDConfig(flags.processLDConfig);
			if (defaultColorNumber != -1)
			{
				lDrawModel->setDefaultColorNumber(defaultColorNumber);
			}
			else
			{
				lDrawModel->setDefaultRGB(defaultR, defaultG, defaultB);
			}
//			readResult = lDrawModel->read(modelFile, 0);
			fclose(modelFile);
			if (readResult == 0)
			{
//				lDrawModel->getMinMax(boundingMin, boundingMax);
//				lDrawModel->setCullBackFaces(cullBackFaces);
//				lDrawModel->writeToFile();
//				lDrawModel->flatten();
				if (true || lDrawModel->compile(0))
				{
					if (progressCallback)
					{
						progressCallback("", 2.0, progressUserData);
					}
					flags.needsRecompile = false;
					if (resetViewpoint)
					{
						resetView();
					}
				}
				else
				{
					failed = true;
				}
			}
			else
			{
				failed = true;
			}
			if (failed)
			{
				lDrawModel->release();
				lDrawModel = NULL;
				resetViewpoint = true;
			}
		}
		if (resetViewpoint)
		{
			resetView();
//			delete rotationMatrix;
//			rotationMatrix = NULL;
		}
		if (mainTREModel)
		{
			flags.needsResize = true;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
*/
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
		mainTREModel->recompile();
		flags.needsRecompile = false;
		TCProgressAlert::send("LDrawModelViewer", TCLocalStrings::get("Done"),
			2.0f);
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
//	float mAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	float mAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
//	float mSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float mSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
//	float mSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};

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

void LDrawModelViewer::setupLight(GLenum light)
{
	float lAmbient[4];
	float lDiffuse[4];
	float lSpecular[4];

	if (flags.subduedLighting)
	{
		lAmbient[0] = lAmbient[1] = lAmbient[2] = 0.5f;
		lDiffuse[0] = lDiffuse[1] = lDiffuse[2] = 0.5f;
	}
	else
	{
		lAmbient[0] = lAmbient[1] = lAmbient[2] = 0.0f;
		lDiffuse[0] = lDiffuse[1] = lDiffuse[2] = 1.0f;
	}
	if (!flags.usesSpecular)
	{
		lSpecular[0] = lSpecular[1] = lSpecular[2] = 0.0f;
	}
	else
	{
		lSpecular[0] = lSpecular[1] = lSpecular[2] = 1.0f;
	}
	lAmbient[3] = 1.0f;
	lDiffuse[3] = 1.0f;
	lSpecular[3] = 1.0f;
	if (light != GL_LIGHT0)
	{
		lAmbient[0] = lAmbient[1] = lAmbient[2] = 0.0f;
	}
	glLightfv(light, GL_AMBIENT, lAmbient);
	glLightfv(light, GL_SPECULAR, lSpecular);
	glLightfv(light, GL_DIFFUSE, lDiffuse);
	glEnable(light);
}

void LDrawModelViewer::setupLighting(void)
{
	if (flags.useLighting)
	{
		setupLight(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		if (flags.oneLight || flags.usesSpecular ||
			(flags.bfc && (flags.redBackFaces | flags.greenFrontFaces)))
		{
			glDisable(GL_LIGHT1);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		}
		else
		{
			setupLight(GL_LIGHT1);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
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
					// That spot should be on, so set the bit.
					imageData[yOffset + (col * FONT_CHAR_WIDTH + j) * 4 + 3] =
						0xFF;
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
	loadVGAFont(fontFilename);
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
			float cx, cy;
			float wx, hy;
			float tx, ty;

			cx = (float)(i % 16) * FONT_CHAR_WIDTH / (float)(FONT_IMAGE_WIDTH);
			cy = (float)(i / 16) * FONT_CHAR_HEIGHT /
				(float)(FONT_IMAGE_HEIGHT);
			wx = (float)FONT_CHAR_WIDTH / FONT_IMAGE_WIDTH;
			hy = (float)FONT_CHAR_HEIGHT / FONT_IMAGE_HEIGHT;
			glNewList(fontListBase + i, GL_COMPILE);
				glBegin(GL_QUADS);
					tx = cx;
					ty = 1.0f - cy - hy;
					glTexCoord2f(tx, ty);			// Bottom Left
					glVertex2i(0, 0);
					tx = cx + wx;
					ty = 1.0f - cy - hy;
					glTexCoord2f(tx, ty);			// Bottom Right
					glVertex2i(FONT_CHAR_WIDTH, 0);
					tx = cx + wx;
					ty = 1 - cy;
					glTexCoord2f(tx, ty);			// Top Right
					glVertex2i(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
					tx = cx;
					ty = 1 - cy;
					glTexCoord2f(tx , ty);			// Top Left
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
		glVertex3fv(boundingMin);
		glVertex3f(boundingMax[0], boundingMin[1], boundingMin[2]);
		glVertex3f(boundingMax[0], boundingMax[1], boundingMin[2]);
		glVertex3f(boundingMin[0], boundingMax[1], boundingMin[2]);
		glVertex3fv(boundingMin);
		glVertex3f(boundingMin[0], boundingMin[1], boundingMax[2]);
		glVertex3f(boundingMax[0], boundingMin[1], boundingMax[2]);
		glVertex3fv(boundingMax);
		glVertex3f(boundingMin[0], boundingMax[1], boundingMax[2]);
		glVertex3f(boundingMin[0], boundingMin[1], boundingMax[2]);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(boundingMin[0], boundingMax[1], boundingMin[2]);
		glVertex3f(boundingMin[0], boundingMax[1], boundingMax[2]);
		glVertex3f(boundingMax[0], boundingMax[1], boundingMin[2]);
		glVertex3f(boundingMax[0], boundingMax[1], boundingMax[2]);
		glVertex3f(boundingMax[0], boundingMin[1], boundingMin[2]);
		glVertex3f(boundingMax[0], boundingMin[1], boundingMax[2]);
	glEnd();
	if (lightingEnabled)
	{
		glEnable(GL_LIGHTING);
	}
}

void LDrawModelViewer::orthoView(void)
{
	int actualWidth = width;
	static char glVendor[1024];
	static bool glVendorRead = false;

	if (!glVendorRead)
	{
		strcpy(glVendor, (const char *)glGetString(GL_VENDOR));
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
		glTranslatef(0.375f, 0.375f, 0.0f);
		debugPrintf("Not an ATI.\n");
	}
}

void LDrawModelViewer::setSeamWidth(float value)
{
	if (!fEq(value, seamWidth))
	{
		seamWidth = value;
		flags.needsReload = true;
	}
}

void LDrawModelViewer::drawString(GLfloat xPos, GLfloat yPos, char* string)
{
	if (!fontListBase)
	{
		setupTextures();
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
	glTranslated(xPos, yPos, 0);
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

void LDrawModelViewer::drawFPS(float fps)
{
	if (mainTREModel)
	{
		char fpsString[1024];
		int lightingEnabled = glIsEnabled(GL_LIGHTING);
		int zBufferEnabled = glIsEnabled(GL_DEPTH_TEST);
		//float xMult = (float)width / (float)height;

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
			strcpy(fpsString, TCLocalStrings::get("FPSSpinPrompt"));
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

void LDrawModelViewer::drawLight(GLenum light, float x, float y, float z)
{
	float position[4];
	float direction[4];
	//float fullIntensity[] = {1.0f, 1.0f, 1.0f, 1.0f};

	position[0] = x;
	position[1] = y;
	position[2] = z;
	position[3] = 0.0f;
	direction[0] = -x;
	direction[1] = -y;
	direction[2] = -z;
	direction[3] = 0.0f;
	glLightfv(light, GL_POSITION, position);
	glLightfv(light, GL_SPOT_DIRECTION, direction);
//	glLightfv(light, GL_DIFFUSE, fullIntensity);
//	glLightfv(light, GL_SPECULAR, fullIntensity);
}

void LDrawModelViewer::drawLights(void)
{
//	drawLight(GL_LIGHT1, 0.0f, 0.0f, -10000.0f);
	drawLight(GL_LIGHT0, 0.0f, 0.0f, 1.0f);
	drawLight(GL_LIGHT1, 0.0f, 0.0f, -1.0f);
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

void LDrawModelViewer::setCutawayLineWidth(float value)
{
	cutawayLineWidth = value;
}

void LDrawModelViewer::setCutawayAlpha(float value)
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

void LDrawModelViewer::setHighlightLineWidth(float value)
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

void LDrawModelViewer::setWireframeLineWidth(float value)
{
	wireframeLineWidth = value;
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

void LDrawModelViewer::zoom(float amount)
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
		float distance = (camera.getPosition()).length();
//		float distance = (camera.getPosition() - center).length();

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
		float distance = (camera.getPosition()).length();

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
	float backgroundColor[3];

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
	backgroundColor[0] = backgroundR;
	backgroundColor[1] = backgroundG;
	backgroundColor[2] = backgroundB;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void LDrawModelViewer::drawSetup(GLfloat eyeXOffset)
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
//		glTranslatef(eyeXOffset + xPan, yPan, -distance);
	}
	else
	{
		camera.project(TCVector(-eyeXOffset - xPan, -yPan, 10.0f));
//		glTranslatef(eyeXOffset + xPan, yPan, -10.0f);
	}
}

void LDrawModelViewer::drawToClipPlaneUsingStencil(GLfloat eyeXOffset)
{
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
//	glTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		glRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		glMultMatrixf(rotationMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		glMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		glTranslatef(-center[0], -center[1], -center[2]);
	}
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
}

/*
void LDrawModelViewer::drawToClipPlaneUsingAccum(GLfloat eyeXOffset)
{
	float weight = 0.25f;
	float oldZoomSpeed = zoomSpeed;

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
		glMultMatrixf(rotationMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		glMultMatrixf(rotationMatrix);
	}
	glTranslatef(-center[0], -center[1], -center[2]);
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
	glTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(rotationMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		glMultMatrixf(rotationMatrix);
	}
	glTranslatef(-center[0], -center[1], -center[2]);
	mainTREModel->draw();
	perspectiveView();
	glAccum(GL_ACCUM, weight);
	glAccum(GL_RETURN, 1.0f);
}
*/

void LDrawModelViewer::drawToClipPlaneUsingDestinationAlpha(GLfloat eyeXOffset)
{
	float weight = cutawayAlpha;

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
//	glTranslatef(eyeXOffset + xPan, yPan, -distance);
	//glTranslatef(0.0f, 0.0f, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		glRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		glMultMatrixf(rotationMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		glMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		glTranslatef(-center[0], -center[1], -center[2]);
	}
	mainTREModel->setCutawayDrawFlag(true);
	mainTREModel->draw();
	mainTREModel->setCutawayDrawFlag(false);
	perspectiveView();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_BLEND);
}

void LDrawModelViewer::drawToClipPlaneUsingNoEffect(GLfloat eyeXOffset)
{
	perspectiveViewToClipPlane();
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(cutawayLineWidth);
	glDisable(GL_FOG);
	glLoadIdentity();
	camera.project(TCVector(-eyeXOffset - xPan, -yPan, 0.0f));
//	glTranslatef(eyeXOffset + xPan, yPan, -distance);
	if (rotationMatrix)
	{
		glPushMatrix();
		glLoadIdentity();
		glRotatef(rotationSpeed, xRotate, yRotate, zRotate);
		glMultMatrixf(rotationMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
		glPopMatrix();
		glMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		glTranslatef(-center[0], -center[1], -center[2]);
	}
	mainTREModel->draw();
	perspectiveView();
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void LDrawModelViewer::drawToClipPlane(GLfloat eyeXOffset)
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
	glClearDepth(1.0);
	glClearColor(backgroundR, backgroundG, backgroundB, backgroundA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool LDrawModelViewer::getLDrawCommandLineMatrix(char *matrixString,
												 int bufferLength)
{
	if (rotationMatrix && bufferLength)
	{
		char buf[1024];
		float matrix[16];
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
		float transformationMatrix[16];

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
		float matrix[16];
		int i;
		TCVector point;
		float scaleFactor = 500.0f;
		float distance = (camera.getPosition()).length();
//		float distance = (camera.getPosition() - center).length();

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
	GLfloat eyeXOffset = 0.0f;

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
		rotationMatrix = new float[16];
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
		float distance = (camera.getPosition()).length();
//		float distance = (camera.getPosition() - center).length();

		eyeXOffset = stereoEyeSpacing * 2.0f / (float)pow((double)distance,
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
			float matrix[16];
			TCVector rotation = TCVector(xRotate, yRotate, zRotate);

			camera.getFacing().getInverseMatrix(matrix);
			glPushMatrix();
			glLoadIdentity();
			rotation = rotation.mult(matrix);
//			printf("[%f %f %f] [%f %f %f]\n", xRotate, yRotate, zRotate,
//				rotation[0], rotation[1], rotation[2]);
//			glRotatef(rotationSpeed, xRotate, yRotate, zRotate);
			glRotatef(rotationSpeed, rotation[0], rotation[1], rotation[2]);
			glMultMatrixf(rotationMatrix);
			glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
			glPopMatrix();
		}
	}
	updateCameraPosition();
	zoom(zoomSpeed);
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
}

void LDrawModelViewer::removeHiddenLines(GLfloat eyeXOffset)
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
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glMultMatrixf(rotationMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	glPopMatrix();
	flags.needsRotationMatrixSetup = false;
/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(50.0f, 0.0f, -1.5f, 1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
	glPopMatrix();
*/
}

void LDrawModelViewer::drawModel(GLfloat eyeXOffset)
{
	drawSetup(eyeXOffset);
	if (rotationMatrix)
	{
		glMultMatrixf(rotationMatrix);
	}
	if (flags.autoCenter)
	{
		glTranslatef(-center[0], -center[1], -center[2]);
	}
//	drawBoundingBox();
	glColor3ub(192, 192, 192);
	if (mainTREModel)
	{
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

void LDrawModelViewer::setRotationSpeed(float value)
{
	rotationSpeed = value;
	if (value)
	{
		flags.paused = false;
	}
}

void LDrawModelViewer::setZoomSpeed(float value)
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
	float adjustment;
	float distance = (camera.getPosition()).length();
//	float distance = (camera.getPosition() - center).length();

	if (width > height)
	{
		adjustment = (float)width;
	}
	else
	{
		adjustment = (float)height;
	}
//	xPan += xValue / (float)pow(distance, 0.001);
//	yPan -= yValue / (float)pow(distance, 0.001);
	xPan += xValue / adjustment / (float)pow(2.0 / distance, 1.1) *
		(float)(sin(deg2rad(fov)) / sin(deg2rad(45.0)));
	yPan -= yValue / adjustment / (float)pow(2.0 / distance, 1.1) *
		(float)(sin(deg2rad(fov)) / sin(deg2rad(45.0)));
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

char *LDrawModelViewer::getOpenGLDriverInfo(int &numExtensions)
{
	const char *vendorString = (const char*)glGetString(GL_VENDOR);
	const char *rendererString = (const char*)glGetString(GL_RENDERER);
	const char *versionString = (const char*)glGetString(GL_VERSION);
	const char *extensionsString = (const char*)glGetString(GL_EXTENSIONS);
	char *extensionsList;
	int len;
	char *message;

	numExtensions = 1;
	if (!vendorString)
	{
		vendorString = TCLocalStrings::get("*Unknown*");
	}
	if (!rendererString)
	{
		rendererString = TCLocalStrings::get("*Unknown*");
	}
	if (!versionString)
	{
		versionString = TCLocalStrings::get("*Unknown*");
	}
	if (!extensionsString)
	{
		extensionsString = TCLocalStrings::get("*None*");
	}
	extensionsList = stringByReplacingSubstring(extensionsString, " ",
		"\r\n");
	stripCRLF(extensionsList);
	len = strlen(vendorString) + strlen(rendererString) +
		strlen(versionString) + strlen(extensionsList) + 128;
	message = new char[len];
	sprintf(message, TCLocalStrings::get("OpenGlInfo"), vendorString,
		rendererString, versionString, extensionsList);
	numExtensions = countStringLines(extensionsList);
	delete extensionsList;
	return message;
}

float LDrawModelViewer::getWideLineMargin(void)
{
	float margin = 0.0f;

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
		float d;
//		float dh;
//		float dv;
		float a[6][6];
		float b[6];
		int index[6];
		TCVector location;
		float tmpMatrix[16];
		float transformationMatrix[16];
		float margin;

		TCVector::initIdentityMatrix(tmpMatrix);
		tmpMatrix[12] = center[0];
		tmpMatrix[13] = center[1];
		tmpMatrix[14] = center[2];
		TCVector::multMatrix(tmpMatrix, rotationMatrix, transformationMatrix);
		zoomToFitWidth = width * numXTiles / getStereoWidthModifier();
		zoomToFitHeight = (float)(height * numYTiles);
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
		ludcmp(a, 6, index, &d);
		lubksb(a, 6, index, b);
		d = cameraData->direction[0] * (b[3] - b[0]) +
			cameraData->direction[1] * (b[4] - b[1]) +
			cameraData->direction[2] * (b[5] - b[2]);
		if (d > 0.0)
		{
			location[0] = b[0];
			location[1] = b[1];
			location[2] = b[2] * (zoomToFitHeight + margin) / zoomToFitHeight;
		}
		else
		{
			location[0] = b[3];
			location[1] = b[4];
			location[2] = b[5] * (zoomToFitWidth + margin) / zoomToFitWidth;
		}
		location[2] *= distanceMultiplier;
		camera.setPosition(location - center);
		xPan = 0.0f;
		yPan = 0.0f;
		flags.autoCenter = false;
	}
}

// More of Lars' L3P auto camera positioning code.
void LDrawModelViewer::scanCameraPoint(const TCVector &point)
{
	float d;
	int i;

	for (i = 0; i < 4; i++)
	{
		d = cameraData->normal[i].dot(point);
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
	float d;
	int i;

	delete cameraData;
	cameraData = new CameraData;
	if (zoomToFitWidth > zoomToFitHeight)
	{
		cameraData->fov = (float)(2.0 * rad2deg(atan(tan(deg2rad(fov / 2.0)) *
			(double)zoomToFitWidth / (double)zoomToFitHeight)));
	}
	else
	{
		cameraData->fov = fov;
	}
	d = (float)(1.0 / tan(deg2rad(cameraData->fov / 2.0)));
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

#define TINY 1.0e-20                      /* A small number.                 */

// More of Lars' L3P auto camera positioning code.
void LDrawModelViewer::ludcmp(float a[6][6], int n, int index[6], float *d)
/* Given a matrix a[0..n-1][0..n-1], this routine replaces it by the LU
   decomposition of a rowwise permutation of itself. a and n are input. a is
   output, arranged as in equation (2.3.14) above; index[0..n-1] is an output
   vector that records the row permutation effected by the partial pivoting; d
   is output as +1/-1 depending on whether the number of row interchanges was
   even or odd, respectively. This routine is used in combination with lubksb
   to solve linear equations or invert a matrix.                             */
{
   int                  i,
                        imax,
                        j,
                        k;
   float                big,
                        dum,
                        sum,
                        temp;
   float                vv[8];            /* vv stores the implicit scaling of
                                             each row.                       */

   imax = 0; // Get rid of warning.
   *d = 1.0;                              /* No row interchanges yet.        */
   for (i = 0; i < n; i++)
   {                                      /* Loop over rows to get the implicit
                                             scaling information.            */
      big = 0.0;
      for (j = 0; j < n; j++)
         if ((temp = (float)fabs(a[i][j])) > big)
            big = temp;
      if (big == 0.0)
      {
		  // Singular matrix
		  // We should display an error, except that hopefully it is impossible
		  // to get here.
      }
      /* No nonzero largest element. */
      vv[i] = (float)(1.0 / big);         /* Save the scaling.               */
   }
   for (j = 0; j < n; j++)
   {                                      /* This is the loop over columns of
                                             Crout's method.                 */
      for (i = 0; i < j; i++)
      {                                   /* This is equation (2.3.12) except
                                             for i = j.                      */
         sum = a[i][j];
         for (k = 0; k < i; k++)
            sum -= a[i][k] * a[k][j];
         a[i][j] = sum;
      }
      big = 0.0;                          /* Initialize for the search for
                                             largest pivot element.          */
      for (i = j; i < n; i++)
      {                                   /* This is i = j of equation (2.3.12)
                                             and i = j +1...N of equation
                                             (2.3.13).                       */
         sum = a[i][j];
         for (k = 0; k < j; k++)
            sum -= a[i][k] * a[k][j];
         a[i][j] = sum;
         if ((dum = (float)(vv[i] * fabs(sum))) >= big)
         {
            /* Is the figure of merit for the pivot better than the best so
               far?                                                          */
            big = dum;
            imax = i;
         }
      }
      if (j != imax)
      {                                   /* Do we need to interchange rows? */
         for (k = 0; k < n; k++)
         {                                /* Yes, do so...                   */
            dum = a[imax][k];
            a[imax][k] = a[j][k];
            a[j][k] = dum;
         }
         *d = -(*d);                      /* ...and change the parity of d.  */
         vv[imax] = vv[j];                /* Also interchange the scale
                                             factor.                         */
      }
      index[j] = imax;
      if (a[j][j] == 0.0)
         a[j][j] = (float)TINY;
      /* If the pivot element is zero the matrix is singular (at least to the
         precision of the algorithm). For some applications on singular
         matrices, it is desirable to substitute TINY for zero.              */
      if (j != n - 1)
      {                                   /* Now, finally, divide by the pivot
                                             element.                        */
         dum = (float)(1.0 / (a[j][j]));
         for (i = j + 1; i < n; i++)
            a[i][j] *= dum;
      }
   }                                      /* Go back for the next column in the
                                             reduction.                      */
}

// More of Lars' L3P auto camera positioning code.
/* Here is the routine for forward substitution and backsubstitution, implementing equations (2.3.6) and (2.3.7). */
void LDrawModelViewer::lubksb(const float a[6][6], int n, const int index[6],
							  float b[6])
/* Solves the set of n linear equations A . X = B. Here a[0..n-1][0..n-1] is
   input, not as the matrix A but rather as its LU decomposition, determined
   by the routine ludcmp. index[0..n-1] is input as the permutation vector
   returned by ludcmp. b[0..n-1] is input as the right-hand side vector B, and
   returns with the solution vector X. a, n, and index are not modified by this
   routine and can be left in place for successive calls with different
   right-hand sides b. This routine takes into account the possibility that b
   will begin with many zero elements, so it is efficient for use in matrix
   inversion.                                                                */
{
   int                  i,
                        ii = -1,
                        ip,
                        j;
   float                sum;

   for (i = 0; i < n; i++)
   {
      /* When ii is set to a value >= 0, it will become the index of the
         first nonvanishing element of b. We now do the forward substitution,
         equation (2.3.6). The only new wrinkle is to unscramble the
         permutation as we go.                                               */
      ip = index[i];
      sum = b[ip];
      b[ip] = b[i];
      if (ii >= 0)
         for (j = ii; j < i; j++)
            sum -= a[i][j] * b[j];
      else if (sum)
         ii = i;                          /* A nonzero element was encountered,
                                             so from now on we will have to do
                                             the sums in the loop above.     */
      b[i] = sum;
   }
   for (i = n - 1; i >= 0; i--)
   {                                      /* Now we do the backsubstitution,
                                             equation (2.3.7).               */
      sum = b[i];
      for (j = i + 1; j < n; j++)
         sum -= a[i][j] * b[j];
      b[i] = sum / a[i][i];               /* Store a component of the solution
                                             vector X.                       */
   }                                      /* All done!                       */
}
/* The LU decomposition in ludcmp requires about 1/3 N^3 executions of the
   inner loops (each with one multiply and one add). This is thus the
   operation count for solving one (or a few) right-hand sides, and is a
   factor of 3 better than the Gauss-Jordan routine gaussj which was given in
   Section 2.1, and a factor of 1.5 better than a Gauss-Jordan routine (not
   given) that does not compute the inverse matrix. For inverting a matrix,
   the total count (including the forward and backsubstitution as discussed
   following equation 2.3.7 above) is ( 1/3 + 1/6 + 1/2 )N^3 = N^3, the same
   as gaussj. To summarize, this is the preferred way to solve the linear set
   of equations A . x = b:                                                   */
/* float **a,*b,d; */
/* int n,*index; */
/* ... */
/* ludcmp(a,n,index,&d); */
/* lubksb(a,n,index,b); */
/* The answer x will be given back in b. Your original matrix A will have
   been destroyed.                                                           */
