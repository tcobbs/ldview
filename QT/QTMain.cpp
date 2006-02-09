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
#include <UserDefaultsKeys.h>
#include <string.h>
#include <qtranslator.h>

static QGLFormat defaultFormat;

void setupDefaultFormat(void)
{
	defaultFormat.setAlpha(true);
	defaultFormat.setStencil(true);
	QGLFormat::setDefaultFormat(defaultFormat);
}

bool load(QString file)
{
	return TCLocalStrings::loadStringTable(file.ascii());
}

int main(int argc, char *argv[])
{
	const char *loc = QTextCodec::locale();
	char locale[3];
	locale[0]=locale[1]=locale[2]=0;
	strncpy(locale,loc,3);
	locale[2]=0;

	if (load(QString("LDViewMessages_")+QString(locale) + ".ini")) {}
	else if (load(QString("/usr/local/share/ldview/LDViewMessages_") +QString(locale) + ".ini")) {}
	else if (TCLocalStrings::loadStringTable("LDViewMessages.ini"))
	{
//		printf("Using LDViewMessages.ini\n");
	}
	else if (TCLocalStrings::loadStringTable("../LDViewMessages.ini"))
	{
//		printf("Using ../LDViewMessages.ini\n");
	}
	else if (TCLocalStrings::loadStringTable("/usr/local/etc/LDViewMessages.ini"))
	{
//		printf("Using /usr/local/etc/LDViewMessages.ini\n");
	}
	else if (TCLocalStrings::loadStringTable("/usr/local/lib/LDViewMessages.ini"))
	{
//      printf("Using /usr/local/lib/LDViewMessages.ini\n");
    }
	else if (TCLocalStrings::loadStringTable("/usr/local/share/ldview/LDViewMessages.ini"))
	{
//		printf("Using /usr/local/share/ldview/LDViewMessages.ini\n");
	}
	else
	{
		// The 64 at the end is padding, so that we can add onto it.
		char *path = copyString(argv[0], 64);

		if (strrchr(path, '/'))
		{
			*strrchr(path, '/') = 0;
		}
		strcat(path, "/LDViewMessages.ini");
		if (TCLocalStrings::loadStringTable(path))
		{
//			printf("Using %s\n", path);
		}
		else
		{
			printf("Could not find LDViewMessages.ini file.\nPlease copy this "
				"file to /usr/local/etc directory.\n");
			exit(0);
		}
		delete path;
	}
	TCUserDefaults::setCommandLine(argv);
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
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	w->modelViewer->setApplication(&a);
    return a.exec();
}

// Just a comment to verify I have commit permissions in cvs.
