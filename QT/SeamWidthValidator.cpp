#include "SeamWidthValidator.h"
#include <qstring.h>

SeamWidthValidator::SeamWidthValidator(QObject *parent, const char *name)
	:QValidator(parent, name)
{
}

void SeamWidthValidator::fixup(QString & /*input*/) const
{
}

QValidator::State SeamWidthValidator::validate(QString &input, int & /*pos*/)
	const
{
	bool ok;
	double value = input.toDouble(&ok);

	if (ok)
	{
		return Acceptable;
	}
	else
	{
		return Invalid;
	}
}

