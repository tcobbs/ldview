#ifndef __LDVIEWMPDMODEL_H__
#define __LDVIEWMPDMODEL_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include "MpdModelSelectionPanel.h"
#include <TCFoundation/TCStlIncludes.h>
#include <LDLib/LDModelTree.h>
#include <LDLoader/LDLMainModel.h>


class LDLMainModel;
class LDModelTree;
class Preferences;
class LDrawModelViewer;
class MpdModel : public MpdModelSelectionPanel
{
//	Q_OBJECT
public:
	MpdModel(ModelViewerWidget *modelViewer);
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

protected:
    void setModel(LDLMainModel *model);
    void setModelWindow(ModelViewerWidget *modelWindow);

	LDModelTree *modeltree;
	ModelViewerWidget *m_modelWindow;
	LDLMainModel *mainmodel;
};

#endif // __LDViewMpdModel_H__
