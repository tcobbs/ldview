#ifndef __LDLSHAPELINE_H__
#define __LDLSHAPELINE_H__

#include <LDLoader/LDLActionLine.h>
#include <LDLib/Vector.h>

class LDLShapeLine : public LDLActionLine
{
public:
	virtual bool parse(void);
	virtual int getNumPoints(void) const = 0;
	virtual int getNumControlPoints(void) { return 0; }
	virtual Vector *getPoints(void) { return m_points; }
	virtual Vector *getControlPoints(void) { return NULL; }
	virtual TCULong getColorNumber(void) { return m_colorNumber; }
protected:
	LDLShapeLine(LDLModel *parentModel, const char *line, int lineNumber);
	LDLShapeLine(const LDLShapeLine &other);
	virtual void dealloc(void);

	Vector *m_points;
	TCULong m_colorNumber;
};

#endif // __LDLSHAPELINE_H__
