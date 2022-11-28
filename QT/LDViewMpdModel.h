#ifndef __LDVIEWMPDMODEL_H__
#define __LDVIEWMPDMODEL_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "ui_MpdModelSelectionPanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <LDLib/LDModelTree.h>
#include <LDLoader/LDLMainModel.h>
#include <QDialog>

class Preferences;
class LDrawModelViewer;
class ModelViewerWidget;
class MpdModel : public QDialog , Ui::MpdModelSelectionPanel
{
	Q_OBJECT
public:
	MpdModel(QWidget *parent, ModelViewerWidget *modelViewer);
	~MpdModel();
	void show(void);
	void hide(void);
	void showOptions(),hideOptions();
	void toggleOptions();
	void updateData();
	void showMpdModel(int);
	LDrawModelViewer *getModelViewer(void);
	void modelAlertCallback(TCAlert *alert);


public slots:
	void doMpdSelected(int i) { showMpdModel(i);}
	void doMpdSelected(QListWidgetItem *a) {showMpdModel(modelList->row(a));}
	void doMpdSelected(QListWidgetItem *a,QListWidgetItem * /* b */) {if (a) showMpdModel(modelList->row(a));}
	void ok(void);
	void cancel(void);


protected:
	void setModel(LDLMainModel *model);
	void setModelWindow(ModelViewerWidget *modelWindow);

	LDModelTree *modeltree;
	ModelViewerWidget *m_modelWindow;
	LDLMainModel *mainmodel;
	bool m_okPressed;
};

#endif // __LDViewMpdModel_H__
