#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qmotifplusstyle.h>
#include <qgl.h>
#include "LDView.h"
#include "ModelViewerWidget.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>
#include <UserDefaultsKeys.h>

static QGLFormat defaultFormat;

void setupDefaultFormat(void)
{
	defaultFormat.setAlpha(true);
	defaultFormat.setStencil(true);
	QGLFormat::setDefaultFormat(defaultFormat);
}

int main(int argc, char *argv[])
{
	if (TCLocalStrings::loadStringTable("LDViewMessages.ini"))
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
    LDView *w = new LDView;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	w->modelViewer->setApplication(&a);
    return a.exec();
}

// Just a comment to verify I have commit permissions in cvs.
