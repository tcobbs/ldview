#ifndef __LDVPREFERENCES_H__
#define __LDVPREFERENCES_H__

//#include <TCFoundation/TCObject.h>
#include <CUI/CUIPropertySheet.h>
#include <CUI/CUIThemes.h>
#include <windows.h>

#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCStlIncludes.h>

//class LDrawModelViewer;

#define LDP_DONE 0x01
#define LDP_UNKNOWN_COMMAND 0x02

typedef std::map<HWND, bool> HwndBoolMap;
typedef std::map<int, int> IntIntMap;
typedef std::vector<HWND> HwndVector;
typedef std::map<HWND, int> HwndIntMap;

class TCAlert;

class LDViewPreferences: public CUIPropertySheet
{
public:
	LDViewPreferences(HINSTANCE hInstance,
		LDrawModelViewer *modelViewer = NULL);

	bool getQualityLighting(void);
	void setQualityLighting(bool value);
	bool getShowsFPS(void);
	bool getShowsHighlightLines(void);
	void setShowsHighlightLines(bool value);
	bool getEdgesOnly(void);
	void setEdgesOnly(bool value);
	bool getDrawConditionalHighlights(void);
	void setDrawConditionalHighlights(bool value);
	bool getShowAllConditionalLines(void);
	void setShowAllConditionalLines(bool value);
	bool getShowConditionalControlPoints(void);
	void setShowConditionalControlPoints(bool value);
	bool getUseFlatShading(void);
	void setUseFlatShading(bool value);
	bool getBoundingBoxesOnly(void);
	void setBoundingBoxesOnly(bool value);
	bool getTransDefaultColor(void);
	void setTransDefaultColor(bool value);
	bool getPerformSmoothing(void);
	void setPerformSmoothing(bool value);
	bool getLineSmoothing(void);
	bool getQualityStuds(void);
	void setQualityStuds(bool value);
	bool getAllowPrimitiveSubstitution(void);
	void setAllowPrimitiveSubstitution(bool value);
	bool getUsesFlatShading(void);
	void setUsesFlatShading(bool value);
	bool getUsesSpecular(void);
	void setUsesSpecular(bool value);
	bool getOneLight(void);
	void setOneLight(bool value);
	bool getDrawLightDats(void);
	void setDrawLightDats(bool value);
	bool getOptionalStandardLight(void);
	void setOptionalStandardLight(bool value);
	COLORREF getBackgroundColor(void);
	COLORREF getDefaultColor(void);
	bool getUseSeams(void);
	void setUseSeams(bool value);
	int getSeamWidth(void);
	bool getDrawWireframe(void);
	void setDrawWireframe(bool value);
	bool getBfc(void);
	void setBfc(bool value);
	bool getRedBackFaces(void);
	void setRedBackFaces(bool value);
	bool getGreenFrontFaces(void);
	void setGreenFrontFaces(bool value);
	bool getBlueNeutralFaces(void);
	void setBlueNeutralFaces(bool value);
	bool getShowAxes(void);
	void setShowAxes(bool value);
	bool getRandomColors(void);
	void setRandomColors(bool value);
	bool getUseWireframeFog(void);
	void setUseWireframeFog(bool value);
	bool getRemoveHiddenLines(void);
	void setRemoveHiddenLines(bool value);
	bool getUsePolygonOffset(void);
	void setUsePolygonOffset(bool value);
	bool getBlackHighlights(void);
	void setBlackHighlights(bool value);
	bool getUseLighting(void);
	void setUseLighting(bool value);
	bool getSubduedLighting(void);
	void setSubduedLighting(bool value);
	int getFullScreenRefresh(void);
	bool getUseStipple(void);
	bool getSortTransparent(void);
	bool getTextureStuds(void);
	void setTextureStuds(bool value);
	bool getTexmaps(void);
	void setTexmaps(bool value);
	LDVCutawayMode getCutawayMode(void);
	void setCutawayMode(LDVCutawayMode value);
	virtual void applyChanges(void);
	virtual INT_PTR run(void);
	LDPreferences *getLDPrefs(void) { return ldPrefs; }
	void checkForDpiChange(void);

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

