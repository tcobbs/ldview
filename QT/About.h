#ifndef __ABOUT_H__
#define __ABOUT_H__

#include "ui_AboutPanel.h"
#include <QLabel>
#include <QString>

class About : public QDialog , Ui::AboutPanel
{
	Q_OBJECT
public:
	About(QDialog *parent = 0)
		 : QDialog(parent) { setupUi(this);connect((QObject *)okButton, SIGNAL(clicked()), this, SLOT(close()));}
	QString getText() {return VersionLabel->text();}
	void setText(QString &s) {VersionLabel->setText(s);}
};

#endif

