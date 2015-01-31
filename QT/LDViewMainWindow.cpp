#include "LDViewMainWindow.h"
#include <QDesktopWidget>
#include "misc.h"
#include <QToolButton>
#include <TCFoundation/TCLocalStrings.h>
#include <QObject>
#include <QAction>

TCStringArray *LDViewMainWindow::recentFiles = NULL;

LDViewMainWindow::LDViewMainWindow(QApplication *a)
	:QMainWindow(),Ui::LDView(),
	fileSeparatorIndex(-1),
	toolbarMaxStep(new QLabel),
	toolbarCurrentStep(new QLabel),
	toolbarStepLabel(new QLabel("Step :")),
	toolbarViewAngle(NULL),
	toolbarWireframeMenu(new QMenu(this)),
	edgeMenu(new QMenu(this)),
	bfcMenu(new QMenu(this)),
	primitiveMenu(new QMenu(this))
{
    setupUi(this);
	toolbar->insertWidget(toolbarFirstStep,toolbarStepLabel);
	toolbar->insertWidget(toolbarFirstStep,toolbarCurrentStep);
	toolbar->insertWidget(toolbarFirstStep,toolbarMaxStep);
    if (modelViewer) modelViewer->setMainWindow(this);
    connect( fileOpenAction, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
    connect( fileSaveSettingsAction, SIGNAL( triggered() ), this, SLOT( fileSaveSettings() ) );
    connect( fileExportAction, SIGNAL( triggered() ), this, SLOT( fileExport() ) );
    connect( fileExportOptionAction, SIGNAL( triggered() ), this, SLOT( fileExportOption() ) );
	connect( file3DSExportOptionAction, SIGNAL( triggered() ), this, SLOT( file3DSExportOption() ) );
    connect( fileJPEGOptionsAction, SIGNAL( triggered() ), this, SLOT( fileJPEGOptions() ) );
    connect( fileSaveAction, SIGNAL( triggered() ), this, SLOT( fileSave() ) );
    connect( filePrintAction, SIGNAL( triggered() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( triggered() ), this, SLOT( fileExit() ) );
    connect( helpContentsAction, SIGNAL( triggered() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( triggered() ), this, SLOT( helpAbout() ) );
    connect( helpAboutQtAction, SIGNAL( triggered() ), this, SLOT( helpAboutQt() ) );
    connect( editPreferencesAction, SIGNAL( triggered() ), this, SLOT( editPreferences() ) );
    connect( viewToolBarAction, SIGNAL( toggled(bool) ), this, SLOT( viewToolBar(bool) ) );
    connect( toolbarWireframeAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarWireframe(bool) ) );
    connect( toolbarEdgeAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarEdge(bool) ) );
    connect( toolbarPrimitiveSubstitutionAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarPrimitiveSubstitution(bool) ) );
    connect( toolbarLightingAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarLighting(bool) ) );
    connect( toolbarBFCAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarBFC(bool) ) );
    connect( toolbarAxesAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarAxes(bool) ) );
    connect( toolbarSeamsAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarSeams(bool) ) );
    connect( viewStatusBarAction, SIGNAL( toggled(bool) ), this, SLOT( viewStatusBar(bool) ) );
    connect( viewLatitudeRotationAction, SIGNAL( toggled(bool) ), this, SLOT( latitudeRotation(bool) ) );
    connect( viewKeepRightSideUpAction, SIGNAL( toggled(bool) ), this, SLOT( keepRightSideUp(bool) ) );
    connect( viewFullScreenAction, SIGNAL( triggered() ), this, SLOT( viewFullScreen() ) );
    connect( viewResetViewAction, SIGNAL( triggered() ), this, SLOT( viewResetView() ) );
    connect( helpOpenGLDriverInfoAction, SIGNAL( triggered() ), this, SLOT( helpOpenGLDriverInfo() ) );
    connect( fileLDrawDirAction, SIGNAL( triggered() ), this, SLOT( fileLDrawDir() ) );
    connect( fileExtraDirAction, SIGNAL( triggered() ), this, SLOT( fileExtraDir() ) );
    connect( fileCheckForUpdatesAction, SIGNAL( triggered() ), this, SLOT( fileCheckForUpdates() ) );
    connect( fileReloadAction, SIGNAL( triggered() ), this, SLOT( fileReload() ) );
    connect( viewShowErrorsAction, SIGNAL( triggered() ), this, SLOT( viewShowErrors() ) );
    connect( pollActionGroup, SIGNAL( triggered(QAction*) ), this, SLOT( pollChanged(QAction*) ) );
    connect( viewZoomToFitAction, SIGNAL( triggered() ), this, SLOT( viewZoomToFit() ) );
	connect( viewRightSideUpAction, SIGNAL( triggered() ), this, SLOT( viewRightSideUp() ) );
    connect( viewModeActionGroup, SIGNAL( triggered(QAction*) ), this, SLOT( viewModeChanged(QAction*) ) );
    connect( frontViewAngleAction, SIGNAL( triggered() ), this, SLOT( frontViewAngle() ) );
    connect( backViewAngleAction, SIGNAL( triggered() ), this, SLOT( backViewAngle() ) );
    connect( leftViewAngleAction, SIGNAL( triggered() ), this, SLOT( leftViewAngle() ) );
    connect( rightViewAngleAction, SIGNAL( triggered() ), this, SLOT( rightViewAngle() ) );
    connect( topViewAngleAction, SIGNAL( triggered() ), this, SLOT( topViewAngle() ) );
    connect( bottomViewAngleAction, SIGNAL( triggered() ), this, SLOT( bottomViewAngle() ) );
    connect( latLongViewAngleAction, SIGNAL( triggered() ), this, SLOT( latLongViewAngle() ) );
    connect( isoViewAngleAction, SIGNAL( triggered() ), this, SLOT( isoViewAngle() ) );
    connect( saveDefaultViewAngleAction, SIGNAL( triggered() ), this, SLOT( saveDefaultViewAngle() ) );
    connect( fileCancelLoadAction, SIGNAL( triggered() ), this, SLOT( fileCancelLoad() ) );
    connect( showViewInfoAction, SIGNAL( triggered() ), this, SLOT( showViewInfo() ) );
    connect( showPovCameraAction, SIGNAL( triggered() ), this, SLOT( showPovCamera() ) );
    connect( showPovAspectRatioAction, SIGNAL( toggled(bool) ), this, SLOT( showPovAspectRatio(bool) ) );
    connect( toolsPartListAction, SIGNAL( triggered() ), this, SLOT( toolsPartList() ) );
    connect( toolbarPrevStep, SIGNAL( triggered() ), this, SLOT( prevStep() ) );
    connect( toolbarNextStep, SIGNAL( triggered() ), this, SLOT( nextStep() ) );
    connect( toolbarFirstStep, SIGNAL( triggered() ), this, SLOT( firstStep() ) );
    connect( toolbarLastStep, SIGNAL( triggered() ), this, SLOT( lastStep() ) );
    connect( stepGoto, SIGNAL( triggered() ), this, SLOT( gotoStep() ) );
    connect( toolsModelTreeAction, SIGNAL( triggered() ), this, SLOT( toolsModelTree() ) );
    connect( toolsBoundingBoxAction, SIGNAL( triggered() ), this, SLOT( toolsBoundingBox() ) );
    connect( toolsMpdModelSelectionAction, SIGNAL( triggered() ), this, SLOT( toolsMpdModelSelection() ) );

    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(doFileMenuAboutToShow()));
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(doEditMenuAboutToShow()));
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(doViewMenuAboutToShow()));
    connect(toolsMenu,SIGNAL(aboutToShow()), this, SLOT(doToolsMenuAboutToShow()));
    connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(doHelpMenuAboutToShow()));

	int cnt,i;
	QAction *item;
    for ( cnt = i = 0; ; i++)
    {
        item = fileMenu->actions()[i];
        if (item->isSeparator())
        {
            if (++cnt == 2)
                break;
        }
    }
    fileSeparatorIndex = i;
