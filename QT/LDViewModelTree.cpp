#include "Preferences.h"
#include "LDViewModelTree.h"
#include <TCFoundation/TCStringArray.h>
#include <QString>
#include <QStatusBar>
#include <QColor>
#include <QColorDialog>
#include "misc.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <QHeaderView>

LDViewModelTree::LDViewModelTree(QWidget *parent,Preferences *pref, ModelViewerWidget *modelViewer)
	:QMainWindow(parent),ModelTreePanel(),
	modeltree(NULL),
	m_modelWindow(modelViewer),
    mainmodel(NULL),
	preferences(pref),
	optionsShown(true)
{
	setupUi(this);
    connect( highlightSelectedLineBox, SIGNAL( clicked() ), this, SLOT( highlightSelectedLine() ) );
    connect( highlightColorEdit, SIGNAL( clicked() ), this, SLOT( highlightColor() ) );
    connect( commentButton, SIGNAL( clicked() ), this, SLOT( comment() ) );
    connect( modelButton, SIGNAL( clicked() ), this, SLOT( model() ) );
    connect( lineButton, SIGNAL( clicked() ), this, SLOT( line() ) );
    connect( triangleButton, SIGNAL( clicked() ), this, SLOT( triangle() ) );
    connect( quadButton, SIGNAL( clicked() ), this, SLOT( quad() ) );
    connect( conditionalLineButton, SIGNAL( clicked() ), this, SLOT( conditionalLine() ) );
    connect( emptyButton, SIGNAL( clicked() ), this, SLOT( empty() ) );
    connect( unknownButton, SIGNAL( clicked() ), this, SLOT( unknown() ) );
    connect( modelTreeView, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( itemexpand(QTreeWidgetItem*) ) );
    connect( modelTreeView, SIGNAL( currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ), this, SLOT( selectionChanged(QTreeWidgetItem*,QTreeWidgetItem*) ) );
    connect( optionsButton, SIGNAL( clicked() ), this, SLOT( toggleOptions() ) );
	connect( searchLeftButton, SIGNAL( clicked() ), this, SLOT ( searchBackward () ) );
	connect( searchRightButton, SIGNAL( clicked () ), this, SLOT ( searchForward () ) );
	connect( searchLineEdit, SIGNAL( returnPressed()), this, SLOT ( search () ) );

	long color = TCUserDefaults::longForKey(MODEL_TREE_HIGHLIGHT_COLOR_KEY,
			(0xa0e0ff), false);
	QPalette palette;
	palette.setColor(QPalette::Button, QColor(color >>16, (color >>8) & 0xff, color & 0xff));
	highlightColorEdit->setPalette(palette);
//	modelTreeView->setColumnWidthMode(0, QListView::Maximum);
	modelTreeView->header()->hide();
//	modelTreeView->setSorting(-1);
	if (!TCUserDefaults::boolForKey(MODEL_TREE_OPTIONS_SHOWN_KEY, true, false))
    {
        hideOptions();
    }
	highlightSelectedLineBox->setChecked(TCUserDefaults::boolForKey(
			MODEL_TREE_HIGHLIGHT_KEY, false, false));
	statusbar = statusBar();
	statusText = new QLabel(statusbar);
	statusbar->addWidget(statusText, 1);
	statusbar->show();
    QStyle *style = highlightColorEdit->style();
	if (style != NULL)
	{
		QString styleName = style->metaObject()->className();
		if (styleName == "QGtkStyle")
		{
			// QGtkStyle uses an image for the background, and doesn't show
			// the background color at all, so update the color buttons to use
			// the QWindowsStyle instead.
#if QT_VERSION < 0x50000
			highlightColorEdit->setStyle(&qlStyle);
#else
			highlightColorEdit->setStyle(QStyleFactory::create("Windows"));
#endif
		}
	}
}

LDViewModelTree::~LDViewModelTree() { }

QTreeWidgetItem *LDViewModelTree::getChild(QTreeWidgetItem *parent, int index)
{
	return (parent ? parent->child(index -1) : modelTreeView->topLevelItem(index - 1));
}

void LDViewModelTree::selectFromHighlightPath(std::string path)
{
	QTreeWidgetItem *item = NULL;
	path = modeltree->adjustHighlightPath(path);
	//printf("%s\n",path.c_str());
	while (path.size() > 0)
	{
		int lineNumber = atoi(&path[1]);
		item = getChild(item, lineNumber);
		if (item)
		{
			size_t index = path.find('/', 1);
			if (index < path.size())
			{
				itemexpand(item);
				modelTreeView->expandItem(item);
				path = path.substr(index);
			}
			else
			{
				LDModelTree *tree = findTree(item);
				if (tree)
				{
					searchPath = tree->getTreePath();
				}
				modelTreeView->setCurrentItem(item);
				path = "";
			}
		}
		else
		{
			return;
		}
	}
}

void LDViewModelTree::show(void)
{
	raise();
//	setActiveWindow();
	fillTreeView();
	QMainWindow::show();
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
		StringList paths = m_modelWindow->getModelViewer()->getHighlightPaths();
		for (StringList::const_iterator it = paths.begin(); it != paths.end(); it++)
		{
			selectFromHighlightPath(*it);
		}
	}
}

