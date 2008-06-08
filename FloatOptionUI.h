#ifndef __FloatOptionUI_H__
#define __FloatOptionUI_H__

#include "NumberOptionUI.h"
#include <TCFoundation/mystring.h>

class FloatOptionUI: public NumberOptionUI
{
public:
	FloatOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~FloatOptionUI(void) {}
	virtual bool validate(ucstring &error);
	virtual void commit(void);

protected:
	TCFloat m_value;
};

#endif // __FloatOptionUI_H__
