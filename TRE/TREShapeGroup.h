#ifndef __TRESHAPEGROUP_H__
#define __TRESHAPEGROUP_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TRE/TREGL.h>

typedef TCTypedObjectArray<TCULongArray> TCULongArrayArray;

class TREVertexStore;
class Vector;

typedef enum
{
	TRESFirst			= 0x0001,
	TRESLine			= TRESFirst,
	TRESTriangle		= 0x0002,
	TRESQuad			= 0x0004,
	TRESConditionalLine	= 0x0008,
	TRESBFCTriangle		= 0x0010,
	TRESBFCQuad			= 0x0020,
	TRESLast			= TRESBFCQuad
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
	virtual TCULongArray *getIndices(TREShapeType shapeType,
		bool create = false);
	virtual void draw(void);
	virtual void setVertexStore(TREVertexStore *vertexStore);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }

	static GLenum modeForShapeType(TREShapeType shapeType);
	static int numPointsForShapeType(TREShapeType shapeType);
protected:
	virtual ~TREShapeGroup(void);
	virtual void dealloc(void);
	virtual TCULong getIndex(TREShapeType shapeType);
	virtual void addShape(TREShapeType shapeType, int index1, int index2);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3, int index4);
	virtual void addShapeType(TREShapeType shapeType, int index);
	virtual void drawShapeType(TREShapeType shapeType);

	TREVertexStore *m_vertexStore;
	TCULongArrayArray *m_indices;
	TCULong m_shapesPresent;
};

#endif __TRESHAPEGROUP_H__