void LDViewModelTree::refreshTreeView()
{
	modelTreeView->clear();
	addChildren(NULL, modeltree);
}

void LDViewModelTree::addChildren(QTreeWidgetItem *parent, const LDModelTree *tree)
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

void LDViewModelTree::addLine(QTreeWidgetItem *parent, const LDModelTree *tree)
{
#ifdef TC_NO_UNICODE
	QString line = QString(tree->getText().c_str());
#else // TC_NO_UNICODE
	QString line;
	wstringtoqstring(line, tree->getTextUC());
#endif // TC_NO_UNICODE
    QTreeWidgetItem *item;

    if (parent)
    {
        if (parent->childCount() > 0)
        {
            QTreeWidgetItem *lastChild = parent->child(parent->childCount()-1);

            item = new QTreeWidgetItem(parent, lastChild);
        }
        else
        {
            item = new QTreeWidgetItem(parent);
		}
	}
	else
	{
		item = new QTreeWidgetItem(modelTreeView, 
								 modelTreeView->topLevelItem(modelTreeView->topLevelItemCount()-1));
	}
	item->setText(0, line);
#if QT_VERSION >= 0x40200
	int r, g, b;
	if (!tree->getBackgroundRGB(r, g, b))
	{
		r = g = b = 255;
	}
	item->setBackground(0, QBrush(QColor(r, g, b)));
#endif
#if QT_VERSION >= 0x40300
	item->setChildIndicatorPolicy(tree->getNumChildren(true)>0 ? QTreeWidgetItem::ShowIndicator : QTreeWidgetItem::DontShowIndicator);
#endif
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

void LDViewModelTree::itemexpand(QTreeWidgetItem *item)
{
	LDModelTree *tree = findTree(item);
	if (tree && !tree->getViewPopulated())
	{
		addChildren(item, tree);
		tree->setViewPopulated(true);
	}
}

void LDViewModelTree::selectionChanged(QTreeWidgetItem *item,QTreeWidgetItem* /* old */)
{
	if (item)
	{
		LDModelTree *tree = findTree(item);
		QString qs;
		if (highlightSelectedLineBox->isChecked())
		{
			m_modelWindow->getModelViewer()->setHighlightPaths(
						tree->getTreePath());
		}
		ucstringtoqstring(qs,tree->getStatusText());
		statusText->setText(qs);
	}
}

LDModelTree *LDViewModelTree::findTree(QTreeWidgetItem *item)
{
	LDModelTree *tparent = NULL;
	QTreeWidgetItem *parent;
	int i;
	if (item->parent())
	{
		parent = item->parent();
		tparent = findTree(item->parent());
		const LDModelTreeArray *children = tparent->getChildren(true);
		for(i=0; (i < parent->childCount()) && (parent->child(i) != item); i++)
		{}
		if (parent->child(i) == item)
			return (LDModelTree *)(*children)[i];
		else
			return NULL;
	}
	else
	{
		tparent = modeltree;
		const LDModelTreeArray *children = tparent->getChildren(true);
		for(i=0; (i < modelTreeView->topLevelItemCount()) && (modelTreeView->topLevelItem(i) != item); i++)
		{}
		if ( modelTreeView->topLevelItem(i) == item)
			return (LDModelTree *)(*children)[i];
		else
			return NULL;			
	}
}

void LDViewModelTree::doLineCheck(QCheckBox *button, LDLLineType lineType)
{
	if (modeltree) 
	{
		modeltree->setShowLineType(lineType,button->isChecked());
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

void LDViewModelTree::searchBackward()
{
	doSearch(LDModelTree::SearchMode::SMPrevious, true);
}

void LDViewModelTree::searchForward()
{
	doSearch(LDModelTree::SearchMode::SMNext, true);
}

void LDViewModelTree::search()
{
	doSearch(LDModelTree::SearchMode::SMType, false);
}

void LDViewModelTree::doSearch(LDModelTree::SearchMode mode, bool updateFocus)
{
	ucstring searchString;
	qstringtoucstring(searchString, searchLineEdit->text());
	std::string pathString = searchPath;
	if (modeltree->search(searchString, pathString, mode))
	{
		selectFromHighlightPath(pathString);
		if (updateFocus)
		{
		}
	}
	else
	{
	}
}

void LDViewModelTree::highlightSelectedLine()
{
	bool checked = highlightSelectedLineBox->isChecked();
	if (!checked)
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	else
		selectionChanged((modelTreeView->selectedItems()[0]),NULL);
	TCUserDefaults::setBoolForKey(checked, MODEL_TREE_HIGHLIGHT_KEY, false);
}

void LDViewModelTree::highlightColor()
{
	long r,g,b;
	QColor color = QColorDialog::getColor(highlightColorEdit->palette().color(QPalette::Button));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button,color);
		highlightColorEdit->setPalette(palette);
		m_modelWindow->getModelViewer()->setHighlightColor(
				r = color.red(), g = color.green(), b = color.blue());
		TCUserDefaults::setLongForKey(
			(r<<16 | g<<8 | b),
			MODEL_TREE_HIGHLIGHT_COLOR_KEY, false);
	}
}

