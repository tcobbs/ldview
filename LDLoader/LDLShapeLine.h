#ifndef __LDLSHAPELINE_H__
#define __LDLSHAPELINE_H__

#include <LDLoader/LDLActionLine.h>
#include <TCFoundation/TCVector.h>

class LDLShapeLine : public LDLActionLine
{
public:
	virtual bool parse(void);
	virtual int getNumPoints(void) const = 0;
	virtual int getNumControlPoints(void) { return 0; }
	virtual TCVector *getPoints(void) { return m_points; }
	virtual TCVector *getControlPoints(void) { return NULL; }
	virtual TCULong getColorNumber(void) { return m_colorNumber; }
protected:
	LDLShapeLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLShapeLine(const LDLShapeLine &other);
	virtual void dealloc(void);
	int middleIndex(const TCVector &p1, const TCVector &p2, const TCVector &p3);

	TCVector *m_points;
	TCULong m_colorNumber;
};

#endif // __LDLSHAPELINE_H__
