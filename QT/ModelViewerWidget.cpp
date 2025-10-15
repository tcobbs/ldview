#include <QFileDialog>
#include <QProgressBar>
#include <QTextBrowser>
#include <QTextStream>
#include <QApplication>
#include <QStatusBar>
#include <QPainter>
#include <QMessageBox>
#include <QDateTime>
#include <QClipboard>
#include <QProcess>
#include <QProgressDialog>
#include <QTimer>
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QInputDialog>
#include <QPrintDialog>
#include <QDesktopServices>
#include <QPrinter>
#include <QFileInfo>
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
#include "About.h"
#include "Help.h"
#include "LDViewMainWindow.h"
#include "LDViewErrors.h"
#include "LDViewModelTree.h"
#include "LDViewExportOption.h"
#include "LDViewSnapshotSettings.h"
#include "LDViewPartList.h"
#include <TCFoundation/TCUserDefaults.h>
#include "LDLib/LDUserDefaultsKeys.h"
#include <LDLib/LDPartsList.h>
#include <assert.h>

#include "ModelViewerWidget.h"
#include "AlertHandler.h"
#include <LDLib/LDConsoleAlertHandler.h>
#include <png.h>
#include <jpeglib.h>
#include <gl2ps.h>
#include <tinyxml.h>
#ifdef HAVE_MINIZIP
#ifdef __has_include
#if __has_include (<minizip/mz.h>)
#include <minizip/mz.h>
#endif
#endif
#endif
#define POLL_INTERVAL 500

#define PNG_IMAGE_TYPE_INDEX 1
#define BMP_IMAGE_TYPE_INDEX 2
#define JPG_IMAGE_TYPE_INDEX 3
#define WIN_WIDTH 640
#define WIN_HEIGHT 480

#define LDRAW_ZIP_SHOW_WARNING_KEY "LDrawZipShowWarning"

ModelViewerWidget::ModelViewerWidget(QWidget *parent)
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
	:QOpenGLWidget(parent),
#else
	:QGLWidget(parent),
#endif
	modeltree(new LDViewModelTree(parent,preferences,this)),
	cameralocation(new CameraLocation(parent, this)),
	rotationcenter(new RotationCenter(parent, this)),
	boundingbox(new BoundingBox(parent, this)),
	mpdmodel(new MpdModel(parent,this)),
	statistics(new Statistics(parent,this)),
	modelViewer(new LDrawModelViewer(100, 100)),
	snapshotTaker(NULL),
	lastX(-1),
	lastY(-1),
	originalZoomY(-1),
	rotationSpeed(0.0f),
	viewMode(LDInputHandler::VMExamine),
	spinButton(1),
	zoomButton(2),
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	fbo(NULL),
#endif
	preferences(NULL),
	snapshotsettings(NULL),
	jpegoptions(NULL),
	extensionsPanel(NULL),
	latitudelongitude(new LatitudeLongitude(parent, this)),
	aboutPanel(NULL),
	helpContents(NULL),
	mainWindow(NULL),
	statusBar(NULL),
	progressBar(NULL),
	progressLabel(NULL),
	progressLatlong(NULL),
	progressMode(NULL),
	loading(false),
	saving(false),
	printing(false),
	cancelLoad(false),
	app(NULL),
	painting(false),
	fps(-1.0),
	numFramesSinceReference(0),
	paintTimer(0),
	pollTimer(0),
	loadTimer(0),
	libraryUpdateTimer(0),
	saveDialog(NULL),
	errors(new LDViewErrors(this, preferences)),
	lastFileSize(0),
	fileInfo(NULL),
	lockCount(0),
	fullscreen(0),
#ifdef __APPLE__
//	openRecentMenu(NULL),
#endif // __APPLE__
	alertHandler(new AlertHandler(this)),
#if !defined(_NO_BOOST) || defined(USE_CPP11)
	libraryUpdater(NULL),
#endif
	libraryUpdateProgressReady(false),
	libraryUpdateWindow(NULL),
	lightingSelection(0),
	commandLineSnapshotSave(false)
{
	int i;

	inputHandler = modelViewer->getInputHandler();
	LDLModel::setFileCaseCallback(staticFileCaseCallback);
	QImage studImage(":/images/images/StudLogo.png");

	TREMainModel::setRawStudTextureData(studImage.bits(),
#if QT_VERSION < 0x40600
			studImage.numBytes());
#else
#if QT_VERSION < QT_VERSION_CHECK(5,10,0)
			studImage.byteCount());
#else
			studImage.sizeInBytes());
#endif
#endif

	for (i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		mouseButtonsDown[i] = false;
	}
	preferences = new Preferences(parent,this);
	snapshotsettings = new SnapshotSettings(parent,this);
	jpegoptions = new JpegOptions(parent,this);
	preferences->doApply();
	setViewMode(Preferences::getViewMode(),
				examineLatLong = Preferences::getLatLongMode(),
			keepRightSide = Preferences::getKeepRightSideUp());
	setFocusPolicy(Qt::StrongFocus);
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
	// If uname below doesn't work, just use the generic "QT" instead.
	QString osName = "QT-";
	QString userAgent;
	// If we can't parse the version out of the AboutPanel, use 3.2.  Note: this
	// should be updated in future versions, but the extraction from the
	// AboutPanel hopefully won't fail.
	QString ldviewVersion = "4.3";
	bool foundVersion = false;
	QString fullVersion;

	int spot;
				osName +=
#if   defined (Q_OS_LINUX)
"Linux"
#elif defined (Q_OS_AIX)
"AIX"
#elif defined (Q_OS_NETBSD)
"NetBSD"
#elif defined (Q_OS_FREEBSD)
"FreeBSD"
#elif defined (Q_OS_SOLARIS)
"Solaris"
#elif defined (Q_OS_WIN32)
"Windows"
#elif defined (Q_OS_WIN64)
"Windows"
#elif defined (Q_OS_HPUX)
"HP/UX"
#elif defined (Q_OS_CYGWIN)
"Cygwin"
#elif defined (Q_OS_IRIX)
"Irix"
#elif defined (Q_OS_OSF)
"Osf"
#else
"unknown"
#endif
;
	// We're going to grab the version label from the about panel, so make sure
	// it's created first.
	createAboutPanel();
	fullVersion = aboutPanel->getText();
	// The version will always begin with a number.
	if ((spot = fullVersion.indexOf(
#if QT_VERSION >= 0x60000
		QRegularExpression("[0-9]")
#else
		QRegExp("[0-9]")
#endif
		)) != -1)
	{
		fullVersion = fullVersion.right(fullVersion.length() - spot);
		// The first thing after the version is an open parenthesis.  Look
		// for that.
		if ((spot = fullVersion.indexOf("(")) != -1)
		{
			fullVersion = fullVersion.left(spot);
			ldviewVersion = fullVersion.trimmed();
			foundVersion = true;
		}
	}
	// Even though we have a default value for the version, we REALLY want to
	// extract it from the about panel.  Assert if the above extraction wasn't
	// successful.
	assert(foundVersion);
	userAgent=QString("LDView/%1 (%2; ldview@gmail.com; "
		"https://github.com/tcobbs/ldview)").arg(ldviewVersion).arg(osName);
	TCWebClient::setUserAgent(userAgent.toLatin1().constData());
}

