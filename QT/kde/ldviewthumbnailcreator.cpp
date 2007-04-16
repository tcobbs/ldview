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
        if(file.open(IO_WriteOnly|IO_Append))
        {
                QTextStream str(&file);
                str << "started\n";
                file.close();
	}};
	virtual bool create (const QString &path, int, int, QImage &img);
	virtual Flags flags() { return (Flags)(DrawFrame | BlendIcon);}
};

bool LDViewCreator::create (const QString &path, int w, int h, QImage &img)
{
	QFile file(QString("/tmp/ldviewkde.log"));
	if(file.open(IO_WriteOnly|IO_Append))
	{
		QTextStream str(&file);
		str << path <<"\n";
		file.close();
	}
	QProcess *process = new QProcess(QString("/usr/local/bin/LDView"));
	process->addArgument(path);
	process->addArgument(QString("-SaveSnapshot=/tmp/ldview-kde.png"));
	process->addArgument(QString("-SaveWidth=")+QString::number(w));
	process->addArgument(QString("-SaveHeight=")+QString::number(h));
	process->addArgument(QString("-ShowErrors=0"));
	process->addArgument(QString("-SaveActualSize=0"));
	process->start();
	while(process->isRunning())
	{
		usleep(50000);
	}
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

