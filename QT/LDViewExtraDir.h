#ifndef __LDVIEWEXTRADIR_H__
#define __LDVIEWEXTRADIR_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ModelViewerWidget.h"
#include <qfiledialog.h>

class ExtraDirPanel;
class QListViewItem;
class QButton;

class ExtraDir
{
public:
	ExtraDir(ModelViewerWidget *modelWidget);
	~ExtraDir(void);

	void show(void);
	void clear(void);
	int populateListView(void);
	void populateExtraDirsListBox(void);
	void doAddExtraDir(void);
	void doDelExtraDir(void);
	void doUpExtraDir(void);
	void doDownExtraDir(void);
	void doOk(void);
	void setButtonState(QButton *button, bool state);
	void recordExtraSearchDirs(void);
	void populateExtraSearchDirs(void);
	static TCStringArray* extraSearchDirs;
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	ExtraDirPanel *panel;
	bool listViewPopulated;
	QFileDialog *fileDialog;
};

#endif 
