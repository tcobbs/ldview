#ifndef __TOOLBARSTRIP_H__
#define __TOOLBARSTRIP_H__

#include <CUI/CUIDialog.h>
#include <LDLib/LDInputHandler.h>
#include "TbButtonInfo.h"

class LDViewWindow;
class LDViewPreferences;
class TCAlert;
class TCImage;

typedef std::vector<HWND> HwndVector;
typedef std::vector<long> LongVector;
typedef std::map<long, size_t> LongSizeTMap;
typedef std::vector<HIMAGELIST> HImageListVector;
typedef std::vector<int> IntVector;
typedef std::map<int, int> IntIntMap;
typedef std::list<HMENU> HMenuList;
typedef std::pair<size_t, int> ImageIndexPair;
typedef std::map<UINT, ImageIndexPair> ImageIndexMap;

class ToolbarStrip: public CUIDialog
{
public:
	ToolbarStrip(HINSTANCE hInstance);
	void create(LDViewWindow *parent);
	void show(void);
	void hide(void);
	void autoSize(void);
	HWND getMainToolbar(void) { return m_hToolbar; }
	void updateNumSteps(void);
	void updateStep(void);
	void enableMainToolbarButton(UINT buttonId, bool enable);
	void checksReflect(void);
	void viewModeReflect(void);
	void activateDeactivatedTooltip(void);
protected:
	virtual ~ToolbarStrip(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual LRESULT doNotify(int controlId, LPNMHDR notification);
	virtual LRESULT doInitMenuPopup(HMENU hPopupMenu, UINT uPos,
		BOOL fSystemMenu);
	virtual LRESULT doEnterMenuLoop(bool isTrackPopupMenu);
	virtual LRESULT doExitMenuLoop(bool isTrackPopupMenu);
	virtual LRESULT doContextMenu(HWND hWnd, int xPos, int yPos);
	virtual LRESULT doTimer(UINT_PTR timerID);

	LRESULT doToolbarGetInfotip(TbButtonInfoVector &infos,
		LPNMTBGETINFOTIPUC dispInfo);
	LRESULT doMainToolbarNotify(int controlId, LPNMHDR notification);
	LRESULT doStepToolbarNotify(int controlId, LPNMHDR notification);
	LRESULT doMainTbGetButtonInfo(NMTOOLBARUC *notification);
	LRESULT doMainToolbarChange(void);
	LRESULT doMainToolbarReset(void);
	void addTbButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, BYTE style = TBSTYLE_BUTTON,
		BYTE state = TBSTATE_ENABLED);
	void addTbCheckButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, bool checked, BYTE style = TBSTYLE_CHECK,
		BYTE state = TBSTATE_ENABLED);
	void addTbStateButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		IntVector commandIds, int selection, BYTE style = TBSTYLE_BUTTON,
		BYTE state = TBSTATE_ENABLED);
	void addTbSeparatorInfo(TbButtonInfoVector &infos);
	int addToImageList(int commandId);
	void populateMainTbButtonInfos(void);
	void populateStepTbButtonInfos(void);
	void loadMainToolbarMenus(void);
	void initMainToolbar(void);
	void fillMainToolbar(void);
	void initStepToolbar(void);
	void initLayout(void);
	void initToolbar(HWND hToolbar, TbButtonInfoVector &infos,
		HIMAGELIST hImageList);
	void modelAlertCallback(TCAlert *alert);
	void stepChanged(void);
	void enableToolbarButton(HWND hToolbar, UINT buttonId, bool enable);
	bool doCheck(bool &value, LPARAM commandId);
	void updateContextMenu(void);
	void updateWireframeMenu(void);
	void updateEdgesMenu(void);
	void updatePrimitivesMenu(void);
	void updateLightingMenu(void);
	void updateBFCMenu(void);
	void updateMenuImages(HMENU hMenu, bool topMenu = false);
	void updateMenus(void);
	void sizeToolbar(HWND hToolbar, int count);

	void forceRedraw(void);

	void doWireframe(void);
	void doSeams(void);
	void doEdges(void);
	void doPrimitiveSubstitution(void);
	void doLighting(void);
	void doBfc(void);
	void doShowAxes(void);
	void doRandomColors(void);
	void doAllConditionals(void);
	void doConditionalControls(void);
	void doCutaway(void);
	void doFlatShading(void);
	void doStudQuality(void);
	void doPartBoundingBoxes(void);
	void doSmoothCurves(void);
	void doTransDefaultColor(void);
	void doFog(void);
	void doRemoveHiddenLines(void);
	void doShowEdgesOnly(void);
	void doConditionalLines(void);
	void doHighQualityEdges(void);
	void doAlwaysBlack(void);
	void doTextureStuds(void);
	void doTexmaps(void);
	void doQualityLighting(void);
	void doSubduedLighting(void);
	void doSpecularHighlight(void);
	void doAlternateLighting(void);
	void doDrawLightDats(void);
	void doOptionalStandardLight(void);
	void doRedBackFaces(void);
	void doGreenFrontFaces(void);
	void doBlueNeutralFaces(void);
	void doCustomizeMainToolbar(void);
	void doMainToolbar(void);
	void doStepsToolbar(void);
	void doViewMode(void);

	void syncViewMode(void);
	void checkReflect(bool &value, bool prefsValue, LPARAM commandID);
	void doDropDown(LPNMTOOLBAR toolbarNot);

	void fillTbButton(TBBUTTON &button, const TbButtonInfo &buttonInfo);
	void stepCountChanged(void);

	int translateCommandId(int commandId);

	LDViewWindow *m_ldviewWindow;
	LDViewPreferences *m_prefs;

	HWND m_hToolbar;
	HWND m_hStepLabel;
	HWND m_hStepField;
	HWND m_hNumStepsLabel;
	HWND m_hStepToolbar;
	HWND m_hDeactivatedTooltip;;

	HMENU m_hMainToolbarMenu;
	HMENU m_hContextMenu;
	HMENU m_hWireframeMenu;
	HMENU m_hEdgesMenu;
	HMENU m_hPrimitivesMenu;
	HMENU m_hLightingMenu;
	HMENU m_hBFCMenu;

	HMODULE m_hGdiPlus;

	double m_scaleFactor;
	int m_imageSize;
	int m_buttonWidth;
	int m_buttonHeight;

	int m_numSteps;
	int m_step;
	ucstring m_numStepsFormat;
	TbButtonInfoVector m_mainButtonInfos;
	TbButtonInfoVector m_stepButtonInfos;
	HwndVector m_controls;
	int m_stripHeight;
	HImageListVector m_imageLists;
	IntIntMap m_commandMap;
	ImageIndexMap m_imagesMap;
	bool m_have32BitBmps;

	int m_stdBitmapStartId;
	int m_tbBitmapStartId;
	bool m_drawWireframe;
	bool m_seams;
	bool m_edges;
	bool m_primitiveSubstitution;
	bool m_lighting;
	bool m_bfc;
	bool m_showAxes;
	bool m_randomColors;
	bool m_allConditionals;
	bool m_conditionalControls;
	bool m_flat;
	bool m_lowStuds;
	bool m_partBBoxes;
	bool m_smoothCurves;
	bool m_transDefaultColor;
	bool m_modelBoundingBox;
	bool m_topmost;
	bool m_wireframeCutaway;
	bool m_examineLatLong;
	bool m_texmaps;
	LDInputHandler::ViewMode m_viewMode;

	bool m_showMain;
	bool m_showSteps;
};

#endif // __TOOLBARSTRIP_H__