void ModelViewerWidget::setApplication(QApplication *value)
{
	char *arg0;

	app = value;
	arg0 = copyString(QCoreApplication::arguments().at(0).toUtf8().constData());
	if (strrchr(arg0, '/'))
	{
		*strrchr(arg0, '/') = 0;
	}
	long color = TCUserDefaults::longForKey(MODEL_TREE_HIGHLIGHT_COLOR_KEY, (0xa0e0ff), false);
	modelViewer->setHighlightColor( color >>16, (color >>8) & 0xff, color & 0xff);
	modelViewer->setProgramPath(arg0);
	delete arg0;
	QString arg1;
	if (QCoreApplication::arguments().size()>1 && QString::compare (QCoreApplication::arguments().at(1), "-specialcharacters")==0)
	{
		QMessageBox::information(this, "Special Characters",
			QString::fromWCharArray(TCLocalStrings::get(L"SpecialCharacters")),
			QMessageBox::Ok, QMessageBox::NoButton);
	}
	if (QCoreApplication::arguments().size()>1 && QString::compare (QCoreApplication::arguments().at(1), "-libver")==0)
	{
		QString ver;
		ver=QString("")+
#ifdef PNG_LIBPNG_VER_STRING
			"libpng:\t\t"+ PNG_LIBPNG_VER_STRING+"\n"+
#endif
#ifdef LIBJPEG_TURBO_VERSION
			"libjpeg-turbo:\t"+QString::number(LIBJPEG_TURBO_VERSION_NUMBER/1000000)+"."+QString::number((LIBJPEG_TURBO_VERSION_NUMBER/1000)%10)+"."+QString::number(LIBJPEG_TURBO_VERSION_NUMBER%10)+"\n"+
#endif
#ifdef ZLIBNG_VERSION
			"zlib-ng:\t\t"+ZLIBNG_VERSION+"\n"+
#endif
#ifdef ZLIB_VERSION
			"zlib:\t\t"+ZLIB_VERSION+"\n"+
#endif
#ifdef MZ_VERSION
			"minizip:\t\t"+MZ_VERSION+"\n"+
#endif
#ifdef GL2PS_MAJOR_VERSION
			"gl2ps:\t\t"+QString::number(GL2PS_MAJOR_VERSION)+"."+QString::number(GL2PS_MINOR_VERSION)+"."+QString::number(GL2PS_PATCH_VERSION)+"\n"+
#endif
#ifdef TINYXML_INCLUDED
			"tinyxml:\t\t"+QString::number(TIXML_MAJOR_VERSION)+"."+QString::number(TIXML_MINOR_VERSION)+"."+QString::number(TIXML_PATCH_VERSION)+"\n"+
#endif
			"Qt:\t\t"+QT_VERSION_STR+"\n";
		QMessageBox::information(this, "Library Versions",
		ver,
		QMessageBox::Ok, QMessageBox::NoButton);
	}
	QString fontFilePath = findPackageFile("SansSerif.fnt");
	QFile fontFile (fontFilePath);
	if (fontFile.exists())
	{
		int len = fontFile.size();
		if (len > 0)
		{
			char *buffer = (char*)malloc(len);
			if ( fontFile.open( QIODevice::ReadOnly ) )
			{
				QDataStream stream( &fontFile );
				stream.readRawData(buffer,len);
				modelViewer->setFontData((TCByte*)buffer,len);
			}
		}
	}
	QImage fontImage2x(":/images/images/SanSerif@2x.png");
#if QT_VERSION < 0x40600
	long len = fontImage2x.numBytes();
#else
#if QT_VERSION < QT_VERSION_CHECK(5,10,0)
	long len = fontImage2x.byteCount();
#else
	long len = fontImage2x.sizeInBytes();
#endif
#endif
	modelViewer->setRawFont2xData(fontImage2x.bits(),len);

	bool shouldExit = false;
	// Let LDSnapshotTaker perform an export if requested, but don't try to use
	// it to save snapshots, because that doesn't work.
	if (LDSnapshotTaker::doCommandLine(false, true))
	{
		shouldExit = true;
	}
	const TCStringArray *commandLine = TCUserDefaults::getProcessedCommandLine();
	const char *commandLineFilename = NULL;

	TCUserDefaults::removeValue(HFOV_KEY, false);
	TCUserDefaults::removeValue(CAMERA_GLOBE_KEY, false);
	if (commandLine)
	{
		int i;
		int count = commandLine->getCount();
		for (i = 0; i < count && !commandLineFilename; i++)
		{
			const char *arg = commandLine->stringAtIndex(i);

			if (arg[0] != '-')
				commandLineFilename = arg;
			if (stringHasCaseInsensitivePrefix(arg, "-ca"))
			{
				float value;

				if (sscanf(arg + 3, "%f", &value) == 1)
				{
					TCUserDefaults::setFloatForKey(value, HFOV_KEY, false);
				}
			}
			else if (stringHasCaseInsensitivePrefix(arg, "-cg"))
			{
				TCUserDefaults::setStringForKey(arg + 3, CAMERA_GLOBE_KEY,false);
			}
		}
	}
	char *snapshotFilename =
		TCUserDefaults::stringForKey(SAVE_SNAPSHOT_KEY);
	commandLineSnapshotSave = (snapshotFilename ? true : false);
	QString current = QDir::currentPath();
	if (commandLineFilename && verifyLDrawDir())
	{
		QUrl qurl(commandLineFilename);
		if (qurl.scheme()=="file")
		{
			commandLineFilename=copyString(QUrl::fromPercentEncoding(qurl.toLocalFile().toUtf8().constData()).toUtf8().constData());
		}	
		QFileInfo fi(commandLineFilename);
		commandLineFilename = copyString(fi.absoluteFilePath().toUtf8().constData());
//		loadModel(commandLineFilename);
		if (chDirFromFilename(commandLineFilename))
		{
			modelViewer->setFilename(commandLineFilename);
//			modelViewer->loadModel();
			if (modelViewer->loadModel())
			{
				getFileInfo(commandLineFilename, lastWriteTime, lastFileSize);
				if (lastWriteTime.isValid())
				{
					startPollTimer();
				}
				setLastOpenFile(commandLineFilename);
				mainWindow->populateRecentFileMenuItems();
				mainWindow->setupStandardSizes();
				mainWindow->fileSaveSetEnabled(true);
				mainWindow->fileReloadSetEnabled(true);
				mainWindow->toolbarViewAngleSetEnabled(true);
				startPaintTimer();
				updateStep();
			}
		}
	}
	if (snapshotFilename)
	{
		if (snapshotFilename)
		{
			LDConsoleAlertHandler *consoleAlertHandler = LDSnapshotTaker::getConsoleAlertHandler();
			QDir::setCurrent(current);
			QFileInfo fi(snapshotFilename);
			QString s(snapshotFilename);
			char *s2=copyString(fi.absoluteFilePath().toUtf8().constData());
		
			QString ext = s.toLower().right(4);
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
			TCObject::release(consoleAlertHandler);
		}
		shouldExit = true;
	}
	if (shouldExit)
	{
		exit(0);
	}
}

