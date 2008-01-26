#ifndef __LDVIEWJPEGOPTIONS_H__
#define __LDVIEWJPEGOPTIONS_H__

#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include "ModelViewerWidget.h"

class JpegOptionsPanel;
class QButton;

class JpegOptions
{
public:
	JpegOptions(ModelViewerWidget *modelWidget);
	~JpegOptions(void);

	void reflectSettings();
	void show(void);
	void clear(void);
	void doOk(void);
	void doCancel(void);
	void doSliderMoved(int);
	void setButtonState(QCheckBox *button, bool state);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	JpegOptionsPanel *panel;
	TCJpegOptions *options;
	int quality;
};

#endif

