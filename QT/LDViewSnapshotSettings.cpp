#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/mystring.h>
#include "ModelViewerWidget.h"
#include "LDViewSnapshotSettings.h"
#include <TCFoundation/TCLocalStrings.h>

SnapshotSettings::SnapshotSettings(QWidget *parent, ModelViewerWidget *modelWidget)
	:QWidget(parent),SnapshotSettingsPanel(),
	modelWidget(modelWidget)
{
	setupUi(this);
	connect( sizeEnabledButton, SIGNAL( toggled(bool) ), this, SLOT( doEnabledSize() ) );
	connect( seriesEnabledButton, SIGNAL( toggled(bool) ), this, SLOT( doEnabledSeries() ) );
	connect( zoomtofitEnabledButton, SIGNAL( toggled(bool) ), this, SLOT( zoomToggled(bool) ) );
	connect( allStepsBox, SIGNAL( toggled(bool) ), this, SLOT( zoomToggled(bool) ) );

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
	seriesEnabledButton->setChecked(
		TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0);
	sizeEnabledButton->setChecked(
		TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false));
	autoCropButton->setChecked(
		TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false));
	transparentBackgroundButton->setChecked(
		TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0);
	allStepsBox->setChecked(
		TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false, false));
	suffixEdit->setText(
		TCUserDefaults::stringForKey(SAVE_STEPS_SUFFIX_KEY,
		TCLocalStrings::get("DefaultStepSuffix"), false));
	sameScaleCheck->setChecked(
		TCUserDefaults::boolForKey(SAVE_STEPS_SAME_SCALE_KEY,
		true, false));
	doEnabledSize();
	doEnabledSeries();
	zoomToggled(true);
}

SnapshotSettings::~SnapshotSettings(void)
{
}

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
	bool saveAllSteps = allStepsBox->isChecked();
	TCUserDefaults::setBoolForKey(saveAllSteps, SAVE_STEPS_KEY, false);
	TCUserDefaults::setBoolForKey(autoCropButton->isChecked(),
								  AUTO_CROP_KEY, false);
	TCUserDefaults::setBoolForKey(transparentBackgroundButton->isChecked(),
				SAVE_ALPHA_KEY, false);
	if (saveAllSteps)
	{
		TCUserDefaults::setStringForKey(suffixEdit->text().toUtf8().constData(),
			SAVE_STEPS_SUFFIX_KEY, false);
		TCUserDefaults::setBoolForKey(sameScaleCheck->isChecked(),
			SAVE_STEPS_SAME_SCALE_KEY, false);
	}
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
	zoomToggled(true);
}

void SnapshotSettings::zoomToggled(bool)
{
	sameScaleCheck->setEnabled(sizeEnabledButton->isChecked() &
		allStepsBox->isChecked() &
		zoomtofitEnabledButton->isChecked());
}

