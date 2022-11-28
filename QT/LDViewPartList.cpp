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
	connect( fieldOrderView, SIGNAL( currentItemChanged(QListWidgetItem *, QListWidgetItem *) ), this, SLOT( doHighlighted(QListWidgetItem *, QListWidgetItem *) ) );

	modelViewer = modelWidget->getModelViewer();
//	fieldOrderView->header()->hide();
//	fieldOrderView->setSorting(-1);
}

PartList::~PartList(void)
{
}

void PartList::populateColumnList(void)
{
	const LDPartListColumnVector &columnOrder =
		m_htmlInventory->getColumnOrder();
	int i;
	fieldOrderView->clear();
	for (i = 0; i < (int)columnOrder.size(); i++)
	{
		LDPartListColumn column = columnOrder[i];
		const char *name = LDHtmlInventory::getColumnName(column);
		QListWidgetItem *item = new QListWidgetItem(name, fieldOrderView);
		item->setCheckState(m_htmlInventory->isColumnEnabled(column) ? Qt::Checked : Qt::Unchecked);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	}
	for (i = LDPLCFirst; i <= LDPLCLast; i++)
	{
		LDPartListColumn column = (LDPartListColumn)i;
		if (!m_htmlInventory->isColumnEnabled(column))
		{
			const char *name = LDHtmlInventory::getColumnName(column);
			QListWidgetItem *item = new QListWidgetItem(name, fieldOrderView);
		item->setCheckState(m_htmlInventory->isColumnEnabled(column) ? Qt::Checked : Qt::Unchecked);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		}
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
	int i,j;
	QListWidgetItem *item;
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
	for (item = fieldOrderView->item(j = 0) ; item && (j < (fieldOrderView->count())) ;
		 item = fieldOrderView->item(++j))
	{
		const char * itemname = item->text().toLatin1().constData();
		QListWidgetItem *item2 = (QListWidgetItem*) item;
		for (i = LDPLCFirst; i <= LDPLCLast; i++)
		{
			LDPartListColumn column = (LDPartListColumn)i;
			const char *name = LDHtmlInventory::getColumnName(column);
			if (strcmp(name,itemname)==0)
			{
				if (item2->checkState() == Qt::Checked)
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
	QListWidgetItem *item = fieldOrderView->currentItem();
	QListWidgetItem *newitem = ( fieldOrderView->item(fieldOrderView->currentRow() + (distance == 1 ? 1 : -1)));
	if (!newitem) return;
	QString ttt=newitem->text();
	Qt::CheckState s = ((QListWidgetItem*)newitem)->checkState();
	newitem->setText(item->text());
	((QListWidgetItem*)newitem)->setCheckState(((QListWidgetItem*)item)->checkState());
	item->setText(ttt);
	((QListWidgetItem*)item)->setCheckState(s);
	fieldOrderView->setCurrentItem(newitem);
	doHighlighted(newitem,newitem);
}

void PartList::doHighlighted(QListWidgetItem * current, QListWidgetItem * /* previous */)
{
	int item = fieldOrderView->row(current);
	upButton->setEnabled(item > 0);
	downButton->setEnabled(item < fieldOrderView->count()-1);
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

