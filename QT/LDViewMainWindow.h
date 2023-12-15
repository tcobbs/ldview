#ifndef __LDVIEWMAINWINDOW_H__
#define __LDVIEWMAINWINDOW_H__

#include "ui_LDView.h"
#include "ModelViewerWidget.h"
#include <QApplication>
#include <QAction>
#include <QToolButton>

class LDViewMainWindow : public QMainWindow , Ui::LDView
{
	Q_OBJECT
public:
	LDViewMainWindow(QApplication * /* a */);

	void fileSaveSetEnabled(bool b)			{fileSaveAction->setEnabled(b);}
	void fileReloadSetEnabled(bool b)		{fileReloadAction->setEnabled(b);}
	void toolbarFirstStepSetEnabled(bool b)	{toolbarFirstStep->setEnabled(b);}
	void toolbarNextStepSetEnabled(bool b)	{toolbarNextStep->setEnabled(b);}
	void toolbarPrevStepSetEnabled(bool b)	{toolbarPrevStep->setEnabled(b);}
	void toolbarLastStepSetEnabled(bool b)	{toolbarLastStep->setEnabled(b);}
	void toolbarViewAngleSetEnabled(bool b)	{toolbarViewAngle->setEnabled(b);}
	void setupStandardSizes();
	void toolbarMaxStepSetText(QString s)	{toolbarMaxStep->setText(s);}
	void toolbarCurrentStepSetText(QString s) {toolbarCurrentStep->setText(s);}
	void setToolbarOn(bool b) {viewToolBarAction->setChecked(b);}
	void setStatusbarOn(bool b){viewStatusBarAction->setChecked(b);}
	void setStepGotoEnabled(bool b) {stepGoto->setEnabled(b);}
	void setToolbarWireframeOn(bool b){toolbarWireframeAction->setChecked(b);toolbarWireframeMenu->setEnabled(b);}
	void setToolbarWireframeFogOn(bool b){wireframeFogAction->setChecked(b);}
	void setToolbarWireframeRemoveHiddenLinesOn(bool b){wireframeRemoveHiddenLinesAction->setChecked(b);}
	void setToolbarEdgeShowEdgeOnlyOn(bool b){edgeShowEdgeOnlyAction->setChecked(b);}
	void setToolbarEdgeConditionalLineOn(bool b){edgeConditionalLineAction->setChecked(b);}
	void setToolbarEdgeHighQualityOn(bool b){edgeHighQualityAction->setChecked(b);}
	void setToolbarEdgeAlwaysBlackOn(bool b){edgeAlwaysBlackAction->setChecked(b);}
	void setToolbarBfcRedBackFacesOn(bool b){bfcRedBackFacesAction->setChecked(b);}
	void setToolbarBfcGreenFrontFacesOn(bool b){bfcGreenFrontFacesAction->setChecked(b);}
	void setToolbarBfcBlueNeutralFacesOn(bool b){bfcBlueNeutralFacesAction->setChecked(b);}
	void setToolbarTextureStudOn(bool b) {textureStudAction->setChecked(b);}
	void setToolbarEdgeOn(bool b) {toolbarEdgeAction->setChecked(b);edgeMenu->setEnabled(b);}
	void setToolbarLightingOn(bool b) {toolbarLightingAction->setChecked(b);}
	void setToolbarBFCOn(bool b){toolbarBFCAction->setChecked(b);bfcMenu->setEnabled(b);}
	void setToolbarAxesOn(bool b){toolbarAxesAction->setChecked(b);}
	void setToolbarSeamsOn(bool b){toolbarSeamsAction->setChecked(b);}
	void setToolbarPrimitiveSubstitutionOn(bool b){toolbarPrimitiveSubstitutionAction->setChecked(b);primitiveMenu->setEnabled(b);}
	void setPollAction(LDVPollMode mode);
	void setExamineModeOn(bool b){examineModeAction->setChecked(b);}
	void setFlythroughModeOn(bool b){flythroughModeAction->setChecked(b);}
	void setWalkModeOn(bool b){walkModeAction->setChecked(b);}
	void setViewLatitudeRotationOn(bool b){viewLatitudeRotationAction->setChecked(b);}
	void setKeepRightSideUpOn(bool b){viewKeepRightSideUpAction->setChecked(b);}
	void setShowPovAspectRatioOn(bool b){showPovAspectRatioAction->setChecked(b);}
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
	void fileOpen()		{modelViewer->doFileOpen();}
	void fileSave()		{modelViewer->doFileSave();}
	void fileSaveSettings(){ modelViewer->doFileSaveSettings();}
	void fileExport()	{modelViewer->fileExport();}
	void fileExportOption(){modelViewer->fileExportOption();}
	void file3DSExportOption() {modelViewer->file3DSExportOption();}
	void fileSTLExportOption() {modelViewer->fileSTLExportOption();}
	void fileJPEGOptions(){ modelViewer->doFileJPEGOptions();}
	void filePrint()	{modelViewer->doFilePrint();}
	void fileExit()		{QApplication::exit();}
	void helpContents()	{modelViewer->doHelpContents();}
	void helpAbout()	{modelViewer->doHelpAbout();}
	void helpAboutQt()	{modelViewer->doHelpAboutQt();}
	void editPreferences(){	modelViewer->showPreferences();}
	void closeEvent(QCloseEvent * /* event */){
#if (QT_VERSION >>16)==3
	QMainWindow::closeEvent(event);
	if (event->isAccepted())
#endif
	fileExit();
}
	void viewStatusBar(bool flag)	{modelViewer->doViewStatusBar(flag);}
	void viewToolBar(bool flag)		{modelViewer->doViewToolBar(flag);}
	void toolbarWireframe(bool flag){modelViewer->doWireframe(flag);toolbarWireframeMenu->setEnabled(flag);}
	void toolbarWireframeFog(bool flag) { modelViewer->doWireframeFog(flag);}
	void toolbarWireframeRemoveHiddenLines(bool flag) {modelViewer->doWireframeRemoveHiddenLines(flag);}
	void textureStud(bool flag) {modelViewer->doTextureStud(flag);}
	void edgeShowEdgeOnly(bool flag) {modelViewer->doShowEdgeOnly(flag);}
	void edgeConditionalLine(bool flag) {modelViewer->doConditionalLine(flag);}
	void edgeHighQuality(bool flag) {modelViewer->doHighQuality(flag);}
	void edgeAlwaysBlack(bool flag) {modelViewer->doAlwaysBlack(flag);}
	void bfcRedBackFaces(bool flag) {modelViewer->doRedBackFaces(flag);}
	void bfcGreenFrontFaces(bool flag) {modelViewer->doGreenFrontFaces(flag);}
	void bfcBlueNeutralFaces(bool flag) {modelViewer->doBlueNeutralFaces(flag);}
	void toolbarEdge(bool flag)		{modelViewer->doEdge(flag);edgeMenu->setEnabled(flag);}
	void toolbarLighting(bool flag)	{modelViewer->doLighting(flag);}
	void toolbarBFC(bool flag)		{modelViewer->doBFC(flag);bfcMenu->setEnabled(flag);}
	void toolbarAxes(bool flag)		{modelViewer->doAxes(flag);}
	void toolbarPrimitiveSubstitution(bool flag){modelViewer->doPrimitiveSubstitution(flag);primitiveMenu->setEnabled(flag);}
	void toolbarSeams(bool flag)	{modelViewer->doSeams(flag);}
	void viewFullScreen()			{modelViewer->doViewFullScreen();}
	void viewResetView()			{modelViewer->doViewReset();}
	void helpOpenGLDriverInfo()		{modelViewer->doHelpOpenGLDriverInfo();}
	void fileLDrawDir()				{modelViewer->doFileLDrawDir();}
	void fileExtraDir()				{modelViewer->showFileExtraDir();}
	void fileCheckForUpdates()		{modelViewer->checkForLibraryUpdates();}
	void fileReload()				{modelViewer->doFileReload();}
	void viewShowErrors()			{modelViewer->doViewErrors();}
	void pollChanged(QAction *action);
	void viewModeChanged(QAction *action){
		if (action == flythroughModeAction) modelViewer->doViewModeChanged(LDInputHandler::VMFlyThrough);
		else if (action == examineModeAction) modelViewer->doViewModeChanged(LDInputHandler::VMExamine);
		else if (action == walkModeAction) modelViewer->doViewModeChanged(LDInputHandler::VMWalk);}
	void viewZoomToFit()			{modelViewer->doZoomToFit();}
	void viewRightSideUp()			{modelViewer->doRightSideUp();}
	void viewCameraLocation()		{modelViewer->doCameraLocation();}
	void viewRotationCenter()		{modelViewer->doRotationCenter();}
	void frontViewAngle()			{modelViewer->doFrontViewAngle();}
	void backViewAngle()			{modelViewer->doBackViewAngle();}
	void leftViewAngle()			{modelViewer->doLeftViewAngle();}
	void rightViewAngle()			{modelViewer->doRightViewAngle();}
	void topViewAngle()				{modelViewer->doTopViewAngle();}
	void bottomViewAngle()			{modelViewer->doBottomViewAngle();}
	void latLongViewAngle()			{modelViewer->doLatLongViewAngle();}
	void isoViewAngle()				{modelViewer->doIsoViewAngle();}
	void saveDefaultViewAngle()		{modelViewer->doSaveDefaultViewAngle();}
	void fileCancelLoad()			{modelViewer->doFileCancelLoad();}
	void showViewInfo()				{modelViewer->doShowViewInfo();}
	void showPovCamera()			{modelViewer->doShowPovCamera();}
	void showPovAspectRatio(bool flag){	modelViewer->doShowPovAspectRatio(flag);}
	void toolsPartList()			{modelViewer->doPartList();}
	void toolsModelTree()			{modelViewer->doModelTree();}
	void toolsBoundingBox()			{modelViewer->doBoundingBox();}
	void toolsMpdModelSelection()	{modelViewer->doMpdModel();}
	void toolsStatitics()			{modelViewer->doStatistics();}
	void latitudeRotation(bool b)	{modelViewer->switchExamineLatLong(b);}
	void keepRightSideUp(bool b)	{modelViewer->keepRightSideUp(b);}
	void prevStep()					{modelViewer->prevStep();}
	void nextStep()					{modelViewer->nextStep();}
	void firstStep()				{modelViewer->firstStep();}
	void lastStep()					{modelViewer->lastStep();}
	void gotoStep()					{modelViewer->gotoStep();}
	void doFileMenuAboutToShow(void);
	void doEditMenuAboutToShow(void);
	void doViewMenuAboutToShow(void);
	void doToolsMenuAboutToShow(void);
	void doHelpMenuAboutToShow(void);
	void doRecentFile();
	void standardSizeSelected();

private:
	LDrawModelViewer::StandardSizeVector standardSizes;
#ifdef __APPLE__
	QMenu *openRecentMenu;
#endif // __APPLE__
	int fileSeparatorIndex;
	QLabel *toolbarMaxStep, *toolbarCurrentStep, *toolbarStepLabel;
	QToolButton *toolbarViewAngle;
	QMenu *toolbarWireframeMenu,*edgeMenu, *bfcMenu, *primitiveMenu;
};

#endif
