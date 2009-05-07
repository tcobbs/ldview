#ifndef __LDVIEWMAINWINDOW_H__
#define __LDVIEWMAINWINDOW_H__

#include "LDView.h"
#include "ModelViewerWidget.h"
#include <qapplication.h>
#include <qaction.h>

class LDViewMainWindow : public LDView
{
	Q_OBJECT
public:
	LDViewMainWindow():LDView()
	{if (modelViewer) modelViewer->setMainWindow(this);
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
}
public slots:
	void fileOpen() { 	modelViewer->doFileOpen();}
	void fileSave(){ 	modelViewer->doFileSave();}
	void fileSaveSettings(){ modelViewer->doFileSaveSettings();}
	void fileExport(){	modelViewer->fileExport();}
	void fileExportOption(){modelViewer->fileExportOption();}
	void fileJPEGOptions(){ modelViewer->doFileJPEGOptions();}
	void filePrint(){	modelViewer->doFilePrint();}
	void fileExit(){    	QApplication::exit();}
	void helpContents(){    modelViewer->doHelpContents();}
	void helpAbout(){	modelViewer->doHelpAbout();}
	void helpAboutQt(){	modelViewer->doHelpAboutQt();}
	void editPreferences(){	modelViewer->showPreferences();}
	void closeEvent(QCloseEvent * /* event */){
#if (QT_VERSION >>16)==3
    QMainWindow::closeEvent(event);
    if (event->isAccepted())
#endif
	fileExit();
}
	void viewStatusBar(bool flag){    modelViewer->doViewStatusBar(flag);}
	void viewToolBar(bool flag){    modelViewer->doViewToolBar(flag);}
	void toolbarWireframe(bool flag){    modelViewer->doWireframe(flag);}
	void toolbarEdge(bool flag){    modelViewer->doEdge(flag);}
	void toolbarLighting(bool flag){    modelViewer->doLighting(flag);}
	void toolbarBFC(bool flag){    modelViewer->doBFC(flag);}
	void toolbarAxes(bool flag){    modelViewer->doAxes(flag);}
	void toolbarPrimitiveSubstitution(bool flag){    modelViewer->doPrimitiveSubstitution(flag);}
	void toolbarSeams(bool flag){    modelViewer->doSeams(flag);}
	void viewFullScreen(){	modelViewer->doViewFullScreen();}
	void viewResetView(){    modelViewer->doViewReset();}
	void helpOpenGLDriverInfo(){    modelViewer->doHelpOpenGLDriverInfo();}
	void fileLDrawDir(){    modelViewer->doFileLDrawDir();}
	void fileExtraDir(){	modelViewer->showFileExtraDir();}
	void fileCheckForUpdates(){	 modelViewer->checkForLibraryUpdates();}
	void fileReload(){    modelViewer->doFileReload();}
	void viewShowErrors(){    modelViewer->doViewErrors();}
	void pollChanged(QAction *action){    modelViewer->doPollChanged(action);}
	void viewModeChanged(QAction *action){    modelViewer->doViewModeChanged(action);}
	void viewZoomToFit(){	modelViewer->doZoomToFit();}
	void frontViewAngle(){	modelViewer->doFrontViewAngle();}
	void backViewAngle(){	modelViewer->doBackViewAngle();}
	void leftViewAngle(){	modelViewer->doLeftViewAngle();}
	void rightViewAngle(){	modelViewer->doRightViewAngle();}
	void topViewAngle(){    modelViewer->doTopViewAngle();}
	void bottomViewAngle(){	modelViewer->doBottomViewAngle();}
	void latLongViewAngle(){modelViewer->doLatLongViewAngle();}
	void isoViewAngle(){	modelViewer->doIsoViewAngle();}
	void saveDefaultViewAngle(){    modelViewer->doSaveDefaultViewAngle();}
	void fileCancelLoad(){	modelViewer->doFileCancelLoad();}
	void showViewInfo(){	modelViewer->doShowViewInfo();}
	void showPovCamera(){	modelViewer->doShowPovCamera();}
	void showPovAspectRatio(bool flag){	modelViewer->doShowPovAspectRatio(flag);}
	void toolsPartList(){	modelViewer->doPartList();}
	void toolsModelTree(){	modelViewer->doModelTree();}
	void toolsBoundingBox(){modelViewer->doBoundingBox();}
	void toolsMpdModelSelection(){	modelViewer->doMpdModel();}
	void latitudeRotation(bool b){	modelViewer->switchExamineLatLong(b);}
	void prevStep(){	modelViewer->prevStep();}
	void nextStep(){	modelViewer->nextStep();}
	void firstStep(){	modelViewer->firstStep();}
	void lastStep(){	modelViewer->lastStep();}
	void gotoStep(){	modelViewer->gotoStep();}
};

#endif