void ModelViewerWidget::initializeGL(void)
{
	lock();
	TREGLExtensions::setup();
	preferences->doCancel();
	doViewStatusBar(preferences->getStatusBar());
	doViewToolBar(preferences->getToolBar());
	if (saving || printing)
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
	if (!loading && !saving && !printing)
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

void ModelViewerWidget::swap_Buffers(void)
{
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
	glDisable(GL_MULTISAMPLE_ARB);
	glDrawBuffer(GL_FRONT);
//	drawFPS();
	glDrawBuffer(GL_BACK);
	glFlush();
	glEnable(GL_MULTISAMPLE_ARB);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
}

void ModelViewerWidget::paintGL(void)
{
	lock();
	if (!isFboActive() && !painting && (saving || printing || !loading))
	{
		painting = true;
		glEnable(GL_DEPTH_TEST);
		if (saving || printing)
		{
			if (!TREGLExtensions::haveFramebufferObjectExtension())
			{
				glDrawBuffer(GL_BACK);
				glReadBuffer(GL_BACK);
			}
			if (saving) {
				saveImageResult = snapshotTaker->saveImage(saveImageFilename,
					saveImageWidth, saveImageHeight, saveImageZoomToFit);
			}
			if (printing) {
			}
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
		//app->lock();
	}
	lockCount++;
}

void ModelViewerWidget::unlock(void)
{
	lockCount--;
	if (lockCount == 0)
	{
		//app->unlock();
	}
}

void ModelViewerWidget::setLibraryUpdateProgress(float progress)
{
	libraryUpdateWindow->setValue((int)(progress * 100));
}

void ModelViewerWidget::timerEvent(QTimerEvent* event)
{
	lock();
	if (event->timerId() == paintTimer)
	{
		if (!loading)
		{
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
			update();
#else
			updateGL();
#endif
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
				libraryUpdateProgressMessage = "";
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
	if (loading && !saving && !printing)
	{
		int r, g, b;

		preferences->getBackgroundColor(r, g, b);

// former Qt bug 79310 caused problem with the next 2 lines

		QPainter painter(this);
		painter.fillRect(event->rect(), QColor(r, g, b));
	}
	else if (!saving && !printing)
	{
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
		QOpenGLWidget::paintEvent(event);
#else
		QGLWidget::paintEvent(event);
#endif
	}
	unlock();
}

void ModelViewerWidget::preLoad(void)
{
	clearErrors();
	makeCurrent();
	modelViewer->clearBackground();
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
	update();
#else
	glDraw();
#endif
}

void ModelViewerWidget::postLoad(void)
{
	makeCurrent();
	resizeGL(width(), height());
	startPaintTimer();
	mainWindow->fileSaveSetEnabled(true);
	mainWindow->fileReloadSetEnabled(true);
	mainWindow->toolbarViewAngleSetEnabled(true);
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
//	printer->setOptionEnabled(QPrinter::PrintSelection,false);
//	printer->setOptionEnabled(QPrinter::PrintPageRange,false);
	printer->setColorMode(QPrinter::Color);
//	printer->setFullPage(true);
#if QT_VERSION >= 0x40400
#if QT_VERSION >= 0x50300
	printer->setPageMargins(QMarginsF(
		TCUserDefaults::longForKey(LEFT_MARGIN_KEY, 500, false) / 1000.0f,
		TCUserDefaults::longForKey(TOP_MARGIN_KEY, 500, false) / 1000.0f,
		TCUserDefaults::longForKey(RIGHT_MARGIN_KEY, 500, false) / 1000.0f,
		(qreal)(TCUserDefaults::longForKey(BOTTOM_MARGIN_KEY, 500, false) / 1000.0f)),
		QPageLayout::Inch);
	printer->setPageOrientation((QPageLayout::Orientation)TCUserDefaults::longForKey(ORIENTATION_KEY,0,false));
	printer->setPageSize(QPageSize((QPageSize::PageSizeId)TCUserDefaults::longForKey(PAPER_SIZE_KEY,0,false)));
#else
	printer->setPageMargins(
		TCUserDefaults::longForKey(LEFT_MARGIN_KEY, 500, false) / 1000.0f,
		TCUserDefaults::longForKey(TOP_MARGIN_KEY, 500, false) / 1000.0f,
		TCUserDefaults::longForKey(RIGHT_MARGIN_KEY, 500, false) / 1000.0f,
		(qreal)(TCUserDefaults::longForKey(BOTTOM_MARGIN_KEY, 500, false) / 1000.0f),
		QPrinter::Inch);
	printer->setOrientation((QPrinter::Orientation)TCUserDefaults::longForKey(ORIENTATION_KEY,0,false));
	printer->setPaperSize((QPrinter::PaperSize)TCUserDefaults::longForKey(PAPER_SIZE_KEY,0,false));
#endif
#endif
	QPrintDialog *printdialog = new QPrintDialog(printer);
	if (printdialog)
	{
#if QT_VERSION >= 0x60000
		printdialog->setOption(QAbstractPrintDialog::PrintToFile);
		printdialog->setOption(QAbstractPrintDialog::PrintShowPageSize);
#else
		printdialog->setEnabledOptions(
					QAbstractPrintDialog::PrintToFile
#if QT_VERSION >= 0x40400
					| QAbstractPrintDialog::PrintShowPageSize
#endif
);
#endif
		printdialog->setMinMax(1,1);
		if (printdialog->exec() != QDialog::Accepted) return;

#if QT_VERSION >= 0x40400
		qreal	*left  = new qreal,
				*right = new qreal,
				*top   = new qreal,
				*bottom= new qreal;
#if QT_VERSION >= 0x50300
		TCUserDefaults::setLongForKey((long)printer->pageLayout().pageSize().id(),PAPER_SIZE_KEY,false);
		TCUserDefaults::setLongForKey((long)printer->pageLayout().orientation(), ORIENTATION_KEY, false);
		QMarginsF margins = printer->pageLayout().margins(QPageLayout::Inch);
		*left = margins.left();
		*right= margins.right();
		*top  = margins.top();
		*bottom=margins.bottom();
#else
		TCUserDefaults::setLongForKey((long)printer->paperSize(),PAPER_SIZE_KEY,false);
		TCUserDefaults::setLongForKey((long)printer->orientation(), ORIENTATION_KEY, false);
		printer->getPageMargins(left,top,right,bottom,QPrinter::Inch);
#endif

		TCUserDefaults::setLongForKey((long)(*left*1000),LEFT_MARGIN_KEY,false);
		TCUserDefaults::setLongForKey((long)(*right*1000),RIGHT_MARGIN_KEY,false);
		TCUserDefaults::setLongForKey((long)(*top*1000),TOP_MARGIN_KEY,false);
		TCUserDefaults::setLongForKey((long)(*bottom*1000),BOTTOM_MARGIN_KEY,false);
#endif
		QPainter p;
		if (!p.begin(printer))
			return;
//		QPaintDeviceMetrics metrics (p.device());
		int dpix = p.device()->logicalDpiX(),
			dpiy = p.device()->logicalDpiY(),
			marginx = (int) (2/2.54)*dpix,
			marginy = (int) (2/2.54)*dpiy,
			pwidth = p.device()->width()-2*marginx,
			pheight = p.device()->height()-2*marginy,
			bytesPerLine;
		long	y, x;
//		printf("%ix%i %ix%i DPI\n",pwidth,pheight,dpix,dpiy);
		int r, g, b;
		preferences->getBackgroundColor(r, g, b);
		modelViewer->setBackgroundRGB(255,255,255);
		if (dpix != dpiy)
			modelViewer->setPixelAspectRatio((float)dpix / dpiy);
		saveImageType = BMP_IMAGE_TYPE_INDEX;
		TCByte *buffer = grabImage(pwidth,pheight,NULL,true);
		QImage *image = new QImage(pwidth,pheight,QImage::Format_RGB32);
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
		delete [] path;
	}
	return retValue;
}

void ModelViewerWidget::finishLoadModel(void)
{
	char *filename = modelViewer->getFilename();

	preLoad();
	if (modelViewer->loadModel())
	{
		getFileInfo(filename, lastWriteTime, lastFileSize);
		if (lastWriteTime.isValid())
		{
			startPollTimer();
		}
		setLastOpenFile(filename);
		mainWindow->populateRecentFileMenuItems();
		mainWindow->setupStandardSizes();
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

#if QT_VERSION < QT_VERSION_CHECK(5,5,0)
		message.sprintf("The directory containing the file %s could not be found.",
			filename);
#else
		message = QString::asprintf("The directory containing the file %s could not be found.",
			filename);
#endif
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
		QString selectedfile = QFileDialog::getOpenFileName(this,"Choose a Model",".",
						"All LDraw Files (*.ldr *.dat *.mpd);;"
						"LDraw Models (*.ldr *.dat);;Multi-part Models (*.mpd);;All Files (*)");
		if(!selectedfile.isEmpty())
		{
			QString filename = selectedfile.replace("\\","/");
			QDir dir(filename);
			QDir::setCurrent(dir.path().replace("\\","/"));
			Preferences::setLastOpenPath(dir.path().replace("\\","/").toUtf8().constData());
			loadModel(filename.toUtf8().constData());
		}
	}
	unlock();
}

void ModelViewerWidget::setLastOpenFile(const char *filename)
{
	if (mainWindow->recentFiles)
	{
		int index = mainWindow->recentFiles->indexOfString(filename);

		mainWindow->recentFiles->insertString(filename);
		if (index >= 0)
		{
			// Insert before removal.  Since the one being removed could have the same
			// pointer value as the string in the array, we could otherwise access a
			// pointer after it had been deleted.
			mainWindow->recentFiles->removeStringAtIndex(index + 1);
		}
		mainWindow->recordRecentFiles();
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
	if (!inputHandler->mouseDown(convertKeyModifiers(event->modifiers()),
		convertMouseButton(event->button()),
#if QT_VERSION >= 0x60000
		event->globalPosition().x(),event->globalPosition().y()
#else
		event->globalX(),event->globalY()
#endif
		))
	{
		event->ignore();
	}
	unlock();
}

void ModelViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseUp(convertKeyModifiers(event->modifiers()),
		convertMouseButton(event->button()),
#if QT_VERSION >= 0x60000
		event->globalPosition().x(),event->globalPosition().y()
#else
		event->globalX(), event->globalY()
#endif
		))
	{
		event->ignore();
	}
	unlock();
}

void ModelViewerWidget::wheelEvent(QWheelEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseWheel(convertKeyModifiers(event->modifiers()),
#if QT_VERSION >= 0x50000
		(TCFloat)event->angleDelta().y() * 0.5f))
#else
		(TCFloat)event->delta() * 0.5f))
#endif
	{
		event->ignore();
	}
	unlock();
}

void ModelViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
	lock();
	if (loading)
	{
		unlock();
		return;
	}
	if (!inputHandler->mouseMove(convertKeyModifiers(event->modifiers()),
#if QT_VERSION >= 0x60000
		event->globalPosition().x(),event->globalPosition().y()
#else
		event->globalX(), event->globalY()
#endif
		))
	{
		event->ignore();
	}
	unlock();
}

void ModelViewerWidget::showPreferences(void)
{
	preferences->show();
}

void ModelViewerWidget::doLibraryUpdateFinished(int finishType)
{
#if !defined(_NO_BOOST) || defined(USE_CPP11)
	if (libraryUpdater)
	{
		QString statusText;

		libraryUpdateWindow->setCancelButtonText(QString::fromWCharArray(TCLocalStrings::get(L"OK")));
		setLibraryUpdateProgress(1.0f);
		if (libraryUpdater->getError() && ucstrlen(libraryUpdater->getError()))
		{
			QString qError;

			statusText = QString::fromWCharArray(TCLocalStrings::get(L"LibraryUpdateError"));
			statusText += ":\n";
			ucstringtoqstring(qError, libraryUpdater->getError());
			statusText += qError;
		}
		switch (finishType)
		{
		case LIBRARY_UPDATE_FINISHED:
			libraryUpdateFinished = true;
			statusText = QString::fromWCharArray(TCLocalStrings::get(L"LibraryUpdateComplete"));
			break;
		case LIBRARY_UPDATE_CANCELED:
			statusText = QString::fromWCharArray(TCLocalStrings::get(L"LibraryUpdateCanceled"));
			break;
		case LIBRARY_UPDATE_NONE:
			statusText = QString::fromWCharArray(TCLocalStrings::get(L"LibraryUpdateUnnecessary"));
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
#if !defined(_NO_BOOST) || defined(USE_CPP11)
	if (!libraryUpdateWindow)
	{
		createLibraryUpdateWindow();
	}
	libraryUpdateWindow->setCancelButtonText(QString::fromWCharArray(TCLocalStrings::get(L"Cancel")));
	libraryUpdateWindow->reset();
	libraryUpdateWindow->show();
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
						QString::fromWCharArray(TCLocalStrings::get(L"CheckingForUpdates")),
						QString::fromWCharArray(TCLocalStrings::get(L"Cancel")),
						0,100,mainWindow);
		libraryUpdateWindow->setMinimumDuration(0);
		libraryUpdateWindow->setAutoReset(false);
	}
}

bool ModelViewerWidget::installLDraw(void)
{
#if !defined(_NO_BOOST) || defined(USE_CPP11)
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
		char *ldrawZip = copyString(ldrawParentDir, 255);

		QDir originalDir = QDir::current();
		bool progressDialogClosed = false;

		libraryUpdateFinished = false;
		strcat(ldrawDir, "/ldraw");
		strcat(ldrawZip, "/ldraw/complete.zip");

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
			LDLModel::setLDrawZipPath(ldrawZip);
			preferences->setLDrawZipPath(ldrawZip);
		}
		delete ldrawDir;
		delete ldrawZip;
		return libraryUpdateFinished;
	}
#endif // _NO_BOOST
}

