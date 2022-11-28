#ifndef __OPENGLEXTENSION_H__
#define __OPENGLEXTENSION_H__

#include "ui_OpenGLExtensionsPanel.h"
#include <QLabel>
#include <QString>

class OpenGLExtensions : public QMainWindow , Ui::OpenGLExtensionsPanel
{
public:
	OpenGLExtensions(QWidget *parent = 0)
		 : QMainWindow(parent) { setupUi(this);}
	void setText(QString text) {extensionsBox->setText(text);}
};

#endif

