#ifndef __LDVPREFERENCES_H__
#define __LDVPREFERENCES_H__

//#include <TCFoundation/TCObject.h>
#include <CUI/CUIPropertySheet.h>
#include <CUI/CUIThemes.h>
#include <windows.h>

#include <LDLib/LDrawModelViewer.h>

//class LDrawModelViewer;

#define LDP_DONE 0x01
#define LDP_UNKNOWN_COMMAND 0x02
/*
#define LDP_NEEDS_SETUP 0x04
#define LDP_NEEDS_RELOAD 0x08
#define LDP_NEEDS_RECOMPILE 0x10
*/

class LDViewPreferences: public CUIPropertySheet
{
public:
	LDViewPreferences(HINSTANCE hInstance,
		LDrawModelViewer *modelViewer = NULL);

	bool getQualityLighting(void) { return qualityLighting; }
	bool getShowsFPS(void) { return showsFPS; }
	bool getShowsHighlightLines(void) { return showsHighlightLines; }
	bool getDrawConditionalHighlights(void)
	{
		return drawConditionalHighlights;
	}
	bool getPerformSmoothing(void) { return performSmoothing; }
	bool getLineSmoothing(void) { return lineSmoothing; }
	bool getQualityStuds(void) { return qualityStuds; }
	bool getAllowPrimitiveSubstitution(void)
	{
		return allowPrimitiveSubstitution;
	}
	bool getUsesFlatShading(void) { return usesFlatShading; }
	bool getUsesSpecular(void) { return usesSpecular; }
	COLORREF getBackgroundColor(void) { return backgroundColor; }
	COLORREF* getCustomColors(void) { return customColors; }
	int getUseSeams(void) { return useSeams; }
	int getSeamWidth(void) { return seamWidth; }
	bool getDrawWireframe(void) { return drawWireframe; }
	bool getUseWireframeFog(void) { return useWireframeFog; }
	bool getRemoveHiddenLines(void) { return removeHiddenLines; }
	bool getUsePolygonOffset(void) { return usePolygonOffset; }
	bool getUseLighting(void) { return useLighting; }
	bool getSubduedLighting(void) { return subduedLighting; }
	int getFullScreenRefresh(void) { return fullScreenRefresh; }
	bool getUseStipple(void) { return useStipple; }
	bool getSortTransparent(void) { return sortTransparent; }
	virtual void applyChanges(void);
	virtual int run(void);

	virtual BOOL doDialogThemeChanged(void);
	virtual BOOL doDialogVScroll(HWND hDlg, int scrollCode, int position,
		HWND hScrollBar);
	int getGeneralPageNumber(void) { return generalPageNumber; }
	int getGeometryPageNumber(void) { return geometryPageNumber; }
	int getEffectsPageNumber(void) { return effectsPageNumber; }
	bool performHotKey(int hotKeyIndex);

	virtual void saveDefaultView(void);
	virtual void resetDefaultView(void);

	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	void loadSettings(void);
	void applySettings(void);
	// *************************************************************************

