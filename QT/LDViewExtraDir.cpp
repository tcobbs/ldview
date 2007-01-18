#include "qt4wrapper.h"

#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TCFoundation/mystring.h>

#include <qpushbutton.h>
#include <qstring.h>
#include "ModelViewerWidget.h"
#include "LDViewExtraDir.h"
#include "ExtraDirPanel.h"
#define	MAX_EXTRA_DIR	10

TCStringArray* ExtraDir::extraSearchDirs = NULL;

ExtraDir::ExtraDir(ModelViewerWidget *modelWidget)
	:modelWidget(modelWidget),
	panel(new ExtraDirPanel),
	fileDialog(NULL)
{
    modelViewer = modelWidget->getModelViewer();
    panel->setExtraDir(this);
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
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}

void ExtraDir::doAddExtraDir(void)
{
	int count=panel->ExtraDirListView->count();
	if (count>=MAX_EXTRA_DIR) { return;}
	if (!fileDialog)
	{
		fileDialog = new QFileDialog(".",
                "All Files (*)",
                panel,
                "open model dialog",
                true);
        fileDialog->setCaption("Choose a Directory");
        fileDialog->setSelectedFilter(0);
		fileDialog->setMode(QFileDialog::DirectoryOnly);
	}
	if (fileDialog->exec() == QDialog::Accepted)
    {
		panel->ExtraDirListView->insertItem(fileDialog->selectedFile());
		extraSearchDirs->addString(fileDialog->selectedFile().ascii());
		panel->delExtraDirButton->setEnabled(true);
		if (count==MAX_EXTRA_DIR-1)
		{
			panel->addExtraDirButton->setEnabled(false);
		}
	}
	doExtraDirSelected();
}

void ExtraDir::doDelExtraDir(void)
{
	int index=panel->ExtraDirListView->currentItem(),
		count=panel->ExtraDirListView->count();
	if (index!=-1)
	{
		extraSearchDirs->removeString(index);
		panel->ExtraDirListView->removeItem(index);
		if (count==1)
		{
			panel->delExtraDirButton->setEnabled(false);
		}
		if (count==MAX_EXTRA_DIR)
		{
			panel->addExtraDirButton->setEnabled(true);
		}
	}
}
void ExtraDir::doExtraDirSelected(void)
{
	int index=panel->ExtraDirListView->currentItem(),
		count=panel->ExtraDirListView->count();
	panel->upExtraDirButton->setEnabled(index>0 ? true : false);
	panel->downExtraDirButton->setEnabled(index == count-1 ? false : true);
}

void ExtraDir::doUpExtraDir(void)
{
	int index=panel->ExtraDirListView->currentItem(),
		count=panel->ExtraDirListView->count();
	char *extraDir;

    if (index>0 && count >1)
    {
		QString tmp=panel->ExtraDirListView->currentText();
		panel->ExtraDirListView->removeItem(index);
		panel->ExtraDirListView->insertItem(tmp,index-1);
		panel->ExtraDirListView->setCurrentItem(index-1);
		extraDir=copyString(extraSearchDirs->stringAtIndex(index));
		extraSearchDirs->removeString(index);
		extraSearchDirs->insertString(extraDir,index-1);
		delete extraDir;
	}
}

void ExtraDir::doDownExtraDir(void)
{
    int index=panel->ExtraDirListView->currentItem(),
        count=panel->ExtraDirListView->count();
	char *extraDir;
    if (index<count-1 && count>0 && index!=-1)
	{
		QString tmp=panel->ExtraDirListView->currentText();
        panel->ExtraDirListView->removeItem(index);
		panel->ExtraDirListView->insertItem(tmp,index+1);
		panel->ExtraDirListView->setCurrentItem(index+1);
		extraDir=copyString(extraSearchDirs->stringAtIndex(index));
		extraSearchDirs->removeString(index);
        extraSearchDirs->insertString(extraDir,index+1);
		delete extraDir;
	}
}

void ExtraDir::doOk()
{
	recordExtraSearchDirs();
}

void ExtraDir::populateExtraDirsListBox(void)
{
    int i;
    int count=panel->ExtraDirListView->count();
	char *dir;
	for (i=0;i<count;i++) { panel->ExtraDirListView->removeItem(0); }
	count = extraSearchDirs->getCount();
	for (i=0;i<count;i++) 
	{
		dir=extraSearchDirs->stringAtIndex(i);
		if (dir && dir[0]) 
		{
			panel->ExtraDirListView->insertItem(extraSearchDirs->stringAtIndex(i));
		}
	}
	if (count==MAX_EXTRA_DIR)
	{
		panel->addExtraDirButton->setEnabled(false);
	}
	panel->delExtraDirButton->setEnabled(count>0 ? true : false);
}

void ExtraDir::recordExtraSearchDirs(void)
{
    int i;
    int count = extraSearchDirs->getCount();

    for (i = 0; i <= count; i++)
    {
        char key[128];
        char *extraDir;
                                                                                                                                                             
        sprintf(key, "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i + 1);
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


        sprintf(key, "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i + 1);
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

        sprintf(key, "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i);
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

