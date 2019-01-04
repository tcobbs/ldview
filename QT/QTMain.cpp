#include <QApplication>
#include <QTextCodec>
#include <qgl.h>
#include "LDViewMainWindow.h"
#include "ModelViewerWidget.h"
#include "SnapshotTaker.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <TRE/TREMainModel.h>
#include <string.h>
#include <QTranslator>
#include <QLocale>
#include <locale.h>
#ifdef __linux__
#include <signal.h>
#include <sys/resource.h>
#endif // __linux__

#ifdef __linux__
#include <QThread>
class KillThread : public QThread
{
public:
	virtual void run()
	{
		// The kill sometimes results in a core dump.  Make sure that doesn't
		// happen by setting the core dump limit to 0.
		struct rlimit limit;
		getrlimit(RLIMIT_CORE, &limit);
		limit.rlim_cur = 0;
		setrlimit(RLIMIT_CORE, &limit);
		// There's no point forcing the user to wait 2 seconds if it is known
		// for a fact that the shutdown is going to fail, so if they manually
		// set the AlwaysForceKill key, then just kill without the 2-second
		// wait.
		if (!TCUserDefaults::boolForKey("AlwaysForceKill", false, false))
		{
			sleep(2);
			// LDView fails to exit when running with the ATI video driver on
			// Linux. If we get here before the program has already killed off
			// this thread due to exiting, then force a kill.  It's very
			// unlikely that atexit() shutdown processing will take 2 full
			// seconds, so if we do get here, it's a pretty safe bet that the
			// program is locked up.
		}
		kill(getpid(), SIGQUIT);
	}
};
#endif // __linux__

static QGLFormat defaultFormat;

void setupDefaultFormat(void)
{
	defaultFormat.setAlpha(true);
	defaultFormat.setStencil(true);
	QGLFormat::setDefaultFormat(defaultFormat);
}

bool doCommandLine()
{
	SnapshotTaker *snapshotTaker = new SnapshotTaker();
	// Load stud texture
	QImage studImage(":/images/images/StudLogo.png");
#if QT_VERSION < 0x40600
	long len = studImage.numBytes();
#else
	long len = studImage.byteCount();
#endif
	TREMainModel::setRawStudTextureData(studImage.bits(), len);
	LDLModel::setFileCaseCallback(ModelViewerWidget::staticFileCaseCallback);

	bool retValue = snapshotTaker->doCommandLine();
	TCObject::release(snapshotTaker);
	return retValue;
}

int main(int argc, char *argv[])
{
	QLocale::setDefault(QLocale::system());
	char locale[3];
	QString filename;

	QApplication::setColorSpec(QApplication::CustomColor);
	QApplication a( argc, argv );

//	printf("Compiled with Qt %s, running with Qt %s\n",QT_VERSION_STR,qVersion());

	// Default locale is "C".  Change it to the default one actually set by the
	// user.
	setlocale(LC_CTYPE, "");
	strcpy(locale,QLocale::system().name().left(2).toLatin1().constData());
	TCUserDefaults::setCommandLine(argv);
	filename = ModelViewerWidget::findPackageFile(
		QString("LDViewMessages_")+QString(locale) + ".ini");
	if (!filename.length())
	{
		filename = ModelViewerWidget::findPackageFile("LDViewMessages.ini");
	}
	if (!TCLocalStrings::loadStringTable(filename.toLatin1().constData() ))
	{
		printf("Could not find LDViewMessages.ini file.\nPlease copy this "
			"file to /usr/share/ldview directory.\n");
		exit(0);
	}
	QString qloc = QString("Windows-")+QString::number(TCLocalStrings::getCodePage());
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName(qloc.toLatin1()));
	TCUserDefaults::setAppName("LDView");
	if (doCommandLine())
	{
		return 0;
	}
#ifdef DEBUG
	FILE *logFile = fopen("/tmp/LDView.log", "w");
	if (logFile != NULL)
	{
		fprintf(logFile, "Args:\n");
		for (int i = 0; i < argc; i++)
		{
			fprintf(logFile, "<%s>\n", argv[i]);
		}
		fclose(logFile);
	}
#endif // DEBUG
	char *sessionName =
        TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
    if (sessionName && sessionName[0])
    {
        TCUserDefaults::setSessionName(sessionName);
    }
    delete sessionName;

	setupDefaultFormat();
	QTranslator translator(0);
//	printf("%s\n",locale);
	if (!translator.load(QString("ldview_")+QString(locale)+".qm",".") &&
		!translator.load(QString("ldview_")+QString(locale)+".qm",
						 "/usr/local/share/ldview") &&
		!translator.load(QString("ldview_")+QString(locale)+".qm",
						 "/usr/share/ldview") &&
		!translator.load(QString("ldview_")+QString(locale)+".qm",
						QDir( QCoreApplication::applicationDirPath() + "/../share/ldview").absolutePath()) &&
		QString(locale).compare("en")!=0)
		printf ("Failed to load translation %s\n",locale);
	a.installTranslator(&translator);
    LDViewMainWindow *w = new LDViewMainWindow(&a);
	if (!TCUserDefaults::stringForKey(SAVE_SNAPSHOT_KEY))
    	w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    int retValue = a.exec();
#ifdef __linux__
	KillThread *killThread = new KillThread;
	killThread->start();
#endif // __linux__
	return retValue;
}

// Just a comment to verify I have commit permissions in cvs.
