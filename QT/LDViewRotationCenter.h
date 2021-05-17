#ifndef __LDVIEWRotationCenter_H__
#define __LDVIEWRotationCenter_H__

#include "ui_RotationCenterPanel.h"
#include <QDialog>
#include <QValidator>
#include <LDLib/LDrawModelViewer.h>

class LDrawModelViewer;
class ModelViewerWidget;

class RotationCenter : public QDialog , Ui::RotationCenterPanel
{
	Q_OBJECT
public:
	RotationCenter(QWidget *parent, ModelViewerWidget *modelWidget);
	~RotationCenter(void);
	void setX(float value) { x = value; }
	void setY(float value) { y = value; }
	void setZ(float value) { z = value; }

public slots:
	void doOk();
	void doCancel();
	void doReset();
	void show(void);
protected:
	LDrawModelViewer *m_modelViewer;
	ModelViewerWidget *m_modelWidget;
	float x,y,z;
	QDoubleValidator *v;
};

#endif

