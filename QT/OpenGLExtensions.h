#ifndef __OPENGLEXTENSION_H__
#define __OPENGLEXTENSION_H__

#include "ui_OpenGLExtensionsPanel.h"
#include <qlabel.h>
#include <qstring.h>

class OpenGLExtensions : public Q3MainWindow , Ui::OpenGLExtensionsPanel
{
public:
	OpenGLExtensions(QWidget *parent = 0)
         : Q3MainWindow(parent) { setupUi(this);}
	void setText(QString text) {extensionsBox->setText(text);}
};

#endif

