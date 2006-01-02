#ifndef __LDLSHAPELINE_H__
#define __LDLSHAPELINE_H__

#include <LDLoader/LDLActionLine.h>
#include <TCFoundation/TCVector.h>

class LDLShapeLine : public LDLActionLine
{
public:
	virtual bool parse(void);
	virtual int getNumPoints(void) const = 0;
	virtual int getNumControlPoints(void) const { return 0; }
	virtual TCVector *getPoints(void) { return m_points; }
	virtual TCVector *getControlPoints(void) { return NULL; }
	virtual TCULong getColorNumber(void) const { return m_colorNumber; }
	virtual bool isXZPlanar(void) const;
	virtual bool isXZPlanar(const TCFloat *matrix) const;
protected:
	LDLShapeLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLShapeLine(const LDLShapeLine &other);
	virtual void dealloc(void);
	int middleIndex(const TCVector &p1, const TCVector &p2, const TCVector &p3)
		const;
	bool getMatchingPoints(const TCVector *points, int count,
		int *index1 = NULL, int *index2 = NULL);
	bool getMatchingPoints(int *index1 = NULL, int *index2 = NULL);

	TCVector *m_points;
	TCULong m_colorNumber;
};

#endif // __LDLSHAPELINE_H__
