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
class ModelTreePanel;
class Preferences;
class QListViewItem;
class QButton;
class QLabel;
class QCheckBox;
class LDrawModelViewer;
class LDViewModelTree
{
public:
	LDViewModelTree(Preferences *preferences, LDrawModelViewer *modelViewer);
	~LDViewModelTree(void);

	void show(void);
	void clear(void);
	int populateListView(void);
	void reflectSettings(void);
	void setValues(bool);
	void expanded(QListViewItem *item);
	void quad(void) {doLineCheck(panel->quadButton,LDLLineTypeQuad);}
	void unknown(void) {doLineCheck(panel->unknownButton,LDLLineTypeUnknown);}
	void line(void) {doLineCheck(panel->lineButton,LDLLineTypeLine);}
	void conditionalLine(void) {doLineCheck(panel->conditionalLineButton,LDLLineTypeConditionalLine);}
	void triangle(void) {doLineCheck(panel->triangleButton,LDLLineTypeTriangle);}
	void model(void) {doLineCheck(panel->modelButton,LDLLineTypeModel);}
	void empty(void) {doLineCheck(panel->emptyButton,LDLLineTypeEmpty);}
	void comment(void) {doLineCheck(panel->commentButton,LDLLineTypeComment);}
	void doLineCheck(QCheckBox *button, LDLLineType lineType);

protected:
	void clearListView(void);
	void fillTreeView(void);
	void refreshTreeView(void);
	void addChildren(QListViewItem *parent, const LDModelTree *tree);
	void addLine(QListViewItem *parent, const LDModelTree *tree);
	void updateLineChecks(void);

	LDModelTree *findTree(QListViewItem *item);
	LDModelTree *modeltree;
	LDLMainModel *mainmodel;
	ModelTreePanel *panel;
	Preferences *preferences;
	bool listViewPopulated;
	QLabel *messageText;
};

#endif // __LDVIEWMODELTREE_H__
