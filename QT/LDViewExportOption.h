#ifndef __LDVIEWExportOption_H__
#define __LDVIEWExportOption_H__

#include "ui_ExportOptionPanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <QtGui/QDialog>

class Preferences;
class LDrawModelViewer;
class LDExporter;
class LDExporterSetting;

typedef std::map<LDExporterSetting *, QWidget *> SettingsMap;

class LDViewExportOption : public QDialog , Ui::ExportOptionPanel
{
	Q_OBJECT
public:
	LDViewExportOption(QWidget *parent,LDrawModelViewer *modelViewer);
	~LDViewExportOption();

public slots:
	int exec(void);
	void doOk(void);
	void doCancel(void);
	void doReset(void);

protected:
//	void populateTypeBox(void);
	void populate(void);

	void doTypeBoxActivated(void);

	LDrawModelViewer *m_modelViewer;
    LDExporter *m_exporter;
	QWidget *m_box;
	QVBoxLayout *m_lay;
	SettingsMap m_settings;
	int m_origType;
};

#endif // __LDVIEWExportOption_H__
