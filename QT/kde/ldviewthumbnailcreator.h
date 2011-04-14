#include <kio/thumbcreator.h>
#include <kimageio.h>
#include <qprocess.h>
#include <qimage.h>
#include <qfile.h>
#include <QObject>

class LDViewCreator : public QObject, public ThumbCreator
{
Q_OBJECT
public:
	LDViewCreator();
	virtual bool create (const QString &path, int, int, QImage &img);
	virtual Flags flags() const;
};
