#include "qt4wrapper.h"

#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/mystring.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include "ModelViewerWidget.h"
#include "LDViewSnapshotSettings.h"

SnapshotSettings::SnapshotSettings(ModelViewerWidget *modelWidget)
	:SnapshotSettingsPanel(),
	modelWidget(modelWidget)
{
    modelViewer = modelWidget->getModelViewer();
	reflectSettings();
}

void SnapshotSettings::setButtonState(QCheckBox *button, bool state)
{
    bool buttonState = button->isChecked();

    if (state != buttonState)
    {
        button->toggle();
    }
}

void SnapshotSettings::reflectSettings(void)
{
    digitBox->setValue(TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false));
    widthBox->setValue(TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false));
    heightBox->setValue(TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false));
    setButtonState(zoomtofitEnabledButton,
		TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1, false));
	setButtonState(seriesEnabledButton,
		TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0);
	setButtonState(sizeEnabledButton,
		TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false));
	setButtonState(pbufferEnabledButton,
		TCUserDefaults::longForKey(IGNORE_PBUFFER_KEY, 1, false));
	doEnabledSize();
	doEnabledSeries();
}

SnapshotSettings::~SnapshotSettings(void)
{
}
/*
void SnapshotSettings::show(void)
{
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}
*/
void SnapshotSettings::doOk()
{
	TCUserDefaults::setLongForKey(digitBox->value(),
		SAVE_DIGITS_KEY, false);
	TCUserDefaults::setLongForKey(widthBox->value(), 
		SAVE_WIDTH_KEY, false);
	TCUserDefaults::setLongForKey(heightBox->value(),
		SAVE_HEIGHT_KEY, false);
	TCUserDefaults::setLongForKey(zoomtofitEnabledButton->isChecked(),
		SAVE_ZOOM_TO_FIT_KEY, false);
	TCUserDefaults::setLongForKey(seriesEnabledButton->isChecked(),
		SAVE_SERIES_KEY, false);
	TCUserDefaults::setLongForKey(sizeEnabledButton->isChecked(),
		SAVE_ACTUAL_SIZE_KEY, false);
	TCUserDefaults::setLongForKey(pbufferEnabledButton->isChecked(),
		IGNORE_PBUFFER_KEY, false);
	close();
}

void SnapshotSettings::doCancel()
{
	reflectSettings();
	close();
}

void SnapshotSettings::doEnabledSeries()
{
    digitBox->setEnabled(seriesEnabledButton->isChecked());
    digitLabel->setEnabled(seriesEnabledButton->isChecked());
}

void SnapshotSettings::doEnabledSize()
{
    widthBox->setEnabled(sizeEnabledButton->isChecked());
    heightBox->setEnabled(sizeEnabledButton->isChecked());
    widthLabel->setEnabled(sizeEnabledButton->isChecked());
    heightLabel->setEnabled(sizeEnabledButton->isChecked());
}

