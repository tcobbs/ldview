#ifndef __LDVIEWMODELTREE_H__
#define __LDVIEWMODELTREE_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ui_ModelTreePanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>
#include <LDLib/LDModelTree.h>
#include <LDLoader/LDLMainModel.h>
#if QT_VERSION < 0x50000
#include <QWindowsStyle>
#endif

class QLabel;
class Preferences;
class LDrawModelViewer;
class ModelViewerWidget;
class LDViewModelTree : public QMainWindow , Ui::ModelTreePanel
{
	Q_OBJECT
public:
	LDViewModelTree(QWidget *parent,Preferences *preferences, ModelViewerWidget *modelViewer);
	~LDViewModelTree();
	void show(void);
	void showOptions(),hideOptions();

public slots:
	void itemexpand(QTreeWidgetItem *item);
	void selectionChanged(QTreeWidgetItem *item,QTreeWidgetItem *old);
	void quad(void) {doLineCheck(quadButton,LDLLineTypeQuad);}
	void unknown(void) {doLineCheck(unknownButton,LDLLineTypeUnknown);}
	void line(void) {doLineCheck(lineButton,LDLLineTypeLine);}
	void conditionalLine(void) {doLineCheck(conditionalLineButton,LDLLineTypeConditionalLine);}
	void triangle(void) {doLineCheck(triangleButton,LDLLineTypeTriangle);}
	void model(void) {doLineCheck(modelButton,LDLLineTypeModel);}
	void empty(void) {doLineCheck(emptyButton,LDLLineTypeEmpty);}
	void highlightSelectedLine();
	void highlightColor();
	void comment(void) {doLineCheck(commentButton,LDLLineTypeComment);}
    void modelAlertCallback(TCAlert *alert);
	void toggleOptions();
	void searchBackward();
	void searchForward();
	void search();
	void doSearch(LDModelTree::SearchMode mode, bool updateFocus);

protected:
    void doLineCheck(QCheckBox *button, LDLLineType lineType);
	void fillTreeView(void);
	void refreshTreeView(void);
	void addChildren(QTreeWidgetItem *parent, const LDModelTree *tree);
	void addLine(QTreeWidgetItem *parent, const LDModelTree *tree);
	void updateLineChecks(void);
    void setModel(LDLMainModel *model);
    void setModelWindow(ModelViewerWidget *modelWindow);
	QTreeWidgetItem *getChild(QTreeWidgetItem *parent, int index);
	void selectFromHighlightPath(std::string path);

	LDModelTree *findTree(QTreeWidgetItem *item);
	LDModelTree *modeltree;
	ModelViewerWidget *m_modelWindow;
	LDLMainModel *mainmodel;
	Preferences *preferences;
	QLabel *messageText;
	bool optionsShown;
	QStatusBar *statusbar;
	QLabel *statusText;
#if QT_VERSION < 0x50000
	QWindowsStyle qlStyle;
#endif
	std::string searchPath;
};

#endif // __LDVIEWMODELTREE_H__
