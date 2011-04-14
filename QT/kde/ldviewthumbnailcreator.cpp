#include "ldviewthumbnailcreator.h"
#include <kdebug.h>

extern "C"
{
   KDE_EXPORT ThumbCreator *new_creator()
   {
     return new LDViewCreator;
   }
};


LDViewCreator::LDViewCreator()
{
	kWarning(1111) << "LDView thumbnail plugin started";
}

bool LDViewCreator::create (const QString &path, int w, int h, QImage &img)
{
	kWarning(1111) << "LDView thumbnail CREATE";
	QProcess *process = new QProcess;
	process->start("/usr/local/bin/LDView", QStringList() << path << "-SaveSnapshot=/tmp/ldview-kde.png" << 
											QString("-SaveWidth=")+QString::number(w) <<
											QString("-SaveHeight=")+QString::number(h) <<
											"-ShowErrors=0" << "-SaveActualSize=0");
	if (!process->waitForFinished())
		return false;
	return img.load(QString("/tmp/ldview-kde.png"));
}

ThumbCreator::Flags LDViewCreator::flags() const
{
	return (Flags)(DrawFrame | BlendIcon);
}

