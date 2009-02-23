#include "qt4wrapper.h"
#include "Preferences.h"

#include "LDViewModelTree.h"

#include <TCFoundation/TCStringArray.h>

#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include "misc.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>

LDViewModelTree::LDViewModelTree(Preferences *pref, ModelViewerWidget *modelViewer)
	:ModelTreePanel(),
	modeltree(NULL),
	m_modelWindow(modelViewer),
    mainmodel(NULL),
	preferences(preferences),
	optionsShown(true)
{
	preferences = pref;
	modelTreeView->setColumnWidthMode(0, QListView::Maximum);
	modelTreeView->header()->hide();
	modelTreeView->setSorting(-1);
	if (!TCUserDefaults::boolForKey(MODEL_TREE_OPTIONS_SHOWN_KEY, true, false))
    {
        hideOptions();
    }
	highlightSelectedLineBox->setChecked(TCUserDefaults::boolForKey(
			MODEL_TREE_HIGHLIGHT_KEY, false, false));
}

LDViewModelTree::~LDViewModelTree() { }

void LDViewModelTree::show(void)
{
	raise();
	setActiveWindow();
	fillTreeView();
	ModelTreePanel::show();
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
	modelTreeView->clear();
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
	QString line = QString(tree->getText().c_str());
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
		item = new QListViewItem(modelTreeView, 
								 modelTreeView->lastItem(), line);
	}
	item->setExpandable(tree->getNumChildren(true)>0);
}

void LDViewModelTree::updateLineChecks(void)
{
	if (!modeltree) return;
	preferences->setButtonState(unknownButton,
								modeltree->getShowLineType(LDLLineTypeUnknown));
	preferences->setButtonState(commentButton,
								modeltree->getShowLineType(LDLLineTypeComment));
	preferences->setButtonState(modelButton,
								modeltree->getShowLineType(LDLLineTypeModel));
	preferences->setButtonState(lineButton,
								modeltree->getShowLineType(LDLLineTypeLine));
	preferences->setButtonState(quadButton,
								modeltree->getShowLineType(LDLLineTypeQuad));
	preferences->setButtonState(triangleButton,
								modeltree->getShowLineType(LDLLineTypeTriangle));
	preferences->setButtonState(conditionalLineButton,
								modeltree->getShowLineType(LDLLineTypeConditionalLine));
}

void LDViewModelTree::itemexpanded(QListViewItem *item)
{
	LDModelTree *tree = findTree(item);
	if (tree && !tree->getViewPopulated())
	{
		addChildren(item, tree);
		tree->setViewPopulated(true);
	}
}

void LDViewModelTree::selectionChanged(QListViewItem *item)
{
	if (highlightSelectedLineBox->isChecked() && item)
	{
		LDModelTree *tree = findTree(item);
		m_modelWindow->getModelViewer()->setHighlightPaths(
						tree->getTreePath());
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
		list = modelTreeView->firstChild();
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
	if (modeltree) 
	{
		modeltree->setShowLineType(lineType,button->state());
		refreshTreeView();
	}
}

void LDViewModelTree::setModel(LDLMainModel *model)
{
    if (mainmodel != model)
    {
        modeltree = NULL;
        mainmodel = model;
    }
}

void LDViewModelTree::modelAlertCallback(TCAlert *alert)
{
    if (alert->getSender() == (TCAlertSender*)m_modelWindow->getModelViewer())
    {
        if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0)
        {
            setModel(m_modelWindow->getModelViewer()->getMainModel());
            fillTreeView();
        }
        else if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
        {
            setModel(NULL);
            fillTreeView();
        }
    }
}

void LDViewModelTree::setModelWindow(ModelViewerWidget *modelWindow)
{
    if (modelWindow != m_modelWindow)
    {
		m_modelWindow = modelWindow;
    }
    setModel(m_modelWindow->getModelViewer()->getMainModel());
}

void LDViewModelTree::hideOptions()
{
//	resize(size()-QSize(showLinesBox->size().width(),0));
	optionsShown = false;
	showLinesBox->hide();
	optionsButton->setText(optionsButton->text().replace(QChar('<'),
														 QChar('>')));

}

void LDViewModelTree::showOptions()
{
    optionsShown = true;
    showLinesBox->show();
//	resize(size()+QSize(showLinesBox->size().width(),0));
    optionsButton->setText(optionsButton->text().replace(QChar('>'),
													  	 QChar('<')));
}

void LDViewModelTree::toggleOptions()
{
	if (optionsShown)
		hideOptions();
	else
		showOptions();
	TCUserDefaults::setBoolForKey(optionsShown, MODEL_TREE_OPTIONS_SHOWN_KEY,
								  false);
}

void LDViewModelTree::highlightSelectedLine()
{
	bool checked = highlightSelectedLineBox->isChecked();
	if (!checked)
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	else
		selectionChanged(modelTreeView->selectedItem());
	TCUserDefaults::setBoolForKey(checked, MODEL_TREE_HIGHLIGHT_KEY, false);
}

void LDViewModelTree::highlightColor()
{
	int r,g,b;
	QColor color = QColorDialog::getColor(QColor(r,g,b));
	if(color.isValid())
	{
	}
}

