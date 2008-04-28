#include "qt4wrapper.h"

#include <qstring.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include "LDViewLatitudeLongitude.h"
#include <LDLib/LDUserDefaultsKeys.h>

LatitudeLongitude::LatitudeLongitude(ModelViewerWidget *modelWidget)
	:LatitudeLongitudePanel(),
	modelWidget(modelWidget),
	v1(new QIntValidator(-90,90,this)),
	v2(new QIntValidator(-180,180,this))
{
	latitudeLine->setValidator(v1);
	longitudeLine->setValidator(v2);
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
	LatitudeLongitudePanel::show();
}

void LatitudeLongitude::doOk()
{
	int lat,lon;
	if (sscanf(latitudeLine->text().ascii(),"%i",&lat) ==1)
		TCUserDefaults::setFloatForKey(lat, LAST_LAT_KEY, false);
	if (sscanf(longitudeLine->text().ascii(),"%i",&lon) ==1)
    	TCUserDefaults::setFloatForKey(lon, LAST_LON_KEY, false);
	modelWidget->getModelViewer()->setLatLon(lat,lon);
	LatitudeLongitudePanel::close();
}

void LatitudeLongitude::doCancel()
{
	LatitudeLongitudePanel::close();
}

