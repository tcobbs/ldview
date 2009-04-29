#ifndef __LDVIEWMAINWINDOW_H__
#define __LDVIEWMAINWINDOW_H__

#include "LDView.h"
#include "ModelViewerWidget.h"
#include <qapplication.h>

class LDViewMainWindow : public LDView
{
	Q_OBJECT
public:
	LDViewMainWindow():LDView()
	{if (modelViewer) modelViewer->setMainWindow(this);}
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
	void closeEvent(QCloseEvent *event){
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