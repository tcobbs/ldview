#include <qglobal.h>
#if (QT_VERSION >>16)==4
#include <q3dragobject.h>
#define QMimeSourceFactory	Q3MimeSourceFactory
#include <Q3MimeSourceFactory>
#include <q3header.h>
#define  getimage(x)    QPixmap(qPixmapFromMimeSource(x))
#endif
