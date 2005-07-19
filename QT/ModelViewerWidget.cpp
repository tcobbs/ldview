#include "qt4wrapper.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCMacros.h>
#include <LDLoader/LDLError.h>
#include <LDLoader/LDLModel.h>
#include <LDLib/LDrawModelViewer.h>
//#include <LDLib/ModelMacros.h>
#include <TRE/TREMainModel.h>
#include "OpenGLExtensionsPanel.h"
#include "AboutPanel.h"
#include "HelpPanel.h"
#include "LDView.h"
#include "LDViewErrors.h"
#include "ExtraDirPanel.h"
#include "LDViewExtraDir.h"
#include <TCFoundation/TCUserDefaults.h>
#include "UserDefaultsKeys.h"

#include "ModelViewerWidget.h"

#include <qtextbrowser.h>
#include <qapplication.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qevent.h>
#include <qframe.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qdatetime.h>
#include <qmime.h>
#include <qregexp.h>
#include <qgroupbox.h>
#include <qlayout.h>

#define POLL_INTERVAL 500

#define PNG_IMAGE_TYPE_INDEX 1
#define BMP_IMAGE_TYPE_INDEX 2

TCStringArray *ModelViewerWidget::recentFiles = NULL;

ModelViewerWidget::ModelViewerWidget(QWidget *parent, const char *name)
	:QGLWidget(parent, name),
	modelViewer(new LDrawModelViewer(100, 100)),
	lastX(-1),
	lastY(-1),
	originalZoomY(-1),
	rotationSpeed(0.0f),
	viewMode(LDVViewExamine),
	spinButton(1),
	zoomButton(2),
	preferences(NULL),
	extradir(NULL),
	extensionsPanel(NULL),
	openGLDriverInfo(NULL),
	aboutPanel(NULL),
	helpContents(NULL),
	mainWindow(NULL),
	menuBar(NULL),
	fileMenu(NULL),
	editMenu(NULL),
	viewMenu(NULL),
	helpMenu(NULL),
	fileSeparatorIndex(-1),
	fileCancelLoadId(-1),
	fileReloadId(-1),
	fileSaveSnapshotId(-1),
	statusBar(NULL),
	toolBar(NULL),
	progressBar(NULL),
	progressLabel(NULL),
	progressMode(NULL),
	loading(false),
	cancelLoad(false),
	app(NULL),
	painting(false),
	fps(-1.0),
	numFramesSinceReference(0),
	paintTimer(0),
	pollTimer(0),
	loadTimer(0),
	fileDialog(NULL),
	saveDialog(NULL),
	errors(NULL),
	fileInfo(NULL),
	lockCount(0),
	fullscreen(0)
{
	int i;
	const QMimeSource *mimeSource =
		QMimeSourceFactory::defaultFactory()->data("StudLogo.png");

	LDLModel::setFileCaseCallback(staticFileCaseCallback);
	if (mimeSource)
	{
		QImage studImage;

		QImageDrag::decode(mimeSource, studImage);
		TREMainModel::setStudTextureData(studImage.bits(),
			studImage.numBytes());
	}
	TCAlertManager::registerHandler(LDLError::alertClass(), this,
		(TCAlertCallback)&ModelViewerWidget::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
		(TCAlertCallback)&ModelViewerWidget::progressAlertCallback);

//	modelViewer->setProgressCallback(staticProgressCallback, this);
//	modelViewer->setErrorCallback(staticErrorCallback, this);
	for (i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		mouseButtonsDown[i] = false;
	}
	preferences = new Preferences(this);
	extradir = new ExtraDir(this);
	preferences->doApply();
	setViewMode(Preferences::getViewMode());
#if (QT_VERSION >>16)==4
	setFocusPolicy(Qt::StrongFocus);
#else
	setFocusPolicy(QWidget::StrongFocus);
#endif
}

ModelViewerWidget::~ModelViewerWidget(void)
{
	if (modelViewer)
	{
		modelViewer->release();
	}
	delete preferences;
	delete extensionsPanel;
	delete openGLDriverInfo;
	delete errors;
}

void ModelViewerWidget::setApplication(QApplication *value)
{
	char *arg0;

	app = value;
	arg0 = copyString(app->argv()[0]);
	if (strrchr(arg0, '/'))
	{
		*strrchr(arg0, '/') = 0;
	}
	modelViewer->setProgramPath(arg0);
	delete arg0;
}

/*
QSize ModelViewerWidget::sizeHint(void)
{
	if (preferences)
	{
		int width = preferences->getWindowWidth();
		int height = preferences->getWindowHeight();

		return QSize(width, height);
	}
	else
	{
		return QSize();
	}
}
*/

void ModelViewerWidget::initializeGL(void)
{
	lock();
	doViewStatusBar(preferences->getStatusBar());
	doViewToolBar(preferences->getToolBar());
	unlock();
}

void ModelViewerWidget::resizeGL(int width, int height)
{
	lock();
	if (!loading)
	{
		QSize mainWindowSize = mainWindow->size();

		modelViewer->setWidth(width);
		modelViewer->setHeight(height);
		glViewport(0, 0, width, height);
		preferences->setWindowSize(mainWindowSize.width(), mainWindowSize.height());
	}
	unlock();
}

void ModelViewerWidget::updateSpinRate(void)
{
	// If the mouse stops for more than a set time, stop the spinning.
	if (mouseButtonsDown[spinButton])
	{
		if (lastMoveTime.elapsed() >= 100)
		{
			updateSpinRateXY(lastX, lastY);
		}
	}
}

void ModelViewerWidget::swap_Buffers(void)
{
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
    glDisable(GL_MULTISAMPLE_ARB);
    glDrawBuffer(GL_FRONT);
//    drawFPS();
    glDrawBuffer(GL_BACK);
    glFlush();
    glEnable(GL_MULTISAMPLE_ARB);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
}

void ModelViewerWidget::paintGL(void)
{
	lock();
	if (!painting && !loading)
	{
		painting = true;
		makeCurrent();
		updateSpinRate();
		modelViewer->update();
		updateFPS();
		if ((fEq(rotationSpeed, 0.0f) && fEq(modelViewer->getZoomSpeed(), 0.0f)
			&& fEq(modelViewer->getCameraXRotate(), 0.0f)
			&& fEq(modelViewer->getCameraYRotate(), 0.0f)
			&& fEq(modelViewer->getCameraZRotate(), 0.0f)
			&& fEq(modelViewer->getCameraMotion().length(), 0.0f))
			|| modelViewer->getPaused())
		{
			killPaintTimer();
			fps = -1.0f;
		}
		else
		{
			startPaintTimer();
		}
		swap_Buffers();
		painting = false;
	}
	unlock();
}

void ModelViewerWidget::lock(void)
{
	if (lockCount == 0)
	{
		app->lock();
	}
	lockCount++;
}