	static char* getLDViewPath(char* helpFilename, bool useQuotes = false);
	static char* getLDViewPath(bool useQuotes = false);
	static int getFSAAFactor(void);
	static bool getUseNvMultisampleFilter(void);
protected:
	virtual ~LDViewPreferences(void);
	virtual void dealloc(void);
	virtual void setupColorButton(HWND hPage, HWND &hColorButton,
		int controlID, HBITMAP &hButtonBitmap, COLORREF color, HTHEME &hTheme);
	virtual void setupFov(bool warn = false);
	virtual void setupBackgroundColorButton(void);
	virtual void setupDefaultColorButton(void);
	virtual void setupSeamWidth(void);
	virtual void setupFullScreenRefresh(void);
	virtual void redrawColorBitmap(HWND hColorButton, HBITMAP hButtonBitmap,
		COLORREF color, HTHEME hTheme);
	virtual void getRGB(int color, int &r, int &g, int &b);
	virtual void chooseColor(HWND hColorButton, HBITMAP hColorBitmap,
		COLORREF &color, HTHEME hTheme);
	virtual void chooseBackgroundColor(void);
	virtual void chooseDefaultColor(void);
	virtual void doFSRefresh(void);
	virtual void doStipple(void);
	virtual void doSort(void);
	virtual void doFlatShading(void);
	virtual void doSmoothCurves(void);
	virtual void doHighlights(void);
	virtual void doConditionals(void);
	virtual void doWireframe(void);
	virtual void doLighting(void);
	virtual void doStereo(void);
	virtual void doCutaway(void);
	virtual void doDeletePrefSet(void);
	virtual void doNewPrefSet(void);
	virtual void doPrefSetHotKey(void);
	virtual void doSeams(void);
	virtual void doPrimitives(void);
	virtual void doTextureStuds(void);
	virtual bool doApply(void);
	virtual DWORD doClick(HWND hPage, int controlId, HWND controlHWnd);
	virtual DWORD doComboSelChange(HWND hPage, int controlId, HWND controlHWnd);
	virtual BOOL doPrefSetSelected(bool force = false);
	virtual void doGeneralClick(int controlId, HWND controlHWnd);
	virtual void doGeometryClick(int controlId, HWND controlHWnd);
	virtual void doEffectsClick(int controlId, HWND controlHWnd);
	virtual void doPrimitivesClick(int controlId, HWND controlHWnd);
	virtual void doPrefSetsClick(int controlId, HWND controlHWnd);
	virtual void doOtherClick(HWND hDlg, int controlId, HWND controlHWnd);
//	virtual void drawButtonBorder(HDC hdc, COLORREF color1, COLORREF color2,
//		RECT rect);
	virtual BOOL doDialogInit(HWND hDlg, HWND hFocusWindow, LPARAM lParam);
	virtual BOOL doNewPrefSetInit(HWND hDlg, HWND hNewPrefSetField);
	virtual BOOL doHotKeyInit(HWND hDlg, HWND hHotKeyCombo);
	virtual BOOL doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification);
	virtual BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
		HWND controlHWnd);
	virtual BOOL doDialogHelp(HWND hDlg, LPHELPINFO helpInfo);
	virtual DWORD getPageDialogID(HWND hDlg);
	virtual void doReset(void);
	virtual void setupPage(int pageNumber);
	virtual void setupGeneralPage(void);
	virtual void setupGeometryPage(void);
	virtual void setupEffectsPage(void);
	virtual void setupPrimitivesPage(void);
	virtual void setupPrefSetsPage(void);
	virtual void setupAntialiasing(void);
	virtual void setupModelGeometry(void);
	virtual void setupWireframe(void);
	virtual void setupSubstitution(void);
	virtual void setupPrefSetsList(void);
	virtual void enableWireframe(void);
	virtual void disableWireframe(void);
	virtual void setupEdgeLines(void);
	virtual void enableEdges(void);
	virtual void disableEdges(void);
	virtual void enableConditionals(void);
	virtual void disableConditionals(void);
	virtual void applyPrefSetsChanges(void);
	virtual void applyGeneralChanges(void);
	virtual void applyGeometryChanges(void);
	virtual void applyEffectsChanges(void);
	virtual void applyPrimitivesChanges(void);
	virtual void clear(void);
	virtual void setupStereo(void);
	virtual void enableStereo(void);
	virtual void disableStereo(void);
	virtual void setupCutaway(void);
	virtual void enableCutaway(void);
	virtual void disableCutaway(void);
	virtual void setupLighting(void);
	virtual void enableLighting(void);
	virtual void disableLighting(void);
	virtual void enableSeams(void);
	virtual void disableSeams(void);
	virtual void enablePrimitives(void);
	virtual void disablePrimitives(void);
	virtual void enableTextureFiltering(void);
	virtual void disableTextureFiltering(void);
	virtual void setupOpacitySlider(void);
	virtual char *getPrefSet(int index);
	virtual char *getSelectedPrefSet(void);
	virtual void selectPrefSet(const char *prefSet = NULL, bool force = false);
	virtual int runPrefSetApplyDialog(void);
	virtual bool shouldSetActive(int index);
	virtual void abandonChanges(void);
	virtual char *getHotKey(int index);
	virtual int getHotKey(const char *currentPrefSetName);
	virtual int getCurrentHotKey(void);
	virtual void saveCurrentHotKey(void);
	virtual BOOL dialogProc(HWND hDlg, UINT message, WPARAM wParam,
		LPARAM lParam);
	virtual float getMinFov(void);
	virtual float getMaxFov(void);


	// These are called from the constructor, and cannot be properly made into
	// virtual functions.
	// *************************************************************************
	virtual void setupDefaultRotationMatrix(void);
	void applyGeneralSettings(void);
	void applyGeometrySettings(void);
	void applyEffectsSettings(void);
	void applyPrimitivesSettings(void);
	void loadDefaultGeneralSettings(void);
	void loadDefaultGeometrySettings(void);
	void loadDefaultEffectsSettings(void);
	void loadDefaultPrimitivesSettings(void);
	void loadGeneralSettings(void);
	void loadGeometrySettings(void);
	void loadEffectsSettings(void);
	void loadPrimitivesSettings(void);
	// *************************************************************************


	BOOL doDrawColorButton(HWND hDlg, HWND hWnd, HTHEME hTheme,
		LPDRAWITEMSTRUCT drawItemStruct);

	LRESULT colorButtonProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);

	static LRESULT CALLBACK staticColorButtonProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);

	LDrawModelViewer* modelViewer;

	int fsaaMode;
	bool lineSmoothing;
	COLORREF backgroundColor;
	COLORREF defaultColor;
	bool transDefaultColor;
	int defaultColorNumber;
	bool processLDConfig;
	bool showsFPS;
	bool showErrors;
	int fullScreenRefresh;
	float fov;
	float defaultZoom;

	int useSeams;
	int seamWidth;
	bool drawWireframe;
	bool useWireframeFog;
	bool removeHiddenLines;
	long wireframeThickness;
	bool showsHighlightLines;
	bool drawConditionalHighlights;
	bool showAllConditionalLines;
	bool showConditionalControlPoints;
	bool edgesOnly;
	bool usePolygonOffset;
	bool blackHighlights;
	long edgeThickness;

	bool useLighting;
	bool qualityLighting;
	bool subduedLighting;
	bool usesSpecular;
	bool oneLight;
	LDVStereoMode stereoMode;
	long stereoEyeSpacing;
	LDVCutawayMode cutawayMode;
	long cutawayAlpha;
	long cutawayThickness;
	bool sortTransparent;
	bool performSmoothing;
	bool useStipple;
	bool usesFlatShading;

	bool allowPrimitiveSubstitution;
	bool textureStuds;
	int textureFilterType;
	long curveQuality;
	bool qualityStuds;
	bool hiResPrimitives;

	COLORREF customColors[16];
	float zoomMax;

	int generalPageNumber;
	int geometryPageNumber;
	int effectsPageNumber;
	int primitivesPageNumber;
	int prefSetsPageNumber;

	char *newPrefSetName;
	int hotKeyIndex;

	HWND hGeneralPage;
	HWND hFullScreenRateField;
	HBITMAP hBackgroundColorBitmap;
	HWND hBackgroundColorButton;
	HBITMAP hDefaultColorBitmap;
	HWND hDefaultColorButton;
	HDC hButtonColorDC;
	HTHEME hBackgroundColorTheme;
	HTHEME hDefaultColorTheme;
	HWND hMouseOverButton;
	long previousButtonWindowProc;

	HWND hGeometryPage;
	HWND hSeamSpin;
	HWND hWireframeFogButton;
	HWND hRemoveHiddenLinesButton;
	HWND hWireframeThicknessLabel;
	HWND hWireframeThicknessSlider;
	HWND hEdgesOnlyButton;
	HWND hConditionalHighlightsButton;
	HWND hShowAllConditionalButton;
	HWND hShowConditionalControlsButton;
	HWND hQualityLinesButton;
	HWND hAlwaysBlackButton;
	HWND hEdgeThicknessLabel;
	HWND hEdgeThicknessSlider;

	HWND hEffectsPage;
	HWND hLightQualityButton;
	HWND hLightSubduedButton;
	HWND hLightSpecularButton;
	HWND hLightAlternateButton;
	HWND hHardwareStereoButton;
	HWND hCrossEyedStereoButton;
	HWND hParallelStereoButton;
	HWND hStereoSpacingSlider;
	HWND hStereoSpacingLabel;
	HWND hCutawayColorButton;
	HWND hCutawayMonochromButton;
	HWND hCutawayOpacityLabel;
	HWND hCutawayOpacitySlider;
	HWND hCutawayThicknessLabel;
	HWND hCutawayThicknessSlider;

	HWND hPrimitivesPage;
	HWND hTextureStudsButton;
	HWND hTextureNearestButton;
	HWND hTextureBilinearButton;
	HWND hTextureTrilinearButton;
	HWND hCurveQualityLabel;
	HWND hCurveQualitySlider;

	HWND hPrefSetsPage;
	HWND hPrefSetsList;
	HWND hDeletePrefSetButton;
	HWND hNewPrefSetButton;
	HWND hPrefSetHotKeyButton;

	bool setActiveWarned;
	bool checkAbandon;

	static char ldviewPath[MAX_PATH];
};

#endif // __LDVPREFERENCES_H__
