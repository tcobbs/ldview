#ifndef __TOOLBARSTRIP_H__
#define __TOOLBARSTRIP_H__

#include <CUI/CUIDialog.h>
#include "TbButtonInfo.h"

class LDViewWindow;
class LDViewPreferences;
class TCAlert;
class TCImage;

typedef std::vector<HWND> HwndVector;
typedef std::vector<long> LongVector;
typedef std::map<long, size_t> LongSizeTMap;
typedef std::list<HIMAGELIST> HImageListList;

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

	LRESULT doToolbarGetInfotip(TbButtonInfoVector &infos,
		LPNMTBGETINFOTIPUC dispInfo);
	//LRESULT doMainToolbarGetInfotip(LPNMTBGETINFOTIPUC dispInfo);
	LRESULT doMainToolbarNotify(int controlId, LPNMHDR notification);
	LRESULT doStepToolbarNotify(int controlId, LPNMHDR notification);
	LRESULT doMainTbGetButtonInfo(NMTOOLBARUC *notification);
	LRESULT doMainToolbarChange(void);
	LRESULT doMainToolbarReset(void);
	void addTbButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, int bmpIndex, BYTE style = TBSTYLE_BUTTON,
		BYTE state = TBSTATE_ENABLED);
	void addTbCheckButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, int bmpIndex, bool checked,
		BYTE style = TBSTYLE_CHECK, BYTE state = TBSTATE_ENABLED);
	void addTbSeparatorInfo(TbButtonInfoVector &infos);
	void populateMainTbButtonInfos(void);
	void populateStepTbButtonInfos(void);
	void loadMainToolbarMenus(void);
	void initMainToolbar(void);
	void fillMainToolbar(void);
	void initStepToolbar(void);
	void initLayout(void);
	//HIMAGELIST initImageList(HWND hToolbar, UINT bitmapId);
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
	void updateMenu(HMENU hMenu, int command, int index, HIMAGELIST hImageList);
	void updateMenus(const TbButtonInfoVector &infos);
	void sizeToolbar(HWND hToolbar, int lastCommandID);

	void forceRedraw(void);

	void doWireframe(void);
	void doSeams(void);
	void doEdges(void);
	void doPrimitiveSubstitution(void);
	void doLighting(void);
	void doBfc(void);
	void doShowAxes(void);
	void doRandomColors(void);
	void doFog(void);
	void doRemoveHiddenLines(void);
	void doShowEdgesOnly(void);
	void doConditionalLines(void);
	void doHighQualityEdges(void);
	void doAlwaysBlack(void);
	void doTextureStuds(void);
	void doQualityLighting(void);
	void doSubduedLighting(void);
	void doSpecularHighlight(void);
	void doAlternateLighting(void);
	void doDrawLightDats(void);
	void doOptionalStandardLight(void);
	void doRedBackFaces(void);
	void doGreenFrontFaces(void);
	void doCustomizeMainToolbar(void);
	void doMainToolbar(void);
	void doStepsToolbar(void);

	void checkReflect(bool &value, bool prefsValue, LPARAM commandID);
	void doDropDown(LPNMTOOLBAR toolbarNot);

	void fillTbButton(TBBUTTON &button, const TbButtonInfo &buttonInfo);

	static HBITMAP createMask(HBITMAP hBitmap, COLORREF maskColor);
	static HBITMAP createMask(TCImage *image);

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

	int m_numSteps;
	int m_step;
	ucstring m_numStepsFormat;
	TbButtonInfoVector m_mainButtonInfos;
	TbButtonInfoVector m_stepButtonInfos;
	HwndVector m_controls;
	int m_stripHeight;
	HImageListList m_imageLists;

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

	bool m_showMain;
	bool m_showSteps;
};

#endif // __TOOLBARSTRIP_H__
