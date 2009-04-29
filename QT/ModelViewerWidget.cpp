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
#include <qdesktopwidget.h>
#include <qinputdialog.h>
#if (QT_VERSION >> 16) >= 4
#define HAVE_QT4
#include <q3paintdevicemetrics.h>
#include <q3popupmenu.h>
#define QPaintDeviceMetrics Q3PaintDeviceMetrics
#else
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>
#endif
#include <qprinter.h>
#include <qfileinfo.h>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <ApplicationServices/ApplicationServices.h>
#endif // __APPLE__
#include "misc.h"

#include <TCFoundation/TCAutoreleasePool.h>
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
#include <LDLib/LDInputHandler.h>
//#include <LDLib/ModelMacros.h>
#include <TRE/TREMainModel.h>
#include <TRE/TREGLExtensions.h>
#include "OpenGLExtensionsPanel.h"
#include "AboutPanel.h"
#include "HelpPanel.h"
#include "LDViewMainWindow.h"
#include "LDViewErrors.h"
#include "LDViewModelTree.h"
#include "LDViewExtraDir.h"
#include "LDViewExportOption.h"
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
#define JPG_IMAGE_TYPE_INDEX 3
#define WIN_WIDTH 640
#define WIN_HEIGHT 480

TCStringArray *ModelViewerWidget::recentFiles = NULL;

ModelViewerWidget::ModelViewerWidget(QWidget *parent, const char *name)
	:QGLWidget(parent, name),
    modeltree(new LDViewModelTree(preferences,this)),
    boundingbox(new BoundingBox(this)),
    mpdmodel(new MpdModel(this)),
	modelViewer(new LDrawModelViewer(100, 100)),
	snapshotTaker(NULL),
	lastX(-1),
	lastY(-1),
	originalZoomY(-1),
	rotationSpeed(0.0f),
	viewMode(LDInputHandler::VMExamine),
	spinButton(1),
	zoomButton(2),
    preferences(NULL),
    extradir(NULL),
    snapshotsettings(NULL),
    jpegoptions(NULL),
    extensionsPanel(NULL),
	latitudelongitude(new LatitudeLongitude(this)),
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
	fileExportId(-1),
	statusBar(NULL),
	toolBar(NULL),
	progressBar(NULL),
	progressLabel(NULL),
	progressLatlong(NULL),
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
	libraryUpdateWindow(NULL),
	lightingSelection(0)
{
	int i;
	const QMimeSource *mimeSource =
		QMimeSourceFactory::defaultFactory()->data("StudLogo.png");

	inputHandler = modelViewer->getInputHandler();
	LDLModel::setFileCaseCallback(staticFileCaseCallback);
	if (mimeSource)
	{
		QImage studImage;

		QImageDrag::decode(mimeSource, studImage);
		TREMainModel::setRawStudTextureData(studImage.bits(),
			studImage.numBytes());
	}
	for (i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		mouseButtonsDown[i] = false;
	}
	preferences = new Preferences(this);
	extradir = new ExtraDir(this);
	snapshotsettings = new SnapshotSettings(this);
	jpegoptions = new JpegOptions(this);
	preferences->doApply();
	setViewMode(Preferences::getViewMode(),
				examineLatLong = Preferences::getLatLongMode());
#ifdef HAVE_QT4
	setFocusPolicy(Qt::StrongFocus);
#else
	setFocusPolicy(QWidget::StrongFocus);
#endif
	setupUserAgent();
}

ModelViewerWidget::~ModelViewerWidget(void)
{
	TCObject::release(snapshotTaker);
	TCObject::release(modelViewer);
	delete preferences;
	delete extensionsPanel;
	delete errors;
	TCObject::release(alertHandler);
	alertHandler = NULL;
}

void ModelViewerWidget::setupUserAgent(void)
{
#ifdef WIN32
	char *unamePath = NULL;
#else
	//char *unamePath = findExecutable("uname");
	char *unamePath = copyString("uname");
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
#ifndef HAVE_QT4
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
#endif
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

    QString fontFilePath = findPackageFile("SansSerif.fnt");
    QFile fontFile (fontFilePath);
    if (fontFile.exists())
    {
        int len = fontFile.size();
        if (len > 0)
        {
            char *buffer = (char*)malloc(len);
            if ( fontFile.open( IO_ReadOnly ) )
            {
                QDataStream stream( &fontFile );
                stream.readRawBytes(buffer,len);
                modelViewer->setFontData((TCByte*)buffer,len);
            }
        }
    }

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
//            modelViewer->loadModel();
			if (modelViewer->loadModel())
			{
        		getFileTime(commandLineFilename, lastWriteTime);
        		if (lastWriteTime.isValid())
        		{
            		startPollTimer();
        		}
        		setLastOpenFile(commandLineFilename);
        		populateRecentFileMenuItems();
				setupStandardSizes();
			    mainWindow->fileSaveAction->setEnabled(true);
    			mainWindow->fileReloadAction->setEnabled(true);
				startPaintTimer();
				updateStep();
    		}
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
		
		QString ext = s.lower().right(4);
		if (ext == ".png")
		{
			saveImageType = PNG_IMAGE_TYPE_INDEX;
		}
		else if (ext == ".bmp")
		{
			saveImageType = BMP_IMAGE_TYPE_INDEX;
		}
		else 
		{
			saveImageType = JPG_IMAGE_TYPE_INDEX;
		}
		saveImage(s2, 
			TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? 
			TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, WIN_WIDTH, false) :
			TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false),
			TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false) ? 
			TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, WIN_HEIGHT, false) :
			TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false), true);
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
	TREGLExtensions::setup();
	preferences->doCancel();
	doViewStatusBar(preferences->getStatusBar());
	doViewToolBar(preferences->getToolBar());
	if (saving)
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

