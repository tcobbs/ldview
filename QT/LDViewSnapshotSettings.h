#ifndef __LDVIEWSNAPSHOTSETTINGS_H__
#define __LDVIEWSNAPSHOTSETTINGS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ModelViewerWidget.h"

class SnapshotSettingsPanel;
class QButton;

class SnapshotSettings
{
public:
	SnapshotSettings(ModelViewerWidget *modelWidget);
	~SnapshotSettings(void);

	void reflectSettings();
	void show(void);
	void clear(void);
	void doOk(void);
	void doCancel(void);
	void setButtonState(QCheckBox *button, bool state);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	SnapshotSettingsPanel *panel;
};

#endif

