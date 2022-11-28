#ifndef __LDVIEWExportOption_H__
#define __LDVIEWExportOption_H__

#include "ui_ExportOptionPanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <QDialog>
#include <QLineEdit>
#include "LDLib/LDrawModelViewer.h"

class Preferences;
class LDrawModelViewer;
class LDExporter;
class LDExporterSetting;

typedef std::map<LDExporterSetting *, QWidget *> SettingsMap;
typedef std::map<QPushButton *, QLineEdit *> ButtonMap;
typedef std::map<QLayout *, SettingsMap> GroupMap;

class LDViewExportOption : public QDialog , Ui::ExportOptionPanel
{
	Q_OBJECT
public:
	LDViewExportOption(QWidget *parent,LDrawModelViewer *modelViewer, LDrawModelViewer::ExportType= LDrawModelViewer::ETPov);
	~LDViewExportOption();

public slots:
	int exec(void);
	void doOk(void);
	void doCancel(void);
	void doReset(void);
	void doResetGroup(void);
	void doBrowse(void);

protected:
//	void populateTypeBox(void);
	void populate(void);

	void doTypeBoxActivated(void);
	void resetSettings(SettingsMap &settings);

	LDrawModelViewer *m_modelViewer;
	LDExporter *m_exporter;
	QWidget *m_box;
	QVBoxLayout *m_lay;
	SettingsMap m_settings;
	ButtonMap m_button;
	GroupMap m_groups;
	int m_origType;
};

#endif // __LDVIEWExportOption_H__
