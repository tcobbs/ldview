#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/mystring.h>

#include "ModelViewerWidget.h"
#include "LDViewExtraDir.h"
#define	MAX_EXTRA_DIR	10

TCStringArray* ExtraDir::extraSearchDirs = NULL;

ExtraDir::ExtraDir(QWidget *parent, ModelViewerWidget *modelWidget)
	:QDialog(parent),ExtraDirPanel(),
	modelWidget(modelWidget)
{
	setupUi(this);
    connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
    connect( addExtraDirButton, SIGNAL( clicked() ), this, SLOT( doAddExtraDir() ) );
    connect( delExtraDirButton, SIGNAL( clicked() ), this, SLOT( doDelExtraDir() ) );
    connect( upExtraDirButton, SIGNAL( clicked() ), this, SLOT( doUpExtraDir() ) );
    connect( downExtraDirButton, SIGNAL( clicked() ), this, SLOT( doDownExtraDir() ) );
    connect( ExtraDirListView, SIGNAL( currentItemChanged ( QListWidgetItem * , QListWidgetItem * ) ), this, SLOT( doExtraDirSelected(QListWidgetItem *,QListWidgetItem *) ) );

    modelViewer = modelWidget->getModelViewer();
    if (!extraSearchDirs)
    {
        extraSearchDirs = new TCStringArray;
        populateExtraSearchDirs();
    }
}

ExtraDir::~ExtraDir(void)
{
}

void ExtraDir::show(void)
{
	populateExtraSearchDirs();
	populateExtraDirsListBox();
	QDialog::show();
	raise();
//	setActiveWindow();
}

void ExtraDir::doAddExtraDir(void)
{
	int count=ExtraDirListView->count();
	if (count>=MAX_EXTRA_DIR) { return;}
	QString selectedfile = QFileDialog::getExistingDirectory(this,"Choose a Directory",".");
	if (!selectedfile.isEmpty())
    {
		new QListWidgetItem(selectedfile,ExtraDirListView);
		extraSearchDirs->addString(selectedfile.toUtf8().constData());
		delExtraDirButton->setEnabled(true);
		if (count==MAX_EXTRA_DIR-1)
		{
			addExtraDirButton->setEnabled(false);
		}
	}
	doExtraDirSelected();
}

void ExtraDir::doDelExtraDir(void)
{
	int index=ExtraDirListView->currentRow(),
		count=ExtraDirListView->count();
	if (index!=-1)
	{
		extraSearchDirs->removeStringAtIndex(index);
		delete ExtraDirListView->currentItem();
		if (count==1)
		{
			delExtraDirButton->setEnabled(false);
		}
		if (count==MAX_EXTRA_DIR)
		{
			addExtraDirButton->setEnabled(true);
		}
	}
}
void ExtraDir::doExtraDirSelected(void)
{
	int index=ExtraDirListView->currentRow(),
		count=ExtraDirListView->count();
	upExtraDirButton->setEnabled(index>0 ? true : false);
	downExtraDirButton->setEnabled(((index == count-1) && (count > 0)) ? false : true);
}

void ExtraDir::doUpExtraDir(void)
{
	int index=ExtraDirListView->currentRow(),
		count=ExtraDirListView->count();
	char *extraDir;

    if (index>0 && count >1)
    {
		QString tmp=ExtraDirListView->currentItem()->text();
		delete ExtraDirListView->currentItem();
		QListWidgetItem *newitem = new QListWidgetItem(tmp);
		ExtraDirListView->insertItem(index-1,newitem);
		ExtraDirListView->setCurrentItem(ExtraDirListView->item(index-1));
		extraDir=copyString(extraSearchDirs->stringAtIndex(index));
		extraSearchDirs->removeStringAtIndex(index);
		extraSearchDirs->insertString(extraDir,index-1);
		delete extraDir;
	}
}

void ExtraDir::doDownExtraDir(void)
{
    int index=ExtraDirListView->currentRow(),
        count=ExtraDirListView->count();
	char *extraDir;
    if (index<count-1 && count>0 && index!=-1)
	{
		QString tmp=ExtraDirListView->currentItem()->text();
		delete ExtraDirListView->currentItem();
		ExtraDirListView->insertItem(index+1,new QListWidgetItem(tmp));
		ExtraDirListView->setCurrentItem(ExtraDirListView->item(index+1));
		extraDir=copyString(extraSearchDirs->stringAtIndex(index));
		extraSearchDirs->removeStringAtIndex(index);
        extraSearchDirs->insertString(extraDir,index+1);
		delete extraDir;
	}
}

void ExtraDir::doOk()
{
	recordExtraSearchDirs();
	close();
}

void ExtraDir::populateExtraDirsListBox(void)
{
    int i;
    int count=ExtraDirListView->count();
	char *dir;
	for (i=0;i<count;i++) { delete ExtraDirListView->item(0); }
	count = extraSearchDirs->getCount();
	for (i=0;i<count;i++) 
	{
		dir=extraSearchDirs->stringAtIndex(i);
		if (dir && dir[0]) 
		{
			new QListWidgetItem(extraSearchDirs->stringAtIndex(i),ExtraDirListView);
		}
	}
	if (count==MAX_EXTRA_DIR)
	{
		addExtraDirButton->setEnabled(false);
	}
	delExtraDirButton->setEnabled(count>0 ? true : false);
}

void ExtraDir::recordExtraSearchDirs(void)
{
    int i;
    int count = extraSearchDirs->getCount();

    for (i = 0; i <= count; i++)
    {
        char key[128];
        char *extraDir;
                                                                                                                                                             
        snprintf(key, sizeof(key), "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i + 1);
        extraDir = extraSearchDirs->stringAtIndex(i);
        if (extraDir)
        {
            TCUserDefaults::setStringForKey(extraDir, key, false);
        }
        else
        {
            TCUserDefaults::removeValue(key, false);
        }
    }
	for (i=count; i<MAX_EXTRA_DIR;i++)
	{
		char key[128];
        char extraDir[]="";


        snprintf(key, sizeof(key), "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i + 1);
		TCUserDefaults::setStringForKey(extraDir, key, false);
	}
    if (modelViewer)
    {
        modelViewer->setExtraSearchDirs(extraSearchDirs);
    }
}

void ExtraDir::populateExtraSearchDirs(void)
{
    int i;

    extraSearchDirs->removeAll();
    for (i = 1; true; i++)
    {
        char key[128];
        char *extraSearchDir;

        snprintf(key, sizeof(key), "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i);
        extraSearchDir = TCUserDefaults::stringForKey(key, NULL, false);
        if (extraSearchDir && extraSearchDir[0])
        {
            extraSearchDirs->addString(extraSearchDir);
            delete extraSearchDir;
        }
        else
        {
            break;
        }
    }
}

