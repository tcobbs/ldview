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
	virtual TCObject *copy(void);
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
	virtual TCULongArray *getStripCounts(TREShapeType shapeType,
		bool create = false);
	virtual void draw(void);
	virtual void drawNonBFC(void);
	virtual void drawBFC(void);
	virtual void drawLines(void);
	virtual void setVertexStore(TREVertexStore *vertexStore);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual void scanPoints(TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void unshrinkNormals(float *matrix, float *unshrinkMatrix);
	virtual void unMirror(void);
	virtual void invert(void);

	static GLenum modeForShapeType(TREShapeType shapeType);
	static int numPointsForShapeType(TREShapeType shapeType);
	static void setGlMultiDrawElementsEXT(PFNGLMULTIDRAWELEMENTSEXTPROC value)
	{
		glMultiDrawElementsEXT = value;
	}
protected:
	virtual ~TREShapeGroup(void);
	virtual void dealloc(void);
	virtual TCULong getShapeTypeIndex(TREShapeType shapeType);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		int count);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, int count);
	virtual void addShapeIndices(TREShapeType shapeType, int firstIndex,
		int count);
	virtual void addShapeStripCount(TREShapeType shapeType, int count);
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
	virtual void initMultiDrawIndices(void);
	virtual void deleteMultiDrawIndices(void);
	virtual void invertShapes(TCULongArray *oldIndices,
		TCULongArray *newIndices);
	virtual int flipNormal(int index);

	virtual void scanPoints(TCULong index, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void scanPoints(const TREVertex &vertex, TCObject *scanner,
		TREScanPointCallback scanPointCallback);
	virtual void scanPoints(TCULongArray *indices, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void scanStripPoints(TCULongArray *indices,
		TCULongArray *stripCounts, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);

	virtual void unshrinkNormal(TCULong index, float *matrix,
		float *unshrinkMatrix);
	virtual void unshrinkNormals(TCULongArray *indices, float *matrix,
		float *unshrinkMatrix);
	virtual void unshrinkStripNormals(TCULongArray *indices,
		TCULongArray *stripCounts, float *matrix,
		float *unshrinkMatrix);

	TREVertexStore *m_vertexStore;
	TCULongArrayArray *m_indices;
	TCULongArrayArray *m_stripCounts;
	TCULong ***m_multiDrawIndices;
	TCULong m_shapesPresent;

	static PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;
};

#endif __TRESHAPEGROUP_H__
