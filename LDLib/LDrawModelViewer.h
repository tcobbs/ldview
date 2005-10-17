#ifndef __LDRAWMODELVIEWER_H__
#define __LDRAWMODELVIEWER_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStringArray.h>
#include <TRE/TRECamera.h>
#include <TRE/TREGL.h>

#define L3ORDERM 6
#define L3ORDERN 6

typedef enum
{
	LDVStereoNone,
	LDVStereoHardware,
	LDVStereoCrossEyed,
	LDVStereoParallel
} LDVStereoMode;

typedef enum
{
	LDVCutawayNormal,
	LDVCutawayWireframe,
	LDVCutawayStencil
} LDVCutawayMode;

typedef enum
{
	LDVAngleDefault,
	LDVAngleFront,
	LDVAngleBack,
	LDVAngleLeft,
	LDVAngleRight,
	LDVAngleTop,
	LDVAngleBottom,
	LDVAngleIso,
} LDVAngle;

class TCImage;
class LDLError;
class TCProgressAlert;
class TREMainModel;

class LDrawModelViewer: public TCObject
{
	public:
		LDrawModelViewer(int, int);
		virtual void update(void);
		virtual void perspectiveView(void);
		virtual void perspectiveView(bool resetViewport);
		void setQualityLighting(bool value) { flags.qualityLighting = value; }
		bool getQualityLighting(void) { return flags.qualityLighting; }
		virtual void setSubduedLighting(bool value);
		bool getSubduedLighting(void) { return flags.subduedLighting; }
		virtual void setShowsHighlightLines(bool value);
		bool getShowsHighlightLines(void) { return flags.showsHighlightLines; }
		virtual void setDrawConditionalHighlights(bool value);
		bool getDrawConditionalHighlights(void)
		{
			return flags.drawConditionalHighlights;
		}
		virtual void setPerformSmoothing(bool value);
		bool getPerformSmoothing(void)
		{
			return flags.performSmoothing;
		}
		void setConstrainZoom(bool value) { flags.constrainZoom = value; }
		bool getConstrainZoom(void) { return flags.constrainZoom; }
		virtual void setLineSmoothing(bool value);
		bool getLineSmoothing(void)
		{
			return flags.lineSmoothing;
		}
		virtual void setMemoryUsage(int value);
		int getMemoryUsage(void) { return memoryUsage; }
		virtual void setQualityStuds(bool value);
/*
		{
			flags.qualityStuds = value;
			flags.needsReload = true;
		}
*/
		bool getQualityStuds(void) { return flags.qualityStuds; }
		virtual void setAllowPrimitiveSubstitution(bool value);
/*
		{
			flags.allowPrimitiveSubstitution = value;
			flags.needsReload = true;
		}
*/
		bool getAllowPrimitiveSubstitution(void)
		{
			return flags.allowPrimitiveSubstitution;
		}
		void setUsesFlatShading(bool value) { flags.usesFlatShading = value; }
		bool getUsesFlatShading(void) { return flags.usesFlatShading; }
		virtual void setUsesSpecular(bool value);
		bool getUsesSpecular(void) { return flags.usesSpecular; }
		virtual void setOneLight(bool value);
		bool getOneLight(void) { return flags.oneLight; }
		virtual void setWidth(int value);
		virtual void setHeight(int value);
		int getWidth(void) { return width; }
		int getHeight(void) { return height; }
		void setXRotate(float value) { xRotate = value; }
		void setYRotate(float value) { yRotate = value; }
		void setZRotate(float value) { zRotate = value; }
		float getXRotate(void) { return xRotate; }
		float getYRotate(void) { return yRotate; }
		float getZRotate(void) { return zRotate; }
		void panXY(int xValue, int yValue);
		float getXPan(void) { return xPan; }
		float getYPan(void) { return yPan; }
		void setXYPan(float xValue, float yValue);
		void setRotationSpeed(float value);
		void setCameraXRotate(float value) { cameraXRotate = value; }
		void setCameraYRotate(float value) { cameraYRotate = value; }
		void setCameraZRotate(float value) { cameraZRotate = value; }
		float getCameraXRotate(void) { return cameraXRotate; }
		float getCameraYRotate(void) { return cameraYRotate; }
		float getCameraZRotate(void) { return cameraZRotate; }
		void setCameraMotion(const TCVector &value) { cameraMotion = value; }
		TCVector getCameraMotion(void) { return cameraMotion; }
		virtual void setZoomSpeed(float value);
		float getZoomSpeed(void) { return zoomSpeed; }
		virtual void zoom(float amount, bool apply = true);
		virtual void updateCameraPosition(void);
		virtual void applyZoom(void);
		void setClipZoom(bool value) { clipZoom = value; }
		bool getClipZoom(void) { return clipZoom; }
		virtual void setFilename(const char*);
		virtual void setProgramPath(const char *value);
		virtual void setFileIsPart(bool);
		char* getFilename(void) { return filename; }
		virtual int loadModel(bool = true);
		virtual void drawFPS(float);
		virtual void drawBoundingBox(void);
		virtual void setup(void);
		virtual void setBackgroundRGB(int r, int g, int b);
		virtual void setBackgroundRGBA(int r, int g, int b, int a);
		int getBackgroundR(void) { return (int)(backgroundR * 255); }
		int getBackgroundG(void) { return (int)(backgroundG * 255); }
		int getBackgroundB(void) { return (int)(backgroundB * 255); }
		int getBackgroundA(void) { return (int)(backgroundA * 255); }
		virtual void setDefaultRGB(TCByte r, TCByte g, TCByte b,
			bool transparent);
		virtual void getDefaultRGB(TCByte &r, TCByte &g, TCByte &b,
			bool &transparent);
		virtual void setDefaultColorNumber(int value);
		int getDefaultColorNumber(void) { return defaultColorNumber; }
		virtual void setSeamWidth(float);
		float getSeamWidth(void) { return seamWidth; }
		virtual void setDrawWireframe(bool);
		bool getDrawWireframe(void) { return flags.drawWireframe; }
		virtual void setBfc(bool value);
		bool getBfc(void) { return flags.bfc != false; }
		virtual void setRedBackFaces(bool value);
		bool getRedBackFaces(void) { return flags.redBackFaces != false; }
		virtual void setGreenFrontFaces(bool value);
		bool getGreenFrontFaces(void) { return flags.greenFrontFaces != false; }
		virtual void setUseWireframeFog(bool);
		bool getUseWireframeFog(void) { return flags.useWireframeFog; }
		virtual void setRemoveHiddenLines(bool value);
		bool getRemoveHiddenLines(void) { return flags.removeHiddenLines; }
		virtual void setEdgesOnly(bool value);
		bool getEdgesOnly(void) { return flags.edgesOnly; }
		virtual void setHiResPrimitives(bool value);
		bool getHiResPrimitives(void) { return flags.hiResPrimitives; }
		virtual void setUsePolygonOffset(bool);
		bool getUsePolygonOffset(void) { return flags.usePolygonOffset; }
		virtual void setUseLighting(bool);
		bool getUseLighting(void) { return flags.useLighting; }
		virtual void setUseStipple(bool);
		bool getUseStipple(void) { return flags.useStipple; }
		virtual void setSortTransparent(bool);
		bool getSortTransparent(void) { return flags.sortTransparent; }
		virtual void setHighlightLineWidth(float value);
		float getHighlightLineWidth(void) { return highlightLineWidth; }
		virtual void setWireframeLineWidth(float value);
		float getWireframeLineWidth(void) { return wireframeLineWidth; }
		virtual void setProcessLDConfig(bool value);
		bool getProcessLDConfig(void) { return flags.processLDConfig; }
		virtual void setSkipValidation(bool value);
		bool getSkipValidation(void) { return flags.skipValidation != false; }
		void setAutoCenter(bool value) { flags.autoCenter = value; }
		bool getAutoCenter(void) { return flags.autoCenter; }
		virtual void setForceZoomToFit(bool value);
		bool getForceZoomToFit(void) { return flags.forceZoomToFit; }
		virtual bool recompile(void);
		virtual void uncompile(void);
		virtual void reload(void);
//		virtual void setProgressCallback(LDMProgressCallback callback,
//			void* userData);
//		virtual void setErrorCallback(LDMErrorCallback callback,
//			void* userData);
		virtual void clear(void);
		virtual void resetView(LDVAngle viewAngle = LDVAngleDefault);
		virtual void pause(void);
		virtual void unpause(void);
		bool getPaused(void) { return flags.paused; }
		virtual void setXTile(int value);
		int getXTile(void) { return xTile; }
		virtual void setYTile(int value);
		int getYTile(void) { return yTile; }
		virtual void setNumXTiles(int value);
		int getNumXTiles(void) { return numXTiles; }
		virtual void setNumYTiles(int value);
		int getNumYTiles(void) { return numYTiles; }
		virtual void setStereoMode(LDVStereoMode mode);
		LDVStereoMode getStereoMode(void) { return stereoMode; }
		void setStereoEyeSpacing(GLfloat spacing)
		{
			stereoEyeSpacing = spacing;
		}
		GLfloat getStereoEyeSpacing(void) { return stereoEyeSpacing; }
		virtual void setCutawayMode(LDVCutawayMode mode);
		LDVCutawayMode getCutawayMode(void) { return cutawayMode; }
		virtual void setCutawayAlpha(GLfloat value);
		GLfloat getCutawayAlpha(void) { return cutawayAlpha; }
		virtual void setCutawayLineWidth(GLfloat value);
		GLfloat getCutawayLineWidth(void) { return cutawayLineWidth; }
		void setSlowClear(bool value) { flags.slowClear = value; }
		bool getSlowClear(void) { return flags.slowClear; }
		virtual void setBlackHighlights(bool value);
		bool getBlackHighlights(void) { return flags.blackHighlights; }
		void setZoomMax(float value) { zoomMax = value; }
		float getZoomMax(void) { return zoomMax; }
		virtual void setShowAllConditionalLines(bool value);
		bool getShowAllConditionalLines(void)
		{
			return flags.showAllConditionalLines;
		}
		virtual void setShowConditionalControlPoints(bool value);
		bool getShowConditionalControlPoints(void)
		{
			return flags.showConditionalControlPoints;
		}
		bool getNeedsReload(void) { return flags.needsReload != 0; }
		bool getNeedsRecompile(void) { return flags.needsRecompile != 0; }
		void setCurveQuality(int value);
		int getCurveQuality(void) { return curveQuality; }
		void setTextureStuds(bool value);
		bool getTextureStuds(void) { return flags.textureStuds; }
		void setTextureFilterType(int value);
		int getTextureFilterType(void) { return textureFilterType; }
		TREMainModel *getMainTREModel(void) { return mainTREModel; }
		bool getCompiled(void);
		void setPixelAspectRatio(float value) { pixelAspectRatio = value; }
		float getPixelAspectRatio(void) { return pixelAspectRatio; }
		bool getLDrawCommandLineMatrix(char *matrixString, int bufferLength);
		bool getLDrawCommandLine(char *shortFilename, char *commandString,
			int bufferLength);
		bool getLDGLiteCommandLine(char *commandString, int bufferLength);
		void setDistanceMultiplier(GLfloat value)
		{
			distanceMultiplier = value;
		}
		GLfloat getDistanceMultiplier(void) { return distanceMultiplier; }
		virtual void clearBackground(void);
		virtual void setFontData(TCByte *fontData, long length);
		virtual void setDefaultRotationMatrix(const float *value);
		const float *getDefaultRotationMatrix(void)
		{
			return defaultRotationMatrix;
		}
		const float *getRotationMatrix(void) { return rotationMatrix; }
		virtual void setFov(float value);
		float getFov(void) { return fov; }
		float getDefaultDistance(void) { return defaultDistance; }
		void setExtraSearchDirs(TCStringArray *value);
		TCStringArray *getExtraSearchDirs(void) { return extraSearchDirs; }
		bool skipCameraPositioning(void);
		virtual TRECamera &getCamera(void) { return camera; }
		virtual void zoomToFit(void);
		virtual void openGlWillEnd(void);
		virtual void setLightVector(const TCVector &value);
		TCVector getLightVector(void) { return lightVector; }

