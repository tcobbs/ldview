#ifndef __TRESHAPEGROUP_H__
#define __TRESHAPEGROUP_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TCFoundation/TCTypedObjectArray.h>

typedef TCTypedObjectArray<TCULongArray> TCULongArrayArray;

class TREVertexArray;
struct TREVertex;
class Vector;

typedef enum
{
	TRESLine			= 0x0001,
	TRESTriangle		= 0x0002,
	TRESQuad			= 0x0004,
	TRESConditionalLine	= 0x0008,
	TRESBFCTriangle		= 0x0010,
	TRESBFCQuad			= 0x0020
} TREShapeType;

class TREShapeGroup : public TCObject
{
public:
	TREShapeGroup(void);
	TREShapeGroup(const TREShapeGroup &other);
	virtual int addLine(Vector *vertices);
	virtual int addTriangle(Vector *vertices);
	virtual int addQuad(Vector *vertices);
	virtual void addConditionalLine(int index1, int index2, int index3,
		int index4);
	virtual void addBFCTriangle(int index1, int index2, int index3);
	virtual void addBFCQuad(int index1, int index2, int index3, int index4);
	virtual TCULongArray *getIndices(TREShapeType shapeType);
	virtual void draw(void);
	virtual void setup(void);
protected:
	virtual ~TREShapeGroup(void);
	virtual void dealloc(void);
	virtual TCULong getIndex(TREShapeType shapeType);
	virtual TCULongArray *getIndices(TREShapeType shapeType, bool create);
	virtual void addShape(TREShapeType shapeType, int index1, int index2);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3, int index4);
	virtual int addVertices(Vector *points, int count);
	virtual int addVertices(TREVertexArray *vertices, Vector *points,
		int count);
	Vector calcNormal(Vector *points, bool normalize = true);
	virtual void initVertex(TREVertex &vertex, Vector &point);
	virtual void addShapeType(TREShapeType shapeType, int index);
	virtual void drawShapeType(TREShapeType shapeType);

	TREVertexArray *m_vertices;
	TREVertexArray *m_normals;
	TCULongArrayArray *m_indices;
	TCULong m_shapesPresent;
};

#endif __TRESHAPEGROUP_H__
