#ifndef __JPEGOPTIONSDIALOG_H__
#define __JPEGOPTIONSDIALOG_H__


#include <CUI/CUIDialog.h>

class TCJpegOptions;

class JpegOptionsDialog: public CUIDialog
{
public:
	JpegOptionsDialog(HINSTANCE hInstance, HWND hParentWindow);
	INT_PTR doModal(HWND hWndParent);
protected:
	virtual ~JpegOptionsDialog(void);
	virtual void dealloc(void);
	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doVScroll(int scrollCode, int position, HWND hScrollBar);
	virtual LRESULT doHScroll(int scrollCode, int position, HWND hScrollBar);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual void doOK(void);
	void setQuality(int value);

	TCJpegOptions *options;
	HWND hQualityField;
	HWND hQualitySpin;
	HWND hQualitySlider;
	int quality;
};

#endif // __JPEGOPTIONSDIALOG_H__