		static char *getOpenGLDriverInfo(int &numExtensions);
	protected:
		~LDrawModelViewer(void);
		void dealloc(void);
		virtual void drawSetup(GLfloat eyeXOffset = 0.0f);
		virtual void drawModel(GLfloat eyeXOffset = 0.0f);
		virtual void removeHiddenLines(GLfloat eyeXOffset = 0.0f);
		virtual void setFieldOfView(double, float, float);
		virtual void setupRotationMatrix(void);
		virtual void setupMaterial(void);
		virtual void setupLight(GLenum);
		virtual void orthoView(void);
		void drawLight(GLenum, float, float, float);
		virtual void setupLighting(void);
		virtual void setupTextures(void);
		void drawLights(void);
		virtual void drawToClipPlane(GLfloat eyeXOffset);
		virtual void drawToClipPlaneUsingStencil(GLfloat eyeXOffset);
//		virtual void drawToClipPlaneUsingAccum(GLfloat eyeXOffset);
		virtual void drawToClipPlaneUsingDestinationAlpha(GLfloat eyeXOffset);
		virtual void drawToClipPlaneUsingNoEffect(GLfloat eyeXOffset);
		virtual void perspectiveViewToClipPlane(void);
		virtual void applyTile(void);
		virtual void drawString(GLfloat xPos, GLfloat yPos, char* string);
		virtual void setupFont(char *fontFilename);
		virtual void loadVGAFont(char *fontFilename);
		virtual void setupDefaultViewAngle(void);
		virtual void setupFrontViewAngle(void);
		virtual void setupBackViewAngle(void);
		virtual void setupLeftViewAngle(void);
		virtual void setupRightViewAngle(void);
		virtual void setupTopViewAngle(void);
		virtual void setupBottomViewAngle(void);
//		void ldlErrorCallback(LDLError *error);
//		void progressAlertCallback(TCProgressAlert *error);
		virtual void setupIsoViewAngle(void);
		virtual void preCalcCamera(void);
		virtual float calcDefaultDistance(void);
		virtual void updateCurrentFov(void);
		virtual float getStereoWidthModifier(void);
		virtual float getWideLineMargin(void);
		virtual float getClipRadius(void);
		virtual float getZDistance(void);
		virtual bool forceOneLight(void);

