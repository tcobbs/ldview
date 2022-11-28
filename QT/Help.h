#ifndef __HELP_H__
#define __HELP_H__

#include "ui_HelpPanel.h"
#include <QLabel>
#include <QString>
#include <QDialog>

class Help : public QDialog , Ui::HelpPanel
{
public:
	Help(QWidget *parent = 0)
		 : QDialog(parent) { setupUi(this);
	HelpTextBrowser->setReadOnly(true);	}
	void setText(QString &text) {HelpTextBrowser->setText(text);}
};

#endif