void ModelViewerWidget::checkForLibraryUpdates(void)
{
#if !defined(_NO_BOOST) || defined(USE_CPP11)
	showLDrawZipMsg = TCUserDefaults::boolForKey(LDRAW_ZIP_SHOW_WARNING_KEY, true, false);
	if (Preferences::getLDrawZipPath() && showLDrawZipMsg)
	{
		QMessageBox mb;
		QString title,message,zipPath=Preferences::getLDrawZipPath();
		message=QString::fromWCharArray(TCLocalStrings::get(L"ReplaceLDrawZipMessage"));
		message.replace(QString("%s"),zipPath);
		mb.setText(message);
		mb.setWindowTitle(QString::fromWCharArray(TCLocalStrings::get(L"ReplaceLDrawZipTitle")));
		mb.addButton(QMessageBox::Yes);
		mb.addButton(QMessageBox::No);
		mb.setIcon(QMessageBox::Icon::Question);
		mb.setDefaultButton(QMessageBox::No);
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
		QCheckBox *cb = new QCheckBox("In the future do not show this message");
		mb.setCheckBox(cb);
#if QT_VERSION >= QT_VERSION_CHECK(6,7,0)
		QObject::connect(cb, &QCheckBox::checkStateChanged, [this](Qt::CheckState state){
		this->showLDrawZipMsg = (state != Qt::CheckState::Checked); });
#else
		QObject::connect(cb, &QCheckBox::stateChanged, [this](int state){
		this->showLDrawZipMsg = (static_cast<Qt::CheckState>(state) != Qt::CheckState::Checked); });
#endif
#endif
		mb.exec();
		TCUserDefaults::setBoolForKey(showLDrawZipMsg, LDRAW_ZIP_SHOW_WARNING_KEY, false);
		if (mb.result()==QMessageBox::No) { return;}
	}
	if (libraryUpdater)
	{
		showLibraryUpdateWindow(false);
	}
	else
	{
		libraryUpdater = new LDLibraryUpdater;
		char *ldrawDir = getLDrawDir();
		char *ldrawZip = Preferences::getLDrawZipPath();
		wchar_t *updateCheckError = NULL;

		libraryUpdateCanceled = false;
		libraryUpdateFinishNotified = false;
		libraryUpdateFinished = false;
		libraryUpdater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
		libraryUpdater->setLdrawDir(ldrawDir);
		libraryUpdater->setLdrawZipPath(ldrawZip);
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

void ModelViewerWidget::setMainWindow(LDViewMainWindow *value)
{
	int width, height;

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
	statusBar = mainWindow->statusBar();
//	toolBar = new QToolBar;
	reflectSettings();
	mainWindow->fileSaveSetEnabled(false);
	mainWindow->fileReloadSetEnabled(false);
	progressBar = new QProgressBar(statusBar);
	progressLabel = new QLabel(statusBar);
	progressLatlong = new QLabel(statusBar);
	progressMode = new QLabel(statusBar);
	progressBar->setTextVisible(false);
	statusBar->addWidget(progressBar);
	statusBar->addWidget(progressLabel, 1);
	statusBar->addWidget(progressLatlong);
	statusBar->addWidget(progressMode);
	mainWindow->setStatusbarOn(preferences->getStatusBar());
	mainWindow->setToolbarOn(preferences->getToolBar());
	mainWindow->setPollAction(Preferences::getPollMode());
	if (viewMode == LDInputHandler::VMExamine)
	{
		mainWindow->setExamineModeOn(true);
		progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"ExamineMode")));
	}
	else if (viewMode == LDInputHandler::VMFlyThrough)
	{
		mainWindow->setFlythroughModeOn(true);
		progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"FlyThroughMode")));
	}
	else if (viewMode == LDInputHandler::VMWalk)
	{
		mainWindow->setWalkModeOn(true);
		progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"WalkMode")));
	}
	mainWindow->setViewLatitudeRotationOn(Preferences::getLatLongMode());
	mainWindow->setKeepRightSideUpOn(keepRightSide = Preferences::getKeepRightSideUp());
	mainWindow->setShowPovAspectRatioOn(
			Preferences::getPovAspectRatio());
	saveAlpha = TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0;
	updateStep();
	unlock();
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
		char *filename = mainWindow->recentFiles->stringAtIndex(index);

		if (filename)
		{
			QFile file (filename);
			if(!file.exists())
			{
				QString message;
				message = QString::fromWCharArray(TCLocalStrings::get(L"ErrorLoadingModel"));
				message.replace(QString("%s"),QString(filename));
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
	progressBar->setValue(0);
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
		progressBar->setValue((int)(progress * 100));
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
	mainWindow->setupStandardSizes();
}

void ModelViewerWidget::doViewToolBar(bool flag)
{
	lock();
	mainWindow->showToolbar(flag);
	preferences->setToolBar(flag);
	unlock();
	mainWindow->setupStandardSizes();
}

void ModelViewerWidget::doViewFullScreen(void)
{
	static QPoint pos;
	static QSize size;
	if (!fullscreen)
	{
		pos=mainWindow->pos();
		size=mainWindow->size();
		statusBar->hide();
		//mainWindow->setMainGroupBoxMargin( 0 );
		mainWindow->showToolbar(false);
		mainWindow->showMenubar(false);
		mainWindow->showFullScreen();
		fullscreen=1;
	} else
	{
		//mainWindow->setMainGroupBoxMargin( 2 );
		mainWindow->showNormal();
		mainWindow->resize(size);
		mainWindow->move(pos);
		mainWindow->showMenubar(true);
		if(preferences->getStatusBar()) {statusBar->show();}
		if(preferences->getToolBar()) {mainWindow->showToolbar(true);}
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
		setViewMode (viewMode, examineLatLong, keepRightSide);
		if (b && Preferences::getViewMode() == LDInputHandler::VMExamine)
			progressLatlong->setHidden(false);
		else
			progressLatlong->setHidden(true);
	}
	unlock();
}

void ModelViewerWidget::keepRightSideUp(bool b)
{
	lock();
	if (keepRightSide != b)
	{
		preferences->setKeepRightSideUp(keepRightSide = b);
		modelViewer->setKeepRightSideUp(b);
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

		extensionsPanel = new OpenGLExtensions(mainWindow);
		ucstringtoqstring(openGLDriverInfo, temp);
		extensionsPanel->setText(openGLDriverInfo);
		extensionsCountLabel = new QLabel(extensionsPanel->statusBar());
		countString = QString::number(extensionCount);
		countString += QString::fromWCharArray((TCLocalStrings::get(L"OpenGlnExtensionsSuffix")));
		extensionsCountLabel->setText(countString);
		extensionsPanel->statusBar()->addWidget(extensionsCountLabel, 1);
	}
	extensionsPanel->show();
	extensionsPanel->raise();
//	extensionsPanel->setActiveWindow();
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
	QString helpFilename = findPackageFile(TCLocalStrings::get("HelpHtml"));
	QString qUrl = QString("file://") + helpFilename;
#ifdef __APPLE__
	helpFilename += "#MacNotes";
	CFURLRef url = NULL;
	CFStringRef urlString;
	bool macSuccess = false;

	urlString = CFStringCreateWithCString(NULL, qUrl.toUtf8(),
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
/*
	if (FSPathMakeRef((const UInt8 *)(const char *)helpFilename, &fsRef,
		&isDirectory) == 0 && !isDirectory)
	{
		if (LSOpenFSRef(&fsRef, NULL) == 0)
		{
			return;
		}
	}
*/
#endif // __APPLE__
	QFile file(helpFilename);
	if (!file.exists())
	{
		return;
	}
	if(!helpContents)
	{
		helpContents = new Help(mainWindow);
		if ( file.open( QIODevice::ReadOnly ) ) {
			QTextStream stream( &file );
			helpContents->setText(
				stream.readAll().replace(
#if QT_VERSION >= 0x60000
					QRegularExpression("(BGCOLOR|COLOR|TEXT|LINK)="),
#else
					QRegExp("(BGCOLOR|COLOR|TEXT|LINK)="),
#endif
												"i=") );
		}
	}
	if (!QDesktopServices::openUrl(helpFilename))
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
		aboutPanel->resize(10, 10);
		QString text = aboutPanel->getText();
		text.replace( "__DATE__",__DATE__);
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

void ModelViewerWidget::doWireframeFog(bool value)
{
	preferences->setUseWireframeFog(value);
	doApply();
}

void ModelViewerWidget::doWireframeRemoveHiddenLines(bool value)
{
	preferences->setRemoveHiddenLines(value);
	doApply();
}

void ModelViewerWidget::doTextureStud(bool value)
{
	preferences->setTextureStud(value);
	doApply();
}

void ModelViewerWidget::doShowEdgeOnly(bool value)
{
	preferences->setEdgeOnly(value);
	doApply();
}

void ModelViewerWidget::doConditionalLine(bool value)
{
	preferences->setConditionalLine(value);
	doApply();
}

void ModelViewerWidget::doHighQuality(bool value)
{
	preferences->setHighQuality(value);
	doApply();
}

void ModelViewerWidget::doAlwaysBlack(bool value)
{
	preferences->setAlwaysBlack(value);
	doApply();
}

void ModelViewerWidget::doRedBackFaces(bool value)
{
	preferences->setRedBackFaces(value);
	doApply();
}

void ModelViewerWidget::doGreenFrontFaces(bool value)
{
	preferences->setGreenFrontFaces(value);
	doApply();
}

void ModelViewerWidget::doBlueNeutralFaces(bool value)
{
	preferences->setBlueNeutralFaces(value);
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
	if (mainWindow && preferences)
	{
		mainWindow->setToolbarWireframeOn(preferences->getDrawWireframe());
		mainWindow->setToolbarWireframeFogOn(preferences->getUseWireframeFog());
		mainWindow->setToolbarWireframeRemoveHiddenLinesOn(preferences->getRemoveHiddenLines());
		mainWindow->setToolbarTextureStudOn(preferences->getTextureStud());
		mainWindow->setToolbarEdgeShowEdgeOnlyOn(preferences->getEdgeOnly());
		mainWindow->setToolbarEdgeConditionalLineOn(preferences->getConditionalLine());
		mainWindow->setToolbarEdgeHighQualityOn(preferences->getHighQuality());
		mainWindow->setToolbarEdgeAlwaysBlackOn(preferences->getAlwaysBlack());
		mainWindow->setToolbarBfcRedBackFacesOn(preferences->getRedBackFaces());
		mainWindow->setToolbarBfcGreenFrontFacesOn(preferences->getGreenFrontFaces());
		mainWindow->setToolbarBfcBlueNeutralFacesOn(preferences->getBlueNeutralFaces());
		mainWindow->setToolbarEdgeOn(preferences->getShowsHighlightLines());
		mainWindow->setToolbarLightingOn(preferences->getUseLighting());
		mainWindow->setToolbarBFCOn(preferences->getUseBFC());
		mainWindow->setToolbarAxesOn(preferences->getShowAxes());
		mainWindow->setToolbarSeamsOn(preferences->getUseSeams());
		mainWindow->setToolbarPrimitiveSubstitutionOn(preferences->getAllowPrimitiveSubstitution());
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
#if QT_VERSION < QT_VERSION_CHECK(5,5,0)
				fpsString.sprintf(TCLocalStrings::get("FPSFormat"), fps);
#else
				fpsString = QString::asprintf(TCLocalStrings::get("FPSFormat"), fps);
#endif
			}
			else
			{
				fpsString = QString::fromWCharArray(TCLocalStrings::get(L"FPSSpinPrompt"));
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
#if QT_VERSION < QT_VERSION_CHECK(5,5,0)
		latlongstring.sprintf(TCLocalStrings::get("LatLonFormat"),lat,lon);
#else
		latlongstring = QString::asprintf(TCLocalStrings::get("LatLonFormat"),lat,lon);
#endif
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
	QString currentDir = QDir::currentPath();
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
				QDir::setCurrent(value);
				if (QDir::current().exists("complete.zip") && !Preferences::getLDrawZipPath())
				{
					char *ldrawZip = copyString(value, 255);
					strcat(ldrawZip, "/complete.zip");
					LDLModel::setLDrawZipPath(ldrawZip);
					preferences->setLDrawZipPath(ldrawZip);
					delete ldrawZip;
				}
			}
		}
		QDir::setCurrent(currentDir);
		if (Preferences::getLDrawZipPath())
		{
			LDLModel::setLDrawDir(value);
			return true;
		}
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
		if (commandLineSnapshotSave) return true;
		bool ans = true;
		if (!verifyLDrawDir(lDrawDir))
		{
			ans = (QMessageBox::question(this, "LDView",
				QString::fromWCharArray(TCLocalStrings::get(L"LDrawDirExistsPrompt")),
#if QT_VERSION >= QT_VERSION_CHECK(6,2,0)
					QMessageBox::Yes | QMessageBox::No , QMessageBox::No
#else
					QMessageBox::Yes, QMessageBox::No | QMessageBox::Default
#endif
				) == QMessageBox::Yes);
		}
		delete lDrawDir;
		if (ans)
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
							QString::fromWCharArray(TCLocalStrings::get(L"InvalidDir")),
							QString::fromWCharArray(TCLocalStrings::get(L"LDrawNotInDir")),
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
				"LDView", QString::fromWCharArray(TCLocalStrings::get(L"WillDownloadLDraw")),
				QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
			{
				LDLModel::setLDrawDir("/");
				if (promptForLDrawDir(
					QString::fromWCharArray(TCLocalStrings::get(L"LDrawInstallDirPrompt"))))
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
			lDrawDir = copyString("/usr/share/ldraw");
		}
	}
	stripTrailingPathSeparators(lDrawDir);
	return lDrawDir;
}

bool ModelViewerWidget::promptForLDrawDir(QString prompt)
{
	char *initialDir = getLDrawDir();
	bool retValue = false;

	if (prompt.isEmpty())
	{
		prompt = QString::fromWCharArray(TCLocalStrings::get(L"LDrawDirPrompt"));
	}
	QDir::setCurrent(initialDir);
	QString selectedfile=QFileDialog::getExistingDirectory(this,prompt,".");
	if (!selectedfile.isEmpty())
	{
		Preferences::setLDrawDir(selectedfile.toUtf8().constData());
		retValue = true;
	}
	return retValue;
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
		errors = new LDViewErrors(mainWindow, preferences);
	}
	errors->addError(error);
	return 1;
}

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
		errors = new LDViewErrors(mainWindow, preferences);
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