#ifdef __APPLE__
    fileMenu->removeItemAt(fileSeparatorIndex);
    fileSeparatorIndex = -1;
    openRecentMenu = new QMenu(this, "openRecentMenu");
    fileMenu->insertItem("Open Recent", openRecentMenu, -1, 1);
#endif // __APPLE__
    if (!recentFiles)
    {
        recentFiles = new TCStringArray(10);
        populateRecentFiles();
    }
    populateRecentFileMenuItems();
	setupStandardSizes();
#ifdef __APPLE__
        // Since Preferences is the only item in the edit menu, we need to
        // delete the edit menu on the Mac, since the item is going to get
        // magically moved to the LDView menu.  The problem is, if we delete
        // the edit menu, the magic stops working, since it's apparently all
        // done on the fly.  So, we're going to create a new fully-functional
        // Preferences menu item at the top of the File menu, and THEN delete
        // the edit menu.  This newly created menu item won't be visible to the
        // user, but it will make the other one continue to function after the
        // deletion of the edit menu.
        fileMenu->insertItem("Preferences", this, SLOT(doPreferences()),
            0, -1, 0);
        // Remove the (empty without Preferences) edit menu.
        menuBar->removeItem(menuBar->idAt(1));
#endif //__APPLE__
	toolbarViewAngle = new QToolButton(toolbar);
	toolbarViewAngle->setMenu(viewingAnglePopupMenu);
	toolbarViewAngle->setPopupMode(QToolButton::InstantPopup);
	toolbarViewAngle->setIcon(QPixmap( ":/images/images/toolbar_view.png"));
	toolbarViewAngle->setEnabled(false);
	toolbar->insertWidget(editPreferencesAction,toolbarViewAngle);
	connect( wireframeFogAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarWireframeFog(bool) ) );
	connect( wireframeRemoveHiddenLinesAction, SIGNAL( toggled(bool) ), this, SLOT( toolbarWireframeRemoveHiddenLines(bool) ) );
	toolbarWireframeMenu->addAction(wireframeFogAction);
	toolbarWireframeMenu->addAction(wireframeRemoveHiddenLinesAction);
	QToolButton *toolbarWireframe =
		(QToolButton *)toolbar->widgetForAction(toolbarWireframeAction);	
	if (toolbarWireframe != NULL)
	{
		toolbarWireframe->setMenu(toolbarWireframeMenu);
		toolbarWireframe->setPopupMode(QToolButton::MenuButtonPopup);
	}
	connect( textureStudAction, SIGNAL( toggled(bool) ), this, SLOT( textureStud(bool) ) );
	primitiveMenu->addAction(textureStudAction);
	QToolButton *toolbarPrimitives =
		(QToolButton *)toolbar->widgetForAction(toolbarPrimitiveSubstitutionAction);
	if (toolbarPrimitives)
	{
		toolbarPrimitives->setMenu(primitiveMenu);
		toolbarPrimitives->setPopupMode(QToolButton::MenuButtonPopup);
	}
	connect( edgeShowEdgeOnlyAction, SIGNAL( toggled(bool) ), this, SLOT( edgeShowEdgeOnly(bool) ) );
	connect( edgeConditionalLineAction, SIGNAL( toggled(bool) ), this, SLOT( edgeConditionalLine(bool) ) );
	connect( edgeHighQualityAction, SIGNAL( toggled(bool) ), this, SLOT( edgeHighQuality(bool) ) );
	connect( edgeAlwaysBlackAction, SIGNAL( toggled(bool) ), this, SLOT( edgeAlwaysBlack(bool) ) );
	edgeMenu->addAction(edgeShowEdgeOnlyAction);
	edgeMenu->addAction(edgeConditionalLineAction);
	edgeMenu->addAction(edgeHighQualityAction);
	edgeMenu->addAction(edgeAlwaysBlackAction);
	QToolButton *toolbarEdge =
		(QToolButton *)toolbar->widgetForAction(toolbarEdgeAction);
	if (toolbarEdge)
	{
		toolbarEdge->setMenu(edgeMenu);
		toolbarEdge->setPopupMode(QToolButton::MenuButtonPopup);
	}
	connect( bfcRedBackFacesAction, SIGNAL( toggled(bool) ), this, SLOT( bfcRedBackFaces(bool) ) );
	connect( bfcGreenFrontFacesAction, SIGNAL( toggled(bool) ), this, SLOT( bfcGreenFrontFaces(bool) ) );
	connect( bfcBlueNeutralFacesAction, SIGNAL( toggled(bool) ), this, SLOT( bfcBlueNeutralFaces(bool) ) );
	bfcMenu->addAction(bfcRedBackFacesAction);
	bfcMenu->addAction(bfcGreenFrontFacesAction);
	bfcMenu->addAction(bfcBlueNeutralFacesAction);
	QToolButton *toolbarBfc =
		(QToolButton *)toolbar->widgetForAction(toolbarBFCAction);
	if (toolbarBfc)
	{
		toolbarBfc->setMenu(bfcMenu);
		toolbarBfc->setPopupMode(QToolButton::MenuButtonPopup);
	}
    modelViewer->setApplication(a);
}

