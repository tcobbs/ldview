/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include "qapplication.h"

void LDView::init()
{
    if (modelViewer)
    {
	modelViewer->setMainWindow(this);
    }
}

void LDView::fileOpen()
{
    modelViewer->doFileOpen();
}

void LDView::fileSave()
{
}

void LDView::filePrint()
{
}

void LDView::fileExit()
{
    QApplication::exit();
}

void LDView::helpContents()
{
    modelViewer->doHelpContents();
}

void LDView::helpAbout()
{
    modelViewer->doHelpAbout();
}

void LDView::editPreferences()
{
    modelViewer->showPreferences();
}

void LDView::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if (event->isAccepted())
    {
	fileExit();
    }
}

void LDView::viewStatusBar(bool flag)
{
    modelViewer->doViewStatusBar(flag);
}


void LDView::viewResetView()
{
    modelViewer->doViewReset();
}


void LDView::helpOpenGLDriverInfo()
{
    modelViewer->doHelpOpenGLDriverInfo();
}

void LDView::fileLDrawDir()
{
    modelViewer->doFileLDrawDir();
}

void LDView::fileReload()
{
    modelViewer->doFileReload();
}

void LDView::viewShowErrors()
{
    modelViewer->doViewErrors();
}

void LDView::pollChanged(QAction *action)
{
    modelViewer->doPollChanged(action);
}

void LDView::viewModeChanged(QAction *action)
{
    modelViewer->doViewModeChanged(action);
}

void LDView::fileCancelLoad()
{
    modelViewer->doFileCancelLoad();
}
