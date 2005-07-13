#include <qglobal.h>
#if (QT_VERSION >>16)==4
#define QFileDialog	Q3FileDialog
#include <q3filedialog.h>
#define QPopupMenu	QMenu
#include <q3popupmenu.h>
#define QListBox	Q3ListBox
#include <q3listbox.h>
#define QListView	Q3ListView
#define QListViewItem	Q3ListViewItem
#include <q3listview.h>
#define QProgressBar	Q3ProgressBar
#include <q3progressbar.h>
#include <q3dragobject.h>
#define QRangeControl	QSpinBox
#define QButton		Q3Button
#include <q3button.h>
#define QMimeSourceFactory	Q3MimeSourceFactory
#include <Q3MimeSourceFactory>
#define QImageDrag	Q3ImageDrag
#include <Q3ImageDrag>
#include <q3rangecontrol.h>
#include <q3header.h>
#include <QMenuItem>
#define  getimage(x)    QPixmap(qPixmapFromMimeSource(x))
#else
#include <qfiledialog.h>
#include <qpopupmenu.h>
#include <qdragobject.h>
#include <qheader.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qprogressbar.h>
#include <qrangecontrol.h>
#include <qbutton.h>
#define getimage(x)     QPixmap::fromMimeSource(x)
#endif
