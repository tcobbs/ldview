#include "ldviewthumbnailcreator.h"
#include <QTemporaryFile>

extern "C"
{
   Q_DECL_EXPORT ThumbCreator *new_creator()
   {
     return new LDViewCreator;
   }
};


bool LDViewCreator::create (const QString &path, int w, int h, QImage &img)
{
	QString tmpname;
	bool result;
	QTemporaryFile tmpfile;
	if (tmpfile.open())
	{
		tmpname=tmpfile.fileName();
		tmpfile.close();
		tmpfile.remove();
	}
	tmpname+=".png";
	QProcess *process = new QProcess;
	process->start("/usr/bin/LDView", 
		QStringList() << path << "-SaveSnapshot="+tmpname << 
		QString("-SaveWidth=")+QString::number(w) <<
		QString("-SaveHeight=")+QString::number(h) <<
		"-ShowErrors=0" << "-SaveActualSize=0");
	if (!process->waitForFinished())
		return false;
	result = img.load(tmpname);
	QFile::remove (tmpname);
	return result;
}

ThumbCreator::Flags LDViewCreator::flags() const
{
	return (Flags)(DrawFrame | BlendIcon);
}

