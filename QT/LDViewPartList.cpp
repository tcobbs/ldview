#include "qt4wrapper.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "ModelViewerWidget.h"
#include "PartList.h"
#include "LDViewPartList.h"

PartList::PartList(ModelViewerWidget *modelWidget, LDHtmlInventory *htmlInventory)
        :modelWidget(modelWidget),
		panel(new PartListPanel),
		m_htmlInventory(htmlInventory)
{
    modelViewer = modelWidget->getModelViewer();
    panel->setPartList(this);
	panel->fieldOrderView->header()->hide();
}

PartList::~PartList(void)
{
}

void PartList::populateColumnList(void)
{
	const LDPartListColumnVector &columnOrder =
        m_htmlInventory->getColumnOrder();
	int count = 0;
}

void PartList::doUp()
{
}

void PartList::doDown()
{
}

void PartList::doOk()
{
	m_htmlInventory->setExternalCssFlag(panel->generateExternalSSButton->isChecked());
	m_htmlInventory->setPartImagesFlag(panel->showPartImageButton->isChecked());
	m_htmlInventory->setShowModelFlag(panel->showModelButton->isChecked());
}

void PartList::doCancel()
{
}

void PartList::show()
{
	if (panel)
	{
    	panel->generateExternalSSButton->setChecked(
			m_htmlInventory->getExternalCssFlag());
    	panel->showPartImageButton->setChecked(
			m_htmlInventory->getPartImagesFlag());
    	panel->showModelButton->setChecked(
			m_htmlInventory->getShowModelFlag());
		panel->show();
		panel->raise();
      	panel->setActiveWindow();
	}
}