void ModelViewerWidget::unlock(void)
{
	lockCount--;
	if (lockCount == 0)
	{
		app->unlock();
	}
}

void ModelViewerWidget::timerEvent(QTimerEvent* event)
{
	lock();
	if (event->timerId() == paintTimer)
	{
		if (!loading)
		{
			updateGL();
		}
	}
	else if (event->timerId() == pollTimer)
	{
		if (!loading)
		{
			checkFileForUpdates();
		}
	}
	else if (event->timerId() == loadTimer)
	{
		killLoadTimer();
		finishLoadModel();
	}
	unlock();
}

void ModelViewerWidget::paintEvent(QPaintEvent *event)
{
	lock();
	if (loading)
	{
		int r, g, b;

		preferences->getRGB(preferences->getBackgroundColor(), r, g, b);
#if QT_VERSION < 0x40000
		QPainter painter(this);
		painter.fillRect(event->rect(), QColor(r, g, b));
#endif
	}
	else
	{
		QGLWidget::paintEvent(event);
	}
	unlock();
}

void ModelViewerWidget::preLoad(void)
{
	clearErrors();
	makeCurrent();
	modelViewer->clearBackground();
	glDraw();
}

void ModelViewerWidget::postLoad(void)
{
	makeCurrent();
	resizeGL(width(), height());
	startPaintTimer();
    mainWindow->fileSaveAction->setEnabled(true);
    mainWindow->fileReloadAction->setEnabled(true);

}

void ModelViewerWidget::doFileReload(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	preLoad();
	modelViewer->reload();
	postLoad();
	unlock();
}

bool ModelViewerWidget::chDirFromFilename(const char *filename)
{
	char *fileSpot = strrchr(filename, '/');
	bool retValue = false;

	if (fileSpot)
	{
		int len = fileSpot - filename;
		char *path = new char[len + 1];

		strncpy(path, filename, len);
		path[len] = 0;
		retValue = QDir::setCurrent(path);
		if (retValue)
		{
			Preferences::setLastOpenPath(path);
		}
		delete path;
	}
	return retValue;
}

void ModelViewerWidget::finishLoadModel(void)
{
	char *filename = modelViewer->getFilename();

	preLoad();
	if (modelViewer->loadModel())
	{
		getFileTime(filename, lastWriteTime);
		if (lastWriteTime.isValid())
		{
			startPollTimer();
		}
		setLastOpenFile(filename);
		populateRecentFileMenuItems();
	}
	postLoad();
}

void ModelViewerWidget::loadModel(const char *filename)
{
	if (chDirFromFilename(filename))
	{
		killPollTimer();
		modelViewer->setFilename(filename);
		// I'm getting occasional crashes, so schedule load to happen RSN.
		startLoadTimer();
	}
	else
	{
		QString message;

		message.sprintf("The directory containing the file %s could not be found.",
			filename);
		QMessageBox::warning(this, "Can't find directory", message, QMessageBox::Ok,
			QMessageBox::NoButton);
	}
}

void ModelViewerWidget::doFileOpen(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if (verifyLDrawDir())
	{
		char *initialDir = Preferences::getLastOpenPath();

		QDir::setCurrent(initialDir);
		if (!fileDialog)
		{
			fileDialog = new QFileDialog(".",
				"All LDraw Files (*.ldr *.dat *.mpd)",
				this,
				"open model dialog",
				true);
			fileDialog->setCaption("Choose a Model");
			fileDialog->addFilter("LDraw Models (*.ldr *.dat)");
			fileDialog->addFilter("Multi-part Models (*.mpd)");
			fileDialog->addFilter("All Files (*)");
			fileDialog->setSelectedFilter(0);
			fileDialog->setIcon(getimage("LDViewIcon16.png"));
		}
		if (fileDialog->exec() == QDialog::Accepted)
		{
			QString filename = fileDialog->selectedFile();
			QDir::setCurrent(fileDialog->dirPath());
			Preferences::setLastOpenPath(fileDialog->dirPath());
			loadModel(filename);
		}
	}
	unlock();
}

void ModelViewerWidget::setLastOpenFile(const char *filename)
{
	if (recentFiles)
	{
		int index = recentFiles->indexOfString(filename);

		recentFiles->insertString(filename);
		if (index >= 0)
		{
			// Insert before removal.  Since the one being removed could have the same
			// pointer value as the string in the array, we could otherwise access a
			// pointer after it had been deleted.
			recentFiles->removeString(index + 1);
		}
		recordRecentFiles();
	}
}

void ModelViewerWidget::mousePressEvent(QMouseEvent *event)
{
	int i;
	int button;

	lock();
	button = event->button();
	if (loading)
	{
//		event->ignore();
		unlock();
		return;
	}
	startPaintTimer();
	if (button >= MAX_MOUSE_BUTTONS)
	{
		// Don't even try to handle if the button number it too high.
		event->ignore();
		unlock();
		return;
	}
	for (i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		if (mouseButtonsDown[i])
		{
			// Don't try to handle multiple presses at once.
			unlock();
			return;
		}
	}
	mouseButtonsDown[button] = true;
	if (button == spinButton)
	{
		spinButtonPress(event);
	}
	else if (button == zoomButton)
	{
		zoomButtonPress(event);
	}
	unlock();
}

void ModelViewerWidget::spinButtonPress(QMouseEvent *event)
{
	lastX = event->globalX();
	lastY = event->globalY();
	if (viewMode == LDVViewExamine)
	{
		updateSpinRateXY(lastX, lastY);
	}
}

void ModelViewerWidget::zoomButtonPress(QMouseEvent *event)
{
	if (viewMode == LDVViewExamine)
	{
		originalZoomY = event->globalY();
	}
}

void ModelViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	int button;

	lock();
	button = event->button();
	if (loading)
	{
//		event->ignore();
		unlock();
		return;
	}
	startPaintTimer();
	if (button >= MAX_MOUSE_BUTTONS)
	{
		unlock();
		return;
	}
	if (mouseButtonsDown[button])
	{
		mouseButtonsDown[button] = false;
		if (button == spinButton)
		{
			spinButtonRelease(event);
		}
		else if (button == zoomButton)
		{
			zoomButtonRelease(event);
		}
	}
	unlock();
}

void ModelViewerWidget::spinButtonRelease(QMouseEvent * /*event*/)
{
	if (viewMode != LDVViewExamine)
	{
		modelViewer->setCameraXRotate(0.0f);
		modelViewer->setCameraYRotate(0.0f);
	}
}

void ModelViewerWidget::zoomButtonRelease(QMouseEvent * /*event*/)
{
	modelViewer->setZoomSpeed(0.0f);
}

void ModelViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
	bool controlPressed;

	lock();
	controlPressed = event->state() & Qt::ControlButton;
	if (loading)
	{
//		event->ignore();
		unlock();
		return;
	}
	startPaintTimer();
	if (mouseButtonsDown[spinButton] || mouseButtonsDown[zoomButton])
	{
		lastMoveTime.start();
	}
	if (mouseButtonsDown[spinButton])
	{
		if (viewMode == LDVViewExamine)
		{
			if (controlPressed)
			{
				updatePanXY(event->globalX(), event->globalY());
			}
			else
			{
				updateSpinRateXY(event->globalX(), event->globalY());
			}
		}
		else
		{
			updateHeadXY(event->globalX(), event->globalY());
		}
	}
	if (mouseButtonsDown[zoomButton])
	{
		if (controlPressed)
		{
			modelViewer->setClipZoom(true);
		}
		else
		{
			modelViewer->setClipZoom(false);
		}
		updateZoom(event->globalY());
	}
	unlock();
}

void ModelViewerWidget::updateSpinRateXY(int xPos, int yPos)
{
	int deltaX = xPos - lastX;
	int deltaY = yPos - lastY;
	float magnitude = (float)sqrt((float)(deltaX * deltaX + deltaY * deltaY));

	lastX = xPos;
	lastY = yPos;
	rotationSpeed = magnitude / 10.0f;
	if (fEq(rotationSpeed, 0.0f))
	{
		rotationSpeed = 0.0f;
	}
	else
	{
		modelViewer->setXRotate((float)deltaY);
		modelViewer->setYRotate((float)deltaX);
	}
	modelViewer->setRotationSpeed(rotationSpeed);
}

void ModelViewerWidget::updatePanXY(int xPos, int yPos)
{
	int deltaX = xPos - lastX;
	int deltaY = yPos - lastY;

	lastX = xPos;
	lastY = yPos;
	modelViewer->panXY(deltaX, deltaY);
	rotationSpeed = 0;
	modelViewer->setRotationSpeed(0);
}

void ModelViewerWidget::updateHeadXY(int xPos, int yPos)
{
	float magnitude = (float)(xPos - lastX);
	float denom = 5000.0f;

	modelViewer->setCameraXRotate(magnitude / denom);
	magnitude = (float)(yPos - lastY);
	modelViewer->setCameraYRotate(magnitude / -denom);
}

void ModelViewerWidget::updateZoom(int yPos)
{
	float magnitude = (float)(yPos - originalZoomY);

	modelViewer->setZoomSpeed(magnitude / 2.0f);
}

void ModelViewerWidget::showPreferences(void)
{
	preferences->show();
}

void ModelViewerWidget::showFileExtraDir(void)
{
	extradir->show();
}

void ModelViewerWidget::connectMenuShows(void)
{
	connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(doFileMenuAboutToShow()));
	connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(doEditMenuAboutToShow()));
	connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(doViewMenuAboutToShow()));
	connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(doHelpMenuAboutToShow()));
}

void ModelViewerWidget::setMainWindow(LDView *value)
{
	QMenuItem *item;
	int i;
	QAction *pollAction;
	int width, height;
//	QSize windowSize;

	lock();
	mainWindow = value;
	width = preferences->getWindowWidth();
	height = preferences->getWindowHeight();
	mainWindow->resize(width, height);
/*
	windowSize = mainWindow->size();
	mainWindow->resize(width - 320 + windowSize.width(),
		height - 240 + windowSize.height());
*/
	pollAction = mainWindow->noPollingAction;
	statusBar = mainWindow->statusBar();
	toolBar = new QToolBar;
	reflectSettings();
    mainWindow->fileSaveAction->setEnabled(false);
    mainWindow->fileReloadAction->setEnabled(false);
	progressBar = new QProgressBar(statusBar);
	progressLabel = new QLabel(statusBar);
	progressMode = new QLabel(statusBar);
	progressBar->setPercentageVisible(false);
	statusBar->addWidget(progressBar);
	statusBar->addWidget(progressLabel, 1);
	statusBar->addWidget(progressMode);
	mainWindow->viewStatusBarAction->setOn(preferences->getStatusBar());
	mainWindow->viewToolBarAction->setOn(preferences->getToolBar());
	switch (Preferences::getPollMode())
	{
	case LDVPollPrompt:
		pollAction = mainWindow->promptPollingAction;
		break;
	case LDVPollAuto:
		pollAction = mainWindow->autoPollingAction;
		break;
	case LDVPollBackground:
		pollAction = mainWindow->backgroundPollingAction;
		break;
	default:
		break;
	}
	pollAction->setOn(true);
	if (viewMode == LDVViewExamine)
	{
		mainWindow->examineModeAction->setOn(true);
		progressMode->setText("Examine");
	}
	else
	{
		mainWindow->flythroughModeAction->setOn(true);
		progressMode->setText("Fly-through");
	}
	menuBar = mainWindow->menuBar();
	item = menuBar->findItem(menuBar->idAt(0));
	if (item)
	{
		fileMenu = mainWindow->fileMenu;
		fileCancelLoadId = fileMenu->idAt(6);
		fileReloadId = fileMenu->idAt(1);
	}
	for (i = 0; ; i++)
	{
		item = fileMenu->findItem(fileMenu->idAt(i));
		if (item->isSeparator())
		{
			break;
		}
	}
	fileSeparatorIndex = i;
	if (!recentFiles)
	{
		recentFiles = new TCStringArray(10);
		populateRecentFiles();
	}
	populateRecentFileMenuItems();
	item = menuBar->findItem(menuBar->idAt(1));
	if (item)
	{
		editMenu = mainWindow->editMenu;
	}
	item = menuBar->findItem(menuBar->idAt(2));
	if (item)
	{
		viewMenu = mainWindow->viewMenu;
	}
	item = menuBar->findItem(menuBar->idAt(3));
	if (item)
	{
		helpMenu = mainWindow->helpMenu;
	}
	connectMenuShows();
	unlock();
}

void ModelViewerWidget::populateRecentFiles(void)
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

void ModelViewerWidget::recordRecentFiles(void)
{
	int i;
	long maxRecentFiles = Preferences::getMaxRecentFiles();

	for (i = 1; i <= maxRecentFiles; i++)
	{
		char *filename = recentFiles->stringAtIndex(i - 1);

		Preferences::setRecentFile(i, filename);
	}
}

void ModelViewerWidget::clearRecentFileMenuItems(void)
{
	QMenuItem *item;
	int index = fileSeparatorIndex + 1;
	int i;
	int count = fileMenu->count();

	for (i = index; i < count - 1; i++)
	{
		item = fileMenu->findItem(fileMenu->idAt(index));
		fileMenu->removeItemAt(index);
	}
}

char *ModelViewerWidget::truncateFilename(const char *filename)
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

void ModelViewerWidget::populateRecentFileMenuItems(void)
{
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
				int id = fileMenu->insertItem(filename, this, SLOT(doRecentFile(int)),
					0, -1, fileSeparatorIndex + i + 1);

				fileMenu->setItemParameter(id, i);
				delete filename;
			}
		}
		fileMenu->insertSeparator(fileMenu->count() - 1);
	}
}

