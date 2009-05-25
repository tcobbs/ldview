#include <qglobal.h>
#if (QT_VERSION >>16)==4
#define QProgressBar	Q3ProgressBar
#include <q3progressbar.h>
#include <q3dragobject.h>
#define QMimeSourceFactory	Q3MimeSourceFactory
#include <Q3MimeSourceFactory>
#include <q3rangecontrol.h>
#include <q3header.h>
#include <QMenuItem>
#define  getimage(x)    QPixmap(qPixmapFromMimeSource(x))
#include <Q3ScrollView>
#define QScrollView Q3ScrollView
#include <Q3HBox>
#define QHBox Q3HBox
#include <Q3VBox>
#define QVBox Q3VBox
#include <Q3GroupBox>
#define QGroupBox Q3GroupBox
#endif
