#ifndef __LDLQUADLINE_H__
#define __LDLQUADLINE_H__

#include <LDLoader/LDLShapeLine.h>

class LDLTriangleLine;

class LDLQuadLine : public LDLShapeLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual int getNumPoints(void) const { return 4; }
	virtual LDLLineType getLineType(void) const { return LDLLineTypeQuad; }
	virtual LDLFileLineArray *getReplacementLines(void);
protected:
	LDLQuadLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLQuadLine(const LDLQuadLine &other);
	void swapPointsIfNeeded(void);
	void checkForColinearPoints(void);
	bool swapNeeded(int index1, int index2, int index3, int index4);
	void swapPoints(int index1, int index2);
	void rotPoints(int index1, int index2, int index3);
	int getColinearIndex(void);
	LDLFileLineArray *removePoint(int index);
	LDLFileLineArray *removeMatchingPoint(void);
	LDLFileLineArray *removeColinearPoint(void);
	LDLFileLineArray *splitConcaveQuad(void);
	LDLFileLineArray *splitConcaveQuad(int index1, int index2, int index3,
		int index4);
	void reportQuadSplit(bool flat, const TCVector& q1, const TCVector& q2,
		const TCVector& q3, const TCVector& q4, const TCVector& t1,
		const TCVector& t2, const TCVector& t3, const TCVector& t4,
		const TCVector& t5, const TCVector& t6);
	void reportBadVertexOrder(int index1, int index2, int index3, int index4);
	LDLTriangleLine *newTriangleLine(const TCVector &p1, const TCVector &p2,
		const TCVector &p3);

	int m_colinearIndex;
	int m_matchingIndex;

	friend class LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLQUADLINE_H__
