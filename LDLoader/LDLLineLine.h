#ifndef __LDLLINELINE_H__
#define __LDLLINELINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLLineLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 2; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeLine; }
protected:
	LDLLineLine(LDLModel *mainModel, const char *line, int lineNumber);
	LDLLineLine(const LDLLineLine &other);

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLLINELINE_H__
