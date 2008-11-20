#ifndef __OptionsCanvas_H__
#define __OptionsCanvas_H__

#include <CUI/CUIDialog.h>
#include <LDExporter/LDExporter.h>

class OptionUI;
class GroupOptionUI;
class LDExporter;
class OptionsScroller;

typedef std::list<OptionUI *> OptionUIList;

/*
OptionsCanvas

Window that is designed to be like a form for options.  The window is sized to
be tall enough to hold all options, so it needs to be inside another window that
scrolls.  Note that this uses a dialog box in order to get the standard dialog
box backround painting and keyboard navigation.  The dialog resource is
completely empty.
*/
class OptionsCanvas: public CUIDialog
{
public:
	OptionsCanvas(HINSTANCE hInstance);
	void create(OptionsScroller *parent);
	int calcHeight(int newWidth, int &optimalWidth, bool update = false);
	void addGroup(LDExporterSetting &setting);
	void addBoolSetting(LDExporterSetting &setting);
	void addFloatSetting(LDExporterSetting &setting);
	void addLongSetting(LDExporterSetting &setting);
	void addStringSetting(LDExporterSetting &setting);
	void addEnumSetting(LDExporterSetting &setting);
	bool commitSettings(void);
	void updateEnabled(void);
	void resetSettings(void);
protected:
	virtual ~OptionsCanvas(void);
	virtual void dealloc(void);
	void calcOptionHeight(HDC hdc, OptionUIList::iterator &it, int &y,
		int width, int leftMargin, int rightMargin, int numberWidth,
		int &optimalWidth, bool update, bool enabled);


	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);

	OptionUIList m_optionUIs;
	OptionsScroller *m_parent;
	int m_spacing;
	int m_margin;
};

#endif // __OptionsCanvas_H__
