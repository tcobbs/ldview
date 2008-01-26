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
	:modelWidget(modelWidget),
	panel(new JpegOptionsPanel),
	options(new TCJpegOptions)
{
    modelViewer = modelWidget->getModelViewer();
    panel->setJpegOptions(this);
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
	panel->colorSubsamplingBox->setCurrentItem(index);
	quality = options->getQuality();
	panel->qualitySlider->setValue(quality);
	sprintf(number,"%d",quality);
	panel->qualityValueLabel->setText(number);
	panel->progressiveCheckBox->setChecked(options->getProgressive());
}

JpegOptions::~JpegOptions(void)
{
	TCObject::release(options);
}

void JpegOptions::show(void)
{
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}

void JpegOptions::doOk()
{
	TCJpegOptions::SubSampling subSampling = TCJpegOptions::SS444;
	switch (panel->colorSubsamplingBox->currentItem())
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
    options->setQuality(panel->qualitySlider->value());
    options->setSubSampling(subSampling);
    options->setProgressive(panel->progressiveCheckBox->isChecked());
    options->save();

}

void JpegOptions::doCancel()
{
	reflectSettings();
}
void JpegOptions::doSliderMoved(int i)
{
    char number[15];
    sprintf(number,"%d",i);
    panel->qualityValueLabel->setText(number);
}
