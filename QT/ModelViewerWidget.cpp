#include "qt4wrapper.h"
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
#include <qclipboard.h>
#include <qpainter.h>
#include <qprocess.h>
#include <qprogressdialog.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#if (QT_VERSION >>16)==4
#include <q3paintdevicemetrics.h>
#define QPaintDeviceMetrics Q3PaintDeviceMetrics
#else
#include <qpaintdevicemetrics.h>
#endif
#include <qprinter.h>
#include <qobjectlist.h>
#include <qfileinfo.h>

#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCWebClient.h>
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
#include "SnapshotSettingsPanel.h"
#include "PartList.h"
#include "LDViewSnapshotSettings.h"
#include "LDViewPartList.h"
#include <TCFoundation/TCUserDefaults.h>
#include "LDLib/LDUserDefaultsKeys.h"
#include <LDLib/LDPartsList.h>
#include <assert.h>

#include "ModelViewerWidget.h"
#include "AlertHandler.h"

#define POLL_INTERVAL 500

#define PNG_IMAGE_TYPE_INDEX 1
#define BMP_IMAGE_TYPE_INDEX 2
#define WIN_WIDTH 640
#define WIN_HEIGHT 480

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
	snapshotsettings(NULL),
	extensionsPanel(NULL),
	openGLDriverInfo(NULL),
	aboutPanel(NULL),
	helpContents(NULL),
	mainWindow(NULL),
	menuBar(NULL),
	fileMenu(NULL),
	editMenu(NULL),
	viewMenu(NULL),
	toolsMenu(NULL),
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
	saving(false),
	cancelLoad(false),
	app(NULL),
	painting(false),
	fps(-1.0),
	numFramesSinceReference(0),
	paintTimer(0),
	pollTimer(0),
	loadTimer(0),
	libraryUpdateTimer(0),
	fileDialog(NULL),
	saveDialog(NULL),
	errors(NULL),
	fileInfo(NULL),
	lockCount(0),
	fullscreen(0),
#ifdef __APPLE__
	openRecentMenu(NULL),
#endif // __APPLE__
	alertHandler(new AlertHandler(this)),
	libraryUpdater(NULL),
	libraryUpdateProgressReady(false),
	libraryUpdateWindow(NULL)
{
	int i;
	const QMimeSource *mimeSource =
		QMimeSourceFactory::defaultFactory()->data("StudLogo.png");

	LDLModel::setFileCaseCallback(staticFileCaseCallback);
	if (mimeSource)
	{
		QImage studImage;

		QImageDrag::decode(mimeSource, studImage);
		TREMainModel::setRawStudTextureData(studImage.bits(),
			studImage.numBytes());
	}
/*
	TCAlertManager::registerHandler(LDLError::alertClass(),
		dynamic_cast<TCObject *>(this),
		(TCAlertCallback)&ModelViewerWidget::ldlErrorCallback);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(),
		dynamic_cast<TCObject *>(this),
		(TCAlertCallback)&ModelViewerWidget::progressAlertCallback);
*/

//	modelViewer->setProgressCallback(staticProgressCallback, this);
//	modelViewer->setErrorCallback(staticErrorCallback, this);
	for (i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		mouseButtonsDown[i] = false;
	}
	preferences = new Preferences(this);
	extradir = new ExtraDir(this);
	snapshotsettings = new SnapshotSettings(this);
	preferences->doApply();
	setViewMode(Preferences::getViewMode());
#if (QT_VERSION >>16)==4
	setFocusPolicy(Qt::StrongFocus);
#else
	setFocusPolicy(QWidget::StrongFocus);
#endif
	setupUserAgent();
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
	TCObject::release(alertHandler);
	alertHandler = NULL;
}

