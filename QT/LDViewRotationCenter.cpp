#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>

RotationCenter::RotationCenter(QWidget *parent,ModelViewerWidget *modelWidget)
	:QDialog(parent),RotationCenterPanel(),
	m_modelWidget(modelWidget),
	v(new QDoubleValidator())
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
	connect( resetButton, SIGNAL( clicked() ), this, SLOT( doReset() ) );
	xLine->setValidator(v);
	yLine->setValidator(v);
	zLine->setValidator(v);
}


RotationCenter::~RotationCenter(void)
{
}

void RotationCenter::doOk()
{
	m_modelViewer->setRotationCenter(TCVector(xLine->text().toFloat(), yLine->text().toFloat(), zLine->text().toFloat()));
	accept();
}

void RotationCenter::doCancel()
{
	reject();
}

void RotationCenter::doReset()
{
	m_modelViewer->resetRotationCenter();
	accept();
}

void RotationCenter::show(void)
{
	m_modelViewer = m_modelWidget->getModelViewer();
	TCVector cameraLocation = m_modelViewer->getRotationCenter();
	setX(cameraLocation[0]);
	setY(cameraLocation[1]);
	setZ(cameraLocation[2]);
	xLine->setText(QString::number(x));
	yLine->setText(QString::number(y));
	zLine->setText(QString::number(z));
	QDialog::show();
}

