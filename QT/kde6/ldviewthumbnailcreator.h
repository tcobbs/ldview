#include <KIO/ThumbnailCreator>
#include <QObject>

class LDViewCreator : public KIO::ThumbnailCreator
{
Q_OBJECT
public:
	LDViewCreator(QObject *parent, const QVariantList &args);
	KIO::ThumbnailResult create (const KIO::ThumbnailRequest &request) override;
};
