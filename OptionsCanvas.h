#ifndef __OptionsCanvas_H__
#define __OptionsCanvas_H__

#include <CUI/CUIDialog.h>
#include <LDExporter/LDExporter.h>

class OptionUI;
class GroupOptionUI;
class LDExporter;
class OptionsScroller;

typedef std::list<OptionUI *> OptionUIList;

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
	bool commitSettings(void);
	void updateEnabled(void);
protected:
	virtual ~OptionsCanvas(void);
	virtual void dealloc(void);
	virtual void closeGroup(GroupOptionUI *&currentGroup, int &y,
		int &optimalWidth, int &leftMargin, int &rightMargin, int &numberWidth,
		int spacing, bool &enabled, bool update);

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
