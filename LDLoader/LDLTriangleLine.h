#ifndef __LDLTRIANGLELINE_H__
#define __LDLTRIANGLELINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLTriangleLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 3; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeTriangle; }
protected:
	LDLTriangleLine(LDLModel *parentModel, const char *line, int lineNumber);
	LDLTriangleLine(const LDLTriangleLine &other);

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLTRIANGLELINE_H__
