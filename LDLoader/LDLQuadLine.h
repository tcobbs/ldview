#ifndef __LDLQUADLINE_H__
#define __LDLQUADLINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLQuadLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 4; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeQuad; }
protected:
	LDLQuadLine(LDLModel *parentModel, const char *line, int lineNumber);
	LDLQuadLine(const LDLQuadLine &other);

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLQUADLINE_H__
