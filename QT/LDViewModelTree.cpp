#include "qt4wrapper.h"
#include "Preferences.h"

#include "LDViewModelTree.h"

#include <TCFoundation/TCStringArray.h>

#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include "misc.h"

LDViewModelTree::LDViewModelTree(Preferences *preferences, LDrawModelViewer *modelViewer)
	:modeltree(NULL),
	mainmodel(NULL),
	panel(new ModelTreePanel),
	preferences(preferences),
	listViewPopulated(false)
{
	panel->setModelTree(this);
	panel->modelTreeView->setColumnWidthMode(0, QListView::Maximum);
	panel->modelTreeView->header()->hide();
	panel->modelTreeView->setSorting(-1);
	mainmodel=modelViewer->getMainModel();
}

void LDViewModelTree::reflectSettings(void)
{ 
}

void LDViewModelTree::setValues(bool value)
{
}

LDViewModelTree::~LDViewModelTree(void)
{
}

void LDViewModelTree::show(void)
{
	panel->show();
	panel->raise();
	panel->setActiveWindow();
	fillTreeView();
}

void LDViewModelTree::clear(void)
{
	clearListView();
}

void LDViewModelTree::clearListView(void)
{
	listViewPopulated = false;
}

int LDViewModelTree::populateListView(void)
{
}

void LDViewModelTree::fillTreeView(void)
{
	if(!modeltree)
	{
		if(mainmodel)
		{
			modeltree = new LDModelTree(mainmodel);
		}
		updateLineChecks();
		refreshTreeView();
	}
}

void LDViewModelTree::refreshTreeView()
{
	panel->modelTreeView->clear();
	addChildren(NULL, modeltree);
}

void LDViewModelTree::addChildren(QListViewItem *parent, const LDModelTree *tree)
{
	if (tree != NULL && tree->hasChildren(true))
	{
		const LDModelTreeArray *children = tree->getChildren(true);
		int count = children->getCount();

		for (int i = 0; i < count; i++)
		{
			const LDModelTree *child = (*children)[i];
			addLine(parent, child);
		}
	}
}

void LDViewModelTree::addLine(QListViewItem *parent, const LDModelTree *tree)
{
	QString line = tree->getText();
    QListViewItem *item;

    if (parent)
    {
        if (parent->childCount() > 0)
        {
            QListViewItem *lastChild = parent->firstChild();

            while (lastChild->nextSibling() != NULL)
            {
                lastChild = lastChild->nextSibling();
            }
            item = new QListViewItem(parent, lastChild, line);
        }
        else
        {
            item = new QListViewItem(parent, line);
		}
	}
	else
	{
		item = new QListViewItem(panel->modelTreeView, 
								 panel->modelTreeView->lastItem(), line);
	}
	item->setExpandable(tree->getNumChildren(true)>0);
}

void LDViewModelTree::updateLineChecks(void)
{
	preferences->setButtonState(panel->unknownButton,
								modeltree->getShowLineType(LDLLineTypeUnknown));
	preferences->setButtonState(panel->commentButton,
								modeltree->getShowLineType(LDLLineTypeComment));
	preferences->setButtonState(panel->modelButton,
								modeltree->getShowLineType(LDLLineTypeModel));
	preferences->setButtonState(panel->lineButton,
								modeltree->getShowLineType(LDLLineTypeLine));
	preferences->setButtonState(panel->quadButton,
								modeltree->getShowLineType(LDLLineTypeQuad));
	preferences->setButtonState(panel->triangleButton,
								modeltree->getShowLineType(LDLLineTypeTriangle));
	preferences->setButtonState(panel->conditionalLineButton,
								modeltree->getShowLineType(LDLLineTypeConditionalLine));
}

void LDViewModelTree::expanded(QListViewItem *item)
{
	LDModelTree *tree = findTree(item);
	if (tree && !tree->getViewPopulated())
	{
		addChildren(item, tree);
		tree->setViewPopulated(true);
	}
}

LDModelTree *LDViewModelTree::findTree(QListViewItem *item)
{
	LDModelTree *tparent = NULL;
	QListViewItem *list;
	if (item->parent())
	{
		list = item->parent()->firstChild();
		tparent = findTree(item->parent());
	}
	else
	{
		tparent = modeltree;
		list = panel->modelTreeView->firstChild();
	}
	const LDModelTreeArray *children = tparent->getChildren(true);
	int i=0;
    while (list->nextSibling() != NULL && list != item)
    {
        list = list->nextSibling();
		i++;
    }
	if (list == item)
		return (LDModelTree *)(*children)[i];
	else
		return NULL;
}

void LDViewModelTree::doLineCheck(QCheckBox *button, LDLLineType lineType)
{
	modeltree->setShowLineType(lineType,button->state());
	refreshTreeView();
}
