#ifndef __LDVIEWPARTLIST_H__
#define __LDVIEWPARTLIST_H__

#include "ModelViewerWidget.h"
#include "LDLib/LDHtmlInventory.h"
#include "ui_PartList.h"

class PartList : public QDialog , Ui::PartListPanel
{
	Q_OBJECT
public:
	PartList(QWidget *parent, ModelViewerWidget *modelWidget, LDHtmlInventory *htmlInventory);
	~PartList(void);
	void populateColumnList();
//	QCheckListItem *description;
	int exec();

public slots:
	void doOk();
	void doCancel();
	void doUp();
	void doDown();
	void doHighlighted(QListWidgetItem * current, QListWidgetItem * previous );
	void doShowModel();
protected:
	void doMoveColumn(int distance);
	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	LDHtmlInventory *m_htmlInventory;
};
#endif //__LDVIEWPARTLIST_H__