void ModelViewerWidget::getFileInfo(const char *filename, QDateTime &value, qint64 &size)
{
	if (filename)
	{
		if (!fileInfo)
		{
			fileInfo = new QFileInfo;
		}
		fileInfo->setFile(filename);

		value = QDateTime(fileInfo->lastModified());
		size = fileInfo->size();
	}
	else
	{
		value = QDateTime();
		size = 0;
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
			qint64 newFileSize;

			getFileInfo(filename, newWriteTime, newFileSize);
			if (newWriteTime != lastWriteTime)
			{
				bool update = true;

				if(newFileSize != lastFileSize)
				{
					startPollTimer();
					lastFileSize = newFileSize;
					return;
				}
				lastFileSize = 0;
				lastWriteTime = QDateTime(newWriteTime);
				if (Preferences::getPollMode() == LDVPollPrompt)
				{
					if (QMessageBox::information(this, QString::fromWCharArray(TCLocalStrings::get(L"PollFileUpdate")),
						QString::fromWCharArray(TCLocalStrings::get(L"PollReloadCheck")),
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
#if QT_VERSION < 0x50000
	QGLWidget::windowActivationChange(oldActive);
#else
	Q_UNUSED(oldActive)
#endif
}

void ModelViewerWidget::doPollChanged(LDVPollMode newMode)
{
	lock();
	Preferences::setPollMode(newMode);
	killPollTimer();
	startPollTimer(true);
	unlock();
}

void ModelViewerWidget::setViewMode(LDInputHandler::ViewMode value,
bool examine, bool keep, bool /*saveSettings*/)
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
			progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"ExamineMode")));
		}
		if (progressLatlong)
		{
			progressLatlong->setHidden(!examine);
		}
		modelViewer->setExamineMode(examineMode);
	}
	else if (viewMode == LDInputHandler::VMFlyThrough)
	{
		inputHandler->setViewMode(LDInputHandler::VMFlyThrough);
		modelViewer->setConstrainZoom(false);
		modelViewer->setKeepRightSideUp(keep);
		if (progressMode)
		{
			progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"FlyThroughMode")));
		}
		if (progressLatlong) progressLatlong->setHidden(true);
	}
	else if (viewMode == LDInputHandler::VMWalk)
	{
		inputHandler->setViewMode(LDInputHandler::VMWalk);
		modelViewer->setKeepRightSideUp(true);
		if (progressMode)
		{
			progressMode->setText(QString::fromWCharArray(TCLocalStrings::get(L"WalkMode")));
		}
	}
	Preferences::setViewMode(viewMode);
}

void ModelViewerWidget::doViewModeChanged(LDInputHandler::ViewMode newMode)
{
	lock();
	setViewMode(newMode,examineLatLong, keepRightSide);
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

void ModelViewerWidget::doRightSideUp(void)
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
	modelViewer->rightSideUp();
	startPaintTimer();
	unlock();
}

void ModelViewerWidget::doCameraLocation(void)
{
	cameralocation->show();
}

