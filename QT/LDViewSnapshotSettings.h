#ifndef __LDVIEWSNAPSHOTSETTINGS_H__
#define __LDVIEWSNAPSHOTSETTINGS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ui_SnapshotSettingsPanel.h"

class ModelViewerWidget;
class LDrawModelViewer;

class SnapshotSettings : public QWidget , Ui::SnapshotSettingsPanel
{
	Q_OBJECT
public:
	SnapshotSettings(QWidget *parent,ModelViewerWidget *modelWidget);
	~SnapshotSettings(void);

	void reflectSettings();
	void setButtonState(QCheckBox *button, bool state);
	void doOk();
	void doCancel();
public slots:
	void doEnabledSeries();
	void doEnabledSize();
	void zoomToggled(bool);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
};

#endif

