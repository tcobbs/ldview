#include <kio/thumbcreator.h>
#include <QProcess>
#include <QImage>
#include <QFile>
#include <QObject>

class LDViewCreator : public QObject, public ThumbCreator
{
Q_OBJECT
public:
	virtual bool create (const QString &path, int, int, QImage &img);
	virtual Flags flags() const;
};