		int L3Solve6(float x[L3ORDERN], const float A[L3ORDERM][L3ORDERN],
			const float b[L3ORDERM]);

		void scanCameraPoint(const TCVector &point);

		TREMainModel *mainTREModel;
		char* filename;
		char* programPath;
		int width;
		int height;
		float pixelAspectRatio;
		float size;
		TCVector center;
		TCVector boundingMin;
		TCVector boundingMax;
		int lastFrameTimeLength;
		int cullBackFaces;
		float xRotate;
		float yRotate;
		float zRotate;
		float rotationSpeed;
		float cameraXRotate;
		float cameraYRotate;
		float cameraZRotate;
		TCVector cameraMotion;
		float zoomSpeed;
		float xPan;
		float yPan;
		float* rotationMatrix;
		float* defaultRotationMatrix;
		float clipAmount;
		float nextClipAmount;
		float nextDistance;
		float highlightLineWidth;
		float wireframeLineWidth;
		bool clipZoom;
		GLuint fontListBase;
		GLclampf backgroundR;
		GLclampf backgroundG;
		GLclampf backgroundB;
		GLclampf backgroundA;
		TCByte defaultR;
		TCByte defaultG;
		TCByte defaultB;
		int defaultColorNumber;
//		LDMProgressCallback progressCallback;
//		void* progressUserData;
//		LDMErrorCallback errorCallback;
//		void* errorUserData;
		int xTile;
		int yTile;
		int numXTiles;
		int numYTiles;
		LDVStereoMode stereoMode;
		GLfloat stereoEyeSpacing;
		LDVCutawayMode cutawayMode;
		GLfloat cutawayAlpha;
		GLfloat cutawayLineWidth;
		float zoomMax;
		int curveQuality;
		int textureFilterType;
		GLfloat distanceMultiplier;
		TCImage *fontImage;
		GLuint fontTextureID;
		TRECamera camera;
		float aspectRatio;
		float currentFov;
		float fov;
		float defaultDistance;
		TCStringArray *extraSearchDirs;
		float seamWidth;
		float zoomToFitWidth;
		float zoomToFitHeight;
		int memoryUsage;
		TCVector lightVector;
		struct
		{
			bool qualityLighting:1;
			bool showsHighlightLines:1;
			bool qualityStuds:1;
			bool usesFlatShading:1;
			bool usesSpecular:1;
			bool drawWireframe:1;
			bool useWireframeFog:1;
			bool removeHiddenLines:1;
			bool usePolygonOffset:1;
			bool useLighting:1;
			bool subduedLighting:1;
			bool allowPrimitiveSubstitution:1;
			bool useStipple:1;
			bool fileIsPart:1;
			bool sortTransparent:1;
			bool needsSetup:1;
			bool needsTextureSetup:1;
			bool needsMaterialSetup:1;
			bool needsLightingSetup:1;
			bool needsReload:1;
			bool needsRecompile:1;
			bool needsResize:1;
			bool paused:1;
			bool slowClear:1;
			bool blackHighlights:1;
			bool textureStuds:1;
			bool oneLight:1;
			bool drawConditionalHighlights:1;
			bool showAllConditionalLines:1;
			bool showConditionalControlPoints:1;
			bool performSmoothing:1;
			bool lineSmoothing:1;
			bool constrainZoom:1;
			bool needsRotationMatrixSetup:1;
			bool edgesOnly:1;
			bool hiResPrimitives:1;
			bool needsViewReset:1;
			bool processLDConfig:1;
			bool skipValidation:1;
			bool autoCenter:1;
			bool forceZoomToFit:1;
			bool defaultTrans:1;
			bool bfc:1;
			bool redBackFaces:1;
			bool greenFrontFaces:1;
			bool defaultLightVector:1;
		} flags;
		struct CameraData
		{
			CameraData(void)
				:direction(0.0f, 0.0f, -1.0f),
				horizontal(1.0f, 0.0f, 0.0f),
				vertical(0.0f, -1.0f, 0.0f),
				horMin(1e6),
				horMax(-1e6),
				verMin(1e6),
				verMax(-1e6)
			{
				int i;
//				TCVector up = vertical;

//				horizontal = direction * up;
//				vertical = horizontal * direction;
				for (i = 0; i < 4; i++)
				{
					dMin[i] = 1e6;
				}
			}
			TCVector direction;
			TCVector horizontal;
			TCVector vertical;
			TCVector normal[4];
			float dMin[4];
			float horMin;
			float horMax;
			float verMin;
			float verMax;
			float fov;
		} *cameraData;
};

#endif // __LDRAWMODELVIEWER_H__
