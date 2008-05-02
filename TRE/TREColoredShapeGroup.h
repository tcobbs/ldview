#ifndef __TRECOLOREDSHAPEGROUP_H__
#define __TRECOLOREDSHAPEGROUP_H__

#include <TRE/TREShapeGroup.h>

class TREColoredShapeGroup : public TREShapeGroup
{
public:
	TREColoredShapeGroup(void);
	TREColoredShapeGroup(const TREColoredShapeGroup &other);
	virtual TCObject *copy(void) const;
	virtual int addLine(TCULong color, const TCVector *vertices);
	virtual int addTriangle(TCULong color, const TCVector *vertices,
		const TCVector *normals);
	virtual int addTriangle(TCULong color, const TCVector *vertices);
	virtual int addTriangle(TCULong color, const TCVector *vertices,
		const TCVector *normals, const TCVector *textureCoords);
	virtual int addQuad(TCULong color, const TCVector *vertices);
	virtual int addQuad(TCULong color, const TCVector *vertices,
		const TCVector *normals);
	virtual int addQuadStrip(TCULong color, const TCVector *vertices,
		const TCVector *normals, int count);
	virtual int addTriangleFan(TCULong color, const TCVector *vertices,
		const TCVector *normals, int count);
	virtual void transferColoredTransparent(const TCFloat *matrix);
	virtual void cleanupTransparent(void);
protected:
	virtual ~TREColoredShapeGroup(void);
	virtual void dealloc(void);
	virtual int addStrip(TCULong color, TREShapeType shapeType,
		const TCVector *vertices, const TCVector *normals, int count);
	virtual int addShape(TREShapeType shapeType, TCULong color,
		const TCVector *vertices, int count);
	virtual int addShape(TREShapeType shapeType, TCULong color,
		const TCVector *vertices, const TCVector *normals, int count);
	virtual int addShape(TREShapeType shapeType, TCULong color,
		const TCVector *vertices, const TCVector *normals,
		const TCVector *textureCoords, int count);
	virtual void transferColoredTransparent(TREShapeType shapeType,
		TCULongArray *indices, TCULongArray *transparentIndices,
		const TCFloat *matrix);
	virtual TCULongArray *getTransparentIndices(TREShapeType shapeType);
	virtual TCULongArray *getTransparentStripCounts(TREShapeType shapeType);
	virtual bool isColored(void) { return true; }

	TCULongArrayArray *m_transparentIndices;
	TCULongArrayArray *m_transparentStripCounts;
};

#endif // __TRECOLOREDSHAPEGROUP_H__
