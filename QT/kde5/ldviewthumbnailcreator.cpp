#include "ldviewthumbnailcreator.h"
#include <QTemporaryFile>
#include <QProcess>
#include <QImage>
#include <QFile>
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(LDViewCreator, "ldviewthumbnail.json")

LDViewCreator::LDViewCreator(QObject *parent, const QVariantList &args)
    : KIO::ThumbnailCreator(parent, args)
{
}

KIO::ThumbnailResult LDViewCreator::create (const KIO::ThumbnailRequest &request)
{
	QString tmpname;
	QTemporaryFile tmpfile;
	QImage img;
	QString path = request.url().toLocalFile();
	const int w = request.targetSize().width();
	const int h = request.targetSize().height();

	if (tmpfile.open())
	{
		tmpname = tmpfile.fileName();
		tmpfile.close();
		tmpfile.remove();
	}
	tmpname += ".png";
	QProcess process;
	process.start("/usr/bin/LDView",
		QStringList() << path << "-SaveSnapshot="+tmpname <<
		QString("-SaveWidth=")+QString::number(w) <<
		QString("-SaveHeight=")+QString::number(h) <<
		"-ShowErrors=0" << "-SaveActualSize=0");
	if (!process.waitForFinished())
		return KIO::ThumbnailResult::fail();
	img.load(tmpname);
	QFile::remove (tmpname);
	return KIO::ThumbnailResult::pass(img);
}

#include "ldviewthumbnailcreator.moc"
