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

void SnapshotSettings::reflectSettings(void)
{
    panel->digitBox->setValue(TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false));
    panel->widthBox->setValue(TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false));
    panel->heightBox->setValue(TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false));
    panel->zoomtofitEnabledButton->setDown(TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1, false));
	panel->seriesEnabledButton->setDown(TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0);
	panel->sizeEnabledButton->setDown(TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false));
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
}

void SnapshotSettings::doCancel()
{
	reflectSettings();
}
