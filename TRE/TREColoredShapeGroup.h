#ifndef __TRECOLOREDSHAPEGROUP_H__
#define __TRECOLOREDSHAPEGROUP_H__

#include <TRE/TREShapeGroup.h>

class TREColoredShapeGroup : public TREShapeGroup
{
public:
	TREColoredShapeGroup(void);
	TREColoredShapeGroup(const TREColoredShapeGroup &other);
	virtual int addTriangle(TCULong color, Vector *vertices);
	virtual int addQuad(TCULong color, Vector *vertices);
	virtual void setup(void);
protected:
	virtual ~TREColoredShapeGroup(void);
	virtual void dealloc(void);
	virtual void addShapeType(TREShapeType shapeType, int index);
	virtual void addColor(TREShapeType shapeType, TCULong color, int count);
	virtual void drawShapeType(TREShapeType shapeType);
	virtual TCULongArray *getColors(TREShapeType shapeType);

	TCULongArrayArray *m_colors;
};

#endif __TRECOLOREDSHAPEGROUP_H__