void LDViewMainWindow::standardSizeSelected()
{
	QAction *action = qobject_cast<QAction *>(sender());
    QString text;
    QRegExp sep( "\\s+" );
    text = action->text();
    if (text != QString::null)
    {
        int w,h;
        bool ok;
        w = text.section(sep,0,0).toInt(&ok);
        h = text.section(sep,2,2).toInt(&ok);
        resize(w + size().width() -
                               modelViewer->getModelViewer()->getWidth(),
                           h + size().height() -
                               modelViewer->getModelViewer()->getHeight());
    }
}

void LDViewMainWindow::setupStandardSizes()
{
    QSize workArea = QApplication::desktop()->availableGeometry(this).size();
    QSize windowSize = frameSize();
    LDrawModelViewer::getStandardSizes(workArea.width() - windowSize.width() +
                                       modelViewer->getModelViewer()->getWidth(),
                                       workArea.height() - windowSize.height() +
                                       modelViewer->getModelViewer()->getHeight(),
                                       standardSizes);
    standardSizesPopupMenu->clear();
    for (size_t i = 0; i < standardSizes.size(); i++)
    {
        QString qs;
        ucstringtoqstring(qs, standardSizes[i].name);
        QAction *act = standardSizesPopupMenu->addAction(qs);
		connect( act, SIGNAL( triggered() ), this, SLOT(standardSizeSelected()) );
    }
}