void ModelViewerWidget::doRotationCenter(void)
{
	rotationcenter->show();
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
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#ifndef QOPENGLWIDGET
	if (fbo == NULL && (TCUserDefaults::longForKey(IGNORE_FRAMEBUFFER_OBJECT_KEY, 0, false)==0))
	{
		QOpenGLFramebufferObjectFormat fboFormat;
		GLsizei fboSize = 1024;
		fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		fbo = new QOpenGLFramebufferObject(fboSize, fboSize, fboFormat);
		saving = true;
		if (fbo->isValid() && fbo->bind())
		{
			snapshotTaker->setUseFBO(false);
			glViewport(0, 0, fboSize, fboSize);
			if (modelViewer->getMainTREModel() == NULL && !modelViewer->getNeedsReload())
			{
				modelViewer->loadModel(true);
			}
			inputHandler->stopRotation();
			bool retValue = snapshotTaker->saveImage(saveImageFilename, imageWidth, imageHeight, saveImageZoomToFit);
			fbo->release();
			delete fbo;
			fbo = NULL;
			saving = false;
			return retValue;
		}
		else
		{
			delete fbo;
			fbo = NULL;
			saving = false;
		}
	}
#endif
#endif
	if (!fromCommandLine)
	{
		saving = true;
		makeCurrent();
		if (modelViewer->getMainTREModel() == NULL && !modelViewer->getNeedsReload())
		{
			modelViewer->loadModel(true);
		}
		bool retValue = snapshotTaker->saveImage(saveImageFilename, imageWidth, imageHeight, saveImageZoomToFit);
		saving = false;
		return retValue;
	}
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
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
//		Code to be added for renderPixmap functionality
//		Without this code saved snapshot image is corrupted/garbage
#else
		renderPixmap(newWidth, newHeight);
#endif
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
									
									

TCByte *ModelViewerWidget::grabImage(
	int &imageWidth,
	int &imageHeight,
	TCByte *buffer, bool /*zoomToFit*/,
	bool * /*saveAlpha*/)
{
	int newWidth = 800;
	int newHeight = 600;
	int origWidth = mwidth;
	int origHeight = mheight;
	int numXTiles, numYTiles;
	bool origSlowClear = modelViewer->getSlowClear();
	int origMemoryUsage = modelViewer->getMemoryUsage();
	bool sa=false;
	TCByte *bufferReturn = NULL;
	if (!snapshotTaker)
	{
		snapshotTaker = new LDSnapshotTaker(modelViewer);
	}
#ifndef QOPENGLWIDGET
	if (TREGLExtensions::haveFramebufferObjectExtension())
	{
		snapshotTaker->setUseFBO(true);
	}
#else
	snapshotTaker->setUseFBO(false);
#endif
	snapshotTaker->setImageType(getSaveImageType());
	snapshotTaker->setTrySaveAlpha(saveAlpha =
		TCUserDefaults::longForKey(SAVE_ALPHA_KEY, 0, false) != 0);
	snapshotTaker->setAutoCrop(
		TCUserDefaults::boolForKey(AUTO_CROP_KEY, false, false));

	printing = true;
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
			bufferReturn = snapshotTaker->grabImage(saveImageWidth, saveImageHeight, saveImageZoomToFit,buffer,&sa);
	}
	makeCurrent();
	TREGLExtensions::setup();
	if (!snapshotTaker->getUseFBO())
	{
		modelViewer->openGlWillEnd();
	}
	printing = false;
	mwidth = origWidth;
	mheight = origHeight;
	modelViewer->setWidth(mwidth);
	modelViewer->setHeight(mheight);
	modelViewer->setMemoryUsage(origMemoryUsage);
	modelViewer->setSlowClear(origSlowClear);
	modelViewer->setup();
	doApply();
	return bufferReturn;
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
				snprintf(format, sizeof(format), "%%s%%0%dd.%%s", saveDigits);
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
							suffix.toUtf8().constData(), 1, modelViewer->getNumSteps());
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
//	QStringList::const_iterator exportFilterIt;
	QPushButton *exportDialogOptions = new QPushButton("Options...");

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
		saveDialog = new QFileDialog(this,QString::fromWCharArray(TCLocalStrings::get(L"ExportModel")),".");
		saveDialog->setWindowIcon(QPixmap( ":/images/images/LDViewIcon16.png"));
#if QT_VERSION < 0x40400
		saveDialog->setFilters(exportFilters);
		saveDialog->selectFilter(saveDialog->filters().at(exportType - LDrawModelViewer::ETFirst));
#else
		saveDialog->setNameFilters(exportFilters);
		saveDialog->selectNameFilter(exportFilters.at(exportType - LDrawModelViewer::ETFirst));
#endif
		saveDialog->setFileMode(QFileDialog::AnyFile);
		saveDialog->setAcceptMode(QFileDialog::AcceptSave);
		saveDialog->setLabelText(QFileDialog::Accept,"Export");
		saveDialog->setOption(QFileDialog::DontUseNativeDialog);
		saveDialog->layout()->addWidget(exportDialogOptions);
		connect(exportDialogOptions, SIGNAL(clicked()),this, SLOT( fileExportOptionButton()));
		break;
	case LDPreferences::SOSnapshot:
	default:
		saveDialog = new QFileDialog(this,QString::fromWCharArray(TCLocalStrings::get(L"SaveSnapshot")),".",
			"Portable Network Graphics (*.png);;Windows Bitmap (*.bmp);;Jpeg (*.jpg)");
#if QT_VERSION < 0x40400
		saveDialog->selectFilter(saveDialog->filters().at(saveImageType-1));
#else
		saveDialog->selectNameFilter(saveDialog->nameFilters().at(saveImageType-1));
#endif
		saveDialog->setWindowIcon(QPixmap( ":/images/images/LDViewIcon16.png"));
		saveDialog->setFileMode(QFileDialog::AnyFile);
		saveDialog->setAcceptMode(QFileDialog::AcceptSave);
		saveDialog->setLabelText(QFileDialog::Accept,"Save");
		saveDialog->setOption(QFileDialog::DontConfirmOverwrite);
		break;
	}
	saveDialog->selectFile(saveFilename);
	if (saveDialog->exec() == QDialog::Accepted)
	{
		QString selectedfile="";
		if (!saveDialog->selectedFiles().isEmpty())
		{
			selectedfile = saveDialog->selectedFiles()[0];
		}
		QString filename = selectedfile, dir = saveDialog->directory().path();
		switch (curSaveOp)
		{
		case LDPreferences::SOExport:
			TCUserDefaults::setPathForKey(dir.toUtf8().constData(), LAST_EXPORT_DIR_KEY, false);
			break;
		case LDPreferences::SOSnapshot:
		default:
			TCUserDefaults::setPathForKey(dir.toUtf8().constData(), LAST_SNAPSHOT_DIR_KEY, false);
			break;
		}
		QDir::setCurrent(dir);
		strncpy(saveFilename,filename.toUtf8().constData(),len);
#if QT_VERSION < 0x40400
		QString filter = saveDialog->selectedFilter();
#else
		QString filter = saveDialog->selectedNameFilter();
#endif
		if (filter.indexOf(".png") != -1)
		{
			saveImageType = PNG_IMAGE_TYPE_INDEX;
		}
		if (filter.indexOf(".jpg") != -1)
		{
			saveImageType = JPG_IMAGE_TYPE_INDEX;
		}
		if (filter.indexOf(".bmp") != -1)
		{
			saveImageType = BMP_IMAGE_TYPE_INDEX;
		}
		if (filter.indexOf(".pov") != -1)
		{
			exportType = LDrawModelViewer::ETPov;
		}
		if (filter.indexOf(".stl") != -1)
		{
			exportType = LDrawModelViewer::ETStl;
		}
#ifdef EXPORT_3DS
		if (filter.indexOf(".3ds") != -1)
		{
			exportType = LDrawModelViewer::ET3ds;
		}
#endif
		
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
//
//	The command line snapshot will be blank if LDSnapshotTaker has no argument
//
			snapshotTaker = new LDSnapshotTaker(modelViewer);
		}
		else
		{
			snapshotTaker = new LDSnapshotTaker(modelViewer);
		}
	}
#ifndef QOPENGLWIDGET
	if (TREGLExtensions::haveFramebufferObjectExtension())
	{
		snapshotTaker->setUseFBO(true);
	}
#else
	snapshotTaker->setUseFBO(false);