void ModelViewerWidget::doRecentFile(int index)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if (verifyLDrawDir())
	{
		char *filename = recentFiles->stringAtIndex(index);

		if (filename)
		{
			loadModel(filename);
		}
	}
	unlock();
}

/*
int ModelViewerWidget::staticProgressCallback(char *message, float progress,
	void *userData)
{
	return ((ModelViewerWidget *)userData)->progressCallback(message, progress,
		true);
}
*/

void ModelViewerWidget::setupProgress(void)
{
	clearErrors();
	lastProgressTime.start();
	loading = true;
	cancelLoad = false;
}

void ModelViewerWidget::endLoad(void)
{
	progressBar->setProgress(0);
	progressLabel->setText("");
	loading = false;
}

int ModelViewerWidget::progressCallback(char *message, float progress,
	bool /*showErrors*/)
{
	if (progress == 2.0f)
	{
		showErrorsIfNeeded(true);
		endLoad();
		makeCurrent();
		return 1;
	}
	if (!loading)
	{
		setupProgress();
	}
	if (message)
	{
		progressLabel->setText(message);
	}
	if (progress >= 0.0f)
	{
		progressBar->setProgress((int)(progress * 100));
	}
	if (lastProgressTime.elapsed() >= 100 || progress == 1.0f)
	{
		app->processEvents();
		lastProgressTime.start();
		makeCurrent();
	}
	if (cancelLoad)
	{
		endLoad();
		makeCurrent();
		return 0;
	}
	return 1;
}

void ModelViewerWidget::doViewStatusBar(bool flag)
{
	lock();
	if (flag)
	{
		statusBar->show();
	}
	else
	{
		statusBar->hide();
	}
	preferences->setStatusBar(flag);
	unlock();
}

void ModelViewerWidget::doViewToolBar(bool flag)
{
    lock();
    if (flag)
    {
		mainWindow->dockWindows().at(0)->show();
    }
    else
    {
        mainWindow->dockWindows().at(0)->hide();
	}
	preferences->setToolBar(flag);
	unlock();
}

void ModelViewerWidget::doViewFullScreen(void)
{
	static QPoint pos;
	static QSize size;
	if (!fullscreen)
	{
		pos=mainWindow->pos();
		size=mainWindow->size();
		menuBar->hide();
		statusBar->hide();
#if (QT_VERSION >>16)==3
		mainWindow->GroupBox12->setFrameShape( QFrame::NoFrame );
#endif
		mainWindow->GroupBox12->layout()->setMargin( 0 );
		mainWindow->dockWindows().at(0)->hide();
		mainWindow->showFullScreen();
		fullscreen=1;
	} else
	{
#if (QT_VERSION >>16)==3
		mainWindow->GroupBox12->setFrameShape( QGroupBox::WinPanel );
#endif
        mainWindow->GroupBox12->layout()->setMargin( 2 );
        mainWindow->showNormal();
		mainWindow->resize(size);
		mainWindow->move(pos);
        menuBar->show();
        if(preferences->getStatusBar()) {statusBar->show();}
		if(preferences->getToolBar()) {mainWindow->dockWindows().at(0)->show();}
		fullscreen=0;
	}
}

void ModelViewerWidget::doViewReset(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	rotationSpeed = 0.0f;
	modelViewer->setRotationSpeed(0.0f);
	modelViewer->setZoomSpeed(0.0f);
	modelViewer->resetView();
	startPaintTimer();
	unlock();
}

void ModelViewerWidget::doHelpOpenGLDriverInfo(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if (!extensionsPanel)
	{
		QLabel *extensionsCountLabel;
		QString countString;
		int extensionCount;

		extensionsPanel = new OpenGLExtensionsPanel;
		openGLDriverInfo = LDrawModelViewer::getOpenGLDriverInfo(
			extensionCount);
		extensionsPanel->extensionsBox->setText(openGLDriverInfo);
		extensionsCountLabel = new QLabel(extensionsPanel->statusBar());
		countString.sprintf("%d Extensions", extensionCount);
		extensionsCountLabel->setText(countString);
		extensionsPanel->statusBar()->addWidget(extensionsCountLabel, 1);
	}
	extensionsPanel->show();
	extensionsPanel->raise();
	extensionsPanel->setActiveWindow();
	unlock();
}

void ModelViewerWidget::doHelpContents(void)
{
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if(!helpContents)
	{
		QString dir = QDir::currentDirPath();
        QFile file( "Help.html" );
		if (!file.exists()) QDir::setCurrent("..");
		if (!file.exists()) QDir::setCurrent("/usr/local/share/ldview");
		if (!file.exists()) 
		{
			QDir::setCurrent(dir);
			return;
		}	
		helpContents = new HelpPanel;
        if ( file.open( IO_ReadOnly ) ) {
            QTextStream stream( &file );
            helpContents->HelpTextBrowser->setText(
				stream.read().replace(QRegExp("(BGCOLOR|COLOR|TEXT|LINK)="),
												"i=") );
		    helpContents->HelpTextBrowser->setReadOnly(TRUE);
        }
		QDir::setCurrent(dir);
	}
	helpContents->show();
}

void ModelViewerWidget::doHelpAbout(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if (!aboutPanel)
	{
		aboutPanel = new AboutPanel;
		connect(aboutPanel->okButton, SIGNAL(clicked()), this,
			SLOT(doAboutOK()));
		aboutPanel->resize(10, 10);
	}
	aboutPanel->show();
	unlock();
}

void ModelViewerWidget::doHelpAboutQt(void)
{
	lock();
	QMessageBox::aboutQt(this,"");
	unlock();
}

void ModelViewerWidget::doAboutOK(void)
{
	lock();
	aboutPanel->hide();
	unlock();
}

void ModelViewerWidget::doWireframe(bool value)
{
	preferences->setDrawWireframe(value);
	doApply();
}

void ModelViewerWidget::doEdge(bool value)
{
    preferences->setShowsHighlightLines(value);
    doApply();
}

void ModelViewerWidget::doLighting(bool value)
{
    preferences->setUseLighting(value);
    doApply();
}

void ModelViewerWidget::doPrimitiveSubstitution(bool value)
{
    preferences->setAllowPrimitiveSubstitution(value);
	doApply();
}

void ModelViewerWidget::doSeams(bool value)
{
	preferences->setUseSeams(value);
	doApply();
}

