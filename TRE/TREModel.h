#ifndef __TREMODEL_H__
#define __TREMODEL_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TRE/TREShapeGroup.h>
#include <LDLib/Vector.h>	// TODO: remove ldlib dependency

struct TREVertex;
class TRESubModel;
class TREMainModel;
class TREColoredShapeGroup;
class TREVertexArray;

typedef TCTypedObjectArray<TRESubModel> TRESubModelArray;

class TREModel : public TCObject
{
public:
	TREModel(void);
	TREModel(const TREModel &other);
	TCObject *copy(void);
	virtual void setMainModel(TREMainModel *mainModel)
	{
		m_mainModel = mainModel;
	}
	virtual TREMainModel *getMainModel(void) const { return m_mainModel; }
	virtual void setName(const char *name);
	virtual const char *getName(void) const { return m_name; }
	virtual TREShapeGroup *getShapes(void) { return m_shapes; }
//	virtual TREVertexArray *getVertices(void) { return m_vertices; }
	virtual TRESubModel *addSubModel(float *matrix, TREModel *model);
	virtual TRESubModel *addSubModel(TCULong color, float *matrix,
		TREModel *model);
	virtual void addTriangle(Vector *vertices);
	virtual void addTriangle(TCULong color, Vector *vertices);
	virtual void addQuad(Vector *vertices);
	virtual void addQuad(TCULong color, Vector *vertices);
	virtual void addQuadStrip(Vector *vertices, Vector *normals, int count);
	virtual void addQuadStrip(TCULong color, Vector *vertices, Vector *normals,
		int count);
	virtual void addTriangleFan(Vector *vertices, Vector *normals, int count);
	virtual void addTriangleFan(TCULong color, Vector *vertices,
		Vector *normals, int count);
	virtual void draw(void);
	virtual void compileDefaultColor(void);
	virtual void compileColored(void);
	virtual void drawDefaultColor(void);
	virtual void drawColored(void);
	virtual void setPart(bool part) { m_flags.part = part; }
	virtual bool isPart(void) { return m_flags.part; }
	virtual void flatten(void);
	virtual void addCylinder(const Vector &center, float radius, float height,
		int numSegments, int usedSegments = -1);
	virtual void addCone(const Vector &center, float radius, float height,
		int numSegments, int usedSegments = -1);
	virtual void addOpenCone(const Vector &center, float radius1, float radius2,
		float height, int numSegments, int usedSegments = -1);
	virtual void addDisk(const Vector &center, float radius, int numSegments,
		int usedSegments = -1);
	virtual void calculateBoundingBox(void);
	virtual void getMinMax(Vector& min, Vector& max);
	virtual void getMinMax(Vector& min, Vector& max, float* matrix);

	static void multMatrix(float* left, float* right, float* result);
	static void transformVertex(TREVertex &vertex, float *matrix);
	static void transformNormal(TREVertex &normal, float *matrix);
	static float determinant(float* matrix);
	static float invertMatrix(float* matrix, float* inverseMatrix);
protected:
	virtual ~TREModel(void);
	virtual void dealloc(void);
	virtual void setup(void);
	virtual void setupColored(void);
	virtual void flatten(TREModel *model, float *matrix, TCULong color,
		bool colorSet, bool includeShapes);
	virtual void flattenShapes(TREShapeGroup *dstShapes,
		TREShapeGroup *srcShapes, float *matrix, TCULong color, bool colorSet);
	virtual void flattenShapes(TREShapeType shapeType,
		TREVertexArray *dstVertices, TREVertexArray *dstNormals,
		TCULongArray *dstColors, TCULongArray *dstIndices,
		TREVertexArray *srcVertices, TREVertexArray *srcNormals,
		TCULongArray *srcColors, TCULongArray *srcIndices, float *matrix,
		TCULong color, bool colorSet);
	virtual void flattenStrips(TREShapeType shapeType,
		TREVertexArray *dstVertices, TREVertexArray *dstNormals,
		TCULongArray *dstColors, TCULongArray *dstIndices,
		TREVertexArray *srcVertices, TREVertexArray *srcNormals,
		TCULongArray *srcColors, TCULongArray *srcIndices, float *matrix,
		TCULong color, bool colorSet);
	void setCirclePoint(float angle, float radius, const Vector& center,
		Vector& point);

	static void setGlNormalize(bool value);

	char *m_name;
	TREMainModel *m_mainModel;
	TRESubModelArray *m_subModels;
	TREShapeGroup *m_shapes;
	TREColoredShapeGroup *m_coloredShapes;
	int m_defaultColorListID;
	int m_coloredListID;
	Vector m_boundingMin;
	Vector m_boundingMax;
	struct
	{
		bool part:1;
		bool boundingBox:1;
	} m_flags;

	static bool sm_normalizeOn;
};

#endif // __TREMODEL_H__
