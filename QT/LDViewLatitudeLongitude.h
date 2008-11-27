#ifndef __LDVIEWLATITUDELONGITUDE_H__
#define __LDVIEWLATITUDELONGITUDE_H__

#include "LatitudeLongitude.h"

class QButton;
class ModelViewerWidget;
class QIntValidator;

class LatitudeLongitude : public LatitudeLongitudePanel
{
public:
	LatitudeLongitude(ModelViewerWidget *modelWidget);
	~LatitudeLongitude(void);

public slots:
	void show(void);
	void doOk();
	void doCancel();
	void doCurrent();
	void doDefault();
	void distanceChanged();
protected:
	ModelViewerWidget *modelWidget;
	QIntValidator *v1,*v2;
};

#endif