void ModelViewerWidget::setupUserAgent(void)
{
#ifdef WIN32
	char *unamePath = NULL;
#else
	char *unamePath = findExecutable("uname");
#endif
	// If uname below doesn't work, just use the generic "QT" instead.
	QString osName = "QT";
	QString userAgent;
	// If we can't parse the version out of the AboutPanel, use 3.2.  Note: this
	// should be updated in future versions, but the extraction from the
	// AboutPanel hopefully won't fail.
	QString ldviewVersion = "3.2";
	bool foundVersion = false;
	QString fullVersion;

	int spot;

	// Try to use the uname command to determine our OS name.
	if (unamePath)
	{
		// uname was found in the path.
		QProcess unameProcess((QString)unamePath);

		delete unamePath;
		// We don't care about stdin and stderr.
		unameProcess.setCommunication(QProcess::Stdout);
		if (unameProcess.start())
		{
			QTime startTime;

			startTime.start();
			// Wait until we get a line of output on stdout.  That line will
			// be the OS name.
			while (!unameProcess.canReadLineStdout())
			{
				if (startTime.elapsed() > 5000)
				{
					// If uname takes 5 seconds to execute, we're in trouble.
					break;
				}
				// sleep for 50ms.
#ifdef WIN32
				Sleep(50);
#else // WIN32
				usleep(50000);
#endif // WIN32
			}
			if (unameProcess.canReadLineStdout())
			{
				// readLineStdout strips off any CR/LF, so we're left with the
				// OS name, which is what we want.
				osName = QString("QT-") + unameProcess.readLineStdout();
			}
		}
	}
	// We're going to grab the version label from the about panel, so make sure
	// it's created first.
	createAboutPanel();
	fullVersion = aboutPanel->VersionLabel->text();
	// The version will always begin with a number.
	if ((spot = fullVersion.find(QRegExp("[0-9]"))) != -1)
	{
		fullVersion = fullVersion.right(fullVersion.length() - spot);
		// The first thing after the version is an open parenthesis.  Look
		// for that.
		if ((spot = fullVersion.find("(")) != -1)
		{
			fullVersion = fullVersion.left(spot);
			ldviewVersion = fullVersion.stripWhiteSpace();
			foundVersion = true;
		}
	}
	// Even though we have a default value for the version, we REALLY want to
	// extract it from the about panel.  Assert if the above extraction wasn't
	// successful.
	assert(foundVersion);
	userAgent.sprintf("LDView/%s (%s; ldview@gmail.com; "
		"http://ldview.sf.net/)", (const char *)ldviewVersion,
		(const char *)osName);
	TCWebClient::setUserAgent(userAgent);
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
    TCStringArray *commandLine = TCUserDefaults::getProcessedCommandLine();
    char *commandLineFilename = NULL;

    if (commandLine)
    {
        int i;
        int count = commandLine->getCount();
        for (i = 0; i < count && !commandLineFilename; i++)
        {
            char *arg = commandLine->stringAtIndex(i);

            if (arg[0] != '-')
                commandLineFilename = arg;
        }
    }
	QString current = QDir::currentDirPath();
    if (commandLineFilename && verifyLDrawDir())
    {
        QFileInfo fi(commandLineFilename);
        commandLineFilename = copyString(fi.absFilePath().ascii());
//      loadModel(commandLineFilename);
        if (chDirFromFilename(commandLineFilename))
        {
            modelViewer->setFilename(commandLineFilename);
            modelViewer->loadModel();
        }
    }
    char *snapshotFilename =
        TCUserDefaults::stringForKey(SAVE_SNAPSHOT_KEY);
    if (snapshotFilename)
    {
		QDir::setCurrent(current);
		QFileInfo fi(snapshotFilename);
		QString s(snapshotFilename);
		char *s2=copyString(fi.absFilePath().ascii());
		saveImageType = ( s.lower().right(4)==".png" ? 
			PNG_IMAGE_TYPE_INDEX : BMP_IMAGE_TYPE_INDEX);
		saveImage(s2, 
			TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? 
			TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, WIN_WIDTH, false) :
			TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false),
            TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? 
			TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, WIN_HEIGHT, false) :
			TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false));
//		QApplication::exit();
		exit(0);
    }
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
	if(saving)
	{
		modelViewer->setup();
		modelViewer->openGlWillEnd();
//		modelViewer->recompile();
	}
	unlock();
}