/*
void ModelViewerWidget::updateSpinRate(void)
{
	// If the mouse stops for more than a set time, stop the spinning.
	if (mouseButtonsDown[spinButton] && !lightingSelection)
	{
		if (lastMoveTime.elapsed() >= 100)
		{
			updateSpinRateXY(lastX, lastY);
		}
	}
}
*/

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
		if (saving)
		{
			if (!TREGLExtensions::haveFramebufferObjectExtension())
			{
				glDrawBuffer(GL_BACK);
				glReadBuffer(GL_BACK);
			}
			saveImageResult = snapshotTaker->saveImage(saveImageFilename,
				saveImageWidth, saveImageHeight, saveImageZoomToFit);
		}
		else
		{
			makeCurrent();
			//updateSpinRate();
			redrawRequested = false;
			modelViewer->update();
			//updateFPS();
			//if ((fEq(rotationSpeed, 0.0f) && fEq(modelViewer->getZoomSpeed(), 0.0f)
			//	&& fEq(modelViewer->getCameraXRotate(), 0.0f)
			//	&& fEq(modelViewer->getCameraYRotate(), 0.0f)
			//	&& fEq(modelViewer->getCameraZRotate(), 0.0f)
			//	&& fEq(modelViewer->getCameraMotion().length(), 0.0f))
			//	|| modelViewer->getPaused())
			if (!redrawRequested)
			{
				killPaintTimer();
				fps = -1.0f;
			}
			else
			{
				startPaintTimer();
			}
			updateFPS();
			updateLatlong();
			//swap_Buffers();
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
#ifdef HAVE_QT4
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
		TCAutoreleasePool::processReleases();
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
#ifndef HAVE_QT4
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
	updateStep();
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
	const char *fileSpot = strrchr(filename, '/');
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
		setupStandardSizes();
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
		delete initialDir;
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
			recentFiles->removeStringAtIndex(index + 1);
		}
		recordRecentFiles();
	}
}

// Note: static method.
LDInputHandler::MouseButton ModelViewerWidget::convertMouseButton(int button)
{
	switch (button)
	{
	case 1:
		return LDInputHandler::MBLeft;
	case 2:
		return LDInputHandler::MBRight;
	case 3:
		return LDInputHandler::MBMiddle;
	default:
		// Don't even try to handle if the button number it too high.
		return LDInputHandler::MBUnknown;
	}
}

void ModelViewerWidget::mousePressEvent(QMouseEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseDown(convertKeyModifiers(event->state()),
		convertMouseButton(event->button()), event->globalX(),
		event->globalY()))
	{
		event->ignore();
	}
	unlock();
/*
	int i;
	int button;

	button = event->button();
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
*/
}

/*
void ModelViewerWidget::spinButtonPress(QMouseEvent *event)
{
	if (event->state() & Qt::ShiftButton)
	{
		if (modelViewer && modelViewer->mouseDown(LDVMouseLight, 
					event->globalX(), event->globalY()))
		{
			lightingSelection = 1;
			return;
		}
	}
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
*/

void ModelViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseUp(convertKeyModifiers(event->state()),
		convertMouseButton(event->button()), event->globalX(),
		event->globalY()))
	{
		event->ignore();
	}
	unlock();
/*
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
*/
}

/*
void ModelViewerWidget::spinButtonRelease(QMouseEvent *event)
{
	if (modelViewer && modelViewer->mouseUp(event->globalX(), event->globalY()))
	{
		lightingSelection = 0;
		//preferences->checkLightVector();
		return;
	}
	if (viewMode != LDVViewExamine)
	{
		modelViewer->setCameraXRotate(0.0f);
		modelViewer->setCameraYRotate(0.0f);
	}
}

void ModelViewerWidget::zoomButtonRelease(QMouseEvent *event)
{
	modelViewer->setZoomSpeed(0.0f);
}
*/

void ModelViewerWidget::wheelEvent(QWheelEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseWheel(convertKeyModifiers(event->state()),
		(TCFloat)event->delta() * 0.5f))
	{
		event->ignore();
	}
	unlock();
/*
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
*/
}

void ModelViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseMove(convertKeyModifiers(event->state()),
		event->globalX(), event->globalY()))
	{
		event->ignore();
	}
	unlock();
/*
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
			if (modelViewer && modelViewer->mouseMove(event->globalX(),
												  	  event->globalY()))
			{
				return;
			}
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
*/
}

/*
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
*/

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
#ifndef _NO_BOOST
    if (libraryUpdater)
    {
		QString statusText;

		libraryUpdateWindow->setCancelButtonText(TCLocalStrings::get("OK"));
		setLibraryUpdateProgress(1.0f);
        if (libraryUpdater->getError() && ucstrlen(libraryUpdater->getError()))
        {
			QString qError;

			statusText = TCLocalStrings::get("LibraryUpdateError");
			statusText += ":\n";
			ucstringtoqstring(qError, libraryUpdater->getError());
			statusText += qError;
        }
        switch (finishType)
        {
        case LIBRARY_UPDATE_FINISHED:
            libraryUpdateFinished = true;
			statusText = TCLocalStrings::get("LibraryUpdateComplete");
            break;
        case LIBRARY_UPDATE_CANCELED:
            statusText = TCLocalStrings::get("LibraryUpdateCanceled");
            break;
        case LIBRARY_UPDATE_NONE:
            statusText = TCLocalStrings::get("LibraryUpdateUnnecessary");
			break;
        }
		debugPrintf("About to release library updater.\n");
        libraryUpdater->release();
		debugPrintf("Released library updater.\n");
        libraryUpdater = NULL;
        if (statusText.length())
        {
			libraryUpdateWindow->setLabelText(statusText);
		}
	}
#endif // _NO_BOOST
}

void ModelViewerWidget::showLibraryUpdateWindow(bool initialInstall)
{
#ifndef _NO_BOOST
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
#endif // _NO_BOOST
}

