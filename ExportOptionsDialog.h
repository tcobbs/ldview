#ifndef __ExportOptionsDialog_H__
#define __ExportOptionsDialog_H__

#include <CUI/CUIDialog.h>
#include <LDExporter/LDExporter.h>
#include <CommCtrl.h>

class CUIWindowResizer;
class OptionsCanvas;
class OptionsScroller;

class ExportOptionsDialog: public CUIDialog
{
public:
	ExportOptionsDialog(HINSTANCE hInstance, LDExporter *exporter);
	INT_PTR doModal(HWND hWndParent);
protected:
	virtual ~ExportOptionsDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual LRESULT doMouseWheel(short keyFlags, short zDelta, int xPos,
		int yPos);
	//virtual LRESULT doNotify(int controlId, LPNMHDR notification);

	virtual void doOK(void);
	virtual LRESULT doBeginLabelEdit(NMLVDISPINFO *notification);
	virtual LRESULT doEndLabelEdit(NMLVDISPINFO *notification);

	void initCanvas(void);
	void initCanvasOptions(void);
	void canvasSizeChanged(void);
	//void addRow(LDExporterSetting &setting, LVITEM &item);
	//void initOptionsList(void);
	//void addBoolSetting(LDExporterSetting &setting);
	//void addFloatSetting(LDExporterSetting &setting);
	//void addLongSetting(LDExporterSetting &setting);

	LDExporter *m_exporter;
	CUIWindowResizer *m_resizer;
	//HWND hOptionsList;
	OptionsScroller *m_scroller;
	OptionsCanvas *m_canvas;
};

#endif // __ExportOptionsDialog_H__
