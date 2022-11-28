#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>

BoundingBox::BoundingBox(QWidget *parent,ModelViewerWidget *modelWidget)
	:QDialog(parent),BoundingBoxPanel(),
	m_modelWindow(modelWidget),
	m_model(NULL)
{
	setupUi(this);
}


BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::show()
{
	showBoundingBox(true);
	QDialog::show();
}

void BoundingBox::setModel(LDLMainModel *model)
{
    if (model != m_model)
    {
		m_model = model;
    }
}


void BoundingBox::modelAlertCallback(TCAlert *alert)
{
    if (alert->getSender() == (TCAlertSender*)m_modelWindow->getModelViewer())
    {
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0)
		{
			setModel(getModelViewer()->getMainModel());
			updateData();
		}
		else if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
		{
			setModel(NULL);
			updateData();
		}
    }
}

void BoundingBox::setModelWindow(ModelViewerWidget *modelWindow)
{
    if (modelWindow != m_modelWindow)
    {
		m_modelWindow = modelWindow;
    }
    setModel(getModelViewer()->getMainModel());
}

void BoundingBox::showBoundingBox(bool value)
{
    LDrawModelViewer *modelViewer = getModelViewer();

    if (modelViewer)
    {
		modelViewer->setShowBoundingBox(value);
    }
}

LDrawModelViewer *BoundingBox::getModelViewer(void)
{
    if (m_modelWindow)
    {
		return m_modelWindow->getModelViewer();
    }
    else
    {
		return NULL;
    }
}

void BoundingBox::updateData(void)
{
    LDrawModelViewer *modelViewer = getModelViewer();
    if (m_model != NULL && modelViewer != NULL)
    {
		char buf1[1024];
		char buf2[1026];

		modelViewer->getBoundingMin().print(buf1, sizeof(buf1));
		snprintf(buf2, sizeof(buf2), "<%s>", buf1);
		minimumPointLine->setText(buf2);
		modelViewer->getBoundingMax().print(buf1, sizeof(buf1));
		snprintf(buf2, sizeof(buf2), "<%s>", buf1);
		maximumPointLine->setText(buf2);
    }
    else
    {
		minimumPointLine->setText("");
		maximumPointLine->setText("");
    }
}

void BoundingBox::hide()
{
	showBoundingBox(false);
	QDialog::hide();
}
