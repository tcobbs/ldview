#include "qt4wrapper.h"

#include <qstring.h>
#include <qlabel.h>
#include <qlineedit.h>
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>

BoundingBox::BoundingBox(ModelViewerWidget *modelWidget)
	:BoundingBoxPanel(),
	m_modelWindow(modelWidget),
	m_model(NULL)
{
}


BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::show()
{
	showBoundingBox(true);
	BoundingBoxPanel::show();
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
        char buf2[1024];

        modelViewer->getBoundingMin().print(buf1);
        sprintf(buf2, "<%s>", buf1);
        minimumPointLine->setText(buf2);
        modelViewer->getBoundingMax().print(buf1);
        sprintf(buf2, "<%s>", buf1);
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
	BoundingBoxPanel::hide();
}