	int getFSAAFactor(void);
	bool getUseNvMultisampleFilter(void);

	void checkLightVector(void);

	static char* getLDViewPath(const char* helpFilename,
		bool useQuotes = false);
	static char* getLDViewPath(bool useQuotes = false);
	static COLORREF getColor(const char *key, COLORREF defaultColor = 0);
protected:
	virtual ~LDViewPreferences(void);
	virtual void dealloc(void);
	virtual void setupIconButton(HWND hButton);
	virtual void setupColorButton(HWND hPage, HWND &hColorButton,
		int controlID, HBITMAP &hButtonBitmap, COLORREF color);
	virtual void setupMemoryUsage(void);
	virtual void setupFov(bool warn = false);
	virtual void setupBackgroundColorButton(void);
	virtual void setupDefaultColorButton(void);
	virtual void setupSeamWidth(void);
	virtual void setupFullScreenRefresh(void);
	virtual void setupSaveDirs(void);
	virtual void updateSaveDir(HWND hTextField, HWND hBrowseButton,
		LDPreferences::DefaultDirMode dirMode, const std::string &filename);
	virtual void setupSaveDir(HWND hComboBox, HWND hTextField,
		HWND hBrowseButton, LDPreferences::DefaultDirMode dirMode,
		const std::string &filename, CUCSTR nameKey);
	virtual void redrawColorBitmap(HWND hColorButton, HBITMAP hButtonBitmap,
		COLORREF color);
	virtual void getRGB(int color, int &r, int &g, int &b);
	virtual void chooseColor(HWND hColorButton, HBITMAP hColorBitmap,
		COLORREF &color);
	virtual void chooseBackgroundColor(void);
	virtual void browseForDir(const char *prompt, HWND hTextField,
		std::string &dir);
	virtual void chooseDefaultColor(void);
	virtual void doFSRefresh(void);
	virtual void doStipple(void);
	virtual void doSort(void);
	virtual void doFlatShading(void);
	virtual void doSmoothCurves(void);
	virtual void doHighlights(void);
	virtual void doConditionals(void);
	virtual void doWireframe(void);
	virtual void doBfc(void);
	virtual void doLighting(void);
	virtual void doDrawLightDats(void);
	virtual void doStereo(void);
	virtual void doCutaway(void);
	virtual void doDeletePrefSet(void);
	virtual void doNewPrefSet(void);
	virtual void doPrefSetHotKey(void);
	virtual void doSeams(void);
	virtual void doPrimitives(void);
	virtual void doTextureStuds(void);
	virtual void doTexmaps(void);
	virtual void doCheckPartTracker(void);
	virtual bool doApply(void);
	virtual DWORD doClick(HWND hPage, int controlId, HWND controlHWnd);
	virtual DWORD doComboSelChange(HWND hPage, int controlId, HWND controlHWnd);
	virtual BOOL doPrefSetSelected(bool force = false);
	virtual void doGeneralClick(int controlId, HWND controlHWnd);
	virtual void doGeometryClick(int controlId, HWND controlHWnd);
	virtual void doEffectsClick(int controlId, HWND controlHWnd);
	virtual void doPrimitivesClick(int controlId, HWND controlHWnd);
	virtual void doUpdatesClick(int controlId, HWND controlHWnd);
	virtual void doPrefSetsClick(int controlId, HWND controlHWnd);
	virtual void doOtherClick(HWND hDlg, int controlId, HWND controlHWnd);
	virtual BOOL doDialogInit(HWND hDlg, HWND hFocusWindow, LPARAM lParam);
	virtual BOOL doNewPrefSetInit(HWND hDlg, HWND hNewPrefSetField);
	virtual BOOL doHotKeyInit(HWND hDlg, HWND hHotKeyCombo);
	virtual BOOL doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification);
	virtual BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
		HWND controlHWnd);
	virtual BOOL doDialogHelp(HWND hDlg, LPHELPINFO helpInfo);
	virtual BOOL doDrawItem(HWND hDlg, int itemId,
		LPDRAWITEMSTRUCT drawItemStruct);
	virtual DWORD getPageDialogID(HWND hDlg);
	virtual void doReset(void);
	virtual void setupPage(int pageNumber);
	virtual void setupGeneralPage(void);
	virtual void setupGeometryPage(void);
	virtual void setupEffectsPage(void);
	virtual void setupPrimitivesPage(void);
	virtual void setupUpdatesPage(void);
	virtual void setupPrefSetsPage(void);
	virtual void setupAntialiasing(void);
	virtual void setupWireframe(void);
	virtual void setupBfc(void);
	virtual void setupSubstitution(void);
	virtual void setupTextures(void);
	virtual void setupProxy(void);
	virtual void setupPrefSetsList(void);
	virtual void enableCheckPartTracker(void);
	virtual void disableCheckPartTracker(void);
	virtual void enableProxyServer(void);
	virtual void disableProxyServer(void);
	virtual void enableWireframe(BOOL enable = TRUE);
	virtual void disableWireframe(void);
	virtual void enableBfc(BOOL enable = TRUE);
	virtual void disableBfc(void);
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
	virtual void applyUpdatesChanges(void);
	virtual void clear(void);
	virtual void setupStereo(void);
	virtual void enableStereo(void);
	virtual void disableStereo(void);
	virtual void setupCutaway(void);
	virtual void enableCutaway(void);
	virtual void disableCutaway(void);
	virtual void setupLighting(void);
	//virtual void setupLightAngleToolbar(void);
	virtual void setupLightAngleButtons(void);
	virtual void uncheckLightDirections(void);
	virtual void enableLighting(void);
	virtual void disableLighting(void);
	virtual void enableSeams(void);
	virtual void disableSeams(void);
	virtual void enablePrimitives(void);
	virtual void disablePrimitives(void);
	virtual void updateTextureFilteringEnabled(void);
	virtual void updateTexmapsEnabled(void);
	virtual void enableTextureFiltering(void);
	virtual void disableTextureFiltering(void);
	virtual void enableTexmaps(void);
	virtual void disableTexmaps(void);
	virtual void setupOpacitySlider(void);
	virtual char *getPrefSet(int index);
	virtual char *getSelectedPrefSet(void);
	virtual void selectPrefSet(const char *prefSet = NULL, bool force = false);
	virtual INT_PTR runPrefSetApplyDialog(void);
	virtual bool shouldSetActive(int index);
	virtual void abandonChanges(void);
	virtual char *getHotKey(int index);
	virtual int getHotKey(const char *currentPrefSetName);
	virtual int getCurrentHotKey(void);
	virtual void saveCurrentHotKey(void);
	virtual INT_PTR dialogProc(HWND hDlg, UINT message, WPARAM wParam,
		LPARAM lParam);
	virtual TCFloat getMinFov(void);
	virtual TCFloat getMaxFov(void);
	virtual void initThemesButton(HWND hButton);
	//virtual void initThemesTab(HWND hWnd);
	virtual void setupGroupCheckButton(HWND hPage, int buttonId, bool state);
	virtual bool getCachedCheck(HWND hPage, int buttonId, bool action = false);
	virtual bool getCheck(HWND hPage, int buttonId);
	virtual void setCheck(HWND hPage, int buttonId, bool value);
	virtual void lightVectorChangedCallback(TCAlert *alert);
	virtual void userDefaultChangedAlertCallback(TCAlert *alert);
	virtual void setAniso(int value);
	//virtual void setToolbarCheck(HWND hToolbar, int id, bool value);
	//virtual bool getToolbarCheck(HWND hToolbar, int id);

	virtual LDPreferences::LightDirection getSelectedLightDirection(void);


	BOOL doDrawColorButton(HWND hDlg, HWND hWnd, HTHEME hTheme,
		LPDRAWITEMSTRUCT drawItemStruct);
	BOOL doDrawGroupCheckBox(HWND hWnd, HTHEME hTheme,
		LPDRAWITEMSTRUCT drawItemStruct);
	BOOL doDrawIconPushButton(HWND hWnd, HTHEME hTheme,
		LPDRAWITEMSTRUCT drawItemStruct);

	LRESULT iconButtonProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	LRESULT colorButtonProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	LRESULT groupCheckButtonProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);

	static LRESULT CALLBACK staticIconButtonProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK staticColorButtonProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK staticGroupCheckButtonProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);
	static void setColor(const char *key, COLORREF color);
	static int sliderValueFromTextureOffset(double value);
	static TCFloat32 textureOffsetFromSliderValue(int value);
	static int sliderValueFromAniso(double value);
	static TCFloat32 anisoFromSliderValue(int value);

	LDrawModelViewer* modelViewer;
	LDPreferences* ldPrefs;

	int generalPageNumber;
	int geometryPageNumber;
	int effectsPageNumber;
	int primitivesPageNumber;
	int updatesPageNumber;
	int prefSetsPageNumber;

	char *newPrefSetName;
	int hotKeyIndex;

	HWND hGeneralPage;
	HWND hFullScreenRateField;
	HBITMAP hBackgroundColorBitmap;
	HWND hBackgroundColorButton;
	HBITMAP hDefaultColorBitmap;
	HWND hDefaultColorButton;
	HWND hSnapshotDirCombo;
	HWND hSnapshotDirField;
	HWND hSnapshotBrowseButton;
	LDPreferences::DefaultDirMode snapshotDirMode;
	std::string snapshotDir;
	HWND hPartsListDirCombo;
	HWND hPartsListDirField;
	HWND hPartsListBrowseButton;
	LDPreferences::DefaultDirMode partsListDirMode;
	std::string partsListDir;
	HWND hExportDirCombo;
	HWND hExportDirField;
	HWND hExportBrowseButton;
	LDPreferences::DefaultDirMode exportDirMode;
	std::string exportDir;
	HDC hButtonColorDC;
	HWND hMouseOverButton;
	LONG_PTR origButtonWindowProc;

	HWND hGeometryPage;
	HWND hSeamSpin;
	HWND hWireframeFogButton;
	HWND hRemoveHiddenLinesButton;
	HWND hWireframeThicknessLabel;
	HWND hWireframeThicknessSlider;
	HWND hRedBackFacesButton;
	HWND hGreenFrontFacesButton;
	HWND hBlueNeutralFacesButton;
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
	HWND hLightDrawLightDatsButton;
	HWND hLightOptionalStandardButton;
	//HWND hLightDirectionToolbar;
	HWND hHardwareStereoButton;
	//HWND hLightDirStatic;
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
	HWND hTexmapsAfterTransparentButton;
	HWND hTextureOffsetLabel;
	HWND hTextureOffsetSlider;
	HWND hTextureStudsButton;
	HWND hTextureNearestButton;
	HWND hTextureBilinearButton;
	HWND hTextureTrilinearButton;
	HWND hTextureAnisoButton;
	HWND hAnisoLevelSlider;
	HWND hAnisoLevelLabel;
	HWND hCurveQualityLabel;
	HWND hCurveQualitySlider;

	HWND hUpdatesPage;
	HWND hProxyServerLabel;
	HWND hProxyServer;
	HWND hProxyPortLabel;
	HWND hProxyPort;
	HWND hCheckPartTracker;
	HWND hMissingPartsLabel;
	HWND hMissingParts;
	HWND hUpdatedPartsLabel;
	HWND hUpdatedParts;

	HWND hPrefSetsPage;
	HWND hPrefSetsList;
	HWND hDeletePrefSetButton;
	HWND hNewPrefSetButton;
	HWND hPrefSetHotKeyButton;

	bool setActiveWarned;
	bool checkAbandon;

	HTHEME hButtonTheme;
	//HTHEME hTabTheme;

	static char ldviewPath[MAX_PATH];
	HwndBoolMap checkStates;
	IntIntMap lightDirIndexToId;
	IntIntMap lightDirIdToIndex;
	HwndIntMap buttonTypes;
	HwndVector lightAngleButtons;
	//static LRESULT CALLBACK staticLightDirWindowProc(HWND hWnd,
	//	UINT message, WPARAM wParam, LPARAM lParam);
};

#endif // __LDVPREFERENCES_H__
