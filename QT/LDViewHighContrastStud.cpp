#include "LDViewHighContrastStud.h"
#include <QColorDialog>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include "Preferences.h"

HighContrastStud::HighContrastStud(QWidget *parent)
	:QDialog(parent),HighContrastStudPanel()
{
	setupUi(this);
	connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
	connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
	connect( lineColorValueSlider ,SIGNAL( valueChanged(int) ), this, SLOT( doLineColorValueSlider(int) ) );
	connect( studCylinderColorCheckBox , SIGNAL( stateChanged(int) ), this, SLOT( doStudyCylinderColorCheckBox() ) );
	connect( partEdgeColorCheckBox , SIGNAL( stateChanged(int) ), this, SLOT( doPartEdgeColorCheckBox() ) );
	connect( blackPartEdgeColorCheckBox , SIGNAL( stateChanged(int) ), this, SLOT( doBlackPartEdgeColorCheckBox() ) );
	connect( darkPartEdgeColorCheckBox , SIGNAL( stateChanged(int) ), this, SLOT( doDarkPartEdgeColorCheckBox() ) );
	connect( studCylinderColorButton , SIGNAL( clicked() ) , this, SLOT ( doStudCylinderColorButton() ) );
	connect( partEdgeColorButton, SIGNAL( clicked() ) , this, SLOT ( doPartEdgeColorButton() ) );
	connect( blackPartEdgeColorButton , SIGNAL( clicked() ) , this, SLOT ( doBlackPartEdgeColorButton() ) );
	connect( darkPartEdgeColorButton, SIGNAL( clicked() ) , this, SLOT ( doDarkPartEdgeColorButton() ) );

	connect( lineColorResetButton, SIGNAL( clicked() ) , this, SLOT ( resetLineColor() ) );
	connect( studCylinderColorResetButton, SIGNAL( clicked() ) , this, SLOT ( resetStudCylinderColor() ) );
	connect( partEdgeColorResetButton, SIGNAL( clicked() ) , this, SLOT ( resetPartEdgeColor() ) );
	connect( blackPartEdgeColorResetButton, SIGNAL( clicked() ) , this, SLOT ( resetBlackPartEdgeColor() ) );
	connect( darkPartEdgeColorResetButton, SIGNAL( clicked() ) , this, SLOT ( resetDarkPartEdgeColor() ) );
	reflectSettings();
}

void HighContrastStud::reflectSettings()
{
//	studCylinderColorCheckBox->setChecked(preferences->getStudCylinderColorEnabled());
//	partEdgeColorCheckBox->setChecked(preferences->getPartEdgeColorEnabled());
//	blackPartEdgeColorCheckBox->setChecked(preferences->getBlackEdgeColorEnabled());
//	darkPartEdgeColorCheckBox->setChecked(preferences->getDarkEdgeColorEnabled());
	studCylinderColorCheckBox->setChecked(TCUserDefaults::boolForKey(STUD_CYLINDER_COLOR_ENABLED_KEY));
	partEdgeColorCheckBox->setChecked(TCUserDefaults::boolForKey(PART_EDGE_COLOR_ENABLED_KEY));
	blackPartEdgeColorCheckBox->setChecked(TCUserDefaults::boolForKey(BLACK_EDGE_COLOR_ENABLED_KEY));
	darkPartEdgeColorCheckBox->setChecked(TCUserDefaults::boolForKey(DARK_EDGE_COLOR_ENABLED_KEY));

	float f;
	f=TCUserDefaults::floatForKey(PART_COLOR_LD_INDEX_KEY);
	lineColorValueLabel->setText(QString::number(f));
	lineColorValueSlider->setValue(f*10.0);
	bool b;
	b=studCylinderColorCheckBox->isChecked();
	studCylinderColorButton->setEnabled(b);
	studCylinderColorResetButton->setEnabled(b);
	b=partEdgeColorCheckBox->isChecked();
	partEdgeColorButton->setEnabled(b);
	partEdgeColorResetButton->setEnabled(b);
	b=blackPartEdgeColorCheckBox->isChecked();
	blackPartEdgeColorButton->setEnabled(b);
	blackPartEdgeColorResetButton->setEnabled(b);
	b=darkPartEdgeColorCheckBox->isChecked();
	darkPartEdgeColorButton->setEnabled(b);
	darkPartEdgeColorResetButton->setEnabled(b);

	int cr,cg,cb,ca;
	QColor qc;
	QPalette palette;
	std::string s;
	s = TCUserDefaults::stringForKey(STUD_CYLINDER_COLOR_KEY, "27,42,52,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		studCylinderColorButton->setPalette(palette);
	}
	s = TCUserDefaults::stringForKey(PART_EDGE_COLOR_KEY, "0,0,0,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		partEdgeColorButton->setPalette(palette);
	}
	s = TCUserDefaults::stringForKey(BLACK_EDGE_COLOR_KEY, "255,255,255,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		blackPartEdgeColorButton->setPalette(palette);
	}
	s = TCUserDefaults::stringForKey(DARK_EDGE_COLOR_KEY, "27,42,52,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		darkPartEdgeColorButton->setPalette(palette);
	}
}

