#include "JpegOptionsDialog.h"
#include "Resource.h"
#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

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
	quality = value;
	trackBarSetPos(IDC_JPEG_QUAL_SLIDER, quality);
	upDownSetPos(IDC_JPEG_QUAL_SPIN, quality);
	windowSetValue(IDC_JPEG_QUAL_FIELD, (long)quality);
}

BOOL JpegOptionsDialog::doInitDialog(HWND /*hKbControl*/)
{
	int index;
	int i;

	quality = options->getQuality();
	hQualityField = GetDlgItem(hWindow, IDC_JPEG_QUAL_FIELD);
	hQualitySpin = GetDlgItem(hWindow, IDC_JPEG_QUAL_SPIN);
	hQualitySlider = GetDlgItem(hWindow, IDC_JPEG_QUAL_SLIDER);
	trackBarSetup(IDC_JPEG_QUAL_SLIDER, 1, 100, 10, quality);
	for (i = 10; i < 100; i += 10)
	{
		trackBarSetTic(IDC_JPEG_QUAL_SLIDER, i);
	}
	textFieldSetLimitText(IDC_JPEG_QUAL_FIELD, 3);
	upDownSetup(IDC_JPEG_QUAL_SPIN, 1, 100, quality);
	setQuality(quality);
	comboAddString(IDC_JPEG_SUBSAMPLING_COMBO,
		ls(_UC("Jpeg444ss")));
	comboAddString(IDC_JPEG_SUBSAMPLING_COMBO,
		ls(_UC("Jpeg422ss")));
	comboAddString(IDC_JPEG_SUBSAMPLING_COMBO,
		ls(_UC("Jpeg420ss")));
	switch (options->getSubSampling())
	{
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
	comboSetCurSel(IDC_JPEG_SUBSAMPLING_COMBO, index);
	checkSet(IDC_JPEG_PROGRESSIVE_CHECK, options->getProgressive());
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
		{
			switch (scrollCode)
			{
			case SB_PAGEUP:
				position = quality - 10;
				break;
			case SB_PAGEDOWN:
				position = quality + 10;
				break;
			}
			if (position < 1)
			{
				position = 1;
			}
			if (position > 100)
			{
				position = 100;
			}
			setQuality(position);
			return 0;
		}
	}
	return CUIDialog::doHScroll(scrollCode, position, hScrollBar);
}

LRESULT JpegOptionsDialog::doTextFieldChange(int controlId, HWND control)
{
	if (controlId == IDC_JPEG_QUAL_FIELD)
	{
		long value;

		if (windowGetValue(controlId, value))
		{
			int start, end;

			textFieldGetSelection(controlId, start, end);
			if (value > 100)
			{
				while (value > 100)
				{
					value /= 10;
				}
			}
			setQuality(value);
			if (start > 2)
			{
				if (value == 100)
				{
					start = 3;
				}
				else
				{
					start = 2;
				}
			}
			textFieldSetSelection(controlId, start, start);
		}
		return 0;
	}
	return CUIDialog::doTextFieldChange(controlId, control);
}

void JpegOptionsDialog::doOK(void)
{
	TCJpegOptions::SubSampling subSampling = TCJpegOptions::SS444;

	switch (comboGetCurSel(IDC_JPEG_SUBSAMPLING_COMBO))
	{
	case 0:
		subSampling = TCJpegOptions::SS444;
		break;
	case 1:
		subSampling = TCJpegOptions::SS422;
		break;
	case 2:
		subSampling = TCJpegOptions::SS420;
		break;
	}
	options->setQuality(trackBarGetPos(IDC_JPEG_QUAL_SLIDER));
	options->setSubSampling(subSampling);
	options->setProgressive(checkGet(IDC_JPEG_PROGRESSIVE_CHECK));
	options->save();
	CUIDialog::doOK();
}
