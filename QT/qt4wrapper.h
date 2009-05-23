#include <qglobal.h>
#if (QT_VERSION >>16)==4
#define QFileDialog	Q3FileDialog
#include <q3filedialog.h>
#define QPopupMenu	QMenu
#include <q3popupmenu.h>
//#define QListBox	Q3ListBox
//#define QListBoxItem Q3ListBoxItem
//#define QListBoxText Q3ListBoxText
//#include <q3listbox.h>
//#define QListView	Q3ListView
//#define QListViewItem	Q3ListViewItem
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
#include <Q3CheckListItem>
//#define QCheckListItem  Q3CheckListItem
#include <Q3ScrollView>
#define QScrollView Q3ScrollView
#include <Q3HBox>
#define QHBox Q3HBox
#include <Q3VBox>
#define QVBox Q3VBox
#include <Q3VGroupBox>
#define QVGroupBox Q3VGroupBox
#include <Q3GroupBox>
#define QGroupBox Q3GroupBox
#endif
