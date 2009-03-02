#ifndef __LDVIEWPARTLIST_H__
#define __LDVIEWPARTLIST_H__

#include "ModelViewerWidget.h"
#include "LDLib/LDHtmlInventory.h"
#include "PartList.h"

//class QButton;
class LDHtmlInventory;
class QCheckListItem;

class PartList : public PartListPanel
{
public:
	PartList(ModelViewerWidget *modelWidget, LDHtmlInventory *htmlInventory);
	~PartList(void);
    void populateColumnList();
    QCheckListItem *description;
    int exec();
    int result();

public slots:
	void doOk();
	void doCancel();
	void doUp();
	void doDown();
	void doHighlighted();
	void doShowModel();
protected:
	void doMoveColumn(int distance);
	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	LDHtmlInventory *m_htmlInventory;
};
#endif //__LDVIEWPARTLIST_H__
