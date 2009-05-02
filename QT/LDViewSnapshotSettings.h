#ifndef __LDVIEWSNAPSHOTSETTINGS_H__
#define __LDVIEWSNAPSHOTSETTINGS_H__

#include <TCFoundation/TCTypedObjectArray.h>
//#include "ModelViewerWidget.h"
#include "SnapshotSettingsPanel.h"

class QButton;
class ModelViewerWidget;
class QCheckBox;
class LDrawModelViewer;

class SnapshotSettings : public SnapshotSettingsPanel
{
	Q_OBJECT
public:
	SnapshotSettings(ModelViewerWidget *modelWidget);
	~SnapshotSettings(void);

	void reflectSettings();
    void setButtonState(QCheckBox *button, bool state);
//	void show(void);
public slots:
//	void clear(void);
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

