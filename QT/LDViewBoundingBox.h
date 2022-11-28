#ifndef __LDVIEWBoundingBox_H__
#define __LDVIEWBoundingBox_H__

#include "ui_BoundingBoxPanel.h"
#include <QDialog>
#include <QCloseEvent>

class ModelViewerWidget;

class BoundingBox : public QDialog , Ui::BoundingBoxPanel
{
public:
	BoundingBox(QWidget *parent, ModelViewerWidget *modelWidget);
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
	void closeEvent(QCloseEvent * /* event */) {hide();}
};

#endif

