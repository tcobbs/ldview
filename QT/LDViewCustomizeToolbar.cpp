#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/mystring.h>

#include "ModelViewerWidget.h"
#include "LDViewCustomizeToolbar.h"

CustomizeToolbar::CustomizeToolbar(QWidget *parent, ModelViewerWidget *modelWidget)
	:QDialog(parent),CustomizeToolbarPanel(),
	modelWidget(modelWidget)
{
	setupUi(this);
	connect( addButton, SIGNAL( clicked() ), this, SLOT( doAdd() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( doRemove() ) );
	connect( resetButton, SIGNAL( clicked() ), this, SLOT( doReset() ) );
	connect( closeButton, SIGNAL( clicked() ), this, SLOT( doClose() ) );
	connect( moveUpButton, SIGNAL( clicked() ), this, SLOT( doMoveUp() ) );
	connect( moveDownButton, SIGNAL( clicked() ), this, SLOT( doMoveDown() ) );
	connect( CurrentButtonListView, SIGNAL( currentItemChanged ( QListWidgetItem * , QListWidgetItem * ) ), this, SLOT( doCurrentButtonSelected(QListWidgetItem *,QListWidgetItem *) ) );
	connect( AvailableButtonListView, SIGNAL( currentItemChanged ( QListWidgetItem * , QListWidgetItem * ) ), this, SLOT( doAvailableButtonSelected(QListWidgetItem *,QListWidgetItem *) ) );

	modelViewer = modelWidget->getModelViewer();
}

CustomizeToolbar::~CustomizeToolbar(void)
{
}

void CustomizeToolbar::show(void)
{
	QDialog::show();
	raise();
}

void CustomizeToolbar::doAdd(void)
{
}

void CustomizeToolbar::doRemove(void)
{
}

void CustomizeToolbar::doClose(void)
{
}

void CustomizeToolbar::doReset(void)
{
}

void CustomizeToolbar::doMoveUp(void)
{
}

void CustomizeToolbar::doMoveDown(void)
{
}

void CustomizeToolbar::doCurrentButtonSelected(QListWidgetItem * /*current*/,QListWidgetItem * /*previous*/)
{
}

void CustomizeToolbar::doAvailableButtonSelected(QListWidgetItem * /*current*/,QListWidgetItem * /*previous*/)
{
}

