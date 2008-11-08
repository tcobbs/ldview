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
	// LDView fails to exit when running with the ATI video driver on Linux.
	kill(getpid(), 9);
#endif // __linux__
	return retValue;
}

// Just a comment to verify I have commit permissions in cvs.