void ModelViewerWidget::createLibraryUpdateWindow(void)
{
	if (!libraryUpdateWindow)
	{
		libraryUpdateWindow = new QProgressDialog(
						TCLocalStrings::get("CheckingForUpdates"),
						TCLocalStrings::get("Cancel"),
#ifdef HAVE_QT4
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
#ifndef _NO_BOOST
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
#endif // _NO_BOOST
}

void ModelViewerWidget::checkForLibraryUpdates(void)
{
#ifndef _NO_BOOST
    if (libraryUpdater)
    {
        showLibraryUpdateWindow(false);
    }
    else
    {
        libraryUpdater = new LDLibraryUpdater;
        char *ldrawDir = getLDrawDir();
		wchar_t *updateCheckError = NULL;

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
			QString qs;
			wcstoqstring(qs, updateCheckError);
			QMessageBox::warning(this,"LDView", qs,
				QMessageBox::Ok, QMessageBox::NoButton);
			delete updateCheckError;
		}
    }
#endif // _NO_BOOST
}

void ModelViewerWidget::connectMenuShows(void)
{
	connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(doFileMenuAboutToShow()));
	connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(doEditMenuAboutToShow()));
	connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(doViewMenuAboutToShow()));
	connect(toolsMenu,SIGNAL(aboutToShow()), this, SLOT(doToolsMenuAboutToShow()));
	connect(helpMenu, SIGNAL(aboutToShow()), this, SLOT(doHelpMenuAboutToShow()));
}

void ModelViewerWidget::setMainWindow(LDViewMainWindow *value)
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
	progressLatlong = new QLabel(statusBar);
	progressMode = new QLabel(statusBar);
	progressBar->setPercentageVisible(false);
	statusBar->addWidget(progressBar);
	statusBar->addWidget(progressLabel, 1);
	statusBar->addWidget(progressLatlong);
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
	if (viewMode == LDInputHandler::VMExamine)
	{
		const wchar_t *message = TCLocalStrings::get(L"ExamineMode");
		int len = wcslen(message);
		QChar *qcString = new QChar[len];
		for (int i = 0; i < len; i++)
		{
			qcString[i] = (QChar)message[i];
		}
		mainWindow->examineModeAction->setOn(true);
		progressMode->setText(QString(qcString, len));
		delete qcString;
		progressMode->setText(TCLocalStrings::get("ExamineMode"));
	}
	else
	{
		mainWindow->flythroughModeAction->setOn(true);
		progressMode->setText(TCLocalStrings::get("FlyThroughMode"));
	}
	mainWindow->viewLatitudeRotationAction->setOn(Preferences::getLatLongMode());
	mainWindow->showPovAspectRatioAction->setOn(
			Preferences::getPovAspectRatio());
	menuBar = mainWindow->menuBar();
	item = menuBar->findItem(menuBar->idAt(0));
	if (item)
	{
		fileMenu = mainWindow->fileMenu;
		fileCancelLoadId = fileMenu->idAt(10);
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
#ifdef HAVE_QT4
	openRecentMenu = new Q3PopupMenu(this, "openRecentMenu");
#else // QT3
	openRecentMenu = new QPopupMenu(this, "openRecentMenu");
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
#ifndef HAVE_QT4
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
#endif
	updateStep();
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
				QString message;
				message = TCLocalStrings::get("ErrorLoadingModel");
				message.replace(QRegExp("%s"),QString(filename));
				QMessageBox::warning(this, "LDView", message, 
					QMessageBox::Ok, QMessageBox::NoButton);

			}
			else
				loadModel(filename);
		}
	}
	unlock();
}

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

int ModelViewerWidget::progressCallback(const QString &message, float progress,
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
	if (message.length())
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
		if (statusBar) statusBar->show();
	}
	else
	{
		if (statusBar) statusBar->hide();
	}
	preferences->setStatusBar(flag);
	unlock();
	setupStandardSizes();
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
	setupStandardSizes();
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
#ifndef HAVE_QT4
		mainWindow->GroupBox12->setFrameShape( QFrame::NoFrame );
