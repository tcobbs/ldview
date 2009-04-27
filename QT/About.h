#include "AboutPanel.h"
#include <QLabel.h>
#include <QString.h>

class About : public AboutPanel
{
	Q_OBJECT
public:
	About(QDialog *parent = 0)
         : AboutPanel(parent) {}
	QLabel *getVersionLabel() {return VersionLabel;}
	QString getText() {return VersionLabel->text();}
	void setText(QString &s) {VersionLabel->setText(s);}
};