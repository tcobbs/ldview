#ifndef __LDLSHAPELINE_H__
#define __LDLSHAPELINE_H__

#include <LDLoader/LDLFileLine.h>
#include <LDLib/Vector.h>

class LDLShapeLine : public LDLFileLine
{
public:
	virtual bool parse(void);
	virtual int getNumPoints(void) const = 0;
	virtual int getNumControlPoints(void) { return 0; }
	virtual Vector *getPoints(void) { return m_points; }
	virtual Vector *getControlPoints(void) { return NULL; }
protected:
	LDLShapeLine(LDLModel *mainModel, const char *line, int lineNumber);
	LDLShapeLine(const LDLShapeLine &other);
	virtual void dealloc(void);

	Vector *m_points;
	int m_colorNumber;

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLSHAPELINE_H__
