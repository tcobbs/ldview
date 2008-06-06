#ifndef __OptionsCanvas_H__
#define __OptionsCanvas_H__

#include <CUI/CUIDialog.h>
#include <LDExporter/LDExporter.h>

class OptionUI;
class LDExporter;

typedef std::list<OptionUI *> OptionUIList;

class OptionsCanvas: public CUIDialog
{
public:
	OptionsCanvas(HINSTANCE hInstance);
	void create(CUIWindow *parent);
	int calcHeight(int newWidth, bool update = false);
	void addBoolSetting(LDExporterSetting &setting);
	void addFloatSetting(LDExporterSetting &setting);
	void addLongSetting(LDExporterSetting &setting);
	bool commitSettings(void);
protected:
	virtual ~OptionsCanvas(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);

	OptionUIList m_optionUIs;
};

#endif // __OptionsCanvas_H__
