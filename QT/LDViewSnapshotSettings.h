#ifndef __LDVIEWSNAPSHOTSETTINGS_H__
#define __LDVIEWSNAPSHOTSETTINGS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ui_SnapshotSettingsPanel.h"

class ModelViewerWidget;
class LDrawModelViewer;

class SnapshotSettings : public QDialog , Ui::SnapshotSettingsPanel
{
	Q_OBJECT
public:
	SnapshotSettings(QWidget *parent,ModelViewerWidget *modelWidget);
	~SnapshotSettings(void);

	void reflectSettings();
	void setButtonState(QCheckBox *button, bool state);
public slots:
	void doEnabledSeries();
	void doEnabledSize();
	void doOk(void);
	void doCancel(void);
	void zoomToggled(bool);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
};

#endif

