#ifndef __TRECOLOREDSHAPEGROUP_H__
#define __TRECOLOREDSHAPEGROUP_H__

#include <TRE/TREShapeGroup.h>

class TREColoredShapeGroup : public TREShapeGroup
{
public:
	TREColoredShapeGroup(void);
	TREColoredShapeGroup(const TREColoredShapeGroup &other);
	virtual TCObject *copy(void);
	virtual int addLine(TCULong color, TCVector *vertices);
	virtual int addTriangle(TCULong color, TCVector *vertices,
		TCVector *normals);
	virtual int addTriangle(TCULong color, TCVector *vertices);
	virtual int addQuad(TCULong color, TCVector *vertices);
	virtual int addQuad(TCULong color, TCVector *vertices, TCVector *normals);
	virtual int addQuadStrip(TCULong color, TCVector *vertices,
		TCVector *normals, int count);
	virtual int addTriangleFan(TCULong color, TCVector *vertices,
		TCVector *normals, int count);
protected:
	virtual ~TREColoredShapeGroup(void);
	virtual void dealloc(void);
	virtual int addStrip(TCULong color, TREShapeType shapeType,
		TCVector *vertices, TCVector *normals, int count);
	virtual int addShape(TREShapeType shapeType, TCULong color,
		TCVector *vertices, int count);
	virtual int addShape(TREShapeType shapeType, TCULong color,
		TCVector *vertices, TCVector *normals, int count);
};

#endif __TRECOLOREDSHAPEGROUP_H__
