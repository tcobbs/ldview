#include "Preferences.h"

#include "LDViewMpdModel.h"

#include <TCFoundation/TCStringArray.h>

#include "misc.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>

MpdModel::MpdModel(QWidget *parent,ModelViewerWidget *modelViewer)
	:QDialog(parent),MpdModelSelectionPanel(),
	m_modelWindow(modelViewer),
    mainmodel(NULL),
	m_okPressed(false)
{
	setupUi(this);
    connect( modelList, SIGNAL( currentItemChanged(QListWidgetItem *,QListWidgetItem *) ), this, SLOT( doMpdSelected(QListWidgetItem *,QListWidgetItem *) ) );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( ok() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancel() ) );
}

MpdModel::~MpdModel() { }

void MpdModel::show(void)
{
	raise();
//	setActiveWindow();
	updateData();
	QDialog::show();
}

void MpdModel::hide()
{
	if (!m_okPressed)
		showMpdModel(0);
	QDialog::hide();
}

void MpdModel::setModel(LDLMainModel *model)
{
    if (mainmodel != model)
    {
        modeltree = NULL;
        mainmodel = model;
    }
}

void MpdModel::modelAlertCallback(TCAlert *alert)
{
    if (alert->getSender() == (TCAlertSender*)m_modelWindow->getModelViewer())
    {
        if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0)
        {
            setModel(m_modelWindow->getModelViewer()->getMainModel());
            updateData();
        }
        else if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
        {
            setModel(NULL);
            updateData();
        }
    }
}

void MpdModel::setModelWindow(ModelViewerWidget *modelWindow)
{
    if (modelWindow != m_modelWindow)
    {
		m_modelWindow = modelWindow;
    }
    setModel(m_modelWindow->getModelViewer()->getMainModel());
}

void MpdModel::updateData()
{
	LDrawModelViewer *modelViewer = getModelViewer();
	QListWidgetItem *current = NULL, *item;
	modelList->clear();
	if (mainmodel != NULL && m_modelWindow != NULL)
	{
		LDLModelVector &mpdModels = mainmodel->getMpdModels();
		if (mpdModels.size() > 0)
		{
			for (size_t i = 0; i < mpdModels.size(); i++)
			{
				LDLModel *mpdModel = mpdModels[i];
				item = new QListWidgetItem(mpdModel->getName(),modelList);
				if ( i == (unsigned int) modelViewer->getMpdChildIndex())
					current = item;
			}
			modelList->setCurrentItem(current);
		}
	}
}

void MpdModel::showMpdModel(int index)
{
	LDrawModelViewer *modelViewer = getModelViewer();

	if (modelViewer)
	{
		modelViewer->setMpdChildIndex(index);
	}
}

LDrawModelViewer * MpdModel::getModelViewer(void)
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

void MpdModel::ok()
{
	m_okPressed = true;
	hide();
}

void MpdModel::cancel()
{
	m_okPressed = false;
	hide();
}

