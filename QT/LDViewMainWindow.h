#ifndef __LDVIEWMAINWINDOW_H__
#define __LDVIEWMAINWINDOW_H__

#include "ui_LDView.h"
#include "ModelViewerWidget.h"
#include <qapplication.h>
#include <qaction.h>

class LDViewMainWindow : public QMainWindow , Ui::LDView
{
	Q_OBJECT
public:
	LDViewMainWindow(QApplication * /* a */);

	void fileSaveSetEnabled(bool b)        {fileSaveAction->setEnabled(b);}
	void fileReloadSetEnabled(bool b)      {fileReloadAction->setEnabled(b);}
	void toolbarFirstStepSetEnabled(bool b){toolbarFirstStep->setEnabled(b);}
	void toolbarNextStepSetEnabled(bool b) {toolbarNextStep->setEnabled(b);}
	void toolbarPrevStepSetEnabled(bool b) {toolbarPrevStep->setEnabled(b);}
	void toolbarLastStepSetEnabled(bool b) {toolbarLastStep->setEnabled(b);}
	void setupStandardSizes();
	void toolbarMaxStepSetText(QString s)  {toolbarMaxStep->setText(s);}
	void toolbarCurrentStepSetText(QString s) {toolbarCurrentStep->setText(s);}
	void setToolbarOn(bool b) {viewToolBarAction->setOn(b);}
	void setStatusbarOn(bool b){viewStatusBarAction->setOn(b);}
	void setStepGotoEnabled(bool b) {stepGoto->setEnabled(b);}
	void setToolbarWireframeOn(bool b){toolbarWireframeAction->setOn(b);}
	void setToolbarEdgeOn(bool b) {toolbarEdgeAction->setOn(b);}
	void setToolbarLightingOn(bool b) {toolbarLightingAction->setOn(b);}
	void setToolbarBFCOn(bool b){toolbarBFCAction->setOn(b);}
	void setToolbarAxesOn(bool b){toolbarAxesAction->setOn(b);}
	void setToolbarSeamsOn(bool b){toolbarSeamsAction->setOn(b);}
	void setToolbarPrimitiveSubstitutionOn(bool b){toolbarPrimitiveSubstitutionAction->setOn(b);}
	void setPollAction(LDVPollMode mode);
	void setExamineModeOn(bool b){examineModeAction->setOn(b);}
	void setFlythroughModeOn(bool b){flythroughModeAction->setOn(b);}
	void setViewLatitudeRotationOn(bool b){viewLatitudeRotationAction->setOn(b);}
	void setShowPovAspectRatioOn(bool b){showPovAspectRatioAction->setOn(b);}
//	void setMainGroupBoxMargin(int i){MainGroupBox->layout()->setMargin(i);}
	void setMenuItemsEnabled(QMenu *menu, bool enabled);
	void showToolbar(bool b) { if (b) {toolbar->show();} else {toolbar->hide();}}
	void showMenubar(bool b) { if (b) {menubar->show();} else {menubar->hide();}}
	void clearRecentFileMenuItems(void);
	char *truncateFilename(const char *i /* filename */);
	void populateRecentFileMenuItems(void);
	void populateRecentFiles(void);
	void recordRecentFiles(void);
	static TCStringArray* recentFiles;

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
	void pollChanged(QAction *action);
	void viewModeChanged(QAction *action){
    modelViewer->doViewModeChanged((action == flythroughModeAction) ? LDInputHandler::VMFlyThrough : LDInputHandler::VMExamine);}
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
    void doFileMenuAboutToShow(void);
    void doEditMenuAboutToShow(void);
    void doViewMenuAboutToShow(void);
    void doToolsMenuAboutToShow(void);
    void doHelpMenuAboutToShow(void);
	void doRecentFile(int);
	void standardSizeSelected(int);

private:
	LDrawModelViewer::StandardSizeVector standardSizes;
#ifdef __APPLE__
    QMenu *openRecentMenu;
#endif // __APPLE__
	int fileSeparatorIndex;
	QLabel *toolbarMaxStep, *toolbarCurrentStep, *toolbarStepLabel;
};

#endif