void ModelViewerWidget::reflectSettings(void)
{
    if (mainWindow && mainWindow->toolbarWireframeAction && preferences)
    {
        mainWindow->toolbarWireframeAction->setOn(preferences->getDrawWireframe());
		mainWindow->toolbarEdgeAction->setOn(preferences->getShowsHighlightLines());
		mainWindow->toolbarLightingAction->setOn(preferences->getUseLighting());
		mainWindow->toolbarSeamsAction->setOn(preferences->getUseSeams());
		mainWindow->toolbarPrimitiveSubstitutionAction->setOn(preferences->getAllowPrimitiveSubstitution());
    }
}

void ModelViewerWidget::updateFPS(void)
{
	numFramesSinceReference++;
	if (fps == -1.0f)
	{
		referenceFrameTime.start();
		numFramesSinceReference = 0;
		fps = 0.0f;
	}
	else
	{
		long elapsed = referenceFrameTime.elapsed();

		if (elapsed >= 250)
		{
			fps = 1000.0f / (float)elapsed * numFramesSinceReference;
			referenceFrameTime.start();
			numFramesSinceReference = 0;
		}
	}
	drawFPS();
}

void ModelViewerWidget::drawFPS(void)
{
	if (showFPS && modelViewer->getMainTREModel())
	{
		if (statusBar->isHidden())
		{
			modelViewer->drawFPS(fps);
		}
		else
		{
			QString fpsString;

			if (fps > 0.0f)
			{
				fpsString.sprintf("FPS: %4.4f", fps);
			}
			else
			{
				fpsString = "Spin Model for FPS";
			}
			progressLabel->setText(fpsString);
		}
	}
}

void ModelViewerWidget::startPaintTimer(void)
{
	if (!paintTimer)
	{
		paintTimer = startTimer(0);
	}
}

void ModelViewerWidget::killPaintTimer(void)
{
	if (paintTimer)
	{
		killTimer(paintTimer);
		paintTimer = 0;
	}
}

void ModelViewerWidget::startPollTimer(bool immediate)
{
	if (!pollTimer && Preferences::getPollMode() != LDVPollNone)
	{
		pollTimer = startTimer(POLL_INTERVAL);
		if (!loading && immediate)
		{
			checkFileForUpdates();
		}
	}
}

void ModelViewerWidget::killPollTimer(void)
{
	if (pollTimer)
	{
		killTimer(pollTimer);
		pollTimer = 0;
	}
}

void ModelViewerWidget::startLoadTimer(void)
{
	if (!loadTimer)
	{
		loadTimer = startTimer(0);
	}
}

void ModelViewerWidget::killLoadTimer(void)
{
	if (loadTimer)
	{
		killTimer(loadTimer);
		loadTimer = 0;
	}
}

void ModelViewerWidget::doApply(void)
{
	lock();
	startPaintTimer();
	unlock();
}

bool ModelViewerWidget::verifyLDrawDir(char *value)
{
	QString currentDir = QDir::currentDirPath();
	bool found = false;
	char buf[128];

	if (QDir::setCurrent(value))
	{
		strcpy(buf, "parts");
		if (staticFileCaseCallback(buf) && QDir::current().cd(buf))
		{
			QDir::setCurrent(value);
			strcpy(buf, "p");
			if (staticFileCaseCallback(buf) && QDir::current().cd(buf))
			{
				LDLModel::setLDrawDir(value);
				found = true;
			}
		}
		QDir::setCurrent(currentDir);
	}
	return found;
}

bool ModelViewerWidget::verifyLDrawDir(bool forceChoose)
{
	char *lDrawDir = getLDrawDir();
	bool found = false;
	
	if (!forceChoose && verifyLDrawDir(lDrawDir))
	{
		delete lDrawDir;
		found = true;
	}
	else
	{
		delete lDrawDir;
		if (!forceChoose)
		{
			QMessageBox::warning(this, "Can't find LDraw", "The LDraw directory has "
				"not yet been chosen, or it has moved.  Please select the directory "
				"that contains LDraw.", QMessageBox::Ok, QMessageBox::NoButton);
		}
		while (!found)
		{
			if (promptForLDrawDir())
			{
				lDrawDir = getLDrawDir();
				if (verifyLDrawDir(lDrawDir))
				{
					found = true;
				}
				else
				{
					QMessageBox::warning(this, "Invalid directory",
						"The directory you selected does not contain LDraw.",
						QMessageBox::Ok, QMessageBox::NoButton);
				}
				delete lDrawDir;
			}
			else
			{
				break;
			}
		}
	}
	return found;
}

char *ModelViewerWidget::getLDrawDir(void)
{
	char *lDrawDir = Preferences::getLDrawDir();

	if (!lDrawDir)
	{
		lDrawDir = copyString(getenv("LDRAWDIR"));
		if (!lDrawDir)
		{
			lDrawDir = copyString("/mnt/c/ldraw");
		}
	}
	stripTrailingPathSeparators(lDrawDir);
	return lDrawDir;
}

bool ModelViewerWidget::promptForLDrawDir(void)
{
	char *initialDir = getLDrawDir();
	QFileDialog *dirDialog;
	bool retValue = false;

	QDir::setCurrent(initialDir);
	dirDialog = new QFileDialog(".",
		"Directories",
		this,
		"open LDraw dir dialog",
		true);
	dirDialog->setCaption("Choose the LDraw directory");
	dirDialog->setIcon(getimage("LDViewIcon16.png"));
	dirDialog->setMode(QFileDialog::DirectoryOnly);
	if (dirDialog->exec() == QDialog::Accepted)
	{
		QString chosenDir = dirDialog->selectedFile();
		Preferences::setLDrawDir(chosenDir);
		retValue = true;
	}
	delete dirDialog;
	return retValue;
/*
	QString chosenDir = QFileDialog::getExistingDirectory(initialDir, this,
		"get LDraw dir", "Choose the LDraw directory");

	delete initialDir;
	if (chosenDir.isNull())
	{
		return false;
	}
	else
	{
		Preferences::setLDrawDir(chosenDir);
		return true;
	}
*/
}

void ModelViewerWidget::doFileLDrawDir(void)
{
	char *oldDir;

	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	oldDir = getLDrawDir();
	if (!verifyLDrawDir(true))
	{
		if (oldDir)
		{
			Preferences::setLDrawDir(oldDir);
		}
	}
	delete oldDir;
	unlock();
}

void ModelViewerWidget::doFileCancelLoad(void)
{
	lock();
	cancelLoad = true;
	unlock();
}

int ModelViewerWidget::errorCallback(LDLError* error)
{
	if (!errors)
	{
		errors = new LDViewErrors(preferences);
	}
	errors->addError(error);
	return 1;
}

/*
int ModelViewerWidget::staticErrorCallback(LDLError* error, void* userData)
{
	return ((ModelViewerWidget*)userData)->errorCallback(error);
}
*/

void ModelViewerWidget::clearErrors(void)
{
	if (errors)
	{
		errors->clear();
	}
}

void ModelViewerWidget::showErrorsIfNeeded(bool onlyIfNeeded)
{
	if (errors)
	{
		int errorCount = errors->populateListView();

		if (!onlyIfNeeded || (errorCount && preferences->getShowErrors()))
		{
			errors->show();
		}
	}
}