void LDViewMainWindow::pollChanged(QAction *action)
{
	LDVPollMode newMode = LDVPollNone;
    if (action == promptPollingAction)
    {
        newMode = LDVPollPrompt;
    }
    else if (action == autoPollingAction)
    {
        newMode = LDVPollAuto;
    }
    else if (action == backgroundPollingAction)
    {
        newMode = LDVPollBackground;
    }
	modelViewer->doPollChanged(newMode);
}

void LDViewMainWindow::setPollAction(LDVPollMode mode)
{
	QAction *pollAction;
    switch (mode)
    {
    case LDVPollPrompt:
        pollAction = promptPollingAction;
        break;
    case LDVPollAuto:
        pollAction = autoPollingAction;
        break;
    case LDVPollBackground:
        pollAction = backgroundPollingAction;
        break;
    default:
		pollAction = noPollingAction;
        break;
    }
    pollAction->setChecked(true);

}

void LDViewMainWindow::setMenuItemsEnabled(QMenu *menu, bool enabled)
{
    int count = menu->actions().count();
    int i;

    for (i = 0; i < count; i++)
    {
        menu->actions()[i]->setEnabled(enabled);
    }
}

void LDViewMainWindow::doFileMenuAboutToShow(void)
{
    if (modelViewer->isLoading())
    {
        setMenuItemsEnabled(fileMenu, false);
        fileCancelLoadAction->setEnabled(true);
    }
    else
    {
        setMenuItemsEnabled(fileMenu, true);
        fileCancelLoadAction->setEnabled(false);
        if (!modelViewer->getModelViewer() || !modelViewer->getModelViewer()->getMainTREModel())
        {
            fileReloadAction->setEnabled(false);
			fileSaveAction->setEnabled(false);
			fileExportAction->setEnabled(false);
			filePrintAction->setEnabled(false);
			fileCancelLoadAction->setEnabled(false);
        }
    }
}

void LDViewMainWindow::doEditMenuAboutToShow(void)
{
    if (modelViewer->isLoading())
    {
        setMenuItemsEnabled(editMenu, false);
    }
    else
    {
        setMenuItemsEnabled(editMenu, true);
    }
}

