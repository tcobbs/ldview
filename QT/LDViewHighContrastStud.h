#ifndef __LDVIEWHIGHCONTRASTSTUD_H__
#define __LDVIEWHIGHCONTRASTSTUD_H__

#include "ui_HighContrastStud.h"
#include <QDialog>
#include <LDLib/LDPreferences.h>
class Preferences;

class HighContrastStud : public QDialog , Ui::HighContrastStudPanel
{
	Q_OBJECT
public:
	HighContrastStud(QWidget *parent);
	~HighContrastStud(void);
public slots:
	void doOk(void);
	void doCancel(void);
	void doLineColorValueSlider(int);
	void doStudyCylinderColorCheckBox(void);
	void doPartEdgeColorCheckBox(void);
	void doBlackPartEdgeColorCheckBox(void);
	void doDarkPartEdgeColorCheckBox(void);
	void doStudCylinderColorButton(void);
	void doPartEdgeColorButton(void);
	void doBlackPartEdgeColorButton(void);
	void doDarkPartEdgeColorButton(void);
	void resetLineColor(void);
	void resetStudCylinderColor(void);
	void resetPartEdgeColor(void);
	void resetBlackPartEdgeColor(void);
	void resetDarkPartEdgeColor(void);
protected:
	void reflectSettings();
};

#endif

