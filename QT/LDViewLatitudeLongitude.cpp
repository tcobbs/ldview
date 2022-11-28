#include <QString>
#include <QValidator>
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "LDViewLatitudeLongitude.h"
#include <LDLib/LDUserDefaultsKeys.h>

LatitudeLongitude::LatitudeLongitude(QWidget *parent , ModelViewerWidget *modelWidget)
	:QDialog(parent),LatitudeLongitudePanel(),
	modelWidget(modelWidget),
	v1(new QIntValidator(-90,90,this)),
	v2(new QIntValidator(-180,180,this)),
	v3(new QDoubleValidator(0.0,999999.9,4,this))
{
	setupUi(this);
    connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
    connect( defaultButton, SIGNAL( clicked() ), this, SLOT( doDefault() ) );
    connect( currentButton, SIGNAL( clicked() ), this, SLOT( doCurrent() ) );
    connect( distanceCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( distanceChanged() ) );
	latitudeLine->setValidator(v1);
	longitudeLine->setValidator(v2);
	distanceLine->setValidator(v3);
}


LatitudeLongitude::~LatitudeLongitude(void)
{
	latitudeLine->setValidator(NULL);
	longitudeLine->setValidator(NULL);
}

void LatitudeLongitude::show()
{
	QString qs;
	qs.setNum(TCUserDefaults::floatForKey(LAST_LAT_KEY, 30.0f, false));
	latitudeLine->setText(qs);
	qs.setNum(TCUserDefaults::floatForKey(LAST_LON_KEY, 45.0f, false));
	longitudeLine->setText(qs);
	distanceCheckBox->setChecked(
				TCUserDefaults::boolForKey(LAST_HAVE_DIST_KEY, false, false));
	qs.setNum(distance = TCUserDefaults::floatForKey(LAST_DIST_KEY,
				distance = modelWidget->getModelViewer()->getDefaultDistance(),
										  false));
	distanceLine->setText(qs);
	distanceChanged();
	QDialog::show();
}

void LatitudeLongitude::doOk()
{
	int lat,lon;
	bool checked,ok;
	float f;
	lat = latitudeLine->text().toInt(&ok);
	if (ok)
		TCUserDefaults::setFloatForKey(lat, LAST_LAT_KEY, false);
	lon = longitudeLine->text().toInt(&ok);
	if (ok)
    	TCUserDefaults::setFloatForKey(lon, LAST_LON_KEY, false);
	TCUserDefaults::setBoolForKey(checked = distanceCheckBox->isChecked(),
								  LAST_HAVE_DIST_KEY, false);
	f = distanceLine->text().toFloat(&ok);
	if (checked && ok)
	{
		TCUserDefaults::setFloatForKey(distance = f, LAST_DIST_KEY, false);
	}
	else
	{
		distance = -1.0f;
	}
	modelWidget->getModelViewer()->setLatLon(lat,lon, distance);
	QDialog::close();
}

void LatitudeLongitude::doCancel()
{
	QDialog::close();
}

void LatitudeLongitude::doCurrent()
{
	distance = modelWidget->getModelViewer()->getDistance();
	distanceChanged();
}

void LatitudeLongitude::doDefault()
{
	distance = modelWidget->getModelViewer()->getDefaultDistance();
	distanceChanged();
}

void LatitudeLongitude::distanceChanged()
{
	QString qs;
	bool checked = distanceCheckBox->isChecked();
	currentButton->setEnabled(checked);
	defaultButton->setEnabled(checked);
	distanceLine->setEnabled(checked);
	qs.setNum(distance);
	distanceLine->setText(checked ? qs : "");
}

