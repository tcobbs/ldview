#ifndef __SEAMWIDTHVALIDATOR_H__
#define __SEAMWIDTHVALIDATOR_H__

#include <qvalidator.h>

class SeamWidthValidator : public QValidator
{
	Q_OBJECT

public:
	SeamWidthValidator(QObject *parent, const char *name = 0);
	void fixup(QString &input) const;
	State validate(QString &input, int &pos) const;
};

#endif //  __SEAMWIDTHVALIDATOR_H__
