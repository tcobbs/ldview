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
class TREMainModel;
class TREVertexArray;
class TCVector;

typedef enum
{
	TRESLine			= 0x0001,
	TRESFirst			= TRESLine,
	TRESConditionalLine	= 0x0002,
	TRESTriangle		= 0x0004,
	TRESQuad			= 0x0008,
	TRESTriangleStrip	= 0x0010,
	TRESFirstStrip		= TRESTriangleStrip,
	TRESQuadStrip		= 0x0020,
	TRESTriangleFan		= 0x0040,
	TRESLast			= TRESTriangleFan
} TREShapeType;

class TREShapeGroup : public TCObject
{
public:
	TREShapeGroup(void);
	TREShapeGroup(const TREShapeGroup &other);
	virtual TCObject *copy(void);
	virtual int addLine(TCVector *vertices);
	virtual int addConditionalLine(TCVector *vertices, TCVector *controlPoints);
	virtual int addTriangle(TCVector *vertices);
	virtual int addTriangle(TCVector *vertices, TCVector *normals);
	virtual int addTriangle(TCVector *vertices, TCVector *normals,
		TCVector *textureCoords);
	virtual int addQuad(TCVector *vertices);
	virtual int addQuad(TCVector *vertices, TCVector *normals);
	virtual int addTriangleStrip(TCVector *vertices, TCVector *normals,
		int count);
	virtual int addQuadStrip(TCVector *vertices, TCVector *normals, int count);
	virtual int addTriangleFan(TCVector *vertices, TCVector *normals,
		int count);
	virtual int addTriangleFan(TCVector *vertices, TCVector *normals,
		TCVector *textureCoords, int count);
	virtual TCULongArray *getIndices(TREShapeType shapeType,
		bool create = false);
	virtual TCULongArray *getControlPointIndices(bool create = false);
	virtual TCULongArray *getStripCounts(TREShapeType shapeType,
		bool create = false);
	virtual void draw(void);
	virtual void drawLines(void);
	virtual void drawConditionalLines(void);
	virtual void setVertexStore(TREVertexStore *vertexStore);
	virtual TREVertexStore *getVertexStore(void) { return m_vertexStore; }
	virtual void scanPoints(TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void unshrinkNormals(float *matrix, float *unshrinkMatrix);
	virtual void unMirror(void);
	virtual void invert(void);
	virtual void transferTransparent(TCULong color, const float *matrix);
	virtual void flatten(TREShapeGroup *srcShapes, float *matrix, TCULong color,
		bool colorSet);
	void setMainModel(TREMainModel *value) { m_mainModel = value; }
	TREMainModel *getMainModel(void) { return m_mainModel; }

	static GLenum modeForShapeType(TREShapeType shapeType);
	static int numPointsForShapeType(TREShapeType shapeType);
	static void setGlMultiDrawElementsEXT(PFNGLMULTIDRAWELEMENTSEXTPROC value)
	{
		glMultiDrawElementsEXT = value;
	}
	static bool isTransparent(TCULong color, bool hostFormat);
	static void transformVertex(TREVertex &vertex, float *matrix);
	static void transformNormal(TREVertex &normal, float *matrix);
protected:
	virtual ~TREShapeGroup(void);
	virtual void dealloc(void);
	virtual TCULong getShapeTypeIndex(TREShapeType shapeType);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		int count);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, int count);
	virtual int addShape(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, TCVector *textureCoords, int count);
	virtual void addShapeIndices(TREShapeType shapeType, int firstIndex,
		int count);
	virtual void addIndices(TCULongArray *indices, int firstIndex, int count);
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
	virtual void drawNormals(TCULongArray *indexArray);
	virtual void drawStripShapeType(TREShapeType shapeType);
	virtual int addStrip(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, int count);
	virtual int addStrip(TREShapeType shapeType, TCVector *vertices,
		TCVector *normals, TCVector *textureCoords, int count);
	virtual void initMultiDrawIndices(void);
	virtual void deleteMultiDrawIndices(void);
	virtual void invertShapes(TCULongArray *oldIndices,
		TCULongArray *newIndices);
	virtual int flipNormal(int index);
	virtual void transferTriangle(TCULong color, TCULong index0, TCULong index1,
		TCULong index2, const float *matrix);
	virtual void transferQuadStrip(int shapeTypeIndex, TCULong color,
		int offset, int stripCount, const float *matrix);
	virtual void transferTriangleStrip(int shapeTypeIndex, TCULong color,
		int offset, int stripCount, const float *matrix);
	virtual void transferTriangleFan(int shapeTypeIndex, TCULong color,
		int offset, int stripCount, const float *matrix);
	virtual void transferTransparent(TCULong color, TREShapeType shapeType,
		TCULongArray *indices, const float *matrix);
	virtual bool shouldDrawConditional(TCULong index1, TCULong index2,
		TCULong cpIndex1, TCULong cpIndex2, const float *matrix);

	virtual void scanPoints(TCULong index, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void scanPoints(const TREVertex &vertex, TCObject *scanner,
		TREScanPointCallback scanPointCallback);
	virtual void scanPoints(TCULongArray *indices, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void scanStripPoints(TCULongArray *indices,
		TCULongArray *stripCounts, TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void flattenShapes(TREVertexArray *dstVertices,
		TREVertexArray *dstNormals,
		TREVertexArray *dstTextureCoords,
		TCULongArray *dstColors,
		TCULongArray *dstIndices,
		TCULongArray *dstCPIndices,
		TREVertexArray *srcVertices,
		TREVertexArray *srcNormals,
		TREVertexArray *srcTextureCoords,
		TCULongArray *srcColors,
		TCULongArray *srcIndices,
		TCULongArray *srcCPIndices,
		float *matrix,
		TCULong color,
		bool colorSet);
	virtual void flattenStrips(TREVertexArray *dstVertices,
		TREVertexArray *dstNormals,
		TREVertexArray *dstTextureCoords,
		TCULongArray *dstColors,
		TCULongArray *dstIndices,
		TCULongArray *dstStripCounts,
		TREVertexArray *srcVertices,
		TREVertexArray *srcNormals,
		TREVertexArray *srcTextureCoords,
		TCULongArray *srcColors,
		TCULongArray *srcIndices,
		TCULongArray *srcStripCounts,
		float *matrix,
		TCULong color,
		bool colorSet);
	virtual void mirrorTextureCoords(TCULongArray *indices);

	virtual void unshrinkNormal(TCULong index, float *matrix,
		float *unshrinkMatrix);
	virtual void unshrinkNormals(TCULongArray *indices, float *matrix,
		float *unshrinkMatrix);
	virtual void unshrinkStripNormals(TCULongArray *indices,
		TCULongArray *stripCounts, float *matrix,
		float *unshrinkMatrix);

	static void transformPoint(const TCVector &point, const float *matrix,
		float *tx, float *ty);
	static int TREShapeGroup::turnVector(float vx1, float vy1, float vx2,
		float vy2);

	TREVertexStore *m_vertexStore;
	TCULongArrayArray *m_indices;
	TCULongArray *m_controlPointIndices;
	TCULongArrayArray *m_stripCounts;
	TCULong ***m_multiDrawIndices;
	TCULong m_shapesPresent;
	TREMainModel *m_mainModel;

	static PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;
};

#endif // __TRESHAPEGROUP_H__
