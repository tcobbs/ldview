#ifndef __TOOLBARSTRIP_H__
#define __TOOLBARSTRIP_H__

#include <CUI/CUIDialog.h>
#include "TbButtonInfo.h"

class LDViewWindow;
class LDViewPreferences;
class TCAlert;

typedef std::vector<HWND> HwndVector;

class ToolbarStrip: public CUIDialog
{
public:
	ToolbarStrip(HINSTANCE hInstance);
	void create(LDViewWindow *parent);
	void show(void);
	void hide(void);
	void autoSize(void);
	HWND getHToolbar(void) { return m_hToolbar; }
	void updateNumSteps(void);
	void updateStep(void);
protected:
	virtual ~ToolbarStrip(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual LRESULT doNotify(int controlId, LPNMHDR notification);

	void addTbButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, int stdBmpId, int tbBmpId, BYTE style = TBSTYLE_BUTTON,
		BYTE state = TBSTATE_ENABLED);
	void addTbCheckButtonInfo(TbButtonInfoVector &infos, CUCSTR tooltipText,
		int commandId, int stdBmpId, int tbBmpId, bool checked,
		BYTE style = TBSTYLE_CHECK, BYTE state = TBSTATE_ENABLED);
	void addTbSeparatorInfo(TbButtonInfoVector &infos);
	void populateMainTbButtonInfos(void);
	void populateStepTbButtonInfos(void);
	void initMainToolbar(void);
	void initStepToolbar(void);
	void initLayout(void);
	void initToolbar(HWND hToolbar, TbButtonInfoVector &infos, UINT bitmapId);
	void modelAlertCallback(TCAlert *alert);

	static HBITMAP createMask(HBITMAP hBitmap, COLORREF maskColor);

	LDViewWindow *m_ldviewWindow;
	LDViewPreferences *m_prefs;

	HWND m_hToolbar;
	HWND m_hStepLabel;
	HWND m_hStepField;
	HWND m_hNumStepsLabel;
	HWND m_hStepToolbar;

	int m_numSteps;
	int m_step;
	ucstring m_numStepsFormat;
	TbButtonInfoVector m_mainButtonInfos;
	TbButtonInfoVector m_stepButtonInfos;
	HwndVector m_controls;
	int m_stripHeight;

	int m_stdBitmapStartId;
	int m_tbBitmapStartId;
	bool m_drawWireframe;
	bool m_seams;
	bool m_edges;
	bool m_primitiveSubstitution;
	bool m_lighting;
	bool m_bfc;
};

#endif // __TOOLBARSTRIP_H__
