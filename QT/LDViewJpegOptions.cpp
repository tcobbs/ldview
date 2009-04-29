#include "qt4wrapper.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcombobox.h>
#include "ModelViewerWidget.h"
#include "JpegOptionsPanel.h"
#include "LDViewJpegOptions.h"

JpegOptions::JpegOptions(ModelViewerWidget *modelWidget)
	:JpegOptionsPanel(),
	modelWidget(modelWidget),
	options(new TCJpegOptions)
{
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
	char number[15];
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
	colorSubsamplingBox->setCurrentItem(index);
	quality = options->getQuality();
	qualitySlider->setValue(quality);
	sprintf(number,"%d",quality);
	qualityValueLabel->setText(number);
	progressiveCheckBox->setChecked(options->getProgressive());
}

JpegOptions::~JpegOptions(void)
{
	TCObject::release(options);
}

void JpegOptions::doOk()
{
	TCJpegOptions::SubSampling subSampling = TCJpegOptions::SS444;
	switch (colorSubsamplingBox->currentItem())
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
    char number[15];
    sprintf(number,"%d",i);
    qualityValueLabel->setText(number);
}
