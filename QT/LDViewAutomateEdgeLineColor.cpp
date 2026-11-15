#include "LDViewAutomateEdgeLineColor.h"
#include <QColorDialog>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include "Preferences.h"

AutomateEdgeLineColor::AutomateEdgeLineColor(QWidget *parent)
	:QDialog(parent),AutomateEdgeLineColorPanel()
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
	connect( saturationSlider ,SIGNAL( valueChanged(int) ), this, SLOT( doSaturationSlider(int) ) );
	connect( contrastSlider, SIGNAL( valueChanged(int) ), this, SLOT( doContrastSlider(int) ) );
	connect( contrastResetButton, SIGNAL( clicked() ) , this, SLOT ( resetContrast() ) );
	connect( saturationResetButton, SIGNAL( clicked() ) , this, SLOT ( resetSaturation() ) );
	reflectSettings();
}

void AutomateEdgeLineColor::reflectSettings()
{

	float f;
	f=TCUserDefaults::floatForKey(PART_EDGE_CONTRAST_KEY);
	contrastValueLabel->setText(QString::number(f));
	contrastSlider->setValue(f*10.0);
	f=TCUserDefaults::floatForKey(PART_EDGE_SATURATION_KEY);
	saturationValueLabel->setText(QString::number(f));
	saturationSlider->setValue(f*10.0);
}

void AutomateEdgeLineColor::doOk()
{
	TCUserDefaults::setFloatForKey(contrastValueLabel->text().toFloat(),PART_EDGE_CONTRAST_KEY, false);
	TCUserDefaults::setFloatForKey(saturationValueLabel->text().toFloat(),PART_EDGE_SATURATION_KEY, false);
	QDialog::close();
}

void AutomateEdgeLineColor::doCancel()
{
	QDialog::close();
}

void AutomateEdgeLineColor::doContrastSlider(int i)
{
	contrastValueLabel->setText(QString::number(i/10.0));
}

void AutomateEdgeLineColor::doSaturationSlider(int i)
{
	saturationValueLabel->setText(QString::number(i/10.0));
}

void AutomateEdgeLineColor::resetContrast()
{
	float f;
	f=TCUserDefaults::floatForKey(PART_EDGE_CONTRAST_KEY);
	contrastSlider->setValue(f*10.0);
}

void AutomateEdgeLineColor::resetSaturation()
{
	float f;
	f=TCUserDefaults::floatForKey(PART_EDGE_SATURATION_KEY);
	saturationSlider->setValue(f*10.0);
}

AutomateEdgeLineColor::~AutomateEdgeLineColor()
{
}

