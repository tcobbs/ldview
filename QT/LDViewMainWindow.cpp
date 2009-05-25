#include "LDViewMainWindow.h"
#include <qdesktopwidget.h>
#include "misc.h"
#include <qtoolbutton.h>
#include <TCFoundation/TCLocalStrings.h>
#include <QObject>

TCStringArray *LDViewMainWindow::recentFiles = NULL;

LDViewMainWindow::LDViewMainWindow(QApplication *a)
	:Q3MainWindow(),Ui::LDView(),
	fileSeparatorIndex(-1)
{
    setupUi(this);
    if (modelViewer) modelViewer->setMainWindow(this);
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileSaveSettingsAction, SIGNAL( activated() ), this, SLOT( fileSaveSettings() ) );
    connect( fileExportAction, SIGNAL( activated() ), this, SLOT( fileExport() ) );
    connect( fileExportOptionAction, SIGNAL( activated() ), this, SLOT( fileExportOption() ) );
    connect( fileJPEGOptionsAction, SIGNAL( activated() ), this, SLOT( fileJPEGOptions() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( helpAboutQtAction, SIGNAL( activated() ), this, SLOT( helpAboutQt() ) );
    connect( editPreferencesAction, SIGNAL( activated() ), this, SLOT( editPreferences() ) );
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
    connect( viewFullScreenAction, SIGNAL( activated() ), this, SLOT( viewFullScreen() ) );
    connect( viewResetViewAction, SIGNAL( activated() ), this, SLOT( viewResetView() ) );
    connect( helpOpenGLDriverInfoAction, SIGNAL( activated() ), this, SLOT( helpOpenGLDriverInfo() ) );
    connect( fileLDrawDirAction, SIGNAL( activated() ), this, SLOT( fileLDrawDir() ) );
    connect( fileExtraDirAction, SIGNAL( activated() ), this, SLOT( fileExtraDir() ) );
    connect( fileCheckForUpdatesAction, SIGNAL( activated() ), this, SLOT( fileCheckForUpdates() ) );
    connect( fileReloadAction, SIGNAL( activated() ), this, SLOT( fileReload() ) );
    connect( viewShowErrorsAction, SIGNAL( activated() ), this, SLOT( viewShowErrors() ) );
    connect( pollActionGroup, SIGNAL( selected(QAction*) ), this, SLOT( pollChanged(QAction*) ) );
    connect( viewZoomToFitAction, SIGNAL( activated() ), this, SLOT( viewZoomToFit() ) );
    connect( viewModeActionGroup, SIGNAL( selected(QAction*) ), this, SLOT( viewModeChanged(QAction*) ) );
    connect( frontViewAngleAction, SIGNAL( activated() ), this, SLOT( frontViewAngle() ) );
    connect( backViewAngleAction, SIGNAL( activated() ), this, SLOT( backViewAngle() ) );
    connect( leftViewAngleAction, SIGNAL( activated() ), this, SLOT( leftViewAngle() ) );
    connect( rightViewAngleAction, SIGNAL( activated() ), this, SLOT( rightViewAngle() ) );
    connect( topViewAngleAction, SIGNAL( activated() ), this, SLOT( topViewAngle() ) );
    connect( bottomViewAngleAction, SIGNAL( activated() ), this, SLOT( bottomViewAngle() ) );
    connect( latLongViewAngleAction, SIGNAL( activated() ), this, SLOT( latLongViewAngle() ) );
    connect( isoViewAngleAction, SIGNAL( activated() ), this, SLOT( isoViewAngle() ) );
    connect( saveDefaultViewAngleAction, SIGNAL( activated() ), this, SLOT( saveDefaultViewAngle() ) );
    connect( fileCancelLoadAction, SIGNAL( activated() ), this, SLOT( fileCancelLoad() ) );
    connect( showViewInfoAction, SIGNAL( activated() ), this, SLOT( showViewInfo() ) );
    connect( showPovCameraAction, SIGNAL( activated() ), this, SLOT( showPovCamera() ) );
    connect( showPovAspectRatioAction, SIGNAL( toggled(bool) ), this, SLOT( showPovAspectRatio(bool) ) );
    connect( toolsPartListAction, SIGNAL( activated() ), this, SLOT( toolsPartList() ) );
    connect( toolbarPrevStep, SIGNAL( activated() ), this, SLOT( prevStep() ) );
    connect( toolbarNextStep, SIGNAL( activated() ), this, SLOT( nextStep() ) );
    connect( toolbarFirstStep, SIGNAL( activated() ), this, SLOT( firstStep() ) );
    connect( toolbarLastStep, SIGNAL( activated() ), this, SLOT( lastStep() ) );
    connect( stepGoto, SIGNAL( activated() ), this, SLOT( gotoStep() ) );
    connect( toolsModelTreeAction, SIGNAL( activated() ), this, SLOT( toolsModelTree() ) );
    connect( toolsBoundingBoxAction, SIGNAL( activated() ), this, SLOT( toolsBoundingBox() ) );
    connect( toolsMpdModelSelectionAction, SIGNAL( activated() ), this, SLOT( toolsMpdModelSelection() ) );

    connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(doFileMenuAboutToShow()));
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(doEditMenuAboutToShow()));
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(doViewMenuAboutToShow()));
    connect(toolsMenu,SIGNAL(aboutToShow()), this, SLOT(doToolsMenuAboutToShow()));
    connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(doHelpMenuAboutToShow()));

	int cnt,i;
	QMenuItem *item;
    for ( cnt = i = 0; ; i++)
    {
        item = fileMenu->findItem(fileMenu->idAt(i));
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
#ifdef HAVE_QT4
    openRecentMenu = new Q3PopupMenu(this, "openRecentMenu");
#else // QT3
    openRecentMenu = new QMenu(this, "openRecentMenu");
#endif // QT3
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
#if (QT_VERSION >>16)!=4
    QObjectList *toolButtons = toolbar->queryList("QToolButton");
    QObjectListIt it(*toolButtons);
    QObject *object;
    for (; (object = it.current()) != NULL; ++it)
    {
        QToolButton *button = (QToolButton*)object;
        if (button->textLabel() == TCLocalStrings::get("ViewingAngle"))
        {
            button->setPopup(viewingAnglePopupMenu);
            button->setPopupDelay(1);
        }
    }
#endif
    modelViewer->setApplication(a);
}

