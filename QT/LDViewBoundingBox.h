#ifndef __LDVIEWBoundingBox_H__
#define __LDVIEWBoundingBox_H__

#include "BoundingBoxPanel.h"

class QButton;
class ModelViewerWidget;

class BoundingBox : public BoundingBoxPanel
{
public:
	BoundingBox(ModelViewerWidget *modelWidget);
	~BoundingBox(void);
    void modelAlertCallback(TCAlert *alert);
    void setModel(LDLMainModel *model);
    void setModelWindow(ModelViewerWidget *modelWindow);
    void updateData(void);
    void showBoundingBox(bool value);
    LDrawModelViewer *getModelViewer(void);

    ModelViewerWidget *m_modelWindow;
    LDLMainModel *m_model;

public slots:
	void show(void);
	void hide();
protected:
//	LDrawModelViewer *modelViewer;
//	ModelViewerWidget *modelWidget;
};

#endif

