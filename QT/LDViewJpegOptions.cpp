#include "ModelViewerWidget.h"
#include "LDViewJpegOptions.h"

JpegOptions::JpegOptions(QWidget *parent,ModelViewerWidget *modelWidget)
	:QDialog(parent),JpegOptionsPanel(),
	modelWidget(modelWidget),
	options(new TCJpegOptions)
{
	setupUi(this);
	connect( qualitySlider, SIGNAL( sliderMoved(int) ), this, SLOT( doSliderMoved(int) ) );
	connect( qualitySlider, SIGNAL( valueChanged(int) ), this, SLOT( doSliderMoved(int) ) );
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );

	modelViewer = modelWidget->getModelViewer();
	reflectSettings();
}

void JpegOptions::setButtonState(QCheckBox *button, bool state)
{
	bool buttonState = button->isChecked();

	if (state != buttonState)
	{
		button->toggle();
	}
}

void JpegOptions::reflectSettings(void)
{
	int index;
	switch (options->getSubSampling())
	{
	case TCJpegOptions::SS422:
		index = 1;
		break;
	case TCJpegOptions::SS420:
		index = 2;
		break;
	default:    // TCJpegOptions::SS444
		index = 0;
		break;
	}
	colorSubsamplingBox->setCurrentIndex(index);
	quality = options->getQuality();
	qualitySlider->setValue(quality);
	qualityValueLabel->setText(QString::number(quality));
	progressiveCheckBox->setChecked(options->getProgressive());
}

JpegOptions::~JpegOptions(void)
{
	TCObject::release(options);
}

void JpegOptions::doOk()
{
	TCJpegOptions::SubSampling subSampling = TCJpegOptions::SS444;
	switch (colorSubsamplingBox->currentIndex())
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
	options->setQuality(qualitySlider->value());
	options->setSubSampling(subSampling);
	options->setProgressive(progressiveCheckBox->isChecked());
	options->save();
	close();
}

void JpegOptions::doCancel()
{
	reflectSettings();
	close();
}
void JpegOptions::doSliderMoved(int i)
{
	qualityValueLabel->setText(QString::number(i));
}