void ModelViewerWidget::doViewErrors(void)
{
	lock();
	if (loading)
	{
		if (app)
		{
			app->beep();
		}
		return;
	}
	if (!errors)
	{
		errors = new LDViewErrors(preferences);
	}
	showErrorsIfNeeded(false);
	unlock();
}

void ModelViewerWidget::getFileTime(const char *filename, QDateTime &value)
{
	if (filename)
	{
		if (!fileInfo)
		{
			fileInfo = new QFileInfo;
		}
		fileInfo->setFile(filename);
		
		value = QDateTime(fileInfo->lastModified());
	}
	else
	{
		value = QDateTime();
	}
}

void ModelViewerWidget::checkFileForUpdates(void)
{
	if (pollTimer && modelViewer)
	{
		char *filename = modelViewer->getFilename();

		killPollTimer();
		if (filename)
		{
			QDateTime newWriteTime;

			getFileTime(filename, newWriteTime);
			if (newWriteTime != lastWriteTime)
			{
				bool update = true;

				lastWriteTime = QDateTime(newWriteTime);
				if (Preferences::getPollMode() == LDVPollPrompt)
				{
					if (QMessageBox::information(this, "File updated",
						"The model has been modified.\n"
						"Do you want to reload?",
						QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
					{
						update = false;
					}
				}
				if (update)
				{
					doFileReload();
				}
			}
		}
		startPollTimer();
	}
}

void ModelViewerWidget::windowActivationChange(bool oldActive)
{
	lock();
	if (isActiveWindow())
	{
		startPollTimer(true);
	}
	else
	{
		if (Preferences::getPollMode() != LDVPollBackground)
		{
			killPollTimer();
		}
	}
	unlock();
	QGLWidget::windowActivationChange(oldActive);
}

void ModelViewerWidget::doPollChanged(QAction *action)
{
	LDVPollMode newMode = LDVPollNone;

	lock();
	if (action == mainWindow->promptPollingAction)
	{
		newMode = LDVPollPrompt;
	}
	else if (action == mainWindow->autoPollingAction)
	{
		newMode = LDVPollAuto;
	}
	else if (action == mainWindow->backgroundPollingAction)
	{
		newMode = LDVPollBackground;
	}
	Preferences::setPollMode(newMode);
	killPollTimer();
	startPollTimer(true);
	unlock();
}

void ModelViewerWidget::setViewMode(LDVViewMode value)
{
	if (value != viewMode)
	{
		viewMode = value;
		if (viewMode == LDVViewExamine)
		{
			modelViewer->setConstrainZoom(true);
			if (progressMode) {progressMode->setText("Examine");}
		}
		else
		{
			modelViewer->setConstrainZoom(false);
			if (progressMode) {progressMode->setText("Fly-through");}
		}
		Preferences::setViewMode(viewMode);
	}
}

void ModelViewerWidget::doViewModeChanged(QAction *action)
{
	LDVViewMode newMode = LDVViewExamine;

	lock();
	if (action == mainWindow->flythroughModeAction)
	{
		newMode = LDVViewFlythrough;
	}
	setViewMode(newMode);
	unlock();
}

void ModelViewerWidget::doZoomToFit(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
	modelViewer->zoomToFit();
    startPaintTimer();
    unlock();
}

bool ModelViewerWidget::staticImageProgressCallback(char* message, float progress,
                                              void* userData)
{
    return ((ModelViewerWidget*)userData)->progressCallback(message, progress, true);
}

bool ModelViewerWidget::writeImage(char *filename, int width, int height,
                             char *buffer, char *formatName)
{
    TCImage *image = new TCImage;
    bool retValue;
                                                                                                                                                             
    image->setSize(width, height);
    image->setLineAlignment(4);
    image->setImageData((TCByte*)buffer);
    image->setFormatName(formatName);
    image->setFlipped(true);
    retValue = image->saveFile(filename, staticImageProgressCallback, this);
    image->release();
    return retValue;
}

int ModelViewerWidget::roundUp(int value, int nearest)
{
    return (value + nearest - 1) / nearest * nearest;
}

char *ModelViewerWidget::grabImage(int imageWidth, int imageHeight, 
									char *buffer)
{
    bool oldSlowClear = modelViewer->getSlowClear();
    GLenum bufferFormat = GL_RGB;
    TCVector origCameraPosition = modelViewer->getCamera().getPosition();

    modelViewer->setSlowClear(true);
    makeCurrent();
    modelViewer->update();
    glReadBuffer(GL_BACK);
    if (!buffer)
    {
       buffer = (char *)malloc(roundUp(imageWidth * 3, 4) * imageHeight);
    }
    glReadPixels(0, 0, imageWidth, imageHeight, bufferFormat, GL_UNSIGNED_BYTE,
        buffer);
	modelViewer->setSlowClear(oldSlowClear);
	doApply();
    return buffer;
}

bool ModelViewerWidget::getSaveFilename(char* saveFilename, int len)
{
	char *initialDir = Preferences::getLastOpenPath();
                                                                                                                                                             
    QDir::setCurrent(initialDir);
    if (!saveDialog)
    {
        saveDialog = new QFileDialog(".",
            "Portable Network Graphics (*.png)",
            this,
            "open model dialog",
            true);
        saveDialog->setCaption("Save Snapshot");
        saveDialog->addFilter("Windows Bitmap (*.bmp)");
        saveDialog->setSelectedFilter(0);
        saveDialog->setIcon(getimage("LDViewIcon16.png"));
		saveDialog->setMode(QFileDialog::AnyFile);
    }
    if (saveDialog->exec() == QDialog::Accepted)
    {
        QString filename = saveDialog->selectedFile();
        QDir::setCurrent(saveDialog->dirPath());
        strncpy(saveFilename,filename,len);
		saveImageType = (strcmp(saveDialog->selectedFilter().ascii(),
			"Portable Network Graphics (*.png)")==0 ? PNG_IMAGE_TYPE_INDEX :
			BMP_IMAGE_TYPE_INDEX);
		if(strlen(saveFilename)>5 && saveFilename[strlen(saveFilename)-4]!='.')
		{
            if (saveImageType == PNG_IMAGE_TYPE_INDEX)
            {
                strcat(saveFilename, ".png");
            }
            else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
            {
                strcat(saveFilename, ".bmp");
            }
		}
		return true;
    }
	return false;
}

bool ModelViewerWidget::writeBmp(char *filename, int width, int height, 
								 char *buffer)
{
    return writeImage(filename, width, height, buffer, "BMP");
}
                                                                                                                                                             
bool ModelViewerWidget::writePng(char *filename, int width, int height, 
								 char *buffer)
{
    return writeImage(filename, width, height, buffer, "PNG");
}

bool ModelViewerWidget::saveImage(char *filename, int imageWidth, 
									int imageHeight)
{
    char *buffer = grabImage(imageWidth, imageHeight);
    bool retValue = false;
    if (buffer)
    {
        if (saveImageType == PNG_IMAGE_TYPE_INDEX)
        {
            retValue = writePng(filename, imageWidth, imageHeight, buffer);
        }
        else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
        {
            retValue = writeBmp(filename, imageWidth, imageHeight, buffer);
        }
       	free(buffer);
    }
    return retValue;
}

bool ModelViewerWidget::fileExists(char* filename)
{
    FILE* file = fopen(filename, "r");
                                                                                
    if (file)
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

bool ModelViewerWidget::shouldOverwriteFile(char* filename)
{
    char buf[256];
                                                                                
    sprintf(buf, "%s\nThis file already exists.\nReplace existing file?",
        filename);
	switch( QMessageBox::warning( this, "LDView",
        buf,
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape )) {
    case QMessageBox::Yes: 
		return true;
        break;
	}
		return false;
}

bool ModelViewerWidget::doFileSave(void)
{
    char saveFilename[1024] = "";

    return doFileSave(saveFilename);
}

bool ModelViewerWidget::doFileSave(char *saveFilename)
{
    if (getSaveFilename(saveFilename, 1024))
    {
		if(fileExists(saveFilename)&&!shouldOverwriteFile(saveFilename))
		{
			return false;
		}
        return saveImage(saveFilename, modelViewer->getWidth(), 
				modelViewer->getHeight());
    }
    else
    {
        return false;
    }
}

void ModelViewerWidget::doFrontViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleFront);
    startPaintTimer();
    unlock();
}

void ModelViewerWidget::doBackViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleBack);
    startPaintTimer();
    unlock();
}

