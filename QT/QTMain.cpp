#include "qt4wrapper.h"
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qradiobutton.h>
#include <qtextcodec.h>
//#include <qmotifplusstyle.h>
#include <qgl.h>
#include "LDView.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <string.h>
#include <qtranslator.h>
#include <qlocale.h>
#include <locale.h>
#ifdef __linux__
#include <signal.h>
#include <sys/resource.h>
#endif // __linux__

#ifdef __linux__
#include <qthread.h>
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

int main(int argc, char *argv[])
{
	const char *loc = QTextCodec::locale();
	QLocale::setDefault(QLocale(loc));
	char locale[3];
	locale[0]=locale[1]=locale[2]=0;
	strncpy(locale,loc,3);
	locale[2]=0;
	QString filename;

	// Default locale is "C".  Change it to the default one actually set by the
	// user.
	setlocale(LC_CTYPE, "");
	TCUserDefaults::setCommandLine(argv);
	filename = ModelViewerWidget::findPackageFile(
		QString("LDViewMessages_")+QString(locale) + ".ini");
	if (!filename.length())
	{
		filename = ModelViewerWidget::findPackageFile("LDViewMessages.ini");
	}
	if (!TCLocalStrings::loadStringTable(filename))
	{
		printf("Could not find LDViewMessages.ini file.\nPlease copy this "
			"file to /usr/local/etc directory.\n");
		exit(0);
	}
	TCUserDefaults::setAppName("LDView");
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

    QApplication::setColorSpec(QApplication::CustomColor);
	setupDefaultFormat();
//	QApplication::setStyle(new QWindowsStyle);
//	QApplication::setStyle(new QMotifStyle);
//	QApplication::setStyle(new QMotifPlusStyle);
    QApplication a( argc, argv );
	QTranslator translator(0);
//	printf("%s\n",locale);
	if (!translator.load(QString("ldview_")+QString(locale)+".qm",".") &&
		!translator.load(QString("ldview_")+QString(locale)+".qm",
						 "/usr/local/share/ldview"))
		printf ("Failed to load translation %s\n",QTextCodec::locale());
	a.installTranslator(&translator);
    LDView *w = new LDView;
	if (!TCUserDefaults::stringForKey(SAVE_SNAPSHOT_KEY))
    	w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	w->modelViewer->setApplication(&a);
    int retValue = a.exec();
#ifdef __linux__
	KillThread *killThread = new KillThread;
	killThread->start();
#endif // __linux__
	return retValue;
}

// Just a comment to verify I have commit permissions in cvs.
