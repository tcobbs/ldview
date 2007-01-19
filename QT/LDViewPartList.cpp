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
	panel->fieldOrderView->setSorting(-1);
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
    for (i = LDPLCLast; i >= LDPLCFirst; i--)
    {
        LDPartListColumn column = (LDPartListColumn)i;
        if (!m_htmlInventory->isColumnEnabled(column))
        {
            const char *name = LDHtmlInventory::getColumnName(column);
            QCheckListItem *item = new QCheckListItem(panel->fieldOrderView,
                name, QCheckListItem::CheckBoxController );
            item->setOn(m_htmlInventory->isColumnEnabled(column));
			item->setTristate(false);
            count++;
        }
    }
	for (i = (int)columnOrder.size()-1; i >= 0 ; i--)
	{
		LDPartListColumn column = columnOrder[i];
		const char *name = LDHtmlInventory::getColumnName(column);
		QCheckListItem *item = new QCheckListItem(panel->fieldOrderView,
								name, QCheckListItem::CheckBoxController );
		item->setOn(m_htmlInventory->isColumnEnabled(column));
		item->setTristate(false);
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
	LDPartListColumnVector columnOrder;
	m_htmlInventory->setExternalCssFlag(panel->generateExternalSSButton->isChecked());
	m_htmlInventory->setPartImagesFlag(panel->showPartImageButton->isChecked());
	m_htmlInventory->setShowModelFlag(panel->showModelButton->isChecked());

	for (item = panel->fieldOrderView->firstChild() ; item ;
		 item = item->itemBelow())
	{
		const char * itemname = item->text(0).ascii();
		QCheckListItem *item2 = (QCheckListItem*) item;
		for (i = LDPLCFirst; i <= LDPLCLast; i++)
		{
			LDPartListColumn column = (LDPartListColumn)i;
			const char *name = LDHtmlInventory::getColumnName(column);
			if (strcmp(name,itemname)==0)
			{
				if (item2->isOn())
				{
					columnOrder.push_back(column);
				}
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
	bool s = ((QCheckListItem*)newitem)->isOn();
	newitem->setText(0,item->text(0));
	((QCheckListItem*)newitem)->setOn(((QCheckListItem*)item)->isOn());
	item->setText(0,ttt);
	((QCheckListItem*)item)->setOn(s);
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

int PartList::exec()
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
		return panel->exec();
	}
	return 0;
}

int PartList::result()
{
	return panel->result();
}
