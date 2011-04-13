#include <kio/thumbcreator.h>
#include <kimageio.h>
#include <qprocess.h>
#include <qimage.h>
#include <qfile.h>
#include <qtextstream.h>
#include <unistd.h>

class LDViewCreator : public ThumbCreator
{
public:
	LDViewCreator() {QFile file(QString("/tmp/ldviewkde.log"));
        if(file.open(QIODevice::WriteOnly|QIODevice::Append))
        {
                QTextStream str(&file);
                str << "started\n";
                file.close();
	}};
	virtual bool create (const QString &path, int, int, QImage &img);
	virtual Flags flags() const { return (Flags)(DrawFrame | BlendIcon);}
};

bool LDViewCreator::create (const QString &path, int w, int h, QImage &img)
{
	QFile file(QString("/tmp/ldviewkde.log"));
	if(file.open(QIODevice::WriteOnly|QIODevice::Append))
	{
		QTextStream str(&file);
		str << path <<"\n";
		file.close();
	}
	QProcess *process = new QProcess;
	process->start("/usr/local/bin/LDView", QStringList() << path << "-SaveSnapshot=/tmp/ldview-kde.png" << 
											QString("-SaveWidth=")+QString::number(w) <<
											QString("-SaveHeight=")+QString::number(h) <<
											"-ShowErrors=0" << "-SaveActualSize=0");
	process->waitForFinished();
	return img.load(QString("/tmp/ldview-kde.png"));
//	char command[1024];
//	sprintf(command,"/usr/local/bin/LDView %s -SaveSnapshot=%s -SaveWidth=%i -SaveHeight=%i -ShowErrors=0 -SaveActualSize=0",path.ascii(),"/tmp/ldview-kde.png",w,h);
}

extern "C"
{
   KDE_EXPORT ThumbCreator *new_creator()
   {
     return new LDViewCreator;
   }
};

