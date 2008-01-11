#include "JpegOptionsDialog.h"
#include "Resource.h"
#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/mystring.h>

JpegOptionsDialog::JpegOptionsDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIDialog(hInstance, hParentWindow),
options(new TCJpegOptions)
{
}

JpegOptionsDialog::~JpegOptionsDialog(void)
{
}

void JpegOptionsDialog::dealloc(void)
{
	TCObject::release(options);
	CUIDialog::dealloc();
}

INT_PTR JpegOptionsDialog::doModal(HWND hWndParent)
{
	return CUIDialog::doModal(IDD_JPEG_OPTIONS, hWndParent);
}

void JpegOptionsDialog::setQuality(int value)
{
	char buf[128];

	quality = value;
	setSliderValue(IDC_JPEG_QUAL_SLIDER, quality);
	setSpinValue(IDC_JPEG_QUAL_SPIN, quality);
	sprintf(buf, "%d", quality);
	SetWindowTextA(hQualityField, buf);

}

BOOL JpegOptionsDialog::doInitDialog(HWND /*hKbControl*/)
{
	int index;
	int i;

	quality = options->getQuality();
	hQualityField = GetDlgItem(hWindow, IDC_JPEG_QUAL_FIELD);
	hQualitySpin = GetDlgItem(hWindow, IDC_JPEG_QUAL_SPIN);
	hQualitySlider = GetDlgItem(hWindow, IDC_JPEG_QUAL_SLIDER);
	setupSlider(IDC_JPEG_QUAL_SLIDER, 1, 100, 10, quality);
	for (i = 10; i < 100; i += 10)
	{
		setSliderTic(IDC_JPEG_QUAL_SLIDER, i);
	}
	setupSpin(IDC_JPEG_QUAL_SPIN, 1, 100, quality);
	setQuality(quality);
	switch (options->getSubSampling())
	{
	case TCJpegOptions::SS444:
	case TCJpegOptions::SS422:
		index = 1;
		break;
	case TCJpegOptions::SS420:
		index = 2;
		break;
	default:	// TCJpegOptions::SS444
		index = 0;
		break;
	}
	setCheck(IDC_JPEG_PROGRESSIVE_CHECK, options->getProgressive());
	return TRUE;
}

LRESULT JpegOptionsDialog::doVScroll(
	int scrollCode,
	int position,
	HWND hScrollBar)
{
	if (scrollCode == SB_THUMBPOSITION && hScrollBar == hQualitySpin)
	{
		setQuality(position);
		return 0;
	}
	return CUIDialog::doVScroll(scrollCode, position, hScrollBar);
}

LRESULT JpegOptionsDialog::doHScroll(
	int scrollCode,
	int position,
	HWND hScrollBar)
{
	if (hScrollBar == hQualitySlider)
	{
		if (scrollCode != SB_ENDSCROLL)
		//if (scrollCode == SB_THUMBTRACK || scrollCode == SB_THUMBPOSITION)
		{
			setQuality(position);
			return 0;
		}
	}
	return CUIDialog::doHScroll(scrollCode, position, hScrollBar);
}

/*
LRESULT JpegOptionsDialog::doNotify(int controlId, LPNMHDR notification)
{
	if (controlId == IDC_JPEG_QUAL_SLIDER)
	{
		if (notification->code == NM_RELEASEDCAPTURE)
		{
			setQuality(getSliderValue(IDC_JPEG_QUAL_SLIDER));
		}
	}
	return CUIDialog::doNotify(controlId, notification);
}
*/