#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>

CameraLocation::CameraLocation(QWidget *parent,ModelViewerWidget *modelWidget)
	:QDialog(parent),CameraLocationPanel(),
	m_modelWidget(modelWidget),
	v(new QDoubleValidator())
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
	connect( lookAtModelCenterBox, SIGNAL( stateChanged(int) ), this, SLOT ( lookAtModelCenterBoxChanged(int) ) );
	connect( lookAtOriginBox, SIGNAL( stateChanged(int) ), this, SLOT ( lookAtOriginBoxChanged(int) ) );
	xLine->setValidator(v);
	yLine->setValidator(v);
	zLine->setValidator(v);
}


CameraLocation::~CameraLocation(void)
{
}

void CameraLocation::doOk()
{
	m_modelViewer->setCameraLocation(TCVector(xLine->text().toFloat(), yLine->text().toFloat(), zLine->text().toFloat()),m_lookAt);
	TCUserDefaults::setLongForKey(m_lookAt, CAMERA_LOCATION_LOOK_AT_KEY, false);
	accept();
}

void CameraLocation::doCancel()
{
	reject();
}

void CameraLocation::show(void)
{
    m_modelViewer = m_modelWidget->getModelViewer();
	TCVector cameraLocation = m_modelViewer->getCameraLocation();
	setX(cameraLocation[0]);
	setY(cameraLocation[1]);
	setZ(cameraLocation[2]);
	xLine->setText(QString::number(x));
	yLine->setText(QString::number(y));
	zLine->setText(QString::number(z));
	m_lookAt = (LDVLookAt)TCUserDefaults::longForKey(CAMERA_LOCATION_LOOK_AT_KEY,
		LDVLookAtModel, false);
	updateLookAtChecks();
	QDialog::show();
}

void CameraLocation::updateLookAtChecks()
{
	lookAtModelCenterBox->setCheckState(m_lookAt == LDVLookAtModel ? Qt::Checked : Qt::Unchecked);
	lookAtOriginBox->setCheckState(m_lookAt == LDVLookAtOrigin ? Qt::Checked : Qt::Unchecked);
}

void CameraLocation::lookAtModelCenterBoxChanged(int state)
{
	if (state == Qt::Checked) m_lookAt = LDVLookAtModel;
	updateLookAtChecks();
}

void CameraLocation::lookAtOriginBoxChanged(int state)
{
	if (state == Qt::Checked) m_lookAt = LDVLookAtOrigin;
	updateLookAtChecks();
}

