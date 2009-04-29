#ifndef __ABOUT_H__
#define __ABOUT_H__

#include "AboutPanel.h"
#include <qlabel.h>
#include <qstring.h>

class About : public AboutPanel
{
	Q_OBJECT
public:
	About(QDialog *parent = 0)
         : AboutPanel(parent) {connect((QObject *)okButton, SIGNAL(clicked()), this, SLOT(close()));}
	QLabel *getVersionLabel() {return VersionLabel;}
	QString getText() {return VersionLabel->text();}
	void setText(QString &s) {VersionLabel->setText(s);}
};

#endif