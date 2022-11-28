#ifndef __LDVIEWCUSTOMIZETOOLBAR_H__
#define __LDVIEWCUSTOMIZETOOLBAR_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ui_CustomizeToolbar.h"
#include <QDialog>

class ModelViewerWidget;
class LDrawModelViewer ;
class TCStringArray;

class CustomizeToolbar : public QDialog , Ui::CustomizeToolbarPanel
{
	Q_OBJECT
public:
	CustomizeToolbar(QWidget *parent,ModelViewerWidget *modelWidget);
	~CustomizeToolbar(void);

	void show(void);
	void clear(void);

public slots:
	void doAdd(void);
	void doRemove(void);
	void doClose(void);
	void doReset(void);
	void doMoveUp(void);
	void doMoveDown(void);
	void doCurrentButtonSelected(QListWidgetItem *,QListWidgetItem *);
	void doAvailableButtonSelected(QListWidgetItem *,QListWidgetItem *);

protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
};

#endif
