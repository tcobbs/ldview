#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include <stdlib.h>
#include <LDLib/LDrawModelViewer.h>

class PreferencesPanel;
class ModelViewerWidget;
class QButton;
class QRangeControl;
class QString;
class TCColorButton;

typedef enum
{
	LDVViewExamine,
	LDVViewFlythrough
} LDVViewMode;

typedef enum
{
	LDVPollNone,
	LDVPollPrompt,
	LDVPollAuto,
	LDVPollBackground
} LDVPollMode;

class Preferences
{
public:
	Preferences(ModelViewerWidget *modelWidget = NULL);
	~Preferences(void);

	void doApply(void);
	void doCancel(void);
	void doResetGeneral(void);
	void doResetGeometry(void);
	void doResetEffects(void);
	void doResetPrimitives(void);
	void doWireframeCutaway(bool value);
	void doLighting(bool value);
	void doStereo(bool value);
	void doWireframe(bool value);
	void doEdgeLines(bool value);
	void doPrimitiveSubstitution(bool value);
	void doTextureStuds(bool value);

	void show(void);
	bool getAllowPrimitiveSubstitution(void);
	void getRGB(int color, int &r, int &g, int &b);
	int getBackgroundColor(void) { return backgroundColor; }
	bool getShowErrors(void) { return showErrors; }

	void setShowError(int errorNumber, bool value);
	bool getShowError(int errorNumber);

	bool getStatusBar(void) { return statusBar; }
	void setStatusBar(bool value);
	void setButtonState(QButton *button, bool state);
	void setWindowSize(int width, int height);
	int getWindowWidth(void);
	int getWindowHeight(void);

	static char *getLastOpenPath(char *pathKey = NULL);
	static void setLastOpenPath(const char *path, char *pathKey = NULL);
	static char *getLDrawDir(void);
	static void setLDrawDir(const char *path);
	static long getMaxRecentFiles(void);
	static char *getRecentFile(int index);
	static void setRecentFile(int index, char *filename);
	static LDVPollMode getPollMode(void);
	static void setPollMode(LDVPollMode value);
	static LDVViewMode getViewMode(void);
	static void setViewMode(LDVViewMode value);

protected:
	void doGeneralApply(void);
	void doGeometryApply(void);
	void doEffectsApply(void);
	void doPrimitivesApply(void);

	void loadSettings(void);
	void loadGeneralSettings(void);
	void loadGeometrySettings(void);
	void loadEffectsSettings(void);
	void loadPrimitivesSettings(void);
	void loadOtherSettings(void);

	void loadDefaultGeneralSettings(void);
	void loadDefaultGeometrySettings(void);
	void loadDefaultEffectsSettings(void);
	void loadDefaultPrimitivesSettings(void);
	void loadDefaultOtherSettings(void);

	void reflectSettings(void);
	void reflectGeneralSettings(void);
	void reflectGeometrySettings(void);
	void reflectWireframeSettings(void);
	void reflectEffectsSettings(void);
	void reflectPrimitivesSettings(void);

	void setRangeValue(QRangeControl *rangeConrol, int value);
	void setupColorButton(TCColorButton *button, int colorNumber);

	void enableWireframeCutaway(void);
	void enableLighting(void);
	void enableStereo(void);
	void enableWireframe(void);
	void enableEdgeLines(void);
	void enablePrimitiveSubstitution(void);
	void enableTextureStuds(void);

	void disableWireframeCutaway(void);
	void disableLighting(void);
	void disableStereo(void);
	void disableWireframe(void);
	void disableEdgeLines(void);
	void disablePrimitiveSubstitution(void);
	void disableTextureStuds(void);
	
	char *getErrorKey(int errorNumber);
	static const QString &getRecentFileKey(int index);

	ModelViewerWidget *modelWidget;
	LDrawModelViewer *modelViewer;
	PreferencesPanel *panel;

	// General Settings
	bool lineSmoothing;
	bool showFPS;
	bool showErrors;
	int backgroundColor;
	int defaultColor;
	int defaultColorNumber;	// No UI for this.

	// Geometry Settings
	bool seams;
	int seamWidth;
	bool wireframe;
	bool wireframeFog;
	int wireframeThickness;
	bool edgeLines;
	bool conditionalLines;
	bool polygonOffset;
	bool blackEdgeLines;
	int edgeThickness;

	// Effects Settings
	bool lighting;
	bool qualityLighting;
	bool subduedLighting;
	bool specular;
	bool alternateLighting;
	LDVStereoMode stereoMode;
	long stereoEyeSpacing;
	LDVCutawayMode cutawayMode;
	long cutawayAlpha;
	long cutawayThickness;
	bool sortTransparent;
	bool stipple;
	bool flatShading;
	bool smoothing;

	// Primitives Settings
	bool allowPrimitiveSubstitution;
	bool textureStuds;
	int textureFilterType;
	int curveQuality;
	bool qualityStuds;

	// Other Settings
	bool statusBar;
	int windowWidth;
	int windowHeight;
};

#endif // __PREFERENCES_H__
