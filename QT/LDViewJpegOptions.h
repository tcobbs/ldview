#ifndef __LDVIEWJPEGOPTIONS_H__
#define __LDVIEWJPEGOPTIONS_H__

#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include "ModelViewerWidget.h"
#include "JpegOptionsPanel.h"

class QButton;

class JpegOptions : public JpegOptionsPanel
{
public:
	JpegOptions(ModelViewerWidget *modelWidget);
	~JpegOptions(void);

	void reflectSettings();
    void setButtonState(QCheckBox *button, bool state);
	void clear(void);

public slots:
	void doOk(void);
	void doCancel(void);
	void doSliderMoved(int);
protected:

	LDrawModelViewer *modelViewer;
	ModelViewerWidget *modelWidget;
	TCJpegOptions *options;
	int quality;
};

#endif

