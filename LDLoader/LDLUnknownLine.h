#ifndef __LDLUNKNOWNLINE_H__
#define __LDLUNKNOWNLINE_H__

#include <LDLoader/LDLFileLine.h>

class LDLUnknownLine : public LDLFileLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual LDLLineType getLineType(void) const { return LDLLineTypeUnknown; }
protected:
	LDLUnknownLine(LDLModel *mainModel, const char *line, int lineNumber);
	LDLUnknownLine(const LDLUnknownLine &other);

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLUNKNOWNLINE_H__
