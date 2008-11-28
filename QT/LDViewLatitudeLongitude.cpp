#include "qt4wrapper.h"

#include <qstring.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "LDViewLatitudeLongitude.h"
#include <LDLib/LDUserDefaultsKeys.h>

LatitudeLongitude::LatitudeLongitude(ModelViewerWidget *modelWidget)
	:LatitudeLongitudePanel(),
	modelWidget(modelWidget),
	v1(new QIntValidator(-90,90,this)),
	v2(new QIntValidator(-180,180,this)),
	v3(new QDoubleValidator(0.0,999999.9,4,this))
{
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
	LatitudeLongitudePanel::show();
}

void LatitudeLongitude::doOk()
{
	int lat,lon;
	bool checked;
	float f;
	if (sscanf(latitudeLine->text().ascii(),"%i",&lat) ==1)
		TCUserDefaults::setFloatForKey(lat, LAST_LAT_KEY, false);
	if (sscanf(longitudeLine->text().ascii(),"%i",&lon) ==1)
    	TCUserDefaults::setFloatForKey(lon, LAST_LON_KEY, false);
	TCUserDefaults::setBoolForKey(checked = distanceCheckBox->isChecked(), 
								  LAST_HAVE_DIST_KEY, false);
	if (checked && (sscanf(distanceLine->text().ascii(),"%f",&f) == 1))
	{
		TCUserDefaults::setFloatForKey(distance = f, LAST_DIST_KEY, false);
	}
	modelWidget->getModelViewer()->setLatLon(lat,lon, distance);
	LatitudeLongitudePanel::close();
}

void LatitudeLongitude::doCancel()
{
	LatitudeLongitudePanel::close();
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

