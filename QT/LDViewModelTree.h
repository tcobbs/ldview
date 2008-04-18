#ifndef __LDVIEWMODELTREE_H__
#define __LDVIEWMODELTREE_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ModelTreePanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>
#include <LDLib/LDModelTree.h>
#include <LDLoader/LDLMainModel.h>


class LDLMainModel;
class LDModelTree;
class Preferences;
class LDrawModelViewer;
class LDViewModelTree : public ModelTreePanel
{
//	Q_OBJECT
public:
	LDViewModelTree(Preferences *preferences, LDrawModelViewer *modelViewer,
					QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel);
	~LDViewModelTree();
	void show(void);

public slots:
	void itemexpanded(QListViewItem *item);
	void quad(void) {doLineCheck(quadButton,LDLLineTypeQuad);}
	void unknown(void) {doLineCheck(unknownButton,LDLLineTypeUnknown);}
	void line(void) {doLineCheck(lineButton,LDLLineTypeLine);}
	void conditionalLine(void) {doLineCheck(conditionalLineButton,LDLLineTypeConditionalLine);}
	void triangle(void) {doLineCheck(triangleButton,LDLLineTypeTriangle);}
	void model(void) {doLineCheck(modelButton,LDLLineTypeModel);}
	void empty(void) {doLineCheck(emptyButton,LDLLineTypeEmpty);}
	void comment(void) {doLineCheck(commentButton,LDLLineTypeComment);}

protected:
    void doLineCheck(QCheckBox *button, LDLLineType lineType);
	void fillTreeView(void);
	void refreshTreeView(void);
	void addChildren(QListViewItem *parent, const LDModelTree *tree);
	void addLine(QListViewItem *parent, const LDModelTree *tree);
	void updateLineChecks(void);

	LDModelTree *findTree(QListViewItem *item);
	LDModelTree *modeltree;
	LDLMainModel *mainmodel;
	Preferences *preferences;
	QLabel *messageText;
};

#endif // __LDVIEWMODELTREE_H__