void ModelViewerWidget::doLeftViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleLeft);
    startPaintTimer();
    unlock();
}
                                                                                                                                                             
void ModelViewerWidget::doRightViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleRight);
    startPaintTimer();
    unlock();
}

void ModelViewerWidget::doTopViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleTop);
    startPaintTimer();
    unlock();
}
                                                                                                                                                             
void ModelViewerWidget::doBottomViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleBottom);
    startPaintTimer();
    unlock();
}
                                                                                                                                                             
void ModelViewerWidget::doIsoViewAngle(void)
{
    lock();
    if (loading)
    {
        if (app)
        {
            app->beep();
        }
        return;
    }
    rotationSpeed = 0.0f;
    modelViewer->setRotationSpeed(0.0f);
    modelViewer->setZoomSpeed(0.0f);
    modelViewer->resetView(LDVAngleIso);
    startPaintTimer();
    unlock();
}
                                                                                                                                                             
void ModelViewerWidget::doSaveDefaultViewAngle(void)
{
    float matrix[16];
    float rotationMatrix[16];
    float otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
    char matrixString[1024];
                                                                                                                                                             
    memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
        sizeof(rotationMatrix));
    TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    sprintf(matrixString, "%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g",
        matrix[0], matrix[4], matrix[8],
        matrix[1], matrix[5], matrix[9],
        matrix[2], matrix[6], matrix[10]);
    TCUserDefaults::setStringForKey(matrixString, DEFAULT_MATRIX_KEY);
    modelViewer->setDefaultRotationMatrix(matrix);
}

void cleanupMatrix(float *matrix)
{
    int i;
                                                                                
    for (i = 0; i < 16; i++)
    {
        if (fabs(matrix[i]) < 1e-6)
        {
            matrix[i] = 0.0f;
        }
    }
}

void ModelViewerWidget::doShowViewInfo(void)
{
	QString qmessage;
    if (modelViewer)
	{
        float matrix[16];
        float rotationMatrix[16];
        float otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
        char matrixString[1024];
        char zoomString[128];
        char message[4096];
        TRECamera &camera = modelViewer->getCamera();
        float defaultDistance = modelViewer->getDefaultDistance();
        float distanceMultiplier = modelViewer->getDistanceMultiplier();
        float cameraDistance;
                                                                                
        memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
            sizeof(rotationMatrix));
        TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
        cleanupMatrix(matrix);
        sprintf(matrixString,
            "%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g", matrix[0],
            matrix[4], matrix[8], matrix[1], matrix[5], matrix[9],
            matrix[2], matrix[6], matrix[10]);
        cameraDistance = camera.getPosition().length();
        if (distanceMultiplier == 0.0f || cameraDistance == 0.0f)
        {
            // If we don't have a model, we don't know the default zoom, so
            // just say 1.
            strcpy(zoomString, "1");
        }
        else
        {
            sprintf(zoomString, "%.6g", defaultDistance /
               distanceMultiplier / cameraDistance);
        }
        sprintf(message, "The following is the current rotation matrix:\n\n"                "%s\n\nThe following is the current zoom level:\n\n"
            "%s\n\n",
            matrixString, zoomString);
 


    qmessage.sprintf("%s",message);

	QMessageBox::information(this, "View Info", qmessage, QMessageBox::Ok,
            QMessageBox::NoButton);
	}
}
                                                                                                                                                             
void ModelViewerWidget::processKey(QKeyEvent *event, bool press)
{
	TCVector cameraMotion = modelViewer->getCameraMotion();
	float originalMotionAmount = 1.0f;
	float originalRotationAmount = 0.01f;
	float motionAmount;
	float rotationAmount;
	int i;

	event->accept();
	if (press)
	{
		bool shift = false;

		motionAmount = originalMotionAmount;
		rotationAmount = originalRotationAmount;
		if (event->state() & Qt::ShiftButton)
		{
			shift = true;
		}
		else
		{
			QString text = event->text();
			
			if (text.length() > 0)
			{
				QChar qChar = text.at(0);
				ushort unicharacter = qChar.unicode();
				
				if (unicharacter < 128)
				{
					char character = qChar.latin1();

					if (character >= 'A' && character <= 'Z')
					{
						shift = true;
					}
				}
			}
		}
		if (shift)
		{
			motionAmount *= 2.0f;
			rotationAmount *= 2.0f;
		}
	}
	else
	{
		motionAmount = 0.0f;
		rotationAmount = 0.0f;
	}
	switch (event->key())
	{
	case Qt::Key_W:
		cameraMotion[2] = -motionAmount;
		break;
	case Qt::Key_S:
		cameraMotion[2] = motionAmount;
		break;
	case Qt::Key_A:
		cameraMotion[0] = -motionAmount;
		break;
	case Qt::Key_D:
		cameraMotion[0] = motionAmount;
		break;
	case Qt::Key_R:
		cameraMotion[1] = motionAmount;
		break;
	case Qt::Key_F:
		cameraMotion[1] = -motionAmount;
		break;
	case Qt::Key_E:
		modelViewer->setCameraZRotate(rotationAmount);
		break;
	case Qt::Key_Q:
		modelViewer->setCameraZRotate(-rotationAmount);
		break;
	case Qt::Key_Shift:
		if (press)
		{
			motionAmount = originalMotionAmount * 2.0f;
			rotationAmount = originalRotationAmount * 2.0f;
		}
		else
		{
			motionAmount = originalMotionAmount;
			rotationAmount = originalRotationAmount;
		}
		for (i = 0; i < 3; i++)
		{
			if (cameraMotion[i] > 0.0f)
			{
				cameraMotion[i] = motionAmount;
			}
			else if (cameraMotion[i] < 0.0f)
			{
				cameraMotion[i] = -motionAmount;
			}
		}
		if (modelViewer->getCameraZRotate() > 0.0f)
		{
			modelViewer->setCameraZRotate(rotationAmount);
		}
		else if (modelViewer->getCameraZRotate() < 0.0f)
		{
			modelViewer->setCameraZRotate(-rotationAmount);
		}
		break;
	default:
		event->ignore();
		break;
	}
	if (event->isAccepted())
	{
		modelViewer->setCameraMotion(cameraMotion);
		startPaintTimer();
	}
}

