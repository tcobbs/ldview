#ifndef __LDLLINELINE_H__
#define __LDLLINELINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLTriangleLine;

class LDLLineLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 2; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeLine; }
protected:
	LDLLineLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLLineLine(const LDLLineLine &other);

	friend LDLFileLine;		// Needed because constructors are protected.
	friend LDLTriangleLine;	// Needed because constructors are protected, and
							// some bad triangles can be converted to a line.
};

#endif // __LDLLINELINE_H__
