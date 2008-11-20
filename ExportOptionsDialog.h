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
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual LRESULT doReset(void);

	virtual void doOK(void);

	void initCanvas(void);
	void initCanvasOptions(void);
	void canvasSizeChanged(void);

	LDExporter *m_exporter;
	CUIWindowResizer *m_resizer;
	OptionsScroller *m_scroller;
	OptionsCanvas *m_canvas;
};

#endif // __ExportOptionsDialog_H__