void LDViewMainWindow::doViewMenuAboutToShow(void)
{
    if (modelViewer->isLoading())
    {
        setMenuItemsEnabled(viewMenu, false);
    }
    else
    {
        if (!modelViewer->getModelViewer() || !modelViewer->getModelViewer()->getMainTREModel())
        {
            setMenuItemsEnabled(viewMenu, false);
            viewToolBarAction->setEnabled(true);
			viewStatusBarAction->setEnabled(true);
        }
        else
        {
            setMenuItemsEnabled(viewMenu, true);
			viewLatitudeRotationAction->setEnabled( modelViewer->getViewMode() == LDInputHandler::VMExamine);
			viewKeepRightSideUpAction->setEnabled( modelViewer->getViewMode() == LDInputHandler::VMFlyThrough);
        }
    }
	//setupStandardSizes();
}

void LDViewMainWindow::doToolsMenuAboutToShow(void)
{
    if (modelViewer->isLoading())
    {
        setMenuItemsEnabled(toolsMenu, false);
    }
    else
    {
        if (!modelViewer->getModelViewer() || !modelViewer->getModelViewer()->getMainTREModel())
        {
            setMenuItemsEnabled(toolsMenu, false);
        }
        else
            setMenuItemsEnabled(toolsMenu, true);
    }
}

void LDViewMainWindow::doHelpMenuAboutToShow(void)
{
    setMenuItemsEnabled(helpMenu, !modelViewer->isLoading());
}

char *LDViewMainWindow::truncateFilename(const char *filename)
{
    if (filename)
    {
        int len = strlen(filename);

        if (len > 40)
        {
            char *retValue = new char[44];

            strncpy(retValue, filename, 10);
            strcpy(retValue + 10, "...");
            strcat(retValue, filename + len - 30);
            return retValue;
        }
        else
        {
            return copyString(filename);
        }
    }
    else
    {
        return NULL;
    }
}


void LDViewMainWindow::clearRecentFileMenuItems(void)
{
#ifdef __APPLE__
    if (openRecentMenu)
    {
        openRecentMenu->clear();
    }
#else // __APPLE__
    QAction *item;
    int index = fileSeparatorIndex + 1;
    int i;
    int count = fileMenu->actions().count();

    for (i = index; i < count - 1; i++)
    {
        item = fileMenu->actions()[index];
        fileMenu->removeAction(item);
    }
#endif // __APPLE__
}

void LDViewMainWindow::populateRecentFiles(void)
{
    int i;
    long maxRecentFiles = Preferences::getMaxRecentFiles();

    recentFiles->removeAll();
    for (i = 1; i <= maxRecentFiles; i++)
    {
        char *filename = Preferences::getRecentFile(i);

        if (filename)
        {
            recentFiles->addString(filename);
            delete filename;
        }
        else
        {
            recentFiles->addString(NULL);
        }
    }
}

void LDViewMainWindow::populateRecentFileMenuItems(void)
{
#ifdef __APPLE__
    if (!openRecentMenu)
    {
        return;
    }
#endif // __APPLE__
    clearRecentFileMenuItems();

    if (recentFiles->stringAtIndex(0))
    {
        int i;
        long maxRecentFiles = Preferences::getMaxRecentFiles();

        for (i = 0; i < maxRecentFiles; i++)
        {
            char *filename = truncateFilename(recentFiles->stringAtIndex(i));

            if (filename)
            {
#ifdef __APPLE__
                QMenu *menu = openRecentMenu;
#else // __APPLE__
                QMenu *menu = fileMenu;
#endif // __APPLE__
				QAction *act;
				act = new QAction(filename,this);
				act->setData(i);
				menu->insertAction(fileMenu->actions()[fileMenu->actions().count() - 1],
								   act);
				connect( act, SIGNAL( triggered() ), this, SLOT(doRecentFile()) );
                delete filename;
            }
        }
#ifndef __APPLE__
        fileMenu->insertSeparator(fileMenu->actions()[fileMenu->actions().count() - 1]);
#endif // __APPLE__
    }
}

void LDViewMainWindow::recordRecentFiles(void)
{
    int i;
    long maxRecentFiles = Preferences::getMaxRecentFiles();

    for (i = 1; i <= maxRecentFiles; i++)
    {
        char *filename = recentFiles->stringAtIndex(i - 1);

        Preferences::setRecentFile(i, filename);
    }
}

void LDViewMainWindow::doRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	modelViewer->doRecentFile(action->data().toInt());
}

