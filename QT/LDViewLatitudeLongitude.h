#ifndef __LDVIEWLATITUDELONGITUDE_H__
#define __LDVIEWLATITUDELONGITUDE_H__

#include "ui_LatitudeLongitude.h"
#include <QValidator>

class ModelViewerWidget;

class LatitudeLongitude : public QDialog , Ui::LatitudeLongitudePanel
{
	Q_OBJECT
public:
	LatitudeLongitude(QWidget *parent, ModelViewerWidget *modelWidget);
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
	QDoubleValidator *v3;
	float distance;
	void showEvent(QShowEvent * /* event */) {show();}
};

#endif