void LDViewMainWindow::standardSizeSelected(int i)
{
    QString text;
    QRegExp sep( "\\s+" );
    text = standardSizesPopupMenu->text(i);
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
        standardSizesPopupMenu->insertItem(qs, this,
                                SLOT(standardSizeSelected(int)), 0, i );
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
    pollAction->setOn(true);

}

void LDViewMainWindow::setMenuItemsEnabled(QMenu *menu, bool enabled)
{
    int count = menu->count();
    int i;

    for (i = 0; i < count; i++)
    {
        menu->setItemEnabled(menu->idAt(i), enabled);
    }
}

void LDViewMainWindow::doFileMenuAboutToShow(void)
{
    if (modelViewer->isLoading())
    {
        setMenuItemsEnabled(fileMenu, false);
        fileMenu->setItemEnabled(fileMenu->idAt(11), true); //Cancel
    }
    else
    {
        setMenuItemsEnabled(fileMenu, true);
        fileMenu->setItemEnabled(fileMenu->idAt(11), false);//Cancel
        if (!modelViewer->getModelViewer() || !modelViewer->getModelViewer()->getMainTREModel())
        {
            fileMenu->setItemEnabled(fileMenu->idAt(1), false); //Reload
            fileMenu->setItemEnabled(fileMenu->idAt(3), false);	//Save Snapshot
            fileMenu->setItemEnabled(fileMenu->idAt(6), false); //Export
            fileMenu->setItemEnabled(fileMenu->idAt(11), false);//Cancel Load
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
            viewMenu->setItemEnabled(viewMenu->idAt(3), true);
            viewMenu->setItemEnabled(viewMenu->idAt(2), true);
        }
        else
        {
            setMenuItemsEnabled(viewMenu, true);
            viewMenu->setItemEnabled(viewMenu->idAt(9), modelViewer->getViewMode() == LDInputHandler::VMExamine);
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
    QMenuItem *item;
    int index = fileSeparatorIndex + 1;
    int i;
    int count = fileMenu->count();

    for (i = index; i < count - 1; i++)
    {
        item = fileMenu->findItem(fileMenu->idAt(index));
        fileMenu->removeItemAt(index);
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
                int id = menu->insertItem(filename, this,
                    SLOT(doRecentFile(int)), 0, -1, fileSeparatorIndex + i + 1);

                menu->setItemParameter(id, i);
                delete filename;
            }
        }
#ifndef __APPLE__
        fileMenu->insertSeparator(fileMenu->count() - 1);
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

void LDViewMainWindow::doRecentFile(int index)
{
	modelViewer->doRecentFile(index);
}

