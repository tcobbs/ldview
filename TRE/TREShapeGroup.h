#ifndef __TRESHAPEGROUP_H__
#define __TRESHAPEGROUP_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCVector.h>
#include <TRE/TREGL.h>

typedef void (TCObject::*TREScanPointCallback)(const TCVector &point);

typedef TCTypedObjectArray<TCULongArray> TCULongArrayArray;

struct TREVertex;
class TREVertexStore;
class TCVector;

typedef enum
{
	TRESLine			= 0x0001,
	TRESFirst			= TRESLine,
	TRESTriangle		= 0x0002,
	TRESQuad			= 0x0004,
	TRESConditionalLine	= 0x0008,
	TRESBFCTriangle		= 0x0010,
	TRESBFCQuad			= 0x0020,
	TRESTriangleStrip	= 0x0040,
	TRESFirstStrip		= TRESTriangleStrip,
	TRESQuadStrip		= 0x0080,
	TRESTriangleFan		= 0x0100,
	TRESLast			= TRESTriangleFan
} TREShapeType;

class TREShapeGroup : public TCObject
{
public:
	TREShapeGroup(void);
	TREShapeGroup(const TREShapeGroup &other);
	virtual int addLine(TCVector *vertices);
	virtual int addTriangle(TCVector *vertices);
	virtual int addTriangle(TCVector *vertices, TCVector *normals);
	virtual int addQuad(TCVector *vertices);
	virtual int addQuad(TCVector *vertices, TCVector *normals);
	virtual int addQuadStrip(TCVector *vertices, TCVector *normals, int count);
	virtual int addTriangleFan(TCVector *vertices, TCVector *normals, int count);
	virtual int addBFCTriangle(TCVector *vertices);
	virtual int addBFCTriangle(TCVector *vertices, TCVector *normals);
	virtual int addBFCQuad(TCVector *vertices);
	virtual int addBFCQuad(TCVector *vertices, TCVector *normals);
/*
	virtual void addConditionalLine(int index1, int index2, int index3,
		int index4);
	virtual void addBFCTriangle(int index1, int index2, int index3);
	virtual void addBFCQuad(int index1, int index2, int index3, int index4);
*/
	virtual TCULongArray *getIndices(TREShapeType shapeType,
		bool create = false);
	virtual void draw(void);
	virtual void drawNonBFC(void);
	virtual void drawBFC(void);
	virtual void drawLines(void);
	virtual void setVertexStore(TREVertexStore *vertexStore);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual void getMinMax(TCVector& min, TCVector& max, float* matrix);
	virtual void getMaxRadiusSquared(const TCVector &center, float &rSquared,
		float *matrix);
	virtual void scanPoints(TCObject *scanner,
		TREScanPointCallback scanPointCallback, float* matrix);

	static GLenum modeForShapeType(TREShapeType shapeType);
	static int numPointsForShapeType(TREShapeType shapeType);
protected:
	virtual ~TREShapeGroup(void);
	virtual void dealloc(void);
	virtual TCULong getIndex(TREShapeType shapeType);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		int count);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, int count);
	virtual void addShapeIndices(TREShapeType shapeType, int firstIndex,
		int count);
/*
	virtual void addShape(TREShapeType shapeType, int index1, int index2);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3);
	virtual void addShape(TREShapeType shapeType, int index1, int index2,
		int index3, int index4);
*/
	virtual void addShapeType(TREShapeType shapeType, int index);
	virtual void drawShapeType(TREShapeType shapeType);
	virtual void drawStripShapeType(TREShapeType shapeType);
	virtual int addStrip(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, int count);

	virtual void getMinMax(TCULong index, TCVector& min, TCVector& max,
		float* matrix);
	virtual void getMinMax(const TREVertex &vertex, TCVector& min, TCVector& max);
	virtual void getMinMax(TCULongArray *indices, TCVector& min, TCVector& max,
		float* matrix);
	virtual void getStripMinMax(TCULongArray *indices, TCVector& min, TCVector& max,
		float* matrix);

	virtual void getMaxRadiusSquared(TCULong index, const TCVector &center,
		float &rSquared, float* matrix);
	virtual void getMaxRadiusSquared(const TREVertex &vertex,
		const TCVector &center, float &rSquared);
	virtual void getMaxRadiusSquared(TCULongArray *indices,
		const TCVector &center, float &rSquared, float* matrix);
	virtual void getStripMaxRadiusSquared(TCULongArray *indices,
		const TCVector &center, float &rSquared, float* matrix);

	virtual void scanPoints(TCULong index, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float* matrix);
	virtual void scanPoints(const TREVertex &vertex, TCObject *scanner,
		TREScanPointCallback scanPointCallback);
	virtual void scanPoints(TCULongArray *indices, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float* matrix);
	virtual void scanStripPoints(TCULongArray *indices, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float* matrix);

	TREVertexStore *m_vertexStore;
	TCULongArrayArray *m_indices;
	TCULong m_shapesPresent;
};

#endif __TRESHAPEGROUP_H__
