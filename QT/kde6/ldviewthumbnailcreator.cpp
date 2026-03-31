#include "ldviewthumbnailcreator.h"
#include <QTemporaryFile>
#include <QTextStream>
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
	QString path=QFile::encodeName(request.url().toLocalFile()).data();
	const int w=request.targetSize().width();
	const int h=request.targetSize().height();
//	QFile outFile("/tmp/ldview.log");
//	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//	QTextStream textStream(&outFile);
//	textStream <<  path << ":" << w << ":" << h <<"\n";
//	outFile.close();

	if (tmpfile.open())
	{
		tmpname=tmpfile.fileName();
		tmpfile.close();
		tmpfile.remove();
	}
	tmpname+=".png";
	QProcess *process = new QProcess;
	process->start("/usr/bin/ldview", 
		QStringList() << path << "-SaveSnapshot="+tmpname << 
		QString("-SaveWidth=")+QString::number(w) <<
		QString("-SaveHeight=")+QString::number(h) <<
		"-ShowErrors=0" << "-SaveActualSize=0");
	if (!process->waitForFinished())
		return KIO::ThumbnailResult::fail();
	img.load(tmpname);
	QFile::remove (tmpname);
	return KIO::ThumbnailResult::pass(img);
}

#include "ldviewthumbnailcreator.moc"
#include "moc_ldviewthumbnailcreator.cpp"
