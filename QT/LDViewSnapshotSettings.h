#ifndef __LDVIEWSNAPSHOTSETTINGS_H__
#define __LDVIEWSNAPSHOTSETTINGS_H__

#include <TCFoundation/TCTypedObjectArray.h>
//#include "ModelViewerWidget.h"
#include "SnapshotSettingsPanel.h"

class QButton;

class SnapshotSettings : public SnapshotSettingsPanel
{
public:
	SnapshotSettings(ModelViewerWidget *modelWidget);
	~SnapshotSettings(void);

	void reflectSettings();
    void setButtonState(QCheckBox *button, bool state);
	void doEnabledSeries();
	void doEnabledSize();
//	void show(void);
public slots:
	void clear(void);
	void doOk(void);
	void doCancel(void);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
};

#endif

