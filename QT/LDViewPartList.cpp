#include "qt4wrapper.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "ModelViewerWidget.h"
#include "LDViewPartList.h"
#include <TCFoundation/mystring.h>

PartList::PartList(QWidget *parent, ModelViewerWidget *modelWidget, LDHtmlInventory *htmlInventory)
        :QDialog(parent),PartListPanel(),
		modelWidget(modelWidget),
		m_htmlInventory(htmlInventory)
{
	setupUi(this);
    connect( upButton, SIGNAL( clicked() ), this, SLOT( doUp() ) );
    connect( downButton, SIGNAL( clicked() ), this, SLOT( doDown() ) );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
    connect( showModelButton, SIGNAL( clicked() ), this, SLOT( doShowModel() ) );
    connect( fieldOrderView, SIGNAL( currentChanged(Q3ListViewItem *) ), this, SLOT( doHighlighted() ) );

    modelViewer = modelWidget->getModelViewer();
	fieldOrderView->header()->hide();
	fieldOrderView->setSorting(-1);
}

PartList::~PartList(void)
{
}

void PartList::populateColumnList(void)
{
	const LDPartListColumnVector &columnOrder =
        m_htmlInventory->getColumnOrder();
	int count = 0, i;
	fieldOrderView->clear();
    for (i = LDPLCLast; i >= LDPLCFirst; i--)
    {
        LDPartListColumn column = (LDPartListColumn)i;
        if (!m_htmlInventory->isColumnEnabled(column))
        {
            const char *name = LDHtmlInventory::getColumnName(column);
            QCheckListItem *item = new QCheckListItem(fieldOrderView,
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
		QCheckListItem *item = new QCheckListItem(fieldOrderView,
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
	bool showmodel;
	LDPartListColumnVector columnOrder;
	m_htmlInventory->setExternalCssFlag(generateExternalSSButton->isChecked());
	m_htmlInventory->setPartImagesFlag(showPartImageButton->isChecked());
	m_htmlInventory->setShowModelFlag(showmodel = 
					  showModelButton->isChecked());
	if (showmodel)
	{
		 m_htmlInventory->setOverwriteSnapshotFlag(
					overwriteExistingButton->isChecked());
	}
	for (item = fieldOrderView->firstChild() ; item ;
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
	accept();
}

void PartList::doCancel()
{
	reject();
}

void PartList::doMoveColumn(int distance)
{
	QListViewItem *item = fieldOrderView->currentItem();
	QListViewItem *newitem = ( distance == 1 ? item->itemBelow() : item->itemAbove());
	if (!newitem) return;
	QString ttt=newitem->text(0);
	bool s = ((QCheckListItem*)newitem)->isOn();
	newitem->setText(0,item->text(0));
	((QCheckListItem*)newitem)->setOn(((QCheckListItem*)item)->isOn());
	item->setText(0,ttt);
	((QCheckListItem*)item)->setOn(s);
	fieldOrderView->setCurrentItem(newitem);
	doHighlighted();
}

void PartList::doHighlighted()
{
	QListViewItem *item = fieldOrderView->currentItem();
	if (item)
	{
		upButton->setEnabled(item->itemAbove() ? true : false);
		downButton->setEnabled(item->itemBelow() ? true : false);
	}
}

int PartList::exec()
{
	bool showmodel;
	populateColumnList();
  	generateExternalSSButton->setChecked(
		m_htmlInventory->getExternalCssFlag());
   	showPartImageButton->setChecked(
		m_htmlInventory->getPartImagesFlag());
   	showModelButton->setChecked(showmodel =
		m_htmlInventory->getShowModelFlag());
	overwriteExistingButton->setChecked(showmodel ?
		m_htmlInventory->getOverwriteSnapshotFlag() : false);
	doShowModel();
	return QDialog::exec();
}

void PartList::doShowModel()
{
	overwriteExistingButton->setEnabled(showModelButton->isChecked());
}

