#ifndef __LDLCONDITIONALLINELINE_H__
#define __LDLCONDITIONALLINELINE_H__

#include <LDLoader/LDLLineLine.h>

class LDLConditionalLineLine : public LDLLineLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumControlPoints(void) const { return 2; }
	virtual Vector *getControlPoints(void) { return m_controlPoints; }
	virtual LDLLineType getLineType(void) const
	{
		return LDLLineTypeConditionalLine;
	}
protected:
	LDLConditionalLineLine(LDLModel *parentModel, const char *line,
		int lineNumber);
	LDLConditionalLineLine(const LDLConditionalLineLine &other);
	virtual void dealloc(void);

	Vector *m_controlPoints;

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLCONDITIONALLINELINE_H__
