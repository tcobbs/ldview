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
	virtual int addQuadStrip(TCULong color, Vector *vertices, Vector *normals,
		int count);
	virtual int addTriangleFan(TCULong color, Vector *vertices, Vector *normals,
		int count);
protected:
	virtual ~TREColoredShapeGroup(void);
	virtual void dealloc(void);
	virtual int addStrip(TCULong color, TREShapeType shapeType,
		Vector *vertices, Vector *normals, int count);
};

#endif __TRECOLOREDSHAPEGROUP_H__