void HighContrastStud::doOk()
{
	TCUserDefaults::setBoolForKey(studCylinderColorCheckBox->isChecked(), STUD_CYLINDER_COLOR_ENABLED_KEY);
	TCUserDefaults::setBoolForKey(partEdgeColorCheckBox->isChecked(), PART_EDGE_COLOR_ENABLED_KEY, false);
	TCUserDefaults::setBoolForKey(blackPartEdgeColorCheckBox->isChecked(),BLACK_EDGE_COLOR_ENABLED_KEY, false);
	TCUserDefaults::setBoolForKey(darkPartEdgeColorCheckBox->isChecked(),DARK_EDGE_COLOR_ENABLED_KEY, false);
	TCUserDefaults::setFloatForKey(lineColorValueLabel->text().toFloat(),PART_COLOR_LD_INDEX_KEY, false);
	QColor color;
	QString str;
	int r,g,b;
	color = studCylinderColorButton->palette().color(QPalette::Button);
	color.getRgb(&r, &g, &b);
	str = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(color.alpha());
	TCUserDefaults::setStringForKey(str.toLatin1().constData(),STUD_CYLINDER_COLOR_KEY);
	color = partEdgeColorButton->palette().color(QPalette::Button);
	color.getRgb(&r, &g, &b);
	str = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(color.alpha());
	TCUserDefaults::setStringForKey(str.toLatin1().constData(),PART_EDGE_COLOR_KEY);
	color = blackPartEdgeColorButton->palette().color(QPalette::Button);
	color.getRgb(&r, &g, &b);
	str = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(color.alpha());
	TCUserDefaults::setStringForKey(str.toLatin1().constData(),BLACK_EDGE_COLOR_KEY);
	color = darkPartEdgeColorButton->palette().color(QPalette::Button);
	color.getRgb(&r, &g, &b);
	str = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(color.alpha());
	TCUserDefaults::setStringForKey(str.toLatin1().constData(),DARK_EDGE_COLOR_KEY);
	QDialog::close();
}

void HighContrastStud::doCancel()
{
	QDialog::close();
}

void HighContrastStud::doLineColorValueSlider(int i)
{
	lineColorValueLabel->setText(QString::number(i/10.0));
}

void HighContrastStud::doStudyCylinderColorCheckBox()
{
	bool b=studCylinderColorCheckBox->isChecked();
	studCylinderColorButton->setEnabled(b);
	studCylinderColorResetButton->setEnabled(b);
}

void HighContrastStud::doPartEdgeColorCheckBox()
{
	bool b=partEdgeColorCheckBox->isChecked();
	partEdgeColorButton->setEnabled(b);
	partEdgeColorResetButton->setEnabled(b);
}

void HighContrastStud::doBlackPartEdgeColorCheckBox()
{
	bool b=blackPartEdgeColorCheckBox->isChecked();
	blackPartEdgeColorButton->setEnabled(b);
	blackPartEdgeColorResetButton->setEnabled(b);
}

void HighContrastStud::doDarkPartEdgeColorCheckBox()
{
	bool b=darkPartEdgeColorCheckBox->isChecked();
	darkPartEdgeColorButton->setEnabled(b);
	darkPartEdgeColorResetButton->setEnabled(b);
}

void HighContrastStud::doStudCylinderColorButton()
{
	QColor color = QColorDialog::getColor(studCylinderColorButton->palette().color(QPalette::Button));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		studCylinderColorButton->setPalette(palette);
	}
}

void HighContrastStud::doPartEdgeColorButton()
{
	QColor color = QColorDialog::getColor(partEdgeColorButton->palette().color(QPalette::Button));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		partEdgeColorButton->setPalette(palette);
	}
}

void HighContrastStud::doBlackPartEdgeColorButton()
{
	QColor color = QColorDialog::getColor(blackPartEdgeColorButton->palette().color(QPalette::Button));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		blackPartEdgeColorButton->setPalette(palette);
	}
}

void HighContrastStud::doDarkPartEdgeColorButton()
{
	QColor color = QColorDialog::getColor(darkPartEdgeColorButton->palette().color(QPalette::Button));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		darkPartEdgeColorButton->setPalette(palette);
	}
}

void HighContrastStud::resetLineColor()
{
	float f;
	f=TCUserDefaults::floatForKey(PART_COLOR_LD_INDEX_KEY);
	lineColorValueSlider->setValue(f*10.0);
}

void HighContrastStud::resetStudCylinderColor()
{
	int cr,cg,cb,ca;
	QColor qc;
	QPalette palette;
	std::string s;
	s = TCUserDefaults::stringForKey(STUD_CYLINDER_COLOR_KEY, "27,42,52,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		studCylinderColorButton->setPalette(palette);
	}
}

void HighContrastStud::resetPartEdgeColor()
{
	int cr,cg,cb,ca;
	QColor qc;
	QPalette palette;
	std::string s;
	s = TCUserDefaults::stringForKey(PART_EDGE_COLOR_KEY, "0,0,0,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		partEdgeColorButton->setPalette(palette);
	}}

void HighContrastStud::resetBlackPartEdgeColor()
{
	int cr,cg,cb,ca;
	QColor qc;
	QPalette palette;
	std::string s;
	s = TCUserDefaults::stringForKey(BLACK_EDGE_COLOR_KEY, "255,255,255,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		blackPartEdgeColorButton->setPalette(palette);
	}}

void HighContrastStud::resetDarkPartEdgeColor()
{
	int cr,cg,cb,ca;
	QColor qc;
	QPalette palette;
	std::string s;
	s = TCUserDefaults::stringForKey(DARK_EDGE_COLOR_KEY, "27,42,52,255");
	if (sscanf(s.c_str(), "%d,%d,%d,%d", &cr, &cg, &cb, &ca) == 4)
	{
		qc=QColor(cr,cg,cb);
		palette.setColor(QPalette::Button, qc);
		darkPartEdgeColorButton->setPalette(palette);
	}}


HighContrastStud::~HighContrastStud()
{
}