#endif
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
																				
	snprintf(buf, sizeof(buf), TCLocalStrings::get("OverwritePrompt"),
		filename);
	switch( QMessageBox::warning( this, "LDView",
		buf,
#if QT_VERSION >= QT_VERSION_CHECK(6,2,0)
		QMessageBox::Yes, QMessageBox::No
#else
		QMessageBox::Yes | QMessageBox::Default,
		QMessageBox::No | QMessageBox::Escape
#endif
	)) {
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

void ModelViewerWidget::fileExportOptionButton()
{
	QString filter = saveDialog->selectedNameFilter();
	if (filter.indexOf(".pov") != -1)
	{
		fileExportOption();
	}
	if (filter.indexOf(".stl") != -1)
	{
		fileSTLExportOption();
	}
#ifdef EXPORT_3DS
	if (filter.indexOf(".3ds") != -1)
	{
		file3DSExportOption();
	}
#endif
}

void ModelViewerWidget::fileExportOption()
{
	LDViewExportOption exportOption(mainWindow,modelViewer);

	if (exportOption.exec() == QDialog::Rejected)
	{
		modelViewer->getExporter((LDrawModelViewer::ExportType)0, true);
	}
}

void ModelViewerWidget::file3DSExportOption()
{
#ifdef EXPORT_3DS
	LDViewExportOption dsExportOption(mainWindow,modelViewer,LDrawModelViewer::ET3ds);
	dsExportOption.setWindowTitle("3DS Export Options");
	if (dsExportOption.exec() == QDialog::Rejected)
	{
		modelViewer->getExporter((LDrawModelViewer::ExportType)0, true);
	}
#endif
}

void ModelViewerWidget::fileSTLExportOption()
{
	LDViewExportOption stlExportOption(mainWindow,modelViewer,LDrawModelViewer::ETStl);
	stlExportOption.setWindowTitle("STL Export Options");
	if (stlExportOption.exec() == QDialog::Rejected)
	{
		modelViewer->getExporter((LDrawModelViewer::ExportType)0,true);
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
				QString::fromWCharArray(TCLocalStrings::get(L"ViewInfoTitle")),
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
				QString::fromWCharArray(TCLocalStrings::get(L"PovCameraTitle")), quserMessage,
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
			TCUserDefaults::setLongForKey(800, SAVE_WIDTH_KEY, false);
			TCUserDefaults::setLongForKey(600, SAVE_HEIGHT_KEY, false);

			// By saying it's from the command line, none of the above settings
			// will be written to TCUserDefaults.  I know it's not really from
			// the command line, but it produces the behavior we want.
			saveImageType = PNG_IMAGE_TYPE_INDEX;
			saveImage(snapshotPath, 800, 600);
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
			QString::fromWCharArray(TCLocalStrings::get(L"PLGenerateError")),
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
			PartList *partlist = new PartList(mainWindow, this, htmlInventory);
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
				int findSpot = filename.lastIndexOf((
#if QT_VERSION >= 0x60000
						QRegularExpression("/\\")
#else
						QRegExp("/\\")
#endif
					));
				if (findSpot >= 0 && findSpot < (int)filename.length())
					filename=filename.mid(findSpot+1);
				findSpot = filename.lastIndexOf(('.'));
				if (findSpot >= 0 && findSpot < (int)filename.length())
					filename=filename.left(findSpot);
				filename += ".html";
				findSpot = filename.lastIndexOf(('/'));
				if (findSpot >= 0 && findSpot < (int)filename.length())
					filename = filename.mid(findSpot + 1);
				QString startWith = QString(htmlInventory->getLastSavePath()) +
					QString("/") + filename;
				QString filter = QString(TCLocalStrings::get("HtmlFileType")) +
					" (*.html)";
				while (!done)
				{
					QString htmlFilename = QFileDialog::getSaveFileName(this,
						QString::fromWCharArray(TCLocalStrings::get(L"GeneratePartsList")),
						initialDir + "/" + filename,
						filter);
					if (htmlFilename.isEmpty())
					{
						done = true;
					}
					else
					{
						if (fileExists(htmlFilename.toUtf8().constData()))
						{
							QString prompt =
								QString::fromWCharArray(TCLocalStrings::get(L"OverwritePrompt"));

							prompt.replace("%s", htmlFilename);
							if (QMessageBox::warning(this, "LDView", prompt,
								QMessageBox::Yes, QMessageBox::No) ==
								QMessageBox::No)
							{
								continue;
							}
						}
						doPartList(htmlInventory, partsList,
							htmlFilename.toUtf8().constData());
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

void ModelViewerWidget::doStatistics()
{
	if (modelViewer)
	{
		if (modelViewer->getBoundingBoxesOnly())
		{
			QMessageBox::warning(this, "Error",
				QString::fromWCharArray(TCLocalStrings::get(L"NoStatisticsError")),
				QMessageBox::Ok, QMessageBox::NoButton);

		}
		else
		{
			statistics->show();
		}
	}
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
		case Qt::Key_PageUp:
			return LDInputHandler::KCPageUp;
		case Qt::Key_PageDown:
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
	if (inputHandler->keyDown(convertKeyModifiers(event->modifiers()),
		convertKeyCode(event->key())))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
	if((event->modifiers() & Qt::AltModifier) &&
		(event->key() >= Qt::Key_0) &&
		(event->key() <= Qt::Key_9) && preferences)
	{
		int i = event->key()-Qt::Key_0;
		preferences->performHotKey(i);
	}
	if(event->key() == Qt::Key_F10 && fullscreen)
	{
		doViewFullScreen();
	}
	unlock();
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
	QOpenGLWidget::keyPressEvent(event);
#else
	QGLWidget::keyPressEvent(event);
#endif
}

void ModelViewerWidget::keyReleaseEvent(QKeyEvent *event)
{
	lock();
	if (inputHandler->keyUp(convertKeyModifiers(event->modifiers()),
		convertKeyCode(event->key())))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
	unlock();
#if (QT_VERSION >= 0x50400) && defined(QOPENGLWIDGET)
	QOpenGLWidget::keyReleaseEvent(event);
#else
	QGLWidget::keyReleaseEvent(event);
#endif
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
			if (isFboActive())
			{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
				if (!fbo->isBound())
				{
					fbo->bind();
				}
#endif
			}
			else
			{
				makeCurrent();
			}
			//glEnable(GL_DEPTH_TEST);
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
	libraryUpdateProgressMessage = QString::fromWCharArray(alert->getWMessage());
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
	if (alert && !saving)
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
			wildcard.append(letter.toLower());
			wildcard.append(letter.toUpper());
			wildcard.append(']');
		}
		else
		{
			wildcard.append(letter);
		}
	}
	dir.setNameFilters(QStringList(wildcard));
	files = dir.entryList();
	if (files.count())
	{
		QString file = files[0];

		if (file.length() == (int)strlen(filename))
		{
			// This should never be false, but just want to be sure.
			strcpy(filename, file.toUtf8().constData());
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

	dir.setFilter(QDir::AllEntries | QDir::Readable | QDir::Hidden | QDir::System);
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
				delete [] dirName;
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
		delete [] dirName;
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
	QString dir = QDir::currentPath();
	QFile file(filename);
	QString retValue;

//	if (!file.exists())
//		QDir::setCurrent("..");
	if (!file.exists())
		QDir::setCurrent("Textures");
	if (!file.exists())
		QDir::setCurrent("/usr/share/ldview");
	if (!file.exists())
		QDir::setCurrent("/usr/local/share/ldview");
	if (!file.exists())
		QDir::setCurrent("/usr/local/etc");
	if (!file.exists())
		QDir::setCurrent("/usr/local/lib");
	if (!file.exists())
		QDir::setCurrent(QDir( QCoreApplication::applicationDirPath() + "/../share/ldview").absolutePath());
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
		QString newDir = QDir::currentPath();
		retValue = newDir + "/" + file.fileName();
	}
	QDir::setCurrent(dir);
	return retValue;
}

// Note: static method.
TCULong ModelViewerWidget::convertKeyModifiers(Qt::KeyboardModifiers osModifiers)
{
	TCULong retValue = 0;
	if (osModifiers & Qt::ShiftModifier)
	{
		retValue |= LDInputHandler::MKShift;
	}
	if (osModifiers & Qt::ControlModifier)
	{
		retValue |= LDInputHandler::MKControl;
	}
	return retValue;
}

void ModelViewerWidget::nextStep()
{
	if (modelViewer->getStep()>=(ptrdiff_t)modelViewer->getNumSteps())
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
	unsigned int step = modelViewer->getStep();
	QString max = QString::number(modelViewer->getNumSteps());
	mainWindow->toolbarFirstStepSetEnabled(step>1);
	mainWindow->toolbarPrevStepSetEnabled(step>1);
	mainWindow->toolbarNextStepSetEnabled(modelViewer->getNumSteps()>step);
	mainWindow->toolbarLastStepSetEnabled(modelViewer->getNumSteps()>step);
	mainWindow->setStepGotoEnabled(modelViewer->getNumSteps()>0);
	mainWindow->toolbarMaxStepSetText(" / "+max);
	mainWindow->toolbarCurrentStepSetText(QString::number(step));
}

void ModelViewerWidget::gotoStep()
{
	bool ok;
	int step =
#if QT_VERSION < 0x40500
			QInputDialog::getInteger(
#else
			QInputDialog::getInt(
#endif
			this,"Step","Go to Step:",
			modelViewer->getStep(), 1, modelViewer->getNumSteps(), 1, &ok );
	if (ok)
	{
		modelViewer->setStep(step);
		updateStep();
		doApply();
	}
}

