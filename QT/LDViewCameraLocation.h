#ifndef __LDVIEWCameraLocation_H__
#define __LDVIEWCameraLocation_H__

#include "ui_CameraLocationPanel.h"
#include <QDialog>
#include <QValidator>
#include <LDLib/LDrawModelViewer.h>

class LDrawModelViewer;
class ModelViewerWidget;

class CameraLocation : public QDialog , Ui::CameraLocationPanel
{
	Q_OBJECT
public:
	CameraLocation(QWidget *parent, ModelViewerWidget *modelWidget);
	~CameraLocation(void);
	void setX(float value) { x = value; }
	void setY(float value) { y = value; }
	void setZ(float value) { z = value; }

public slots:
	void doOk();
	void doCancel();
	void show(void);
	void lookAtModelCenterBoxChanged(int);
	void lookAtOriginBoxChanged(int);
protected:
	void updateLookAtChecks();
	LDrawModelViewer *m_modelViewer;
	ModelViewerWidget *m_modelWidget;
	float x,y,z;
	QDoubleValidator *v;
	LDVLookAt m_lookAt;
};

#endif

