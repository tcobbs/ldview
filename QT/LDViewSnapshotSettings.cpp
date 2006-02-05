#include "qt4wrapper.h"

#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <UserDefaultsKeys.h>
#include <TCFoundation/mystring.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "ModelViewerWidget.h"
#include "SnapshotSettingsPanel.h"
#include "LDViewSnapshotSettings.h"

SnapshotSettings::SnapshotSettings(ModelViewerWidget *modelWidget)
	:modelWidget(modelWidget),
	panel(new SnapshotSettingsPanel)
{
    modelViewer = modelWidget->getModelViewer();
    panel->setSnapshotSettings(this);
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
    panel->digitBox->setValue(TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false));
    panel->widthBox->setValue(TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false));
    panel->heightBox->setValue(TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false));
    setButtonState(panel->zoomtofitEnabledButton,
		TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1, false));
	setButtonState(panel->seriesEnabledButton,
		TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0);
	setButtonState(panel->sizeEnabledButton,
		TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false));
	setButtonState(panel->pbufferEnabledButton,
		TCUserDefaults::longForKey(IGNORE_PBUFFER_KEY, 1, false));
	panel->doEnabledSize();
	panel->doEnabledSeries();
}

SnapshotSettings::~SnapshotSettings(void)
{
}

void SnapshotSettings::show(void)
{
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}

void SnapshotSettings::doOk()
{
	TCUserDefaults::setLongForKey(panel->digitBox->value(),
		SAVE_DIGITS_KEY, false);
	TCUserDefaults::setLongForKey(panel->widthBox->value(), 
		SAVE_WIDTH_KEY, false);
	TCUserDefaults::setLongForKey(panel->heightBox->value(),
		SAVE_HEIGHT_KEY, false);
	TCUserDefaults::setLongForKey(panel->zoomtofitEnabledButton->isChecked(),
		SAVE_ZOOM_TO_FIT_KEY, false);
	TCUserDefaults::setLongForKey(panel->seriesEnabledButton->isChecked(),
		SAVE_SERIES_KEY, false);
	TCUserDefaults::setLongForKey(panel->sizeEnabledButton->isChecked(),
		SAVE_ACTUAL_SIZE_KEY, false);
	TCUserDefaults::setLongForKey(panel->pbufferEnabledButton->isChecked(),
		IGNORE_PBUFFER_KEY, false);
}

void SnapshotSettings::doCancel()
{
	reflectSettings();
}
