#ifndef __LDLTRIANGLELINE_H__
#define __LDLTRIANGLELINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLQuadLine;
class LDLLineLine;

class LDLTriangleLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 3; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeTriangle; }
	virtual LDLFileLineArray *getReplacementLines(void);
protected:
	LDLTriangleLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLTriangleLine(const LDLTriangleLine &other);
	void checkForColinearPoints(void);
	int getColinearIndex(void);
	LDLFileLineArray *removeColinearPoint(void);
	LDLLineLine *newLineLine(const TCVector &p1, const TCVector &p2);

	int m_colinearIndex;

	friend LDLFileLine; // Needed because constructors are protected.
	friend LDLQuadLine; // Needed because constructors are protected, and bad
						// quads can be converted to one or two triangles.
};

#endif // __LDLTRIANGLELINE_H__