void ModelViewerWidget::resizeGL(int width, int height)
{
	lock();
	if (!loading && !saving)
	{
		QSize mainWindowSize = mainWindow->size();

		modelViewer->setWidth(mwidth=width);
		modelViewer->setHeight(mheight=height);
		glViewport(0, 0, width, height);
		preferences->setWindowSize(mainWindowSize.width(),
			mainWindowSize.height());
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
	if (!painting && (saving || !loading))
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
		if (!saving)
		{
			swap_Buffers();
		}
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

void ModelViewerWidget::setLibraryUpdateProgress(float progress)
{
#if (QT_VERSION >= 0x40000)
	libraryUpdateWindow->setValue((int)(progress * 100));
#else
	libraryUpdateWindow->setProgress((int)(progress * 100));
#endif
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
	else if (event->timerId() == libraryUpdateTimer)
	{
		if (libraryUpdateFinishNotified)
		{
			killTimer(libraryUpdateTimer);
			libraryUpdateTimer = 0;
			doLibraryUpdateFinished(libraryUpdateFinishCode);
		}
		else if (!libraryUpdateCanceled)
		{
			lock();
			if (libraryUpdateProgressReady)
			{
				libraryUpdateProgressReady = false;
				libraryUpdateWindow->setLabelText(libraryUpdateProgressMessage);
				delete libraryUpdateProgressMessage;
				libraryUpdateProgressMessage = NULL;
				setLibraryUpdateProgress(libraryUpdateProgressValue);
			}
			unlock();
		}
	}
	unlock();
}

void ModelViewerWidget::paintEvent(QPaintEvent *event)
{
	lock();
	if (loading)
	{
		int r, g, b;

		preferences->getBackgroundColor(r, g, b);
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
void ModelViewerWidget::doFilePrint(void)
{
	QPrinter *printer;
	printer = new QPrinter(QPrinter::HighResolution);
	printer->setOptionEnabled(QPrinter::PrintSelection,false);
	printer->setOptionEnabled(QPrinter::PrintPageRange,false);
	printer->setColorMode(QPrinter::Color);
	printer->setMinMax(1,1);
	printer->setFullPage(TRUE);
	if (printer->setup())
	{
		QPainter p;
		if (!p.begin(printer))
			return;
		QPaintDeviceMetrics metrics (p.device());
		int dpix = metrics.logicalDpiX(), 
		    dpiy = metrics.logicalDpiY(),
			marginx = (int) (2/2.54)*dpix,
			marginy = (int) (2/2.54)*dpiy,
			pwidth = metrics.width()-2*marginx,
			pheight = metrics.height()-2*marginy,
			bytesPerLine,
			y, x;
		printf("%ix%i %ix%i DPI\n",pwidth,pheight,dpix,dpiy);
		int r, g, b;
        preferences->getBackgroundColor(r, g, b);
		modelViewer->setBackgroundRGB(255,255,255);
		if (dpix != dpiy)
			modelViewer->setPixelAspectRatio((float)dpix / dpiy);
		saveImageType = BMP_IMAGE_TYPE_INDEX;
		TCByte *buffer = grabImage(pwidth,pheight,NULL,true);
		QImage *image = new QImage(pwidth,pheight,32);
		bytesPerLine = roundUp(pwidth * 3, 4);
		for(y = 0 ; y < pheight; y++)
			for(x = 0 ; x < pwidth; x++)
			{
				image->setPixel(x,pheight-y-1,qRgb(buffer[x*3 + y*bytesPerLine],
								buffer[x*3 + y*bytesPerLine + 1], 
								buffer[x*3 + y*bytesPerLine + 2]));
			}
		p.drawImage(marginx,marginy,*image);
		delete image;
		delete buffer;
		modelViewer->setBackgroundRGB(r, g, b);
		modelViewer->setPixelAspectRatio(1.0f);
	}
	delete printer;
	endLoad();
	makeCurrent();
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

void ModelViewerWidget::wheelEvent(QWheelEvent *event)
{
	bool controlPressed;

	lock();
	if (loading)
	{
		unlock();
		return;
	}
	controlPressed = event->state() & Qt::ControlButton;
	startPaintTimer();
	if (controlPressed)
	{
		modelViewer->setClipZoom(true);
	}
	else
	{
		modelViewer->setClipZoom(false);
	}
	modelViewer->zoom((TCFloat)event->delta() * -0.1f);
	unlock();
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
	TCFloat magnitude = (TCFloat)sqrt((TCFloat)(deltaX * deltaX + deltaY * deltaY));

	lastX = xPos;
	lastY = yPos;
	rotationSpeed = magnitude / 10.0f;
	if (fEq(rotationSpeed, 0.0f))
	{
		rotationSpeed = 0.0f;
	}
	else
	{
		modelViewer->setXRotate((TCFloat)deltaY);
		modelViewer->setYRotate((TCFloat)deltaX);
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
	TCFloat magnitude = (TCFloat)(xPos - lastX);
	TCFloat denom = 5000.0f;

	modelViewer->setCameraXRotate(magnitude / denom);
	magnitude = (TCFloat)(yPos - lastY);
	modelViewer->setCameraYRotate(magnitude / -denom);
}

void ModelViewerWidget::updateZoom(int yPos)
{
	TCFloat magnitude = (TCFloat)(yPos - originalZoomY);

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

void ModelViewerWidget::doLibraryUpdateFinished(int finishType)
{
    if (libraryUpdater)
    {
        char statusText[1024] = "";
		libraryUpdateWindow->setCancelButtonText(TCLocalStrings::get("OK"));
		setLibraryUpdateProgress(1.0f);
        if (libraryUpdater->getError() && strlen(libraryUpdater->getError()))
        {
            sprintf(statusText, "%s:\n%s",
                TCLocalStrings::get("LibraryUpdateError"),
                libraryUpdater->getError());
        }
        switch (finishType)
        {
        case LIBRARY_UPDATE_FINISHED:
            libraryUpdateFinished = true;
            strcpy(statusText, TCLocalStrings::get("LibraryUpdateComplete"));
            break;
        case LIBRARY_UPDATE_CANCELED:
            strcpy(statusText, TCLocalStrings::get("LibraryUpdateCanceled"));
            break;
        case LIBRARY_UPDATE_NONE:
            strcpy(statusText, TCLocalStrings::get("LibraryUpdateUnnecessary"));            break;
        }
		debugPrintf("About to release library updater.\n");
        libraryUpdater->release();
		debugPrintf("Released library updater.\n");
        libraryUpdater = NULL;
        if (strlen(statusText))
        {
			libraryUpdateWindow->setLabelText(statusText);
		}
	}
}

void ModelViewerWidget::showLibraryUpdateWindow(bool initialInstall)
{
	if (!libraryUpdateWindow)
	{
		createLibraryUpdateWindow();
	}
	libraryUpdateWindow->setCancelButtonText(TCLocalStrings::get("Cancel"));
	libraryUpdateWindow->reset();
	if (initialInstall)
	{
		libraryUpdateWindow->setModal(true);
	}
	else
	{
		libraryUpdateWindow->setModal(false);
		connect(libraryUpdateWindow, SIGNAL(canceled()), this,
			SLOT(doLibraryUpdateCanceled()));
	}
}

void ModelViewerWidget::createLibraryUpdateWindow(void)
{
	if (!libraryUpdateWindow)
	{
		libraryUpdateWindow = new QProgressDialog(
						TCLocalStrings::get("CheckingForUpdates"),
						TCLocalStrings::get("Cancel"),
#if QT_VERSION >= 0x40000
						0,100,mainWindow);
#else
						100,mainWindow,"progress",TRUE);
#endif
		libraryUpdateWindow->setMinimumDuration(0);
		libraryUpdateWindow->setAutoReset(false);
	}
}

bool ModelViewerWidget::installLDraw(void)
{
	// Don't lock here unless you're REALLY careful.  In particular, you
	// DEFINITELY have to unlock prior to doing the event processing.
    if (libraryUpdater)
    {
        return false;
    }
    else
    {
        char *ldrawParentDir = getLDrawDir();
        char *ldrawDir = copyString(ldrawParentDir, 255);
        QDir originalDir = QDir::current();
		bool progressDialogClosed = false;

        libraryUpdateFinished = false;
        strcat(ldrawDir, "/ldraw");
		
		QDir dir(ldrawDir);
		if (!dir.exists())
		{
			dir.mkdir(ldrawDir);
		}
		libraryUpdater = new LDLibraryUpdater;
        libraryUpdateCanceled = false;
		libraryUpdateFinishNotified = false;
		libraryUpdateFinished = false;
		progressDialogClosed = false;
        libraryUpdater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
        libraryUpdater->setLdrawDir(ldrawDir);
        libraryUpdater->installLDraw();
        showLibraryUpdateWindow(true);
		if (!libraryUpdateTimer)
		{
			libraryUpdateTimer = startTimer(50);
		}
		while (libraryUpdater || !progressDialogClosed)
		{
			// We want the update window to be modal, so process events in a
			// tight modal loop.  (See modal section in QProgressDialog
			// documentation.)
			qApp->processEvents();
			if (!progressDialogClosed && libraryUpdateWindow->wasCanceled())
			{
				progressDialogClosed = true;
				// When the install finishes for real, we change the button
				// title from "Cancel" to "OK".  However, it still acts like
				// a cancel.  So check to se if the update really finished, and
				// if it didn't, then note that the user canceled.
				if (!libraryUpdateFinished)
				{
					libraryUpdateCanceled = true;
					doLibraryUpdateFinished(LIBRARY_UPDATE_CANCELED);
					//libraryUpdateCanceled = false;
				}
				break;
			}
			if (libraryUpdateFinishNotified)
			{
                doLibraryUpdateFinished(libraryUpdateFinishCode);
			}
			// Sleep for 50ms.  Unlike the QProgressDialog example, we aren't
			// doing anything inside this loop other than processing the
			// events.  All the work is happening in other threads.  So sleep
			// for a short time in order to avoid monopolizing the CPU.  Keep in
			// mind that while 50ms is essentially unnoticable to a user, it's
			// quite a long time to the computer.
#ifdef WIN32
			Sleep(50);
#else // WIN32
			usleep(50000);
#endif // WIN32
		}
        if (libraryUpdateFinished)
        {
            LDLModel::setLDrawDir(ldrawDir);
			preferences->setLDrawDir(ldrawDir);
        }
        delete ldrawDir;
        return libraryUpdateFinished;
	}
}

void ModelViewerWidget::checkForLibraryUpdates(void)
{
    if (libraryUpdater)
    {
        showLibraryUpdateWindow(false);
    }
    else
    {
        libraryUpdater = new LDLibraryUpdater;
        char *ldrawDir = getLDrawDir();
		char *updateCheckError = NULL;

        libraryUpdateCanceled = false;
		libraryUpdateFinishNotified = false;
		libraryUpdateFinished = false;
        libraryUpdater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
        libraryUpdater->setLdrawDir(ldrawDir);
        delete ldrawDir;
		if (libraryUpdater->canCheckForUpdates(updateCheckError))
		{
			showLibraryUpdateWindow(false);
			if (!libraryUpdateTimer)
			{
				libraryUpdateTimer = startTimer(50);
			}
			libraryUpdater->checkForUpdates();
		}
		else
		{
			QMessageBox::warning(this,"LDView", updateCheckError,
				QMessageBox::Ok, QMessageBox::NoButton);
			delete updateCheckError;
		}
    }
}

void ModelViewerWidget::connectMenuShows(void)
{
	connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(doFileMenuAboutToShow()));
	connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(doEditMenuAboutToShow()));
	connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(doViewMenuAboutToShow()));
	connect(toolsMenu,SIGNAL(aboutToShow()), this, SLOT(doToolsMenuAboutToShow()));
	connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(doHelpMenuAboutToShow()));
}

void ModelViewerWidget::setMainWindow(LDView *value)
{
	QMenuItem *item;
	QAction *pollAction;
	int width, height;
//	QSize windowSize;
	int i;
	int cnt;

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
		fileCancelLoadId = fileMenu->idAt(8);
		fileReloadId = fileMenu->idAt(1);
	}
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
	openRecentMenu = new QPopupMenu(this, "openRecentMenu");
	fileMenu->insertItem("Open Recent", openRecentMenu, -1, 1);
#endif // __APPLE__
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
	}
	item = menuBar->findItem(menuBar->idAt(2));
	if (item)
	{
		viewMenu = mainWindow->viewMenu;
	}
	item = menuBar->findItem(menuBar->idAt(3));
	if (item)
	{
		toolsMenu = mainWindow->toolsMenu;
	}
	item = menuBar->findItem(menuBar->idAt(4));
	if (item)
	{
		helpMenu = mainWindow->helpMenu;
	}
	connectMenuShows();
    saveAlpha = TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0;
	QObjectList *toolButtons = mainWindow->toolbar->queryList("QToolButton");
	QObjectListIt it(*toolButtons);
	QObject *object;
	for (; (object = it.current()) != NULL; ++it)
	{
		QToolButton *button = (QToolButton*)object;
		if (button->textLabel() == TCLocalStrings::get("ViewingAngle"))
		{
			button->setPopup(mainWindow->viewingAnglePopupMenu);
			button->setPopupDelay(1);
		}
	}
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
				QPopupMenu *menu = openRecentMenu;
#else // __APPLE__
				QPopupMenu *menu = fileMenu;
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
			QFile file (filename);
			if(!file.exists())
			{
				char message[2048];
				sprintf(message,TCLocalStrings::get("ErrorLoadingModel"),
						filename);
				QMessageBox::warning(this, "LDView", message, 
					QMessageBox::Ok, QMessageBox::NoButton);

			}
			else
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
		countString.sprintf(TCLocalStrings::get("OpenGlnExtensions"), extensionCount);
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
		QString helpFilename = findPackageFile("Help.html");
		QFile file(helpFilename);
		if (!file.exists())
		{
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
	createAboutPanel();
	aboutPanel->show();
	unlock();
}

void ModelViewerWidget::createAboutPanel(void)
{
	if (!aboutPanel)
	{
		aboutPanel = new AboutPanel;
		connect(aboutPanel->okButton, SIGNAL(clicked()), this,
			SLOT(doAboutOK()));
		aboutPanel->resize(10, 10);
		QString text = aboutPanel->VersionLabel->text();
		text.replace( QRegExp("__DATE__"),__DATE__);
		aboutPanel->VersionLabel->setText(text);
	}
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

void ModelViewerWidget::doLibraryUpdateCanceled(void)
{
	libraryUpdateCanceled = true;
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

void ModelViewerWidget::doBFC(bool value)
{
	preferences->setUseBFC(value);
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
		mainWindow->toolbarBFCAction->setOn(preferences->getUseBFC());
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
				fpsString.sprintf(TCLocalStrings::get("FPSFormat"), fps);
			}
			else
			{
				fpsString = TCLocalStrings::get("FPSSpinPrompt");
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
	
	if (!forceChoose &&
		(!TCUserDefaults::longForKey(VERIFY_LDRAW_DIR_KEY, 1, false) ||
		verifyLDrawDir(lDrawDir)))
	{
		delete lDrawDir;
		found = true;
	}
	else
	{
		delete lDrawDir;
/*
		if (!forceChoose)
		{
			QMessageBox::warning(this, "Can't find LDraw", "The LDraw directory has "
				"not yet been chosen, or it has moved.  Please select the directory "
				"that contains LDraw.", QMessageBox::Ok, QMessageBox::NoButton);
		}
*/
		if (QMessageBox::question(this, "LDView",
			TCLocalStrings::get("LDrawDirExistsPrompt"), QMessageBox::Yes,
			QMessageBox::No | QMessageBox::Default) == QMessageBox::Yes)
		{
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
						QMessageBox::warning(this, 
							TCLocalStrings::get("InvalidDir"),
							TCLocalStrings::get("LDrawNotInDir"),
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
		else
		{
			if (QMessageBox::warning(this,
				"LDView", TCLocalStrings::get("WillDownloadLDraw"),
				QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
			{
				LDLModel::setLDrawDir("/");
				if (promptForLDrawDir(
					TCLocalStrings::get("LDrawInstallDirPrompt")))
				{
					if (installLDraw())
					{
						found = true;
					}
				}
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
			lDrawDir = copyString("/usr/local/share/ldraw");
		}
	}
	stripTrailingPathSeparators(lDrawDir);
	return lDrawDir;
}

bool ModelViewerWidget::promptForLDrawDir(const char *prompt)
{
	char *initialDir = getLDrawDir();
	QFileDialog *dirDialog;
	bool retValue = false;

	if (!prompt)
	{
		prompt = TCLocalStrings::get("LDrawDirPrompt");
	}
	QDir::setCurrent(initialDir);
	dirDialog = new QFileDialog(".",
		"Directories",
		this,
		"open LDraw dir dialog",
		true);
	dirDialog->setCaption(prompt);
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
					if (QMessageBox::information(this, TCLocalStrings::get("PollFileUpdate"),
						TCLocalStrings::get("PollReloadCheck"),
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
                             TCByte *buffer, char *formatName, bool saveAlpha)
{
    TCImage *image = new TCImage;
    bool retValue;

    if (saveAlpha)
    {
        image->setDataFormat(TCRgba8);
    }
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

void ModelViewerWidget::setupSnapshotBackBuffer(int imageWidth, int imageHeight)
{
    modelViewer->setSlowClear(true);
    modelViewer->setWidth(imageWidth);
    modelViewer->setHeight(imageHeight);
    modelViewer->setup();
    glReadBuffer(GL_BACK);
}

TCByte *ModelViewerWidget::grabImage(int &imageWidth, int &imageHeight, 
									TCByte *buffer, bool zoomToFit, 												bool * /*saveAlpha*/)
{
    bool oldSlowClear = modelViewer->getSlowClear();
    bool origForceZoomToFit = modelViewer->getForceZoomToFit();
    TCVector origCameraPosition = modelViewer->getCamera().getPosition();
    TCFloat origXPan = modelViewer->getXPan();
    TCFloat origYPan = modelViewer->getYPan();
    bool origAutoCenter = modelViewer->getAutoCenter();
    int newWidth = 1600;
    int newHeight = 1200;
	int origWidth = mwidth;
	int origHeight = mheight;
    int numXTiles, numYTiles;
    int xTile;
    int yTile;
    int bytesPerPixel = 3;
    int bytesPerLine;
    bool canceled = false;
    bool bufferAllocated = false;
	int memoryusage = modelViewer->getMemoryUsage();

	saving = true;
	modelViewer->setMemoryUsage(0);
    if (zoomToFit)
    {
        modelViewer->setForceZoomToFit(true);
    }
    calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
        numYTiles);
    imageWidth = newWidth * numXTiles;
    imageHeight = newHeight * numYTiles;
	if (1)
	{
        newWidth = mwidth;       // width is OpenGL window width
        newHeight = mheight;     // height is OpenGL window height
        calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
            numYTiles);
		setupSnapshotBackBuffer(newWidth, newHeight);
	}
/*    if (canSaveAlpha())
    {
        bytesPerPixel = 4;
        bufferFormat = GL_RGBA;
        if (saveAlpha)
        {
            *saveAlpha = true;
        }
    }
    else
    {
        if (saveAlpha)
        {
            *saveAlpha = false;
        }
    } */
    bytesPerLine = roundUp(imageWidth * bytesPerPixel, 4);
    if (!buffer)
    {
        buffer = new TCByte[bytesPerLine * imageHeight];
        bufferAllocated = true;
    }
	modelViewer->setNumXTiles(numXTiles);
    modelViewer->setNumYTiles(numYTiles);
	QImage screen;
	QRgb rgb;
	modelViewer->setWidth(newWidth);
	modelViewer->setHeight(newHeight);
    for (yTile = 0; yTile < numYTiles; yTile++)
	{
		modelViewer->setYTile(yTile);
		for (xTile = 0; xTile < numXTiles && !canceled; xTile++)
		{
			modelViewer->setXTile(xTile);
			//renderOffscreenImage();
			screen = renderPixmap(newWidth, newHeight).convertToImage();
			//screen.save("/tmp/ldview.png","PNG");
			//printf("file %ux%ix%i\n",screen.width(),screen.height(),
			//	screen.depth());
			if (progressCallback((char*)TCLocalStrings::get(
				"RenderingSnapshot"),
				(float)(yTile * numXTiles + xTile) / (numYTiles * numXTiles),
				true))
			{
				int x;
				int y;

				for (y = 0; y < newHeight; y++)
				{
					int offset = (y + (numYTiles - yTile - 1) * newHeight) *
						bytesPerLine;

					for (x = 0; x < newWidth; x++)
					{
						int spot = offset + x * bytesPerPixel +
							xTile * newWidth * bytesPerPixel;
						rgb = screen.pixel(x,newHeight - y - 1);
						buffer[spot] = qRed(rgb);
						buffer[spot + 1] = qGreen(rgb);
						buffer[spot + 2] = qBlue(rgb);
					}
					// We only need to zoom to fit on the first tile; the
					// rest will already be correct.
					modelViewer->setForceZoomToFit(false);
				}
			}
		}
	}
	mwidth = origWidth;
	mheight = origHeight;
	modelViewer->setWidth(mwidth);
	modelViewer->setHeight(mheight);
	modelViewer->setMemoryUsage(memoryusage);
	modelViewer->setSlowClear(true);
	modelViewer->setXTile(0);
	modelViewer->setYTile(0);
	modelViewer->setNumXTiles(1);
	modelViewer->setNumYTiles(1);
	if (canceled && bufferAllocated)
	{
		delete buffer;
		buffer = NULL;
	}
	modelViewer->setWidth(mwidth);
	modelViewer->setHeight(mheight);
	modelViewer->setup();
	if (zoomToFit)
	{
		modelViewer->setForceZoomToFit(origForceZoomToFit);
		modelViewer->getCamera().setPosition(origCameraPosition);
		modelViewer->setXYPan(origXPan, origYPan);
		modelViewer->setAutoCenter(origAutoCenter);
	}
	modelViewer->setSlowClear(oldSlowClear);
	doApply();
	saving = false;
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
		saveDialog->setCaption(TCLocalStrings::get("SaveSnapshot"));
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
								 TCByte *buffer)
{
	return writeImage(filename, width, height, buffer, "BMP");
}
																																							 
bool ModelViewerWidget::writePng(char *filename, int width, int height, 
								 TCByte *buffer, bool saveAlpha)
{
	return writeImage(filename, width, height, buffer, "PNG", saveAlpha);
}

bool ModelViewerWidget::saveImage(char *filename, int imageWidth, 
									int imageHeight)
{
	bool saveAlpha = false;
	TCByte *buffer = grabImage(imageWidth, imageHeight, NULL, 
				TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1, false),
				&saveAlpha);
	bool retValue = false;
	if (buffer)
	{
		if (saveAlpha)
		{
			int i;
			int totalBytes = imageWidth * imageHeight * 4;

			for (i = 3; i < totalBytes; i += 4)
			{
				if (buffer[i] != 0 && buffer[i] != 255)
				{
					if (buffer[i] == 74)
					{
						buffer[i] = 255 - 28;
					}
					else
					{
						buffer[i] = 255;
					}
				}
			}
		}
		if (saveImageType == PNG_IMAGE_TYPE_INDEX)
		{
			retValue = writePng(filename, imageWidth, imageHeight, buffer,
								saveAlpha);
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
		return saveImage(saveFilename, TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false) : modelViewer->getWidth(), 
				TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false) :  modelViewer->getHeight());
	}
	else
	{
		return false;
	}
}

void ModelViewerWidget::doFileSaveSettings(void)
{
	snapshotsettings->show();
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
	TCFloat matrix[16];
	TCFloat rotationMatrix[16];
	TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
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

void ModelViewerWidget::cleanupFloats(TCFloat *array, int count)
{
	int i;
																				
	for (i = 0; i < count; i++)
	{
		if (fabs(array[i]) < 1e-6)
		{
			array[i] = 0.0f;
		}
	}
}

void ModelViewerWidget::doShowViewInfo(void)
{
	QString qmessage;
	if (modelViewer)
	{
		TCFloat matrix[16];
		TCFloat rotationMatrix[16];
		TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
		char matrixString[1024];
		char zoomString[128];
		char message[4096];
		TRECamera &camera = modelViewer->getCamera();
		TCFloat defaultDistance = modelViewer->getDefaultDistance();
		TCFloat distanceMultiplier = modelViewer->getDistanceMultiplier();
		TCFloat cameraDistance;
																				
		memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
			sizeof(rotationMatrix));
		TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
		cleanupFloats(matrix);
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

	QMessageBox::information(this, TCLocalStrings::get("ViewInfoTitle"), 
			qmessage, QMessageBox::Ok,
			QMessageBox::NoButton);
	}
}

void ModelViewerWidget::doShowPovCamera(void)
{
	if (modelViewer)
	{
		char *userMessage, *povCamera;
		modelViewer->getPovCameraInfo(userMessage, povCamera);
		if (userMessage && povCamera)
		{
			if(QMessageBox::information(this, TCLocalStrings::get("PovCameraTitle"), 
				QString(userMessage), QMessageBox::Ok, QMessageBox::Cancel)==
				QMessageBox::Ok)
			{
				QApplication::clipboard()->setText(QString(povCamera));
			};
		}
		delete userMessage;
		delete povCamera;
	}
}

void ModelViewerWidget::doPartList(void)
{
	if (modelViewer)
	{
		LDPartsList *partsList = modelViewer->getPartsList();
		if (partsList)
		{
			LDHtmlInventory *htmlInventory = new LDHtmlInventory;
			PartList *partlist = new PartList(this, htmlInventory);
			if (partlist->exec() == QDialog::Accepted)
			{
				QString filename = modelViewer->getFilename();
				int findSpot = filename.findRev(QRegExp("/\\"));
				if (findSpot < (int)filename.length())
					filename=filename.mid(findSpot+1);
				findSpot = filename.findRev('.');
				if (findSpot < (int)filename.length())
                    filename=filename.left(findSpot);
				filename += ".html";
            	QFileDialog *fileDialog = new QFileDialog(
			    	htmlInventory->getLastSavePath(),
                	QString(TCLocalStrings::get("HtmlFileType"))+" (*.html)",
                	this,
                	"open model dialog",
                	true);
            	fileDialog->setCaption(QString(
							TCLocalStrings::get("GeneratePartsList")));
            	fileDialog->setIcon(getimage("LDViewIcon16.png"));
				fileDialog->setMode(QFileDialog::AnyFile);
				fileDialog->setSelection(filename);
        		if (fileDialog->exec() == QDialog::Accepted)
        		{
					QString filename = fileDialog->selectedFile();
					htmlInventory->generateHtml(filename.ascii(),
						partsList, modelViewer->getFilename());
        		}
			}
			htmlInventory->release();
			partsList->release();
		}
	}
}

void ModelViewerWidget::processKey(QKeyEvent *event, bool press)
{
	TCVector cameraMotion = modelViewer->getCameraMotion();
	TCFloat originalMotionAmount = 1.0f;
	TCFloat originalRotationAmount = 0.01f;
	TCFloat motionAmount;
	TCFloat rotationAmount;
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
			fileMenu->setItemEnabled(fileMenu->idAt(3), false);
			fileMenu->setItemEnabled(fileMenu->idAt(10), false);
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
		if (!modelViewer || !modelViewer->getMainTREModel())
		{
			setMenuItemsEnabled(viewMenu, false);
			viewMenu->setItemEnabled(viewMenu->idAt(3), true);
			viewMenu->setItemEnabled(viewMenu->idAt(2), true);
		}
		else
			setMenuItemsEnabled(viewMenu, true);
	}
}

void ModelViewerWidget::doToolsMenuAboutToShow(void)
{
    if (loading)
    {
        setMenuItemsEnabled(toolsMenu, false);
    }
    else
    {
        if (!modelViewer || !modelViewer->getMainTREModel())
        {
            setMenuItemsEnabled(toolsMenu, false);
        }
        else
            setMenuItemsEnabled(toolsMenu, true);
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

void ModelViewerWidget::modelViewerAlertCallback(TCAlert *alert)
{
	if (alert)
	{
		QMessageBox::warning(this,"LDView",alert->getMessage(),
			QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void ModelViewerWidget::libraryUpdateProgress(TCProgressAlert *alert)
{
	// NOTE: this gets called from inside one of the library update threads.  It
	// does NOT happen in the app's main thread.


	// Are we allowed to update widgets from outside the main thread? NOPE!
	lock();
	//debugPrintf("Updater progress (%s): %f\n", alert->getMessage(),
	//	alert->getProgress());
	libraryUpdateProgressMessage = copyString(alert->getMessage());
	libraryUpdateProgressValue = alert->getProgress();
	libraryUpdateProgressReady = true;
	unlock();
	if (alert->getProgress() == 1.0f)
	{
		// Progress of 1.0 means the library updater is done.
		if (alert->getExtraInfo())
		{
			// We can't call doLibraryUpdateFinished directly, because we're
			// executing from the library update thread.  The 
			// doLibraryUpdateFinished function waits for the library update
			// thread to complete.  That will never happen if it's executing
			// inside the library update thread.  So we record the finish code,
			// tell ourselves we have done so, and the doLibraryUpdateTimer()
			// slot will take care of the rest when running in non-modal mode,
			// and the modal loop will take care of things when running in
			// modal mode (during initial library install).
			if (strcmp((*(alert->getExtraInfo()))[0], "None") == 0)
			{
				libraryUpdateFinishCode = LIBRARY_UPDATE_NONE;
			}
			else
			{
				libraryUpdateFinishCode = LIBRARY_UPDATE_FINISHED;
			}
		}
		else
		{
			libraryUpdateFinishCode = LIBRARY_UPDATE_CANCELED;
		}
		// Just as a note, while I believe that assignment of an int is an
		// atomic operation (and therefore doesn't require thread checking),
		// I'm not 100% sure of this.  So set the code first, and then once
		// it's definitely set, set the notification.  I really couldn't care
		// less if the notification setting is atomic, since I'm only doing a
		// boolean compare on it.
		libraryUpdateFinishNotified = true;
	}
	else if (alert->getProgress() == 2.0f)
	{
		// Progress of 2.0 means the library updater encountered an
		// error.
		libraryUpdateFinishCode = LIBRARY_UPDATE_ERROR;
		libraryUpdateFinishNotified = true;
	}
	if (libraryUpdateCanceled)
	{
		alert->abort();
	}
}

void ModelViewerWidget::progressAlertCallback(TCProgressAlert *alert)
{
	if (alert)
	{
		if (strcmp(alert->getSource(), "LDLibraryUpdater") == 0)
		{
			libraryUpdateProgress(alert);
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

bool ModelViewerWidget::canSaveAlpha(void)
{
	if (saveAlpha && (saveImageType == PNG_IMAGE_TYPE_INDEX))
	{
		GLint alphaBits;

		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		return alphaBits > 0;
	}
	return false;
}

void ModelViewerWidget::renderOffscreenImage(void)
{
	modelViewer->update();
	repaint();
//	offscreen = renderPixmap();
	if(canSaveAlpha())
	{
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |            GL_VIEWPORT_BIT);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_GREATER);
		glDepthRange(0.0f, 1.0f);
		glBegin(GL_QUADS);
			treGlVertex3f(0.0f, 0.0f, -1.0f);
			treGlVertex3f((TCFloat)mwidth, 0.0f, -1.0f);
			treGlVertex3f((TCFloat)mwidth, (TCFloat)mheight, -1.0f);
			treGlVertex3f(0.0f, (TCFloat)mheight, -1.0f);
		glEnd();
		glColor4ub(0, 0, 0, 129);
		glBlendFunc(GL_DST_ALPHA, GL_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_LESS, 1.0);
		glBegin(GL_QUADS);
			treGlVertex3f(0.0f, 0.0f, -1.0f);
			treGlVertex3f((TCFloat)mwidth, 0.0f, -1.0f);
			treGlVertex3f((TCFloat)mwidth, (TCFloat)mheight, -1.0f);
			treGlVertex3f(0.0f, (TCFloat)mheight, -1.0f);
		glEnd();
		glPopAttrib();
	}
}

void ModelViewerWidget::calcTiling(int desiredWidth, int desiredHeight,
							 int &bitmapWidth, int &bitmapHeight,
							 int &numXTiles, int &numYTiles)
{
	if (desiredWidth > bitmapWidth)
	{
		numXTiles = (desiredWidth + bitmapWidth - 1) / bitmapWidth;
	}
	else
	{
		numXTiles = 1;
	}
	bitmapWidth = desiredWidth / numXTiles;
	if (desiredHeight > bitmapHeight)
	{
		numYTiles = (desiredHeight + bitmapHeight - 1) / bitmapHeight;
	}
	else
	{
		numYTiles = 1;
	}
	bitmapHeight = desiredHeight / numYTiles;
}

void ModelViewerWidget::userDefaultChangedAlertCallback(TCAlert *alert)
{
	if (preferences)
	{
		preferences->userDefaultChangedAlertCallback(alert);
	}
}

void ModelViewerWidget::doPreferences(void)
{
	showPreferences();
}

QString ModelViewerWidget::findPackageFile(const QString &filename)
{
	QString dir = QDir::currentDirPath();
	QFile file(filename);
	QString retValue;

	if (!file.exists())
		QDir::setCurrent("..");
	if (!file.exists())
		QDir::setCurrent("/usr/local/share/ldview");
	if (!file.exists())
		QDir::setCurrent("/usr/local/etc");
	if (!file.exists())
		QDir::setCurrent("/usr/local/lib");
	if (!file.exists()) 
	{
		const char *argv0 = TCUserDefaults::getArgv0();

		QDir::setCurrent(dir);
		if (argv0)
		{
			char *path = copyString(argv0, filename.length() + 5);

			if (strrchr(path, '/'))
			{
				*strrchr(path, '/') = 0;
			}
			QDir::setCurrent(path);
		}
	}
	if (file.exists())
	{
		QString newDir = QDir::currentDirPath();
		retValue = newDir + "/" + file.name();
	}
	QDir::setCurrent(dir);
	return retValue;
}
