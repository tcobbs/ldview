#ifndef __LongOptionUI_H__
#define __LongOptionUI_H__

#include "NumberOptionUI.h"
#include <TCFoundation/mystring.h>

/*
LongOptionUI

This class is used for a long setting.  It displays the name of the setting on
the left and an edit control on the right.
*/
class LongOptionUI: public NumberOptionUI
{
public:
	LongOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~LongOptionUI(void) {}
	virtual bool validate(ucstring &error);
	virtual void commit(void);

protected:
	long m_value;
};

#endif // __LongOptionUI_H__
