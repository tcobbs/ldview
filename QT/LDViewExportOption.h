#ifndef __LDVIEWExportOption_H__
#define __LDVIEWExportOption_H__

#include "ExportOptionPanel.h"
#include <TCFoundation/TCStlIncludes.h>

class QScrollView;
class QVBox;
class Preferences;
class LDrawModelViewer;
class LDExporter;
class LDExporterSetting;

typedef std::map<LDExporterSetting *, QWidget *> SettingsMap;

class LDViewExportOption : public ExportOptionPanel
{
//	Q_OBJECT
public:
	LDViewExportOption(LDrawModelViewer *modelViewer);
	~LDViewExportOption();

public slots:
	int exec(void);


protected:
	void populateTypeBox(void);
	void populate(void);
	void doOk(void);
	void doCancel(void);
	void doReset(void);
	void doTypeBoxActivated(void);

	LDrawModelViewer *m_modelViewer;
    LDExporter *m_exporter;
	QScrollView *m_sv;
	QVBox *m_box;
	SettingsMap m_settings;
};

#endif // __LDVIEWExportOption_H__