#endif
		mainWindow->GroupBox12->layout()->setMargin( 0 );
		mainWindow->dockWindows().at(0)->hide();
		mainWindow->showFullScreen();
		fullscreen=1;
	} else
	{
#ifndef HAVE_QT4
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

void ModelViewerWidget::switchExamineLatLong(bool b)
{
	lock();
	if (examineLatLong != b)
	{
		preferences->setLatLongMode (examineLatLong = b);
		setViewMode (viewMode, examineLatLong);
		if (b && Preferences::getViewMode() == LDInputHandler::VMExamine) 
			progressLatlong->setHidden(false); 
		else 
			progressLatlong->setHidden(true);
	}
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
		UCSTR temp = LDrawModelViewer::getOpenGLDriverInfo(extensionCount);

		extensionsPanel = new OpenGLExtensionsPanel;
		ucstringtoqstring(openGLDriverInfo, temp);
		extensionsPanel->extensionsBox->setText(openGLDriverInfo);
		extensionsCountLabel = new QLabel(extensionsPanel->statusBar());
		countString.sprintf("%d", extensionCount);
		countString += TCLocalStrings::get("OpenGlnExtensionsSuffix");
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
#ifdef __APPLE__
	QString helpFilename = findPackageFile(TCLocalStrings::get("HelpHtml"));
	QString qUrl = QString("file://") + helpFilename + "#MacNotes";
	CFURLRef url = NULL;
	CFStringRef urlString;
	bool macSuccess = false;

	urlString = CFStringCreateWithCString(NULL, qUrl.utf8(),
		kCFStringEncodingUTF8);
	if (urlString && (url = CFURLCreateWithString(NULL, urlString, NULL)) !=
		NULL)
	{
		if (LSOpenCFURLRef(url, NULL) == 0)
		{
			macSuccess = true;
		}
	}
	if (urlString)
	{
		CFRelease(urlString);
	}
	if (url)
	{
		CFRelease(url);
	}
	if (macSuccess)
	{
		return;
	}
	FSRef fsRef;
	Boolean isDirectory;

	if (FSPathMakeRef((const UInt8 *)(const char *)helpFilename, &fsRef,
		&isDirectory) == 0 && !isDirectory)
	{
		if (LSOpenFSRef(&fsRef, NULL) == 0)
		{
			return;
		}
	}
#endif // __APPLE__
	if(!helpContents)
	{
		QString helpFilename = findPackageFile(TCLocalStrings::get("HelpHtml"));
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
#ifndef HAVE_QT4
	QProcess *proc = new QProcess(this);
	proc->addArgument("gnome-open");
	proc->addArgument(findPackageFile(TCLocalStrings::get("HelpHtml")));
	if (!proc->start())
#endif
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
		aboutPanel = new About;
		connect(aboutPanel->okButton, SIGNAL(clicked()), this,
			SLOT(doAboutOK()));
		aboutPanel->resize(10, 10);
		QString text = aboutPanel->getText();
		text.replace( QRegExp("__DATE__"),__DATE__);
		aboutPanel->setText(text);
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

void ModelViewerWidget::doAxes(bool value)
{
    preferences->setShowAxes(value);
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
		mainWindow->toolbarAxesAction->setOn(preferences->getShowAxes());
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

void ModelViewerWidget::updateLatlong(void)
{
	if (modelViewer && 
		modelViewer->getViewMode() == LDrawModelViewer::VMExamine &&
		modelViewer->getExamineMode() == LDrawModelViewer::EMLatLong)
	{
		int lat = (int)(modelViewer->getExamineLatitude()+.5);
		int lon = (int)(modelViewer->getExamineLongitude()+.5);
		if (lon == -180) lon = 180;
		QString latlongstring;
		latlongstring.sprintf(TCLocalStrings::get("LatLonFormat"),lat,lon);
		progressLatlong->setText(latlongstring);
	}
	else progressLatlong->setText("");
	
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

void ModelViewerWidget::setViewMode(LDInputHandler::ViewMode value, 
									bool examine, bool /*saveSettings*/)
{
	viewMode = value;
	if (viewMode == LDInputHandler::VMExamine)
	{
		LDrawModelViewer::ExamineMode examineMode = ( examine ? 
				LDrawModelViewer::EMLatLong : LDrawModelViewer::EMFree );
		inputHandler->setViewMode(LDInputHandler::VMExamine);
		modelViewer->setConstrainZoom(true);
		if (progressMode)
		{
			progressMode->setText(TCLocalStrings::get("ExamineMode"));
		}
		if (progressLatlong)
		{
			progressLatlong->setHidden(!examine);
		}
		modelViewer->setExamineMode(examineMode);
	}
	else
	{
		inputHandler->setViewMode(LDInputHandler::VMFlyThrough);
		modelViewer->setConstrainZoom(false);
		if (progressMode)
		{
			progressMode->setText(TCLocalStrings::get("FlyThroughMode"));
		}
		if (progressLatlong) progressLatlong->setHidden(true);
	}
	Preferences::setViewMode(viewMode);
}

void ModelViewerWidget::doViewModeChanged(QAction *action)
{
	LDInputHandler::ViewMode newMode = LDInputHandler::VMExamine;

	lock();
	if (action == mainWindow->flythroughModeAction)
	{
		newMode = LDInputHandler::VMFlyThrough;
	}
	setViewMode(newMode,examineLatLong);
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

bool ModelViewerWidget::staticImageProgressCallback(const wchar_t* message, 
							float progress, void* userData)
{
	QString qs="";
	if (message) wcstoqstring(qs,message);
    return ((ModelViewerWidget*)userData)->progressCallback(qs, progress, true);
}

bool ModelViewerWidget::writeImage(char *filename, int width, int height,
                             TCByte *buffer, const char *formatName,
							 bool saveAlpha)
{
    TCImage *image = new TCImage;
    bool retValue;
	char comment[1024];

    if (saveAlpha)
    {
        image->setDataFormat(TCRgba8);
    }
    image->setSize(width, height);
    image->setLineAlignment(4);
    image->setImageData((TCByte*)buffer);
    image->setFormatName(formatName);
    image->setFlipped(true);
	if (strcasecmp(formatName, "PNG") == 0)
	{
		strcpy(comment, "Software:!:!:LDView");
	}
	else
	{
		strcpy(comment, "Created by LDView");
	}
	image->setComment(comment);
	if (TCUserDefaults::longForKey(AUTO_CROP_KEY, 0, false))
	{
		image->autoCrop((TCByte)modelViewer->getBackgroundR(),
			(TCByte)modelViewer->getBackgroundG(),
			(TCByte)modelViewer->getBackgroundB());
	}
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

bool ModelViewerWidget::grabImage(
	int &imageWidth,
	int &imageHeight,
	bool fromCommandLine /*= false*/)
{
    int newWidth = 800;
    int newHeight = 600;
	int origWidth = mwidth;
	int origHeight = mheight;
    int numXTiles, numYTiles;
	bool origSlowClear = modelViewer->getSlowClear();
	int origMemoryUsage = modelViewer->getMemoryUsage();

	saving = true;
	modelViewer->setMemoryUsage(0);
	if (snapshotTaker->getUseFBO())
	{
		newWidth = snapshotTaker->getFBOSize();
		newHeight = snapshotTaker->getFBOSize();
	}
	snapshotTaker->calcTiling(imageWidth, imageHeight, newWidth, newHeight,
		numXTiles, numYTiles);
	if (!snapshotTaker->getUseFBO())
	{
		setupSnapshotBackBuffer(newWidth, newHeight);
	}
    imageWidth = newWidth * numXTiles;
    imageHeight = newHeight * numYTiles;
	saveImageWidth = imageWidth;
	saveImageHeight = imageHeight;
	if (snapshotTaker->getUseFBO())
	{
		makeCurrent();
		if (fromCommandLine)
		{
			saveImageResult = snapshotTaker->saveImage();
		}
		else
		{
			saveImageResult = snapshotTaker->saveImage(saveImageFilename,
				saveImageWidth, saveImageHeight, saveImageZoomToFit);
		}
	}
	else
	{
		renderPixmap(newWidth, newHeight);
	}
	makeCurrent();
	TREGLExtensions::setup();
	if (!snapshotTaker->getUseFBO())
	{
		modelViewer->openGlWillEnd();
	}
	saving = false;
	mwidth = origWidth;
	mheight = origHeight;
	modelViewer->setWidth(mwidth);
	modelViewer->setHeight(mheight);
	modelViewer->setMemoryUsage(origMemoryUsage);
	modelViewer->setSlowClear(origSlowClear);
	modelViewer->setup();
	doApply();
	return saveImageResult;
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
    int newWidth = 800;
    int newHeight = 600;
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
	if ((mwidth > 0) && (mheight > 0))
	{
        newWidth = mwidth;       // width is OpenGL window width
        newHeight = mheight;     // height is OpenGL window height
        calcTiling(imageWidth, imageHeight, newWidth, newHeight, numXTiles,
            numYTiles);
		setupSnapshotBackBuffer(newWidth, newHeight);
	}
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
			makeCurrent();
			//screen.save("/tmp/ldview.png","PNG");
			//printf("file %ux%ix%i\n",screen.width(),screen.height(),
			//	screen.depth());
			if (progressCallback(TCLocalStrings::get("RenderingSnapshot"),
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
				}
				// We only need to zoom to fit on the first tile; the
				// rest will already be correct.
				modelViewer->setForceZoomToFit(false);
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

bool ModelViewerWidget::calcSaveFilename(char* saveFilename, int /*len*/)
{
	char* filename = modelViewer->getFilename();
	saveDigits = TCUserDefaults::longForKey(SAVE_DIGITS_KEY, 1, false);
	if (filename)
	{
		char baseFilename[1024];

		if (strrchr(filename, '/'))
		{
			filename = strrchr(filename, '/') + 1;
		}
		if (strrchr(filename, '\\'))
		{
			filename = strrchr(filename, '\\') + 1;
		}
		strcpy(baseFilename, filename);
		if (strchr(baseFilename, '.'))
		{
			*strchr(baseFilename, '.') = 0;
		}
		if (curSaveOp == LDPreferences::SOExport)
		{
			sprintf(saveFilename, "%s.%s", baseFilename,
				modelViewer->getExporter(
				(LDrawModelViewer::ExportType)exportType)->
				getExtension().c_str());
			return true;
		}
		else
		{
			char format[32] = "%s.%s";
			const char *extension = NULL;
			int max;
			if (TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0)
			{
				max = (int)(pow(10.0, saveDigits + 0.1));
				sprintf(format, "%%s%%0%dd.%%s", saveDigits);	
			}
			else max = 2;
			int i;
			for (i = 1; i < max; i++)
			{
				if (saveImageType == PNG_IMAGE_TYPE_INDEX)
				{
					extension = "png";
				}
				else if (saveImageType == BMP_IMAGE_TYPE_INDEX)
				{
					extension = "bmp";
				}
				else if (saveImageType == JPG_IMAGE_TYPE_INDEX)
				{
					extension = "jpg";
				}
				if (TCUserDefaults::longForKey(SAVE_SERIES_KEY, 1, false) != 0)
				{
					sprintf(saveFilename, format, baseFilename, i , extension);
				}
				else
				{
					sprintf(saveFilename, format, baseFilename, extension);
				}
				if (TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false, false))
				{
					QString suffix = TCUserDefaults::stringForKey(SAVE_STEPS_SUFFIX_KEY,
							TCLocalStrings::get("DefaultStepSuffix"), false);
					std::string temp = LDSnapshotTaker::addStepSuffix(saveFilename,
                    	    suffix.ascii(), 1, modelViewer->getNumSteps());
	                strcpy(saveFilename, temp.c_str());
	
				}
				if (!fileExists(saveFilename))
				{
					return true;
				}
			}
			return true;
		}
	}
	return false;
}

bool ModelViewerWidget::getSaveFilename(char* saveFilename, int len)
{
	QString initialDir = preferences->getSaveDir(curSaveOp,
		modelViewer->getFilename());
	LDrawModelViewer::ExportType origExportType = modelViewer->getExportType();
	QStringList exportFilters;
	QStringList::const_iterator exportFilterIt;

	QDir::setCurrent(initialDir);
	saveImageType = TCUserDefaults::longForKey(SAVE_IMAGE_TYPE_KEY, 1, false);
	exportType = TCUserDefaults::longForKey(SAVE_EXPORT_TYPE_KEY,
		LDrawModelViewer::ETPov, false);
	if (!calcSaveFilename(saveFilename, len))
	{
		saveFilename[0] = 0;
	}
	switch (curSaveOp)
	{
	case LDPreferences::SOExport:
		origExportType = modelViewer->getExportType();
		for (int i = LDrawModelViewer::ETFirst; i <= LDrawModelViewer::ETLast;
			i++)
		{
			const LDExporter *exporter = modelViewer->getExporter(
				(LDrawModelViewer::ExportType)i);

			if (exporter != NULL)
			{
				ucstring ucFileType = exporter->getTypeDescription();
				QString qFileType;

				ucstringtoqstring(qFileType, ucFileType);
				qFileType += " (*.";
				qFileType += exporter->getExtension().c_str();
				qFileType += ")";
				exportFilters << qFileType;
			}
		}
		modelViewer->getExporter(origExportType);
		exportFilterIt = exportFilters.begin();
		saveDialog = new QFileDialog(".", *exportFilterIt, this, "", true);
		for (exportFilterIt++; exportFilterIt != exportFilters.end();
			exportFilterIt++)
		{
			saveDialog->addFilter(*exportFilterIt);
		}
		saveDialog->setIcon(getimage("LDViewIcon16.png"));
		saveDialog->setSelectedFilter(exportType - LDrawModelViewer::ETFirst);
		saveDialog->setMode(QFileDialog::AnyFile);
		saveDialog->setCaption(TCLocalStrings::get("ExportModel"));
		break;
	case LDPreferences::SOSnapshot:
	default:
		saveDialog = new QFileDialog(".",
			"Portable Network Graphics (*.png)",
			this,
			"open model dialog",
			true);
		saveDialog->setCaption(TCLocalStrings::get("SaveSnapshot"));
		saveDialog->addFilter("Windows Bitmap (*.bmp)");
		saveDialog->addFilter("Jpeg (*.jpg)");
		saveDialog->setSelectedFilter(saveImageType-1);
		saveDialog->setIcon(getimage("LDViewIcon16.png"));
		saveDialog->setMode(QFileDialog::AnyFile);
		break;
	}
	saveDialog->setSelection(saveFilename);
	if (saveDialog->exec() == QDialog::Accepted)
	{
		QString filename = saveDialog->selectedFile(), dir = saveDialog->dirPath();
        switch (curSaveOp)
        {
        case LDPreferences::SOExport:
            TCUserDefaults::setPathForKey(dir, LAST_EXPORT_DIR_KEY, false);
            break;
        case LDPreferences::SOSnapshot:
        default:
            TCUserDefaults::setPathForKey(dir, LAST_SNAPSHOT_DIR_KEY, false);
            break;
        }
		QDir::setCurrent(dir);
		strncpy(saveFilename,filename,len);
		QString filter = saveDialog->selectedFilter();
		if (filter.find(".png") != -1)
		{
			saveImageType = PNG_IMAGE_TYPE_INDEX;
		}
        if (filter.find(".jpg") != -1)
        {
            saveImageType = JPG_IMAGE_TYPE_INDEX;
        }
        if (filter.find(".bmp") != -1)
        {
            saveImageType = BMP_IMAGE_TYPE_INDEX;
        }
		if (filter.find(".pov") != -1)
		{
			exportType = LDrawModelViewer::ETPov;
		}
		if (filter.find(".stl") != -1)
		{
			exportType = LDrawModelViewer::ETStl;
		}
		
		TCUserDefaults::setLongForKey(saveImageType, SAVE_IMAGE_TYPE_KEY,
			false);
		TCUserDefaults::setLongForKey(exportType, SAVE_EXPORT_TYPE_KEY, false);
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
			else if (saveImageType == JPG_IMAGE_TYPE_INDEX)
			{
				strcat(saveFilename, ".jpg");
			}
		}
		delete saveDialog;
		return true;
	}
	delete saveDialog;
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

LDSnapshotTaker::ImageType ModelViewerWidget::getSaveImageType(void)
{
	switch (saveImageType)
	{
		case PNG_IMAGE_TYPE_INDEX:
			return LDSnapshotTaker::ITPng;
		case BMP_IMAGE_TYPE_INDEX:
			return LDSnapshotTaker::ITBmp;
		case JPG_IMAGE_TYPE_INDEX:
			return LDSnapshotTaker::ITJpg;
		default:
			return LDSnapshotTaker::ITPng;
	}
}

bool ModelViewerWidget::saveImage(
	char *filename,
	int imageWidth, 
	int imageHeight,
	bool fromCommandLine /*= false*/)
{
	bool retValue = false;

	if (!snapshotTaker)
	{
		if (fromCommandLine)
		{
			snapshotTaker =  new LDSnapshotTaker;
		}
		else
		{
			snapshotTaker =  new LDSnapshotTaker(modelViewer);
		}
	}
	if (TREGLExtensions::haveFramebufferObjectExtension())
	{
		snapshotTaker->setUseFBO(true);
	}
	snapshotTaker->setImageType(getSaveImageType());
	snapshotTaker->setTrySaveAlpha(saveAlpha =
		TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0);
	snapshotTaker->setAutoCrop(
		TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false));
	saveImageFilename = filename;
	//snapshotTaker->setProductVersion(
	//	((LDViewWindow *)parentWindow)->getProductVersion());
	saveImageZoomToFit = TCUserDefaults::longForKey(SAVE_ZOOM_TO_FIT_KEY, 1,
		false);
	retValue = grabImage(imageWidth, imageHeight, fromCommandLine);
	return retValue;

}

bool ModelViewerWidget::fileExists(const char* filename)
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

void ModelViewerWidget::fileExport()
{
	curSaveOp = LDPreferences::SOExport;
	char saveFilename[1024] = "";
	if (getSaveFilename(saveFilename, 1024)&&
		(!fileExists(saveFilename)||shouldOverwriteFile(saveFilename)))
	{
		modelViewer->setExportType((LDrawModelViewer::ExportType)exportType);
		modelViewer->exportCurModel(saveFilename);
	}
}

void ModelViewerWidget::fileExportOption()
{
	LDViewExportOption exportOption(modelViewer);

	if (exportOption.exec() == QDialog::Rejected)
	{
		modelViewer->getExporter((LDrawModelViewer::ExportType)0, true);
	}
}

bool ModelViewerWidget::doFileSave(void)
{
	char saveFilename[1024] = "";

	return doFileSave(saveFilename);
}

bool ModelViewerWidget::doFileSave(char *saveFilename)
{
	curSaveOp = LDPreferences::SOSnapshot;
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

void ModelViewerWidget::doFileJPEGOptions(void)
{
	jpegoptions->show();
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

void ModelViewerWidget::doLatLongViewAngle(void)
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
	latitudelongitude->exec();
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
	preferences->doSaveDefaultViewAngle();
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
	QString qmessage,qcl;
	if (modelViewer)
	{
		ucstring message, commandLine;
		if (modelViewer->getViewInfo(message, commandLine))
		{
			ucstringtoqstring(qmessage,message);
			ucstringtoqstring(qcl,commandLine);
			if(QMessageBox::information(this, 
				TCLocalStrings::get("ViewInfoTitle"), 
				qmessage, QMessageBox::Ok,
				QMessageBox::Cancel)==QMessageBox::Ok)
			{
				QApplication::clipboard()->setText(qcl);
			}
		}
	}
}

void ModelViewerWidget::doShowPovCamera(void)
{
	if (modelViewer)
	{
		UCSTR userMessage;
		char *povCamera;
		modelViewer->getPovCameraInfo(userMessage, povCamera);
		if (userMessage && povCamera)
		{
			QString quserMessage;

			ucstringtoqstring(quserMessage, userMessage);
			if (QMessageBox::information(this,
				TCLocalStrings::get("PovCameraTitle"), quserMessage,
				QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
			{
				QApplication::clipboard()->setText(QString(povCamera));
			};
		}
		delete userMessage;
		delete povCamera;
	}
}
void ModelViewerWidget::doShowPovAspectRatio(bool flag)
{
	modelViewer->setPovCameraAspect(flag, true);
}

void ModelViewerWidget::doPartList(
	LDHtmlInventory *htmlInventory,
	LDPartsList *partsList,
	const char *filename)
{
	if (htmlInventory->generateHtml(filename, partsList,
		modelViewer->getFilename()))
	{
		if (htmlInventory->isSnapshotNeeded())
		{
			char *snapshotPath = copyString(htmlInventory->getSnapshotPath());
			bool saveZoomToFit = modelViewer->getForceZoomToFit();
            bool saveActualSize = TCUserDefaults::longForKey(SAVE_ACTUAL_SIZE_KEY, 1, false);
            int saveWidth = TCUserDefaults::longForKey(SAVE_WIDTH_KEY, 1024, false);
            int saveHeight = TCUserDefaults::longForKey(SAVE_HEIGHT_KEY, 768, false);
            bool origSteps = TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false,
                false);
            int origStep = modelViewer->getStep();

            TCUserDefaults::setBoolForKey(false, SAVE_STEPS_KEY, false);
            modelViewer->setStep(modelViewer->getNumSteps());
			htmlInventory->prepForSnapshot(modelViewer);
			modelViewer->setForceZoomToFit(true);
			TCUserDefaults::setLongForKey(false, SAVE_ACTUAL_SIZE_KEY, false);
			TCUserDefaults::setLongForKey(400, SAVE_WIDTH_KEY, false);
			TCUserDefaults::setLongForKey(300, SAVE_HEIGHT_KEY, false);

			// By saying it's from the command line, none of the above settings
			// will be written to TCUserDefaults.  I know it's not really from
			// the command line, but it produces the behavior we want.
			saveImageType = PNG_IMAGE_TYPE_INDEX;
			saveImage(snapshotPath, 400, 300);
			delete snapshotPath;
			htmlInventory->restoreAfterSnapshot(modelViewer);
			modelViewer->setForceZoomToFit(saveZoomToFit);
			TCUserDefaults::setLongForKey(saveActualSize, SAVE_ACTUAL_SIZE_KEY, false);
            TCUserDefaults::setLongForKey(saveWidth, SAVE_WIDTH_KEY, false);
            TCUserDefaults::setLongForKey(saveHeight, SAVE_HEIGHT_KEY, false);
            modelViewer->setStep(origStep);
            TCUserDefaults::setBoolForKey(origSteps, SAVE_STEPS_KEY, false);
			doApply();
		}
	}
	else
	{
		QMessageBox::warning(this,"LDView",
			TCLocalStrings::get("PLGenerateError"),
			QMessageBox::Ok, QMessageBox::NoButton);
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
			    QString initialDir = preferences->getSaveDir(LDPreferences::SOPartsList, 
				modelViewer->getFilename());
				QDir::setCurrent(initialDir);

				bool done = false;
				char *cFilename = modelViewer->getFilename();
				QString filename;

				if (cFilename)
				{
					filename = cFilename;
				}
				else
				{
					consolePrintf("No filename from modelViewer.\n");
				}
				int findSpot = filename.findRev(QRegExp("/\\"));
				if (findSpot >= 0 && findSpot < (int)filename.length())
					filename=filename.mid(findSpot+1);
				findSpot = filename.findRev('.');
				if (findSpot >= 0 && findSpot < (int)filename.length())
                    filename=filename.left(findSpot);
				filename += ".html";
				findSpot = filename.findRev('/');
				if (findSpot >= 0 && findSpot < (int)filename.length())
					filename = filename.mid(findSpot + 1);
				QString startWith = QString(htmlInventory->getLastSavePath()) +
					QString("/") + filename;
				QString filter = QString(TCLocalStrings::get("HtmlFileType")) +
					" (*.html)";
				while (!done)
				{
					QString htmlFilename = QFileDialog::getSaveFileName(
						initialDir + "/" + filename,
						filter,
						this,
						"Generate Parts List dialog",
						TCLocalStrings::get("GeneratePartsList"));
					if (htmlFilename.isEmpty())
					{
						done = true;
					}
					else
					{
						if (fileExists(htmlFilename.ascii()))
						{
							QString prompt =
								TCLocalStrings::get("OverwritePrompt");

							prompt.replace("%s", htmlFilename);
							if (QMessageBox::warning(this, "LDView", prompt,
								QMessageBox::Yes, QMessageBox::No) ==
								QMessageBox::No)
							{
								continue;
							}
						}
						doPartList(htmlInventory, partsList,
							htmlFilename.ascii());
						done = true;
					}
				}
			}
			htmlInventory->release();
			partsList->release();
		}
	}
}

void ModelViewerWidget::doModelTree()
{
	modeltree->show();
}

void ModelViewerWidget::doBoundingBox()
{
	boundingbox->show();
}

void ModelViewerWidget::doMpdModel()
{
	mpdmodel->show();
}

// Note: static method
LDInputHandler::KeyCode ModelViewerWidget::convertKeyCode(int osKey)
{
	if (osKey >= Qt::Key_A && osKey <= Qt::Key_Z)
	{
		return (LDInputHandler::KeyCode)(osKey - Qt::Key_A +
			LDInputHandler::KCA);
	}
	else
	{
		switch (osKey)
		{
		case Qt::Key_Up:
			return LDInputHandler::KCUp;
		case Qt::Key_Down:
			return LDInputHandler::KCDown;
		case Qt::Key_Left:
			return LDInputHandler::KCLeft;
		case Qt::Key_Right:
			return LDInputHandler::KCRight;
		case Qt::Key_Space:
			return LDInputHandler::KCSpace;
		case Qt::Key_Prior:
			return LDInputHandler::KCPageUp;
		case Qt::Key_Next:
			return LDInputHandler::KCPageDown;
		case Qt::Key_Home:
			return LDInputHandler::KCHome;
		case Qt::Key_End:
			return LDInputHandler::KCEnd;
		case Qt::Key_Insert:
			return LDInputHandler::KCInsert;
		case Qt::Key_Delete:
			return LDInputHandler::KCDelete;
		case Qt::Key_Escape:
			return LDInputHandler::KCEscape;
		default:
			return LDInputHandler::KCUnknown;
		}
	}
}

void ModelViewerWidget::keyPressEvent(QKeyEvent *event)
{
	lock();
	if (inputHandler->keyDown(convertKeyModifiers(event->state()),
		convertKeyCode(event->key())))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
	if((event->state() & Qt::AltButton) &&
		(event->key() >= Qt::Key_0) &&
		(event->key() <= Qt::Key_9) && preferences)
	{
		int i = event->key()-Qt::Key_0;
		preferences->performHotKey(i);
	}
/*	if(event->key() == Qt::Key_PageDown)
	{
		nextStep();
	}
	if(event->key() == Qt::Key_PageUp)
	{
		prevStep();
	}
	if(event->key() == Qt::Key_End)
	{
		lastStep();
	}
	if(event->key() == Qt::Key_Home)
	{
        firstStep();
	}
*/
	unlock();
	QGLWidget::keyPressEvent(event);
/*
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
*/
}

void ModelViewerWidget::keyReleaseEvent(QKeyEvent *event)
{
	lock();
	if (inputHandler->keyUp(convertKeyModifiers(event->state()),
		convertKeyCode(event->key())))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
	unlock();
	QGLWidget::keyReleaseEvent(event);
/*
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
*/
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
			fileMenu->setItemEnabled(fileMenu->idAt(6), false);
			fileMenu->setItemEnabled(fileMenu->idAt(11), false);
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
		{
			setMenuItemsEnabled(viewMenu, true);
			viewMenu->setItemEnabled(viewMenu->idAt(9), viewMode == LDInputHandler::VMExamine);
		}
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

void ModelViewerWidget::snapshotTakerAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == snapshotTaker)
	{
		if (strcmp(alert->getMessage(), "MakeCurrent") == 0)
		{
			makeCurrent();
		}
	}
}

void ModelViewerWidget::redrawAlertCallback(TCAlert * /*alert*/)
{
	lock();
    startPaintTimer();
	redrawRequested = true;
	unlock();
}

void ModelViewerWidget::captureAlertCallback(TCAlert * /*alert*/)
{
}

void ModelViewerWidget::releaseAlertCallback(TCAlert * /*alert*/)
{
}

void ModelViewerWidget::lightVectorChangedAlertCallback(TCAlert * /*alert*/)
{
	if (preferences)
	{
		preferences->checkLightVector();
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
			QString message;

			if (strcmp(alert->getSource(), "TCImage") == 0)
			{
				showErrors = false;
			}
			wcstoqstring(message, alert->getWMessage());
			if (!progressCallback(message, alert->getProgress(),
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

//	if (!file.exists())
//		QDir::setCurrent("..");
	if (!file.exists())
		QDir::setCurrent("Textures");
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

// Note: static method.
TCULong ModelViewerWidget::convertKeyModifiers(Qt::ButtonState osModifiers)
{
	TCULong retValue = 0;
	if (osModifiers & Qt::ShiftButton)
	{
		retValue |= LDInputHandler::MKShift;
	}
	if (osModifiers & Qt::ControlButton)
	{
		retValue |= LDInputHandler::MKControl;
	}
	return retValue;
}

void ModelViewerWidget::nextStep()
{
	if (modelViewer->getStep()>=modelViewer->getNumSteps())
		return;
	modelViewer->setStep(modelViewer->getStep()+1);
	updateStep();
	doApply();
}

void ModelViewerWidget::prevStep()
{
	if (modelViewer->getStep()<=1)
		return;
	modelViewer->setStep(modelViewer->getStep()-1);
	updateStep();
	doApply();
}

void ModelViewerWidget::firstStep()
{
	modelViewer->setStep(1);
    updateStep();
    doApply();
}

void ModelViewerWidget::lastStep()
{
    modelViewer->setStep(modelViewer->getNumSteps());
    updateStep();
    doApply();
}


void ModelViewerWidget::updateStep()
{
    int step = modelViewer->getStep();
    QString max = QString::number(modelViewer->getNumSteps());
	mainWindow->toolbarFirstStep->setEnabled(step>1);
    mainWindow->toolbarPrevStep->setEnabled(step>1);
    mainWindow->toolbarNextStep->setEnabled(modelViewer->getNumSteps()>step);
	mainWindow->toolbarLastStep->setEnabled(modelViewer->getNumSteps()>step);
	mainWindow->stepGoto->setEnabled(modelViewer->getNumSteps()>0);
    mainWindow->toolbarMaxStep->setText(" / "+max);
    mainWindow->toolbarCurrentStep->setText(QString::number(step));
}

void ModelViewerWidget::gotoStep()
{
	bool ok;
	int step = QInputDialog::getInteger("Step","Go to Step:",
			modelViewer->getStep(), 1, modelViewer->getNumSteps(), 1, &ok, this);
	if (ok)
	{
		modelViewer->setStep(step);
		updateStep();
		doApply();
	}
}

void ModelViewerWidget::setupStandardSizes()
{
	QSize workArea = QApplication::desktop()->availableGeometry(mainWindow).size();
	QSize windowSize = mainWindow->frameSize();
	LDrawModelViewer::getStandardSizes(workArea.width() - windowSize.width() +
									   modelViewer->getWidth(),
									   workArea.height() - windowSize.height() +
									   modelViewer->getHeight(),
									   standardSizes);
	mainWindow->standardSizesPopupMenu->clear();
	for (size_t i = 0; i < standardSizes.size(); i++)
	{
		QString qs;
		ucstringtoqstring(qs, standardSizes[i].name);
		mainWindow->standardSizesPopupMenu->insertItem(qs, this, 
								SLOT(standardSizeSelected(int)), 0, i );
	}
}

void ModelViewerWidget::standardSizeSelected(int i)
{
	QString text;
	QRegExp sep( "\\s+" );
	text = mainWindow->standardSizesPopupMenu->text(i);
	if (text != QString::null)
	{
		int w,h;
		bool ok;
		w = text.section(sep,0,0).toInt(&ok);
		h = text.section(sep,2,2).toInt(&ok);
		mainWindow->resize(w + mainWindow->size().width() - 
							   modelViewer->getWidth(),
						   h + mainWindow->size().height() - 
							   modelViewer->getHeight());
	}
}

