#include "qt4wrapper.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "ModelViewerWidget.h"
#include "PartList.h"
#include "LDViewPartList.h"
#include <TCFoundation/mystring.h>

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
	int count = 0, i;
	panel->fieldOrderView->clear();
	for (i = 0; i < (int)columnOrder.size(); i++)
	{
		LDPartListColumn column = columnOrder[i];
		const char *name = LDHtmlInventory::getColumnName(column);
		QCheckListItem *item = new QCheckListItem(panel->fieldOrderView,
								name, QCheckListItem::CheckBoxController );
		item->setState(m_htmlInventory->isColumnEnabled(column) ? 
						QCheckListItem::On : QCheckListItem::Off);
		count++;
	}
}

void PartList::doUp()
{
	doMoveColumn(-1);
}

void PartList::doDown()
{
	doMoveColumn(1);
}

void PartList::doOk()
{
	int i;
	QListViewItem *item;
	QCheckListItem *item2;
	LDPartListColumnVector columnOrder;
	m_htmlInventory->setExternalCssFlag(panel->generateExternalSSButton->isChecked());
	m_htmlInventory->setPartImagesFlag(panel->showPartImageButton->isChecked());
	m_htmlInventory->setShowModelFlag(panel->showModelButton->isChecked());

	for (item = panel->fieldOrderView->firstChild() ; item ;
		 item = item->itemBelow())
	{
		const char * itemname = item->text(0).ascii();
		
		for (i = LDPLCFirst; i <= LDPLCLast; i++)
		{
			LDPartListColumn column = (LDPartListColumn)i;
			char *name = copyString(LDHtmlInventory::getColumnName(column));

			const char * itemname = item->text(0).ascii();
			if (strcmp(name,itemname)==0)
			{
				columnOrder.push_back(column);
			}
		}
	}
	m_htmlInventory->setColumnOrder(columnOrder);
}

void PartList::doCancel()
{
}

void PartList::doMoveColumn(int distance)
{
	QListViewItem *item = panel->fieldOrderView->currentItem();
	QListViewItem *newitem = ( distance == 1 ? item->itemBelow() : item->itemAbove());
	if (!newitem) return;
	QString ttt=newitem->text(0);
	QCheckListItem::ToggleState s = ((QCheckListItem*)newitem)->state();
	newitem->setText(0,item->text(0));
	((QCheckListItem*)newitem)->setState(((QCheckListItem*)item)->state());
	item->setText(0,ttt);
	((QCheckListItem*)item)->setState(s);
	panel->fieldOrderView->setCurrentItem(newitem);
	controlDirectionButtons();
}

void PartList::controlDirectionButtons()
{
	QListViewItem *item = panel->fieldOrderView->currentItem();
	if (item)
	{
		panel->upButton->setEnabled(item->itemAbove() ? true : false);
		panel->downButton->setEnabled(item->itemBelow() ? true : false);
	}
}

void PartList::show()
{
	if (panel)
	{
		populateColumnList();
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