void ModelViewerWidget::keyPressEvent(QKeyEvent *event)
{
	lock();
	if((event->state() & Qt::AltButton) &&
		(event->key() >= Qt::Key_0) &&
		(event->key() <= Qt::Key_9) && preferences)
	{
		int i = event->key()-Qt::Key_0;
		preferences->performHotKey(i);
	}
	if (viewMode == LDVViewFlythrough)
	{
		processKey(event, true);
	}
	unlock();
	QGLWidget::keyPressEvent(event);
}

void ModelViewerWidget::keyReleaseEvent(QKeyEvent *event)
{
	lock();
	if(fullscreen && (event->key()==Qt::Key_Escape))
	{
		event->accept();
		doViewFullScreen();
	}
	if (viewMode == LDVViewFlythrough)
	{
		processKey(event, false);
	}
	unlock();
	QGLWidget::keyReleaseEvent(event);
}

void ModelViewerWidget::setMenuItemsEnabled(QPopupMenu *menu, bool enabled)
{
	int count = menu->count();
	int i;

	for (i = 0; i < count; i++)
	{
		menu->setItemEnabled(menu->idAt(i), enabled);
	}
}

void ModelViewerWidget::doFileMenuAboutToShow(void)
{
	if (loading)
	{
		setMenuItemsEnabled(fileMenu, false);
		fileMenu->setItemEnabled(fileCancelLoadId, true);
	}
	else
	{
		setMenuItemsEnabled(fileMenu, true);
		fileMenu->setItemEnabled(fileCancelLoadId, false);
		if (!modelViewer || !modelViewer->getMainTREModel())
		{
			fileMenu->setItemEnabled(fileReloadId, false);
			fileMenu->setItemEnabled(fileSaveSnapshotId, false);
		}
	}
}

void ModelViewerWidget::doEditMenuAboutToShow(void)
{
	if (loading)
	{
		setMenuItemsEnabled(editMenu, false);
	}
	else
	{
		setMenuItemsEnabled(editMenu, true);
	}
}

void ModelViewerWidget::doViewMenuAboutToShow(void)
{
	if (loading)
	{
		setMenuItemsEnabled(viewMenu, false);
	}
	else
	{
		setMenuItemsEnabled(viewMenu, true);
	}
}

void ModelViewerWidget::doHelpMenuAboutToShow(void)
{
	if (loading)
	{
		setMenuItemsEnabled(helpMenu, false);
	}
	else
	{
		setMenuItemsEnabled(helpMenu, true);
	}
}

void ModelViewerWidget::ldlErrorCallback(LDLError *error)
{
	if (error)
	{
		if (!errorCallback(error))
		{
			error->cancelLoad();
		}
	}
}

void ModelViewerWidget::progressAlertCallback(TCProgressAlert *alert)
{
	if (alert)
	{
		if (strcmp(alert->getSource(), "LDLibraryUpdater") == 0)
		{
			debugPrintf("Updater progress (%s): %f\n", alert->getMessage(),
				alert->getProgress());
		}
		else
		{
			bool showErrors = true;

			if (strcmp(alert->getSource(), "TCImage") == 0)
			{
				showErrors = false;
			}
			if (!progressCallback(alert->getMessage(), alert->getProgress(),
				showErrors))
			{
				alert->abort();
			}
		}
	}
}

bool ModelViewerWidget::staticFileCaseLevel(QDir &dir, char *filename)
{
	int i;
	int len = strlen(filename);
	QString wildcard;
	QStringList files;

	if (!dir.isReadable())
	{
		return false;
	}
	for (i = 0; i < len; i++)
	{
		QChar letter = filename[i];

		if (letter.isLetter())
		{
			wildcard.append('[');
			wildcard.append(letter.lower());
			wildcard.append(letter.upper());
			wildcard.append(']');
		}
		else
		{
			wildcard.append(letter);
		}
	}
	dir.setNameFilter(wildcard);
	files = dir.entryList();
	if (files.count())
	{
		QString file = files[0];

		if (file.length() == strlen(filename))
		{
			// This should never be false, but just want to be sure.
			strcpy(filename, file);
			return true;
		}
	}
	return false;
}

bool ModelViewerWidget::staticFileCaseCallback(char *filename)
{
	char *shortName;
	QDir dir;
	char *firstSlashSpot;

	dir.setFilter(QDir::All | QDir::Readable | QDir::Hidden | QDir::System);
	replaceStringCharacter(filename, '\\', '/');
	firstSlashSpot = strchr(filename, '/');
	if (firstSlashSpot)
	{
		char *lastSlashSpot = strrchr(filename, '/');
		int dirLen;
		char *dirName;

		while (firstSlashSpot != lastSlashSpot)
		{
			char *nextSlashSpot = strchr(firstSlashSpot + 1, '/');

			dirLen = firstSlashSpot - filename + 1;
			dirName = new char[dirLen + 1];
			*nextSlashSpot = 0;
			strncpy(dirName, filename, dirLen);
			dirName[dirLen] = 0;
			if (dirLen)
			{
				dir.setPath(dirName);
				delete dirName;
				if (!staticFileCaseLevel(dir, firstSlashSpot + 1))
				{
					return false;
				}
			}
			firstSlashSpot = nextSlashSpot;
			*firstSlashSpot = '/';
		}
		dirLen = lastSlashSpot - filename;
		dirName = new char[dirLen + 1];
		strncpy(dirName, filename, dirLen);
		dirName[dirLen] = 0;
		dir.setPath(dirName);
		shortName = lastSlashSpot + 1;
		delete dirName;
	}
	else
	{
		shortName = filename;
	}
	return staticFileCaseLevel(dir, shortName);
}
