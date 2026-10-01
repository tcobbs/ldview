#ifndef __LDVIEWAUTOMATEEDGLELINECOLOR_H__
#define __LDVIEWAUTOMATEEDGLELINECOLOR_H__

#include "ui_AutomateEdgeLineColor.h"
#include <QDialog>
#include <LDLib/LDPreferences.h>
class Preferences;

class AutomateEdgeLineColor : public QDialog , Ui::AutomateEdgeLineColorPanel
{
	Q_OBJECT
public:
	AutomateEdgeLineColor(QWidget *parent);
	~AutomateEdgeLineColor(void);
public slots:
	void doOk(void);
	void doCancel(void);
	void doContrastSlider(int);
	void doSaturationSlider(int);
	void resetContrast();
	void resetSaturation();
protected:
	void reflectSettings();
};

#endif

