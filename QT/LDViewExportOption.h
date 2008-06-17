#ifndef __LDVIEWExportOption_H__
#define __LDVIEWExportOption_H__

#include "ExportOptionPanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <LDExporter/LDExporter.h>

class QScrollView;
class QVBox;
class ExportOptionPanel;
class Preferences;
class LDrawModelViewer;
class LDViewExportOption : public ExportOptionPanel
{
//	Q_OBJECT
public:
	LDViewExportOption(LDExporter *exporter);
	~LDViewExportOption();

public slots:


protected:
    LDExporter *m_exporter;
	QScrollView *sv;
	QVBox *box;
};

#endif // __LDVIEWExportOption_H__
